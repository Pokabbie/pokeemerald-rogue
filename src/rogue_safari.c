#include "global.h"
#include "random.h"
#include "string_util.h"

#include "rogue_save.h"
#include "rogue_safari.h"

#define INVALID_SAFARI_MON_IDX 255

struct SafariData
{
    u8 spawnIndex;
    u8 pendingBattleIdx;
    u8 monIndexMap[FOLLOWMON_MAX_SPAWN_SLOTS];
};

static EWRAM_DATA struct SafariData sSafariData = {0};

static void ZeroSafariMon(struct RogueSafariMon* mon);
static void MonToSafariMon(struct BoxPokemon* inMon, struct RogueSafariMon* outMon);

void RogueSafari_PushMon(struct Pokemon* mon)
{
    RogueSafari_PushBoxMon(&mon->box);
}

void RogueSafari_PushBoxMon(struct BoxPokemon* monToCopy)
{
    u8 index = Random() % ARRAY_COUNT(gRogueSaveBlock->safariMons); // temp
    struct RogueSafariMon* writeMon = &gRogueSaveBlock->safariMons[index];

    ZeroSafariMon(writeMon);
    RogueSafari_CopyToSafariMon(monToCopy, writeMon);
}

static void ZeroSafariMon(struct RogueSafariMon* mon)
{
    memset(mon, 0, sizeof(struct RogueSafariMon));
    mon->nickname[0] = 0xFF;
}

void RogueSafari_ResetSpawns()
{
    u8 i;

    sSafariData.pendingBattleIdx = INVALID_SAFARI_MON_IDX;

    for(i = 0; i < ARRAY_COUNT(sSafariData.monIndexMap); ++i)
    {
        sSafariData.monIndexMap[i] = INVALID_SAFARI_MON_IDX;
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
}

#undef COPY_MON_DATA

static bool8 IsMonAlreadySpawned(u8 safariIndex)
{
    u8 i;

    for(i = 0; i < ARRAY_COUNT(sSafariData.monIndexMap); ++i)
    {
        if(sSafariData.monIndexMap[i] == safariIndex)
            return TRUE;
    }

    return FALSE;
}

struct RogueSafariMon* RogueSafari_ChooseNewSafariMon(u8 index)
{
    u8 i;

    for(i = 0; i < ARRAY_COUNT(gRogueSaveBlock->safariMons); ++i)
    {
        sSafariData.spawnIndex = (sSafariData.spawnIndex + 1) % ARRAY_COUNT(gRogueSaveBlock->safariMons);

        if(gRogueSaveBlock->safariMons[sSafariData.spawnIndex].species != SPECIES_NONE && !IsMonAlreadySpawned(sSafariData.spawnIndex))
        {
            sSafariData.monIndexMap[index] = sSafariData.spawnIndex;
            return &gRogueSaveBlock->safariMons[sSafariData.spawnIndex];
        }
    }

    // Couldn't find a mon to spawn
    return NULL;
}

struct RogueSafariMon* RogueSafari_GetSafariMonAt(u8 index)
{
    if(index < ARRAY_COUNT(sSafariData.monIndexMap))
    {
        u8 safariIndex = sSafariData.monIndexMap[index];

        if(safariIndex < ARRAY_COUNT(gRogueSaveBlock->safariMons))
            return &gRogueSaveBlock->safariMons[safariIndex];
    }

    return NULL;
}

void RogueSafari_ClearSafariMonAt(u8 index)
{
    if(index < ARRAY_COUNT(sSafariData.monIndexMap))
    {
        sSafariData.monIndexMap[index] = INVALID_SAFARI_MON_IDX;
    }
}

void RogueSafari_EnqueueBattleMon(u8 index)
{
    AGB_ASSERT(index < ARRAY_COUNT(sSafariData.monIndexMap));

    if(index < ARRAY_COUNT(sSafariData.monIndexMap))
    {
        sSafariData.pendingBattleIdx = sSafariData.monIndexMap[index];
    }
}

struct RogueSafariMon* RogueSafari_ConsumePendingBattleMon()
{
    u8 safariIndex = sSafariData.pendingBattleIdx;
    sSafariData.pendingBattleIdx = INVALID_SAFARI_MON_IDX;

    if(safariIndex < ARRAY_COUNT(gRogueSaveBlock->safariMons))
    {
        return &gRogueSaveBlock->safariMons[safariIndex];
    }

    return NULL;
}
