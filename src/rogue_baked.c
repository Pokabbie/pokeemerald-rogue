//
// This file is shared between the game src and the offline bake to assist in making 
// queries and other stuff which can be prepared offline a bit faster
//
#include "constants/battle_ai.h"
#include "constants/items.h"
#include "constants/pokemon.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "constants/trainers.h"
#include "constants/form_change_types.h"

#ifdef ROGUE_BAKING
// Manually reinclude this if regenerating
#include "BakeHelpers.h"

#undef AGB_ASSERT
#define AGB_ASSERT(...)

#else
#include "global.h"
#include "data.h"
#include "event_data.h"
#include "graphics.h"
#include "item.h"
#include "item_use.h"
#include "party_menu.h"
#include "string_util.h"

#include "rogue_automation.h"
#include "rogue_campaign.h"
#include "rogue_controller.h"
#include "rogue_charms.h"
#include "rogue_pokedex.h"
#include "rogue_trainers.h"
#endif

#ifdef ROGUE_EXPANSION
#include "constants/form_change_types.h"

extern const struct SpeciesInfo gSpeciesInfo[];
#else
#define EVOLUTIONS_END 0

extern struct Evolution gEvolutionTable[][EVOS_PER_MON];
extern const struct BaseStats gBaseStats[];
#endif

#include "rogue_baked.h"


#ifdef ROGUE_BAKING
#define ROGUE_BAKE_INVALID
#else
// Swap to force runtime resolution
//#define ROGUE_BAKE_INVALID
#define ROGUE_BAKE_VALID
#endif

extern const u8 gText_TrainerName_Brendan[];
extern const u8 gText_TrainerName_May[];
extern const u8 gText_TrainerName_Red[];
extern const u8 gText_TrainerName_Leaf[];
extern const u8 gText_TrainerName_Ethan[];
extern const u8 gText_TrainerName_Lyra[];

extern const struct RogueItem gRogueItems[];

#ifdef ROGUE_BAKE_VALID
extern const struct RogueSpeciesBakedData gRogueBake_SpeciesData[NUM_SPECIES];
extern const u8 gRogueBake_PokedexVariantBitFlags[POKEDEX_VARIANT_COUNT][SPECIES_FLAGS_BYTE_COUNT];
extern const u16 gRogueBake_EvoItems[];
extern const u16 gRogueBake_EvoItems_Count;
extern const u16 gRogueBake_FormItems[];
extern const u16 gRogueBake_FormItems_Count;

extern const u16 gRogueBake_FinalEvoSpecies[];
extern const u16 gRogueBake_FinalEvoSpecies_Count;
extern const u16 gRogueBake_EggSpecies[];
extern const u16 gRogueBake_EggSpecies_Count;
#endif

void HistoryBufferPush(u16* buffer, u16 capacity, u16 value)
{
    u16 i;
    u16 j;
    for(i = 1; i < capacity; ++i)
    {
        // Reverse to avoid stomping on top of old values
        j = capacity - i - 1;
        buffer[j] = buffer[j - 1];
    }

    buffer[0] = value;
}

bool8 HistoryBufferContains(u16* buffer, u16 capacity, u16 value)
{
    u16 i;
    for(i = 0; i < capacity; ++i)
    {
        if(buffer[i] == value)
            return TRUE;
    }

    return FALSE;
}

u16 SelectIndexFromWeights(u16* weights, u16 count, u16 rngValue)
{
    u16 totalWeight;
    u16 targetWeight;
    u8 i;

    totalWeight = 0;
    for(i = 0; i < count; ++i)
    {
        totalWeight += weights[i];
    }

    targetWeight = rngValue % totalWeight;
    totalWeight = 0;

    for(i = 0; i < count; ++i)
    {
        totalWeight += weights[i];

        if(targetWeight < totalWeight)
        {
            return i;
        }
    }

    AGB_ASSERT(FALSE);
    return 0;
}

bool8 Rogue_CheckPokedexVariantFlag(u8 dexVariant, u16 species, bool8* result)
{
#ifdef ROGUE_BAKE_VALID
    if(dexVariant < POKEDEX_VARIANT_COUNT)
    {
        u16 idx = species / 8;
        u16 bit = species % 8;

        u8 bitMask = 1 << bit;

        *result = (gRogueBake_PokedexVariantBitFlags[dexVariant][idx] & bitMask) != 0;

        return TRUE;
    }
    else
    {
        AGB_ASSERT(FALSE);
    }
#endif
    return FALSE;
}

#ifdef ROGUE_EXPANSION
// For species where we want to override the source evos as this is preferred to editing source data that may change :/
//
static const struct Evolution sMeltanEvolutions[] =
{
    {EVO_NONE, 0, SPECIES_MELMETAL},
    {EVOLUTIONS_END, 0, 0},
};
#endif

static const struct Evolution* GetBaseEvolution(u16 species, u8 evoIdx)
{
#ifdef ROGUE_EXPANSION
    switch (species)
    {
    case SPECIES_MELTAN:
        return &sMeltanEvolutions[evoIdx];
    }
    
    // Eq. to GetSpeciesEvolutions
    return &gSpeciesInfo[species].evolutions[evoIdx];
#else
    return &gEvolutionTable[species][evoIdx];
#endif
}

#ifndef ROGUE_BAKE_VALID
static u8 GetMaxEvolutionCountInternal(u16 species)
{
    //EVOS_PER_MON

    u8 i;
    u8 count = 0;
    const struct Evolution* evo;

    evo = GetBaseEvolution(species, 0);

    if(evo == NULL)
        return 0;

    for(i = 0; evo->method != EVOLUTIONS_END; ++i)
    {
        evo = GetBaseEvolution(species, i);

        if(evo->method != EVOLUTIONS_END)
            ++count;
    }

    return count;
}
#endif

static void ModifyKnowMoveEvo(u16 species, struct Evolution* outEvo, u16 fromMethod, u16 toMethod)
{
    if(outEvo->method == fromMethod)
    {
        // While baking just assume everything is lvl 30 evo
#ifndef ROGUE_BAKING
        u16 i;

        for (i = 0; gRoguePokemonProfiles[species].levelUpMoves[i].move != MOVE_NONE; i++)
        {
            if(gRoguePokemonProfiles[species].levelUpMoves[i].move == outEvo->param)
            {
                outEvo->method = toMethod;
                outEvo->param = max(38, gRoguePokemonProfiles[species].levelUpMoves[i].level);
                break;
            }
        }
#endif

        if(outEvo->method == EVO_MOVE)
        {
            // Assume this was a tutor move
            outEvo->method = toMethod;
            outEvo->param = 30;
        }
    }
}

