using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator.OverworldSprites
{
	public static class SpriteSheetSplitter_Gen6
	{
		public static void AppendMonSprites()
		{
			AppendMonSpritesInternal("", "res://gen_6__kalos__pokemon_overworld_sprites_by_princess_phoenix_d8p510p.png");

			if(OverworldSpriteGenerator.s_GenerateShinies)
				AppendMonSpritesInternal("_shiny", "res://shiny_gen_6__kalos__pokemon_overworld_sprites_by_princess_phoenix_d929j1d.png");
		}

		private static void AppendMonSpritesInternal(string groupKey, string sourcePath)
		{
			int pokedexNumber = 650;
			int originX = 0;
			int originY = 0;

			SpriteSheetSplitter.Settings settings = new SpriteSheetSplitter.Settings();
			settings.CategoryName = "gen6";
			settings.CellSize = 32;
			settings.FrameNames = new string[] 
			{
				$"back{groupKey}_1",	$"side{groupKey}_1",
				$"back{groupKey}_2",	$"side{groupKey}_2",
				$"front{groupKey}_1",	$"_",
				$"front{groupKey}_2",	$"_"
			};
			settings.FrameStride = 2;
			settings.Source = ContentCache.GetImageContent(sourcePath);

			Console.WriteLine($"Gathering '{settings.CategoryName}'");

			string[] monNames = new string[]
			{
				"chespin",
				"quilladin",
				"chesnaught",
				"fennekin",
				"braixen",
				"delphox",
				"froakie",
				"frogadier",
				"greninja",
				"bunnelby",
				"diggersby",
				"fletchling",
				"fletchinder",
				"talonflame",
				"scatterbug",
				"spewpa",
				"vivillon",
				"litleo",
				"pyroar",
				"_",
			};
			SpriteSheetSplitter.AppendMonBlockSprites(monNames, ref pokedexNumber, settings, ref originX, ref originY);

			// These are flipped
			//
			settings.FrameNames = new string[]
			{
				$"back{groupKey}_1",    $"_",
				$"back{groupKey}_2",    $"_",
				$"front{groupKey}_1",   $"side{groupKey}_1",
				$"front{groupKey}_2",   $"side{groupKey}_2"
			};
			monNames = new string[]
			{
				"flabebe",
				"_",
				"_",
				"_",
				"_",
				"floette",
				"_",
				"_",
				"_",
				"_",
				"_",
				"florges",
				"_",
				"_",
				"_",
				"_"
			};
			SpriteSheetSplitter.AppendMonBlockSprites(monNames, ref pokedexNumber, settings, ref originX, ref originY);

			// Back to normal
			//
			settings.FrameNames = new string[]
			{
				$"back{groupKey}_1",    $"side{groupKey}_1",
				$"back{groupKey}_2",    $"side{groupKey}_2",
				$"front{groupKey}_1",   $"_",
				$"front{groupKey}_2",   $"_"
			};
			monNames = new string[]
			{
				"skiddo",
				"gogoat",
				"pancham",
				"pangoro",
				"furfrou",
				"espurr",
				"meowstic",
				"meowstic_female",
				"honedge",
				"doublade",
				"aegislash",
				"_",
				"spritzee",
				"aromatisse",
				"swirlix",
				"slurpuff",
				"inkay",
				"malamar",
				"binacle",
				"barbaracle",
				"skrelp",
				"dragalge",
				"clauncher",
				"clawitzer",
				"helioptile",
				"heliolisk",
				"tyrunt",
				"tyrantrum",
				"amaura",
				"aurorus",
				"sylveon",
				"hawlucha",
				"dedenne",
				"carbink",
				"goomy",
				"sliggoo",
				"goodra",
				"klefki",
				"phantump",
				"trevenant",
				"pumpkaboo",
				"gourgeist",
				"bergmite",
				"avalugg",
				"noibat",
				"noivern",
				"_",
				"_",
				"_",
				"diancie",
				"_",
				"hoopa",
				"_",
				"volcanion",
			};
			SpriteSheetSplitter.AppendMonBlockSprites(monNames, ref pokedexNumber, settings, ref originX, ref originY);

			// Special large legendary format
			//
			settings.CellSize = 64;
			settings.FrameStride = 4;
			settings.FrameNames = new string[]
			{
				$"front{groupKey}_1", $"back{groupKey}_1", $"side{groupKey}_1", $"_",
				$"front{groupKey}_2", $"back{groupKey}_2", $"side{groupKey}_2", $"_"
			};
			SpriteSheetSplitter.AppendMonSprites("xerneas", 716, settings, 0, 1280);
			SpriteSheetSplitter.AppendMonSprites("zygarde", 718, settings, 0, 1280 + 128);

			SpriteSheetSplitter.AppendMonSprites("yveltal", 717, settings, 320, 1280);
			SpriteSheetSplitter.AppendMonSprites("hoopa_unbound", 719, settings, 320, 1280 + 128);
		}
	}
}
