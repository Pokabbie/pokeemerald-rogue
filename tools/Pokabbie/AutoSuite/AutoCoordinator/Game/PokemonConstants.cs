using System;
using System.Collections.Generic;
using System.Text;

namespace AutoCoordinator.Game
{
	public enum PokemonDataID
	{
		Personality		= 0,
		//..
		Species			= 11,
		HeldItem,
		Move1,
		Move2,
		Move3,
		Move4,
		PP1,
		PP2,
		PP3,
		PP4,
		//..
		HP_EV			= 26,
		ATK_EV,
		DEF_EV,
		SPEED_EV,
		SPATK_EV,
		SPDEF_EV,
		Friendship,
		//..
		HP_IV			= 39,
		ATK_IV,
		DEF_IV,
		SPEED_IV,
		SPATK_IV,
		SPDEF_IV,
		//..
		AbilityNum		= 46,
		//..
		Level			= 56,
		HP,
		MaxHP,
		ATK,
		DEF,
		SPEED,
		SPATK,
		SPDEF,
	}

	public enum PokemonFlagID
	{
		RogueRunActive = 0x20,
		RogueExpAll,
		SeedEnabled,
		SeedItems,
		SeedTrainers,
		SeedBosses,
		SeedWildMons,
		RogueDoubleBattles,
		RogueCanOverLvl,
		RogueHardTrainers,
		//..
		RogueEasyTrainers = 0x266,
		RogueEasyItems,
		RogueHardItems,
		//..
		RogueEVsEnabled = 0x26B,
		//..
		RogueGauntletMode = 0x26E,
		RogueExpansionActive,
		RogueRareItemMartDisabled,
		RogueRandomTradeDisabled,
		RogueRainbowMode,
		//..
		RogueForceBasicBag = 0x276,
		RogueQuestsAskForRetire,
		RogueQuestsNeverAskForRetire,
		RogueHoennRoutes,
		RogueHoennBosses,
		RogueKantoRoutes,
		RogueKantoBosses,
		RogueJohtoRoutes,
		RogueJohtoBosses,
		RogueEasyLegendaries,
		RogueHardLegendaries,
	}

	public enum PokemonVarID
	{
		RogueDifficulty = 0x407D,
		RogueFurthestDifficulty,
		//..
		RogueSkipToDifficulty = 0x409D,
		//..
		RogueEnabledGenLimit = 0x40A8,
		//..
		RogueCurrentLevelCap = 0x40BB,
		//..
		RogueRegionDexLimit = 0x40DC,
		//..
		DesiredCampaign = 0x40F8,
		ActiveCampaign,
	}

	public enum PokemonMapLayoutID
	{ 
		Unknown = 0,
		RogueHub = 442,
		RogueHubTransition,
		//..
		RogueBoss0 = 447,
		//..
		RogueBoss8 = 449,
		RogueBoss9,
		RogueBoss10,
		RogueBoss11,
		RogueBoss12,
		RogueBoss13,
		//..
		RogueAdventurePaths = 480,
		//..
		RogueHubAdventureEntrance = 491
	}

	public enum RogueMapID
	{
		RogueHub					= (1 | (34 << 8)),
		RogueHubTransition			= (2 | (34 << 8)),
		RogueHubAdventureEntrance	= (7 | (34 << 8)),

		RogueAdventurePaths			= (0 | (35 << 8)),
		RogueBoss0					= (24 | (35 << 8)),
		RogueBoss8					= (25 | (35 << 8)),
		RogueBoss9					= (26 | (35 << 8)),
		RogueBoss10					= (27 | (35 << 8)),
		RogueBoss11					= (28 | (35 << 8)),
		RogueBoss12					= (29 | (35 << 8)),
		RogueBoss13					= (30 | (35 << 8)),
	}

	public static class RogueMapIDMethods
	{
		public static int ToMapGroup(this RogueMapID mapId)
		{
			return (byte)(((int)mapId) >> 8);
		}

		public static int ToMapNum(this RogueMapID mapId)
		{
			return (byte)(((int)mapId) & 0xFF);
		}
	}
}
