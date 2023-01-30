using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator.OverworldSprites
{
	public static class SpriteSheetSplitter
	{
		public struct Settings
		{
			public string CategoryName;
			public Bitmap Source;
			public int CellSize;
			public string[] FrameNames;
			public int FrameStride;
		}

		public static void AppendMonSprites(string mon, int dexNumber, Settings settings, int originX, int originY)
		{
			int x = 0;
			int y = 0;

			Console.WriteLine($"\tSplitting {settings.CategoryName} {mon}..");

			foreach (var frameKey in settings.FrameNames)
			{
				if(OverworldSpriteGenerator.IsValidSpriteKey(frameKey))
				{
					string localFilePath = ContentCache.GetWriteableCachePath($"sprite_splitting\\{settings.CategoryName}\\{frameKey}\\{dexNumber.ToString("D4")}_{mon}.png");

					if (File.Exists(localFilePath))
					{
						Console.WriteLine($"\t\tAlready exists (skipping creation)");
					}
					else
					{
						int tl_x = originX + x * settings.CellSize;
						int tl_y = originY + y * settings.CellSize;

						Bitmap spriteFrame = settings.Source.Clone(new Rectangle(tl_x, tl_y, settings.CellSize, settings.CellSize), settings.Source.PixelFormat);
						spriteFrame.Save(localFilePath);
					}

					OverworldSpriteGenerator.AppendMonSpriteUri(mon, dexNumber, frameKey, localFilePath);
				}

				if(++x >= settings.FrameStride)
				{
					x = 0;
					y++;
				}
			}
		}

		public static void AppendMonBlockSprites(string[] mons, ref int dexNumber, Settings settings, ref int originX, ref int originY)
		{
			foreach(var mon in mons)
			{
				if (mon != "_") // _ reserved for "skip"
				{
					AppendMonSprites(mon, dexNumber, settings, originX, originY);
					++dexNumber;
				}

				originX += settings.FrameStride * settings.CellSize;

				if(originX >= settings.Source.Width)
				{
					originX = 0;
					originY += settings.CellSize * (settings.FrameNames.Length / settings.FrameStride);
				}
			}
		}
	}
}
