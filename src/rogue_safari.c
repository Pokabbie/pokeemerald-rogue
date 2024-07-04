#include "global.h"
#include "constants/items.h"
#include "constants/layouts.h"

#include "event_data.h"
#include "event_object_movement.h"
#include "random.h"
#include "string_util.h"

#include "rogue_controller.h"
#include "rogue_followmon.h"
#include "rogue_gifts.h"
#include "rogue_pokedex.h"
#include "rogue_save.h"
#include "rogue_safari.h"

#define INVALID_SAFARI_MON_IDX 255

struct SafariData
{
    u8 spawnIndex;
    u8 pendingBattleIdx;
    u8 slotToIndexMap[FOLLOWMON_MAX_SPAWN_SLOTS];
};

static EWRAM_DATA struct SafariData sSafariData = {0};

static void ZeroSafariMon(struct RogueSafariMon* mon);
static u8 AllocSafariMonSlotFor(struct BoxPokemon* mon);
static u8 FreeSafariMonSlotCount();
static u8 AllocCustomMonSafariSlot(u32 customMonId, u8 forSafariIndex);

static void PushBoxMonInternal(struct BoxPokemon* monToCopy, bool32 isLowPriority)
{
    u32 customMonId;
    u8 index = AllocSafariMonSlotFor(monToCopy);
    struct RogueSafariMon* writeMon = &gRogueSaveBlock->safariMons[index];

    ZeroSafariMon(writeMon);
    RogueSafari_CopyToSafariMon(monToCopy, writeMon);

    customMonId = RogueGift_GetCustomBoxMonId(monToCopy);
    if(customMonId)
    {
        writeMon->customMonLookup = 1 + AllocCustomMonSafariSlot(customMonId, index);
    }

    // Default priority to max mons so most of the time we're going to free in the order they we last caught
    writeMon->priorityCounter = ROGUE_SAFARI_TOTAL_MONS;

    if(isLowPriority)
    {
        // We're only gonna keep this mon if it's a shiny
        writeMon->priorityCounter = 0;
    }
    else
    {
        u8 metLevel = GetBoxMonData(monToCopy, MON_DATA_MET_LEVEL);
        u8 currentLevel = GetBoxMonData(monToCopy, MON_DATA_LEVEL);
        u8 levelsGained = currentLevel - metLevel;

        // For each 5 levels we raise priority by 2
        // if we caught a mon at the very begining and took it to the end it would have +40 priority
        writeMon->priorityCounter = (levelsGained * 2) / 5;
    }

    if(writeMon->shinyFlag)
    {
        // Shinies will last much longer than regular mons
        writeMon->priorityCounter += ROGUE_SAFARI_TOTAL_MONS / 2;
    }
}

void RogueSafari_PushMon(struct Pokemon* mon)
{
    if(!mon->rogueExtraData.isSafariIllegal)
    {
        PushBoxMonInternal(&mon->box, FALSE);

        // Just in case we somehow try to add this mon twice, don't
        mon->rogueExtraData.isSafariIllegal = TRUE;
    }
}

void RogueSafari_PushLowPriorityMon(struct Pokemon* mon)
{
    if(!mon->rogueExtraData.isSafariIllegal)
    {
        PushBoxMonInternal(&mon->box, TRUE);

        // Just in case we somehow try to add this mon twice, don't
        mon->rogueExtraData.isSafariIllegal = TRUE;
    }
}

void RogueSafari_PushBoxMon(struct BoxPokemon* monToCopy)
{
    PushBoxMonInternal(monToCopy, FALSE);
}

static void ZeroSafariMon(struct RogueSafariMon* mon)
{
    memset(mon, 0, sizeof(struct RogueSafariMon));
    mon->nickname[0] = 0xFF;
}

void RogueSafari_ResetSpawns()
{
    u8 i;

    sSafariData.spawnIndex = Random() % ROGUE_SAFARI_TOTAL_MONS;
    sSafariData.pendingBattleIdx = INVALID_SAFARI_MON_IDX;

    for(i = 0; i < ARRAY_COUNT(sSafariData.slotToIndexMap); ++i)
    {
        sSafariData.slotToIndexMap[i] = INVALID_SAFARI_MON_IDX;
    }
}

#define COPY_MON_DATA(param, data) \
    temp = fromMon->param; \
    SetBoxMonData(toMon, data, &temp);