void Rogue_ModifyEvolution(u16 species, u8 evoIdx, struct Evolution* outEvo)
{
    //AGB_ASSERT(evoIdx < Rogue_GetMaxEvolutionCount(species));
    memcpy(outEvo, GetBaseEvolution(species, evoIdx), sizeof(*outEvo));

    // Any species alterations
    if(species == SPECIES_AZURILL && evoIdx == 0)
    {
        outEvo->method = EVO_LEVEL;
        outEvo->param = 10;
    }

#ifdef ROGUE_EXPANSION
    if(species == SPECIES_YAMASK_GALARIAN && evoIdx == 0)
    {
        outEvo->method = EVO_LEVEL;
        outEvo->param = 34;
    }

    if(species == SPECIES_MELTAN && evoIdx == 0)
    {
        outEvo->method = EVO_ITEM;
        outEvo->param = ITEM_METAL_COAT;
    }

    if(species == SPECIES_SLIGGOO && evoIdx == 0)
    {
        outEvo->method = EVO_LEVEL;
    }
    if((species == SPECIES_SLIGGOO || species == SPECIES_SLIGGOO_HISUIAN) && evoIdx == 1)
    {
        outEvo->targetSpecies = SPECIES_NONE;
        outEvo->method = EVO_NONE;
    }

    if(species == SPECIES_KUBFU && (evoIdx == 1 || evoIdx == 3))
    {
        outEvo->targetSpecies = SPECIES_NONE;
        outEvo->method = EVO_NONE;
    }

    if(species == SPECIES_BASCULIN_WHITE_STRIPED && (evoIdx == 0 || evoIdx == 1))
    {
        if(evoIdx == 0)
        {
            outEvo->targetSpecies = SPECIES_BASCULEGION_MALE;
            outEvo->method = EVO_LEVEL_MALE;
            outEvo->param = 36;
        }
        else if(evoIdx == 1)
        {
            outEvo->targetSpecies = SPECIES_BASCULEGION_FEMALE;
            outEvo->method = EVO_LEVEL_FEMALE;
            outEvo->param = 36;
        }
    }

    // Alola evos
    if(
        (species == SPECIES_PIKACHU && evoIdx == 1) ||
        (species == SPECIES_EXEGGCUTE && evoIdx == 1) ||
        (species == SPECIES_CUBONE && evoIdx == 1)
    )
    {
        outEvo->method = EVO_ITEM;
        outEvo->param = ITEM_ALOLA_STONE;
    }

    // Galar evos
    if(
        (species == SPECIES_KOFFING && evoIdx == 1) ||
        (species == SPECIES_MIME_JR && evoIdx == 1)
    )
    {
        outEvo->method = EVO_ITEM;
        outEvo->param = ITEM_GALAR_STONE;
    }

    // Hisui evos
    if(
        (species == SPECIES_QUILAVA && evoIdx == 1) ||
        (species == SPECIES_DEWOTT && evoIdx == 1) ||
        (species == SPECIES_PETILIL && evoIdx == 1) ||
        (species == SPECIES_RUFFLET && evoIdx == 1) ||
        (species == SPECIES_GOOMY && evoIdx == 1) ||
        (species == SPECIES_BERGMITE && evoIdx == 1) ||
        (species == SPECIES_DARTRIX && evoIdx == 1)
    )
    {
        outEvo->method = EVO_ITEM;
        outEvo->param = ITEM_HISUI_STONE;
    }

    if(species == SPECIES_URSARING && evoIdx == 0)
    {
        outEvo->method = EVO_ITEM;
        outEvo->param = ITEM_PEAT_BLOCK;
    }
    if(species == SPECIES_URSARING && evoIdx == 1)
    {
        outEvo->method = EVO_ITEM;
        outEvo->param = ITEM_MOON_STONE;
    }

    if(species == SPECIES_BISHARP && evoIdx == 0)
    {
        outEvo->method = EVO_ITEM;
        outEvo->param = ITEM_LEADERS_CREST;
    }

    // Walking evos
    if(species == SPECIES_PAWMO && evoIdx == 0)
    {
        outEvo->method = EVO_LEVEL;
        outEvo->param = 20;
    }
    if(species == SPECIES_BRAMBLIN && evoIdx == 0)
    {
        outEvo->method = EVO_LEVEL;
        outEvo->param = 20;
    }
    if(species == SPECIES_RELLOR && evoIdx == 0)
    {
        outEvo->method = EVO_LEVEL;
        outEvo->param = 20;
    }

    if(species == SPECIES_GIMMIGHOUL && evoIdx == 0)
    {
        outEvo->method = EVO_ITEM;
        outEvo->param = ITEM_GIMMIGHOUL_COIN;
    }

#endif

#ifndef ROGUE_BAKING
    if(outEvo->targetSpecies != SPECIES_NONE && (Rogue_IsRunActive() && !RoguePokedex_IsSpeciesEnabled(outEvo->targetSpecies)))
    {
        // Invalid evo
        outEvo->targetSpecies = SPECIES_NONE;
        outEvo->method = 0;
        return;
    }
#endif

    if(outEvo->targetSpecies != SPECIES_NONE)
    {
#ifdef ROGUE_EXPANSION
        if(species == SPECIES_MILCERY)
        {
            if(evoIdx == 0)
            {
                // Default form for neurtal natures
                outEvo->method = EVO_LEVEL;
                outEvo->targetSpecies = SPECIES_ALCREMIE_STRAWBERRY;
                outEvo->param = 30;
            }
            else
            {
                u16 supportedForms[] = 
                {
                    // SPECIES_ALCREMIE_STRAWBERRY // this is base form
                    SPECIES_ALCREMIE_BERRY,
                    SPECIES_ALCREMIE_LOVE,
                    SPECIES_ALCREMIE_STAR,
                    SPECIES_ALCREMIE_CLOVER,
                    SPECIES_ALCREMIE_FLOWER,
                    SPECIES_ALCREMIE_RIBBON,
                };
                u8 const supportedNatures[] = 
                {
                    NATURE_ADAMANT,
                    NATURE_LAX,
                    NATURE_MODEST,
                    NATURE_GENTLE,
                    NATURE_TIMID,
                    NATURE_JOLLY,

                    //NATURE_LONELY,
                    //NATURE_BRAVE,
                    //NATURE_ADAMANT,
                    //NATURE_NAUGHTY,
                    //NATURE_BOLD,
                    //NATURE_RELAXED,
                    //NATURE_IMPISH,
                    //NATURE_LAX,
                    //NATURE_TIMID,
                    //NATURE_HASTY,
                    //NATURE_JOLLY,
                    //NATURE_NAIVE,
                    //NATURE_MODEST,
                    //NATURE_MILD,
                    //NATURE_QUIET,
                    //NATURE_RASH,
                    //NATURE_CALM,
                    //NATURE_GENTLE,
                    //NATURE_SASSY,
                    //NATURE_CAREFUL,
                };

                if((evoIdx - 1) >= ARRAY_COUNT(supportedNatures))
                {
                    outEvo->method = EVO_NONE;
                    outEvo->targetSpecies = SPECIES_NONE;
                }
                else
                {
                    outEvo->method = EVO_LEVEL_30_NATURE;
                    outEvo->targetSpecies = supportedForms[evoIdx - 1];
                    outEvo->param = supportedNatures[evoIdx - 1];
                }
            }
        }
#endif

        ModifyKnowMoveEvo(species, outEvo, EVO_MOVE, EVO_LEVEL);
#ifdef ROGUE_EXPANSION
        ModifyKnowMoveEvo(species, outEvo, EVO_MOVE_TWO_SEGMENT, EVO_LEVEL_TWO_SEGMENT);
        ModifyKnowMoveEvo(species, outEvo, EVO_MOVE_THREE_SEGMENT, EVO_LEVEL_THREE_SEGMENT);
#endif

        switch(outEvo->method)
        {
            case(EVO_ITEM):
#ifdef ROGUE_EXPANSION
                if(outEvo->param == ITEM_LINKING_CORD)
                {
                    // We already have different evos via link cable
                    outEvo->method = EVO_NONE;
                    outEvo->targetSpecies = SPECIES_NONE;
                }
#endif
                break;

            case(EVO_BEAUTY):
#ifdef ROGUE_EXPANSION
                // Use prism scale evo
                outEvo->method = EVO_NONE;
                outEvo->targetSpecies = SPECIES_NONE;
#else 
                // Make level evo
                outEvo->method = EVO_LEVEL;
                outEvo->param = 36;
#endif
                break;

            case(EVO_FRIENDSHIP):
                outEvo->method = EVO_LEVEL;
                outEvo->param = 30;
                break;

            case(EVO_TRADE):
                outEvo->method = EVO_ITEM;
                outEvo->param = ITEM_LINK_CABLE;
                break;
            case(EVO_LEVEL_ITEM):
            case(EVO_ITEM_HOLD_DAY):
            case(EVO_ITEM_HOLD_NIGHT):
                outEvo->method = EVO_ITEM;
                break;

            case(EVO_FRIENDSHIP_DAY):
                if(species == SPECIES_EEVEE)
                {
                    outEvo->method = EVO_ITEM;
                    outEvo->param = ITEM_SUN_STONE;
                }
                else
                {
                    outEvo->method = EVO_LEVEL;
                    outEvo->param = 30;
                }
                break;
            case(EVO_FRIENDSHIP_NIGHT):
                if(species == SPECIES_EEVEE)
                {
                    outEvo->method = EVO_ITEM;
                    outEvo->param = ITEM_MOON_STONE;
                }
                else
                {
                    outEvo->method = EVO_LEVEL;
                    outEvo->param = 30;
                }
                break;

            case(EVO_LEVEL_DAY):
            case(EVO_LEVEL_NIGHT):
#ifdef ROGUE_EXPANSION
                // Rockruff & Cosmoem are the only species which still has time of day based evo
                if(species != GET_BASE_SPECIES_ID(SPECIES_ROCKRUFF) && species != GET_BASE_SPECIES_ID(SPECIES_COSMOEM))
#endif
                {
                    outEvo->method = EVO_LEVEL;
                }
                break;

            case(EVO_TRADE_ITEM):
#ifdef ROGUE_EXPANSION
                // These methods aren't needed now as Emerald Expansion has use item by default now
                outEvo->method = EVO_NONE;
                outEvo->targetSpecies = SPECIES_NONE;
#else
                outEvo->method = EVO_ITEM;
#endif
                break;

#ifdef ROGUE_EXPANSION
            case(EVO_SPECIFIC_MON_IN_PARTY):
            case(EVO_CRITICAL_HITS):
            case(EVO_SCRIPT_TRIGGER_DMG):
                outEvo->method = EVO_LEVEL;
                outEvo->param = 36;
                break;

            case(EVO_WATER_SCROLL):
                outEvo->method = EVO_ITEM;
                outEvo->param = ITEM_WATER_STONE;
                break;
            case(EVO_DARK_SCROLL):
                outEvo->method = EVO_ITEM;
                outEvo->param = ITEM_MOON_STONE;
                break;

            case(EVO_TRADE_SPECIFIC_MON):
                outEvo->method = EVO_ITEM;
                outEvo->param = ITEM_LINK_CABLE;
                break;

            case(EVO_LEVEL_RAIN):
            case(EVO_LEVEL_DARK_TYPE_MON_IN_PARTY):
                outEvo->method = EVO_LEVEL;
                break;

            case(EVO_FRIENDSHIP_MOVE_TYPE):
                outEvo->method = EVO_MOVE_TYPE;
                break;

            case(EVO_SPECIFIC_MAP):
            case(EVO_ITEM_NIGHT):
            case(EVO_ITEM_DAY):
                    outEvo->method = EVO_NONE;
                    outEvo->targetSpecies = SPECIES_NONE;
                break;

            case(EVO_MAPSEC):
                // These methods aren't needed now as Emerald Expansion has use item by default now
                outEvo->method = EVO_NONE;
                outEvo->targetSpecies = SPECIES_NONE;
                break;
#endif
        }
    }
}

