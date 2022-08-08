using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator
{
	public static class MonPresetGenerator
	{
		private class PokemonPreset
		{
			public List<string> Moves = new List<string>();
			public string Ability;
			public string Item;

			public bool ContainsHiddenPower
			{
				get => Moves.Where((m) => m.StartsWith("Hidden Power", StringComparison.CurrentCultureIgnoreCase)).Any();
			}
		}

		private class PokemonData
		{
			public string PokemonName;
			public List<PokemonPreset> Presets = new List<PokemonPreset>();
		}

		private static Dictionary<string, PokemonData> s_PerPokemonData = new Dictionary<string, PokemonData>();

		private static PokemonData FindOrCreate(string pokemon)
		{
			PokemonData output;

			pokemon = pokemon.ToLower().Trim();
			if (!s_PerPokemonData.TryGetValue(pokemon, out output))
			{
				output = new PokemonData();
				output.PokemonName = pokemon;
				s_PerPokemonData.Add(pokemon, output);
			}

			return output;
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

		public static void GenerateFromURL(string url, bool useGen3Format)
		{
			using (HttpClient web = new HttpClient())
			{
				var task = web.GetStringAsync(url);
				task.Wait();
				GenerateFromJson(task.Result, useGen3Format);
			}
		}

		public static void GenerateFromJson(string jsonStr, bool useGen3Format)
		{
			JObject library = JObject.Parse(jsonStr);

			foreach (var categoryKvp in library)
			{
				var category = (JObject)categoryKvp.Value;

				if (categoryKvp.Key.EndsWith("hackmons", StringComparison.CurrentCultureIgnoreCase) ||
					categoryKvp.Key.EndsWith("cap", StringComparison.CurrentCultureIgnoreCase))
					continue;

				// There is stats too, although they don't seem to be as populated?
				var entries = (JObject)(category.ContainsKey("dex") ? category["dex"] : category["stats"]);

				foreach (var pokemonKvp in entries)
				{
					string pokemonName = pokemonKvp.Key;

					if (pokemonName.Equals("Eevee-starter", StringComparison.CurrentCultureIgnoreCase))
						continue;

					if (pokemonName.StartsWith("Deoxys", StringComparison.CurrentCultureIgnoreCase))
					{
						// Only use the normal form, as others don't exist in emerald for AI
						if (pokemonName.Equals("Deoxys", StringComparison.CurrentCultureIgnoreCase))
							pokemonName = "Deoxys";
						else
							continue;
					}

					if (pokemonName.StartsWith("Darmanitan-Zen", StringComparison.CurrentCultureIgnoreCase))
						pokemonName = "Darmanitan-Zen-Mode" + pokemonName.Substring("Darmanitan Zen".Length);

					if (pokemonName.Equals("Darmanitan-Galar-Zen", StringComparison.CurrentCultureIgnoreCase))
						pokemonName = "Darmanitan-Zen-Mode-Galar";

					if (pokemonName.Equals("Meowstic-f", StringComparison.CurrentCultureIgnoreCase))
						pokemonName = "Meowstic-female";

					if (pokemonName.Equals("Indeedee-f", StringComparison.CurrentCultureIgnoreCase))
						pokemonName = "Indeedee-female";

					if (pokemonName.StartsWith("Calyrex-", StringComparison.CurrentCultureIgnoreCase))
						pokemonName += "-rider";

					if (pokemonName.EndsWith("Urshifu-rapid-strike", StringComparison.CurrentCultureIgnoreCase))
						pokemonName += "-style";

					if (pokemonName.EndsWith("Wormadam-Sandy", StringComparison.CurrentCultureIgnoreCase))
						pokemonName += "-cloak";
					if (pokemonName.EndsWith("Wormadam-Trash", StringComparison.CurrentCultureIgnoreCase))
						pokemonName += "-cloak";

					if (pokemonName.EndsWith("Alola", StringComparison.CurrentCultureIgnoreCase))
						pokemonName += "n";
					if (pokemonName.EndsWith("Galar", StringComparison.CurrentCultureIgnoreCase))
						pokemonName += "ian";

					var sets = (JObject)pokemonKvp.Value;

					var pokemonData = FindOrCreate(pokemonName);

					foreach (var setKvp in sets)
					{
						PokemonPreset preset = new PokemonPreset();
						var set = (JObject)setKvp.Value;

						foreach (var move in set["moves"])
						{
							string moveString = move.ToString();
							if (moveString.StartsWith("Hidden Power", StringComparison.CurrentCultureIgnoreCase)) // Not tracking the Hidden power type
							{
								moveString = "Hidden Power";
							}
							else if (useGen3Format)
							{
								if (moveString.Equals("High Jump Kick", StringComparison.CurrentCultureIgnoreCase))
								{
									moveString = "Hi Jump Kick";
								}
								else if (moveString.Equals("Feint Attack", StringComparison.CurrentCultureIgnoreCase))
								{
									moveString = "Faint Attack";
								}
							}
							else
							{
							}

							preset.Moves.Add(moveString);
						}

						preset.Ability = null;
						if (set.ContainsKey("ability"))
						{
							preset.Ability = set["ability"].ToString();

							if (preset.Ability.Equals("As One (GLASTRIER)", StringComparison.CurrentCultureIgnoreCase))
								preset.Ability = null;
							else if (preset.Ability.Equals("As One (SPECTRIER)", StringComparison.CurrentCultureIgnoreCase))
								preset.Ability = null;
						}

						preset.Item = null;
						if (set.ContainsKey("item"))
						{
							preset.Item = set["item"].ToString();

							if (!useGen3Format)
							{
								if (preset.Item.Equals("Stick", StringComparison.CurrentCultureIgnoreCase))
									preset.Item = null; // Doesn't seem to exist in expansion :(
							}
						}

						pokemonData.Presets.Add(preset);
					}
				}
			}
		}

		public static void ExportToHeader(string filePath)
		{
			StringBuilder upperBlock = new StringBuilder();
			StringBuilder lowerBlock = new StringBuilder();

			upperBlock.AppendLine("// == WARNING ==");
			upperBlock.AppendLine("// DO NOT EDIT THIS FILE");
			upperBlock.AppendLine("// This file was automatically generated by PokemonDataGenerator\n");

			lowerBlock.AppendLine("const struct RogueMonPresetCollection gPresetMonTable[NUM_SPECIES] =");
			lowerBlock.AppendLine("{");

			foreach (var pokemon in s_PerPokemonData.Values)
			{
				upperBlock.AppendLine($"static const struct RogueMonPreset sRoguePresets_{FormatKeyword(pokemon.PokemonName)}[] = ");
				upperBlock.AppendLine("{");

				IEnumerable<PokemonPreset> presets = pokemon.Presets;
				//if (presets.Where((p) => !p.ContainsHiddenPower).Any())
				//{
				//	// We have presets which don't contain hidden power so prefer those
				//	// As we're not controlling the hidden power typing
				//	presets = presets.Where((p) => !p.ContainsHiddenPower);
				//}

				foreach (var preset in pokemon.Presets)
				{
					upperBlock.AppendLine("\t{");

					if (preset.Ability == null || preset.Ability.Equals("No Ability", StringComparison.CurrentCultureIgnoreCase))
						upperBlock.AppendLine($"\t\t.abilityNum = ABILITY_NONE,");
					else
						upperBlock.AppendLine($"\t\t.abilityNum = ABILITY_{FormatKeyword(preset.Ability)},");

					if (preset.Item == null)
						upperBlock.AppendLine($"\t\t.heldItem = ITEM_NONE,");
					else
						upperBlock.AppendLine($"\t\t.heldItem = ITEM_{FormatKeyword(preset.Item)},");

					upperBlock.Append($"\t\t.moves = {{ ");

					for (int i = 0; i < 4; ++i)
					{
						if (i != 0)
							upperBlock.Append($", ");

						if (i < preset.Moves.Count)
							upperBlock.Append($"MOVE_{FormatKeyword(preset.Moves[i])}");
						else
							upperBlock.Append($"MOVE_NONE");
					}

					upperBlock.AppendLine($"}}");

					upperBlock.AppendLine("\t},");
				}

				upperBlock.AppendLine("};");
				upperBlock.AppendLine("");

				lowerBlock.AppendLine($"\t[SPECIES_{FormatKeyword(pokemon.PokemonName)}] = {{");
				lowerBlock.AppendLine($"\t\t.presetCount = ARRAY_COUNT(sRoguePresets_{FormatKeyword(pokemon.PokemonName)}),");
				lowerBlock.AppendLine($"\t\t.presets = sRoguePresets_{FormatKeyword(pokemon.PokemonName)}");
				lowerBlock.AppendLine("\t},");
				lowerBlock.AppendLine("");
			}

			lowerBlock.AppendLine("};");

			string fullStr = upperBlock.ToString() + "\n" + lowerBlock.ToString();
			File.WriteAllText(filePath, fullStr);

			Console.WriteLine($"Output to '{filePath}'");
		}
	}
}
