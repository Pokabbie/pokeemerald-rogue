using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator.OverworldSprites
{
	public static class SpriteSheetSplitter_Gen9
	{
		public static void AppendMonSprites()
		{
			AppendMonSpritesInternal("", "res://_wip__gen_9__paldea__pokemon_overworld_sprites_by_darkusshadow_dg06t9u.png");

			if (OverworldSpriteGenerator.s_GenerateShinies)
				AppendMonSpritesInternal("_shiny", "res://shiny_gen_9__paldea__pokemon_overworld_sprites_by_darkusshadow_dg06vh7.png");
		}

		private static void AppendMonSpritesInternal(string groupKey, string sourcePath)
		{
			int pokedexNumber = 810;
			int originX = 0;
			int originY = 0;

			SpriteSheetSplitter.Settings settings = new SpriteSheetSplitter.Settings();
			settings.CategoryName = "paldea" + groupKey;
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
				"SPRIGATITO",
				"FLORAGATO",
				"MEOWSCARADA",
				"FUECOCO",
				"CROCALOR",
				"SKELEDIRGE",
				"QUAXLY",
				"QUAXWELL",
				"QUAQUAVAL",
				"LECHONK",
				"OINKOLOGNE_MALE",
				"OINKOLOGNE_FEMALE",
				"TAROUNTULA",
				"SPIDOPS",
				"NYMBLE",
				"LOKIX",
				"PAWMI",
				"PAWMO",
				"PAWMOT",
				"TANDEMAUS",
				"MAUSHOLD_FAMILY_OF_FOUR",
				"MAUSHOLD_FAMILY_OF_THREE",
				"FIDOUGH",
				"DACHSBUN",
				"SMOLIV",
				"DOLLIV",
				"ARBOLIVA",
				"SQUAWKABILLY_GREEN_PLUMAGE",
				"SQUAWKABILLY_BLUE_PLUMAGE",
				"SQUAWKABILLY_YELLOW_PLUMAGE",
				"SQUAWKABILLY_WHITE_PLUMAGE",
				"NACLI",
				"NACLSTACK",
				"GARGANACL",
				"CHARCADET",
				"ARMAROUGE",
				"CERULEDGE",
				"TADBULB",
				"BELLIBOLT",
				"WATTREL",
				"KILOWATTREL",
				"MASCHIFF",
				"MABOSSTIFF",
				"SHROODLE",
				"GRAFAIAI",
				"BRAMBLIN",
				"BRAMBLEGHAST",
				"TOEDSCOOL",
				"TOEDSCRUEL",
				"KLAWF",
				"CAPSAKID",
				"SCOVILLAIN",
				"RELLOR",
				"RABSCA",
				"FLITTLE",
				"ESPATHRA",
				"TINKATINK",
				"TINKATUFF",
				"TINKATON",
				"WIGLETT",
				"WUGTRIO",
				"BOMBIRDIER",
				"FINIZEN",
				"PALAFIN_ZERO",
				"PALAFIN_HERO",
				"VAROOM",
				"REVAVROOM",
				"_", // alt for CYCLIZAR
				"CYCLIZAR",
				"ORTHWORM",
				"GLIMMET",
				"GLIMMORA",
				"GREAVARD",
				"HOUNDSTONE",
				"FLAMIGO",
				"CETODDLE",
				"CETITAN",
				"VELUZA",
				"_", // SPECIES_DONDOZO
				"_", // SPECIES_DONDOZO
				"_", // SPECIES_DONDOZO
				"_", // SPECIES_DONDOZO
				"TATSUGIRI_CURLY",
				"TATSUGIRI_DROOPY",
				"TATSUGIRI_STRETCHY",
				"ANNIHILAPE",
				"CLODSIRE",
				"FARIGIRAF",
				"DUDUNSPARCE_TWO_SEGMENT",
				"_", // SPECIES_DUDUNSPARCE_THREE_SEGMENT
				"_", // SPECIES_DUDUNSPARCE_THREE_SEGMENT
				"_", // SPECIES_DUDUNSPARCE_THREE_SEGMENT
				"_", // SPECIES_DUDUNSPARCE_THREE_SEGMENT
				"_", // SPECIES_KINGAMBIT
				"_", // SPECIES_KINGAMBIT
				"_", // SPECIES_KINGAMBIT
				"_", // SPECIES_KINGAMBIT
				"GREAT_TUSK",
				"SCREAM_TAIL",
				"BRUTE_BONNET",
				"FLUTTER_MANE",
				"SLITHER_WING",
				"SANDY_SHOCKS",
				"IRON_TREADS",
				"IRON_BUNDLE",
				"IRON_HANDS",
				"IRON_JUGULIS",
				"IRON_MOTH",
				"IRON_THORNS",
				"FRIGIBAX",
				"ARCTIBAX",
				"BAXCALIBUR",
				"_", // GIMMIGHOUL_ROAMING
				"GIMMIGHOUL_CHEST",
				"_", // alt for SPECIES_GIMMIGHOUL_CHEST
				"GHOLDENGO",
				"WO_CHIEN",
				"CHIEN_PAO",
				"TING_LU",
				"CHI_YU",
				"ROARING_MOON",
				"IRON_VALIANT",
				"_", // SPECIES_KORAIDON
				"_", // SPECIES_KORAIDON
				"_", // SPECIES_KORAIDON
				"_", // SPECIES_KORAIDON
				"_", // SPECIES_MIRAIDON
				"_", // SPECIES_MIRAIDON
				"_", // SPECIES_MIRAIDON
				"_", // SPECIES_MIRAIDON
				"WALKING_WAKE",
				"IRON_LEAVES",
				"DIPPLIN",
				"POLTCHAGEIST",
				"SINISTCHA",
				"OKIDOGI",
				"MUNKIDORI",
				"FEZANDIPITI",
				"_", // alt for SPECIES_OGERPON_TEAL_MASK
				"OGERPON_TEAL_MASK",
				"_", // alt for SPECIES_OGERPON_WELLSPRING_MASK
				"OGERPON_WELLSPRING_MASK",
				"_", // alt for SPECIES_OGERPON_HEARTHFLAME_MASK
				"OGERPON_HEARTHFLAME_MASK",
				"_", // alt for SPECIES_OGERPON_CORNERSTONE_MASK
				"OGERPON_CORNERSTONE_MASK",
				"ARCHALUDON",
				"_", // SPECIES_HYDRAPPLE
				"_", // SPECIES_HYDRAPPLE
				"_", // SPECIES_HYDRAPPLE
				"_", // SPECIES_HYDRAPPLE
				"GOUGING_FIRE",
				"_", // SPECIES_RAGING_BOLT
				"_", // SPECIES_RAGING_BOLT
				"_", // SPECIES_RAGING_BOLT
				"_", // SPECIES_RAGING_BOLT
				"IRON_BOULDER",
				"IRON_CROWN",
				"TERAPAGOS_NORMAL",
				"TERAPAGOS_TERASTAL",
				"PECHARUNT",
				"_", // alt for SPECIES_PECHARUNT
				"TAUROS_PALDEAN_COMBAT_BREED",
				"TAUROS_PALDEAN_BLAZE_BREED",
				"TAUROS_PALDEAN_AQUA_BREED",
				"WOOPER_PALDEAN",
				"URSALUNA_BLOODMOON",
			};
			SpriteSheetSplitter.AppendMonBlockSprites(monNames, ref pokedexNumber, settings, ref originX, ref originY);


			// Special large format
			//
			settings.CellSize = 64;
			settings.FrameNames = new string[]
			{
				$"front{groupKey}_1",   $"front{groupKey}_2", $"side{groupKey}_1",    $"side{groupKey}_2",
				$"_", $"_",  $"back{groupKey}_1",    $"back{groupKey}_2",
			};
			settings.FrameStride = 4;
			SpriteSheetSplitter.AppendMonSprites("DONDOZO", pokedexNumber++, settings, 512, 640);

			settings.FrameNames = new string[]
			{
				$"front{groupKey}_1",   $"front{groupKey}_2", $"_",    $"_",
				$"side{groupKey}_1",    $"side{groupKey}_2", $"back{groupKey}_1",    $"back{groupKey}_2",
			};
			monNames = new string[]
			{
				"DUDUNSPARCE_THREE_SEGMENT",
				"KINGAMBIT",
			};
			originX = 320;
			originY = 768;
			SpriteSheetSplitter.AppendMonBlockSprites(monNames, ref pokedexNumber, settings, ref originX, ref originY);

			monNames = new string[]
			{
				"KORAIDON",
				"MIRAIDON",
			};
			originX = 640;
			originY = 1024;
			SpriteSheetSplitter.AppendMonBlockSprites(monNames, ref pokedexNumber, settings, ref originX, ref originY);

			monNames = new string[]
			{
				"HYDRAPPLE",
			};
			originX = 448;
			originY = 1280;
			SpriteSheetSplitter.AppendMonBlockSprites(monNames, ref pokedexNumber, settings, ref originX, ref originY);

			monNames = new string[]
			{
				"RAGING_BOLT",
			};
			originX = 768;
			originY = 1280;
			SpriteSheetSplitter.AppendMonBlockSprites(monNames, ref pokedexNumber, settings, ref originX, ref originY);
		}
	}
}
