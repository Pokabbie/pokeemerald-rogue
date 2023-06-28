using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator.Pokedex
{

	public static class PokedexGenerator
	{

		private class PokedexData
		{
			public string InternalName;
			public string DisplayName;
			public int GenLimit;
			public List<string> Mons;
		}

		private static readonly string c_API = "https://pokeapi.co/api/v2/";
		private static Dictionary<string, string> m_DexApiLinks = null;


		public static void GeneratePokedexEntries(bool isVanillaVersion)
		{
			GatherAPILinks();

			List<PokedexData> fullDexes = new List<PokedexData>();

			if (isVanillaVersion)
			{
				fullDexes.Add(GatherDexData("kanto_RBY", "Red/Blue/Yellow", 1, "kanto"));
				fullDexes.Add(GatherDexData("johto_GSC", "Gold/Silver/Crystal", 2, "original-johto"));
				fullDexes.Add(GatherDexData("hoenn_RSE", "Ruby/Sapphire/Emerald", 3, "hoenn"));
			}
			else
			{
				// Purposely order so the most recent regional dex is first
				fullDexes.Add(GatherResourceDexData("rogue_main", "Emerald Rogue", 8, "Rogue Dex.csv"));

				fullDexes.Add(GatherDexData("kanto_letsgo", "Let'sGo", 7, "letsgo-kanto"));
				fullDexes.Add(GatherDexData("kanto_RBY", "Red/Blue/Yellow", 1, "kanto"));

				fullDexes.Add(GatherDexData("johto_HGSS", "HeartGold/SoulSilver", 4, "updated-johto"));
				fullDexes.Add(GatherDexData("johto_GSC", "Gold/Silver/Crystal", 2, "original-johto"));

				fullDexes.Add(GatherDexData("hoenn_ORAS", "OmegaRuby/AlphaSapphire", 6, "updated-hoenn"));
				fullDexes.Add(GatherDexData("hoenn_RSE", "Ruby/Sapphire/Emerald", 3, "hoenn"));

				fullDexes.Add(GatherDexData("sinnoh_PL", "Platinum", 4, "extended-sinnoh"));
				fullDexes.Add(GatherDexData("sinnoh_DP", "Diamond/Pearl", 4, "original-sinnoh"));

				fullDexes.Add(GatherDexData("unova_BW2", "Black2/White2", 5, "updated-unova"));
				fullDexes.Add(GatherDexData("unova_BW", "Black/White", 5, "original-unova"));
				fullDexes.Add(GatherDexData("unova_conquest", "Conquest", 5, "conquest-gallery"));

				fullDexes.Add(GatherDexData("kalos", "X/Y", 6, "kalos-central", "kalos-coastal", "kalos-mountain"));

				fullDexes.Add(GatherDexData("alola_USUM", "UltraSun/UltraMoon", 7, "updated-alola", "updated-melemele", "updated-akala", "updated-ulaula", "updated-poni"));
				fullDexes.Add(GatherDexData("alola_SM", "Sun/Moon", 7, "original-alola", "original-melemele", "original-akala", "original-ulaula", "original-poni"));

				fullDexes.Add(GatherDexData("galar_fulldlc", "Sword/Shield + DLC", 8, "galar", "isle-of-armor", "crown-tundra"));
				fullDexes.Add(GatherDexData("galar_crowntundra", "CrownTundra", 8, "crown-tundra"));
				fullDexes.Add(GatherDexData("galar_isleofarmor", "IsleOfArmor", 8, "isle-of-armor"));
				fullDexes.Add(GatherDexData("galar_swsh", "Sword/Shield", 8, "galar"));

				fullDexes.Add(GatherDexData("hisui_legendsarceus", "LegendsArceus", 8, "hisui"));
							}

			Dictionary<string, List<PokedexData>> regionVariants = new Dictionary<string, List<PokedexData>>();

			foreach (var dex in fullDexes)
			{
				string region = dex.InternalName.Split('_')[0];

				if (!regionVariants.ContainsKey(region))
					regionVariants[region] = new List<PokedexData>();

				regionVariants[region].Add(dex);
			}

			string outputDir = Path.GetFullPath($"pokedex_generator");
			ExportConstants(Path.Combine(outputDir, "constants", "rogue_pokedex.h"), fullDexes, regionVariants);
			ExportData(Path.Combine(outputDir, "src/data", "rogue_pokedex.h"), fullDexes, regionVariants);


			return;
		}

		private static void GatherAPILinks()
		{
			m_DexApiLinks = new Dictionary<string, string>();

			JObject dex = ContentCache.GetJsonContent(c_API + "pokedex");
			Console.WriteLine("Gathering Dex info...");

			while (true)
			{
				foreach (JObject result in dex["results"])
				{
					string name = result["name"].ToString();
					string url = result["url"].ToString();

					Console.WriteLine("\tFound " + name);
					m_DexApiLinks.Add(name, url);
				}

				var nextUri = dex["next"].ToString();
				if (!string.IsNullOrEmpty(nextUri))
				{
					dex = ContentCache.GetJsonContent(nextUri.ToString());
				}
				else
					break;
			}
		}

		private static PokedexData GatherDexData(string name, string displayName, int genLimit, params string[] dexIds)
		{
			Console.WriteLine($"Generating {name} dex data");

			PokedexData data = new PokedexData();
			data.InternalName = name;
			data.DisplayName = displayName;
			data.GenLimit = genLimit;
			data.Mons = new List<string>();

			foreach (string dexId in dexIds)
			{
				AppendDexMons(dexId, data);
			}

			return data;
		}

		private static void AppendDexMons(string dexId, PokedexData target)
		{
			string uri = m_DexApiLinks[dexId];
			JObject dex = ContentCache.GetJsonContent(uri);

			bool isHGSS = target.InternalName.Equals("johto_HGSS", StringComparison.CurrentCultureIgnoreCase);

			foreach (JObject entry in dex["pokemon_entries"])
			{
				string species = entry["pokemon_species"]["name"].ToString();
				if (!target.Mons.Contains(species))
				{
					target.Mons.Add(species);

					// HGSS dex is missing a lot of evos from regional dex, as they weren't techincally obtainable until post game
					// so forcefully insert them here
					if (isHGSS)
						AppendDexMon_ExtraHGSS(species, target);
				}
			}

			return;
		}

		private static PokedexData GatherResourceDexData(string name, string displayName, int genLimit, params string[] dexIds)
		{
			Console.WriteLine($"Generating {name} dex data");

			PokedexData data = new PokedexData();
			data.InternalName = name;
			data.DisplayName = displayName;
			data.GenLimit = genLimit;
			data.Mons = new List<string>();

			foreach (string dexId in dexIds)
			{
				AppendResourceDexMons(dexId, data);
			}

			return data;
		}

		private static void AppendResourceDexMons(string dexId, PokedexData target)
		{
			string csvContent = ContentCache.GetHttpContent("res://Pokedex/" + dexId);

			using(StringReader reader = new StringReader(csvContent))
			{
				string line;

				while((line = reader.ReadLine()) != null)
				{
					// Skip header
					if (line.StartsWith("Name,"))
						continue;

					string[] columns = line.Split(',');

					string species = columns[0].ToLower().Trim();
					if (!target.Mons.Contains(species))
					{
						target.Mons.Add(species);
					}
				}
			}

			return;
		}

		private static void AppendDexMon_ExtraHGSS(string species, PokedexData target)
		{
			if (species.Equals("magneton", StringComparison.CurrentCultureIgnoreCase))
			{
				target.Mons.Add("magnezone");
			}
			else if (species.Equals("lickitung", StringComparison.CurrentCultureIgnoreCase))
			{
				target.Mons.Add("lickilicky");
			}
			else if (species.Equals("rhydon", StringComparison.CurrentCultureIgnoreCase))
			{
				target.Mons.Add("rhyperior");
			}
			else if (species.Equals("tangela", StringComparison.CurrentCultureIgnoreCase))
			{
				target.Mons.Add("tangrowth");
			}
			else if (species.Equals("electabuzz", StringComparison.CurrentCultureIgnoreCase))
			{
				target.Mons.Add("electivire");
			}
			else if (species.Equals("magmar", StringComparison.CurrentCultureIgnoreCase))
			{
				target.Mons.Add("magmortar");
			}
			else if (species.Equals("umbreon", StringComparison.CurrentCultureIgnoreCase))
			{
				target.Mons.Add("leafeon");
				target.Mons.Add("glaceon");
			}
			else if (species.Equals("togetic", StringComparison.CurrentCultureIgnoreCase))
			{
				target.Mons.Add("togekiss");
			}
			else if (species.Equals("aipom", StringComparison.CurrentCultureIgnoreCase))
			{
				target.Mons.Add("ambipom");
			}
			else if (species.Equals("yanma", StringComparison.CurrentCultureIgnoreCase))
			{
				target.Mons.Add("yanmega");
			}
			else if (species.Equals("murkrow", StringComparison.CurrentCultureIgnoreCase))
			{
				target.Mons.Add("honchkrow");
			}
			else if (species.Equals("misdreavus", StringComparison.CurrentCultureIgnoreCase))
			{
				target.Mons.Add("mismagius");
			}
			else if (species.Equals("gligar", StringComparison.CurrentCultureIgnoreCase))
			{
				target.Mons.Add("gliscor");
			}
			else if (species.Equals("sneasel", StringComparison.CurrentCultureIgnoreCase))
			{
				target.Mons.Add("weavile");
			}
			else if (species.Equals("piloswine", StringComparison.CurrentCultureIgnoreCase))
			{
				target.Mons.Add("mamoswine");
			}
			else if (species.Equals("kirlia", StringComparison.CurrentCultureIgnoreCase))
			{
				target.Mons.Add("gallade");
			}
			else if (species.Equals("nosepass", StringComparison.CurrentCultureIgnoreCase))
			{
				target.Mons.Add("probopass");
			}
			else if (species.Equals("roselia", StringComparison.CurrentCultureIgnoreCase))
			{
				target.Mons.Add("roserade");
			}
			else if (species.Equals("dusclops", StringComparison.CurrentCultureIgnoreCase))
			{
				target.Mons.Add("dusknoir");
			}
			else if (species.Equals("snorunt", StringComparison.CurrentCultureIgnoreCase))
			{
				target.Mons.Add("froslass");
			}
		}

		private static void ExportConstants(string fileName, List<PokedexData> data, Dictionary<string, List<PokedexData>> regionData)
		{
			Console.WriteLine($"Exporting to '{fileName}'");
			Directory.CreateDirectory(Path.GetDirectoryName(fileName));

			StringBuilder content = new StringBuilder();

			content.AppendLine("// == WARNING ==");
			content.AppendLine("// DO NOT EDIT THIS FILE");
			content.AppendLine("// This file was automatically generated by PokemonDataGenerator");
			content.AppendLine("//");
			content.AppendLine($"");

			// Variants
			content.AppendLine($"#define POKEDEX_VARIANT_START    0");
			content.AppendLine($"");

			int counter = 0;
			foreach (var dex in data)
			{
				int dexIdx = counter++;
				content.AppendLine($"#define POKEDEX_VARIANT_{FormatKeyword(dex.InternalName)}    {dexIdx}");
			}

			content.AppendLine($"");
			content.AppendLine($"#define POKEDEX_VARIANT_END    POKEDEX_VARIANT_{FormatKeyword(data.Last().InternalName)}");
			content.AppendLine($"");
			content.AppendLine($"#define POKEDEX_VARIANT_COUNT    (POKEDEX_VARIANT_END - POKEDEX_VARIANT_START + 1)");
			content.AppendLine($"#define POKEDEX_VARIANT_NONE     (255)");
			content.AppendLine($"");

			// Regions
			content.AppendLine($"");
			content.AppendLine($"#define POKEDEX_REGION_START    0");
			content.AppendLine($"");

			counter = 0;
			foreach (var region in regionData)
			{
				int regionIdx = counter++;
				content.AppendLine($"#define POKEDEX_REGION_{FormatKeyword(region.Key)}    {regionIdx}");
			}

			content.AppendLine($"");
			content.AppendLine($"#define POKEDEX_REGION_END    POKEDEX_REGION_{FormatKeyword(regionData.Last().Key)}");
			content.AppendLine($"");
			content.AppendLine($"#define POKEDEX_REGION_COUNT    (POKEDEX_REGION_END - POKEDEX_REGION_START + 1)");
			content.AppendLine($"#define POKEDEX_REGION_NONE     (255)");
			content.AppendLine($"");

			string fullStr = content.ToString();
			File.WriteAllText(fileName, fullStr);
		}

		private static void ExportData(string fileName, List<PokedexData> data, Dictionary<string, List<PokedexData>> regionData)
		{
			Console.WriteLine($"Exporting to '{fileName}'");
			Directory.CreateDirectory(Path.GetDirectoryName(fileName));

			StringBuilder content = new StringBuilder();

			content.AppendLine("// == WARNING ==");
			content.AppendLine("// DO NOT EDIT THIS FILE");
			content.AppendLine("// This file was automatically generated by PokemonDataGenerator");
			content.AppendLine("//");

			// Add all mons in dex
			foreach (var dex in data)
			{
				content.AppendLine($"");
				content.AppendLine($"const u8 sRogueDexVariantName_{FormatKeyword(dex.InternalName)}[] = _(\"{dex.DisplayName}\");");

				content.AppendLine($"");
				content.AppendLine($"const u16 sRogueDexVariant_{FormatKeyword(dex.InternalName)}[] = ");
				content.AppendLine($"{{");

				foreach(var mon in dex.Mons)
				{
					content.AppendLine($"	SPECIES_{FormatKeyword(mon)},");
				}

				content.AppendLine($"}};");
			}

			// Add all regional dex variants
			foreach (var region in regionData)
			{
				string displayName = region.Key;
				displayName = char.ToUpper(displayName[0]).ToString() + string.Join("", displayName.Skip(1));

				content.AppendLine($"");
				content.AppendLine($"const u8 sRogueDexRegionName_{FormatKeyword(region.Key)}[] = _(\"{displayName}\");");

				content.AppendLine($"");
				content.AppendLine($"const u16 sRogueDexRegion_{FormatKeyword(region.Key)}[] = ");
				content.AppendLine($"{{");

				foreach (var dex in region.Value)
				{
					content.AppendLine($"\tPOKEDEX_VARIANT_{FormatKeyword(dex.InternalName)},");
				}

				content.AppendLine($"}};");
			}

			// Group all of the data into table

			// Variant
			content.AppendLine($"");
			content.AppendLine($"const struct RoguePokedexVariant gPokedexVariants[POKEDEX_VARIANT_COUNT] = ");

			content.AppendLine($"{{");
			foreach (var dex in data)
			{
				content.AppendLine($"\t[POKEDEX_VARIANT_{FormatKeyword(dex.InternalName)}] = ");
				content.AppendLine($"\t{{");
				content.AppendLine($"\t\t.displayName = sRogueDexVariantName_{FormatKeyword(dex.InternalName)},");
				content.AppendLine($"\t\t.speciesList = sRogueDexVariant_{FormatKeyword(dex.InternalName)},");
				content.AppendLine($"\t\t.speciesCount = ARRAY_COUNT(sRogueDexVariant_{FormatKeyword(dex.InternalName)}),");
				content.AppendLine($"\t\t.genLimit = {dex.GenLimit},");
				content.AppendLine($"\t}},");
			}
			content.AppendLine($"}};");

			// Regions
			content.AppendLine($"");
			content.AppendLine($"const struct RoguePokedexRegion gPokedexRegions[POKEDEX_REGION_COUNT] = ");

			// Group all of the data into table
			content.AppendLine($"{{");
			foreach (var region in regionData)
			{
				content.AppendLine($"\t[POKEDEX_REGION_{FormatKeyword(region.Key)}] = ");
				content.AppendLine($"\t{{");
				content.AppendLine($"\t\t.displayName = sRogueDexRegionName_{FormatKeyword(region.Key)},");
				content.AppendLine($"\t\t.variantList = sRogueDexRegion_{FormatKeyword(region.Key)},");
				content.AppendLine($"\t\t.variantCount = ARRAY_COUNT(sRogueDexRegion_{FormatKeyword(region.Key)}),");
				content.AppendLine($"\t}},");
			}
			content.AppendLine($"}};");

			string fullStr = content.ToString();
			File.WriteAllText(fileName, fullStr);
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
