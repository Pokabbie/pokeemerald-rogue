#include "global.h"
//#include "constants/abilities.h"
//#include "constants/battle.h"
#include "constants/event_objects.h"
//#include "constants/heal_locations.h"
//#include "constants/hold_effects.h"
#include "constants/items.h"
//#include "constants/layouts.h"
#include "constants/moves.h"
//#include "constants/rogue.h"
//#include "constants/rgb.h"
#include "constants/weather.h"
//#include "data.h"
#include "gba/isagbprint.h"
//
//#include "battle.h"
//#include "battle_setup.h"
//#include "berry.h"
#include "event_data.h"
//#include "graphics.h"
//#include "item.h"
//#include "load_save.h"
#include "malloc.h"
//#include "main.h"
//#include "money.h"
//#include "m4a.h"
//#include "overworld.h"
#include "party_menu.h"
//#include "palette.h"
//#include "play_time.h"
//#include "player_pc.h"
//#include "pokemon.h"
//#include "pokemon_icon.h"
//#include "pokemon_storage_system.h"
//#include "random.h"
//#include "rtc.h"
//#include "safari_zone.h"
//#include "script.h"
//#include "siirtc.h"
//#include "strings.h"
//#include "string_util.h"
//#include "text.h"

#include "rogue.h"
//#include "rogue_assistant.h"
//#include "rogue_automation.h"
#include "rogue_adventurepaths.h"
//#include "rogue_campaign.h"
//#include "rogue_charms.h"
#include "rogue_controller.h"
//#include "rogue_followmon.h"
//#include "rogue_popup.h"
#include "rogue_query.h"
//#include "rogue_quest.h"
//#include "rogue_timeofday.h"
#include "rogue_trainers.h"

struct TrainerHeldItemScratch
{
    bool8 hasLeftovers : 1;
    bool8 hasShellbell : 1;
#ifdef ROGUE_EXPANSION
    bool8 hasMegaStone : 1;
    bool8 hasZCrystal : 1;
#endif
};

struct TrainerGeneratorScratch
{
    u16 trainerNum;
    u8 generatorCounter;
    u8 generatorUseCount;
    u8 totalMonCount;
    bool8 useFinalChampionGen : 1;
    bool8 isSelectingAceMon : 1;
    bool8 useAceGenerator : 1;
    bool8 isGeneratorQueryValid : 1;
    struct TrainerHeldItemScratch heldItems;
    struct Pokemon* partyPtr;
    const struct RogueTrainer* trainerPtr;
    struct RogueTrainerMonGenerator monGenerator;
};

static EWRAM_DATA struct TrainerGeneratorScratch* sTrainerScratch = NULL;

bool8 IsSpeciesLegendary(u16 species);

static void BeginTrainerSpeciesQuery(u16 trainerNum, struct Pokemon* party);
static void EndTrainerSpeciesQuery();
static u16 NextTrainerSpecies(struct Pokemon* party);
static void NextMonGenerator();
static void ApplyMonGeneratorQuery(struct RogueTrainerMonGenerator* generator);

static bool8 UseCompetitiveMoveset(u8 monIdx, u8 totalMonCount);
static bool8 SelectNextPreset(u16 species, struct RogueMonPreset* outPreset);
static void ModifyTrainerMonPreset(struct RogueMonPreset* preset);

static void ReorderPartyMons(struct Pokemon *party, u8 monCount);

bool8 Rogue_IsBossTrainer(u16 trainerNum)
{
    return trainerNum < gRogueTrainers.bossCount;
}

bool8 Rogue_IsMiniBossTrainer(u16 trainerNum)
{
    return FALSE;
}

bool8 Rogue_IsAnyBossTrainer(u16 trainerNum)
{
    return Rogue_IsBossTrainer(trainerNum) || Rogue_IsMiniBossTrainer(trainerNum);
}

static bool8 HasValidAceGenerator(const struct RogueTrainer* trainer)
{
    return trainer->aceMonGenerators[0].monCount != 0;
}

static u8 GetTrainerLevel(u16 trainerNum)
{
    if(Rogue_IsBossTrainer(trainerNum))
    {
        return Rogue_CalculateBossMonLvl();
    }

    if(Rogue_IsMiniBossTrainer(trainerNum))
    {
        return Rogue_CalculateMiniBossMonLvl();
    }

    return Rogue_CalculateTrainerMonLvl();
}

bool8 Rogue_TryGetTrainer(u16 trainerNum, const struct RogueTrainer** trainerPtr)
{
    if(trainerNum < gRogueTrainers.bossCount)
    {
        *trainerPtr = &gRogueTrainers.boss[trainerNum];
        return TRUE;
    }

    return FALSE;
}

u16 Rogue_GetTrainerObjectEventGfx(u16 trainerNum)
{
    const struct RogueTrainer* trainer;
    if(Rogue_TryGetTrainer(trainerNum, &trainer))
    {
        return trainer->objectEventGfx;
    }

    // Fallback in event of error
    return OBJ_EVENT_GFX_ITEM_BALL;
}

u8 Rogue_GetTrainerWeather(u16 trainerNum)
{
    const struct RogueTrainer* trainer;
    u8 weatherType = WEATHER_NONE;

    if(Rogue_IsAnyBossTrainer(trainerNum) && Rogue_TryGetTrainer(trainerNum, &trainer))
    {
        if(FlagGet(FLAG_ROGUE_HARD_TRAINERS))
        {
            if(gRogueRun.currentDifficulty > 0)
                weatherType = trainer->preferredWeather;
        }
        else if(gRogueRun.currentDifficulty > 2)
        {
            weatherType = trainer->preferredWeather;
        }
    }

    if(weatherType == WEATHER_DEFAULT)
    {
        weatherType = gRogueTypeWeatherTable[trainer->monGenerators[0].incTypes[0]];
    }

    return weatherType;
}

static u8 CalculateLvlFor(u8 difficulty)
{
    if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
    {
        return MAX_LEVEL;
    }

    // Gym leaders lvs 15 -> 85
    if(difficulty <= 7)
    {
        return min(100, 15 + 10 * difficulty);
    }
    else
    {
        // Both champions are lvl 100
        difficulty -= 7; // (1 - 6)
        return min(100, 85 + 3 * difficulty);
    }
}

u8 Rogue_CalculatePlayerMonLvl()
{
    return Rogue_CalculateBossMonLvl() - gRogueRun.currentLevelOffset;
}

