#ifndef GUARD_ROGUE_H
#define GUARD_ROGUE_H

struct RogueRunData
{
    u16 currentRoomIdx;
};

struct RogueHubData
{
    u8 playerPartyCount;
    struct Pokemon playerParty[PARTY_SIZE];
    u32 money;
    u16 registeredItem;
    struct ItemSlot bagPocket_Items[BAG_ITEMS_COUNT];
    struct ItemSlot bagPocket_KeyItems[BAG_KEYITEMS_COUNT];
    struct ItemSlot bagPocket_PokeBalls[BAG_POKEBALLS_COUNT];
    struct ItemSlot bagPocket_TMHM[BAG_TMHM_COUNT];
    struct ItemSlot bagPocket_Berries[BAG_BERRIES_COUNT];
};

struct SpeciesTable
{
    u8 wildSpeciesCount;
    const u16* wildSpecies;
    u8 trainerSpeciesCount;
    const u16* trainerSpecies;
    ///*0x00*/ u8 partyFlags;
    ///*0x01*/ u8 trainerClass;
    ///*0x02*/ u8 encounterMusic_gender; // last bit is gender
    ///*0x03*/ u8 trainerPic;
    ///*0x04*/ u8 trainerName[12];
    ///*0x10*/ u16 items[MAX_TRAINER_ITEMS];
    ///*0x18*/ bool8 doubleBattle;
    ///*0x1C*/ u32 aiFlags;
    ///*0x20*/ u8 partySize;
    ///*0x24*/ union TrainerMonPtr party;
};

struct RogueMonPreset
{
    u16 heldItem;
    u16 abilityNum;
    u16 moves[MAX_MON_MOVES];
};

struct RogueMonPresetCollection
{
    u16 presetCount;
    const struct RogueMonPreset* presets;
};

#endif  // GUARD_ROGUE_H
