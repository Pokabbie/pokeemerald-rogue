using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator.OverworldSprites
{
	public static class SpriteSheetSplitter_Gen5
	{
		public static void AppendMonSprites()
		{
			AppendMonSpritesInternal("", "res://unova_overworlds_by_hamsterskull_d3cbfbw.png");

			//if (OverworldSpriteGenerator.s_GenerateShinies)
			//	AppendMonSpritesInternal("_shiny", "res://unova_overworlds__shiny__by_2and2makes5_d51nqik.png");
		}

		private static void AppendMonSpritesInternal(string groupKey, string sourcePath)
		{
			int pokedexNumber = 494;
			int originX = 6;
			int originY = 206;
			int lineOffset = originX;
			int blockWidth = 75 - originX + 1;
			int blockHeight = 337 - originY + 1;


			SpriteSheetSplitter.Settings settings = new SpriteSheetSplitter.Settings();
			settings.Gen5BorderRemoval = true;
			settings.Gen5CorrectionMode = (groupKey == "") ? 1 : 2;
			settings.CategoryName = "gen5" + groupKey;
			settings.CellSize = 0; // Should be automatically calculated for non-uniform sprites
			settings.FrameNames = new string[]
			{
				$"back{groupKey}_1", $"side{groupKey}_1",
				$"back{groupKey}_2", $"side{groupKey}_2",
				$"front{groupKey}_1", $"_",
				$"front{groupKey}_2", $"_",
			};
			settings.FrameStride = 2;
			settings.CustomStepX = 84 - originX;
			settings.CustomStepY = 345 - originY;
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
				"victini",
				"snivy",
				"servine",
				"serperior",
				"tepig",
				"pignite",
				"emboar",
				"oshawott",
				"dewott",
				"samurott",
				"patrat",
				"watchog",
				"lillipup",
				"herdier",
				"stoutland",
				"purrloin",
				"liepard",
				"pansage",
				"simisage",
				"pansear",
				"simisear",
				"panpour",
				"simipour",
				"munna",
				"musharna",
				"pidove",
				"tranquill",
				"unfezant",
				"blitzle",
				"zebstrika",
				"roggenrola",
				"boldore",
				"gigalith",
				"woobat",
				"swoobat",
				"drilbur",
				"excadrill",
				"audino",
				"timburr",
				"gurdurr",
				"conkeldurr",
				"tympole",
				"palpitoad",
				"seismitoad",
				"throh",
				"sawk",
				"sewaddle",
				"swadloon",
				"leavanny",
				"venipede",
				"whirlipede",
				"scolipede",
				"cottonee",
				"_",  // wrong look direction (see below)
				"petilil",
				"lilligant",
				"basculin",
				"sandile",
				"krokorok",
				"krookodile",
				"darumaka",
				"darmanitan",
				"maractus",
				"dwebble",
				"crustle",
				"scraggy",
				"scrafty",
				"sigilyph",
				"yamask",
				"cofagrigus",
				"tirtouga",
				"carracosta",
				"archen",
				"archeops",
				"trubbish",
				"garbodor",
				"zorua",
				"zoroark",
				"minccino",
				"cinccino",
				"gothita",
				"gothorita",
				"gothitelle",
				"solosis",
				"duosion",
				"reuniclus",
				"ducklett",
				"swanna",
				"vanillite",
				"vanillish",
				"vanilluxe",
				"deerling",
				"sawsbuck",
				"emolga",
				"karrablast",
				"_",  // wrong look direction (see below)
				"foongus",
				"amoonguss",
				"_",  // wrong look direction (see below)
				"_",  // wrong look direction (see below)
				"alomomola",
				"joltik",
				"galvantula",
				"_",  // wrong look direction (see below)
				"_",  // wrong look direction (see below)
				"klink",
				"klang",
				"klinklang",
				"tynamo",
				"eelektrik",
				"eelektross",
				"_",  // wrong look direction (see below)
				"_",  // wrong look direction (see below)
				"_",  // wrong look direction (see below)
				"_",  // wrong look direction (see below)
				"_",  // wrong look direction (see below)
				"axew",
				"fraxure",
				"haxorus",
				"cubchoo",
				"beartic",
				"cryogonal",
				"shelmet",
				"accelgor",
				"stunfisk",
				"mienfoo",
				"mienshao",
				"_",  // wrong look direction (see below)
				"golett",
				"golurk",
				"pawniard",
				"bisharp",
				"bouffalant",
				"rufflet",
				"braviary",
				"vullaby",
				"mandibuzz",
				"heatmor",
				"durant",
				"deino",
				"zweilous",
				"_",  // wrong look direction (see below)
				"larvesta",
				"volcarona",
				"cobalion",
				"terrakion",
				"virizion",
				"tornadus",
				"thundurus",
			};
			SpriteSheetSplitter.AppendNonUniformMonBlockSprites(monNames, ref pokedexNumber, settings, lineOffset, ref originX, ref originY, blockWidth, blockHeight);

			monNames = new string[]
			{
				"landorus"
			};
			originX = 552;
			originY = 1597;
			SpriteSheetSplitter.AppendNonUniformMonBlockSprites(monNames, ref pokedexNumber, settings, lineOffset, ref originX, ref originY, blockWidth, blockHeight);

			monNames = new string[]
			{
				"keldeo",
				"meloetta",
				"_",  // wrong look direction (see below)
			};
			originX = 900;
			originY = 1596;
			SpriteSheetSplitter.AppendNonUniformMonBlockSprites(monNames, ref pokedexNumber, settings, lineOffset, ref originX, ref originY, blockWidth, blockHeight);
			
			monNames = new string[]
			{
				"_",
				"basculin_blue_striped",
				"deerling_summer",
				"deerling_autumn",
				"deerling_winter",
				"sawsbuck_summer",
				"sawsbuck_autumn",
				"sawsbuck_winter",
				"_",
				"_",
				"meloetta_pirouette",
			};
			originX = 7;
			originY = 1736;
			SpriteSheetSplitter.AppendNonUniformMonBlockSprites(monNames, ref pokedexNumber, settings, lineOffset, ref originX, ref originY, blockWidth, blockHeight);

			// These sprites were flipped for some reason
			// (But not in the shiny set)
			//
			if (groupKey == "")
			{
				settings.FrameNames = new string[]
				{
					$"back{groupKey}_1", $"_",
					$"back{groupKey}_2", $"_",
					$"front{groupKey}_1", $"side{groupKey}_1",
					$"front{groupKey}_2", $"side{groupKey}_2",
				};
			}
			monNames = new string[]
			{
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
				"whimsicott",
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
				"_",
				"_",
				"_",
				"_",
				"_",
				"escavalier",
				"_",
				"_",
				"frillish", 
				"jellicent",
				"_",
				"_",
				"_",
				"ferroseed",
				"ferrothorn",
				"_",
				"_",
				"_",
				"_",
				"_",
				"_",
				"elgyem",
				"beheeyem",
				"litwick", 
				"lampent", // ?? in it's own order for non shiny
				"chandelure",
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
				"druddigon",
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
				"hydreigon",
			};


			if (groupKey == "")
			{
				monNames[Array.IndexOf(monNames, "lampent")] = "_";
			}

			originX = 6;
			originY = 206;
			SpriteSheetSplitter.AppendNonUniformMonBlockSprites(monNames, ref pokedexNumber, settings, lineOffset, ref originX, ref originY, blockWidth, blockHeight);

			monNames = new string[]
			{
				"_",
				"_",
				"genesect"
			};
			originX = 900;
			originY = 1596;
			SpriteSheetSplitter.AppendNonUniformMonBlockSprites(monNames, ref pokedexNumber, settings, lineOffset, ref originX, ref originY, blockWidth, blockHeight);


			// Is in it's own weird order for non shiny
			if (groupKey == "")
			{
				monNames = new string[]
				{
					"lampent",
				};
				settings.FrameNames = new string[]
				{
					$"front{groupKey}_2", $"front{groupKey}_1",
					$"back{groupKey}_1", $"back{groupKey}_2",
					$"side{groupKey}_1", $"side{groupKey}_2",
					"_", "_",
				};
				originX = 708;
				originY = 1179;
				SpriteSheetSplitter.AppendNonUniformMonBlockSprites(monNames, ref pokedexNumber, settings, lineOffset, ref originX, ref originY, blockWidth, blockHeight);
			}


			// Special large format
			//
			settings.FrameNames = new string[]
			{
				$"front{groupKey}_1", $"side{groupKey}_1", "_", $"back{groupKey}_1",
				$"front{groupKey}_2", $"side{groupKey}_2", "_", $"back{groupKey}_2",
			};
			settings.FrameStride = 4;
			SpriteSheetSplitter.AppendNonUniformMonSprites("reshiram", 643, settings, new Rectangle(6, 1597, 268 - 6, 1728 - 1597));
			SpriteSheetSplitter.AppendNonUniformMonSprites("zekrom", 644, settings, new Rectangle(279, 1597, 268 - 6, 1728 - 1597));
			SpriteSheetSplitter.AppendNonUniformMonSprites("kyurem", 645, settings, new Rectangle(631, 1596, 891 - 631, 1727 - 1596));
		}
	}
}
