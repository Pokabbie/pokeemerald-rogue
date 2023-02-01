using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator.OverworldSprites
{
	public static class SpritePaletteGenerator
	{
		public static int s_OutputCount = 0;

		public static void GenerateFromLocalData()
		{
			string inputDir = Path.GetFullPath("sprite_temp_RAW"); // This is the directory with the uncollapsed sprites

			Dictionary<Color, int> colorUsages = new Dictionary<Color, int>();

			foreach(var imgPath in Directory.EnumerateFiles(inputDir, "*.png"))
			{
				Bitmap img = new Bitmap(imgPath);
				HashSet<Color> usedColors = new HashSet<Color>();

				Console.WriteLine($"\tConsidering '{Path.GetFileName(imgPath)}'");

				for (int x = 0; x < img.Width; ++x)
					for(int y = 0; y < img.Height; ++y)
					{
						Color col = img.GetPixel(x, y);

						// transform colour
						int snappingFactor = 1;

						//if (col.GetSaturation() < 0.7 || col.GetBrightness() < 0.4)
						//	snappingFactor = 32;
						//else if (col.GetBrightness() >= 0.2)
						//	snappingFactor = 32;
						//else
						//	snappingFactor = 2;

						col = Color.FromArgb(255, (col.R / snappingFactor) * snappingFactor, (col.G / snappingFactor) * snappingFactor, (col.B / snappingFactor) * snappingFactor);

						if (col.A != 0)
							usedColors.Add(col);
					}

				foreach(var col in usedColors)
				{
					//int weight = (col.GetBrightness() < 0.6 ? 1 : 6) + (col.GetSaturation() < 0.5 ? 1 : 6);
					int weight = 1;// (col.GetBrightness() * col.GetSaturation() < 0.3 ? 1 : 3);

					if (colorUsages.ContainsKey(col))
						colorUsages[col] += weight;
					else
						colorUsages[col] = weight;
				}
			}

			Console.WriteLine($"Collapsing used colours");

			// Collapse used colours down
			Random random = new Random(3104971); // Want the randomness to be deteminisitic

			//bool reduceBasedOnUsage = false;
			//if (reduceBasedOnUsage)
			//{
			//	while (colorUsages.Count > 15 * s_OutputCount)
			//	{
			//		// Find colour with least usages
			//		///
			//		int minUsage = colorUsages.Values.Aggregate((a, b) => Math.Min(a, b));
			//
			//		KeyValuePair<Color, int> entryToRemove = colorUsages.Where((kvp) => kvp.Value == minUsage).OrderBy((a) => random.Next(0, 100)).First();
			//
			//		// Find the nearest colour to collapse this on into
			//		Color currTarget = Color.Empty;
			//		double currDistance = double.MaxValue;
			//
			//		foreach (var kvp in colorUsages)
			//		{
			//			if (kvp.Key != entryToRemove.Key)
			//			{
			//				double distance = ImagePalette.GetColorDistance_YUV(kvp.Key, entryToRemove.Key);
			//				if (distance < currDistance)
			//				{
			//					currDistance = distance;
			//					currTarget = entryToRemove.Key;
			//				}
			//			}
			//		}
			//
			//		colorUsages[currTarget] += entryToRemove.Value;
			//
			//		colorUsages.Remove(entryToRemove.Key);
			//
			//
			//		if ((colorUsages.Count % 100) == 0)
			//			Console.WriteLine($"\tReduced Colours down to {colorUsages.Count}");
			//	}
			//}
			//else
			{
				// Reduce base on color distance
				//
				Dictionary<Color, Color> closestColourMatch = new Dictionary<Color, Color>();

				void RecalculateClosestColour(Color key)
				{
					Color currTarget = Color.Empty;
					double currDistance = double.MaxValue;

					foreach (var kvp in colorUsages)
					{
						if (kvp.Key != key)
						{
							double distance = ImagePalette.GetColorDistance_YUV(kvp.Key, key);
							if (distance < currDistance)
							{
								currDistance = distance;
								currTarget = kvp.Key;
							}
						}
					}

					closestColourMatch[key] = currTarget;
				}

				Console.WriteLine($"\tCalculating closest matches for {colorUsages.Count}");
				foreach (var kvp in colorUsages)
				{
					RecalculateClosestColour(kvp.Key);

					if ((closestColourMatch.Count % 100) == 0)
						Console.WriteLine($"\t\tCalculated {closestColourMatch.Count} / {colorUsages.Count}");
				}

				while (colorUsages.Count > 15 * s_OutputCount)
				{
					Color currTargetA = Color.Empty;
					Color currTargetB = Color.Empty;
					double currDistance = double.MaxValue;

					// Get the closets colours
					foreach (var kvp in closestColourMatch)
					{
						double distance = ImagePalette.GetColorDistance_YUV(kvp.Key, kvp.Value);
						if (distance < currDistance)
						{
							currDistance = distance;
							currTargetA = kvp.Key;
							currTargetB = kvp.Value;
						}
					}

					// Remove the one with the least usages
					if (colorUsages[currTargetA] < colorUsages[currTargetB])
					{
						colorUsages[currTargetB] += colorUsages[currTargetA];
						colorUsages.Remove(currTargetA);
						closestColourMatch.Remove(currTargetA);

						// Correct any colors which point to the removed one
						foreach (var kvp in closestColourMatch.ToArray())
						{
							if (kvp.Value == currTargetA)
								RecalculateClosestColour(kvp.Key);
						}
					}
					else
					{
						colorUsages[currTargetA] += colorUsages[currTargetB];
						colorUsages.Remove(currTargetB);
						closestColourMatch.Remove(currTargetB);

						// Correct any colors which point to the removed one
						foreach (var kvp in closestColourMatch.ToArray())
						{
							if (kvp.Value == currTargetB)
								RecalculateClosestColour(kvp.Key);
						}
					}

					if ((colorUsages.Count % 10) == 0)
						Console.WriteLine($"\tReduced Colours down to {colorUsages.Count}");
				}
			}

			Color backgroundColour = Color.FromArgb(255, 115, 197, 164);
			while(colorUsages.ContainsKey(backgroundColour))
			{
				backgroundColour = Color.FromArgb(255, random.Next(0, 256), random.Next(0, 256), random.Next(0, 256));
			}

			Color[] orderedColours = colorUsages.OrderBy((kvp) => kvp.Value).Select((kvp) => kvp.Key).ToArray();

			for (int i = 0; i < s_OutputCount; ++i)
			{
				Color[] paletteColours = new Color[16];
				paletteColours[0] = backgroundColour;

				for (int j = 0; j < 15; ++j)
				{
					Color srcCol = orderedColours[i + j * s_OutputCount];
					colorUsages.Remove(srcCol);

					paletteColours[j + 1] = srcCol;
				}

				string outputPath = Path.GetFullPath($"sprite_palettes\\followmon_{i + 1}.pal");
				Console.WriteLine($"Exporting '{outputPath}'");

				Directory.CreateDirectory(Path.GetDirectoryName(outputPath));

				ImagePalette palette = new ImagePalette(paletteColours);
				palette.Save(outputPath);
			}
		}
	}
}