u8 Rogue_CalculateTrainerMonLvl()
{
    // TODO - Route difficulty

    if(gRogueRun.currentDifficulty == 0)
    {
        return 5;
    }
    else
    {
        u8 lastLvl = CalculateLvlFor(gRogueRun.currentDifficulty - 1);
        u8 currLvl = Rogue_CalculatePlayerMonLvl();

        return (lastLvl + currLvl) / 2;
    }

    return CalculateLvlFor(gRogueRun.currentDifficulty);
}

u8 Rogue_CalculateMiniBossMonLvl()
{
    return Rogue_CalculatePlayerMonLvl() - 5;
}

u8 Rogue_CalculateBossMonLvl()
{
    return CalculateLvlFor(gRogueRun.currentDifficulty);
}

u8 Rogue_GetTrainerTypeAssignment(u16 trainerNum)
{
    const struct RogueTrainer* trainer;

    if(Rogue_TryGetTrainer(trainerNum, &trainer))
    {
        return trainer->monGenerators[0].incTypes[0];
    }

    return TYPE_NONE;
}

bool8 Rogue_UseCustomPartyGenerator(u16 trainerNum)
{
    return TRUE;
}


static u16 GetBossHistoryKey(u16 bossId)
{
    // We're gonna always use the trainer's assigned type to prevent dupes
    // The history buffer will be wiped between stages to allow for types to re-appear later e.g. juan can appear as gym and wallace can appear as champ
    u16 type = gRogueTrainers.boss[bossId].monGenerators[0].incTypes[0];

    // None type trainers are unqiue, so we don't care about the type repeat
    if(type == TYPE_NONE)
        return NUMBER_OF_MON_TYPES + bossId;

    return type;
}

static bool8 IsBossEnabled(u16 bossId)
{
    const struct RogueTrainer* trainer = &gRogueTrainers.boss[bossId];
    u16 includeFlags = TRAINER_FLAG_NONE;
    u16 excludeFlags = TRAINER_FLAG_NONE;
    
    if(!FlagGet(FLAG_ROGUE_RAINBOW_MODE))
    {
        if(FlagGet(FLAG_ROGUE_HOENN_BOSSES))
            includeFlags |= TRAINER_FLAG_HOENN;

        if(FlagGet(FLAG_ROGUE_KANTO_BOSSES))
            includeFlags |= TRAINER_FLAG_KANTO;

        if(FlagGet(FLAG_ROGUE_JOHTO_BOSSES))
            includeFlags |= TRAINER_FLAG_JOHTO;

        // Use the custom fallback set >:3
        if(includeFlags == TRAINER_FLAG_NONE)
        {
            includeFlags |= TRAINER_FLAG_FALLBACK_REGION;
        }

        if(gRogueRun.currentDifficulty < 8)
            excludeFlags |= TRAINER_FLAG_ELITE | TRAINER_FLAG_PRE_CHAMP | TRAINER_FLAG_FINAL_CHAMP;
        else if(gRogueRun.currentDifficulty < 12)
            excludeFlags |= TRAINER_FLAG_GYM | TRAINER_FLAG_PRE_CHAMP | TRAINER_FLAG_FINAL_CHAMP;
        else if(gRogueRun.currentDifficulty < 13)
            excludeFlags |= TRAINER_FLAG_GYM | TRAINER_FLAG_ELITE | TRAINER_FLAG_FINAL_CHAMP;
        else if(gRogueRun.currentDifficulty < 14)
            excludeFlags |= TRAINER_FLAG_GYM | TRAINER_FLAG_ELITE | TRAINER_FLAG_PRE_CHAMP;
    }
    else
    {
        excludeFlags |= TRAINER_FLAG_RAINBOW_EXCLUDE;

        if(gRogueRun.currentDifficulty >= 13)
            includeFlags |= TRAINER_FLAG_RAINBOW_CHAMP;
        else
        {
            // Don't use special trainers for rainbow mode
            if(gRogueTrainers.boss[bossId].monGenerators[0].incTypes[0] == TYPE_NONE)
                return FALSE;
        }
    }

    if(excludeFlags != TRAINER_FLAG_NONE && (trainer->trainerFlags & excludeFlags) != 0)
    {
        return FALSE;
    }

    if(includeFlags == TRAINER_FLAG_NONE || (trainer->trainerFlags & includeFlags) != 0)
    {
        if(!HistoryBufferContains(&gRogueRun.bossHistoryBuffer[0], ARRAY_COUNT(gRogueRun.bossHistoryBuffer), GetBossHistoryKey(bossId)))
        {
            return TRUE;
        }
    }

    return FALSE;
}

static u16 NextBossId()
{
    u16 i;
    u16 randIdx;
    u16 enabledBossesCount = 0;

    for(i = 0; i < gRogueTrainers.bossCount; ++i)
    {
        if(IsBossEnabled(i))
            ++enabledBossesCount;
    }

    randIdx = RogueRandomRange(enabledBossesCount, OVERWORLD_FLAG);
    enabledBossesCount = 0;

    for(i = 0; i < gRogueTrainers.bossCount; ++i)
    {
        if(IsBossEnabled(i))
        {
            if(enabledBossesCount == randIdx)
                return i;
            else
                ++enabledBossesCount;
        }
    }

    return gRogueTrainers.bossCount - 1;
}

u16 Rogue_NextBossTrainerId()
{
    u16 bossId = NextBossId();

    HistoryBufferPush(&gRogueRun.bossHistoryBuffer[0], ARRAY_COUNT(gRogueRun.bossHistoryBuffer), GetBossHistoryKey(bossId));

    return bossId;
}

void Rogue_GetPreferredElite4Map(u16 trainerNum, s8* mapGroup, s8* mapNum)
{
    const struct RogueTrainer* trainerPtr;
    u8 type = TYPE_NORMAL;

    if(Rogue_TryGetTrainer(trainerNum, &trainerPtr))
    {
        type = trainerPtr->monGenerators[0].incTypes[0];
    }

    *mapGroup = gRogueTypeToEliteRoom[type].group;
    *mapNum = gRogueTypeToEliteRoom[type].num;
}

