using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator.OverworldSprites
{
	public static class SpriteSheetSplitter_Gen5
	{
		public static void AppendMonSprites()
		{
			int pokedexNumber = 494;
			OverworldSpriteGenerator.AppendMonSpriteUri("", pokedexNumber++, "", "res://");
		}
	}
}
