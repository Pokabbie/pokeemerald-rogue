using Newtonsoft.Json;
using PokemonDataGenerator.OverworldSprites;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace PokemonDataGenerator
{
	public static class OverworldSpriteGenerator
	{
		public static bool s_TargettingVanilla = true;
		public static bool s_TargettingDebugSet = false;
		public static bool s_GenerateShinies = true;

		private struct Subpath
		{
			public string key;
			public string path;
		}

		private class MonSpriteData
		{
			public Dictionary<string, string> spriteUri = new Dictionary<string, string>();
			public int pokedexNumber;
			public Bitmap collatedSpriteSheet;
			public Bitmap collatedShinySpriteSheet;
			public Dictionary<ImagePalette, double> paletteScores = new Dictionary<ImagePalette, double>();
			public Dictionary<ImagePalette, double> shinyPaletteScores = new Dictionary<ImagePalette, double>();
			public int paletteIdx;
			public int shinyPaletteIdx;
			public int spriteSize;
			public int shinySpriteSize;
		}

		public class ImportData
		{
			public Dictionary<string, string> monPaletteAssignment = new Dictionary<string, string>();
		}

		private static readonly string c_BaseURI = @"https://www.pokencyclopedia.info";
		private static readonly string c_BaseURL = @"https://www.pokencyclopedia.info/en/index.php?id=sprites/overworlds";
		private static readonly Subpath[] c_Subpaths =
		{
			new Subpath { key="front", path="o_hgss" },
			new Subpath { key="front_shiny", path="o_hgss_shiny" },
			new Subpath { key="back", path="o-b_hgss" },
			new Subpath { key="back_shiny", path="o-b_hgss_shiny" },
			new Subpath { key="side", path="o-l_hgss" },
			new Subpath { key="side_shiny", path="o-l_hgss_shiny" },
		};

		private static readonly Tuple<string, ImagePalette>[] c_PaletteOptions =
		{
				new Tuple<string, ImagePalette>("OBJ_EVENT_PAL_TAG_NPC_1", ImagePalette.FromFile(@"G:\SideProjects\Pokemon\pokeemerald-rogue\graphics\object_events\palettes\npc_1.pal", false)),
				new Tuple<string, ImagePalette>("OBJ_EVENT_PAL_TAG_NPC_2", ImagePalette.FromFile(@"G:\SideProjects\Pokemon\pokeemerald-rogue\graphics\object_events\palettes\npc_2.pal", false)),
				new Tuple<string, ImagePalette>("OBJ_EVENT_PAL_TAG_NPC_3", ImagePalette.FromFile(@"G:\SideProjects\Pokemon\pokeemerald-rogue\graphics\object_events\palettes\npc_3.pal", false)),
				new Tuple<string, ImagePalette>("OBJ_EVENT_PAL_TAG_NPC_4", ImagePalette.FromFile(@"G:\SideProjects\Pokemon\pokeemerald-rogue\graphics\object_events\palettes\npc_4.pal", false)),
				new Tuple<string, ImagePalette>("OBJ_EVENT_PAL_TAG_FOLLOW_MON_1", ImagePalette.FromFile(@"G:\SideProjects\Pokemon\pokeemerald-rogue\graphics\object_events\palettes\followmon_1.pal", false)),
				new Tuple<string, ImagePalette>("OBJ_EVENT_PAL_TAG_FOLLOW_MON_2", ImagePalette.FromFile(@"G:\SideProjects\Pokemon\pokeemerald-rogue\graphics\object_events\palettes\followmon_2.pal", false)),
				new Tuple<string, ImagePalette>("OBJ_EVENT_PAL_TAG_FOLLOW_MON_3", ImagePalette.FromFile(@"G:\SideProjects\Pokemon\pokeemerald-rogue\graphics\object_events\palettes\followmon_3.pal", false)),
				new Tuple<string, ImagePalette>("OBJ_EVENT_PAL_TAG_FOLLOW_MON_4", ImagePalette.FromFile(@"G:\SideProjects\Pokemon\pokeemerald-rogue\graphics\object_events\palettes\followmon_4.pal", false)),

				new Tuple<string, ImagePalette>("OBJ_EVENT_PAL_TAG_NPC_1", ImagePalette.FromFile(@"G:\SideProjects\Pokemon\pokeemerald-rogue\graphics\object_events\palettes\npc_1.pal", true)),
				new Tuple<string, ImagePalette>("OBJ_EVENT_PAL_TAG_NPC_2", ImagePalette.FromFile(@"G:\SideProjects\Pokemon\pokeemerald-rogue\graphics\object_events\palettes\npc_2.pal", true)),
				new Tuple<string, ImagePalette>("OBJ_EVENT_PAL_TAG_NPC_3", ImagePalette.FromFile(@"G:\SideProjects\Pokemon\pokeemerald-rogue\graphics\object_events\palettes\npc_3.pal", true)),
				new Tuple<string, ImagePalette>("OBJ_EVENT_PAL_TAG_NPC_4", ImagePalette.FromFile(@"G:\SideProjects\Pokemon\pokeemerald-rogue\graphics\object_events\palettes\npc_4.pal", true)),
				new Tuple<string, ImagePalette>("OBJ_EVENT_PAL_TAG_FOLLOW_MON_1", ImagePalette.FromFile(@"G:\SideProjects\Pokemon\pokeemerald-rogue\graphics\object_events\palettes\followmon_1.pal", true)),
				new Tuple<string, ImagePalette>("OBJ_EVENT_PAL_TAG_FOLLOW_MON_2", ImagePalette.FromFile(@"G:\SideProjects\Pokemon\pokeemerald-rogue\graphics\object_events\palettes\followmon_2.pal", true)),
				new Tuple<string, ImagePalette>("OBJ_EVENT_PAL_TAG_FOLLOW_MON_3", ImagePalette.FromFile(@"G:\SideProjects\Pokemon\pokeemerald-rogue\graphics\object_events\palettes\followmon_3.pal", true)),
				new Tuple<string, ImagePalette>("OBJ_EVENT_PAL_TAG_FOLLOW_MON_4", ImagePalette.FromFile(@"G:\SideProjects\Pokemon\pokeemerald-rogue\graphics\object_events\palettes\followmon_4.pal", true)),
			};

		private static Dictionary<string, MonSpriteData> s_MonToSpriteData = new Dictionary<string, MonSpriteData>();

		public static bool IsValidSpriteKey(string key)
		{
			return c_Subpaths.Where((p) => p.key + "_1" == key || p.key + "_2" == key).Any();
		}

		public static void GenerateFromURL(bool isCollating)
		{
			SpriteSheetSplitter.AppendGenericMonSprites32("none", 0, "", "res://none_overworld.png");

			if(s_GenerateShinies)
				SpriteSheetSplitter.AppendGenericMonSprites32("none", 0, "_shiny", "res://none_overworld_shiny.png");

			//if (!s_TargettingDebugSet) // TEMP DON'T CHECK IN
			//{
			// Gather source paths
			foreach (var subpath in c_Subpaths)
			{
				string fullUrl = c_BaseURL + "/" + subpath.path;
				Console.WriteLine($"Gathering '{fullUrl}'");

				string content = ContentCache.GetHttpContent(fullUrl);
				AppendSpriteResults(content, subpath.key);
			}
			//}

			if (!s_TargettingVanilla)
			{
				SpriteSheetSplitter_Gen5.AppendMonSprites();
				SpriteSheetSplitter_Gen6.AppendMonSprites();
				SpriteSheetSplitter_Gen7.AppendMonSprites();
				SpriteSheetSplitter_Gen8.AppendMonSprites();
			}

			string outDir = Path.GetFullPath("sprite_out");

			if (isCollating)
			{
				if (Directory.Exists(outDir))
					Directory.Delete(outDir, true);

				Directory.CreateDirectory(outDir);

				CollateSpriteSheets(Path.Combine(outDir, "raw"));

				for (int i = 0; i < c_PaletteOptions.Length; ++i)
				{
					Console.WriteLine($"Generated paletted sprite for {i}:{c_PaletteOptions[i].Item1}");
					GenerateSetForPalette(c_PaletteOptions[i].Item2, Path.Combine(outDir, "pal_" + i));
				}
			}
			else
			{
				string finalOutputDir = Path.Combine(outDir, "output");

				if (Directory.Exists(finalOutputDir))
					Directory.Delete(finalOutputDir, true);

				Directory.CreateDirectory(finalOutputDir);

				// Import previously generated palette mapping (Manually reviewed based on generated sprites to get best option)
				string paletteMappingPath = Path.GetFullPath(@"..\..\mon_palette_mapping.json");
				string jsonData = File.ReadAllText(paletteMappingPath);
				ImportData importData = JsonConvert.DeserializeObject<ImportData>(jsonData);

				foreach (var kvp in s_MonToSpriteData)
				{
					var mon = kvp.Key;
					var data = kvp.Value;

					// Remove "pal_"
					string mappedPallete = importData.monPaletteAssignment[mon];
					data.paletteIdx = int.Parse(mappedPallete.Substring("pal_".Length));

					string sourceFile = Path.Combine(outDir, mappedPallete, mon + ".png");
					data.spriteSize = Bitmap.FromFile(sourceFile).Height;

					File.Copy(sourceFile, Path.Combine(finalOutputDir, mon + ".png"));

					if (s_GenerateShinies)
					{
						string mappedShinyPallete = importData.monPaletteAssignment[mon + "_shiny"];
						data.shinyPaletteIdx = int.Parse(mappedShinyPallete.Substring("pal_".Length));

						string sourceShinyFile = Path.Combine(outDir, mappedPallete, mon + "_shiny.png");
						data.shinySpriteSize = Bitmap.FromFile(sourceShinyFile).Height;

						File.Copy(sourceShinyFile, Path.Combine(finalOutputDir, mon + "_shiny.png"));
					}

					Console.WriteLine($"\t{mon} using palletes normal:{data.paletteIdx} shiny:{data.shinyPaletteIdx}");
				}

				ExportPaletteAssignedSprites(c_PaletteOptions, finalOutputDir);
			}
		}

		private static string GetPalSlotFromTag(string tagName)
		{
			switch(tagName)
			{
				case "OBJ_EVENT_PAL_TAG_NPC_1":
					return "2";
				case "OBJ_EVENT_PAL_TAG_NPC_2":
					return "3";
				case "OBJ_EVENT_PAL_TAG_NPC_3":
					return "4";
				case "OBJ_EVENT_PAL_TAG_NPC_4":
					return "5";

				case "OBJ_EVENT_PAL_TAG_FOLLOW_MON_1":
					return "6";
				case "OBJ_EVENT_PAL_TAG_FOLLOW_MON_2":
					return "7";
				case "OBJ_EVENT_PAL_TAG_FOLLOW_MON_3":
					return "8";
				case "OBJ_EVENT_PAL_TAG_FOLLOW_MON_4":
					return "9";
			}

			return "???";
		}

		private static void CollateSpriteSheets(string outDir)
		{
			Directory.CreateDirectory(outDir);

			foreach (var kvp in s_MonToSpriteData)
			{
				var mon = kvp.Key;
				var data = kvp.Value;

				data.collatedSpriteSheet = CollateSpriteSheet(mon, "");

				data.spriteSize = data.collatedSpriteSheet.Height;
				data.paletteIdx = -1;

				Console.WriteLine($"\tCollated {mon} sprites");
				string localPath = Path.Combine(outDir, mon + ".png");
				data.collatedSpriteSheet.Save(localPath);

				if (s_GenerateShinies)
				{
					data.collatedShinySpriteSheet = CollateSpriteSheet(mon, "_shiny");

					data.shinySpriteSize = data.collatedShinySpriteSheet.Height;
					data.shinyPaletteIdx = -1;

					Console.WriteLine($"\tCollated {mon} sprites");
					string shinyLocalPath = Path.Combine(outDir, mon + "_shiny.png");
					data.collatedShinySpriteSheet.Save(shinyLocalPath);
				}
			}
		}

		private static void GenerateSetForPalette(ImagePalette palette, string outDir)
		{
			Directory.CreateDirectory(outDir);

			foreach (var kvp in s_MonToSpriteData)
			{
				var mon = kvp.Key;
				var data = kvp.Value;

				Bitmap convertedSprite = palette.CreateIndexedBitmap(data.collatedSpriteSheet);
				string localPath = Path.Combine(outDir, mon + ".png");
				convertedSprite.Save(localPath);

				data.paletteScores.Add(palette, palette.GetBitmapMatchScore(data.collatedSpriteSheet));

				if(s_GenerateShinies)
				{
					Bitmap convertedShinySprite = palette.CreateIndexedBitmap(data.collatedShinySpriteSheet);
					string localShinyPath = Path.Combine(outDir, mon + "_shiny.png");
					convertedShinySprite.Save(localShinyPath);

					data.shinyPaletteScores.Add(palette, palette.GetBitmapMatchScore(data.collatedSpriteSheet));
				}
			}
		}

		private static void ExportPaletteAssignedSprites(Tuple<string, ImagePalette>[] palettes, string outDir)
		{
			// Output any boilerplate code to copy
			//
			string dataDir = Path.Combine(outDir, "include");
			Directory.CreateDirectory(dataDir);

			using (FileStream stream = new FileStream(Path.Combine(dataDir, "spritesheet_rules_gen.mk"), FileMode.Create))
			using (StreamWriter writer = new StreamWriter(stream))
			{
				foreach (var kvp in s_MonToSpriteData.OrderBy((kvp) => kvp.Value.pokedexNumber))
				{
					var mon = kvp.Key;
					var data = kvp.Value;
					int dims = data.spriteSize / 8;

					writer.WriteLine($"$(OBJEVENTGFXDIR)/pokemon_ow/{mon}.4bpp: %.4bpp: %.png");
					writer.WriteLine($"	$(GFX) $< $@ -mwidth {dims} -mheight {dims}");
					writer.WriteLine($"");

					if (s_GenerateShinies)
					{
						dims = data.shinySpriteSize / 8;

						writer.WriteLine($"$(OBJEVENTGFXDIR)/pokemon_ow/{mon}_shiny.4bpp: %.4bpp: %.png");
						writer.WriteLine($"	$(GFX) $< $@ -mwidth {dims} -mheight {dims}");
						writer.WriteLine($"");
					}
				}
			}

			using (FileStream stream = new FileStream(Path.Combine(dataDir, "object_event_graphics_gen.h"), FileMode.Create))
			using (StreamWriter writer = new StreamWriter(stream))
			{
				foreach (var kvp in s_MonToSpriteData.OrderBy((kvp) => kvp.Value.pokedexNumber))
				{
					var mon = kvp.Key;
					var data = kvp.Value;

					writer.WriteLine($"const u32 gObjectEventPic_Overworld_{mon}[] = INCBIN_U32(\"graphics/object_events/pics/pokemon_ow/{mon}.4bpp\");");

					if (s_GenerateShinies)
						writer.WriteLine($"const u32 gObjectEventPic_Overworld_{mon}_shiny[] = INCBIN_U32(\"graphics/object_events/pics/pokemon_ow/{mon}_shiny.4bpp\");");
				}

				// If we want to dynamically build the object event gfx info table this can be useful, otherwise it doesn't matter
				//writer.WriteLine($"");
				//writer.WriteLine($"const struct PokemonObjectEventInfo gPokemonObjectEventInfo[NUM_SPECIES] = {{");
				//
				//foreach (var kvp in s_MonToSpriteData.OrderBy((kvp) => kvp.Value.pokedexNumber))
				//{
				//	var mon = kvp.Key;
				//	var data = kvp.Value;
				//	int dims = data.spriteSize / 8;
				//
				//	if (s_GenerateShinies)
				//		writer.WriteLine($"	[SPECIES_{mon.ToUpper()}] = {{ .defaultPic = gObjectEventPic_Overworld_{mon}, .shinyPic = gObjectEventPic_Overworld_{mon}_shiny, .width={dims}, .height={dims}, .defaultPaletteOffset={data.paletteIdx}, .shinyPaletteOffset={data.shinyPaletteIdx} }},");
				//	else
				//		writer.WriteLine($"	[SPECIES_{mon.ToUpper()}] = {{ .defaultPic = gObjectEventPic_Overworld_{mon}, .shinyPic = NULL, .width={dims}, .height={dims}, .defaultPaletteOffset={data.paletteIdx}, .shinyPaletteOffset=0 }},");
				//}
				//
				//writer.WriteLine($"}};");
			}

			using (FileStream stream = new FileStream(Path.Combine(dataDir, "object_event_graphics_info_gen.h"), FileMode.Create))
			using (StreamWriter writer = new StreamWriter(stream))
			{
				foreach (var kvp in s_MonToSpriteData.OrderBy((kvp) => kvp.Value.pokedexNumber))
				{
					var mon = kvp.Key;
					var data = kvp.Value;
					int dims = data.spriteSize / 8;

					writer.WriteLine($"static const struct SpriteFrameImage sPicTable_Mon_{mon}[] = {{");
					writer.WriteLine($"    overworld_frame(gObjectEventPic_Overworld_{mon}, {dims}, {dims}, 0),");
					writer.WriteLine($"    overworld_frame(gObjectEventPic_Overworld_{mon}, {dims}, {dims}, 1),");
					writer.WriteLine($"    overworld_frame(gObjectEventPic_Overworld_{mon}, {dims}, {dims}, 2),");
					writer.WriteLine($"    overworld_frame(gObjectEventPic_Overworld_{mon}, {dims}, {dims}, 0),");
					writer.WriteLine($"    overworld_frame(gObjectEventPic_Overworld_{mon}, {dims}, {dims}, 3),");
					writer.WriteLine($"    overworld_frame(gObjectEventPic_Overworld_{mon}, {dims}, {dims}, 1),");
					writer.WriteLine($"    overworld_frame(gObjectEventPic_Overworld_{mon}, {dims}, {dims}, 4),");
					writer.WriteLine($"    overworld_frame(gObjectEventPic_Overworld_{mon}, {dims}, {dims}, 2),");
					writer.WriteLine($"    overworld_frame(gObjectEventPic_Overworld_{mon}, {dims}, {dims}, 5),");
					writer.WriteLine($"}};");

					if (s_GenerateShinies)
					{
						dims = data.shinySpriteSize / 8;

						writer.WriteLine($"static const struct SpriteFrameImage sPicTable_Mon_{mon}_shiny[] = {{");
						writer.WriteLine($"    overworld_frame(gObjectEventPic_Overworld_{mon}_shiny, {dims}, {dims}, 0),");
						writer.WriteLine($"    overworld_frame(gObjectEventPic_Overworld_{mon}_shiny, {dims}, {dims}, 1),");
						writer.WriteLine($"    overworld_frame(gObjectEventPic_Overworld_{mon}_shiny, {dims}, {dims}, 2),");
						writer.WriteLine($"    overworld_frame(gObjectEventPic_Overworld_{mon}_shiny, {dims}, {dims}, 0),");
						writer.WriteLine($"    overworld_frame(gObjectEventPic_Overworld_{mon}_shiny, {dims}, {dims}, 3),");
						writer.WriteLine($"    overworld_frame(gObjectEventPic_Overworld_{mon}_shiny, {dims}, {dims}, 1),");
						writer.WriteLine($"    overworld_frame(gObjectEventPic_Overworld_{mon}_shiny, {dims}, {dims}, 4),");
						writer.WriteLine($"    overworld_frame(gObjectEventPic_Overworld_{mon}_shiny, {dims}, {dims}, 2),");
						writer.WriteLine($"    overworld_frame(gObjectEventPic_Overworld_{mon}_shiny, {dims}, {dims}, 5),");
						writer.WriteLine($"}};");
					}

					writer.WriteLine($"");
				}

				writer.WriteLine($"");

				foreach (var kvp in s_MonToSpriteData.OrderBy((kvp) => kvp.Value.pokedexNumber))
				{
					var mon = kvp.Key;
					var data = kvp.Value;
					int dims = data.spriteSize / 8;

					if (dims == 8) // 64x64
						writer.WriteLine($"const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_Mon_{mon} = {{TAG_NONE, {palettes[data.paletteIdx].Item1}, OBJ_EVENT_PAL_TAG_NONE, 2048, 64, 64, {GetPalSlotFromTag(palettes[data.paletteIdx].Item1)}, SHADOW_SIZE_M, FALSE, FALSE, TRACKS_FOOT, &gObjectEventBaseOam_64x64, sOamTables_64x64, sAnimTable_GenericOverworldMon, sPicTable_Mon_{mon}, gDummySpriteAffineAnimTable}};");
					else
						writer.WriteLine($"const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_Mon_{mon} = {{TAG_NONE, {palettes[data.paletteIdx].Item1}, OBJ_EVENT_PAL_TAG_NONE, 512, 32, 32, {GetPalSlotFromTag(palettes[data.paletteIdx].Item1)}, SHADOW_SIZE_M, FALSE, FALSE, TRACKS_FOOT, &gObjectEventBaseOam_32x32, sOamTables_32x32, sAnimTable_GenericOverworldMon, sPicTable_Mon_{mon}, gDummySpriteAffineAnimTable}};");

					if (s_GenerateShinies)
					{
						dims = data.shinySpriteSize / 8;

						if (dims == 8) // 64x64
							writer.WriteLine($"const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_Mon_{mon}_shiny = {{TAG_NONE, {palettes[data.shinyPaletteIdx].Item1}, OBJ_EVENT_PAL_TAG_NONE, 2048, 64, 64, {GetPalSlotFromTag(palettes[data.shinyPaletteIdx].Item1)}, SHADOW_SIZE_M, FALSE, FALSE, TRACKS_FOOT, &gObjectEventBaseOam_64x64, sOamTables_64x64, sAnimTable_GenericOverworldMon, sPicTable_Mon_{mon}_shiny, gDummySpriteAffineAnimTable}};");
						else
							writer.WriteLine($"const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_Mon_{mon}_shiny = {{TAG_NONE, {palettes[data.shinyPaletteIdx].Item1}, OBJ_EVENT_PAL_TAG_NONE, 512, 32, 32, {GetPalSlotFromTag(palettes[data.shinyPaletteIdx].Item1)}, SHADOW_SIZE_M, FALSE, FALSE, TRACKS_FOOT, &gObjectEventBaseOam_32x32, sOamTables_32x32, sAnimTable_GenericOverworldMon, sPicTable_Mon_{mon}_shiny, gDummySpriteAffineAnimTable}};");

					}
				}

				writer.WriteLine($"");
				writer.WriteLine($"const struct ObjectEventGraphicsInfo *const gObjectEventMonGraphicsInfoPointers[NUM_SPECIES] = {{");

				foreach (var kvp in s_MonToSpriteData.OrderBy((kvp) => kvp.Value.pokedexNumber))
				{
					var mon = kvp.Key;
					var data = kvp.Value;
					int dims = data.spriteSize / 8;

					writer.WriteLine($"	[SPECIES_{mon.ToUpper()}] = &gObjectEventGraphicsInfo_Mon_{mon},");
				}
				writer.WriteLine($"}};");

				writer.WriteLine($"");
				writer.WriteLine($"const struct ObjectEventGraphicsInfo *const gObjectEventShinyMonGraphicsInfoPointers[NUM_SPECIES] = {{");

				if (s_GenerateShinies)
				{
					foreach (var kvp in s_MonToSpriteData.OrderBy((kvp) => kvp.Value.pokedexNumber))
					{
						var mon = kvp.Key;
						var data = kvp.Value;
						int dims = data.shinySpriteSize / 8;

						writer.WriteLine($"	[SPECIES_{mon.ToUpper()}] = &gObjectEventGraphicsInfo_Mon_{mon}_shiny,");
					}
				}
				writer.WriteLine($"}};");
			}
		}

		public static void AppendMonSpriteUri(string mon, int pokedexNumber, string key, string uri)
		{
			// Debug - Smaller set for testing
			//if (s_TargettingDebugSet && pokedexNumber >= 10)
			//{
			//	Console.WriteLine($"\tSkipping '{mon}' '{key}' (Entry number too high)");
			//	return;
			//}

			if (s_TargettingVanilla && pokedexNumber >= 387)
			{
				Console.WriteLine($"\tSkipping '{mon}' '{key}' (Entry number too high)");
				return;
			}

			switch(mon)
			{
				case "spiky_eared_pichu":
						Console.WriteLine($"\tSkipping '{mon}' '{key}' (Excluded from set)");
						return;
			}

			if(s_TargettingVanilla)
			{
				switch (mon)
				{
					case "deoxys_attack_forme":
					case "deoxys_defense_forme":
					case "deoxys_speed_forme":
						Console.WriteLine($"\tSkipping '{mon}' '{key}' (Excluded from set)");
						return;
				}

				// Temp hack until decide what is happening with unown (JUST FOR VANILLA)
				if (mon.StartsWith("unown"))
				{
					if (mon == "unown_qmark")
					{
						mon = "unown";
					}
					else
					{
						Console.WriteLine($"\tSkipping '{mon}' '{key}' (Excluded from set)");
						return;
					}
				}
			}
			else
			{
				if (mon.EndsWith("_forme"))
					mon = mon.Substring(0, mon.Length - "_forme".Length);

				if(mon.StartsWith("arceus_") && mon.EndsWith("_type"))
				{
					//mon = mon.Substring(0, mon.Length - "_type".Length);
					Console.WriteLine($"\tSkipping '{mon}' '{key}' (Excluded from set)");
					return;
				}

				switch (mon)
				{
					// Renames
					//
					case "unown_a":
						mon = "unown";
						break;

					case "burmy_plant_cloak":
						mon = "burmy";
						break;
					case "wormadam_plant_cloak":
						mon = "wormadam";
						break;

					case "shellos_west_sea":
						mon = "shellos";
						break;
					case "gastrodon_west_sea":
						mon = "gastrodon";
						break;

					case "heat_rotom":
						mon = "rotom_heat";
						break;
					case "wash_rotom":
						mon = "rotom_wash";
						break;
					case "frost_rotom":
						mon = "rotom_frost";
						break;
					case "fan_rotom":
						mon = "rotom_fan";
						break;
					case "mow_rotom":
						mon = "rotom_mow";
						break;

					case "shaymin_land":
						mon = "shaymin";
						break;
					case "giratina_altered":
						mon = "giratina";
						break;

					// Excludes
					//
					case "arceus_qmarkqmarkqmark":
						Console.WriteLine($"\tSkipping '{mon}' '{key}' (Excluded from set)");
						return;
				}
			}

			if (!s_MonToSpriteData.ContainsKey(mon))
				s_MonToSpriteData[mon] = new MonSpriteData();

			s_MonToSpriteData[mon].pokedexNumber = pokedexNumber;
			s_MonToSpriteData[mon].spriteUri[key] = uri;
			Console.WriteLine($"\tDiscovered '{mon}' '{key}'");
		}

		private static void AppendSpriteResults(string pageData, string key)
		{
			using(StringReader reader = new StringReader(pageData))
			{
				string line;
				while((line = reader.ReadLine()) != null)
				{
					if(line.StartsWith("<img alt=\"#"))
					{
						//<img alt="#001 Bulbasaur" src="../sprites/overworlds/o_hgss/o_hs_001_1.png" />

						int endIdx = line.IndexOf("/>");
						line = line.Substring(0, endIdx + 2);

						var node = XDocument.Parse(line).Root;
						string rawName = node.Attribute("alt").Value;
						string rawPath = node.Attribute("src").Value;
						string rawNumber = rawName.Substring(1, rawName.IndexOf(' ') - 1).Trim();

						string formattedName = rawName.Substring(rawName.IndexOf(' ')).Trim().ToLower();
						string formattedPath = c_BaseURI + rawPath.Substring(2);
						int pokedexNumber = int.Parse(rawNumber);

						formattedName = formattedName
							.Replace(".", "")
							.Replace("’", "")
							.Replace(" ", "_")
							.Replace("-", "_")
							.Replace("♂", "_m")
							.Replace("♀", "_f")
							.Replace("?", "qmark")
							.Replace("!", "emark");

						if (formattedPath.EndsWith("1.png"))
							AppendMonSpriteUri(formattedName, pokedexNumber, key + "_1", formattedPath);
						else if (formattedPath.EndsWith("2.png"))
							AppendMonSpriteUri(formattedName, pokedexNumber, key + "_2", formattedPath);
						else
							throw new Exception("Unknown suffix");

						continue;
					}
				}
			}


			return;
		}

		private static Bitmap CollateSpriteSheet(string mon, string variant)
		{
			Console.WriteLine($"\tCollating '{mon}{variant}'");

			Bitmap DownloadBitmap(string key)
			{
				string url = s_MonToSpriteData[mon].spriteUri[key];
				return ContentCache.GetImageContent(url);
			}

			void BlitBitmap(Bitmap dst, Bitmap src, int offsetX, int offsetY)
			{
				for(int x = 0; x < src.Width; ++x)
					for(int y = 0; y < src.Height; ++y)
						dst.SetPixel(offsetX + x, offsetY + y, src.GetPixel(x, y));
			}

			Bitmap front1 = DownloadBitmap($"front{variant}_1");
			Bitmap front2 = DownloadBitmap($"front{variant}_2");
			Bitmap back1 = DownloadBitmap($"back{variant}_1");
			Bitmap back2 = DownloadBitmap($"back{variant}_2");
			Bitmap side1 = DownloadBitmap($"side{variant}_1");
			Bitmap side2 = DownloadBitmap($"side{variant}_2");

			Bitmap result = new Bitmap(front1.Width * 6, front1.Height);
			int i = 0;

			BlitBitmap(result, front1, front1.Width * i++, 0);
			BlitBitmap(result, back1, front1.Width * i++, 0);
			BlitBitmap(result, side1, front1.Width * i++, 0);
			BlitBitmap(result, front2, front1.Width * i++, 0);
			BlitBitmap(result, back2, front1.Width * i++, 0);
			BlitBitmap(result, side2, front1.Width * i++, 0);

			//BlitBitmap(result, front1, front1.Width * i++, 0);
			//BlitBitmap(result, front2, front1.Width * i++, 0);
			//
			//BlitBitmap(result, back1, front1.Width * i++, 0);
			//BlitBitmap(result, back2, front1.Width * i++, 0);
			//
			//BlitBitmap(result, side1, front1.Width * i++, 0);
			//BlitBitmap(result, side2, front1.Width * i++, 0);
			return result;
		}

		private static int SelectBestPaletteOption(Tuple<string, ImagePalette>[] palettes, Bitmap source)
		{
			int palIdx = 0;
			double palScore = double.MaxValue;

			for (int p = 0; p < palettes.Length; ++p)
			{
				var pal = palettes[p];

				double score = pal.Item2.GetBitmapMatchScore(source);
				if (score < palScore)
				{
					palIdx = p;
					palScore = score;
				}
			}

			return palIdx;
		}
	}
}