u8 Rogue_CreateTrainerParty(u16 trainerNum, struct Pokemon* party, u8 monCapacity, bool8 firstTrainer)
{
    u8 i;
    u8 monCount;
    u8 level;
    u8 fixedIV;
    u16 species;
    const struct RogueTrainer* trainerPtr;

    if(!Rogue_TryGetTrainer(trainerNum, &trainerPtr))
    {
        // No mons??? D:
        return 0;
    }

    level = GetTrainerLevel(trainerNum);

    if(FlagGet(FLAG_ROGUE_EASY_TRAINERS))
        fixedIV = 0;
    else if(FlagGet(FLAG_ROGUE_HARD_TRAINERS))
    {
        if(Rogue_IsBossTrainer(trainerNum))
        {
            if(gRogueRun.currentDifficulty >= 12)
                fixedIV = 31;
            else if(gRogueRun.currentDifficulty >= 8)
                fixedIV = 21;
            else if(gRogueRun.currentDifficulty >= 6)
                fixedIV = 19;
            else if(gRogueRun.currentDifficulty >= 3)
                fixedIV = 15;
            else if(gRogueRun.currentDifficulty >= 1)
                fixedIV = 11;
            else
                fixedIV = 5;
        }
        else
        {
            fixedIV = (gRogueRun.currentDifficulty > 8) ? 13 : 5;
        }
    }
    else
    {
        if(Rogue_IsBossTrainer(trainerNum))
        {
            if(gRogueRun.currentDifficulty >= 6)
                fixedIV = 16;
            else if(gRogueRun.currentDifficulty >= 5)
                fixedIV = 10;
            else if(gRogueRun.currentDifficulty >= 4)
                fixedIV = 8;
            else if(gRogueRun.currentDifficulty >= 3)
                fixedIV = 6;
            else
                fixedIV = 0;
        }
        else
        {
            fixedIV = 0;
        }
    }

    // Decide on mon count
    {
        if(Rogue_IsAnyBossTrainer(trainerNum))
        {
            if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
                monCount = 6;
            else if(FlagGet(FLAG_ROGUE_HARD_TRAINERS)) // Hard
            {
                if(gRogueRun.currentDifficulty == 0)
                    monCount = 4;
                else if(gRogueRun.currentDifficulty == 1)
                    monCount = 5;
                else
                    monCount = 6;
            }
            else // Average & Easy
            {
                if(gRogueRun.currentDifficulty == 0)
                    monCount = 3;
                else if(gRogueRun.currentDifficulty <= 2)
                    monCount = 4;
                else if(gRogueRun.currentDifficulty <= 5)
                    monCount = 5;
                else
                    monCount = 6;
            }
        }
        else
        {
            u8 minMonCount;
            u8 maxMonCount;
            // TODO - Exp trainer support

            if(gRogueRun.currentDifficulty <= 1)
            {
                minMonCount = 1;
                maxMonCount = 2;
            }
            else if(gRogueRun.currentDifficulty <= 2)
            {
                minMonCount = 1;
                maxMonCount = 3;
            }
            else if(gRogueRun.currentDifficulty <= 11)
            {
                minMonCount = 2;
                maxMonCount = 4;
            }
            else
            {
                minMonCount = 3;
                maxMonCount = 4;
            }

            monCount = minMonCount + RogueRandomRange(maxMonCount - minMonCount, FLAG_SET_SEED_TRAINERS);
        }

        monCount = min(monCount, monCapacity);
    }

    // Generate team
    BeginTrainerSpeciesQuery(trainerNum, party);
    {
        struct RogueMonPreset preset;

        for(i = 0; i < monCount; ++i)
        {
            species = NextTrainerSpecies(party);

#if defined(ROGUE_DEBUG) && defined(ROGUE_DEBUG_LVL_5_TRAINERS)
            CreateMon(&party[i], species, 5, fixedIV, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);
#else
            CreateMon(&party[i], species, level, fixedIV, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);
#endif

            if(UseCompetitiveMoveset(i, monCount) && SelectNextPreset(species, &preset))
            {
                ModifyTrainerMonPreset(&preset);
                Rogue_ApplyMonPreset(&party[i], level, &preset);
            }
        }

        ReorderPartyMons(party, monCount);
    }
    EndTrainerSpeciesQuery();

    // Debug steal team
#if defined(ROGUE_DEBUG) && defined(ROGUE_DEBUG_STEAL_TEAM)
    {
        u8 i;
        u16 exp = Rogue_ModifyExperienceTables(1, 100);

        for(i = 0; i < PARTY_SIZE; ++i)
        {
            ZeroMonData(&gPlayerParty[i]);
        }

        gPlayerPartyCount = monCount;

        for(i = 0; i < gPlayerPartyCount; ++i)
        {
            CopyMon(&gPlayerParty[i], &party[i], sizeof(gPlayerParty[i]));

            SetMonData(&gPlayerParty[i], MON_DATA_EXP, &exp);
            CalculateMonStats(&gPlayerParty[i]);
        }
    }
#endif

    return monCount;
}

static void BeginTrainerSpeciesQuery(u16 trainerNum, struct Pokemon* party)
{
    AGB_ASSERT(sTrainerScratch == NULL);
    sTrainerScratch = AllocZeroed(sizeof(struct TrainerGeneratorScratch));

    sTrainerScratch->trainerNum = trainerNum;
    sTrainerScratch->partyPtr = party;
    Rogue_TryGetTrainer(trainerNum, &sTrainerScratch->trainerPtr);

    NextMonGenerator();
}

static void EndTrainerSpeciesQuery()
{
    AGB_ASSERT(sTrainerScratch != NULL);
    free(sTrainerScratch);
    sTrainerScratch = NULL;
}


static u16 GetDuplicateCheckSpecies(u16 species)
{
#ifdef ROGUE_EXPANSION
    u16 baseSpecies = GET_BASE_SPECIES_ID(species);

    switch (baseSpecies)
    {
    case SPECIES_DEOXYS:
    case SPECIES_BURMY:
    case SPECIES_WORMADAM:
    case SPECIES_SHELLOS:
    case SPECIES_GASTRODON:
    case SPECIES_ROTOM:
    case SPECIES_GIRATINA:
    case SPECIES_SHAYMIN:
    case SPECIES_ARCEUS:
    case SPECIES_BASCULIN:
    case SPECIES_DARMANITAN:
    case SPECIES_DARMANITAN_GALARIAN:
    case SPECIES_DEERLING:
    case SPECIES_SAWSBUCK:
    case SPECIES_TORNADUS:
    case SPECIES_THUNDURUS:
    case SPECIES_LANDORUS:
    case SPECIES_KYUREM:
    case SPECIES_KELDEO:
    case SPECIES_MELOETTA:
    case SPECIES_GENESECT:
    case SPECIES_VIVILLON:
    case SPECIES_FLABEBE:
    case SPECIES_FLOETTE:
    case SPECIES_FLORGES:
    case SPECIES_FURFROU:
    case SPECIES_MEOWSTIC:
    case SPECIES_HOOPA:
    case SPECIES_ORICORIO:
    case SPECIES_LYCANROC:
    case SPECIES_SILVALLY:
    case SPECIES_NECROZMA:
    case SPECIES_TOXTRICITY:
    case SPECIES_SINISTEA:
    case SPECIES_POLTEAGEIST:
    case SPECIES_ALCREMIE:
    case SPECIES_INDEEDEE:
    case SPECIES_ZACIAN:
    case SPECIES_ZAMAZENTA:
    case SPECIES_URSHIFU:
    case SPECIES_CALYREX:
        return baseSpecies;
    }
#endif

    return species;
}

