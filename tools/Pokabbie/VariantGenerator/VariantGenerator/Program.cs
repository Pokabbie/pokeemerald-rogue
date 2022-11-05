using System;
using System.Collections.Generic;
using System.IO;

namespace VariantGenerator
{
	class Program
	{
		private static readonly string c_PaletteRelativePath = "..\\..\\..\\..\\..\\..\\..\\graphics\\rogue_palettes";

		static void Main(string[] args)
		{
			string baseDir = Path.GetFullPath(c_PaletteRelativePath);

			foreach (var basePalette in Directory.EnumerateFileSystemEntries(baseDir, "0_0.pal", SearchOption.AllDirectories))
			{
				GeneratePaletteVariants(basePalette);
			}

			Console.WriteLine("Finished..");
		}

		private class PaletteProfile
		{
			private string[] m_Lines;
			private bool[] m_LineEdits;

			private PaletteProfile(string[] lines)
			{
				m_Lines = new string[lines.Length];
				m_LineEdits = new bool[lines.Length];

				Array.Copy(lines, m_Lines, m_Lines.Length);
			}

			public static PaletteProfile LoadBase(string path)
			{
				PaletteProfile profile = new PaletteProfile(File.ReadAllLines(path));
				return profile;
			}

			public static PaletteProfile LoadDiff(string path, PaletteProfile baseProfile)
			{
				PaletteProfile profile = new PaletteProfile(File.ReadAllLines(path));

				for (int i = 0; i < profile.m_Lines.Length; ++i)
					profile.m_LineEdits[i] = profile.m_Lines[i].Trim() != baseProfile.m_Lines[i].Trim();

				return profile;
			}

			public static PaletteProfile CollapseProfiles(PaletteProfile baseProfile, params PaletteProfile[] editProfiles)
			{
				PaletteProfile profile = new PaletteProfile(baseProfile.m_Lines);

				foreach (var edit in editProfiles)
				{
					for (int i = 0; i < edit.m_Lines.Length; ++i)
					{
						if (edit.m_LineEdits[i])
							profile.m_Lines[i] = edit.m_Lines[i];
					}
				}

				return profile;
			}

			public void WriteToFile(string path)
			{
				Console.WriteLine($"Exporting '{path}'");
				File.WriteAllLines(path, m_Lines);
			}
		}

		private static void GeneratePaletteVariants(string basePalette)
		{
			string dir = Path.GetDirectoryName(basePalette);

			PaletteProfile baseProfile = PaletteProfile.LoadBase(basePalette);

			List<PaletteProfile> style0Profiles = new List<PaletteProfile>();
			List<PaletteProfile> style1Profiles = new List<PaletteProfile>();

			for (int i = 1; ; ++i)
			{
				string path = Path.Combine(dir, $"{i}_0.pal");
				if (File.Exists(path))
				{
					style0Profiles.Add(PaletteProfile.LoadDiff(path, baseProfile));
				}
				else
					break;
			}

			for (int i = 1; ; ++i)
			{
				string path = Path.Combine(dir, $"0_{i}.pal");
				if (File.Exists(path))
				{
					style1Profiles.Add(PaletteProfile.LoadDiff(path, baseProfile));
				}
				else
					break;
			}

			for (int i = 0; i < style0Profiles.Count; ++i)
			{
				for (int j = 0; j < style1Profiles.Count; ++j)
				{
					PaletteProfile outputProfile = PaletteProfile.CollapseProfiles(baseProfile, style0Profiles[i], style1Profiles[j]);

					string outputPath = Path.Combine(dir, $"{i + 1}_{j + 1}.pal");

					//File.Delete(outputPath);
					outputProfile.WriteToFile(outputPath);
				}
			}
		}
	}
}