void Rogue_ModifyEvolution_ApplyCurses(u16 species, u8 evoIdx, struct Evolution* outEvo)
{
#ifndef ROGUE_BAKING
    if(Rogue_IsRunActive() && outEvo->targetSpecies != SPECIES_NONE)
    {
        // Apply evo curse
        if(IsCurseActive(EFFECT_EVERSTONE_EVOS))
        {
#ifdef ROGUE_EXPANSION
            outEvo->method = EVO_NONE;
#else
            outEvo->method = 0;
#endif
            outEvo->targetSpecies = SPECIES_NONE;
        }
    }
#endif
}


#if defined(ROGUE_EXPANSION)
static const struct FormChange* GetBaseFormChange(u16 species)
{
#ifdef ROGUE_BAKING
    const struct FormChange* formChanges = gSpeciesInfo[species].formChangeTable;
    if (formChanges == NULL)
        return gSpeciesInfo[SPECIES_NONE].formChangeTable;
    return formChanges;
#else
    return GetSpeciesFormChanges(species);
#endif
}
#endif

void Rogue_ModifyFormChange(u16 species, u8 changeIdx, struct FormChange* outFormChange)
{
#if defined(ROGUE_EXPANSION)
    const struct FormChange *formChanges = GetBaseFormChange(species);

    if(formChanges != NULL)
    {
        memcpy(outFormChange, &formChanges[changeIdx], sizeof(*outFormChange));

#if !defined(ROGUE_BAKING)
        if(!IsMegaEvolutionEnabled())
        {
            if(
                outFormChange->method == FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM || 
                outFormChange->method == FORM_CHANGE_BATTLE_MEGA_EVOLUTION_MOVE || 
                outFormChange->method == FORM_CHANGE_BATTLE_PRIMAL_REVERSION)
            {
                outFormChange->method = FORM_CHANGE_DISABLED_STUB;
            }
        }

        if(!IsDynamaxEnabled())
        {
            if(outFormChange->method == FORM_CHANGE_BATTLE_GIGANTAMAX)
            {
                outFormChange->method = FORM_CHANGE_DISABLED_STUB;
            }
        }
#endif
    }
    else
    {
        outFormChange->method = FORM_CHANGE_TERMINATOR;
    }
#endif
}

const u8* Rogue_GetTrainerName(u16 trainerNum)
{
#if TESTING
    // Keep compat with EE tests
    return gText_TrainerName_Leaf;
#elif !defined(ROGUE_BAKING)
    const struct RogueTrainer* trainer = Rogue_GetTrainer(trainerNum);
    //if((trainer->trainerFlags & TRAINER_FLAG_NAME_IS_PLAYER))
    //{
    //    return gSaveBlock2Ptr->playerName;
    //}

    //if((trainer->trainerFlags & TRAINER_FLAG_NAME_IS_OPPOSITE_AVATAR))
    //{
    //    switch(gSaveBlock2Ptr->playerGender)
    //    {
    //        case(STYLE_EMR_BRENDAN):
    //            return gText_TrainerName_May;
    //        case(STYLE_EMR_MAY):
    //            return gText_TrainerName_Brendan;
//
    //        case(STYLE_RED):
    //            return gText_TrainerName_Leaf;
    //        case(STYLE_LEAF):
    //            return gText_TrainerName_Red;
//
    //        case(STYLE_ETHAN):
    //            return gText_TrainerName_Lyra;
    //        case(STYLE_LYRA):
    //            return gText_TrainerName_Ethan;
    //    };
    //}

    return trainer->trainerName;
#else
    return NULL;
#endif
}

static u16 ModifyTrainerClass(u16 trainerNum, u16 trainerClass, bool8 forMusic)
{
#ifndef ROGUE_BAKING
    if(forMusic)
    {
        // Repoint to keep the music player simple
        switch (trainerClass)
        {
        case TRAINER_CLASS_DEVELOPER:
            trainerClass = TRAINER_CLASS_RIVAL;
            break;

        case TRAINER_CLASS_COMMUNITY_MOD:
            trainerClass = TRAINER_CLASS_LEADER;
            break;
        }
    }

    // TODO - Always redirect TRAINER_CLASS_COMMUNITY_MOD, not just music, if in rainbow?

    if(trainerClass == TRAINER_CLASS_LEADER || trainerClass == TRAINER_CLASS_TOTEM_LEADER)
    {
        if(Rogue_IsVictoryLapActive())
        {
            const struct RogueTrainer* trainer = Rogue_GetTrainer(trainerNum);

            if(trainer->classFlags & CLASS_FLAG_BOSS_CHAMP)
            {
                trainerClass = TRAINER_CLASS_CHAMPION;
            }
            else if(trainer->classFlags & CLASS_FLAG_BOSS_ANY_ELITE)
            {
                trainerClass = TRAINER_CLASS_ELITE_FOUR;
            }
        }
        else if(Rogue_GetCurrentDifficulty() >= ROGUE_CHAMP_START_DIFFICULTY)
        {
            trainerClass = TRAINER_CLASS_CHAMPION;
        }
        else if(Rogue_GetCurrentDifficulty() >= ROGUE_ELITE_START_DIFFICULTY)
        {
            trainerClass = TRAINER_CLASS_ELITE_FOUR;
        }
    }
    else if(trainerClass == TRAINER_CLASS_RIVAL)
    {
        if(Rogue_GetCurrentDifficulty() >= ROGUE_FINAL_CHAMP_DIFFICULTY || Rogue_AssumeFinalQuestFakeChamp())
        {
            trainerClass = TRAINER_CLASS_CHAMPION;
        }
    }
    else if(trainerClass == TRAINER_CLASS_DEVELOPER)
    {
        if(Rogue_GetCurrentDifficulty() >= ROGUE_FINAL_CHAMP_DIFFICULTY || Rogue_AssumeFinalQuestFakeChamp())
        {
            trainerClass = TRAINER_CLASS_DEVELOPER_CHAMPION;
        }
    }
#endif
    return trainerClass;
}

void Rogue_ModifyTrainer(u16 trainerNum, struct Trainer* outTrainer)
{
#ifndef ROGUE_BAKING
    const struct RogueTrainer* trainer = Rogue_GetTrainer(trainerNum);

    // Early out for NULL trainer
    //if(trainerNum == 0)
    //    return;

    if(TRUE)
    {
        //struct RogueBattleMusic const* musicPlayer = Rogue_GetTrainerMusic(trainerNum);

        outTrainer->trainerClass = ModifyTrainerClass(trainerNum, trainer->trainerClass, FALSE);
        outTrainer->encounterMusic_gender = trainer->teamGenerator.preferredGender == MALE ? 0 : F_TRAINER_FEMALE; // not actually used for encounter music anymore
        outTrainer->trainerPic = trainer->trainerPic;

        //outTrainer->partyFlags = 0;
        outTrainer->doubleBattle = FALSE;
#ifdef ROGUE_EXPANSION
        outTrainer->aiFlags = AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_WILL_SUICIDE;

        if(Rogue_ShouldTrainerBeSmart(trainerNum))
             outTrainer->aiFlags |= AI_FLAG_TRY_TO_FAINT | AI_FLAG_HELP_PARTNER | AI_FLAG_SMART_MON_CHOICES;

        if(Rogue_ShouldTrainerTrySetup(trainerNum))
             outTrainer->aiFlags |= AI_FLAG_SETUP_FIRST_TURN;

        if(Rogue_ShouldTrainerSmartSwitch(trainerNum))
             outTrainer->aiFlags |= AI_FLAG_SMART_SWITCHING;

        if(Rogue_ShouldTrainerBeDoubleAware(trainerNum))
             outTrainer->aiFlags |= AI_FLAG_DOUBLE_BATTLE;

        if(Rogue_ShouldTrainerSaveAceMon(trainerNum))
            outTrainer->aiFlags |= AI_FLAG_ACE_POKEMON;

        if(IsCurseActive(EFFECT_AUTO_MOVE_SELECT) || Rogue_GetActiveCampaign() == ROGUE_CAMPAIGN_AUTO_BATTLER)
        {
            // AI will be dumb for this campaign
            outTrainer->aiFlags &= ~(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_WILL_SUICIDE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_HELP_PARTNER | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_SETUP_FIRST_TURN | AI_FLAG_DOUBLE_BATTLE);
        }
#else
        outTrainer->aiFlags = AI_SCRIPT_CHECK_BAD_MOVE | AI_SCRIPT_CHECK_VIABILITY;

        if(Rogue_ShouldTrainerBeSmart(trainerNum))
             outTrainer->aiFlags |= AI_SCRIPT_TRY_TO_FAINT;

        if(Rogue_ShouldTrainerTrySetup(trainerNum))
             outTrainer->aiFlags |= AI_SCRIPT_SETUP_FIRST_TURN;

        if(IsCurseActive(EFFECT_AUTO_MOVE_SELECT) || Rogue_GetActiveCampaign() == ROGUE_CAMPAIGN_AUTO_BATTLER)
        {
            // AI will be dumb for this campaign
            outTrainer->aiFlags = 0;
        }
#endif
#ifdef ROGUE_FEATURE_AUTOMATION
        else if(Rogue_AutomationGetFlag(AUTO_FLAG_TRAINER_RANDOM_AI))
        {
#ifdef ROGUE_EXPANSION
            // Still want AI to still do weird switching just for completeness?
            outTrainer->aiFlags = AI_FLAG_SMART_SWITCHING;
#else
            outTrainer->aiFlags = 0;
#endif
        }
#endif
    }
    else
    {
        // We shouldn't ever get here
        AGB_ASSERT(FALSE);
    }
#endif
}

