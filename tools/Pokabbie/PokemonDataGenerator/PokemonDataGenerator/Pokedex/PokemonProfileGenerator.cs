using Newtonsoft.Json.Linq;
using PokemonDataGenerator.Utils;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator.Pokedex
{
	public static class PokemonProfileGenerator
	{
		private static readonly string c_API = "https://pokeapi.co/api/v2/";


		private struct MoveInfo
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
			public string[] Types = new string[2];
			public string[] Abilities = new string[3];

			public PokemonProfile(string name)
			{
				Species = name;
				Moves = new List<MoveInfo>();

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

			public void CollapseLearntMoves()
			{
				Moves.RemoveAll((m) => PokemonMoveHelpers.IsMoveUnsupported(m.moveName));

				HashSet<string> versions = GetMoveSourceVerions();

				if (versions.Count != 1)
					throw new NotImplementedException(); // TODO
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
				return EnsureMovesUnique(Moves.Where((m) => GameDataHelpers.MoveToTMHMItem.ContainsKey(FormatKeyword(m.moveName))));
			}

			public IEnumerable<MoveInfo> GetTutorMoves()
			{
				return EnsureMovesUnique(Moves.Where((m) => GameDataHelpers.TutorMoveDefines.ContainsKey(FormatKeyword(m.moveName))));
			}

			public IEnumerable<MoveInfo> GetLeftoverMoves()
			{
				return EnsureMovesUnique(Moves.Where((m) =>
					m.originMethod != MoveInfo.LearnMethod.LevelUp &&
					m.originMethod != MoveInfo.LearnMethod.Egg &&
					GameDataHelpers.MoveToTMHMItem.ContainsKey(FormatKeyword(m.moveName)) &&
					GameDataHelpers.TutorMoveDefines.ContainsKey(FormatKeyword(m.moveName)))
				);
			}
		}

		public static void GatherProfiles()
		{
			List<PokemonProfile> profiles = new List<PokemonProfile>();

			foreach(var kvp in GameDataHelpers.SpeciesDefines)
			{
				try
				{
					string speciesName = kvp.Key;
					string apiName = kvp.Key.Substring("SPECIES_".Length).ToLower().Replace("_", "-");
					bool isValid = false;

					if (apiName == "none" || apiName == "count" || apiName.StartsWith("old-"))
						continue;

					if (false)
					{
						// Test just limit to hisui mons
						if (apiName.EndsWith("-hisuian"))
						{
							isValid = true;
							apiName = apiName.Replace("-hisuian", "-hisui");
						}
						else
						{
							switch (apiName)
							{
								case "wyrdeer":
								case "kleavor":
								case "ursaluna":
								case "sneasler":
								case "overqwil":
								case "enamorus-therian":
								case "basculegion-female":
								case "basculegion-white-striped":
									isValid = true;
									break;

								case "enamorus":
									isValid = true;
									apiName += "-incarnate";
									break;

								case "basculegion":
									isValid = true;
									apiName += "-male";
									break;
							}
						}
					}

					if(isValid)
					{
						PokemonProfile profile = GatherProfileFor(speciesName, apiName);
						profile.CollapseLearntMoves();
						profiles.Add(profile);
					}
				}
				catch(AggregateException e)
				{
					if (e.InnerException is HttpRequestException)
						Console.WriteLine($"\tCaught Http Exception '{e.InnerException.Message}'");
					else
						throw e;
				}
			}

			//profiless
			string outputDir = Path.GetFullPath("pokemon_profiles");
			Directory.CreateDirectory(outputDir);

			ExportLevelUpLearnsets(Path.Combine(outputDir, "level_up_learnsets.h"), profiles);
			ExportLevelUpLearnsetsPointers(Path.Combine(outputDir, "level_up_learnsets_pointers.h"), profiles);
			ExportTMHMsLearnsets(Path.Combine(outputDir, "tmhm_learnsets.h"), profiles);
			ExportTutorLearnsets(Path.Combine(outputDir, "tutor_learnsets.h"), profiles);
			ExportEggMoves(Path.Combine(outputDir, "egg_moves.h"), profiles);
		}

		private static PokemonProfile GatherProfileFor(string speciesName, string apiName)
		{
			Console.WriteLine($"Gathering '{speciesName}' profile ({apiName})");


			PokemonProfile profile = new PokemonProfile(speciesName);

			JObject monEntry = ContentCache.GetJsonContent(c_API + "pokemon/" + apiName);

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

			return profile;
		}

		private static void ExportLevelUpLearnsets(string filePath, IEnumerable<PokemonProfile> profiles)
		{
			StringBuilder content = new StringBuilder();

			content.AppendLine("// == WARNING ==");
			content.AppendLine("// DO NOT EDIT THIS FILE");
			content.AppendLine("// This file was automatically generated by PokemonDataGenerator");
			content.AppendLine("//");

			foreach(var profile in profiles)
			{
				var moves = profile.GetLevelUpMoves();

				if(moves.Any())
				{
					content.AppendLine($"");
					content.AppendLine($"static const struct LevelUpMove s{profile.PrettySpeciesName}LevelUpLearnset[] = {{");

					foreach (var move in moves)
					{
						content.AppendLine($"\tLEVEL_UP_MOVE({move.learnLevel}, MOVE_{FormatKeyword(move.moveName)}),");
					}
					content.AppendLine($"\tLEVEL_UP_END");

					content.AppendLine($"}};");
				}
			}

			string fullStr = content.ToString().Replace("\t", "    ");
			File.WriteAllText(filePath, fullStr);
		}

		private static void ExportLevelUpLearnsetsPointers(string filePath, IEnumerable<PokemonProfile> profiles)
		{
			StringBuilder content = new StringBuilder();

			content.AppendLine("// == WARNING ==");
			content.AppendLine("// DO NOT EDIT THIS FILE");
			content.AppendLine("// This file was automatically generated by PokemonDataGenerator");
			content.AppendLine("//");
			content.AppendLine($"");

			content.AppendLine($"const struct LevelUpMove *const gLevelUpLearnsets[NUM_SPECIES] =\n{{");

			foreach (var profile in profiles)
			{
				var moves = profile.GetLevelUpMoves();

				if (moves.Any())
				{
					content.AppendLine($"\t[{profile.Species}] = s{profile.PrettySpeciesName}LevelUpLearnset,");
				}
			}

			content.AppendLine($"}};");

			string fullStr = content.ToString().Replace("\t", "    ");
			File.WriteAllText(filePath, fullStr);
		}

		private static void ExportTMHMsLearnsets(string filePath, IEnumerable<PokemonProfile> profiles)
		{
			StringBuilder content = new StringBuilder();

			content.AppendLine("// == WARNING ==");
			content.AppendLine("// DO NOT EDIT THIS FILE");
			content.AppendLine("// This file was automatically generated by PokemonDataGenerator");
			content.AppendLine("//");
			content.AppendLine($"");

			content.AppendLine($"#define TMHM_ToBitIndex(item) (item > ITEM_LAST_VALID_TM ? (ITEM_LAST_VALID_TM - ITEM_TM01 + item - ITEM_HM01) : (item - ITEM_TM01))");
			content.AppendLine($"");
			content.AppendLine($"#define TMHM_EMPTY_LEARNSET() {{ ITEM_TMHM_COUNT }}");
			content.AppendLine($"#define TMHM_LEARNSET(moves) {{ moves ITEM_TMHM_COUNT }}");
			content.AppendLine($"#define TMHM(tmhm) (u16)(ITEM_##tmhm - ITEM_TM01),");
			content.AppendLine($"");

			content.AppendLine($"const u8 gTMHMLearnsets[][64] =\n{{");

			foreach (var profile in profiles)
			{
				var moves = profile.GetTMHMMoves();

				if (moves.Any())
				{
					content.AppendLine($"\t[{profile.Species}] = TMHM_LEARNSET(");

					foreach (var move in moves)
					{
						string item = GameDataHelpers.MoveToTMHMItem[FormatKeyword(move.moveName)];
						content.AppendLine($"\t\tTMHM({item.Substring("ITEM_".Length)})");
					}

					content.AppendLine($"\t),");
				}
				else
				{
					content.AppendLine($"\t[{profile.Species}] = TMHM_EMPTY_LEARNSET(),");
				}
			}

			content.AppendLine($"}};");

			string fullStr = content.ToString().Replace("\t", "    ");
			File.WriteAllText(filePath, fullStr);
		}

		private static void ExportTutorLearnsets(string filePath, IEnumerable<PokemonProfile> profiles)
		{
			StringBuilder content = new StringBuilder();

			content.AppendLine("// == WARNING ==");
			content.AppendLine("// DO NOT EDIT THIS FILE");
			content.AppendLine("// This file was automatically generated by PokemonDataGenerator");
			content.AppendLine("//");
			content.AppendLine($"");

			content.AppendLine(@"const u16 gTutorMoves[] =
{
    [TUTOR_MOVE_MEGA_PUNCH] = MOVE_MEGA_PUNCH,
    [TUTOR_MOVE_SWORDS_DANCE] = MOVE_SWORDS_DANCE,
    [TUTOR_MOVE_MEGA_KICK] = MOVE_MEGA_KICK,
    [TUTOR_MOVE_BODY_SLAM] = MOVE_BODY_SLAM,
    [TUTOR_MOVE_DOUBLE_EDGE] = MOVE_DOUBLE_EDGE,
    [TUTOR_MOVE_COUNTER] = MOVE_COUNTER,
    [TUTOR_MOVE_SEISMIC_TOSS] = MOVE_SEISMIC_TOSS,
    [TUTOR_MOVE_MIMIC] = MOVE_MIMIC,
    [TUTOR_MOVE_METRONOME] = MOVE_METRONOME,
    [TUTOR_MOVE_SOFT_BOILED] = MOVE_SOFT_BOILED,
    [TUTOR_MOVE_DREAM_EATER] = MOVE_DREAM_EATER,
    [TUTOR_MOVE_THUNDER_WAVE] = MOVE_THUNDER_WAVE,
    [TUTOR_MOVE_EXPLOSION] = MOVE_EXPLOSION,
    [TUTOR_MOVE_ROCK_SLIDE] = MOVE_ROCK_SLIDE,
    [TUTOR_MOVE_SUBSTITUTE] = MOVE_SUBSTITUTE,
    [TUTOR_MOVE_DYNAMIC_PUNCH] = MOVE_DYNAMIC_PUNCH,
    [TUTOR_MOVE_ROLLOUT] = MOVE_ROLLOUT,
    [TUTOR_MOVE_PSYCH_UP] = MOVE_PSYCH_UP,
    [TUTOR_MOVE_SNORE] = MOVE_SNORE,
    [TUTOR_MOVE_ICY_WIND] = MOVE_ICY_WIND,
    [TUTOR_MOVE_ENDURE] = MOVE_ENDURE,
    [TUTOR_MOVE_MUD_SLAP] = MOVE_MUD_SLAP,
    [TUTOR_MOVE_ICE_PUNCH] = MOVE_ICE_PUNCH,
    [TUTOR_MOVE_SWAGGER] = MOVE_SWAGGER,
    [TUTOR_MOVE_SLEEP_TALK] = MOVE_SLEEP_TALK,
    [TUTOR_MOVE_SWIFT] = MOVE_SWIFT,
    [TUTOR_MOVE_DEFENSE_CURL] = MOVE_DEFENSE_CURL,
    [TUTOR_MOVE_THUNDER_PUNCH] = MOVE_THUNDER_PUNCH,
    [TUTOR_MOVE_FIRE_PUNCH] = MOVE_FIRE_PUNCH,
    [TUTOR_MOVE_FURY_CUTTER] = MOVE_FURY_CUTTER,
};

#define TUTOR_LEARNSET(moves) ((u32)(moves))
#define TUTOR(move) ((u64)1 << (TUTOR_##move))
");

			content.AppendLine($"static const u32 sTutorLearnsets[] =\n{{");

			foreach (var profile in profiles)
			{
				var moves = profile.GetTutorMoves();

				if (moves.Any())
				{
					content.AppendLine($"\t[{profile.Species}] = TUTOR_LEARNSET(");

					foreach (var move in moves)
					{
						content.AppendLine($"\t\tTUTOR(MOVE_{FormatKeyword(move.moveName)})");
					}

					content.AppendLine($"\t),");
				}
				else
				{
					content.AppendLine($"\t[{profile.Species}] = (0),");
				}
			}

			content.AppendLine($"}};");

			string fullStr = content.ToString().Replace("\t", "    ");
			File.WriteAllText(filePath, fullStr);
		}

		private static void ExportEggMoves(string filePath, IEnumerable<PokemonProfile> profiles)
		{
			StringBuilder content = new StringBuilder();

			content.AppendLine("// == WARNING ==");
			content.AppendLine("// DO NOT EDIT THIS FILE");
			content.AppendLine("// This file was automatically generated by PokemonDataGenerator");
			content.AppendLine("//");
			content.AppendLine($"");

			content.AppendLine(@"#define EGG_MOVES_SPECIES_OFFSET 20000
#define EGG_MOVES_TERMINATOR 0xFFFF
#define egg_moves(species, moves...) (SPECIES_##species + EGG_MOVES_SPECIES_OFFSET), moves
");

			content.AppendLine($"const u16 gEggMoves[] = {{\n");

			foreach (var profile in profiles)
			{
				var moves = profile.GetEggMoves();

				if (moves.Any())
				{
					content.AppendLine($"\tegg_moves({profile.Species.Substring("SPECIES_".Length)}");
					bool isFirst = true;

					foreach (var move in moves)
					{
						content.AppendLine($"\t\t,MOVE_{FormatKeyword(move.moveName)}");
					}

					content.AppendLine($"\t),");
				}
			}

			content.AppendLine($"\tEGG_MOVES_TERMINATOR\n}};");

			string fullStr = content.ToString().Replace("\t", "    ");
			File.WriteAllText(filePath, fullStr);
		}

		private static string FormatKeyword(string keyword)
		{
			return keyword.Trim()
				.Replace(".", "")
				.Replace("’", "")
				.Replace("'", "")
				.Replace("%", "")
				.Replace(":", "")
				.Replace(" ", "_")
				.Replace("-", "_")
				.Replace("é", "e")
				.ToUpper();
		}
	}
}
