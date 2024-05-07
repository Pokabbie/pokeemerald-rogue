using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator.OverworldSprites.NPC
{
	internal class CodeHintExporter
	{
		public class InternalText
		{
			public StringBuilder Header = new StringBuilder();
			public StringBuilder Footer = new StringBuilder();
			public StringBuilder Body = new StringBuilder();
		}

		private Dictionary<string, InternalText> m_InternalText = new Dictionary<string, InternalText>();

		public InternalText For(string filePath)
		{
			filePath = filePath.ToLower().Replace("/", "\\");
			if (m_InternalText.TryGetValue(filePath, out InternalText text))
				return text;

			m_InternalText[filePath] = new InternalText();
			return m_InternalText[filePath];
		}

		public void WriteTo(string filePath)
		{
			StringBuilder outputText = new StringBuilder();
			foreach(var kvp in m_InternalText)
			{
				outputText.AppendLine("//");
				outputText.AppendLine("// " + kvp.Key);
				outputText.AppendLine("//");
				outputText.AppendLine(kvp.Value.Header.ToString());
				outputText.AppendLine(kvp.Value.Body.ToString());
				outputText.AppendLine(kvp.Value.Footer.ToString());
			}

			File.WriteAllText(filePath, outputText.ToString());
		}
	}

	public static class NpcSpriteSplitter
	{
		private const int c_SourceFrameOffsetX = 0;
		private const int c_SourceFrameOffsetY = 1;
		private const int c_SourceFrameWidth = 16;
		private const int c_SourceFrameHeight = 24;

		private const int c_FrameOffsetX = 0;
		private const int c_FrameOffsetY = 9;
		private const int c_FrameWidth = 16;
		private const int c_FrameHeight = 32;
		private const int c_TotalWidth = c_FrameWidth * 9;
		private const int c_TotalHeight = c_FrameHeight;

		public static readonly Color c_BackgroundColour = Color.FromArgb(0, 0, 0, 0);
		public static readonly Color c_PrettyBackgroundColour = Color.FromArgb(255, 115, 197, 164);

		private struct FrameInfo
		{
			public int x;
			public int y;
			public int width;
			public int height;

			public FrameInfo(int x, int y, int width, int height)
			{
				this.x = x;
				this.y = y;
				this.width = width;
				this.height = height;
			}
		}

		private static Point[] c_FrameLayout = new Point[]
		{
			new Point(0, 0),
			new Point(0, 3),
			new Point(0, 1),

			new Point(3, 0),
			new Point(1, 0),

			new Point(1, 3),
			new Point(3, 3),

			new Point(1, 1),
			new Point(3, 1),
		};

		private static Point[] c_FrameOffset = new Point[]
		{
			new Point(0, 0),
			new Point(0, 0),
			new Point(0, 0),

			new Point(0, 0),
			new Point(0, 0),

			new Point(0, 0),
			new Point(0, 0),

			new Point(0, 0),
			new Point(0, 0),
		};

		public static void ExportDirectory(string inputDirectory, string outputDirectory)
		{
			CodeHintExporter codeExport = new CodeHintExporter();

			if(Directory.Exists(outputDirectory))
				Directory.Delete(outputDirectory, true);

			foreach (var subDirectory in Directory.EnumerateDirectories(inputDirectory))
			{
				string groupName = Path.GetFileName(subDirectory);
				ExportDirectoryInternal(codeExport, groupName, subDirectory, Path.Combine(outputDirectory, groupName.ToLower()));
			}

			codeExport.WriteTo(Path.Combine(outputDirectory, "__code_helper.h"));
		}
		
		private static void ExportDirectoryInternal(CodeHintExporter codeExport, string groupName, string inputDirectory, string outputDirectory)
		{
			Console.WriteLine($"Exporting '{groupName}' directory '{inputDirectory}' -> '{outputDirectory}'");

			Directory.CreateDirectory(outputDirectory);

			foreach (var file in Directory.EnumerateFiles(inputDirectory))
			{
				string fileName = Path.GetFileName(file);

				// Pretty up the file name
				if (fileName.Contains("_highnoonmoon_"))
				{
					string prefix = "trainer_leader_";

					if (fileName.StartsWith("trainer_pokemontrainer_"))
					{
						prefix = "trainer_pokemontrainer_";
					}
					else if (fileName.StartsWith("trainer_champion_"))
					{
						prefix = "trainer_champion_";
					}
					else if (fileName.StartsWith("trainer_elitefour_"))
					{
						prefix = "trainer_elitefour_";
					}

					int index = fileName.IndexOf("_highnoonmoon_");
					fileName = fileName.Substring(0, index).Trim().Substring(prefix.Length) + Path.GetExtension(fileName);
				}
				else if (fileName.Contains("_by_wjj36"))
				{
					if(fileName.StartsWith("npc_leader_"))
					{
						int index = fileName.IndexOf("_by_wjj36");
						fileName = fileName.Substring(0, index).Trim() + Path.GetExtension(fileName);

						index = fileName.LastIndexOf("_");
						fileName = fileName.Substring(index + 1);
					}
					else
					{
						int index = fileName.IndexOf("_ow_");
						fileName = fileName.Substring(0, index).Trim() + Path.GetExtension(fileName);
					}
				}
				else if (fileName.Contains("_by_LightningStrike7"))
				{
					int index = fileName.IndexOf("_by_LightningStrike7");
					fileName = fileName.Substring(0, index).Trim() + Path.GetExtension(fileName);
				}
				else if (fileName.Contains("_by_xdracolich"))
				{
					int index = fileName.IndexOf("_pokemon_");
					fileName = fileName.Substring(0, index).Trim() + Path.GetExtension(fileName);
				}
				else if (fileName.StartsWith("_sprite__"))
				{
					fileName = fileName.Substring("_sprite__".Length);

					int index = fileName.IndexOf("_custom_overworld__");
					fileName = fileName.Substring(0, index).Trim() + Path.GetExtension(fileName);
				}
				else if (fileName.Contains("_gen_3__by_"))
				{
					int index = fileName.IndexOf("_gen_3__by_");
					fileName = fileName.Substring(0, index - 1).Trim() + Path.GetExtension(fileName);
				}
				else if (fileName.Contains("_by_"))
				{
					int index = fileName.IndexOf("_by_");
					fileName = fileName.Substring(0, index).Trim() + Path.GetExtension(fileName);
				}
				else
				{
					throw new InvalidDataException("Can't deduce NPC name from obfusticated file name");
				}

				ExportSpritesheet(file, Path.Combine(outputDirectory, fileName));


				// File Helpers
				string instanceName = Path.GetFileNameWithoutExtension(fileName).Replace(" ", "_");
				string npcName = FormatNpcName(groupName + "_" + instanceName);

				codeExport.For("sprite_rules.mk").Body.AppendLine($"$(OBJEVENTGFXDIR)/rogue/npc/{groupName.ToLower()}/{instanceName}.4bpp: %.4bpp: %.png");
				codeExport.For("sprite_rules.mk").Body.AppendLine($"	$(GFX) $< $@ -mwidth 2 -mheight 4");
				codeExport.For("sprite_rules.mk").Body.AppendLine($"");

				codeExport.For("event_objects.h").Header.AppendLine($"#define OBJ_EVENT_GFX_{npcName.ToUpper()}       __ABCDEF__");

				codeExport.For("object_event_graphics_info_pointers.h").Header.AppendLine($"const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_{npcName};");
				codeExport.For("object_event_graphics_info_pointers.h").Body.AppendLine($"[OBJ_EVENT_GFX_{npcName.ToUpper()}] = &gObjectEventGraphicsInfo_{npcName},");

				codeExport.For("object_event_graphics_info.h").Body.AppendLine($"const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_{npcName} = {{TAG_NONE, OBJ_EVENT_PAL_TAG_{npcName.ToUpper()}, OBJ_EVENT_PAL_TAG_NONE, 256, 16, 32, 10, SHADOW_SIZE_M, FALSE, FALSE, TRACKS_FOOT, &gObjectEventBaseOam_16x32, sOamTables_16x32, sAnimTable_Standard, sPicTable_{npcName}, gDummySpriteAffineAnimTable}};");

				codeExport.For("event_object_movement.c").Header.AppendLine($"#define OBJ_EVENT_PAL_TAG_{npcName.ToUpper()}      __ABCDEF__");
				codeExport.For("event_object_movement.c").Body.AppendLine($"{{gObjectEventPal_{npcName}, OBJ_EVENT_PAL_TAG_{npcName.ToUpper()}}},");

				codeExport.For("object_event_graphics.h").Header.AppendLine($"const u16 gObjectEventPal_{npcName}[] = INCBIN_U16(\"graphics/object_events/pics/rogue/npc/{groupName.ToLower()}/{instanceName}.gbapal\");");
				codeExport.For("object_event_graphics.h").Body.AppendLine($"const u32 gObjectEventPic_{npcName}[] = INCBIN_U32(\"graphics/object_events/pics/rogue/npc/{groupName.ToLower()}/{instanceName}.4bpp\");");

				codeExport.For("object_event_pic_tables.h").Body.AppendLine($"static const struct SpriteFrameImage sPicTable_{npcName}[] = {{");
				codeExport.For("object_event_pic_tables.h").Body.AppendLine($"    overworld_frame(gObjectEventPic_{npcName}, 2, 4, 0),");
				codeExport.For("object_event_pic_tables.h").Body.AppendLine($"    overworld_frame(gObjectEventPic_{npcName}, 2, 4, 1),");
				codeExport.For("object_event_pic_tables.h").Body.AppendLine($"    overworld_frame(gObjectEventPic_{npcName}, 2, 4, 2),");
				codeExport.For("object_event_pic_tables.h").Body.AppendLine($"    overworld_frame(gObjectEventPic_{npcName}, 2, 4, 3),");
				codeExport.For("object_event_pic_tables.h").Body.AppendLine($"    overworld_frame(gObjectEventPic_{npcName}, 2, 4, 4),");
				codeExport.For("object_event_pic_tables.h").Body.AppendLine($"    overworld_frame(gObjectEventPic_{npcName}, 2, 4, 5),");
				codeExport.For("object_event_pic_tables.h").Body.AppendLine($"    overworld_frame(gObjectEventPic_{npcName}, 2, 4, 6),");
				codeExport.For("object_event_pic_tables.h").Body.AppendLine($"    overworld_frame(gObjectEventPic_{npcName}, 2, 4, 7),");
				codeExport.For("object_event_pic_tables.h").Body.AppendLine($"    overworld_frame(gObjectEventPic_{npcName}, 2, 4, 8),");
				codeExport.For("object_event_pic_tables.h").Body.AppendLine($"}};");

				//codeExport.For("front_pic_anims.h").Body.AppendLine($"[TRAINER_PIC_{npcName.ToUpper()}] = sAnims_Kanto,");
				//
				//codeExport.For("front_pic_tables.h").Body.AppendLine($"[TRAINER_PIC_{npcName.ToUpper()}] = {{.size = 8, .y_offset = 1}},");
			}
		}

		private static string FormatNpcName(string name)
		{
			bool nextIsUpper = true;
			string output = "";

			for(int i = 0; i < name.Length; ++i)
			{
				if (nextIsUpper)
				{
					output += char.ToUpper(name[i]);
					nextIsUpper = false;
				}
				else
					output += char.ToLower(name[i]);

				if (name[i] == '_')
					nextIsUpper = true;
			}

			return output;
		}

		public static void ExportSpritesheet(string inputPath, string indexedOutPath)
		{
			Console.WriteLine($"\tExporting file '{inputPath}' -> '{indexedOutPath}'");

			Bitmap sourceImg = new Bitmap(inputPath);
			Bitmap outputImg = new Bitmap(c_TotalWidth, c_TotalHeight);//,System.Drawing.Imaging.PixelFormat.Format4bppIndexed);
			
			// Fill background
			for(int y = 0; y < outputImg.Height; ++y)
				for(int x = 0; x < outputImg.Width; ++x)
				{
					outputImg.SetPixel(x, y, c_PrettyBackgroundColour);
				}

			// Fill frames
			for(int i = 0; i < c_FrameLayout.Length; ++i)
			{
				outputImg.SafeSplatFrom(sourceImg,
					// Dest
					c_FrameOffset[i].X + c_FrameOffsetX + i * c_FrameWidth,
					c_FrameOffset[i].Y + c_FrameOffsetY,
					// Source (Always at 2x res)
					c_SourceFrameOffsetX + c_FrameLayout[i].X * c_SourceFrameWidth,
					c_SourceFrameOffsetY + c_FrameLayout[i].Y * c_SourceFrameHeight,
					c_SourceFrameWidth,
					c_SourceFrameHeight
				);
			}


			ImagePalette palette = ImagePalette.CreateFromContent(outputImg, 16, ImagePalette.DistanceMethod.HSL, c_PrettyBackgroundColour);
			Bitmap indexedOutputImg = palette.CreateIndexedBitmap(outputImg, 0);

			indexedOutputImg.Save(indexedOutPath);
		}

		private static Color SafeReadFromSource(this Bitmap img, int x, int y)
		{
			Color outColour = Color.Transparent;

			if (img.Width == 64 && img.Height == 96)
			{
				// 1 to 1 pixel matching
			}
			else
			{
				// Assume double size
				x *= 2;
				y *= 2;
			}

			if (x < img.Width && y < img.Height)
				outColour = img.GetPixel(x, y);

			return outColour.A == 0 ? c_PrettyBackgroundColour : outColour;
		}

		private static void SafeSplatFrom(this Bitmap dest, Bitmap source, int writeX, int writeY, int x, int y, int width, int height)
		{
			for (int dy = 0; dy < height; ++dy)
				for (int dx = 0; dx < width; ++dx)
				{
					Color srcColor = source.SafeReadFromSource(x + dx, y + dy);

					if(writeX + dx < dest.Width && writeY + dy < dest.Height)
						dest.SetPixel(writeX + dx, writeY + dy, srcColor);
				}
		}
	}
}