void Rogue_ModifyBattleMusic(u16 musicType, u16 trainerSpecies, struct RogueBattleMusic* outMusic)
{
#ifndef ROGUE_BAKING
    u8 i;
    bool8 shouldRedirect;
    u16 trainerClass = 0;
    u16 baseSpecies = 0;
    struct RogueBattleMusic const* currMusic = NULL;
    const struct RogueTrainer* trainer = NULL;

    switch (musicType)
    {
    case BATTLE_MUSIC_TYPE_TRAINER:
        trainer = Rogue_GetTrainer(trainerSpecies);
        trainerClass = ModifyTrainerClass(trainerSpecies, trainer->trainerClass, TRUE);
        currMusic = &gRogueTrainerMusic[trainer->musicPlayer];
        break;
    
    case BATTLE_MUSIC_TYPE_WILD:
        if(RoguePokedex_IsSpeciesLegendary(trainerSpecies))
            currMusic = &gRogueTrainerMusic[BATTLE_MUSIC_LEGENDARY_BATTLE];
        else
            currMusic = &gRogueTrainerMusic[BATTLE_MUSIC_WILD_BATTLE];

        baseSpecies = trainerSpecies;
#ifdef ROGUE_EXPANSION
        baseSpecies = GET_BASE_SPECIES_ID(baseSpecies);
#endif
        break;
    }


    // Execute through redirection chain
    for(i = 0; i < currMusic->redirectCount;)
    {
        shouldRedirect = FALSE;

        switch (currMusic->redirects[i].redirectType)
        {
        case REDIRECT_PARAM_NONE:
            shouldRedirect = TRUE;
            break;
        
        case REDIRECT_PARAM_SPECIES:
            if(currMusic->redirects[i].redirectParam == baseSpecies)
                shouldRedirect = TRUE;
            break;
        
        case REDIRECT_PARAM_TRAINER_CLASS:
            if(currMusic->redirects[i].redirectParam == trainerClass)
                shouldRedirect = TRUE;
            break;

        default:
            AGB_ASSERT(FALSE);
            break;
        }

        if(shouldRedirect)
        {
            // Restart redirect checks for new player
            currMusic = &gRogueTrainerMusic[currMusic->redirects[i].musicPlayer];
            i = 0;
            continue;
        }
        
        // Only increment if we reach the end
        ++i;
    }

    // Copy to output so we can modify it
    memcpy(outMusic, currMusic, sizeof(struct RogueBattleMusic));

    // Check the preferred team gender and then swap out to female (It's just a lazy/easy mechanism that prevents a lot of duplicated data)
    if(outMusic->encounterMusic == MUS_ENCOUNTER_MALE && trainer->teamGenerator.preferredGender != MALE)
    {
        outMusic->encounterMusic = MUS_ENCOUNTER_FEMALE;
    }
    else if(outMusic->encounterMusic == MUS_HG_ENCOUNTER_BOY_1 && trainer->teamGenerator.preferredGender != MALE)
    {
        outMusic->encounterMusic = MUS_HG_ENCOUNTER_BOY_2; // ?
    }
    if(outMusic->encounterMusic == MUS_ENCOUNTER_BRENDAN && trainer->teamGenerator.preferredGender != MALE)
    {
        outMusic->encounterMusic = MUS_ENCOUNTER_MAY;
    }
#endif
}

static u16 SanitizeItemId(u16 itemId)
{
#ifdef ROGUE_FEATURE_REMOVE_HIDDEN_MACHINES
    if(itemId >= ITEM_HM01 && itemId <= ITEM_HM08)
        return ITEM_NONE;
#endif

    if (itemId >= ITEMS_COUNT)
        return ITEM_NONE;
    else
        return itemId;
}

#ifdef ROGUE_BAKING
// DUDs
const u8* Rogue_GetItemName(u16 itemId)
{
    return NULL;
}

const u8* Rogue_GetItemDesc(u16 itemIdx)
{
    return NULL;
}

u16 Rogue_GetPrice(u16 itemId)
{
    return 0;
}

void Rogue_ModifyItem(u16 itemId, struct Item* outItem)
{
}

u32 Rogue_CalculateMovePrice(u16 move)
{
    return 0;
}
#else

extern const u8 gText_EscapeRopeDesc[];
extern const u8 gItemDesc_TM[];
extern const u8 gItemDesc_TR[];
extern const u8 gItemDesc_MaxMushroom[];

extern const u32 *const gItemIconTable[][2];

const u8* Rogue_GetItemName(u16 itemId)
{
    itemId = SanitizeItemId(itemId);

    if(itemId >= ITEM_ROGUE_ITEM_FIRST && itemId <= ITEM_ROGUE_ITEM_LAST)
    {
        return gRogueItems[itemId - ITEM_ROGUE_ITEM_FIRST].name;
    }

    return gItems[itemId].name;
}

const u8* Rogue_GetItemDesc(u16 itemId)
{
    itemId = SanitizeItemId(itemId);

    if(itemId >= ITEM_TM01 && itemId <= ITEM_HM08)
    {
        return gItemDesc_TM;
    }
    else if(itemId >= ITEM_TR01 && itemId <= ITEM_TR50)
    {
        return gItemDesc_TR;
    }

    if(itemId >= ITEM_ROGUE_ITEM_FIRST && itemId <= ITEM_ROGUE_ITEM_LAST)
    {
        // Uncomment if strange item are appearing due to ID gaps
        //DebugPrintf("itemId:%u", (u32)itemId);
        //DebugPrintf("rogueId:%u", (u32)(itemId - ITEM_ROGUE_ITEM_FIRST));

        return gRogueItems[itemId - ITEM_ROGUE_ITEM_FIRST].description;
    }
    
    switch(itemId)
    {
        case ITEM_ESCAPE_ROPE:
            return gText_EscapeRopeDesc;

#ifdef ROGUE_EXPANSION
        case ITEM_MAX_MUSHROOMS:
            return gItemDesc_MaxMushroom;
#endif
    }

    return gItems[itemId].description;
}