bool8 PartyContainsSpecies(struct Pokemon *party, u8 partyCount, u16 species)
{
    u8 i;
    u16 s;

    species = GetDuplicateCheckSpecies(species);

    for(i = 0; i < partyCount; ++i)
    {
        s = GetDuplicateCheckSpecies(GetMonData(&party[i], MON_DATA_SPECIES));

        if(s == species)
            return TRUE;
    }

    return FALSE;
}

static u16 NextTrainerSpecies(struct Pokemon* party)
{
    u16 species = SPECIES_NONE;

    AGB_ASSERT(sTrainerScratch != NULL);

    sTrainerScratch->useFinalChampionGen = Rogue_IsBossTrainer(sTrainerScratch->trainerNum) && gRogueRun.currentDifficulty >= 13;

    while(species == SPECIES_NONE)
    {
        ++sTrainerScratch->generatorUseCount;

        // Ensure we have a valid generator
        if(sTrainerScratch->generatorUseCount >= sTrainerScratch->monGenerator.monCount)
            NextMonGenerator();

        if(Rogue_IsBossTrainer(sTrainerScratch->trainerNum))
        {
            // Enter Ace mon selection mode for last prechamp mon and last 2 final champ mons
            if((gRogueRun.currentDifficulty == 12 && sTrainerScratch->totalMonCount == 5) || (gRogueRun.currentDifficulty >= 13 && sTrainerScratch->totalMonCount == 4))
            {
                sTrainerScratch->isSelectingAceMon = TRUE;

                // Reset, so we will use the starting ace generator
                if(HasValidAceGenerator(sTrainerScratch->trainerPtr))
                {
                    sTrainerScratch->useAceGenerator = TRUE;
                    sTrainerScratch->generatorCounter = 0;
                }

                NextMonGenerator();
            }
        }

        // Recreate query from current generator
        if(!sTrainerScratch->isGeneratorQueryValid)
        {
            sTrainerScratch->isGeneratorQueryValid = TRUE;
            ApplyMonGeneratorQuery(&sTrainerScratch->monGenerator);
        }

        // Attempt to select a random mon from query
        {
            u8 tryCount;
            u16 randIdx;
            u16 queryCount = RogueQuery_BufferSize();

            if(queryCount == 0)
            {
                // Force us to move to next generator
                sTrainerScratch->generatorUseCount = 100;
                continue;
            }

            for(tryCount = 0; tryCount < 10; ++tryCount)
            {
                randIdx = RogueRandomRange(queryCount, FLAG_SET_SEED_TRAINERS);
                species = RogueQuery_BufferPtr()[randIdx];

                // Keep retrying if we are getting duplicates
                if(!PartyContainsSpecies(party, sTrainerScratch->totalMonCount, species))
                {
                    // Probably don't need this?
                    RogueQuery_PopCollapsedIndex(randIdx);
                    break;
                }
            }

            if(tryCount == 10)
            {
                // Force us to move to next generator
                species = SPECIES_NONE;
                sTrainerScratch->generatorUseCount = 100;
            }
        }
    }

    // Increment generation stats
    ++sTrainerScratch->totalMonCount;
    return species;
}

static u8 SelectRandomType()
{
    u8 type;

    do
    {
        type = RogueRandomRange(NUMBER_OF_MON_TYPES, FLAG_SET_SEED_TRAINERS);
    }
    while(type == TYPE_MYSTERY);

    return type;
}