void RogueSafari_CopyFromSafariMon(struct RogueSafariMon* fromMon, struct BoxPokemon* toMon)
{
    u32 temp;

    COPY_MON_DATA(species, MON_DATA_SPECIES);

    COPY_MON_DATA(hpIV, MON_DATA_HP_IV);
    COPY_MON_DATA(attackIV, MON_DATA_ATK_IV);
    COPY_MON_DATA(defenseIV, MON_DATA_DEF_IV);
    COPY_MON_DATA(speedIV, MON_DATA_SPEED_IV);
    COPY_MON_DATA(spAttackIV, MON_DATA_SPATK_IV);
    COPY_MON_DATA(spDefenseIV, MON_DATA_SPDEF_IV);

    COPY_MON_DATA(pokeball, MON_DATA_POKEBALL);
    COPY_MON_DATA(abilityNum, MON_DATA_ABILITY_NUM);
    COPY_MON_DATA(genderFlag, MON_DATA_GENDER_FLAG);
    COPY_MON_DATA(shinyFlag, MON_DATA_IS_SHINY);

    SetBoxMonData(toMon, MON_DATA_NICKNAME, fromMon->nickname);
    SetNatureBoxMon(toMon, fromMon->nature);
}

#undef COPY_MON_DATA

#define COPY_MON_DATA(param, data) \
    temp = GetBoxMonData(fromMon, data, NULL); \
    toMon->param = temp

void RogueSafari_CopyToSafariMon(struct BoxPokemon* fromMon, struct RogueSafariMon* toMon)
{
    u32 temp;

    COPY_MON_DATA(species, MON_DATA_SPECIES);

    COPY_MON_DATA(hpIV, MON_DATA_HP_IV);
    COPY_MON_DATA(attackIV, MON_DATA_ATK_IV);
    COPY_MON_DATA(defenseIV, MON_DATA_DEF_IV);
    COPY_MON_DATA(speedIV, MON_DATA_SPEED_IV);
    COPY_MON_DATA(spAttackIV, MON_DATA_SPATK_IV);
    COPY_MON_DATA(spDefenseIV, MON_DATA_SPDEF_IV);

    COPY_MON_DATA(pokeball, MON_DATA_POKEBALL);
    COPY_MON_DATA(abilityNum, MON_DATA_ABILITY_NUM);
    COPY_MON_DATA(genderFlag, MON_DATA_GENDER_FLAG);
    COPY_MON_DATA(shinyFlag, MON_DATA_IS_SHINY);

    GetBoxMonData(fromMon, MON_DATA_NICKNAME, toMon->nickname);

    // Use original nature
    temp = GetBoxMonData(fromMon, MON_DATA_PERSONALITY);
    toMon->nature = GetNatureFromPersonality(temp);
}

#undef COPY_MON_DATA

static bool8 IsMonAlreadySpawned(u8 safariIndex)
{
    u8 i;

    for(i = 0; i < ARRAY_COUNT(sSafariData.slotToIndexMap); ++i)
    {
        if(sSafariData.slotToIndexMap[i] == safariIndex)
            return TRUE;
    }

    return FALSE;
}

struct RogueSafariMon* RogueSafari_ChooseSafariMonForSlot(u8 slot)
{
    u8 i;

    for(i = 0; i < ROGUE_SAFARI_TOTAL_MONS; ++i)
    {
        sSafariData.spawnIndex = (sSafariData.spawnIndex + 1) % ROGUE_SAFARI_LEGENDS_START_INDEX;

        if(gRogueSaveBlock->safariMons[sSafariData.spawnIndex].species != SPECIES_NONE && !IsMonAlreadySpawned(sSafariData.spawnIndex))
        {
            sSafariData.slotToIndexMap[slot] = sSafariData.spawnIndex;
            return &gRogueSaveBlock->safariMons[sSafariData.spawnIndex];
        }
    }

    // Couldn't find a mon to spawn
    return NULL;
}

void RogueSafari_RemoveMonFromSlot(u8 slot)
{
    if(slot < ARRAY_COUNT(sSafariData.slotToIndexMap))
    {
        sSafariData.slotToIndexMap[slot] = INVALID_SAFARI_MON_IDX;
    }
}

struct RogueSafariMon* RogueSafari_GetSafariMonAt(u8 index)
{
    if(index < ARRAY_COUNT(sSafariData.slotToIndexMap))
    {
        u8 safariIndex = sSafariData.slotToIndexMap[index];

        if(safariIndex < ROGUE_SAFARI_TOTAL_MONS)
            return &gRogueSaveBlock->safariMons[safariIndex];
    }