const void* Rogue_GetItemIconPicOrPalette(u16 itemId, u8 which)
{
    itemId = SanitizeItemId(itemId);

    if((itemId >= ITEM_TM01 && itemId <= ITEM_HM08) || (itemId >= ITEM_TR01 && itemId <= ITEM_TR50))
    {
        if(which == 0)
        {
            // TMs are visually HMs to indicate infinite usage?
            if(itemId >= ITEM_TR01 && itemId <= ITEM_TR50)
                return gItemIcon_TM;
            else
                return gItemIcon_HM;
        }
        else
        {
            u16 move = ItemIdToBattleMoveId(itemId);
            switch (gBattleMoves[move].type)
            {
            case TYPE_FIGHTING:
                return gItemIconPalette_FightingTMHM;

            case TYPE_FLYING:
                return gItemIconPalette_FlyingTMHM;

            case TYPE_POISON:
                return gItemIconPalette_PoisonTMHM;

            case TYPE_GROUND:
                return gItemIconPalette_GroundTMHM;

            case TYPE_ROCK:
                return gItemIconPalette_RockTMHM;

            case TYPE_BUG:
                return gItemIconPalette_BugTMHM;

            case TYPE_GHOST:
                return gItemIconPalette_GhostTMHM;

            case TYPE_STEEL:
                return gItemIconPalette_SteelTMHM;

            case TYPE_FIRE:
                return gItemIconPalette_FireTMHM;

            case TYPE_WATER:
                return gItemIconPalette_WaterTMHM;

            case TYPE_GRASS:
                return gItemIconPalette_GrassTMHM;

            case TYPE_ELECTRIC:
                return gItemIconPalette_ElectricTMHM;

            case TYPE_PSYCHIC:
                return gItemIconPalette_PsychicTMHM;

            case TYPE_ICE:
                return gItemIconPalette_IceTMHM;

            case TYPE_DRAGON:
                return gItemIconPalette_DragonTMHM;

            case TYPE_DARK:
                return gItemIconPalette_DarkTMHM;

#ifdef ROGUE_EXPANSION
            case TYPE_FAIRY:
                return gItemIconPalette_FairyTMHM;
#endif
            
            default: // TYPE_NORMAL, TYPE_MYSTERY
                return gItemIconPalette_NormalTMHM;
            }

        }
    }

    if(itemId >= ITEM_ROGUE_ITEM_FIRST && itemId <= ITEM_ROGUE_ITEM_LAST)
    {
        return which == 0 ? gRogueItems[itemId - ITEM_ROGUE_ITEM_FIRST].iconImage : gRogueItems[itemId - ITEM_ROGUE_ITEM_FIRST].iconPalette;
    }

    return gItemIconTable[itemId][which];
}

#define HELD_ITEM_HIGH_PRICE 8000
#define HELD_ITEM_MID_PRICE  5000

u16 Rogue_GetPrice(u16 itemId)
{
    bool8 applyDefaultHubIncrease = FALSE;
    u16 price = 0;
    struct Item item;

    itemId = SanitizeItemId(itemId);
    Rogue_ModifyItem(itemId, &item);

    AGB_ASSERT(itemId < ITEMS_COUNT);

    if(itemId >= ITEM_ROGUE_ITEM_FIRST && itemId <= ITEM_ROGUE_ITEM_LAST)
        price = gRogueItems[itemId - ITEM_ROGUE_ITEM_FIRST].price;
    else
        price = gItems[itemId].price;


    if(itemId == ITEM_NONE)
        return 0;

    // Range edits
    if(itemId >= ITEM_HP_UP && itemId <= ITEM_PP_MAX)
    {
        price = 4000;
    }

    if(itemId >= ITEM_TR01 && itemId <= ITEM_TR50)
    {
        u16 move = ItemIdToBattleMoveId(itemId);
        price = Rogue_CalculateMovePrice(move);
    }

    if(itemId >= ITEM_TM01 && itemId <= ITEM_HM08)
    {
        u16 move = ItemIdToBattleMoveId(itemId);

        // increase as these are re-usable
        price = Rogue_CalculateMovePrice(move) * 4;
        applyDefaultHubIncrease = TRUE;
    }

    // Set hold items price based on usage and override specifics below
    if(item.holdEffect != 0 && item.pocket != POCKET_BERRIES)
    {
        applyDefaultHubIncrease = TRUE;
        price = 1000 + (min(gRoguePokemonHeldItemUsages[itemId], 280) / 4) * 100;
    }

#ifdef ROGUE_EXPANSION
    if(itemId >= ITEM_X_ATTACK && itemId <= ITEM_GUARD_SPEC)
#else
    if(itemId >= ITEM_GUARD_SPEC && itemId <= ITEM_X_SPECIAL)
#endif
    {
        price = 1500;
    }

    if((itemId >= FIRST_BERRY_INDEX && itemId <= LAST_BERRY_INDEX))
    {
        price = 50;
    }

    if((itemId >= FIRST_ITEM_POKEBLOCK && itemId <= LAST_ITEM_POKEBLOCK))
    {
        price = 50;
    }

    if(Rogue_IsEvolutionItem(itemId))
    {
        price = 2100;
        applyDefaultHubIncrease = FALSE;
    }

#ifdef ROGUE_EXPANSION
    if(itemId >= ITEM_LEVEL_BALL && itemId <= ITEM_CHERISH_BALL)
    {
        price = 2500;
    }

    if(itemId >= ITEM_RED_NECTAR && itemId <= ITEM_PURPLE_NECTAR)
    {
        price = 2100;
    }

    if(itemId >= ITEM_RED_ORB && itemId <= ITEM_DIANCITE)
    {
        // Expect price from above
        price = HELD_ITEM_HIGH_PRICE;
        applyDefaultHubIncrease = TRUE;
    }

    if(itemId >= ITEM_NORMALIUM_Z && itemId <= ITEM_ULTRANECROZIUM_Z)
    {
        // Expect price from above
        price = HELD_ITEM_MID_PRICE;
        applyDefaultHubIncrease = TRUE;
    }

    if(itemId >= ITEM_ROTOM_CATALOG && itemId <= ITEM_REINS_OF_UNITY)
    {
        price = 0;
    }

    if(itemId >= ITEM_LONELY_MINT && itemId <= ITEM_SERIOUS_MINT)
    {
        price = 1500;
    }

    // Plates
    if(itemId >= ITEM_FLAME_PLATE && itemId <= ITEM_PIXIE_PLATE)
    {
        applyDefaultHubIncrease = TRUE;
        price = HELD_ITEM_MID_PRICE;
    }

    if(itemId >= ITEM_DOUSE_DRIVE && itemId <= ITEM_CHILL_DRIVE)
    {
        applyDefaultHubIncrease = TRUE;
        price = HELD_ITEM_MID_PRICE;
    }

    if(itemId >= ITEM_FIRE_MEMORY && itemId <= ITEM_FAIRY_MEMORY)
    {
        applyDefaultHubIncrease = TRUE;
        price = HELD_ITEM_MID_PRICE;
    }

    if(itemId >= ITEM_ADAMANT_CRYSTAL && itemId <= ITEM_LUSTROUS_GLOBE)
    {
        applyDefaultHubIncrease = TRUE;
        price = HELD_ITEM_HIGH_PRICE;
    }

    if(itemId >= ITEM_CORNERSTONE_MASK && itemId <= ITEM_HEARTHFLAME_MASK)
    {
        applyDefaultHubIncrease = TRUE;
        price = HELD_ITEM_HIGH_PRICE;
    }
    
    if((itemId >= ITEM_BUG_TERA_SHARD && itemId <= ITEM_WATER_TERA_SHARD) || itemId == ITEM_STELLAR_TERA_SHARD)
    {
        price = HELD_ITEM_MID_PRICE + 1000;
    }

#endif

    // Individual items
    switch(itemId)
    {
        case ITEM_REVIVE:
            price = 2000;
            break;
        case ITEM_MAX_REVIVE:
            price = 4000;
            break;

        case ITEM_PP_UP:
            price = 2000;
            break;

        case ITEM_ESCAPE_ROPE:
            price = Rogue_IsRunActive() ? 8000 : 16000;
            break;

        case ITEM_MASTER_BALL:
            price = 50000;
            break;

        case ITEM_NUGGET:
            price = 1000;
            break;

        case ITEM_PEARL:
            price = 1500;
            break;

        case ITEM_BIG_PEARL:
            price = 2000;
            break;

        case ITEM_STARDUST:
            price = 3000;
            break;

        case ITEM_STAR_PIECE:
            price = 4000;
            break;

        case ITEM_SACRED_ASH:
            price = 0;
            break;
    
        case ITEM_SOUL_DEW:
            applyDefaultHubIncrease = TRUE;
            price = HELD_ITEM_MID_PRICE;
            break;

#ifdef ROGUE_EXPANSION
        case ITEM_ABILITY_CAPSULE:
            price = 6000;
            break;

        case ITEM_ABILITY_PATCH:
            price = 7000;
            break;

        // Weaker versions
        case ITEM_ADAMANT_ORB:
        case ITEM_LUSTROUS_ORB:
        case ITEM_GRISEOUS_ORB:
            applyDefaultHubIncrease = TRUE;
            price = HELD_ITEM_MID_PRICE;
            break;

        case ITEM_RUSTED_SWORD:
        case ITEM_RUSTED_SHIELD:
            applyDefaultHubIncrease = TRUE;
            price = HELD_ITEM_HIGH_PRICE;
            break;

        case ITEM_MAX_MUSHROOMS:
            //applyDefaultHubIncrease = TRUE;
            price = HELD_ITEM_HIGH_PRICE;
            break;

        case ITEM_DUSK_BALL:
        case ITEM_TIMER_BALL:
        case ITEM_QUICK_BALL:
        case ITEM_BEAST_BALL:
            price = 2500;
            break;

        case ITEM_REPEAT_BALL:
        case ITEM_LEVEL_BALL:
        case ITEM_CHERISH_BALL:
            price = 2000;
            break;

        case ITEM_LURE_BALL:
        case ITEM_MOON_BALL:
        case ITEM_FRIEND_BALL:
        case ITEM_LOVE_BALL:
        case ITEM_FAST_BALL:
        case ITEM_HEAVY_BALL:
        case ITEM_DREAM_BALL:
            price = 1500;
            break;
    
        case ITEM_BERSERK_GENE:
            price = 10000;
            break;
#endif

        case ITEM_RARE_CANDY:
            price = 1000;
            break;

#ifdef ROGUE_EXPANSION
        case ITEM_SPORT_BALL:
        case ITEM_PARK_BALL:
#endif
        case ITEM_SAFARI_BALL:
            price = 0;
            break;
    }

    // Hub is more pricy!
    if(applyDefaultHubIncrease && !Rogue_IsRunActive())
    {
        price *= 2;
    }

    return price;
}

