using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator.OverworldSprites
{
	public static class SpriteSheetSplitter_Gen7
	{
		public static void AppendMonSprites()
		{
			AppendMonSpritesInternal("", "res://gen_7__alola__overworld_sprites_by_larryturbo_ddbjpi0.png");

			if (OverworldSpriteGenerator.s_GenerateShinies)
				AppendMonSpritesInternal("_shiny", "res://gen_7__alola__shiny_overworld_sprites_by_larryturbo_dddtd76.png");
		}

		private static void AppendMonSpritesInternal(string groupKey, string sourcePath)
		{
			int pokedexNumber = 722;
			int originX = 0;
			int originY = 0;

			SpriteSheetSplitter.Settings settings = new SpriteSheetSplitter.Settings();
			settings.CategoryName = "gen7" + groupKey;
			settings.CellSize = 32;
			settings.FrameNames = new string[]
			{
				$"front{groupKey}_1",	$"front{groupKey}_2",
				$"side{groupKey}_1",	$"side{groupKey}_2",
				 $"_",					$"_",
				$"back{groupKey}_1",	$"back{groupKey}_2"
			};
			settings.FrameStride = 2;
			settings.AutomaticCentredFrames = new Tuple<string, string>[]
			{
				new Tuple<string, string>($"front{groupKey}_1", $"front{groupKey}_2"),
				new Tuple<string, string>($"back{groupKey}_1", $"back{groupKey}_2"),
				new Tuple<string, string>($"side{groupKey}_1", $"side{groupKey}_2"),
			};
			settings.Source = ContentCache.GetImageContent(sourcePath);

			Console.WriteLine($"Gathering '{settings.CategoryName}'");

			string[] monNames = new string[]
			{
				"rowlet",
				"dartrix",
				"decidueye",
				"litten",
				"torracat",
				"incineroar",
				"popplio",
				"brionne",
				"primarina",
				"pikipek",
				"trumbeak",
				"toucannon",
				"yungoos",
				"gumshoos",
				"grubbin",
				"charjabug",
				"vikavolt",
				"crabrawler",
				"crabominable",
				"oricorio",
				"oricorio_pom_pom",
				"oricorio_pau",
				"oricorio_sensu",
				"cutiefly",
				"ribombee",
				"rockruff",
				"lycanroc",
				"lycanroc_midnight",
				"lycanroc_dusk",
				"wishiwashi",
				"_",
				"mareanie",
				"toxapex",
				"mudbray",
				"mudsdale",
				"dewpider",
				"araquanid",
				"fomantis",
				"lurantis",
				"morelull",
				"shiinotic",
				"salandit",
				"salazzle",
				"stufful",
				"bewear",
				"bounsweet",
				"steenee",
				"tsareena",
				"comfey",
				"oranguru",
				"passimian",
				"wimpod",
				"golisopod",
				"sandygast",
				"palossand",
				"pyukumuku",
				"type_null",
				"silvally",
				"minior",
				"komala",
				"turtonator",
				"togedemaru",
				"mimikyu",
				"_",
				"bruxish",
				"drampa",
				"dhelmise",
				"jangmo_o",
				"hakamo_o",
				"kommo_o",
				"tapu_koko",
				"tapu_lele",
				"tapu_bulu",
				"tapu_fini",
				"cosmog",
				"cosmoem",
				"solgaleo",
				"lunala",
				"nihilego",
				"buzzwole",
				"pheromosa",
				"xurkitree",
				"celesteela",
				"kartana",
				"guzzlord",
				"necrozma",
				"necrozma_dusk_mane",
				"necrozma_dawn_wings",
				"necrozma_ultra",
				"magearna",
				"marshadow",
				"poipole",
				"naganadel",
				"stakataka",
				"blacephalon",
				"zeraora",
				"meltan",
				"melmetal",
				"marowak_alolan",
				"rattata_alolan",
				"raticate_alolan",
				"raichu_alolan",
				"sandshrew_alolan",
				"sandslash_alolan",
				"vulpix_alolan",
				"ninetales_alolan",
				"diglett_alolan",
				"dugtrio_alolan",
				"meowth_alolan",
				"persian_alolan",
				"geodude_alolan",
				"graveler_alolan",
				"golem_alolan",
				"grimer_alolan",
				"muk_alolan",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_", //"silvally_grass",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_",
				"zygarde_10",
				"zygarde_complete",
			};
			SpriteSheetSplitter.AppendMonBlockSprites(monNames, ref pokedexNumber, settings, ref originX, ref originY);

			// Special large format
			//
			settings.CellSize = 64;
			SpriteSheetSplitter.AppendMonSprites("exeggutor_alolan", pokedexNumber, settings, 448, 1536);
		}
	}
}