static void NextMonGenerator()
{
    bool8 useFallbackGen;
    u16 genIdx;

    AGB_ASSERT(sTrainerScratch != NULL);

    useFallbackGen = FALSE;
    genIdx = sTrainerScratch->generatorCounter++;

    if(sTrainerScratch->trainerPtr)
    {
        if(genIdx < ARRAY_COUNT(sTrainerScratch->trainerPtr->monGenerators) && !sTrainerScratch->useAceGenerator)
        {
            memcpy(&sTrainerScratch->monGenerator, &sTrainerScratch->trainerPtr->monGenerators[genIdx], sizeof(sTrainerScratch->monGenerator));

            // Modify generator
            if(sTrainerScratch->isSelectingAceMon)
            {
                sTrainerScratch->monGenerator.generatorFlags |= TRAINER_GENERATOR_FLAG_LEGENDARY_ONLY;
            }
        }
        else if(genIdx < ARRAY_COUNT(sTrainerScratch->trainerPtr->aceMonGenerators))
        {
            memcpy(&sTrainerScratch->monGenerator, &sTrainerScratch->trainerPtr->aceMonGenerators[genIdx], sizeof(sTrainerScratch->monGenerator));
        }
        else
        {
            useFallbackGen = TRUE;
        }
    }
    else
    {
        useFallbackGen = TRUE;
    }

    //TRAINER_GENERATOR_FLAG_COUNTER_COVERAGE

    // Fallback generator is simple, just select random type
    if(useFallbackGen)
    {
        memset(&sTrainerScratch->monGenerator, 0, sizeof(&sTrainerScratch->monGenerator));

        sTrainerScratch->monGenerator.monCount = 6;
        sTrainerScratch->monGenerator.incTypes[0] = SelectRandomType();
        sTrainerScratch->monGenerator.incTypes[1] = TYPE_NONE;

        sTrainerScratch->monGenerator.excTypes[0] = TYPE_NONE;
    }

    // Modify generator
    sTrainerScratch->monGenerator.targetLevel = GetTrainerLevel(sTrainerScratch->trainerNum);

    if(sTrainerScratch->useFinalChampionGen)
    {
        sTrainerScratch->useFinalChampionGen = FALSE;
        sTrainerScratch->monGenerator.generatorFlags |= TRAINER_GENERATOR_FLAG_COUNTER_COVERAGE;

        // reset generator count so we will use this generator again once we've reached the limit for this generator
        sTrainerScratch->generatorCounter--;
    }

    if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
    {
        sTrainerScratch->monGenerator.generatorFlags |= 
            TRAINER_GENERATOR_FLAG_ALLOW_STRONG_LEGENDARY | 
            TRAINER_GENERATOR_FLAG_ALLOW_WEAK_LEGENDARY |
            TRAINER_GENERATOR_FLAG_PREFER_STRONG_PRESETS;
    }

    if(Rogue_IsAnyBossTrainer(sTrainerScratch->trainerNum))
    {
        if(FlagGet(FLAG_ROGUE_EASY_TRAINERS)) // Easy
        {
            if(gRogueRun.currentDifficulty >= 8)
                sTrainerScratch->monGenerator.generatorFlags |= TRAINER_GENERATOR_FLAG_ALLOW_WEAK_LEGENDARY;

            if(gRogueRun.currentDifficulty >= 8)
                sTrainerScratch->monGenerator.generatorFlags |= TRAINER_GENERATOR_FLAG_ALLOW_ITEM_EVOS;
        }
        else if(FlagGet(FLAG_ROGUE_HARD_TRAINERS)) // Hard
        {
            if(gRogueRun.currentDifficulty >= 5)
                sTrainerScratch->monGenerator.generatorFlags |= TRAINER_GENERATOR_FLAG_ALLOW_STRONG_LEGENDARY;
            else if(gRogueRun.currentDifficulty >= 2)
                sTrainerScratch->monGenerator.generatorFlags |= TRAINER_GENERATOR_FLAG_ALLOW_WEAK_LEGENDARY;

            if(gRogueRun.currentDifficulty >= 5)
                sTrainerScratch->monGenerator.generatorFlags |= TRAINER_GENERATOR_FLAG_PREFER_STRONG_PRESETS;

            if(gRogueRun.currentDifficulty >= 2)
                sTrainerScratch->monGenerator.generatorFlags |= TRAINER_GENERATOR_FLAG_ALLOW_ITEM_EVOS;
        }
        else // Average
        {
            if(gRogueRun.currentDifficulty >= 8)
                sTrainerScratch->monGenerator.generatorFlags |= TRAINER_GENERATOR_FLAG_ALLOW_STRONG_LEGENDARY;
            else if(gRogueRun.currentDifficulty >= 7)
                sTrainerScratch->monGenerator.generatorFlags |= TRAINER_GENERATOR_FLAG_ALLOW_WEAK_LEGENDARY;

            if(gRogueRun.currentDifficulty >= 8)
                sTrainerScratch->monGenerator.generatorFlags |= TRAINER_GENERATOR_FLAG_PREFER_STRONG_PRESETS;

            if(gRogueRun.currentDifficulty >= 4)
                sTrainerScratch->monGenerator.generatorFlags |= TRAINER_GENERATOR_FLAG_ALLOW_ITEM_EVOS;
        }
    }

    if(sTrainerScratch->monGenerator.generatorFlags & TRAINER_GENERATOR_FLAG_LEGENDARY_ONLY)
    {
        // Make sure we at least allow weak legendaries if we're legendary only
        if(!(sTrainerScratch->monGenerator.generatorFlags & TRAINER_GENERATOR_FLAG_ALLOW_WEAK_LEGENDARY) && !(sTrainerScratch->monGenerator.generatorFlags & TRAINER_GENERATOR_FLAG_ALLOW_STRONG_LEGENDARY))
            sTrainerScratch->monGenerator.generatorFlags |= TRAINER_GENERATOR_FLAG_ALLOW_WEAK_LEGENDARY;
    }

    // Reset generator stats
    sTrainerScratch->generatorUseCount = 0;
    sTrainerScratch->isGeneratorQueryValid = FALSE;
}

static void AppendWeakTypes(u8 baseType, u8* typesBuffer, u8* typeCount)
{
    switch(baseType)
    {
        case TYPE_NORMAL:
            typesBuffer[(*typeCount)++] = TYPE_FIGHTING;
            break;
        case TYPE_FIGHTING:
            typesBuffer[(*typeCount)++] = TYPE_FLYING;
            typesBuffer[(*typeCount)++] = TYPE_PSYCHIC;
            break;
        case TYPE_FLYING:
            typesBuffer[(*typeCount)++] = TYPE_ELECTRIC;
            typesBuffer[(*typeCount)++] = TYPE_ROCK;
            typesBuffer[(*typeCount)++] = TYPE_ICE;
            break;
        case TYPE_POISON:
            typesBuffer[(*typeCount)++] = TYPE_GROUND;
            typesBuffer[(*typeCount)++] = TYPE_PSYCHIC;
            break;
        case TYPE_GROUND:
            typesBuffer[(*typeCount)++] = TYPE_WATER;
            typesBuffer[(*typeCount)++] = TYPE_GRASS;
            break;
        case TYPE_ROCK:
            typesBuffer[(*typeCount)++] = TYPE_FIGHTING;
            typesBuffer[(*typeCount)++] = TYPE_GRASS;
            typesBuffer[(*typeCount)++] = TYPE_WATER;
            break;
        case TYPE_BUG:
            typesBuffer[(*typeCount)++] = TYPE_FIRE;
            typesBuffer[(*typeCount)++] = TYPE_FLYING;
            typesBuffer[(*typeCount)++] = TYPE_ROCK;
            break;
        case TYPE_GHOST:
            typesBuffer[(*typeCount)++] = TYPE_DARK;
            break;
        case TYPE_STEEL:
            typesBuffer[(*typeCount)++] = TYPE_GROUND;
            typesBuffer[(*typeCount)++] = TYPE_FIRE;
            typesBuffer[(*typeCount)++] = TYPE_FIGHTING;
            break;
        case TYPE_FIRE:
            typesBuffer[(*typeCount)++] = TYPE_WATER;
            typesBuffer[(*typeCount)++] = TYPE_GROUND;
            typesBuffer[(*typeCount)++] = TYPE_ROCK;
            break;
        case TYPE_WATER:
            typesBuffer[(*typeCount)++] = TYPE_ELECTRIC;
            typesBuffer[(*typeCount)++] = TYPE_GRASS;
            break;
        case TYPE_GRASS:
            typesBuffer[(*typeCount)++] = TYPE_FIRE;
            typesBuffer[(*typeCount)++] = TYPE_FLYING;
            typesBuffer[(*typeCount)++] = TYPE_POISON;
            break;
        case TYPE_ELECTRIC:
            typesBuffer[(*typeCount)++] = TYPE_GROUND;
            break;
        case TYPE_PSYCHIC:
            typesBuffer[(*typeCount)++] = TYPE_GHOST;
            typesBuffer[(*typeCount)++] = TYPE_BUG;
            typesBuffer[(*typeCount)++] = TYPE_DARK;
            break;
        case TYPE_ICE:
            typesBuffer[(*typeCount)++] = TYPE_ROCK;
            typesBuffer[(*typeCount)++] = TYPE_STEEL;
            typesBuffer[(*typeCount)++] = TYPE_FIGHTING;
            break;
        case TYPE_DRAGON:
            typesBuffer[(*typeCount)++] = TYPE_DRAGON;
            typesBuffer[(*typeCount)++] = TYPE_ICE;
#ifdef ROGUE_EXPANSION
            typesBuffer[(*typeCount)++] = TYPE_FAIRY;
#endif
            break;
        case TYPE_DARK:
            typesBuffer[(*typeCount)++] = TYPE_FIGHTING;
            typesBuffer[(*typeCount)++] = TYPE_BUG;
            break;
#ifdef ROGUE_EXPANSION
        case TYPE_FAIRY:
            typesBuffer[(*typeCount)++] = TYPE_POISON;
            typesBuffer[(*typeCount)++] = TYPE_STEEL;
            break;
#endif
    };
}