static bool8 IsCharmItem(u16 itemId)
{
    return (itemId >= FIRST_ITEM_CHARM && itemId <= LAST_ITEM_CHARM) || (itemId >= FIRST_ITEM_CURSE && itemId <= LAST_ITEM_CURSE);
}

void Rogue_ModifyItem(u16 itemId, struct Item* outItem)
{
    itemId = SanitizeItemId(itemId);
    AGB_ASSERT(itemId < ITEMS_COUNT);

    // Rogue items copy in parts
    //
    if(itemId >= ITEM_ROGUE_ITEM_FIRST && itemId <= ITEM_ROGUE_ITEM_LAST)
    {
        const struct RogueItem* rogueItem;
        bool8 isValidItem;

        if(itemId >= ITEM_TR01 && itemId <= ITEM_TR50)
        {
            // Copy from first entry, as these items are going to be dynamic anyway
            rogueItem = &gRogueItems[ITEM_TR01 - ITEM_ROGUE_ITEM_FIRST];
            isValidItem = TRUE;
        }
        else
        {
            rogueItem = &gRogueItems[itemId - ITEM_ROGUE_ITEM_FIRST];
            isValidItem = rogueItem->itemId == itemId;
        }

        if(isValidItem)
        {
            //outItem->itemId = itemId;
            outItem->holdEffect = rogueItem->holdEffect;
            outItem->holdEffectParam = rogueItem->holdEffectParam;
            //outItem->registrability = rogueItem->registrability;
            outItem->pocket = rogueItem->pocket;
            outItem->type = rogueItem->type;
            outItem->fieldUseFunc = rogueItem->fieldUseFunc;
            outItem->battleUsage = rogueItem->battleUsage;
            outItem->secondaryId = rogueItem->secondaryId;

            if(IsCharmItem(itemId))
            {
                // Early out here for charms (We can end up in an infinite loop below)
                outItem->importance = 1;
                return;
            }

            if(outItem->pocket == POCKET_KEY_ITEMS)
            {
                outItem->importance = 1;
            }
        }
        else
        {
            itemId = ITEM_NONE;
            memcpy(outItem, &gItems[ITEM_NONE], sizeof(struct Item));
        }
    }
    else
    {
        memcpy(outItem, &gItems[itemId], sizeof(struct Item));
    }

    if(itemId == ITEM_NONE)
        return;

    // Range edits
    if(itemId >= ITEM_HP_UP && itemId <= ITEM_PP_MAX)
    {
        outItem->pocket = POCKET_MEDICINE;
    }

#ifdef ROGUE_EXPANSION
    if(itemId >= ITEM_X_ATTACK && itemId <= ITEM_GUARD_SPEC)
#else
    if(itemId >= ITEM_GUARD_SPEC && itemId <= ITEM_X_SPECIAL)
#endif
    {
        outItem->pocket = POCKET_MEDICINE;
    }

    if(outItem->pocket == POCKET_ITEMS && 
        (
            outItem->fieldUseFunc == ItemUseOutOfBattle_Medicine ||
            outItem->fieldUseFunc == ItemUseOutOfBattle_ReduceEV ||
            outItem->fieldUseFunc == ItemUseOutOfBattle_SacredAsh ||
            outItem->fieldUseFunc == ItemUseOutOfBattle_PPRecovery ||
            outItem->fieldUseFunc == ItemUseOutOfBattle_PPUp ||
            outItem->fieldUseFunc == ItemUseOutOfBattle_RareCandy
        ))
    {
        outItem->pocket = POCKET_MEDICINE;
    }

    if((itemId >= FIRST_BERRY_INDEX && itemId <= LAST_BERRY_INDEX))
    {
        outItem->pocket = POCKET_BERRIES;
    }


    // Don't move evo items into held item pocket
    if(Rogue_IsEvolutionItem(itemId))
    {
        outItem->pocket = POCKET_ITEMS; 

        // Become evo items
        outItem->type = ITEM_USE_PARTY_MENU;
        outItem->fieldUseFunc = ItemUseOutOfBattle_EvolutionStone;

    }
    else if(outItem->holdEffect != 0 && outItem->pocket != POCKET_BERRIES)
    {
        // Don't move evo items into held item pocket
        outItem->pocket = POCKET_HELD_ITEMS;
    }

#ifdef ROGUE_EXPANSION
    if(itemId >= ITEM_VENUSAURITE && itemId <= ITEM_DIANCITE)
    {
        outItem->pocket = POCKET_STONES;
    }

    if(itemId >= ITEM_NORMALIUM_Z && itemId <= ITEM_ULTRANECROZIUM_Z)
    {
        outItem->pocket = POCKET_STONES;
    }

    if(itemId >= ITEM_DOUSE_DRIVE && itemId <= ITEM_CHILL_DRIVE)
    {
        outItem->pocket = POCKET_STONES;
    }

    if(itemId >= ITEM_FLAME_PLATE && itemId <= ITEM_PIXIE_PLATE)
    {
        outItem->pocket = POCKET_STONES;
    }

    if(itemId >= ITEM_FIRE_MEMORY && itemId <= ITEM_FAIRY_MEMORY)
    {
        outItem->pocket = POCKET_STONES;
    }

    if(itemId >= ITEM_ADAMANT_CRYSTAL && itemId <= ITEM_LUSTROUS_GLOBE)
    {
        outItem->pocket = POCKET_STONES;
    }

    if(itemId >= ITEM_CORNERSTONE_MASK && itemId <= ITEM_HEARTHFLAME_MASK)
    {
        outItem->pocket = POCKET_STONES;
    }

    if((itemId >= ITEM_BUG_TERA_SHARD && itemId <= ITEM_WATER_TERA_SHARD) || itemId == ITEM_STELLAR_TERA_SHARD)
    {
        outItem->type = ITEM_USE_PARTY_MENU,
        outItem->fieldUseFunc = ItemUseOutOfBattle_TeraShard,
        outItem->pocket = POCKET_STONES;
    }

    if(itemId == ITEM_MAX_MUSHROOMS)
    {
        outItem->type = ITEM_USE_PARTY_MENU,
        outItem->battleUsage = 0,
        outItem->fieldUseFunc = ItemUseOutOfBattle_MaxMushroom,
        outItem->pocket = POCKET_STONES;
    }
#endif

    // Individual items
    switch(itemId)
    {
        case ITEM_SACRED_ASH:
            outItem->pocket = POCKET_KEY_ITEMS;
            break;

        case ITEM_ESCAPE_ROPE:
            outItem->pocket = POCKET_ITEMS;
            break;

        case ITEM_NUGGET:
        case ITEM_PEARL:
        case ITEM_BIG_PEARL:
        case ITEM_STARDUST:
        case ITEM_STAR_PIECE:
            outItem->holdEffect = 0;
            break;
    
        case ITEM_TOWN_MAP:
            outItem->type = ITEM_USE_FIELD;
            outItem->fieldUseFunc = ItemUseOutOfBattle_WorldMap;
            break;

        case ITEM_SOUL_DEW:
#ifdef ROGUE_EXPANSION
            outItem->pocket = POCKET_STONES;
#endif
            break;

#ifdef ROGUE_EXPANSION
        case ITEM_ABILITY_CAPSULE:
        case ITEM_ABILITY_PATCH:
            outItem->pocket = POCKET_MEDICINE;
            break;

        case ITEM_RED_ORB:
        case ITEM_BLUE_ORB:
            outItem->pocket = POCKET_STONES;
            break;

        // Weaker versions
        case ITEM_ADAMANT_ORB:
        case ITEM_LUSTROUS_ORB:
        case ITEM_GRISEOUS_ORB:
            outItem->pocket = POCKET_STONES;
            break;

        case ITEM_RUSTED_SWORD:
        case ITEM_RUSTED_SHIELD:
            outItem->pocket = POCKET_STONES;
            break;
#endif
    }

    // Check we're not a charm/curse otherwise we can get infinite loops here
    {
        if(outItem->pocket == POCKET_POKE_BALLS)
        {
            if(IsCurseActive(EFFECT_SNAG_TRAINER_MON) && !FlagGet(FLAG_ROGUE_IN_SNAG_BATTLE))
                outItem->battleUsage = 0;
        }
        else
        {
            if(IsCurseActive(EFFECT_BATTLE_ITEM_BAN))
                outItem->battleUsage = 0;
        }
    }
}

