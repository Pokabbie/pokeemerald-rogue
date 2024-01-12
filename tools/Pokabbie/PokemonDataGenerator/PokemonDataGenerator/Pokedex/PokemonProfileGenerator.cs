using Newtonsoft.Json;
using Newtonsoft.Json.Converters;
using Newtonsoft.Json.Linq;
using PokemonDataGenerator.Utils;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace PokemonDataGenerator.Pokedex
{
	public static class PokemonProfileGenerator
	{
		private static readonly JsonSerializerSettings c_JsonSettings = new JsonSerializerSettings
		{
			Formatting = Formatting.Indented,
			Converters = new List<JsonConverter>(new[] 
			{
				new StringEnumConverter()
			})
		};

		private class MovesetSettings
		{
			private static MovesetSettings s_VanillaSettings;
			private static MovesetSettings s_ExSettings;

			public List<string> levelUpPreference = new List<string>();
			public Dictionary<string, string[]> allowedTutorMoves = new Dictionary<string, string[]>();

			public static MovesetSettings VanillaSettings
			{
				get
				{
					if(s_VanillaSettings == null)
					{
						s_VanillaSettings = new MovesetSettings();

						// Priority order list of level up moves
						s_VanillaSettings.levelUpPreference.Add("rogue");
						s_VanillaSettings.levelUpPreference.Add("emerald");
						s_VanillaSettings.levelUpPreference.Add("firered-leafgreen");

						// Orderless versions we will base tutor moves off
						s_VanillaSettings.allowedTutorMoves.Add("rogue", new[] { "rogue", "emerald", "firered-leafgreen", "xd", "colosseum" });
						s_VanillaSettings.allowedTutorMoves.Add("emerald", new[] { "rogue", "emerald", "firered-leafgreen", "xd", "colosseum" });
						s_VanillaSettings.allowedTutorMoves.Add("firered-leafgreen", new[] { "rogue", "emerald", "firered-leafgreen", "xd", "colosseum" });
					}
					return s_VanillaSettings;
				}
			}

			public static MovesetSettings ExSettings
			{
				get
				{
					if (s_ExSettings == null)
					{
						s_ExSettings = new MovesetSettings();

						// Priority order list of level up moves
						s_ExSettings.levelUpPreference.Add("rogue");
						s_ExSettings.levelUpPreference.Add("scarlet-violet");
						s_ExSettings.levelUpPreference.Add("sword-shield");
						s_ExSettings.levelUpPreference.Add("ultra-sun-ultra-moon");

						// Orderless versions we will base tutor moves off
						s_ExSettings.allowedTutorMoves.Add("rogue", new[] { "rogue", "scarlet-violet", "sword-shield", "ultra-sun-ultra-moon" });
						s_ExSettings.allowedTutorMoves.Add("scarlet-violet", new[] { "rogue", "scarlet-violet" });
						s_ExSettings.allowedTutorMoves.Add("sword-shield", new[] { "rogue", "sword-shield" });
						s_ExSettings.allowedTutorMoves.Add("ultra-sun-ultra-moon", new[] { "rogue", "ultra-sun-ultra-moon" });

						// Just needed for cosplay pikachu
						s_ExSettings.levelUpPreference.Add("omega-ruby-alpha-sapphire");
						s_ExSettings.allowedTutorMoves.Add("omega-ruby-alpha-sapphire", new[] { "rogue", "omega-ruby-alpha-sapphire" });
					}
					return s_ExSettings;
				}
			}

			public void RemoveInvalidMoves(PokemonProfile source)
			{
				// Find out which game we're going to base the level up moveset off
				int levelUpIndex = int.MaxValue;

				foreach(var move in source.Moves)
				{
					int versionIndex = levelUpPreference.IndexOf(move.versionName);
					if(versionIndex != -1 && versionIndex < levelUpIndex)
					{
						levelUpIndex = versionIndex;

						// Found ideal preference
						if (levelUpIndex == 0)
							break;
					}
				}

				if (levelUpIndex >= levelUpPreference.Count)
					throw new NotImplementedException("Unable to find valid level up moveset version");

				string moveGroupName = levelUpPreference[levelUpIndex];

				source.Moves.RemoveAll((move) =>
				{
					if (move.originMethod == MoveInfo.LearnMethod.LevelUp)
					{
						return move.versionName != moveGroupName;
					}
					else
					{
						// Treat all other moves as tutor moves
						return !allowedTutorMoves[moveGroupName].Contains(move.versionName);
					}

				});
			}
		}


		private class MoveInfo
		{
			public enum LearnMethod
			{
				Invalid,
				Egg,
				TM,
				LevelUp,
				Tutor,
			}

			public LearnMethod originMethod;
			public int learnLevel;
			public string moveName;
			public string versionName;
		}

		private class PokemonProfile
		{
			public string Species;
			public List<MoveInfo> Moves;
			public List<PokemonCompetitiveSet> CompetitiveSets;
			public string[] Types = new string[2];
			public string[] Abilities = new string[3];

			public PokemonProfile(string name)
			{
				Species = name;
				Moves = new List<MoveInfo>();
				CompetitiveSets = new List<PokemonCompetitiveSet>();

				for (int i = 0; i < Types.Length; ++i)
					Types[i] = "none";

				for (int i = 0; i < Abilities.Length; ++i)
					Abilities[i] = "none";
			}

			public string PrettySpeciesName
			{ 
				get
				{
					string name = "";
					foreach(var word in Species.Split('_').Skip(1))
					{
						name += word.Substring(0, 1).ToUpper();
						name += word.Substring(1).ToLower();
					}

					return name;
				}
			}

			public void CollapseForExport()
			{
				//Moves.RemoveAll((m) => PokemonMoveHelpers.IsMoveUnsupported(m.moveName));

				var movesetPreferences = (GameDataHelpers.IsVanillaVersion ? MovesetSettings.VanillaSettings : MovesetSettings.ExSettings);
				movesetPreferences.RemoveInvalidMoves(this);

				// Simplify the definitions now and then we'll remove any duplicates
				List<MoveInfo> newMoves = new List<MoveInfo>();

				foreach(var oldMove in Moves)
				{
					MoveInfo newMove = oldMove;
					newMove.moveName = FormatMoveName(newMove.moveName, true);
					newMove.versionName = "rogue";

					if (IsBannedMove(newMove.moveName))
						continue;

					if (!GameDataHelpers.MoveDefines.ContainsKey(newMove.moveName))
						throw new InvalidDataException();

					if (newMove.originMethod == MoveInfo.LearnMethod.LevelUp)
					{
						var existingLevelUpMove = newMoves.Where(m => m.moveName == newMove.moveName && m.originMethod == MoveInfo.LearnMethod.LevelUp).FirstOrDefault();

						// If we already have the same level up move only take it at the lowest learn level
						if (existingLevelUpMove != null)
						{
							existingLevelUpMove.learnLevel = Math.Min(existingLevelUpMove.learnLevel, newMove.learnLevel);
							continue;
						}
					}
					else
					{
						newMove.originMethod = MoveInfo.LearnMethod.Tutor;

						var anyLevelUpMove = Moves.Where(m => m.moveName == newMove.moveName && m.originMethod == MoveInfo.LearnMethod.LevelUp).FirstOrDefault();
						if (anyLevelUpMove != null)
						{
							// If we learn this as a level up move, don't include an entry as a tutor move
							continue;
						}
					}

					if (!newMoves.Contains(newMove))
						newMoves.Add(newMove);
				}

				Moves = newMoves;

				FinaliseForExport();
			}

			public void CollapseFromImport()
			{
				FinaliseForExport();
			}

			private void FinaliseForExport()
			{
				// Now we've added the sets, add any moves that we can't currently learn as tutor moves
				foreach (var set in CompetitiveSets)
				{
					foreach (var move in set.Moves)
					{
						bool canLearnMove = Moves.Where(m => m.moveName == move).Any();
						if (!canLearnMove)
						{
							MoveInfo newMove = new MoveInfo();
							newMove.moveName = move;
							newMove.versionName = "rogue";
							newMove.originMethod = MoveInfo.LearnMethod.Tutor;
							Moves.Add(newMove);
						}
					}
				}

				// Now we have the complete moves so perform any post-processing
				{
					Moves.RemoveAll((move) =>
					{
						return IsBannedMove(move.moveName);
					});

					if(!GameDataHelpers.IsVanillaVersion)
					{
						// For now teach hidden power over tera blast
						var hiddenPower = Moves.Where((move) => move.moveName == "MOVE_HIDDEN_POWER").FirstOrDefault();
						var teraBlast = Moves.Where((move) => move.moveName == "MOVE_TERA_BLAST").FirstOrDefault();
						
						if(teraBlast != null)
						{
							if(hiddenPower == null)
								teraBlast.moveName = "MOVE_HIDDEN_POWER";
							else
								Moves.Remove(teraBlast);
						}

						// Replace all instances of hail with snowscape
						foreach(var hailMove in Moves.Where((move) => move.moveName == "MOVE_HAIL"))
						{
							hailMove.moveName = "MOVE_SNOWSCAPE";
						}

						if (Species == "SPECIES_RAYQUAZA")
						{
							// Make gragon ascent a late level learn
							var dragonAscent = Moves.Where((move) => move.moveName == "MOVE_DRAGON_ASCENT").First();
							dragonAscent.learnLevel = 90;
						}
					}

					// Verify the move is recognised in game here
					foreach (var move in Moves)
					{
						if (!GameDataHelpers.MoveDefines.ContainsKey(move.moveName))
							throw new InvalidDataException();
					}
				}

				// Now sort them before we export
				Moves = Moves.OrderBy((m) => m.originMethod == MoveInfo.LearnMethod.LevelUp ? m.learnLevel.ToString("000") : "999" + m.moveName).ToList();
			}

			public HashSet<string> GetMoveSourceVerions()
			{
				HashSet<string> versions = new HashSet<string>();
				foreach(var move in Moves)
				{
					versions.Add(move.versionName);
				}

				return versions;
			}

			private static IEnumerable<MoveInfo> EnsureMovesUnique(IEnumerable<MoveInfo> moves)
			{
				HashSet<string> visitedMoves = new HashSet<string>();

				foreach(var move in moves)
				{
					if(!visitedMoves.Contains(move.moveName))
					{
						visitedMoves.Add(move.moveName);
						yield return move;
					}
				}
			}

			public IEnumerable<MoveInfo> GetLevelUpMoves()
			{
				return EnsureMovesUnique(Moves.Where((m) => m.originMethod == MoveInfo.LearnMethod.LevelUp).OrderBy((m) => m.learnLevel));
			}

			public IEnumerable<MoveInfo> GetEggMoves()
			{
				return EnsureMovesUnique(Moves.Where((m) => m.originMethod == MoveInfo.LearnMethod.Egg));
			}

			public IEnumerable<MoveInfo> GetTMHMMoves()
			{
				return EnsureMovesUnique(Moves.Where((m) => GameDataHelpers.MoveToTMHMItem.ContainsKey(GameDataHelpers.FormatKeyword(m.moveName))));
			}

			public IEnumerable<MoveInfo> GetTutorMoves()
			{
				return EnsureMovesUnique(Moves.Where((m) => GameDataHelpers.TutorMoveDefines.ContainsKey(GameDataHelpers.FormatKeyword(m.moveName))));
			}

			public IEnumerable<MoveInfo> GetLeftoverMoves()
			{
				return EnsureMovesUnique(Moves.Where((m) =>
					m.originMethod != MoveInfo.LearnMethod.LevelUp &&
					m.originMethod != MoveInfo.LearnMethod.Egg &&
					GameDataHelpers.MoveToTMHMItem.ContainsKey(GameDataHelpers.FormatKeyword(m.moveName)) &&
					GameDataHelpers.TutorMoveDefines.ContainsKey(GameDataHelpers.FormatKeyword(m.moveName)))
				);
			}
		}

		private class PokemonCompetitiveSet
		{
			public List<string> Moves = new List<string>();
			public string Ability;
			public string Item;
			public string Nature;
			public string HiddenPower;
			public List<string> SourceTiers = new List<string>();

			public bool IsCompatibleWith(PokemonCompetitiveSet other)
			{
				if (Ability != other.Ability)
					return false;
				if (Item != other.Item)
					return false;
				if (Nature != other.Nature)
					return false;
				if (HiddenPower != other.HiddenPower)
					return false;

				if (Moves.Count != other.Moves.Count)
					return false;

				for(int i = 0; i < Moves.Count; ++i)
				{
					if (Moves[i] != other.Moves[i])
						return false;
				}

				return true;
			}

			public static PokemonCompetitiveSet ParseFrom(string sourceTier, JObject json)
			{
				PokemonCompetitiveSet output = new PokemonCompetitiveSet();
				output.SourceTiers.Add(sourceTier);

				string ability = json["ability"].Value<string>();
				if (ability != "No Ability")
				{
					output.Ability = "ABILITY_" + GameDataHelpers.FormatKeyword(ability);

					switch(output.Ability)
					{
						case "ABILITY_AS_ONE_(GLASTRIER)":
							output.Ability = "ABILITY_AS_ONE_ICE_RIDER";
							break;
						case "ABILITY_AS_ONE_(SPECTRIER)":
							output.Ability = "ABILITY_AS_ONE_SHADOW_RIDER";
							break;
					}

					if (!GameDataHelpers.AbilityDefines.ContainsKey(output.Ability))
						throw new InvalidDataException();
				}

				if (json.ContainsKey("item"))
				{
					output.Item = FormatItemName(json["item"].Value<string>());

					if (!GameDataHelpers.ItemDefines.ContainsKey(output.Item))
						throw new InvalidDataException();
				}

				if (json.ContainsKey("nature"))
				{
					output.Nature = "NATURE_" + GameDataHelpers.FormatKeyword(json["nature"].Value<string>());

					if (!GameDataHelpers.NatureDefines.ContainsKey(output.Nature))
						throw new InvalidDataException();
				}

				foreach (var move in json["moves"])
				{
					string moveName = move.Value<string>();

					if(moveName.StartsWith("Hidden Power", StringComparison.CurrentCultureIgnoreCase))
					{
						output.HiddenPower = moveName.Substring("Hidden Power".Length).Trim();
						output.HiddenPower = "TYPE_" + GameDataHelpers.FormatKeyword(output.HiddenPower);
						moveName = "Hidden Power";

						if (!GameDataHelpers.TypesDefines.ContainsKey(output.HiddenPower))
							throw new InvalidDataException();
					}
					// As we are replacing tera blast with hidden power for now, pull out the hidden power here
					else if (moveName.StartsWith("Tera Blast", StringComparison.CurrentCultureIgnoreCase))
					{
						moveName = "Hidden Power";

						if (json.ContainsKey("teraType"))
						{
							output.HiddenPower = json["teraType"].Value<string>().Trim();
							output.HiddenPower = "TYPE_" + GameDataHelpers.FormatKeyword(output.HiddenPower);

							if(output.HiddenPower == "TYPE_STELLAR")
							{
								// Not provided by data so expect runtime to just chose the primary type
								output.HiddenPower = "TYPE_MYSTERY";
							}
						}
						else
						{
							// Not provided by data so expect runtime to just chose the primary type
							output.HiddenPower = "TYPE_MYSTERY";
						}

						if (!GameDataHelpers.TypesDefines.ContainsKey(output.HiddenPower))
							throw new InvalidDataException();
					}

					moveName = FormatMoveName(moveName, false);

					output.Moves.Add(moveName);
				}

				return output;
			}
		}

		private static string FormatMoveName(string moveName, bool fromLearnsets)
		{
			// Fix some spellings
			if (GameDataHelpers.IsVanillaVersion)
			{
				switch (moveName.ToLower().Replace("-", " "))
				{
					case "feint attack":
						moveName = "faint attack";
						break;

					case "high jump kick":
						moveName = "hi jump kick";
						break;

					case "smelling salts":
						moveName = "smelling salt";
						break;
				}
			}
			else
			{
				switch (moveName.ToLower().Replace("-", " "))
				{
					case "vice grip":
						moveName = "vise grip";
						break;
				}
			}

			string outputName = "MOVE_" + GameDataHelpers.FormatKeyword(moveName);

			if (!GameDataHelpers.MoveDefines.ContainsKey(outputName))
			{
				// Attempt to remove spaces and find a matching name, as that's pretty minor and it's easier to do this
				string testName = outputName.Replace("_", "");

				foreach(var kvp in GameDataHelpers.MoveDefines)
				{
					if (testName == kvp.Key.Replace("_", ""))
						return kvp.Key;
				}
			}

			return outputName;
		}

		private static string FormatItemName(string itemName)
		{
			if (!GameDataHelpers.IsVanillaVersion)
			{
				switch(itemName.ToLower().Replace("-", " "))
				{
					case "stick":
						itemName = "leek";
						break;
				}
			}

			string outputName = "ITEM_" + GameDataHelpers.FormatKeyword(itemName);

			if (!GameDataHelpers.ItemDefines.ContainsKey(outputName))
			{
				// Attempt to remove spaces and find a matching name, as that's pretty minor and it's easier to do this
				string testName = outputName.Replace("_", "");

				foreach (var kvp in GameDataHelpers.ItemDefines)
				{
					if (testName == kvp.Key.Replace("_", ""))
						return kvp.Key;
				}
			}

			return outputName;
		}

		private static bool IsBannedMove(string moveName)
		{
			switch(moveName)
			{
				// Intentionally banned moves
				case "MOVE_REVIVAL_BLESSING":
					return true;

				// Moves that aren't currently implemented fully
				//case "MOVE_TERA_BLAST": <- is replaced by hidden power
				case "MOVE_LAST_RESPECTS":
				case "MOVE_ORDER_UP":
				case "MOVE_SPICY_EXTRACT":
				case "MOVE_DOODLE":
				case "MOVE_FILLET_AWAY":
				case "MOVE_SHED_TAIL":
				case "MOVE_TIDY_UP":
				case "MOVE_ELECTRO_SHOT":
				case "MOVE_TERA_STARSTORM":
				case "MOVE_FICKLE_BEAM":
				case "MOVE_BURNING_BULWARK":
				case "MOVE_DRAGON_CHEER":
				case "MOVE_ALLURING_VOICE":
				case "MOVE_PSYCHIC_NOISE":
				case "MOVE_UPPER_HAND":
					return true;
			}

			return false;
		}

		public static void GatherProfiles()
		{
			List<PokemonProfile> profiles = new List<PokemonProfile>();
			Dictionary<string, string> redirectedSpecies = new Dictionary<string, string>();
			int exceptionHitCount = 0;

			Stack<string> speciesToProcess = new Stack<string>();
			foreach (var kvp in GameDataHelpers.SpeciesDefines.Reverse())
			{
				speciesToProcess.Push(kvp.Key);
			}

			while(speciesToProcess.Count != 0)
			{
				string speciesName = speciesToProcess.Pop();

				try
				{
					if (!GameDataHelpers.IsUniqueSpeciesDefine(speciesName))
						continue;

					if (GameDataHelpers.IsVanillaVersion && speciesName.StartsWith("SPECIES_UNOWN_"))
					{
						// Skip these as they appear after NUM_SPECIES
						continue;
					}

					// Skipped species (Handle elsewhere)
					string redirectSpecies = null;

					if (speciesName.StartsWith("SPECIES_UNOWN_"))
					{
						redirectSpecies = "SPECIES_UNOWN";
					}
					else if (speciesName.StartsWith("SPECIES_CASTFORM_"))
					{
						redirectSpecies = "SPECIES_CASTFORM";
					}

					if (redirectSpecies == null)
					{
						if (!GameDataHelpers.IsVanillaVersion)
						{
							// Only redirect species which are functionally identical for rogue spawning
							if (speciesName.EndsWith("_MEGA"))
							{
								redirectSpecies = speciesName.Substring(0, speciesName.Length - "_MEGA".Length);
							}
							else if (speciesName.EndsWith("_MEGA_X") || speciesName.EndsWith("_MEGA_Y"))
							{
								redirectSpecies = speciesName.Substring(0, speciesName.Length - "_MEGA_X".Length);
							}
							else if (speciesName.EndsWith("_PRIMAL"))
							{
								redirectSpecies = speciesName.Substring(0, speciesName.Length - "_PRIMAL".Length);
							}
							else if (speciesName.EndsWith("_GIGANTAMAX"))
							{
								switch (speciesName)
								{
									case "SPECIES_TOXTRICITY_AMPED_GIGANTAMAX":
										redirectSpecies = "SPECIES_TOXTRICITY";
										break;

									case "SPECIES_URSHIFU_SINGLE_STRIKE_STYLE_GIGANTAMAX":
										redirectSpecies = "SPECIES_URSHIFU";
										break;

									default:
										redirectSpecies = speciesName.Substring(0, speciesName.Length - "_GIGANTAMAX".Length);
										break;
								}
							}
							else if (speciesName.StartsWith("SPECIES_BURMY_"))
							{
								redirectSpecies = "SPECIES_BURMY";
							}
							else if (speciesName.StartsWith("SPECIES_ARCEUS_"))
							{
								redirectSpecies = "SPECIES_ARCEUS";
							}
							else if (speciesName.StartsWith("SPECIES_DEERLING_"))
							{
								redirectSpecies = "SPECIES_DEERLING";
							}
							else if (speciesName.StartsWith("SPECIES_SAWSBUCK_"))
							{
								redirectSpecies = "SPECIES_SAWSBUCK";
							}
							else if (speciesName.StartsWith("SPECIES_GENESECT_"))
							{
								redirectSpecies = "SPECIES_GENESECT";
							}
							else if (speciesName.StartsWith("SPECIES_VIVILLON_"))
							{
								redirectSpecies = "SPECIES_VIVILLON";
							}
							else if (speciesName.StartsWith("SPECIES_FLABEBE_"))
							{
								redirectSpecies = "SPECIES_FLABEBE";
							}
							else if (speciesName.StartsWith("SPECIES_FLOETTE_"))
							{
								redirectSpecies = "SPECIES_FLOETTE";
							}
							else if (speciesName.StartsWith("SPECIES_FLORGES_"))
							{
								redirectSpecies = "SPECIES_FLORGES";
							}
							else if (speciesName.StartsWith("SPECIES_FURFROU_"))
							{
								redirectSpecies = "SPECIES_FURFROU";
							}
							else if (speciesName.StartsWith("SPECIES_PUMPKABOO_"))
							{
								redirectSpecies = "SPECIES_PUMPKABOO";
							}
							else if (speciesName.StartsWith("SPECIES_GOURGEIST_"))
							{
								redirectSpecies = "SPECIES_GOURGEIST";
							}
							else if (speciesName.StartsWith("SPECIES_SILVALLY_"))
							{
								redirectSpecies = "SPECIES_SILVALLY";
							}
							else if (speciesName.StartsWith("SPECIES_MINIOR_"))
							{
								redirectSpecies = "SPECIES_MINIOR";
							}
							else if (speciesName.StartsWith("SPECIES_CRAMORANT_"))
							{
								redirectSpecies = "SPECIES_CRAMORANT";
							}
							else if (speciesName.StartsWith("SPECIES_ALCREMIE_"))
							{
								redirectSpecies = "SPECIES_ALCREMIE";
							}
							else if (speciesName.StartsWith("SPECIES_SQUAWKABILLY_"))
							{
								redirectSpecies = "SPECIES_SQUAWKABILLY";
							}
							else
							{
								switch (speciesName)
								{
									case "SPECIES_CHERRIM_SUNSHINE":
										redirectSpecies = "SPECIES_CHERRIM";
										break;

									case "SPECIES_SHELLOS_EAST_SEA":
										redirectSpecies = "SPECIES_SHELLOS";
										break;

									case "SPECIES_GASTRODON_EAST_SEA":
										redirectSpecies = "SPECIES_GASTRODON";
										break;

									case "SPECIES_DARMANITAN_ZEN_MODE":
										redirectSpecies = "SPECIES_DARMANITAN";
										break;

									case "SPECIES_DARMANITAN_ZEN_MODE_GALARIAN":
										redirectSpecies = "SPECIES_DARMANITAN_GALARIAN";
										break;

									case "SPECIES_KELDEO_RESOLUTE":
										redirectSpecies = "SPECIES_KELDEO";
										break;

									case "SPECIES_MELOETTA_PIROUETTE":
										redirectSpecies = "SPECIES_MELOETTA";
										break;

									case "SPECIES_AEGISLASH_BLADE":
										redirectSpecies = "SPECIES_AEGISLASH";
										break;

									case "SPECIES_XERNEAS_ACTIVE":
										redirectSpecies = "SPECIES_XERNEAS";
										break;

									case "SPECIES_ZYGARDE_50_POWER_CONSTRUCT":
										redirectSpecies = "SPECIES_ZYGARDE";
										break;

									case "SPECIES_WISHIWASHI_SCHOOL":
										redirectSpecies = "SPECIES_WISHIWASHI";
										break;

									case "SPECIES_MIMIKYU_BUSTED":
										redirectSpecies = "SPECIES_MIMIKYU";
										break;

									case "SPECIES_MAGEARNA_ORIGINAL_COLOR":
										redirectSpecies = "SPECIES_MAGEARNA";
										break;

									case "SPECIES_SINISTEA_ANTIQUE":
										redirectSpecies = "SPECIES_SINISTEA";
										break;
									case "SPECIES_POLTEAGEIST_ANTIQUE":
										redirectSpecies = "SPECIES_POLTEAGEIST";
										break;

									case "SPECIES_EISCUE_NOICE_FACE":
										redirectSpecies = "SPECIES_EISCUE";
										break;

									case "SPECIES_MORPEKO_HANGRY":
										redirectSpecies = "SPECIES_MORPEKO";
										break;

									case "SPECIES_ETERNATUS_ETERNAMAX":
										redirectSpecies = "SPECIES_ETERNATUS";
										break;

									case "SPECIES_ZARUDE_DADA":
										redirectSpecies = "SPECIES_ZARUDE";
										break;

									case "SPECIES_OGERPON_TEAL_MASK_TERA":
										redirectSpecies = "SPECIES_OGERPON";
										break;

									case "SPECIES_OGERPON_WELLSPRING_MASK_TERA":
										redirectSpecies = "SPECIES_OGERPON_WELLSPRING_MASK";
										break;

									case "SPECIES_OGERPON_HEARTHFLAME_MASK_TERA":
										redirectSpecies = "SPECIES_OGERPON_HEARTHFLAME_MASK";
										break;

									case "SPECIES_OGERPON_CORNERSTONE_MASK_TERA":
										redirectSpecies = "SPECIES_OGERPON_CORNERSTONE_MASK";
										break;

									case "SPECIES_WOBBUFFET_ROGUEIAN_PUNCHING":
										redirectSpecies = "SPECIES_WOBBUFFET_ROGUEIAN";
										break;
								}

							}
						}
					}

					if (redirectSpecies != null)
					{
						if (!GameDataHelpers.SpeciesDefines.ContainsKey(redirectSpecies))
							throw new InvalidDataException();

						redirectedSpecies[speciesName] = redirectSpecies;
						continue;
					}

					PokemonProfile profile = GatherProfileFor(speciesName);
					profiles.Add(profile);
				}
				catch (AggregateException e)
				{
					if (e.InnerException is HttpRequestException)
						Console.WriteLine($"\tCaught Http Exception '{e.InnerException.Message}'");

					if (++exceptionHitCount < 5)
					{
						speciesToProcess.Push(speciesName);

						Console.WriteLine($"\tSleeping (assuming just rapid access error)");
						Thread.Sleep(5000);
					}
					{
						// Gone over threshold
						throw e;
					}
				}
			}

			ExportProfiles(profiles, redirectedSpecies, Path.Combine(GameDataHelpers.RootDirectory, "src\\data\\rogue_pokemon_profiles.h"));
		}

		private static PokemonProfile GatherProfileFor(string speciesName)
		{
			string manualPath = ContentCache.GetWriteableCachePath($"res://PokemonProfiles//{(GameDataHelpers.IsVanillaVersion ? "Vanilla" : "EX")}/{speciesName}.json");
			string cachePath = ContentCache.GetWriteableCachePath($"pokemon_profiles/{(GameDataHelpers.IsVanillaVersion ? "Vanilla" : "EX")}/{speciesName}.json");
			PokemonProfile profile;

			if (File.Exists(manualPath))
			{
				Console.WriteLine($"Found '{speciesName}' profile manual override");

				string jsonProfile = File.ReadAllText(manualPath);
				profile = JsonConvert.DeserializeObject<PokemonProfile>(jsonProfile, c_JsonSettings);
				profile.CollapseFromImport();
			}
			else if (File.Exists(cachePath))
			{
				Console.WriteLine($"Found '{speciesName}' profile in cache");

				string jsonProfile = File.ReadAllText(cachePath);
				profile = JsonConvert.DeserializeObject<PokemonProfile>(jsonProfile, c_JsonSettings);
				profile.CollapseFromImport();
			}
			else
			{
				Console.WriteLine($"Gathering '{speciesName}' profile from source");

				profile = new PokemonProfile(speciesName);

				JObject monEntry = PokeAPI.GetPokemonSpeciesEntry(speciesName);

				foreach (var obj in monEntry["abilities"])
				{
					string abilityName = obj["ability"]["name"].ToString();
					string rawSlot = obj["slot"].ToString();

					profile.Abilities[int.Parse(rawSlot) - 1] = abilityName;
				}

				foreach (var obj in monEntry["types"])
				{
					string name = obj["type"]["name"].ToString();
					string rawSlot = obj["slot"].ToString();

					profile.Types[int.Parse(rawSlot) - 1] = name;
				}

				foreach (var moveObj in monEntry["moves"])
				{
					foreach (var versionObj in moveObj["version_group_details"])
					{
						MoveInfo moveInfo = new MoveInfo();
						moveInfo.moveName = moveObj["move"]["name"].ToString();
						moveInfo.versionName = versionObj["version_group"]["name"].ToString();

						string method = versionObj["move_learn_method"]["name"].ToString();
						switch (method)
						{
							case "egg":
								moveInfo.originMethod = MoveInfo.LearnMethod.Egg;
								break;
							case "machine":
								moveInfo.originMethod = MoveInfo.LearnMethod.TM;
								break;
							case "tutor":
								moveInfo.originMethod = MoveInfo.LearnMethod.Tutor;
								break;
							case "level-up":
								moveInfo.originMethod = MoveInfo.LearnMethod.LevelUp;
								moveInfo.learnLevel = int.Parse(versionObj["level_learned_at"].ToString());
								break;

							// Special cases
							//case "stadium-surfing-pikachu":
							//case "light-ball-egg":
							default:
								moveInfo.originMethod = MoveInfo.LearnMethod.Tutor;
								break;

								//default:
								//	throw new NotImplementedException();
						}

						profile.Moves.Add(moveInfo);
					}
				}

				JObject competitiveSets = PokeAPI.GetPokemonSpeciesCompetitiveSets(speciesName);

				foreach (var tierKvp in competitiveSets)
				{
					foreach (var currentSet in tierKvp.Value.Value<JArray>())
					{
						string tierName = GameDataHelpers.FormatKeyword(tierKvp.Key);
						PokemonCompetitiveSet compSet = PokemonCompetitiveSet.ParseFrom(tierName, currentSet.Value<JObject>());

						bool hasMerged = false;

						foreach (var existingSet in profile.CompetitiveSets)
						{
							// No need to contain duplicate sets
							if (existingSet.IsCompatibleWith(compSet))
							{
								existingSet.SourceTiers.Add(tierName);
								hasMerged = true;
								break;
							}
						}

						if (!hasMerged)
							profile.CompetitiveSets.Add(compSet);
					}
				}

				profile.CollapseForExport();

				string cacheDir = Path.GetDirectoryName(cachePath);
				Directory.CreateDirectory(cacheDir);

				string profileJson = JsonConvert.SerializeObject(profile, c_JsonSettings);
				File.WriteAllText(cachePath, profileJson);
			}

			return profile;
		}

		private static void ExportProfiles(List<PokemonProfile> profiles, Dictionary<string, string> redirectedSpecies, string filePath)
		{
			Console.WriteLine($"Exporting profiles to '{filePath}'");

			StringBuilder upperBlock = new StringBuilder();
			StringBuilder lowerBlock = new StringBuilder();

			upperBlock.AppendLine("// == WARNING ==");
			upperBlock.AppendLine("// DO NOT EDIT THIS FILE");
			upperBlock.AppendLine("// This file was automatically generated by PokemonDataGenerator");
			upperBlock.AppendLine("//");
			upperBlock.AppendLine();

			// Move/Item usages
			//
			Dictionary<string, int> moveCount = new Dictionary<string, int>();
			Dictionary<string, int> heldItemCount = new Dictionary<string, int>();

			foreach (var profile in profiles)
			{
				HashSet<string> uniqueItems = new HashSet<string>();
				HashSet<string> uniqueMoves = new HashSet<string>();

				foreach (var compSet in profile.CompetitiveSets)
				{
					foreach (var move in compSet.Moves)
						uniqueMoves.Add(move);

					if (compSet.Item != null)
						uniqueItems.Add(compSet.Item);
				}

				foreach (var move in uniqueMoves)
				{
					if (moveCount.ContainsKey(move))
						++moveCount[move];
					else
						moveCount[move] = 1;
				}

				foreach (var item in uniqueItems)
				{
					if (heldItemCount.ContainsKey(item))
						++heldItemCount[item];
					else
						heldItemCount[item] = 1;
				}
			}

			upperBlock.AppendLine("u16 const gRoguePokemonMoveUsages[MOVES_COUNT] = \n{");

			foreach (var kvp in moveCount)
				upperBlock.AppendLine($"\t[{kvp.Key}] = {kvp.Value},");

			upperBlock.AppendLine("};\n");

			upperBlock.AppendLine("u16 const gRoguePokemonHeldItemUsages[ITEMS_COUNT] = \n{");

			foreach (var kvp in heldItemCount)
				upperBlock.AppendLine($"\t[{kvp.Key}] = {kvp.Value},");

			upperBlock.AppendLine("};\n");

			// Pokemon Profiles
			//
			lowerBlock.AppendLine("struct RoguePokemonProfile const gRoguePokemonProfiles[NUM_SPECIES] = \n{");

			foreach(var profile in profiles)
			{
				// Mon flags
				HashSet<string> sourceTiers = new HashSet<string>();

				foreach (var compSet in profile.CompetitiveSets)
				{
					foreach (var tier in compSet.SourceTiers)
						sourceTiers.Add(GameDataHelpers.FormatKeyword(tier));
				}

				upperBlock.AppendLine($"#ifdef APPEND_MON_FLAGS_{profile.Species}");

				upperBlock.Append($"#define MON_FLAGS_{profile.Species} (APPEND_MON_FLAGS_{profile.Species}"); // allow easily appending flags
				foreach (var tier in sourceTiers)
					upperBlock.Append($" | MON_FLAGS_{tier}");
				upperBlock.AppendLine(")");

				upperBlock.AppendLine("#else");

				upperBlock.Append($"#define MON_FLAGS_{profile.Species} (0");
				foreach (var tier in sourceTiers)
					upperBlock.Append($" | MON_FLAGS_{tier}");
				upperBlock.AppendLine(")");

				upperBlock.AppendLine("#endif");
				upperBlock.AppendLine("");

				// Level moves
				upperBlock.AppendLine($"static struct LevelUpMove const sLevelUpMoves_{profile.Species}[] = \n{{");
				foreach(var move in profile.Moves.Where(m => m.originMethod == MoveInfo.LearnMethod.LevelUp))
				{
					upperBlock.AppendLine($"\t{{ .move={move.moveName}, .level={move.learnLevel} }},");
				}
				upperBlock.AppendLine($"\t{{ .move=MOVE_NONE, .level=0 }},");
				upperBlock.AppendLine($"}};");
				upperBlock.AppendLine();

				// Tutor moves
				upperBlock.AppendLine($"static u16 const sTutorMoves_{profile.Species}[] = \n{{");
				foreach (var move in profile.Moves.Where(m => m.originMethod != MoveInfo.LearnMethod.LevelUp))
				{
					upperBlock.AppendLine($"\t{move.moveName},");
				}
				upperBlock.AppendLine($"\tMOVE_NONE,");
				upperBlock.AppendLine($"}};");
				upperBlock.AppendLine();

				// Comp sets
				upperBlock.AppendLine($"static struct RoguePokemonCompetitiveSet const sCompetitiveSets_{profile.Species}[] = \n{{");
				foreach(var compSet in profile.CompetitiveSets)
				{
					upperBlock.AppendLine($"\t{{");

					upperBlock.Append($"\t\t.flags= (0");
					foreach (var tier in compSet.SourceTiers)
						upperBlock.Append($" | MON_FLAGS_{tier}");
					upperBlock.AppendLine("),");

					if (compSet.Item != null)
						upperBlock.AppendLine($"\t\t.heldItem={compSet.Item},");

					if (compSet.Ability != null)
						upperBlock.AppendLine($"\t\t.ability={compSet.Ability},");

					if (compSet.HiddenPower != null)
						upperBlock.AppendLine($"\t\t.hiddenPowerType={compSet.HiddenPower},");

					if (compSet.Nature != null)
						upperBlock.AppendLine($"\t\t.nature={compSet.Nature},");

					upperBlock.AppendLine($"\t\t.moves=\n\t\t{{");
					foreach(var move in compSet.Moves)
					{
						upperBlock.AppendLine($"\t\t\t{move},");
					}
					upperBlock.AppendLine($"\t\t}},");

					upperBlock.AppendLine($"\t}},");
				}
				upperBlock.AppendLine($"}};");
				upperBlock.AppendLine();


				// Add to species lookup below
				lowerBlock.AppendLine($"\t[{profile.Species}] = \n\t{{");
				lowerBlock.AppendLine($"\t\t.levelUpMoves = sLevelUpMoves_{profile.Species},");
				lowerBlock.AppendLine($"\t\t.tutorMoves = sTutorMoves_{profile.Species},");
				lowerBlock.AppendLine($"\t\t.competitiveSets = sCompetitiveSets_{profile.Species},");
				lowerBlock.AppendLine($"\t\t.competitiveSetCount = ARRAY_COUNT(sCompetitiveSets_{profile.Species}),");
				lowerBlock.AppendLine($"\t\t.monFlags = MON_FLAGS_{profile.Species},");
				lowerBlock.AppendLine($"\t}},");
			}

			// Attach redirected species info too
			foreach(var kvp in redirectedSpecies)
			{
				lowerBlock.AppendLine($"\t[{kvp.Key}] = \n\t{{");
				lowerBlock.AppendLine($"\t\t.levelUpMoves = sLevelUpMoves_{kvp.Value},");
				lowerBlock.AppendLine($"\t\t.tutorMoves = sTutorMoves_{kvp.Value},");
				lowerBlock.AppendLine($"\t\t.competitiveSets = sCompetitiveSets_{kvp.Value},");
				lowerBlock.AppendLine($"\t\t.competitiveSetCount = ARRAY_COUNT(sCompetitiveSets_{kvp.Value}),");
				lowerBlock.AppendLine($"\t\t.monFlags = MON_FLAGS_{kvp.Value},");
				lowerBlock.AppendLine($"\t}},");
			}

			lowerBlock.AppendLine("};");

			File.WriteAllText(filePath, upperBlock.ToString() + "\n"+ lowerBlock.ToString());
		}
	}
}