static void ApplyMonGeneratorQuery(struct RogueTrainerMonGenerator* generator)
{
    // TODO - Support custom queries
    
    // Only valid for this run
    if(generator->generatorFlags & (TRAINER_GENERATOR_FLAG_UNIQUE_COVERAGE | TRAINER_GENERATOR_FLAG_COUNTER_COVERAGE))
        sTrainerScratch->isGeneratorQueryValid = FALSE;

    RogueQuery_Clear();
    RogueQuery_SpeciesExcludeCommon();

    RogueQuery_SpeciesAlternateForms(TRUE);
    RogueQuery_Exclude(SPECIES_UNOWN);
    

    if(generator->generatorFlags & TRAINER_GENERATOR_FLAG_LEGENDARY_ONLY)
        RogueQuery_SpeciesIsLegendary();
    
    if(!(generator->generatorFlags & TRAINER_GENERATOR_FLAG_ALLOW_STRONG_LEGENDARY))
        RogueQuery_SpeciesIsNotStrongLegendary();

    if(!(generator->generatorFlags & TRAINER_GENERATOR_FLAG_ALLOW_WEAK_LEGENDARY))
        RogueQuery_SpeciesIsNotWeakLegendary();


    RogueQuery_TransformToEggSpecies();
    RogueQuery_EvolveSpecies(generator->targetLevel, (generator->generatorFlags & TRAINER_GENERATOR_FLAG_ALLOW_ITEM_EVOS));
    
    // Apply alternate forms a 2nd time before we start filting by type
    RogueQuery_SpeciesAlternateForms(TRUE);

    if(generator->generatorFlags & TRAINER_GENERATOR_FLAG_UNIQUE_COVERAGE)
    {
        // COVERAGE
        RogueQuery_SpeciesNotOfTypes(&generator->excTypes[0], 2);
    }

    // Type query
    {
        if(generator->incTypes[0] != TYPE_NONE)
        {
            if(generator->incTypes[1] != TYPE_NONE)
                RogueQuery_SpeciesOfTypes(&generator->incTypes[0], 2);
            else
                RogueQuery_SpeciesOfType(generator->incTypes[0]);
        }

        if(generator->excTypes[0] != TYPE_NONE)
        {
            if(generator->excTypes[1] != TYPE_NONE)
                RogueQuery_SpeciesNotOfTypes(&generator->excTypes[0], 2);
            else
                RogueQuery_SpeciesNotOfType(generator->excTypes[0]);
        }
        
        if(generator->generatorFlags & TRAINER_GENERATOR_FLAG_COUNTER_COVERAGE)
        {
            u16 s;
            u8 incTypes[6];
            u8 incTypeCount = 0;
            u8 type1, type2;

            if(sTrainerScratch->totalMonCount < CalculatePlayerPartyCount())
            {
                s = GetMonData(&gPlayerParty[sTrainerScratch->totalMonCount], MON_DATA_SPECIES);
                type1 = gBaseStats[s].type1;
                type2 = gBaseStats[s].type2;

                AppendWeakTypes(type1, &incTypes[0], &incTypeCount);

                if(incTypeCount != 0)
                {
                    if(incTypeCount != 1)
                        RogueQuery_SpeciesOfTypes(&incTypes[0], min(4, incTypeCount));
                    else
                        RogueQuery_SpeciesOfType(incTypes[0]);
                }
            }
        }

        // Unique coverage exclude party species we already have
        if(generator->generatorFlags & TRAINER_GENERATOR_FLAG_UNIQUE_COVERAGE)
        {
            u8 i;
            u16 s;
            u8 type1, type2;
            for(i = 0; i < sTrainerScratch->totalMonCount; ++i)
            {
                s = GetMonData(&sTrainerScratch->partyPtr[i], MON_DATA_SPECIES);
                type1 = gBaseStats[s].type1;
                type2 = gBaseStats[s].type2;

                if(type1 != TYPE_NONE && type1 != generator->incTypes[0] && type1 != generator->incTypes[1])
                    RogueQuery_SpeciesNotOfType(type1);

                if(type2 != TYPE_NONE && type1 != type2 && type2 != generator->incTypes[0] && type2 != generator->incTypes[1])
                    RogueQuery_SpeciesNotOfType(type2);
            }
        }
    }

    if(generator->generatorFlags & TRAINER_GENERATOR_FLAG_FORCE_STRONG_PRESETS)
    {
        RogueQuery_SpeciesIncludeMonFlags(MON_FLAG_STRONG);
    }
    else if(generator->generatorFlags & TRAINER_GENERATOR_FLAG_PREFER_STRONG_PRESETS)
    {
        u16 maxGen = VarGet(VAR_ROGUE_ENABLED_GEN_LIMIT);
        u16 targetDex = VarGet(VAR_ROGUE_REGION_DEX_LIMIT);

        // Regional dex and national mode gen prior to gen 2 has strong mons disabled
        if(targetDex == 0 && maxGen >= 3)
            RogueQuery_SpeciesIncludeMonFlags(MON_FLAG_STRONG);
    }

    RogueQuery_CollapseSpeciesBuffer();
}

static bool8 UseCompetitiveMoveset(u8 monIdx, u8 totalMonCount)
{    
    bool8 preferCompetitive = FALSE;
    bool8 result = FALSE;
    u8 difficultyLevel = gRogueRun.currentDifficulty;
    u8 difficultyModifier = 1; // TODO - GetRoomTypeDifficulty();

    AGB_ASSERT(sTrainerScratch != NULL);

    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_LEGENDARY || difficultyModifier == 2) // HARD
    {
        // For regular trainers, Last and first mon can have competitive sets
        preferCompetitive = (monIdx == 0 || monIdx == (totalMonCount - 1));
    }