u32 Rogue_CalculateMovePrice(u16 move)
{
    // Move cost takes into account high level stats and then modifies based on usage
    u32 cost = 0;
    u32 usageCount = gRoguePokemonMoveUsages[move];
    u8 accuracy = gBattleMoves[move].accuracy;
    u8 pp = gBattleMoves[move].pp;
    u8 power = gBattleMoves[move].power;

    AGB_ASSERT(move < MOVES_COUNT);

    // Move specific costs
    switch (move)
    {
    case MOVE_BATON_PASS:
        return 3500;
    }

    switch (move)
    {
    case MOVE_RETURN:
    case MOVE_FRUSTRATION:
        power = 110;
        break;
    
    case MOVE_HIDDEN_POWER:
        power = 70;
        break;

    case MOVE_SPLASH:
        power = 1;
        break;
    }

    // accuracy cost
    if(accuracy == 100 || accuracy == 0)
        cost += 500;
    else if(accuracy >= 90)
        cost += 250;
    else if(accuracy >= 75)
        cost += 200;
    else if(accuracy >= 50)
        cost += 100;

    // pp cost
    if(pp <= 5)
        cost += 1000;
    else if(pp <= 10)
        cost += 500;
    else if(pp <= 20)
        cost += 250;
    else if(pp <= 30)
        cost += 100;

    // power cost
    if(power == 0) // is status move
        cost += 1000;
    else if(power >= 110)
        cost += 2000;
    else if(power >= 100)
        cost += 1500;
    else if(power >= 90)
        cost += 1000;
    else if(power >= 70)
        cost += 500;
    else if(power >= 50)
        cost += 250;
    else
        cost += 150;

    // Modify based on usage
    if(usageCount >= 300)
        cost += 3000;
    else if(usageCount >= 200)
        cost += 2500;
    else if(usageCount >= 100)
        cost += 2000;
    else if(usageCount >= 75)
        cost += 2000;
    else if(usageCount >= 50)
        cost += 1500;
    else if(usageCount >= 20)
        cost += 1000;
    else if(usageCount >= 100)
        cost += 500;

    if(cost < 100)
        cost = 100;

    return cost;
}
#endif

#ifndef ROGUE_BAKE_VALID
static bool8 IsEvolutionItemInternal(u16 itemId)
{
    struct Evolution evo;
    u16 s, e, evoCount;

    for (s = SPECIES_NONE + 1; s < NUM_SPECIES; ++s)
    {
        evoCount = Rogue_GetMaxEvolutionCount(s);

        for (e = 0; e < evoCount; ++e)
        {
            Rogue_ModifyEvolution(s, e, &evo);

            switch (evo.method)
            {
            case EVO_ITEM:
#ifdef ROGUE_EXPANSION
            case EVO_ITEM_MALE:
            case EVO_ITEM_FEMALE:
            case EVO_ITEM_DAY:
            case EVO_ITEM_NIGHT:
#endif
                if (evo.param == itemId)
                    return TRUE;
                break;
            }
        }
    }

    return FALSE;
}
#endif

static u16 BinarySearchForItem(u16 item, u16 const* data, u16 count)
{
    u16 minIndex, maxIndex, currIndex;

    if(count == 0)
        return count;

    // Binary search for item
    minIndex = 0;
    maxIndex = count - 1;
    do
    {
        currIndex = (maxIndex + minIndex) / 2;

        if(data[currIndex] == item)
            return currIndex;
        
        if(item < data[currIndex])
            maxIndex = currIndex;
        else
        {
            if(minIndex == currIndex)
                ++currIndex;

            minIndex = currIndex;
        }
    }
    while(minIndex != maxIndex);

    // Final check
    if(data[currIndex] == item)
        return currIndex;

    // Failed to find
    return count;
}

bool8 Rogue_IsEvolutionItem(u16 itemId)
{
#ifdef ROGUE_BAKE_VALID
    u16 findIndex = BinarySearchForItem(itemId, gRogueBake_EvoItems, gRogueBake_EvoItems_Count);
    return findIndex != gRogueBake_EvoItems_Count;
#else
    return IsEvolutionItemInternal(itemId);
#endif
}

u16 Rogue_GetEvolutionItemIndex(u16 itemId)
{
#ifdef ROGUE_BAKE_VALID
    u16 findIndex = BinarySearchForItem(itemId, gRogueBake_EvoItems, gRogueBake_EvoItems_Count);
    AGB_ASSERT(findIndex != gRogueBake_EvoItems_Count);
    return findIndex;
#else
    return 0;
#endif
}

#ifndef ROGUE_BAKE_VALID
static bool8 IsFormItemInternal(u16 itemId)
{
#ifdef ROGUE_EXPANSION
    struct FormChange form;
    u16 s, e, formCount;

    for (s = SPECIES_NONE + 1; s < NUM_SPECIES; ++s)
    {
        formCount = Rogue_GetActiveFormChangeCount(s);

        for (e = 0; e < formCount; ++e)
        {
            Rogue_ModifyFormChange(s, e, &form);

            switch (form.method)
            {
            case FORM_CHANGE_ITEM_HOLD:
            case FORM_CHANGE_ITEM_USE:
            case FORM_CHANGE_BEGIN_BATTLE:
            case FORM_CHANGE_END_BATTLE:
            case FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM:
            case FORM_CHANGE_BATTLE_PRIMAL_REVERSION:
                if (form.param1 == itemId)
                    return TRUE;
                break;
            }
        }
    }
#endif

    return FALSE;
}
#endif

#ifdef ROGUE_EXPANSION

bool8 Rogue_IsFormItem(u16 itemId)
{
#ifdef ROGUE_BAKE_VALID
    u16 findIndex = BinarySearchForItem(itemId, gRogueBake_FormItems, gRogueBake_FormItems_Count);
    return findIndex != gRogueBake_FormItems_Count;
#else
    return IsFormItemInternal(itemId);
#endif
}

u16 Rogue_GetFormItemIndex(u16 itemId)
{
#ifdef ROGUE_BAKE_VALID
    u16 findIndex = BinarySearchForItem(itemId, gRogueBake_FormItems, gRogueBake_FormItems_Count);
    AGB_ASSERT(findIndex != gRogueBake_FormItems_Count);
    return findIndex;
#else
    return 0;
#endif
}

#else

bool8 Rogue_IsFormItem(u16 itemId)
{
    return FALSE;
}

u16 Rogue_GetFormItemIndex(u16 itemId)
{
    return 0;
}

#endif

u32 Rogue_ModifyExperienceTables(u8 growthRate, u8 level)
{
    // Originallu from const u32 gExperienceTables[][MAX_LEVEL + 1]
    // But want to ideally fit all EXP within u16 since we earn it differently in Rogue anyway
    return level * 300;//MAX_LEVEL;
}


// Taken straight from daycare
u16 Rogue_GetEggSpecies(u16 species)
{
#ifdef ROGUE_BAKE_VALID
    return gRogueBake_SpeciesData[species].eggSpecies;

#else
    u16 e, s, spe;
    bool8 found;
    u8 evo, evoCount;
    struct Evolution evolution;

    // Working backwards up to 5 times seems arbitrary, since the maximum number
    // of times would only be 3 for 3-stage evolutions.
    for (e = 0; e < 2; ++e)//EVOS_PER_MON; i++)
    {
        found = FALSE;
        for (s = 1; s < NUM_SPECIES; s++)
        {
            if (s < species)
                // Work downwards, as the evolution is most likely just before this
                spe = species - s;
            else
                // Start counting upwards now, as we've exhausted all of the before species
                spe = s;

            evoCount = Rogue_GetMaxEvolutionCount(spe);

            for (evo = 0; evo < evoCount; evo++)
            {
                Rogue_ModifyEvolution(spe, evo, &evolution);

                if (evolution.targetSpecies == species)
                {
                    species = spe;
                    found = TRUE;
                    break;
                }
            }

            if (found)
                break;
        }

        if (s == NUM_SPECIES)
            break;
    }

    return species;
#endif
}

u8 Rogue_GetMaxEvolutionCount(u16 species)
{
#ifdef ROGUE_BAKE_VALID
    return gRogueBake_SpeciesData[species].evolutionCount;
#else
    return GetMaxEvolutionCountInternal(species);
#endif
}

u8 Rogue_GetActiveEvolutionCount(u16 species)
{
    u16 s;
    u8 e;
    struct Evolution evo;
    u8 evoCount = Rogue_GetMaxEvolutionCount(species);

    for (e = 0; e < evoCount; e++)
    {
        Rogue_ModifyEvolution(species, e, &evo);

        s = evo.targetSpecies;

        if (s != SPECIES_NONE)
        {
            return 1 + Rogue_GetActiveEvolutionCount(s);
        }
    }

    return 0;
}

