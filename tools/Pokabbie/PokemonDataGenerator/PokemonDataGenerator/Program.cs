using PokemonDataGenerator.OverworldSprites;
using PokemonDataGenerator.Pokedex;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator
{
	class Program
	{
		private static readonly string c_PresetOutputPath = "..\\..\\..\\..\\..\\..\\src\\data\\rogue_presetmons.h";

		static void Main(string[] args)
		{
			Console.WriteLine("1 - Vanilla");
			Console.WriteLine("2 - EX");
			bool isVanillaVersion = ReadOption(1, 2) == 1;

			Console.WriteLine("1 - Generate Presets");
			Console.WriteLine("2 - Generate OW Sprites");
			Console.WriteLine("3 - Generate OW Sprites (DEBUG FAST SET)");
			Console.WriteLine("4 - OW Sprites Palette Generator");
			Console.WriteLine("5 - Generate Pokedex Lists");
			Console.WriteLine("6 - Gather Pokemon data profile");
			int action = ReadOption(1, 6);

			switch(action)
			{
				case 1:
					Console.WriteLine("==Generating Presets==");
					if(isVanillaVersion)
					{
						MonPresetGenerator.GenerateFromURL(@"https://play.pokemonshowdown.com/data/sets/gen3.json", true);
					}
					else
					{
						MonPresetGenerator.GenerateFromURL(@"https://play.pokemonshowdown.com/data/sets/gen7.json", false);
						MonPresetGenerator.GenerateFromURL(@"https://play.pokemonshowdown.com/data/sets/gen8.json", false);
						MonPresetGenerator.GenerateFromURL(@"https://play.pokemonshowdown.com/data/sets/gen9.json", false);
					}
					MonPresetGenerator.ExportToHeader(c_PresetOutputPath);
					break;

				case 2:
					{
						Console.WriteLine("==Generate OW Sprites==");
						OverworldSpriteGenerator.s_TargettingVanilla = isVanillaVersion;
						OverworldSpriteGenerator.s_GenerateShinies = true;// ReadBool("Include Shinies?");

						Console.WriteLine("1 - Collate Sprites");
						Console.WriteLine("2 - Export Sprites");
						bool isCollating = ReadOption(1, 2) == 1;

						OverworldSpriteGenerator.GenerateFromURL(isCollating);
						break;
					}

				case 3:
					{
						Console.WriteLine("==Generate OW Sprites (DEBUG FAST SET)==");
						OverworldSpriteGenerator.s_TargettingDebugSet = true;
						OverworldSpriteGenerator.s_TargettingVanilla = isVanillaVersion;
						OverworldSpriteGenerator.s_GenerateShinies = ReadBool("Include Shinies?");

						Console.WriteLine("1 - Collate Sprites");
						Console.WriteLine("2 - Export Sprites");
						bool isCollating = ReadOption(1, 2) == 1;

						OverworldSpriteGenerator.GenerateFromURL(isCollating);
						break;
					}

				case 4:
					Console.WriteLine("==OW Sprites Palette Generator==");
					SpritePaletteGenerator.s_OutputCount = ReadNumber("Number of palettes?", 1, 100);
					SpritePaletteGenerator.GenerateFromLocalData();
					break;

				case 5:
					Console.WriteLine("==Generating Pokedex Lists==");
					PokedexGenerator.GeneratePokedexEntries(isVanillaVersion);
					break;

				case 6:
					Console.WriteLine("==Gathering Pokemon Data Profile==");
					PokemonProfileGenerator.GatherProfiles();
					break;
			}


			Console.WriteLine("Press any key to exit...");
			Console.ReadKey();
		}

		private static int ReadOption(int min, int max)
		{
			do
			{
				Console.WriteLine("Select an option:");
				string raw = Console.ReadLine();

				if (int.TryParse(raw, out int result))
				{
					if (result >= min && result <= max)
						return result;
				}
			}
			while (true);
		}

		private static int ReadNumber(string prompt, int min, int max)
		{
			do
			{
				Console.WriteLine(prompt);
				string raw = Console.ReadLine();

				if (int.TryParse(raw, out int result))
				{
					if (result >= min && result <= max)
						return result;
				}
			}
			while (true);
		}

		private static bool ReadBool(string prompt)
		{
			do
			{
				Console.WriteLine(prompt);
				string raw = Console.ReadLine();

				switch(raw.ToLower())
				{
					case "y":
					case "yes":
					case "t":
					case "true":
					case "1":
						return true;
					case "n":
					case "no":
					case "f":
					case "false":
					case "0":
						return false;
				}
			}
			while (true);
		}
	}
}
