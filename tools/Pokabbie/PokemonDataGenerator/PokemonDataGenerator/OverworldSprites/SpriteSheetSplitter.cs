using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
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
			public bool Gen5BorderRemoval;
			public int Gen5CorrectionMode;
			public int CustomStepX;
			public int CustomStepY;
			public Tuple<string, string>[] AutomaticCentredFrames;
		}

		public static void AppendMonSprites(string mon, int dexNumber, Settings settings, int originX, int originY)
		{
			int x = 0;
			int y = 0;

			Console.WriteLine($"\tSplitting {settings.CategoryName} {mon}..");

			Dictionary<string, Bitmap> gatheredSprites = new Dictionary<string, Bitmap>();

			foreach (var frameKey in settings.FrameNames)
			{
				if(OverworldSpriteGenerator.IsValidSpriteKey(frameKey))
				{
					string localFilePath = ContentCache.GetWriteableCachePath($"sprite_splitting\\{settings.CategoryName}\\{frameKey}\\{dexNumber.ToString("D4")}_{mon}.png");

					if (!OverworldSpriteGenerator.s_TargettingDebugSet && File.Exists(localFilePath))
					{
						Console.WriteLine($"\t\tAlready exists (skipping creation)");
					}
					else
					{
						int tl_x = originX + x * settings.CellSize;
						int tl_y = originY + y * settings.CellSize;

						Bitmap spriteFrame = settings.Source.Clone(new Rectangle(tl_x, tl_y, settings.CellSize, settings.CellSize), PixelFormat.Format32bppArgb);
						ApplyTransparencyToAlphaChannel(spriteFrame);

						gatheredSprites.Add(frameKey, spriteFrame);
						//spriteFrame.Save(localFilePath);
					}

					OverworldSpriteGenerator.AppendMonSpriteUri(mon, dexNumber, frameKey, localFilePath);
				}

				if(++x >= settings.FrameStride)
				{
					x = 0;
					y++;
				}
			}

			if (gatheredSprites.Count != 0)
			{
				if (settings.AutomaticCentredFrames != null)
				{
					foreach (var pair in settings.AutomaticCentredFrames)
						RecentreSpritesInGroup(gatheredSprites, pair.Item1, pair.Item2);
				}

				// Now finally save the sprites to disk
				foreach (var pair in gatheredSprites)
				{
					string localFilePath = ContentCache.GetWriteableCachePath($"sprite_splitting\\{settings.CategoryName}\\{pair.Key}\\{dexNumber.ToString("D4")}_{mon}.png");
					pair.Value.Save(localFilePath);
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

				if (settings.CustomStepX != 0 || settings.CustomStepY != 0)
				{
					originX += settings.CustomStepX;

					if (originX >= settings.Source.Width)
					{
						originX -= settings.Source.Width;
						originY += settings.CustomStepY;
					}
				}
				else
				{
					originX += settings.FrameStride * settings.CellSize;

					if (originX >= settings.Source.Width)
					{
						originX = 0;
						originY += settings.CellSize * (settings.FrameNames.Length / settings.FrameStride);
					}
				}
			}
		}


		public static void AppendGenericMonSprites32(string mon, int dexNumber, string groupKey, string sourcePath)
		{
			Settings settings = new Settings();
			settings.CategoryName = "generic32" + groupKey;
			settings.CellSize = 32;
			settings.FrameNames = new string[]
			{
				$"front{groupKey}_1", $"back{groupKey}_1", $"side{groupKey}_1", $"front{groupKey}_2", $"back{groupKey}_2", $"side{groupKey}_2",
			};
			settings.FrameStride = 6;
			settings.AutomaticCentredFrames = new Tuple<string, string>[]
			{
				new Tuple<string, string>($"front{groupKey}_1", $"front{groupKey}_2"),
				new Tuple<string, string>($"back{groupKey}_1", $"back{groupKey}_2"),
				new Tuple<string, string>($"side{groupKey}_1", $"side{groupKey}_2"),
			};
			settings.Source = ContentCache.GetImageContent(sourcePath);

			AppendMonSprites(mon, dexNumber, settings, 0, 0);
		}


		// Needed for the very irregular sprite placement of the gen5 sprite sheet
		public static void AppendNonUniformMonSprites(string mon, int dexNumber, Settings settings, Rectangle sourceRect)
		{
			int x = 0;
			int y = 0;

			Console.WriteLine($"\tSplitting {settings.CategoryName} {mon}..");

			Bitmap sourceGroup = settings.Source.Clone(sourceRect, PixelFormat.Format32bppArgb);

			if (settings.Gen5BorderRemoval)
			{
				Color backgroundColour = sourceGroup.GetPixel(10, 2);
				Color boarderColour = sourceGroup.GetPixel(10, 0);

				// Special cases mons due to bad source image formatting
				switch(mon)
				{
					// Fix not uniform colours
					case "unfezant":
						// Do boarder first
						TryPropagateUntilHitColor(sourceGroup, 0, 0, boarderColour);
						TryPropagateUntilHitColor(sourceGroup, sourceGroup.Width - 1, 0, boarderColour);
						TryPropagateUntilHitColor(sourceGroup, 0, sourceGroup.Height - 1, boarderColour);
						TryPropagateUntilHitColor(sourceGroup, sourceGroup.Width - 1, sourceGroup.Height - 1, boarderColour);

						// Boarder is the new background
						// Fill out inside areas
						TryPropagateFillColor(sourceGroup, 6, 1, sourceGroup.GetPixel(6, 1), boarderColour);
						TryPropagateFillColor(sourceGroup, 6, 5, sourceGroup.GetPixel(6, 5), boarderColour);
						break;

					case "meloetta":
						if (settings.Gen5CorrectionMode == 1) // normal set offsets
						{
							// Do boarder first
							TryPropagateUntilHitColor(sourceGroup, 0, 0, boarderColour);
							TryPropagateUntilHitColor(sourceGroup, sourceGroup.Width - 1, 0, boarderColour);
							TryPropagateUntilHitColor(sourceGroup, 0, sourceGroup.Height - 1, boarderColour);
							TryPropagateUntilHitColor(sourceGroup, sourceGroup.Width - 1, sourceGroup.Height - 1, boarderColour);

							// Boarder is the new background
							// Fill out inside areas
							TryPropagateFillColor(sourceGroup, 6, 1, sourceGroup.GetPixel(6, 1), boarderColour);
							TryPropagateFillColor(sourceGroup, 10, 9, sourceGroup.GetPixel(10, 9), boarderColour);
							TryPropagateFillColor(sourceGroup, 25, 37, sourceGroup.GetPixel(25, 37), boarderColour);
						}
						else
						{
							TryPropagateUntilHitColor(sourceGroup, 0, 0, boarderColour);
							TryPropagateUntilHitColor(sourceGroup, sourceGroup.Width - 1, 0, boarderColour);
							TryPropagateUntilHitColor(sourceGroup, 0, sourceGroup.Height - 1, boarderColour);
							TryPropagateUntilHitColor(sourceGroup, sourceGroup.Width - 1, sourceGroup.Height - 1, boarderColour);

							TryPropagateFillColor(sourceGroup, 0, 0, boarderColour, backgroundColour);
						}
						break;

					case "escavalier":
						TryPropagateUntilHitColor(sourceGroup, 0, 0, boarderColour);
						TryPropagateUntilHitColor(sourceGroup, sourceGroup.Width - 1, 0, boarderColour);
						TryPropagateUntilHitColor(sourceGroup, 0, sourceGroup.Height - 1, boarderColour);
						TryPropagateUntilHitColor(sourceGroup, sourceGroup.Width - 1, sourceGroup.Height - 1, boarderColour);

						TryPropagateFillColor(sourceGroup, 0, 0, boarderColour, backgroundColour);
						break;

					default:
						// Now background
						TryPropagateUntilHitColor(sourceGroup, 0, 0, backgroundColour);
						TryPropagateUntilHitColor(sourceGroup, sourceGroup.Width - 1, 0, backgroundColour);
						TryPropagateUntilHitColor(sourceGroup, 0, sourceGroup.Height - 1, backgroundColour);
						TryPropagateUntilHitColor(sourceGroup, sourceGroup.Width - 1, sourceGroup.Height - 1, backgroundColour);
						break;
				}
			}

			int spacing, initialOffset;
			bool isVertical;

			if(sourceRect.Height > sourceRect.Width)
			{
				spacing = sourceRect.Height / (settings.FrameNames.Length / settings.FrameStride);
				initialOffset = spacing / 2;
				isVertical = true;
			}
			else
			{
				spacing = sourceRect.Width / settings.FrameStride;
				initialOffset = spacing / 2;
				isVertical = false;
			}


			if (settings.Gen5CorrectionMode == 1) // normal set offsets
			{
				// Special cases where the sprites are too missmatched so have to manually provide spacing & offset
				switch (mon)
				{
					case "unfezant":
					case "throh":
					case "whimsicott":
						initialOffset = 32;
						spacing = 25;
						break;

					case "archen":
					case "gothita":
					case "emolga":
					case "lampent":
					case "durant":
						initialOffset = 25;
						spacing = 25;
						break;

					case "mandibuzz":
						initialOffset = 20;
						spacing = 26;
						break;

					case "heatmor":
						initialOffset = 20;
						spacing = 30;
						break;

						//case "basculin_blue_striped":
						//	initialOffset = 16;
						//	spacing = 26;
						//	break;
				}
			}
			else if (settings.Gen5CorrectionMode == 2) // shiny set offsets
			{
				// Special cases where the sprites are too missmatched so have to manually provide spacing & offset
				switch (mon)
				{
					case "tympole":
						initialOffset = 23;
						spacing = 30;
						break;

					case "cottonee":
					case "darumaka":
					case "trubbish":
					case "emolga":
					case "joltik":
					case "shelmet":
						initialOffset = 25;
						spacing = 32;
						break;

				}
			}

			// Write to debug path for inspection
			if (OverworldSpriteGenerator.s_TargettingDebugSet)
			{
				string localDebugPath = ContentCache.GetWriteableCachePath($"sprite_splitting\\DEBUG\\{settings.CategoryName}\\{dexNumber.ToString("D4")}_{mon}.png");
				sourceGroup.Save(localDebugPath);
			}

			bool hasAny32Frames = false;
			bool hasAny64Frames = false;

			Dictionary<string, Bitmap> gatheredSprites = new Dictionary<string, Bitmap>();

			foreach (var frameKey in settings.FrameNames)
			{
				if (OverworldSpriteGenerator.IsValidSpriteKey(frameKey))
				{
					string localFilePath = ContentCache.GetWriteableCachePath($"sprite_splitting\\{settings.CategoryName}\\{frameKey}\\{dexNumber.ToString("D4")}_{mon}.png");
			
					if (!OverworldSpriteGenerator.s_TargettingDebugSet && File.Exists(localFilePath))
					{
						Console.WriteLine($"\t\tAlready exists (skipping creation)");
					}
					else
					{
						Bitmap spriteFrame;

						if (isVertical)
						{
							if (x == 0)
								spriteFrame = ExtractNonUniformSprite(sourceGroup, 0, initialOffset + spacing * y, 1, 0);
							else
								spriteFrame = ExtractNonUniformSprite(sourceGroup, sourceGroup.Width - 1, initialOffset + spacing * y, -1, 0);
						}
						else
						{
							if (y == 0)
								spriteFrame = ExtractNonUniformSprite(sourceGroup, initialOffset + spacing * x, 0, 0, 1);
							else
								spriteFrame = ExtractNonUniformSprite(sourceGroup, initialOffset + spacing * x, sourceGroup.Height - 1, 0, -1);
						}

						if (spriteFrame.Width == 32)
							hasAny32Frames = true;
						else
							hasAny64Frames = true;

						ApplyTransparencyToAlphaChannelNonUniform(spriteFrame);

						gatheredSprites.Add(frameKey, spriteFrame);
						//spriteFrame.Save(localFilePath);
					}

					OverworldSpriteGenerator.AppendMonSpriteUri(mon, dexNumber, frameKey, localFilePath);
				}
			
				if (++x >= settings.FrameStride)
				{
					x = 0;
					y++;
				}
			}

			// Upgrade all frames to 64
			if(hasAny32Frames && hasAny64Frames)
			{
				foreach (var frameKey in gatheredSprites.Keys.ToArray())
				{
					Bitmap ogSprite = gatheredSprites[frameKey];

					if (ogSprite.Width != 64)
					{
						Console.WriteLine($"\t\tUpsizing {frameKey} to 64");
						Bitmap resizedSprite = new Bitmap(64, 64, ogSprite.PixelFormat);

						for (int px = 0; px < ogSprite.Width; ++px)
							for (int py = 0; py < ogSprite.Height; ++py)
							{
								resizedSprite.SetPixel(px, py, ogSprite.GetPixel(px, py));
							}

						gatheredSprites[frameKey] = resizedSprite;
					}
				}
			}

			if (gatheredSprites.Count != 0)
			{
				if (settings.AutomaticCentredFrames != null)
				{
					foreach (var pair in settings.AutomaticCentredFrames)
						RecentreSpritesInGroup(gatheredSprites, pair.Item1, pair.Item2);
				}


				// Now finally save the sprites to disk
				foreach (var pair in gatheredSprites)
				{
					string localFilePath = ContentCache.GetWriteableCachePath($"sprite_splitting\\{settings.CategoryName}\\{pair.Key}\\{dexNumber.ToString("D4")}_{mon}.png");
					pair.Value.Save(localFilePath);
				}
			}
		}

		public static void AppendNonUniformMonBlockSprites(string[] mons, ref int dexNumber, Settings settings, int initialOffsetX, ref int originX, ref int originY, int blockWidth, int blockHeight)
		{
			foreach (var mon in mons)
			{
				if (mon != "_") // _ reserved for "skip"
				{
					AppendNonUniformMonSprites(mon, dexNumber, settings, new Rectangle(originX, originY, blockWidth, blockHeight));
					++dexNumber;
				}

				if (settings.CustomStepX != 0 || settings.CustomStepY != 0)
				{
					originX += settings.CustomStepX;

					if (originX >= settings.Source.Width)
					{
						originX = initialOffsetX;
						originY += settings.CustomStepY;
					}
				}
				else
				{
					originX += settings.FrameStride * settings.CellSize;

					if (originX >= settings.Source.Width)
					{
						originX = 0;
						originY += settings.CellSize * (settings.FrameNames.Length / settings.FrameStride);
					}
				}
			}
		}

		private static void ApplyTransparencyToAlphaChannel(Bitmap dst)
		{
			Color transparentColour = dst.GetPixel(0, 0);
			for (int x = 0; x < dst.Width; ++x)
				for (int y = 0; y < dst.Height; ++y)
				{
					if (dst.GetPixel(x, y) == transparentColour)
						dst.SetPixel(x, y, Color.Transparent);
				}
		}

		private static void ApplyTransparencyToAlphaChannelNonUniform(Bitmap dst)
		{
			Color transparentColour = dst.GetPixel(dst.Width - 1, dst.Height - 1); // Take furthest corner
			for (int x = 0; x < dst.Width; ++x)
				for (int y = 0; y < dst.Height; ++y)
				{
					if (dst.GetPixel(x, y) == transparentColour)
						dst.SetPixel(x, y, Color.Transparent);
				}
		}

		private static void TryPropagateFillColor(Bitmap dst, int x, int y, Color startCol, Color targetCol)
		{

			Queue<Point> posQueue = new Queue<Point>();
			posQueue.Enqueue(new Point(x, y));

			while (posQueue.Count != 0)
			{
				Point pos = posQueue.Dequeue();

				if (pos.X < 0 || pos.Y < 0 || pos.X >= dst.Width || pos.Y >= dst.Height)
					continue;

				if (dst.GetPixel(pos.X, pos.Y) == startCol)
				{
					dst.SetPixel(pos.X, pos.Y, targetCol);
					posQueue.Enqueue(new Point(pos.X + 1, pos.Y));
					posQueue.Enqueue(new Point(pos.X - 1, pos.Y));
					posQueue.Enqueue(new Point(pos.X, pos.Y + 1));
					posQueue.Enqueue(new Point(pos.X, pos.Y - 1));
				}
			}
		}

		private static void TryPropagateUntilHitColor(Bitmap dst, int x, int y, Color targetCol)
		{
			Queue<Point> posQueue = new Queue<Point>();
			posQueue.Enqueue(new Point(x, y));

			while(posQueue.Count != 0)
			{
				Point pos = posQueue.Dequeue();

				if (pos.X < 0 || pos.Y < 0 || pos.X >= dst.Width || pos.Y >= dst.Height)
					continue;

				Color col = dst.GetPixel(pos.X, pos.Y);
				if (col != targetCol)
				{
					dst.SetPixel(pos.X, pos.Y, targetCol);
					posQueue.Enqueue(new Point(pos.X + 1, pos.Y));
					posQueue.Enqueue(new Point(pos.X - 1, pos.Y));
					posQueue.Enqueue(new Point(pos.X, pos.Y + 1));
					posQueue.Enqueue(new Point(pos.X, pos.Y - 1));
				}
			}
		}

		private static Bitmap ExtractNonUniformSprite(Bitmap source, int x, int y, int dirX, int dirY)
		{
			Dictionary<Point, Color> imgData = new Dictionary<Point, Color>();
			Color backgroundCol = source.GetPixel(x, y);

			while(true)
			{
				x += dirX;
				y += dirY;

				if (source.GetPixel(x, y) != backgroundCol)
					break;
			}

			ExtractNonUniformSpriteAtPos(source, imgData, x, y, backgroundCol);

			Point minPos = imgData.Keys.Aggregate((a, b) => new Point(Math.Min(a.X, b.X), Math.Min(a.Y, b.Y)));
			Point maxPos = imgData.Keys.Aggregate((a, b) => new Point(Math.Max(a.X, b.X), Math.Max(a.Y, b.Y)));
			Point dims = new Point(maxPos.X - minPos.X, maxPos.Y - minPos.Y);
			//Point centrePos = new Point((minPos.X + maxPos.X) / 2, (minPos.Y + maxPos.Y) / 2);

			int size = 32;
			if (dims.X >= 32 || dims.Y >= 32)
				size = 64;

			Bitmap dest = new Bitmap(size, size, PixelFormat.Format32bppArgb);

			// Position badly, as we'll fix this later
			foreach (var p in imgData)
			{
				int writeX = p.Key.X - minPos.X;
				int writeY = p.Key.Y - minPos.Y;

				dest.SetPixel(writeX, writeY, p.Value);
			}

			return dest;
		}

		private static void ExtractNonUniformSpriteAtPos(Bitmap src, Dictionary<Point, Color> dst, int x, int y, Color backgroundCol)
		{
			Queue<Point> posQueue = new Queue<Point>();
			posQueue.Enqueue(new Point(x, y));

			while (posQueue.Count != 0)
			{
				Point pos = posQueue.Dequeue();

				if (pos.X < 0 || pos.Y < 0 || pos.X >= src.Width || pos.Y >= src.Height)
					continue;

				if (dst.ContainsKey(pos))
					continue;

				Color col = src.GetPixel(pos.X, pos.Y);
				if (src.GetPixel(pos.X, pos.Y) != backgroundCol)
				{
					dst.Add(pos, col);
					posQueue.Enqueue(new Point(pos.X + 1, pos.Y));
					posQueue.Enqueue(new Point(pos.X - 1, pos.Y));
					posQueue.Enqueue(new Point(pos.X, pos.Y + 1));
					posQueue.Enqueue(new Point(pos.X, pos.Y - 1));
					posQueue.Enqueue(new Point(pos.X + 1, pos.Y + 1));
					posQueue.Enqueue(new Point(pos.X - 1, pos.Y + 1));
					posQueue.Enqueue(new Point(pos.X + 1, pos.Y - 1));
					posQueue.Enqueue(new Point(pos.X - 1, pos.Y - 1));
				}
			}
		}

		private static void GetSpriteMinMax(Bitmap target, out Point min, out Point max)
		{
			min = new Point(target.Width - 1, target.Height - 1);
			max = new Point(0, 0);

			for(int x = 0; x < target.Width; ++x)
				for(int y = 0; y < target.Height; ++y) 
				{
					if(target.GetPixel(x,y).A != 0)
					{
						min.X = Math.Min(min.X, x);
						min.Y = Math.Min(min.Y, y);

						max.X = Math.Max(max.X, x);
						max.Y = Math.Max(max.Y, y);
					}
				}
		}

		private static void ShiftPixels(Bitmap src, Bitmap dst, int offsetX, int offsetY)
		{
			Color SafeGetSrcPixel(int x, int y)
			{
				if (x < 0 || y < 0 || x >= src.Width || y >= src.Height)
					return Color.Transparent;

				return src.GetPixel(x, y);
			}

			for (int x = 0; x < dst.Width; ++x)
				for (int y = 0; y < dst.Height; ++y)
					dst.SetPixel(x, y, SafeGetSrcPixel(x - offsetX, y - offsetY));
		}

		private static void RecentreSpritesInGroup(Dictionary<string, Bitmap> spriteSet, string key0, string key1)
		{
			GetSpriteMinMax(spriteSet[key0], out Point min0, out Point max0);
			GetSpriteMinMax(spriteSet[key1], out Point min1, out Point max1);

			Point totalMin = new Point(
				Math.Min(min0.X, min1.X),
				Math.Min(min0.Y, min1.Y)
			);
			Point totalMax = new Point(
				Math.Max(max0.X, max1.X),
				Math.Max(max0.Y, max1.Y)
			);

			void Run(string key, Point min, Point max)
			{
				Bitmap sprite = spriteSet[key];
				Bitmap temp = new Bitmap(sprite.Width, sprite.Height, sprite.PixelFormat);

				// Place in average centre for X?
				int currentCentreX = (int)Math.Ceiling((min.X + max.X) / 2.0f);
				int desiredCentreX = sprite.Width / 2;// (totalMin.X + totalMax.X) / 2;

				// Want to place 1 pixel above bottom
				int desiredY = sprite.Height - 2;

				// Don't shift if at bottom
				if(max.Y == 31 || max.Y == 63)
					desiredY = max.Y;

				ShiftPixels(sprite, temp, 0, desiredY - max.Y);
				ShiftPixels(temp, sprite, desiredCentreX - currentCentreX, 0);
			}

			Run(key0, min0, max0);
			Run(key1, min1, max1);
		}
	}
}