    return NULL;
}

void RogueSafari_ClearSafariMonAtIdx(u8 index)
{
    if(index < ROGUE_SAFARI_TOTAL_MONS)
    {
        u8 i;

        for(i = 0; i < ARRAY_COUNT(sSafariData.slotToIndexMap); ++i)
        {
            if(sSafariData.slotToIndexMap[i] == index)
                sSafariData.slotToIndexMap[i] = INVALID_SAFARI_MON_IDX;
        }

        ZeroSafariMon(&gRogueSaveBlock->safariMons[index]);
        
        if(sSafariData.pendingBattleIdx == index)
            sSafariData.pendingBattleIdx = INVALID_SAFARI_MON_IDX;
    }
}

void RogueSafari_EnqueueBattleMon(u8 slot)
{
    AGB_ASSERT(slot < ARRAY_COUNT(sSafariData.slotToIndexMap));

    if(slot < ARRAY_COUNT(sSafariData.slotToIndexMap))
    {
        sSafariData.pendingBattleIdx = sSafariData.slotToIndexMap[slot];
    }
}

void RogueSafari_EnqueueBattleMonByIndex(u8 index)
{
    sSafariData.pendingBattleIdx = index;
}

u8 RogueSafari_GetPendingBattleMonIdx()
{
    return sSafariData.pendingBattleIdx;
}

struct RogueSafariMon* RogueSafari_GetPendingBattleMon()
{
    u8 safariIndex = sSafariData.pendingBattleIdx;

    if(safariIndex < ROGUE_SAFARI_TOTAL_MONS)
        return &gRogueSaveBlock->safariMons[safariIndex];

    return NULL;
}

static u8 AllocSafariMonSlotFor(struct BoxPokemon* mon)
{
    u8 i;
    u16 species = GetBoxMonData(mon, MON_DATA_SPECIES);

    u8 startIndex = RoguePokedex_IsSpeciesLegendary(species) ? ROGUE_SAFARI_LEGENDS_START_INDEX : 0;
    u8 endIndex = RoguePokedex_IsSpeciesLegendary(species) ? (ROGUE_SAFARI_TOTAL_MONS - 1) : ROGUE_SAFARI_LEGENDS_START_INDEX - 1;

    for(i = startIndex; i <= endIndex; ++i)
    {
        if(gRogueSaveBlock->safariMons[i].species == SPECIES_NONE)
        {
            // There is a free slot here
            return i;
        }
    }

    // If we got here, that means we need to get rid of a mon and take it's slot
    {
        u8 lowestPriority = 255;
        u16 idx, offset;

        // Count down priorities
        for(i = startIndex; i <= endIndex; ++i)
        {
            if(gRogueSaveBlock->safariMons[i].priorityCounter != 0)
                --gRogueSaveBlock->safariMons[i].priorityCounter;

            // Keep track of lowest priority in case there isn't a free slot
            lowestPriority = min(lowestPriority, gRogueSaveBlock->safariMons[i].priorityCounter);
        }

        offset = Random();

        // Find first mon of priority and give back it's slot
        for(i = startIndex; i <= endIndex; ++i)
        {
            idx = startIndex + (offset + i - startIndex) % (endIndex - startIndex + 1);

            if(gRogueSaveBlock->safariMons[idx].priorityCounter == lowestPriority)
            {
                return idx;
            }
        }
    }

    // Should never reach here
    AGB_ASSERT(FALSE);
    return 0;
}

