using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator
{
	class Program
	{
		private static readonly string c_OutputPath = "..\\..\\..\\..\\..\\..\\src\\data\\rogue_presetmons.h";

		static void Main(string[] args)
		{
			MonPresetGenerator.GenerateFromURL(@"https://play.pokemonshowdown.com/data/sets/gen3.json", true);
			//MonPresetGenerator.GenerateFromURL(@"https://play.pokemonshowdown.com/data/sets/gen7.json", false);
			//MonPresetGenerator.GenerateFromURL(@"https://play.pokemonshowdown.com/data/sets/gen8.json", false);

			MonPresetGenerator.ExportToHeader(c_OutputPath);
		}
	}
}