#ifdef ROGUE_FEATURE_AUTOMATION
    if(Rogue_AutomationGetFlag(AUTO_FLAG_TRAINER_FORCE_COMP_MOVESETS))
    {
        return TRUE;
    }
#endif

    if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
    {
        return Rogue_IsAnyBossTrainer(sTrainerScratch->trainerNum);
    }
    if(FlagGet(FLAG_ROGUE_EASY_TRAINERS))
    {
        return FALSE;
    }
    else if(FlagGet(FLAG_ROGUE_HARD_TRAINERS))
    {
        if(difficultyLevel == 0) // Last mon has competitive set
            return (preferCompetitive || Rogue_IsAnyBossTrainer(sTrainerScratch->trainerNum)) && monIdx == (totalMonCount - 1);
        else if(difficultyLevel == 1)
            return (preferCompetitive || Rogue_IsAnyBossTrainer(sTrainerScratch->trainerNum));
        else
            return TRUE;
    }
    else
    {
        // Start using competitive movesets on 3rd gym
        if(difficultyLevel == 0) // Last mon has competitive set
            return FALSE;
        else if(difficultyLevel == 1)
            return (preferCompetitive || Rogue_IsAnyBossTrainer(sTrainerScratch->trainerNum)) && monIdx == (totalMonCount - 1);
        else
            return (preferCompetitive || Rogue_IsAnyBossTrainer(sTrainerScratch->trainerNum));
    }

    return FALSE;
}

static bool8 SelectNextPreset(u16 species, struct RogueMonPreset* outPreset)
{
    u8 i;
    u8 presetCount = gPresetMonTable[species].presetCount;

    AGB_ASSERT(sTrainerScratch != NULL);

    if(presetCount != 0)
    {
        const struct RogueMonPreset* currPreset;
        bool8 isPresetValid;
        u8 randOffset = (presetCount == 1 ? 0 : RogueRandomRange(presetCount, FLAG_SET_SEED_TRAINERS));
        
        // Work from random offset and attempt to find the best preset which slots into this team
        // If none is found, we will use the last option and adjust below
        for(i = 0; i < presetCount; ++i)
        {
            currPreset = &gPresetMonTable[species].presets[((randOffset + i) % presetCount)];
            isPresetValid = TRUE;

            if(currPreset->heldItem == ITEM_LEFTOVERS && sTrainerScratch->heldItems.hasLeftovers)
            {
                isPresetValid = FALSE;
            }

            if(currPreset->heldItem == ITEM_SHELL_BELL && sTrainerScratch->heldItems.hasShellbell)
            {
                isPresetValid = FALSE;
            }

#ifdef ROGUE_EXPANSION
            if(!IsMegaEvolutionEnabled())
            {
                // Special case for primal reversion
                if(currPreset->heldItem == ITEM_RED_ORB || currPreset->heldItem == ITEM_BLUE_ORB)
                {
                    isPresetValid = FALSE;
                }
            }

            if(sTrainerScratch->heldItems.hasMegaStone || !IsMegaEvolutionEnabled())
            {
                if(currPreset->heldItem >= ITEM_VENUSAURITE && currPreset->heldItem <= ITEM_DIANCITE)
                {
                    isPresetValid = FALSE;
                }
            }

            if(sTrainerScratch->heldItems.hasZCrystal || !IsZMovesEnabled())
            {
                if(currPreset->heldItem >= ITEM_NORMALIUM_Z && currPreset->heldItem <= ITEM_ULTRANECROZIUM_Z)
                {
                    isPresetValid = FALSE;
                }
            }
#endif

            if(isPresetValid)
            {
                break;
            }
        }

        memcpy(outPreset, currPreset, sizeof(struct RogueMonPreset));

        // Swap out limited count items, if they already exist
        if(!isPresetValid)
        {
            if(outPreset->heldItem == ITEM_LEFTOVERS && sTrainerScratch->heldItems.hasLeftovers)
            {
                // Swap left overs to shell bell
                outPreset->heldItem = ITEM_SHELL_BELL;
            }

            if(outPreset->heldItem == ITEM_SHELL_BELL && sTrainerScratch->heldItems.hasShellbell)
            {
                // Swap shell bell to NONE (i.e. berry)
                outPreset->heldItem = ITEM_NONE;
            }

#ifdef ROGUE_EXPANSION
            if(!IsMegaEvolutionEnabled())
            {
                // Special case for primal reversion
                if(currPreset->heldItem == ITEM_RED_ORB || currPreset->heldItem == ITEM_BLUE_ORB)
                {
                    outPreset->heldItem = ITEM_NONE;
                }
            }

            if(sTrainerScratch->heldItems.hasMegaStone || !IsMegaEvolutionEnabled())
            {
                if(currPreset->heldItem >= ITEM_VENUSAURITE && currPreset->heldItem <= ITEM_DIANCITE)
                {
                    outPreset->heldItem = ITEM_NONE;
                }
            }

            if(sTrainerScratch->heldItems.hasZCrystal || !IsZMovesEnabled())
            {
                if(currPreset->heldItem >= ITEM_NORMALIUM_Z && currPreset->heldItem <= ITEM_ULTRANECROZIUM_Z)
                {
                    outPreset->heldItem = ITEM_NONE;
                }
            }
#endif
        }

        if(outPreset->heldItem == ITEM_NONE)
        {
            // Swap empty item to a berry either lum or sitrus
            outPreset->heldItem = RogueRandomRange(2, FLAG_SET_SEED_TRAINERS) == 0 ? ITEM_LUM_BERRY : ITEM_SITRUS_BERRY;
        }
        else if(outPreset->heldItem == ITEM_LEFTOVERS)
        {
            sTrainerScratch->heldItems.hasLeftovers = TRUE;
        }
        else if(outPreset->heldItem == ITEM_SHELL_BELL)
        {
            sTrainerScratch->heldItems.hasShellbell = TRUE;
        }
#ifdef ROGUE_EXPANSION
        else if(currPreset->heldItem >= ITEM_VENUSAURITE && currPreset->heldItem <= ITEM_DIANCITE)
        {
            sTrainerScratch->heldItems.hasMegaStone = TRUE;
        }
        else if(currPreset->heldItem >= ITEM_NORMALIUM_Z && currPreset->heldItem <= ITEM_ULTRANECROZIUM_Z)
        {
            sTrainerScratch->heldItems.hasZCrystal = TRUE;
        }
#endif

        return TRUE;
    }

    return FALSE;
}