u8 Rogue_GetActiveFormChangeCount(u16 species)
{
#ifdef ROGUE_EXPANSION
    u32 i;
    struct FormChange formChange;
    u8 count = 0;

    for (i = 0; TRUE; i++)
    {
        Rogue_ModifyFormChange(species, i, &formChange);

        if(formChange.method == FORM_CHANGE_TERMINATOR)
            break;

        if(formChange.method != FORM_CHANGE_DISABLED_STUB)
            ++count;
    }

    return count;
#else
    return 0;
#endif
}

bool8 Rogue_DoesEvolveInto(u16 fromSpecies, u16 toSpecies)
{
    u8 i;
    struct Evolution currentEvo;
    u8 evoCount = Rogue_GetMaxEvolutionCount(fromSpecies);

    for (i = 0; i < evoCount; i++)
    {
        Rogue_ModifyEvolution(fromSpecies, i, &currentEvo);

        if(currentEvo.method != 0)
        {
            if(currentEvo.targetSpecies == toSpecies)
                return TRUE;
            else if(currentEvo.targetSpecies != SPECIES_NONE)
            {
                if(Rogue_DoesEvolveInto(currentEvo.targetSpecies, toSpecies))
                    return TRUE;
            }
        }
    }

    return FALSE;
}

void Rogue_AppendSpeciesTypeFlags(u16 species, u32* outFlags)
{
#ifdef ROGUE_EXPANSION
    *outFlags |= MON_TYPE_VAL_TO_FLAGS(gSpeciesInfo[species].types[0]);
    *outFlags |= MON_TYPE_VAL_TO_FLAGS(gSpeciesInfo[species].types[1]);
#else
    *outFlags |= MON_TYPE_VAL_TO_FLAGS(gRogueSpeciesInfo[species].type1);
    *outFlags |= MON_TYPE_VAL_TO_FLAGS(gRogueSpeciesInfo[species].type2);
#endif
}

u32 Rogue_GetSpeciesEvolutionChainTypeFlags(u16 species)
{
#ifdef ROGUE_BAKE_VALID
    return gRogueBake_SpeciesData[species].evolutionChainTypeFlags;
#elif defined(ROGUE_BAKING)
    return 0;
#else
    AGB_ASSERT(FALSE);
    return FALSE;
#endif
}

u32 Rogue_GetTypeFlagsFromArray(const u8* types, u8 count)
{
    u8 i;
    u32 flags = 0;

    for(i = 0; i < count; ++i)
    {
        if(types[i] == TYPE_NONE)
            break;

        flags |= MON_TYPE_VAL_TO_FLAGS(types[i]);
    }

    return flags;
}

u32 Rogue_GetMonFlags(u16 species)
{
    u32 flags = 0;
#ifndef ROGUE_BAKING
#ifdef ROGUE_EXPANSION
    //u16 species2;;
#endif

    flags = gRoguePokemonProfiles[species].monFlags;

#ifdef ROGUE_EXPANSION
    //species2 = GET_BASE_SPECIES_ID(species);
    //if(species2 != species)
    //    flags |= gPresetMonTable[species2].flags;
#endif
#endif

    return flags;
}

bool8 Rogue_CheckMonFlags(u16 species, u32 flags) 
{
    return (Rogue_GetMonFlags(species) & flags) == flags;
}

u16 Rogue_BerryToPokeblock(u16 berryItem)
{
    switch (berryItem)
    {
    case ITEM_APICOT_BERRY:
    case ITEM_GREPA_BERRY:
        return ITEM_POKEBLOCK_SPDEF;

    case ITEM_GANLON_BERRY:
    case ITEM_QUALOT_BERRY:
        return ITEM_POKEBLOCK_DEF;

    case ITEM_HONDEW_BERRY:
    case ITEM_PETAYA_BERRY:
        return ITEM_POKEBLOCK_SPATK;

    case ITEM_KELPSY_BERRY:
    case ITEM_LIECHI_BERRY:
        return ITEM_POKEBLOCK_ATK;

    case ITEM_ORAN_BERRY:
    case ITEM_SITRUS_BERRY:
    case ITEM_POMEG_BERRY:
        return ITEM_POKEBLOCK_HP;

    case ITEM_SALAC_BERRY:
    case ITEM_TAMATO_BERRY:
        return ITEM_POKEBLOCK_SPEED;


    case ITEM_STARF_BERRY:
        return ITEM_POKEBLOCK_SHINY;


    case ITEM_LUM_BERRY:
        return ITEM_POKEBLOCK_NORMAL;

    case ITEM_RABUTA_BERRY:
        return ITEM_POKEBLOCK_FIGHTING;

    case ITEM_LEPPA_BERRY:
        return ITEM_POKEBLOCK_FLYING;

    case ITEM_PECHA_BERRY:
        return ITEM_POKEBLOCK_POISON;

    case ITEM_LANSAT_BERRY:
        return ITEM_POKEBLOCK_GROUND;

    case ITEM_SPELON_BERRY:
        return ITEM_POKEBLOCK_ROCK;

    case ITEM_WATMEL_BERRY:
        return ITEM_POKEBLOCK_BUG;

    case ITEM_NOMEL_BERRY:
        return ITEM_POKEBLOCK_GHOST;

    case ITEM_MAGOST_BERRY:
        return ITEM_POKEBLOCK_STEEL;

    case ITEM_RAWST_BERRY:
        return ITEM_POKEBLOCK_FIRE;

    case ITEM_BELUE_BERRY:
        return ITEM_POKEBLOCK_WATER;

    case ITEM_DURIN_BERRY:
        return ITEM_POKEBLOCK_GRASS;

    case ITEM_CHERI_BERRY:
        return ITEM_POKEBLOCK_ELECTRIC;

    case ITEM_CHESTO_BERRY:
        return ITEM_POKEBLOCK_PSYCHIC;

    case ITEM_ASPEAR_BERRY:
        return ITEM_POKEBLOCK_ICE;

    case ITEM_PAMTRE_BERRY:
        return ITEM_POKEBLOCK_DRAGON;

    case ITEM_CORNN_BERRY:
        return ITEM_POKEBLOCK_DARK;


#ifdef ROGUE_EXPANSION
    case ITEM_KEE_BERRY:
        return ITEM_POKEBLOCK_FAIRY;

    case ITEM_MICLE_BERRY:
        return ITEM_POKEBLOCK_HP;

    case ITEM_CUSTAP_BERRY:
        return ITEM_POKEBLOCK_SPEED;

    case ITEM_JABOCA_BERRY:
        return ITEM_POKEBLOCK_ATK;

    case ITEM_ROWAP_BERRY:
        return ITEM_POKEBLOCK_SPATK;

    case ITEM_MARANGA_BERRY:
        return ITEM_POKEBLOCK_SPDEF;


    case ITEM_CHILAN_BERRY:
        return ITEM_POKEBLOCK_NORMAL;

    case ITEM_CHOPLE_BERRY:
        return ITEM_POKEBLOCK_FIGHTING;

    case ITEM_COBA_BERRY:
        return ITEM_POKEBLOCK_FLYING;

    case ITEM_KEBIA_BERRY:
        return ITEM_POKEBLOCK_POISON;

    case ITEM_SHUCA_BERRY:
        return ITEM_POKEBLOCK_GROUND;

    case ITEM_CHARTI_BERRY:
        return ITEM_POKEBLOCK_ROCK;

    case ITEM_TANGA_BERRY:
        return ITEM_POKEBLOCK_BUG;

    case ITEM_KASIB_BERRY:
        return ITEM_POKEBLOCK_GHOST;

    case ITEM_BABIRI_BERRY:
        return ITEM_POKEBLOCK_STEEL;

    case ITEM_OCCA_BERRY:
        return ITEM_POKEBLOCK_FIRE;

    case ITEM_PASSHO_BERRY:
        return ITEM_POKEBLOCK_WATER;

    case ITEM_RINDO_BERRY:
        return ITEM_POKEBLOCK_GRASS;

    case ITEM_WACAN_BERRY:
        return ITEM_POKEBLOCK_ELECTRIC;

    case ITEM_PAYAPA_BERRY:
        return ITEM_POKEBLOCK_PSYCHIC;

    case ITEM_YACHE_BERRY:
        return ITEM_POKEBLOCK_ICE;

    case ITEM_HABAN_BERRY:
        return ITEM_POKEBLOCK_DRAGON;

    case ITEM_COLBUR_BERRY:
        return ITEM_POKEBLOCK_DARK;

    case ITEM_ROSELI_BERRY:
        return ITEM_POKEBLOCK_FAIRY;
#endif

    default:
        return ITEM_NONE;
    }
}