static u8 AllocCustomMonSafariSlot(u32 customMonId, u8 forSafariIndex)
{
    u8 i;
    bool8 customMonSlotInUse[ROGUE_SAFARI_TOTAL_CUSTOM_MONS] = {0};

    // To avoid desync issues we're just gonna check which slots are actively in use
    for(i = 0; i < ROGUE_SAFARI_TOTAL_MONS; ++i)
    {
        if(gRogueSaveBlock->safariMons[i].species != SPECIES_NONE && gRogueSaveBlock->safariMons[i].customMonLookup != 0)
        {
            u8 lookupIdx = gRogueSaveBlock->safariMons[i].customMonLookup - 1;
            customMonSlotInUse[lookupIdx] = TRUE;
        }
    }

    // Try to find a free slot
    for(i = 0; i < ROGUE_SAFARI_TOTAL_CUSTOM_MONS; ++i)
    {
        if(!customMonSlotInUse[i])
        {
            // Found free slot
            gRogueSaveBlock->safariMonCustomIds[i] = customMonId;
            return i;
        }
    }

    // If we got here, that means we need to get rid of a mon and take it's custom mon slot
    {
        u8 lowestPriority = 255;
        u16 idx, offset;

        // Count down priorities
        for(i = 0; i < ROGUE_SAFARI_TOTAL_MONS; ++i)
        {
            if(i == forSafariIndex || gRogueSaveBlock->safariMons[i].customMonLookup == 0)
                continue;

            // Keep track of lowest priority in case there isn't a free slot
            lowestPriority = min(lowestPriority, gRogueSaveBlock->safariMons[i].priorityCounter);
        }

        offset = Random();

        // Find first mon of priority and give back it's slot
        for(i = 0; i < ROGUE_SAFARI_TOTAL_MONS; ++i)
        {
            if(i == forSafariIndex || gRogueSaveBlock->safariMons[i].customMonLookup == 0)
                continue;

            idx = (offset + i) % ROGUE_SAFARI_TOTAL_MONS;

            if(gRogueSaveBlock->safariMons[idx].priorityCounter == lowestPriority)
            {
                u8 lookupIdx = gRogueSaveBlock->safariMons[idx].customMonLookup - 1;
                ZeroSafariMon(&gRogueSaveBlock->safariMons[idx]);

                gRogueSaveBlock->safariMonCustomIds[lookupIdx] = customMonId;
                return lookupIdx;
            }
        }
    }

    // Shouldn't reach here
    AGB_ASSERT(FALSE);
    return 0;
}

static u8 UNUSED FreeSafariMonSlotCount()
{
    u8 i;
    u8 count = 0;

    for(i = 0; i < ROGUE_SAFARI_TOTAL_MONS; ++i)
    {
        if(gRogueSaveBlock->safariMons[i].species != SPECIES_NONE)
            ++count;
    }

    return count;
}

u16 RogueSafari_GetActivePokeballType()
{
    if(Rogue_IsCatchingContestActive())
    {
        return ITEM_CATCHING_CONTEST_POKEBALL;
    }
    else
    {
        u16 itemId = VarGet(VAR_ROGUE_SAFARI_BALL_TYPE);

        if(itemId >= FIRST_BALL && itemId <= LAST_BALL)
            return itemId;

        return ITEM_POKE_BALL;
    }
}

void RogueSafari_SetActivePokeballType(u16 itemId)
{
    VarSet(VAR_ROGUE_SAFARI_BALL_TYPE, itemId);
}

static void CompactEmptyEntriesInternal(u8 fromIndex, u8 toIndex)
{
    u8 i;
    u8 write = fromIndex;
    u8 endIndex = toIndex;
    bool8 loop = TRUE;
    //u8 count = 0;

    for(i = fromIndex; i <= toIndex; ++i)
    {
        if(gRogueSaveBlock->safariMons[i].species != SPECIES_NONE)
        {
            if(write != i)
            {
                memcpy(&gRogueSaveBlock->safariMons[write], &gRogueSaveBlock->safariMons[i], sizeof(struct RogueSafariMon));
            }

            ++write;
        }
    }

    for(i = write; i <= toIndex; ++i)
    {
        ZeroSafariMon(&gRogueSaveBlock->safariMons[i]);
    }
}

void RogueSafari_CompactEmptyEntries()
{
    CompactEmptyEntriesInternal(0, ROGUE_SAFARI_LEGENDS_START_INDEX - 1);
    CompactEmptyEntriesInternal(ROGUE_SAFARI_LEGENDS_START_INDEX, ROGUE_SAFARI_TOTAL_MONS - 1);

    if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_AREA_SAFARI_ZONE || gMapHeader.mapLayoutId == LAYOUT_ROGUE_AREA_SAFARI_ZONE_TUTORIAL)
    {
        u32 i;
        RogueSafari_ResetSpawns();

        for(i = 0; i < OBJECT_EVENTS_COUNT; ++i)
        {
            if(gObjectEvents[i].active && FollowMon_IsMonObject(&gObjectEvents[i], TRUE))
                RemoveObjectEvent(&gObjectEvents[i]);
        }
    }
}