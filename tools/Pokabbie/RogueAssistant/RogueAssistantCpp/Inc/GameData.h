#pragma once
#include "Defines.h"

#include <string>

typedef u32 GameAddress;

class GameAddresses
{
public:
	static const u8 c_GameVersionVanilla = 0;
	static const u8 c_GameVersionEX = 1;

	static const GameAddress c_RomAddress = 0x08000000;
	static const GameAddress c_GFHeaderAddress = c_RomAddress + 0x100;
};

namespace GameHelpers
{
	std::string ParseGameString(u8 const* str, size_t length);
}

namespace GameStructures
{
	struct GFRomHeader
	{
		u32 version;
		u32 language;
		u8 gameName[32];
		GameAddress monFrontPics;
		GameAddress monBackPics;
		GameAddress monNormalPalettes;
		GameAddress monShinyPalettes;
		GameAddress monIcons;
		GameAddress monIconPaletteIds;
		GameAddress monIconPalettes;
		GameAddress monSpeciesNames;
		GameAddress moveNames;
		GameAddress decorations;
		u32 flagsOffset;
		u32 varsOffset;
		u32 pokedexOffset;
		u32 seenOffset;
		u32 unusedSeenOffset; // needed for EX
		u32 pokedexVar;
		u32 pokedexFlag;
		u32 mysteryEventFlag;
		u32 pokedexCount;
		u8 playerNameLength;
		u8 unk2;
		u8 pokemonNameLength1;
		u8 pokemonNameLength2;
		u32 rogueAssistantHandshake1;
		GameAddress rogueAssistantHeader;
		u32 rogueAssistantHandshake2;
		u8 unk17;
		u32 saveBlock2Size;
		u32 saveBlock1Size;
		u32 partyCountOffset;
		u32 partyOffset;
		u32 warpFlagsOffset;
		u32 trainerIdOffset;
		u32 playerNameOffset;
		u32 playerGenderOffset;
		u32 frontierStatusOffset;
		u32 frontierStatusOffset2;
		u32 externalEventFlagsOffset;
		u32 externalEventDataOffset;
		u32 unk18;
		GameAddress baseStats;
		GameAddress abilityNames;
		GameAddress abilityDescriptions;
		GameAddress items;
		GameAddress moves;
		GameAddress ballGfx;
		GameAddress ballPalettes;
		u32 gcnLinkFlagsOffset;
		u32 gameClearFlag;
		u32 ribbonFlag;
		u8 bagCountItems;
		u8 bagCountKeyItems;
		u8 bagCountPokeballs;
		u8 bagCountTMHMs;
		u8 bagCountBerries;
		u8 pcItemsCount;
		u32 pcItemsOffset;
		u32 giftRibbonsOffset;
		u32 enigmaBerryOffset;
		u32 enigmaBerrySize;
		GameAddress moveDescriptions;
		u32 unk20;
	};

	struct RogueAssistantHeader
	{
		u8 rogueVersion;
		u8 rogueDebug;
		u32 assistantConfirmSize;
		u32 assistantConfirmOffset;
		u32 netMultiplayerSize;
		u32 netHandshakeOffset;
		u32 netHandshakeSize;
		u32 netHandshakeStateOffset;
		u32 netHandshakePlayerIdOffset;
		u32 netGameStateOffset;
		u32 netGameStateSize;
		u32 netPlayerProfileOffset;
		u32 netPlayerProfileSize;
		u32 netPlayerStateOffset;
		u32 netPlayerStateSize;
		u32 netPlayerCount;
		u32 netRequestStateOffset;
		u32 netCurrentStateOffset;
		u32 homeLocalBoxCount;
		u32 homeTotalBoxCount;
		u32 homeBoxSize;
		u32 homeMinimalBoxOffset;
		u32 homeMinimalBoxSize;
		u32 homeDestMonOffset;
		u32 homeDestMonSize;
		u32 homeRemoteIndexOrderOffset;
		u32 homeTrainerIdOffset;
		GameAddress saveBlock1Ptr;
		GameAddress saveBlock2Ptr;
		GameAddress rogueBlockPtr;
		GameAddress assistantState;
		GameAddress multiplayerPtr;
		GameAddress homeBoxPtr;
	};

	struct RogueAssistantState
	{
		u8 inCommBuffer[16];
		u8 outCommBuffer[32];
		u16 assistantState;
		u16 assistantSubstate;
		u16 requestState;
	};
};