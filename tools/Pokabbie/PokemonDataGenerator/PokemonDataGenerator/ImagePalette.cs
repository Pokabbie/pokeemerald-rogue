using PokemonDataGenerator.Utils;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator
{
	public class ImagePalette
	{
		public enum DistanceMethod
		{
			RGB,
			YUV,
			HSL
		}

		private Color[] m_Colors = null;
		private DistanceMethod m_DistanceMethod = DistanceMethod.RGB;

		private ImagePalette()
		{

		}

		public ImagePalette(Color[] colors)
		{
			m_Colors = colors.ToArray();
		}

		public Color this[int i]
		{
			get => m_Colors[i];
			set => m_Colors[i] = value;
		}

		public static ImagePalette FromFile(string path, bool useSimpleColorDistance = false)
		{
			return FromFile(path, useSimpleColorDistance ? DistanceMethod.RGB : DistanceMethod.YUV);
		}

		public static ImagePalette FromFile(string path, DistanceMethod distanceMethod)
		{
			ImagePalette palette = new ImagePalette();
			palette.m_Colors = new Color[16];
			palette.m_DistanceMethod = distanceMethod;

			using (FileStream stream = new FileStream(path, FileMode.Open))
			using(StreamReader reader = new StreamReader(stream))
			{
				if (reader.ReadLine() != "JASC-PAL")
					throw new FormatException();

				if (reader.ReadLine() != "0100")
					throw new FormatException();

				int palCount;
				if (!int.TryParse(reader.ReadLine(), out palCount))
					palCount = -1;

				if (palCount > 16)
					throw new FormatException();

				for(int i = 0; i < palCount; ++i)
				{
					string[] rawChannels = reader.ReadLine().Split(' ');

					palette.m_Colors[i] = Color.FromArgb(
						int.Parse(rawChannels[0]),
						int.Parse(rawChannels[1]),
						int.Parse(rawChannels[2])
					);
				}
			}

			return palette;
		}

		public void Save(string filePath)
		{
			using(FileStream stream = new FileStream(filePath, FileMode.Create))
			using(StreamWriter writer = new StreamWriter(stream))
			{
				writer.WriteLine("JASC-PAL");
				writer.WriteLine("0100");
				writer.WriteLine("16");

				for (int i = 0; i < 16; ++i)
				{
					if (i < m_Colors.Length)
						writer.WriteLine($"{m_Colors[i].R} {m_Colors[i].G} {m_Colors[i].B}");
					else
						writer.WriteLine("0 0 0");
				}
			}
		}

		public static double GetColorDistance_RGB(Color e1, Color e2)
		{
			long rmean = ((long)e1.R + (long)e2.R) / 2;
			long r = (long)e1.R - (long)e2.R;
			long g = (long)e1.G - (long)e2.G;
			long b = (long)e1.B - (long)e2.B;
			return Math.Sqrt((((512 + rmean) * r * r) >> 8) + 4 * g * g + (((767 - rmean) * b * b) >> 8));
		}


		private static void RGBtoYUV(Color col, out float Y, out float U, out float V)
		{
			float R = (col.R / 255.0f);
			float G = (col.G / 255.0f);
			float B = (col.B / 255.0f);

			Y = 0.257f * R + 0.504f * G + 0.098f * B + 16.0f;
			U = -0.148f * R - 0.291f * G + 0.439f * B + 128.0f;
			V = 0.439f * R - 0.368f * G - 0.071f * B + 128.0f;
		}

		public static double GetColorDistance_YUV(Color e1, Color e2)
		{
			RGBtoYUV(e1, out float e1Y, out float e1U, out float e1V);
			RGBtoYUV(e2, out float e2Y, out float e2U, out float e2V);

			float a = e1Y - e2Y;
			float b = e1U - e2U;
			float c = e1V - e2V;

			return Math.Sqrt(a * a + b * b + c * c);
		}

		public static double GetColorDistance_HSV(Color e1, Color e2)
		{
			float h = 1.0f * (e1.GetHue() / 360.0f - e2.GetHue() / 360.0f);
			float s = 1.0f * (e1.GetSaturation() - e2.GetSaturation());
			float v = 1.0f * (e1.GetBrightness() - e2.GetBrightness());
			
			return Math.Sqrt(h * h + s * s + v * v);
		}

		public static double GetColorDistance_HSL(HSLColor e1, HSLColor e2)
		{
			float h = 1.0f * (float)(e2.Hue - e1.Hue);
			float s = 1.0f * (float)(e2.Saturation - e1.Saturation);
			float v = 1.0f * (float)(e2.Luminosity - e1.Luminosity);

			return Math.Sqrt(h * h + s * s + v * v);
		}

		public static double GetColorDistance_Scoring(Color e1, Color e2, float hw, float sw, float vw)
		{
			float h = hw * (e1.GetHue() / 360.0f - e2.GetHue() / 360.0f);
			float s = sw * (e1.GetSaturation() - e2.GetSaturation());
			float v = vw * (e1.GetBrightness() - e2.GetBrightness());

			return Math.Sqrt(h * h + s * s + v * v);
		}

		public double GetColorDistance(Color e1, Color e2)
		{
			return GetColorDistance(e1, e2, m_DistanceMethod);
		}

		public static double GetColorDistance(Color e1, Color e2, DistanceMethod method)
		{
			if (e1 == e2)
				return 0;

			switch (method)
			{
				case DistanceMethod.RGB:
					return GetColorDistance_RGB(e1, e2);

				case DistanceMethod.YUV:
					return GetColorDistance_YUV(e1, e2);

				case DistanceMethod.HSL:
					return GetColorDistance_HSL(e1, e2);
			}

			return 0.0f;
		}

		public int GetClosestMatchIndex(Color input, int indexStart = 1)
		{
			if (input.A != 255)
				return 0;
			else
			{
				int currentIndex = 0;
				double minDistance = double.MaxValue;

				for(int i = indexStart; i < m_Colors.Length; ++i)
				{
					double distance = GetColorDistance(input, m_Colors[i]);

					if(distance < minDistance)
					{
						minDistance = distance;
						currentIndex = i;
					}
				}

				return currentIndex;
			}
		}

		public double GetBitmapMatchScore(Bitmap src, int indexStart = 1)
		{
			// Going to use std HSV as the weights for scoring, as this may change from sprite to sprite
			//double avgH = 0.0f;
			//double avgS = 0.0f;
			//double avgV = 0.0f;
			//double totalSamples = 0.0f;
			//
			//for (int y = 0; y < src.Height; y += 1)
			//	for (int x = 0; x < src.Width; x += 1)
			//	{
			//		Color col = src.GetPixel(x + 0, y + 0);
			//
			//		if (col.A != 0)
			//		{
			//			++totalSamples;
			//			avgH += col.GetHue() / 360.0f;
			//			avgS += col.GetSaturation();
			//			avgV += col.GetBrightness();
			//		}
			//	}
			//
			//avgH /= totalSamples;
			//avgS /= totalSamples;
			//avgV /= totalSamples;
			//
			//double hDev = 0.0f;
			//double sDev = 0.0f;
			//double vDev = 0.0f;
			//
			//for (int y = 0; y < src.Height; y += 1)
			//	for (int x = 0; x < src.Width; x += 1)
			//	{
			//		Color col = src.GetPixel(x + 0, y + 0);
			//
			//		if (col.A != 0)
			//		{
			//			hDev += Math.Pow(col.GetHue() / 360.0f - avgH, 2);
			//			sDev += Math.Pow(col.GetSaturation() - avgS, 2);
			//			vDev += Math.Pow(col.GetBrightness() - avgV, 2);
			//		}
			//	}
			//
			//hDev = Math.Sqrt(hDev / (totalSamples - 1));
			//sDev = Math.Sqrt(sDev / (totalSamples - 1));
			//vDev = Math.Sqrt(vDev / (totalSamples - 1));
			//
			//// Now calculate the score with the calculated weights
			//
			//double totalScore = 0.0f;
			//for (int y = 0; y < src.Height; y += 1)
			//	for (int x = 0; x < src.Width; x += 1)
			//	{
			//		Color col = src.GetPixel(x + 0, y + 0);
			//
			//		if (col.A != 0)
			//		{
			//			int index = GetClosestMatchIndex(col, indexStart);
			//			totalScore += GetColorDistance_Scoring(col, m_Colors[index], (float)hDev, (float)sDev, (float)vDev);
			//		}
			//	}


			double totalScore = 0.0f;
			double totalSamples = 0.0f;
			HashSet<int> coloursUsed = new HashSet<int>();

			for (int y = 0; y < src.Height; y += 1)
				for (int x = 0; x < src.Width; x += 1)
				{
					Color col = src.GetPixel(x + 0, y + 0);
			
					if (col.A != 0)
					{
						++totalSamples;
						int index = GetClosestMatchIndex(col, indexStart);
						coloursUsed.Add(index);

						float hueWeight = 1.0f - (Math.Abs(col.GetBrightness() - 0.5f) * 2.0f);

						totalScore += GetColorDistance_Scoring(col, m_Colors[index], hueWeight * 2, 1.0f - hueWeight, 0.5f);
						//totalScore += GetColorDistance_HSV(col, m_Colors[index]);

						//totalScore += GetColorDistance(col, m_Colors[index]);
					}
				}

			totalScore = totalSamples != 0.0f ? (totalScore / totalSamples) : double.MaxValue; // Avg score diff

			totalScore += 0.5f * ((16.0 - coloursUsed.Count) / 16.0); // Consider colors used

			return totalScore;
		}

		public static ImagePalette CreateFromContent(Bitmap src, int maxColours, DistanceMethod distanceMethod, Color transparentColour)
		{
			// Don't use a hashset as we want the colour order to be consistent
			List<Color> uniqueColors = new List<Color>();

			for (int x = 0; x < src.Width; ++x)
				for (int y = 0; y < src.Height; ++y)
				{
					if(uniqueColors.FindIndex(c => c.ToArgb() == src.GetPixel(x, y).ToArgb()) == -1)
						uniqueColors.Add(src.GetPixel(x, y));
				}

			List<Color> remainingColours = new List<Color>(uniqueColors);

			while(remainingColours.Count > maxColours)
			{
				int minIndexA = 0;
				int minIndexB = 0;
				double minScore = double.MaxValue;

				// Find closest pair
				for (int i = 0; i < remainingColours.Count; ++i)
				{
					for (int j = 0; j < remainingColours.Count; ++j)
					{
						if(i != j && remainingColours[i] != transparentColour && remainingColours[j] != transparentColour)
						{
							double score = GetColorDistance(remainingColours[i], remainingColours[j], distanceMethod);
							if(score < minScore)
							{
								minScore = score;
								minIndexA = i;
								minIndexB = j;
							}
						}
					}
				}

				// For now just remove the 2nd one
				remainingColours.RemoveAt(minIndexB);
			}

			// Make sure transparent colour is at the front always
			int transparentIndex = remainingColours.FindIndex(colour => transparentColour.ToArgb() == colour.ToArgb());
			if(transparentIndex != 0)
			{
				remainingColours[transparentIndex] = remainingColours[0];
				remainingColours[0] = transparentColour;
			}

			return new ImagePalette(remainingColours.ToArray());
		}

		public Bitmap CreateIndexedBitmap(Bitmap src, int indexStart = 1)
		{
			//Bitmap dst = src.Clone(new Rectangle(0, 0, src.Width, src.Height), PixelFormat.Format4bppIndexed);

			Bitmap dst = new Bitmap(src.Width, src.Height, System.Drawing.Imaging.PixelFormat.Format4bppIndexed);
			BitmapData data = dst.LockBits(new Rectangle(0, 0, dst.Width, dst.Height), ImageLockMode.WriteOnly, PixelFormat.Format4bppIndexed);

			ColorPalette pal = dst.Palette;
			for (int i = 0; i < 16; ++i)
				pal.Entries[i] = i < m_Colors.Length ? m_Colors[i] : Color.Black;
			dst.Palette = pal;

			List<byte> indexedBytes = new List<byte>();

			for (int y = 0; y < src.Height; y += 1)
				for (int x = 0; x < src.Width; x += 2)
				{
					int hiIndex = GetClosestMatchIndex(src.GetPixel(x + 0, y + 0), indexStart);
					int loIndex = GetClosestMatchIndex(src.GetPixel(x + 1, y + 0), indexStart);
					
					indexedBytes.Add((byte)(
						(loIndex & 0xF) +
						((hiIndex << 4) & 0xF0)
					));

					//int loIndex = GetClosestMatchIndex(src.GetPixel(x + 0, y + 0), indexStart);
					//
					//indexedBytes.Add((byte)(
					//	loIndex
					//));
				}

			Marshal.Copy(indexedBytes.ToArray(), 0, data.Scan0, indexedBytes.Count);
			dst.UnlockBits(data);
			return dst;
		}
	}
}
