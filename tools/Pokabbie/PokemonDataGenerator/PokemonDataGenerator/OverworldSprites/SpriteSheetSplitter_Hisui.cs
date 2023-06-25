using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator.OverworldSprites
{
	public static class SpriteSheetSplitter_Hisui
	{
		public static void AppendMonSprites()
		{
			AppendMonSpritesInternal("", "res://hgss_hisuian_pokemon_overworld_sprites_by_darkusshadow_df16oub.png");

			if (OverworldSpriteGenerator.s_GenerateShinies)
				AppendMonSpritesInternal("_shiny", "res://hgss_hisuian_pokemon_overworld_sprites_shiny_by_lasse00_df33tfc.png");
		}

		private static void AppendMonSpritesInternal(string groupKey, string sourcePath)
		{
			int pokedexNumber = 810;
			int originX = 0;
			int originY = 0;

			SpriteSheetSplitter.Settings settings = new SpriteSheetSplitter.Settings();
			settings.CategoryName = "hisui" + groupKey;
			settings.CellSize = 32;
			settings.FrameNames = new string[]
			{
				$"front{groupKey}_1",   $"front{groupKey}_2",
				$"side{groupKey}_1",    $"side{groupKey}_2",
				 $"_",                  $"_",
				$"back{groupKey}_1",    $"back{groupKey}_2",
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
				"decidueye_hisuian",
				"typhlosion_hisuian",
				"samurott_hisuian",
				"wyrdeer",
				"kleavor",
				"qwilfish_hisuian",
				"overqwil",
				"lilligant_hisuian",
				"ursaluna",
				"sliggoo_hisuian",
				"goodra_hisuian",
				"basculin_white_striped",
			};
			SpriteSheetSplitter.AppendMonBlockSprites(monNames, ref pokedexNumber, settings, ref originX, ref originY);

			// These are flipped in shiny set
			if (groupKey != "")
			{
				settings.FrameNames = new string[]
				{
					$"front{groupKey}_1",   $"front{groupKey}_2",
					 $"_",                  $"_",
					$"side{groupKey}_1",    $"side{groupKey}_2",
					$"back{groupKey}_1",    $"back{groupKey}_2",
				};
			}

			monNames = new string[]
			{
				"basculegion",
				"basculegion_female",
			};
			SpriteSheetSplitter.AppendMonBlockSprites(monNames, ref pokedexNumber, settings, ref originX, ref originY);

			// Back to normal
			settings.FrameNames = new string[]
			{
				$"front{groupKey}_1",   $"front{groupKey}_2",
				$"side{groupKey}_1",    $"side{groupKey}_2",
				 $"_",                  $"_",
				$"back{groupKey}_1",    $"back{groupKey}_2",
			};

			monNames = new string[]
			{
				"growlithe_hisuian",
				"arcanine_hisuian",
				"voltorb_hisuian",
				"electrode_hisuian",
				"sneasel_hisuian",
				"sneasler",
			};
			SpriteSheetSplitter.AppendMonBlockSprites(monNames, ref pokedexNumber, settings, ref originX, ref originY);


			// These are flipped in shiny set
			if (groupKey != "")
			{
				settings.FrameNames = new string[]
				{
					$"front{groupKey}_1",   $"front{groupKey}_2",
					 $"_",                  $"_",
					$"side{groupKey}_1",    $"side{groupKey}_2",
					$"back{groupKey}_1",    $"back{groupKey}_2",
				};
			}

			monNames = new string[]
			{
				"avalugg_hisuian",
			};
			SpriteSheetSplitter.AppendMonBlockSprites(monNames, ref pokedexNumber, settings, ref originX, ref originY);
			
			// Back to normal
			settings.FrameNames = new string[]
			{
				$"front{groupKey}_1",   $"front{groupKey}_2",
				$"side{groupKey}_1",    $"side{groupKey}_2",
				 $"_",                  $"_",
				$"back{groupKey}_1",    $"back{groupKey}_2",
			};

			monNames = new string[]
			{
				"zorua_hisuian",
				"zoroark_hisuian",
			};
			SpriteSheetSplitter.AppendMonBlockSprites(monNames, ref pokedexNumber, settings, ref originX, ref originY);

			// These are flipped in shiny set
			if (groupKey != "")
			{
				settings.FrameNames = new string[]
				{
					$"front{groupKey}_1",   $"front{groupKey}_2",
					 $"_",                  $"_",
					$"side{groupKey}_1",    $"side{groupKey}_2",
					$"back{groupKey}_1",    $"back{groupKey}_2",
				};
			}

			monNames = new string[]
			{
				"braviary_hisuian",
			};
			SpriteSheetSplitter.AppendMonBlockSprites(monNames, ref pokedexNumber, settings, ref originX, ref originY);

			settings.CellSize = 64;
			monNames = new string[]
			{
				"enamorus",
				"enamorus_therian",
			};
			SpriteSheetSplitter.AppendMonBlockSprites(monNames, ref pokedexNumber, settings, ref originX, ref originY);

			// Back to normal
			settings.FrameNames = new string[]
			{
				$"front{groupKey}_1",   $"front{groupKey}_2",
				$"side{groupKey}_1",    $"side{groupKey}_2",
				 $"_",                  $"_",
				$"back{groupKey}_1",    $"back{groupKey}_2",
			};

			monNames = new string[]
			{
				"dialga_origin",
				"palkia_origin",
			};
			SpriteSheetSplitter.AppendMonBlockSprites(monNames, ref pokedexNumber, settings, ref originX, ref originY);
		}
	}
}
