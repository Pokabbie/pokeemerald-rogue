using Newtonsoft.Json;
using PokemonDataGenerator.OverworldSprites;
using PokemonDataGenerator.OverworldSprites.NPC;
using PokemonDataGenerator.Utils;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
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

		public class MonSpriteData
		{
			public Dictionary<string, string> spriteUri = new Dictionary<string, string>();
			public int pokedexNumber;
			public Bitmap collatedSpriteSheet;
			public Bitmap collatedShinySpriteSheet;
			public ImagePalette normalPalette;
			public ImagePalette shinyPalette;

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

		private static Dictionary<string, MonSpriteData> s_MonToSpriteData = new Dictionary<string, MonSpriteData>();

		public static bool IsValidSpriteKey(string key)
		{
			return c_Subpaths.Where((p) => p.key + "_1" == key || p.key + "_2" == key).Any();
		}

		public static MonSpriteData GetGatheredSpriteDataFor(string species)
		{
			return s_MonToSpriteData[species];
		}

		public static void GenerateFromURL()
		{
			SpriteSheetSplitter.AppendGenericMonSprites32("none", 0, "", "res://Overworld/Custom/none_overworld.png");

			if(s_GenerateShinies)
				SpriteSheetSplitter.AppendGenericMonSprites32("none", 0, "_shiny", "res://Overworld/Custom/none_overworld_shiny.png");

			if (!s_TargettingVanilla)
			{
				SpriteSheetSplitter.AppendGenericMonSprites32("pikachu_original_cap", 0, "", "res://Overworld/Custom/pikachu_cap_original.png");
				SpriteSheetSplitter.AppendGenericMonSprites32("pikachu_partner_cap", 0, "", "res://Overworld/Custom/pikachu_cap_red.png");
			}

			// Gather all of gens 3/4
			foreach (var subpath in c_Subpaths)
			{
				string fullUrl = c_BaseURL + "/" + subpath.path;
				Console.WriteLine($"Gathering '{fullUrl}'");

				string content = ContentCache.GetHttpContent(fullUrl);
				AppendSpriteResults(content, subpath.key);
			}


			if (!s_TargettingVanilla)
			{
				if (s_TargettingDebugSet)
				{
					SpriteSheetSplitter_Gen9.AppendMonSprites();
				}
				else
				{
					SpriteSheetSplitter_Gen5.AppendMonSprites();
					SpriteSheetSplitter_Gen6.AppendMonSprites();
					SpriteSheetSplitter_Gen7.AppendMonSprites();
					SpriteSheetSplitter_Gen8.AppendMonSprites();
					SpriteSheetSplitter_Hisui.AppendMonSprites();
					SpriteSheetSplitter_Gen9.AppendMonSprites();
				}
			}

			// Auto generate any missing shinies
			if (s_GenerateShinies)
			{
				Console.WriteLine($"Begining auto shiny pass..");

				foreach (var kvp in s_MonToSpriteData.ToArray())
				{
					bool hasShinies = kvp.Value.spriteUri.Keys.Where(k => k.Contains("_shiny")).Any();

					if (!hasShinies)
					{
						Console.WriteLine($"\t{kvp.Key}");
						ShinySpriteGenerator.GenerateMonSprites(kvp.Key);
					}

				}

				Console.WriteLine($"Finished auto shiny pass.");
			}

			string outDir = Path.GetFullPath("sprite_out");
			string finalOutputDir = Path.Combine(GameDataHelpers.RootDirectory, "graphics\\object_events\\pics\\pokemon_ow");

			if (s_TargettingDebugSet)
			{
				finalOutputDir = Path.Combine(outDir, "raw");
			}

			try
			{
				if (Directory.Exists(finalOutputDir))
					Directory.Delete(finalOutputDir, true);

				Directory.CreateDirectory(finalOutputDir);
			}
			catch (Exception e)
			{
				Console.WriteLine("Error caught when deleting dir (ignoring for now)");
				Console.WriteLine(e.Message);
			}

			CollateSpriteSheets(finalOutputDir);
			ExportGameCode(finalOutputDir);
		}

		private static void CollateSpriteSheets(string outDir)
		{
			Directory.CreateDirectory(outDir);

			foreach (var kvp in s_MonToSpriteData)
			{
				try
				{
					CollateSpriteSheet(outDir, kvp.Key, kvp.Value);
				}
				catch(InvalidDataException)
				{
					// try once more
					CollateSpriteSheet(outDir, kvp.Key, kvp.Value);
				}
			}
		}

		private static void CollateSpriteSheet(string outDir, string mon, MonSpriteData data)
		{
			data.collatedSpriteSheet = CollateSpriteSheet(mon, "");
			//ConvertToIndexedImage(data.collatedSpriteSheet, out data.collatedSpriteSheet, out data.normalPalette);

			data.spriteSize = data.collatedSpriteSheet.Height;
			data.paletteIdx = -1;

			Console.WriteLine($"\tCollated {mon} sprites");

			if (s_GenerateShinies)
			{
				data.collatedShinySpriteSheet = CollateSpriteSheet(mon, "_shiny");
				//ConvertToIndexedImage(data.collatedShinySpriteSheet, out data.collatedShinySpriteSheet, out data.shinyPalette);

				data.shinySpriteSize = data.collatedShinySpriteSheet.Height;
				data.shinyPaletteIdx = -1;

				// Verify that we have the same silhouette
				for (int y = 0; y < data.collatedSpriteSheet.Height; ++y)
				{
					for (int x = 0; x < data.collatedSpriteSheet.Width; ++x)
					{
						Color normalColour = data.collatedSpriteSheet.GetPixel(x, y);
						Color shinyColour = data.collatedShinySpriteSheet.GetPixel(x, y);

						if (normalColour.A != shinyColour.A)
						{
							// Force regen the shiny sprites then throw so we try again
							ShinySpriteGenerator.ForcefullyRegenerateMonSprites(mon);
							throw new InvalidDataException("Silhouette missmatch between normal and shiny sprite");
						}
					}
				}
			}

			// Debug output
			if (s_TargettingDebugSet)
			{
				data.collatedSpriteSheet.Save(Path.Combine(outDir, mon + "_raw.png"));

				if (s_GenerateShinies)
					data.collatedShinySpriteSheet.Save(Path.Combine(outDir, mon + "_shiny_raw.png"));
			}

			GenerateIndexedSpritesheets(ref data.collatedSpriteSheet, ref data.collatedShinySpriteSheet, out data.normalPalette, out data.shinyPalette);

			data.collatedSpriteSheet.Save(Path.Combine(outDir, mon + ".png"));
			data.normalPalette.Save(Path.Combine(outDir, mon + ".pal"));
			if (s_GenerateShinies)
				data.shinyPalette.Save(Path.Combine(outDir, mon + "_shiny.pal"));
		}

		private struct ColourPoint
		{
			public int index;
			public Color normalColour;
			public Color shinyColour;

			public ColourPoint(int index, Color normal, Color shiny)
			{
				this.index = index;
				this.normalColour = normal;
				this.shinyColour = shiny;
			}
		};

		private static void GenerateIndexedSpritesheets(ref Bitmap normalSheet, ref Bitmap shinySheet, out ImagePalette normalPalette, out ImagePalette shinyPalette)
		{

			// We need to index both images at the same time as we may be reusing colours in one image but not in the other
			// So what we're going to do is make a grey scale image for new unique colour combinations

			// Reduce to 16 colours
			normalSheet = ImagePalette.CreateFromContent(normalSheet, 16, ImagePalette.DistanceMethod.HSL, NpcSpriteSplitter.c_BackgroundColour).CreateIndexedBitmap(normalSheet);
			shinySheet = ImagePalette.CreateFromContent(shinySheet, 16, ImagePalette.DistanceMethod.HSL, NpcSpriteSplitter.c_BackgroundColour).CreateIndexedBitmap(shinySheet);

			List<ColourPoint> colourPoints = new List<ColourPoint>();
			Bitmap greyScaleImage = new Bitmap(normalSheet.Width, normalSheet.Height);

			// Place background in first slot
			colourPoints.Add(new ColourPoint(0, NpcSpriteSplitter.c_BackgroundColour, NpcSpriteSplitter.c_BackgroundColour));

			for (int y = 0; y < normalSheet.Height; ++y)
			{
				for (int x = 0; x < normalSheet.Width; ++x)
				{
					Color normalColour = normalSheet.GetPixel(x, y);
					Color shinyColour = shinySheet == null ? normalColour : shinySheet.GetPixel(x, y);
					ColourPoint point = new ColourPoint(0, normalColour, shinyColour);

					if(normalColour.A != shinyColour.A)
						throw new InvalidDataException("Silhouette missmatch between normal and shiny sprite");

					point.index = colourPoints.FindIndex(p => p.normalColour.ToArgb() == point.normalColour.ToArgb() && p.shinyColour.ToArgb() == point.shinyColour.ToArgb());
					if (point.index == -1)
					{
						point.index = colourPoints.Count;
						colourPoints.Add(point);
					}

					greyScaleImage.SetPixel(x, y, Color.FromArgb(255, point.index * 1, 0, 0));
				}
			}

			while (colourPoints.Count > 16)
			{
				RemoveSingleColour(greyScaleImage, colourPoints);
			}

			List<Color> normalColours = new List<Color>();
			List<Color> shinyColours = new List<Color>();
			List<Color> greyScaleColours = new List<Color>();

			foreach (var point in colourPoints)
			{
				normalColours.Add(point.normalColour);
				shinyColours.Add(point.shinyColour);
				greyScaleColours.Add(Color.FromArgb(255, point.index * 1, 0, 0));
			}

			normalColours[0] = NpcSpriteSplitter.c_PrettyBackgroundColour;
			shinyColours[0] = NpcSpriteSplitter.c_PrettyBackgroundColour;

			normalPalette = new ImagePalette(normalColours.ToArray());
			shinyPalette = new ImagePalette(shinyColours.ToArray());

			ImagePalette greyScalePalette = new ImagePalette(greyScaleColours.ToArray());
			normalSheet = greyScalePalette.CreateIndexedBitmap(greyScaleImage, 0);

			// Repoint the greyscale palette to make debugging easier
			ColorPalette pal = normalSheet.Palette;
			for (int i = 0; i < 16; ++i)
				pal.Entries[i] = i < normalColours.Count ? normalColours[i] : Color.FromArgb(255, 0, 0, 0);
			normalSheet.Palette = pal;
		}

		private static Color MixColours(Color a, Color b)
		{
			return Color.FromArgb(
				255,
				((int)a.R + (int)b.R) / 2,
				((int)a.G + (int)b.G) / 2,
				((int)a.B + (int)b.B) / 2
			);
		}

		private static void RemoveSingleColour(Bitmap greyScaleSheet, List<ColourPoint> colourPoints)
		{
			// Find the 2 closest colour points
			int bestIndexA = -1;
			int bestIndexB = -1;
			double bestDistance = double.MaxValue;

			for (int i = 1; i < colourPoints.Count; ++i)
			{
				for (int j = 1; j < colourPoints.Count; ++j)
				{
					if(i != j)
					{
						double normalDistance = ImagePalette.GetColorDistance(colourPoints[i].normalColour, colourPoints[j].normalColour, ImagePalette.DistanceMethod.YUV);
						double shinyDistance = ImagePalette.GetColorDistance(colourPoints[i].shinyColour, colourPoints[j].shinyColour, ImagePalette.DistanceMethod.YUV);
						double avgDistance = (normalDistance + shinyDistance) / 2.0;

						if(avgDistance < bestDistance)
						{
							bestDistance = avgDistance;
							bestIndexA = i;
							bestIndexB = j;
						}
					}
				}
			}

			ColourPoint replaceSource = colourPoints[bestIndexA];
			ColourPoint replaceTarget = colourPoints[bestIndexB];

			// Average 2 colours together
			replaceSource.normalColour = MixColours(colourPoints[bestIndexA].normalColour, colourPoints[bestIndexB].normalColour);
			replaceSource.shinyColour = MixColours(colourPoints[bestIndexA].shinyColour, colourPoints[bestIndexB].shinyColour);

			colourPoints[bestIndexA] = replaceSource;

			// Replace all references from 2nd point to first newly blended
			for (int y = 0; y < greyScaleSheet.Height; ++y)
			{
				for (int x = 0; x < greyScaleSheet.Width; ++x)
				{
					Color pixel = greyScaleSheet.GetPixel(x, y);
					if (pixel.R == replaceTarget.index)
					{
						greyScaleSheet.SetPixel(x, y, Color.FromArgb(255, replaceSource.index * 1, 0, 0));
					}
				}
			}

			colourPoints.RemoveAt(bestIndexB);
		}

		private static void ExportGameCode(string outDir)
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
					writer.WriteLine($"const u16 gObjectEventPal_Overworld_{mon}[] = INCBIN_U16(\"graphics/object_events/pics/pokemon_ow/{mon}.gbapal\");");

					if (s_GenerateShinies)
						writer.WriteLine($"const u16 gObjectEventPal_Overworld_{mon}_shiny[] = INCBIN_U16(\"graphics/object_events/pics/pokemon_ow/{mon}_shiny.gbapal\");");
				}
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

					writer.WriteLine($"");
				}

				writer.WriteLine($"");

				foreach (var kvp in s_MonToSpriteData.OrderBy((kvp) => kvp.Value.pokedexNumber))
				{
					var mon = kvp.Key;
					var data = kvp.Value;
					int dims = data.spriteSize / 8;

					// todo
					string paletteTag = "OBJ_EVENT_PAL_TAG_FOLLOW_MON_0";
					int paletteNum = 10; // force to 10 to make the palette load semi dynamically

					if (dims == 8) // 64x64
						writer.WriteLine($"const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_Mon_{mon} = {{TAG_NONE, {paletteTag}, OBJ_EVENT_PAL_TAG_NONE, 2048, 64, 64, {paletteNum}, SHADOW_SIZE_M, FALSE, FALSE, TRACKS_FOOT, &gObjectEventBaseOam_64x64, sOamTables_64x64, sAnimTable_GenericOverworldMon, sPicTable_Mon_{mon}, gDummySpriteAffineAnimTable}};");
					else
						writer.WriteLine($"const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_Mon_{mon} = {{TAG_NONE, {paletteTag}, OBJ_EVENT_PAL_TAG_NONE, 512, 32, 32, {paletteNum}, SHADOW_SIZE_M, FALSE, FALSE, TRACKS_FOOT, &gObjectEventBaseOam_32x32, sOamTables_32x32, sAnimTable_GenericOverworldMon, sPicTable_Mon_{mon}, gDummySpriteAffineAnimTable}};");
				}

				writer.WriteLine($"");
				writer.WriteLine($"const struct RogueFollowMonGraphicsInfo gFollowMonGraphicsInfo[NUM_SPECIES] = {{");

				foreach (var kvp in s_MonToSpriteData.OrderBy((kvp) => kvp.Value.pokedexNumber))
				{
					var mon = kvp.Key;
					var data = kvp.Value;
					int dims = data.spriteSize / 8;
					writer.WriteLine($"\t[SPECIES_{mon.ToUpper()}] =");
					writer.WriteLine($"\t{{");
					writer.WriteLine($"\t\t.objectEventGfxInfo = &gObjectEventGraphicsInfo_Mon_{mon},");
					writer.WriteLine($"\t\t.normalPal = gObjectEventPal_Overworld_{mon},");
					if (s_GenerateShinies)
						writer.WriteLine($"\t\t.shinyPal = gObjectEventPal_Overworld_{mon}_shiny,");
					else
						writer.WriteLine($"\t\t.shinyPal = NULL,");
					writer.WriteLine($"\t}},");
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
				int counter = 0;
				int previousNum = -1;

				while ((line = reader.ReadLine()) != null)
				{
					if (line.StartsWith("<img alt=\"#"))
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

						if (s_TargettingDebugSet)
						{
							if (previousNum != pokedexNumber)
							{
								previousNum = pokedexNumber;
								if (counter++ >= 20)
								{
									return;
								}
							}
						}

						if (formattedPath.EndsWith("1.png"))
							AppendMonSpriteUri(formattedName, pokedexNumber, key + "_1", formattedPath);
						else if (formattedPath.EndsWith("2.png"))
							AppendMonSpriteUri(formattedName, pokedexNumber, key + "_2", formattedPath);
						else
							throw new Exception("Unknown suffix");
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
				for (int x = 0; x < src.Width; ++x)
					for (int y = 0; y < src.Height; ++y)
					{
						Color col = src.GetPixel(x, y);

						if (col.A != 255)
							col = NpcSpriteSplitter.c_BackgroundColour;

						dst.SetPixel(offsetX + x, offsetY + y, col);
					}
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
	}
}
