using PokemonDataGenerator.Utils;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator.OverworldSprites
{
	/// <summary>
	/// Take in the source sprite and then generate a shiny variant based on the game data shiny palettes
	/// </summary>
	public static class ShinySpriteGenerator
	{
		public static void GenerateMonSprites(string mon)
		{
			var spriteData = OverworldSpriteGenerator.GetGatheredSpriteDataFor(mon);

			var normalPalette = ImagePalette.FromFile(GameDataHelpers.GetPokemonNormalPalettePath(mon), ImagePalette.DistanceMethod.YUV);
			var shinyPalette = ImagePalette.FromFile(GameDataHelpers.GetPokemonShinyPalettePath(mon), ImagePalette.DistanceMethod.YUV);

			foreach (var frameKvp in spriteData.spriteUri.ToArray())
			{
				string frameName = frameKvp.Key;
				string framePath = frameKvp.Value;

				if(!frameName.EndsWith("_shiny"))
				{
					var frameImg = new Bitmap(Bitmap.FromFile(framePath));

					for (int y = 0; y < frameImg.Height; ++y)
					{
						for (int x = 0; x < frameImg.Width; ++x)
						{
							Color pixel = frameImg.GetPixel(x, y);

							if(pixel.A != 0)
							{
								// Try to override
								int palIndex = normalPalette.GetClosestMatchIndex(pixel);
								Color normalColour = normalPalette[palIndex];
								Color shinyColour = shinyPalette[palIndex];

								frameImg.SetPixel(x, y, CalculateShinyColour(pixel, normalColour, shinyColour));
							}
						}
					}

					string outputFilePath = ContentCache.GetWriteableCachePath($"sprite_splitting\\auto_shinies\\{frameName}\\{spriteData.pokedexNumber.ToString("D4")}_{mon}.png");

					string outputDirPath = Path.GetDirectoryName(outputFilePath);
					Directory.CreateDirectory(outputDirPath);

					frameImg.Save(outputFilePath);


					string outFrameName = frameName.Substring(0, frameName.Length - 2) + "_shiny" + frameName.Substring(frameName.Length - 2);
					OverworldSpriteGenerator.AppendMonSpriteUri(mon, spriteData.pokedexNumber, outFrameName, outputFilePath);
				}
			}

			return;
		}

		private static Color CalculateShinyColour(Color inColour, Color normalColour, Color shinyColour)
		{
			if (inColour.GetBrightness() <= 0.05f)
				return inColour;

			if (normalColour == shinyColour)
				return inColour;

            HSLColor outputHSL = inColour;

            HSLColor normalHSL = normalColour;
            HSLColor shinyHSL = shinyColour;

			outputHSL.Hue += (shinyHSL.Hue - normalHSL.Hue);
			outputHSL.Saturation += (shinyHSL.Saturation - normalHSL.Saturation);
			outputHSL.Luminosity += (shinyHSL.Luminosity - normalHSL.Luminosity);

			return outputHSL;

        }
    }
}
