using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator.OverworldSprites
{
	public static class SpriteSheetSplitter_Gen8
	{
		public static void AppendMonSprites()
		{
			AppendMonSpritesInternal("", "res://hgss_galar_pokemon_overworls_sprites_by_lasse00_df33ses.png");

			if (OverworldSpriteGenerator.s_GenerateShinies)
				AppendMonSpritesInternal("_shiny", "res://hgss_galar_pokemon_overworls_sprites_shiny_by_lasse00_df33sta.png");
		}

		private static void AppendMonSpritesInternal(string groupKey, string sourcePath)
		{
			int pokedexNumber = 810;
			int originX = 0;
			int originY = 0;

			SpriteSheetSplitter.Settings settings = new SpriteSheetSplitter.Settings();
			settings.CategoryName = "gen8" + groupKey;
			settings.CellSize = 32;
			settings.FrameNames = new string[]
			{
				$"front{groupKey}_1",   $"front{groupKey}_2",
				$"back{groupKey}_1",    $"back{groupKey}_2",
				$"side{groupKey}_1",    $"side{groupKey}_2",
				 $"_",                  $"_",
			};
			settings.FrameStride = 2;
			settings.Source = ContentCache.GetImageContent(sourcePath);

			Console.WriteLine($"Gathering '{settings.CategoryName}'");

			string[] monNames = new string[]
			{
				"grookey",
				"thwackey",
				"rillaboom",
				"scorbunny",
				"raboot",
				"cinderace",
				"sobble",
				"drizzile",
				"inteleon",
				"skwovet",
				"greedent",
				"rookidee",
				"corvisquire",
				"corviknight",
				"blipbug",
				"dottler",
				"orbeetle",
				"nickit",
				"thievul",
				"gossifleur",
				"eldegoss",
				"wooloo",
				"dubwool",
				"chewtle",
				"drednaw",
				"yamper",
				"boltund",
				"rolycoly",
				"carkol",
				"coalossal",
				"applin",
				"flapple",
				"appletun",
				"silicobra",
				"sandaconda",
				"cramorant",
				"arrokuda",
				"barraskewda",
				"toxel",
				"toxtricity",
				"toxtricity_low_key",
				"sizzlipede",
				"centiskorch",
				"clobbopus",
				"grapploct",
				"sinistea",
				"polteageist",
				"hatenna",
				"hattrem",
				"hatterene",
				"impidimp",
				"morgrem",
				"grimmsnarl",
				"milcery",
				"alcremie",
				"falinks",
				"pincurchin",
				"snom",
				"frosmoth",
				"stonjourner",
				"eiscue",
				"eiscue_noice_face",
				"indeedee_female",
				"indeedee",
				"morpeko",
				"morpeko_hangry",
				"cufant",
				"copperajah",
				"dracozolt",
				"arctozolt",
				"dracovish",
				"arctovish",
				"duraludon",
				"dreepy",
				"drakloak",
				"dragapult",
				"zacian",
				"zacian_crowned_sword",
				"zamazenta",
				"zamazenta_crowned_shield",
				"kubfu",
				"urshifu",
				"urshifu_rapid_strike_style",
				"zarude",
				"_",
				"regieleki",
				"regidrago",
				"glastrier",
				"_",
				"_",
				"spectrier",
				"calyrex",
				"calyrex_ice_rider",
				"calyrex_shadow_rider",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_",
				"meowth_galarian",
				"perrserker",
				"ponyta_galarian",
				"rapidash_galarian",
				"farfetchd_galarian",
				"sirfetchd",
				"weezing_galarian",
				"mr_mime_galarian",
				"mr_rime",
				"corsola_galarian",
				"cursola",
				"zigzagoon_galarian",
				"linoone_galarian",
				"obstagoon",
				"darumaka_galarian",
				"darmanitan_galarian",
				"darmanitan_zen_mode_galarian",
				"yamask_galarian",
				"runerigus",
				"stunfisk_galarian",
				"slowpoke_galarian",
				"slowbro_galarian",
				"slowking_galarian",
				"articuno_galarian",
				"zapdos_galarian",
				"moltres_galarian"

			};
			SpriteSheetSplitter.AppendMonBlockSprites(monNames, ref pokedexNumber, settings, ref originX, ref originY);

			// Special large format
			//
			settings.CellSize = 64;
			SpriteSheetSplitter.AppendMonSprites("eternatus", pokedexNumber, settings, 448, 1152);
		}
	}
}