static bool8 MonPresetHasChoiceItem(struct RogueMonPreset* preset)
{
    return preset->heldItem == ITEM_CHOICE_BAND
#ifdef ROGUE_EXPANSION
        || preset->heldItem == ITEM_CHOICE_SPECS
        || preset->heldItem == ITEM_CHOICE_SCARF
#endif
    ;
}

static u8 MonPresetCountMoves(struct RogueMonPreset* preset)
{
    u8 i;
    u8 count = 0;

    for(i = 0; i < MAX_MON_MOVES; ++i)
    {
        if(preset->moves[i] != MOVE_NONE)
            ++count;
    }

    return count;
}

static bool8 MonPresetReplaceMove(struct RogueMonPreset* preset, u16 fromMove, u16 toMove)
{
    u8 i;

    for(i = 0; i < MAX_MON_MOVES; ++i)
    {
        if(preset->moves[i] == fromMove)
            preset->moves[i] == toMove;
    }
}

static void ModifyTrainerMonPreset(struct RogueMonPreset* preset)
{
#ifndef ROGUE_EXPANSION
    // Vanilla only: AI can't use trick
    if(MonPresetReplaceMove(preset, MOVE_TRICK, MOVE_NONE))
        preset->allowMissingMoves = TRUE;
#endif

    // Edge case to handle scarfed ditto
    if(MonPresetHasChoiceItem(preset) && (MonPresetCountMoves(preset) > 2))
    {
        // Need to make sure this mon only has attacking moves
        u8 i = 0;
        preset->allowMissingMoves = TRUE;

        for(i = 0; i < MAX_MON_MOVES; ++i)
        {
            if(gBattleMoves[preset->moves[i]].power == 0)
                preset->moves[i] = MOVE_NONE;
        }
    }
}

static void SwapMons(u8 aIdx, u8 bIdx, struct Pokemon *party)
{
    if(aIdx != bIdx)
    {
        struct Pokemon tempMon;
        CopyMon(&tempMon, &party[aIdx], sizeof(struct Pokemon));

        CopyMon(&party[aIdx], &party[bIdx], sizeof(struct Pokemon));
        CopyMon(&party[bIdx], &tempMon, sizeof(struct Pokemon));
    }
}

// + go to the front - go to the back
s16 CalulcateMonSortScore(struct Pokemon* mon)
{
    s16 score = 0;
    u16 species = GetMonData(mon, MON_DATA_SPECIES);
    u16 item = GetMonData(mon, MON_DATA_HELD_ITEM);

#ifdef ROGUE_EXPANSION
    if(((item >= ITEM_VENUSAURITE && item <= ITEM_DIANCITE) || (item >= ITEM_NORMALIUM_Z && item <= ITEM_ULTRANECROZIUM_Z)))
    {
        score -= 20;
    }
#endif

    if(IsSpeciesLegendary(species))
    {
        score -= 20;
    }

    // Early pri moves
    //
    if(MonKnowsMove(mon, MOVE_FAKE_OUT))
    {
        score += 1;
    }
    if(MonKnowsMove(mon, MOVE_LIGHT_SCREEN))
    {
        score += 1;
    }
    if(MonKnowsMove(mon, MOVE_REFLECT))
    {
        score += 1;
    }
    if(MonKnowsMove(mon, MOVE_SPIKES))
    {
        score += 1;
    }

    if(MonKnowsMove(mon, MOVE_TAUNT))
    {
        score += 1;
    }

    if(MonKnowsMove(mon, MOVE_TRICK))
    {
        score += 1;
    }

    if(MonKnowsMove(mon, MOVE_TOXIC))
    {
        score += 1;
    }

    if(MonKnowsMove(mon, MOVE_BATON_PASS))
    {
        score += 1;

        // Only prioritse sub if we want to baton pass out
        if(MonKnowsMove(mon, MOVE_SUBSTITUTE))
        {
            score += 1;
        }
    }

#ifdef ROGUE_EXPANSION
    if(MonKnowsMove(mon, MOVE_U_TURN))
    {
        score += 1;
    }

    if(MonKnowsMove(mon, MOVE_FLIP_TURN))
    {
        score += 1;
    }

    if(MonKnowsMove(mon, MOVE_PARTING_SHOT))
    {
        score += 1;
    }

    if(MonKnowsMove(mon, MOVE_VOLT_SWITCH))
    {
        score += 1;
    }

    if(MonKnowsMove(mon, MOVE_TOXIC_SPIKES))
    {
        score += 1;
    }

    if(MonKnowsMove(mon, MOVE_STEALTH_ROCK))
    {
        score += 1;
    }

    if(MonKnowsMove(mon, MOVE_STICKY_WEB))
    {
        score += 1;
    }

    if(MonKnowsMove(mon, MOVE_TRICK_ROOM))
    {
        score += 1;
    }
#endif

    return score;
}

static void ReorderPartyMons(struct Pokemon *party, u8 monCount)
{
    bool8 keepExistingLead = FALSE;
    bool8 reorganiseParty = FALSE;
    bool8 clampLeadScore = FALSE;

    AGB_ASSERT(sTrainerScratch != NULL);

    if(Rogue_IsAnyBossTrainer(sTrainerScratch->trainerNum))
    {
        if(!FlagGet(FLAG_ROGUE_GAUNTLET_MODE) && !FlagGet(FLAG_ROGUE_HARD_TRAINERS) && gRogueRun.currentDifficulty < 8)
        {
            // Prior to E4 we don't want to force forward the best lead mon
            // We just want to push final mons to the back
            clampLeadScore = TRUE;
        }

        reorganiseParty = TRUE;
    }
    else
    {
        // Basic trainers don't care and can do whatever with their team order
        reorganiseParty = FALSE;
    }

    if(reorganiseParty)
    {
        u16 i;
        s16 scoreA, scoreB;
        bool8 anySwaps;
        u16 startIndex = keepExistingLead ? 1 : 0;
        u16 sortLength = monCount - 1;

        // Bubble sort party
        while(sortLength != 0)
        {
            anySwaps = FALSE;

            for(i = startIndex; i < monCount - 1; ++i)
            {
                scoreA = CalulcateMonSortScore(&party[i]);
                scoreB = CalulcateMonSortScore(&party[i + 1]);

                if(clampLeadScore)
                {
                    scoreA = min(scoreA, 0);
                    scoreB = min(scoreB, 0);
                }
                
                if(scoreB > scoreA)
                {
                    anySwaps = TRUE;
                    SwapMons(i, i + 1, party);
                }
            }
        
            if(anySwaps)
                --sortLength;
            else
                sortLength = 0;
        }
    }
}