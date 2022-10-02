//
// This file is shared between the game src and the offline bake to assist in making 
// queries and other stuff which can be prepared offline a bit faster
//
#include "constants/battle_ai.h"
#include "constants/items.h"
#include "constants/pokemon.h"
#include "constants/species.h"
#include "constants/trainers.h"

#ifdef ROGUE_BAKING
// Manually reinclude this if regenerating
#include "BakeHelpers.h"
#else
#include "global.h"
#include "data.h"
#include "graphics.h"
#include "item.h"
#include "item_use.h"
#include "string_util.h"

#include "rogue_controller.h"
#endif

#include "rogue_baked.h"


#ifdef ROGUE_BAKING
#define ROGUE_BAKE_INVALID
#else
// Swap to force runtime resolution
//#define ROGUE_BAKE_INVALID
#define ROGUE_BAKE_VALID
#endif

extern const u8 gText_TrainerNameChallenger[];
extern const u8 gText_TrainerNameGrunt[];

extern struct Evolution gEvolutionTable[][EVOS_PER_MON];

#ifdef ROGUE_BAKE_VALID
extern const u16 gRogueBake_EggSpecies[NUM_SPECIES];
extern const u8 gRogueBake_EvolutionCount[NUM_SPECIES];
#endif

void Rogue_ModifyEvolution(u16 species, u8 evoIdx, struct Evolution* outEvo)
{
    memcpy(outEvo, &gEvolutionTable[species][evoIdx], sizeof(struct Evolution));

    // Any species alterations
#ifdef ROGUE_EXPANSION
    if(species == SPECIES_KOFFING && evoIdx == 1)
    {
        outEvo->targetSpecies = SPECIES_WEEZING_GALARIAN;
    }
#endif

    if(outEvo->targetSpecies != SPECIES_NONE && !IsGenEnabled(SpeciesToGen(outEvo->targetSpecies)))
    {
        // Invalid evo
        outEvo->targetSpecies = SPECIES_NONE;
        outEvo->method = 0;
        return;
    }

#ifdef ROGUE_EXPANSION
    if(!IsMegaEvolutionEnabled())
    {
        switch(outEvo->method)
        {
            case(EVO_MEGA_EVOLUTION):
            case(EVO_MOVE_MEGA_EVOLUTION):
            case(EVO_PRIMAL_REVERSION):
                outEvo->targetSpecies = SPECIES_NONE;
                outEvo->method = 0;
                break;
        }
    }
#endif

    if(outEvo->targetSpecies != SPECIES_NONE)
    {
#ifdef ROGUE_EXPANSION
        if(species == SPECIES_MILCERY)
        {
            switch(outEvo->targetSpecies)
            {
                case SPECIES_ALCREMIE:
                    outEvo->method = EVO_LEVEL_ITEM;
                    outEvo->param = ITEM_PECHA_BERRY;
                    break;

                case SPECIES_ALCREMIE_RUBY_CREAM:
                    outEvo->method = EVO_LEVEL_ITEM;
                    outEvo->param = ITEM_PERSIM_BERRY;
                    break;

                case SPECIES_ALCREMIE_MATCHA_CREAM:
                    outEvo->method = EVO_LEVEL_ITEM;
                    outEvo->param = ITEM_RAWST_BERRY;
                    break;

                case SPECIES_ALCREMIE_MINT_CREAM:
                    outEvo->method = EVO_LEVEL_ITEM;
                    outEvo->param = ITEM_CHERI_BERRY;
                    break;

                case SPECIES_ALCREMIE_LEMON_CREAM:
                    outEvo->method = EVO_LEVEL_ITEM;
                    outEvo->param = ITEM_ASPEAR_BERRY;
                    break;

                case SPECIES_ALCREMIE_SALTED_CREAM:
                    outEvo->method = EVO_LEVEL_ITEM;
                    outEvo->param = ITEM_CHESTO_BERRY;
                    break;

                case SPECIES_ALCREMIE_RUBY_SWIRL:
                    outEvo->method = EVO_LEVEL_ITEM;
                    outEvo->param = ITEM_LEPPA_BERRY;
                    break;

                case SPECIES_ALCREMIE_CARAMEL_SWIRL:
                    outEvo->method = EVO_LEVEL_ITEM;
                    outEvo->param = ITEM_SITRUS_BERRY;
                    break;

                case SPECIES_ALCREMIE_RAINBOW_SWIRL:
                    outEvo->method = EVO_LEVEL_ITEM;
                    outEvo->param = ITEM_SALAC_BERRY;
                    break;

                default:
                    outEvo->targetSpecies = SPECIES_NONE;
                    outEvo->method = 0;
                    break;
            }
        }

        if(species == SPECIES_YAMASK_GALARIAN && evoIdx == 0)
        {
            outEvo->method = EVO_LEVEL;
            outEvo->param = 34;
        }

        if(species == SPECIES_MELTAN && evoIdx == 0)
        {
            outEvo->method = EVO_LEVEL_ITEM;
            outEvo->param = ITEM_METAL_COAT;
        }

        if(species == SPECIES_EXEGGCUTE && evoIdx == 1)
        {
            outEvo->method = EVO_LEVEL_ITEM;
            outEvo->param = ITEM_DRAGON_SCALE;
        }

        if(species == SPECIES_MIME_JR && evoIdx == 1)
        {
            outEvo->method = EVO_ITEM;
            outEvo->param = ITEM_ICE_STONE;
        }

        if(species == SPECIES_KOFFING && evoIdx == 1)
        {
            outEvo->method = EVO_ITEM;
            outEvo->param = ITEM_GALARICA_CUFF;
        }

        if(species == SPECIES_PIKACHU && evoIdx == 1)
        {
            outEvo->method = EVO_ITEM;
            outEvo->param = ITEM_SHINY_STONE;
        }
#endif

        switch(outEvo->method)
        {
            case(EVO_BEAUTY):
            case(EVO_FRIENDSHIP):
                outEvo->method = EVO_LEVEL;
                outEvo->param = 20;
                break;

            case(EVO_TRADE):
                outEvo->method = EVO_LEVEL_ITEM;
                outEvo->param = ITEM_LINK_CABLE;
                break;
            case(EVO_TRADE_ITEM):
                outEvo->method = EVO_LEVEL_ITEM;
                break;

            case(EVO_FRIENDSHIP_DAY):
                outEvo->method = EVO_ITEM;
                outEvo->param = ITEM_SUN_STONE;
                break;
            case(EVO_FRIENDSHIP_NIGHT):
                outEvo->method = EVO_ITEM;
                outEvo->param = ITEM_MOON_STONE;
                break;

#ifdef ROGUE_EXPANSION
            case(EVO_ITEM_HOLD_DAY):
            case(EVO_ITEM_HOLD_NIGHT):
                outEvo->method = EVO_LEVEL_ITEM;
                break;

            case(EVO_LEVEL_DAY):
                outEvo->method = EVO_ITEM;
                outEvo->param = ITEM_SUN_STONE;
                break;
            case(EVO_LEVEL_DUSK):
            case(EVO_LEVEL_NIGHT):
                outEvo->method = EVO_ITEM;
                outEvo->param = ITEM_MOON_STONE;
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
                outEvo->method = EVO_SPECIFIC_MON_IN_PARTY;
                break;

            case(EVO_SPECIFIC_MAP):
                if(species == SPECIES_EEVEE)
                {
                    outEvo->targetSpecies = 0;
                    outEvo->method = 0;
                    outEvo->param = 0;
                }
                else
                {
                    // Crabrawler
                    outEvo->method = EVO_ITEM;
                    outEvo->param = ITEM_WATER_STONE;
                }
                break;

            case(EVO_MAPSEC):
                // All these were MAPSEC_NEW_MAUVILLE
                //outEvo->method = EVO_SPECIFIC_MAP;
                //outEvo->param = MAP_ROGUE_ROUTE_URBAN0;
                outEvo->method = EVO_ITEM;
                outEvo->param = ITEM_THUNDER_STONE;
                break;
#endif
        }
    }
}
const u8* Rogue_GetTrainerName(u16 trainerNum)
{
    // TODO - Replace name with _("CHALLENGER")

    if(trainerNum >= TRAINER_ROGUE_BREEDER_F && trainerNum <= TRAINER_ROGUE_POSH_M)
    {
        return gText_TrainerNameChallenger;
    }

    if(trainerNum >= TRAINER_ROGUE_MAGMA_F && trainerNum <= TRAINER_ROGUE_AQUA_M)
    {
        return gText_TrainerNameGrunt;
    }

    switch(trainerNum)
    {
        case TRAINER_ROGUE_MINI_BOSS_MIRROR:
            return gSaveBlock2Ptr->playerName;
    }

    return gTrainers[trainerNum].trainerName;
}

void Rogue_ModifyTrainer(u16 trainerNum, struct Trainer* outTrainer)
{
    memcpy(outTrainer, &gTrainers[trainerNum], sizeof(struct Trainer));

    // We can do this, but ideally we should fixup to use the method above
    //StringCopy(outTrainer->trainerName, Rogue_GetTrainerName(trainerNum));

    outTrainer->partyFlags = 0;
    outTrainer->doubleBattle = FALSE;
#ifdef ROGUE_EXPANSION
    outTrainer->aiFlags = AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SETUP_FIRST_TURN | AI_FLAG_WILL_SUICIDE | AI_FLAG_HELP_PARTNER | AI_FLAG_SMART_SWITCHING;
#else
    outTrainer->aiFlags = AI_SCRIPT_CHECK_BAD_MOVE | AI_SCRIPT_TRY_TO_FAINT | AI_SCRIPT_CHECK_VIABILITY | AI_SCRIPT_SETUP_FIRST_TURN;
#endif

    // AI_SCRIPT_DOUBLE_BATTLE ?

    // Setup bosses/Mini bosses
    switch(trainerNum)
    {
        case TRAINER_ROGUE_BOSS_ROXANNE:
        case TRAINER_ROGUE_BOSS_FLANNERY:
        case TRAINER_ROGUE_BOSS_WINONA:
        case TRAINER_ROGUE_BOSS_TATE_AND_LIZA:
        case TRAINER_ROGUE_BOSS_PHOEBE:
        case TRAINER_ROGUE_BOSS_GLACIA:
        case TRAINER_ROGUE_BOSS_LUCY:
        case TRAINER_ROGUE_BOSS_ANABEL:
            if(gRogueRun.currentDifficulty >= 12)
                outTrainer->trainerClass = TRAINER_CLASS_CHAMPION;
            else if(gRogueRun.currentDifficulty >= 8)
                outTrainer->trainerClass = TRAINER_CLASS_ELITE_FOUR;
            else
                outTrainer->trainerClass = TRAINER_CLASS_LEADER;

            outTrainer->encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE;
            break;

        case TRAINER_ROGUE_BOSS_BRAWLY:
        case TRAINER_ROGUE_BOSS_NORMAN:
        case TRAINER_ROGUE_BOSS_JUAN:
        case TRAINER_ROGUE_BOSS_SIDNEY:
        case TRAINER_ROGUE_BOSS_DRAKE:
        case TRAINER_ROGUE_BOSS_WALLACE:
        case TRAINER_ROGUE_BOSS_STEVEN:
        case TRAINER_ROGUE_BOSS_BRANDON:
        case TRAINER_ROGUE_BOSS_TUCKER:
        case TRAINER_ROGUE_BOSS_SPENSER:
            if(gRogueRun.currentDifficulty >= 12)
                outTrainer->trainerClass = TRAINER_CLASS_CHAMPION;
            else if(gRogueRun.currentDifficulty >= 8)
                outTrainer->trainerClass = TRAINER_CLASS_ELITE_FOUR;
            else
                outTrainer->trainerClass = TRAINER_CLASS_LEADER;

            outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE;
            break;
    }

    switch(trainerNum)
    {
        // Bosses
        //
        case TRAINER_ROGUE_BOSS_ROXANNE:
            outTrainer->trainerPic = TRAINER_PIC_LEADER_ROXANNE;
            break;

        case TRAINER_ROGUE_BOSS_BRAWLY:
            outTrainer->trainerPic = TRAINER_PIC_LEADER_BRAWLY;
            break;

        case TRAINER_ROGUE_BOSS_WATTSON:
            outTrainer->trainerPic = TRAINER_PIC_LEADER_WATTSON;
            break;

        case TRAINER_ROGUE_BOSS_FLANNERY:
            outTrainer->trainerPic = TRAINER_PIC_LEADER_FLANNERY;
            break;

        case TRAINER_ROGUE_BOSS_NORMAN:
            outTrainer->trainerPic = TRAINER_PIC_LEADER_NORMAN;
            break;

        case TRAINER_ROGUE_BOSS_WINONA:
            outTrainer->trainerPic = TRAINER_PIC_LEADER_WINONA;
            break;

        case TRAINER_ROGUE_BOSS_TATE_AND_LIZA:
            outTrainer->trainerPic = TRAINER_PIC_LEADER_TATE_AND_LIZA;
            break;

        case TRAINER_ROGUE_BOSS_JUAN:
            outTrainer->trainerPic = TRAINER_PIC_LEADER_JUAN;
            break;

        case TRAINER_ROGUE_BOSS_SIDNEY:
            outTrainer->trainerPic = TRAINER_PIC_ELITE_FOUR_SIDNEY;
            break;

        case TRAINER_ROGUE_BOSS_PHOEBE:
            outTrainer->trainerPic = TRAINER_PIC_ELITE_FOUR_PHOEBE;
            break;

        case TRAINER_ROGUE_BOSS_GLACIA:
            outTrainer->trainerPic = TRAINER_PIC_ELITE_FOUR_GLACIA;
            break;

        case TRAINER_ROGUE_BOSS_DRAKE:
            outTrainer->trainerPic = TRAINER_PIC_ELITE_FOUR_DRAKE;
            break;

        case TRAINER_ROGUE_BOSS_WALLACE:
            outTrainer->trainerPic = TRAINER_PIC_CHAMPION_WALLACE;
            break;

        case TRAINER_ROGUE_BOSS_STEVEN:
            outTrainer->trainerPic = TRAINER_PIC_STEVEN;
            break;

        // Mini Bosses
        //
        case TRAINER_ROGUE_MINI_BOSS_MAXIE:
            outTrainer->trainerClass = TRAINER_CLASS_MAGMA_LEADER;
            outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MAGMA;
            outTrainer->trainerPic = TRAINER_PIC_MAGMA_LEADER_MAXIE;
            break;

        case TRAINER_ROGUE_MINI_BOSS_ARCHIE:
            outTrainer->trainerClass = TRAINER_CLASS_AQUA_LEADER;
            outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_AQUA;
            outTrainer->trainerPic = TRAINER_PIC_AQUA_LEADER_ARCHIE;
            break;

        case TRAINER_ROGUE_MINI_BOSS_WALLY:
            outTrainer->trainerClass = TRAINER_CLASS_RIVAL;
            outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE;
            outTrainer->trainerPic = TRAINER_PIC_WALLY;
            break;

        case TRAINER_ROGUE_MINI_BOSS_MIRROR:
            outTrainer->trainerClass = TRAINER_CLASS_RIVAL;

            switch(gSaveBlock2Ptr->playerGender)
            {    
                case(STYLE_EMR_BRENDAN):
                    outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE;
                    outTrainer->trainerPic = TRAINER_PIC_BRENDAN;
                    break;
                case(STYLE_EMR_MAY):
                    outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_FEMALE;
                    outTrainer->trainerPic = TRAINER_PIC_MAY;
                    break;

                case(STYLE_RED):
                    outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE;
                    outTrainer->trainerPic = TRAINER_PIC_RED;
                    break;
                case(STYLE_LEAF):
                    outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_FEMALE;
                    outTrainer->trainerPic = TRAINER_PIC_LEAF;
                    break;
            };
            break;

        // Std Trainers
        //
        case TRAINER_ROGUE_BREEDER_F:
            outTrainer->trainerClass = TRAINER_CLASS_PKMN_BREEDER;
            outTrainer->encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE;
            outTrainer->trainerPic = TRAINER_PIC_POKEMON_BREEDER_F;
            break;
        case TRAINER_ROGUE_BREEDER_M:
            outTrainer->trainerClass = TRAINER_CLASS_PKMN_BREEDER;
            outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE;
            outTrainer->trainerPic = TRAINER_PIC_POKEMON_BREEDER_M;
            break;

        case TRAINER_ROGUE_RICH_F:
            outTrainer->trainerClass = TRAINER_CLASS_LADY;
            outTrainer->encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE;
            outTrainer->trainerPic = TRAINER_PIC_LADY;
            break;
        case TRAINER_ROGUE_RICH_M:
            outTrainer->trainerClass = TRAINER_CLASS_RICH_BOY;
            outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE;
            outTrainer->trainerPic = TRAINER_PIC_RICH_BOY;
            break;

        case TRAINER_ROGUE_COOLTRAINER_F:
            outTrainer->trainerClass = TRAINER_CLASS_COOLTRAINER;
            outTrainer->encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE;
            outTrainer->trainerPic = TRAINER_PIC_COOLTRAINER_F;
            break;
        case TRAINER_ROGUE_COOLTRAINER_M:
            outTrainer->trainerClass = TRAINER_CLASS_COOLTRAINER;
            outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE;
            outTrainer->trainerPic = TRAINER_PIC_COOLTRAINER_M;
            break;

        case TRAINER_ROGUE_POKEFAN_F:
            outTrainer->trainerClass = TRAINER_CLASS_POKEFAN;
            outTrainer->encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE;
            outTrainer->trainerPic = TRAINER_PIC_POKEFAN_F;
            break;
        case TRAINER_ROGUE_POKEFAN_M:
            outTrainer->trainerClass = TRAINER_CLASS_POKEFAN;
            outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE;
            outTrainer->trainerPic = TRAINER_PIC_POKEFAN_M;
            break;

        case TRAINER_ROGUE_SCHOOL_KID_F:
            outTrainer->trainerClass = TRAINER_CLASS_SCHOOL_KID;
            outTrainer->encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE;
            outTrainer->trainerPic = TRAINER_PIC_SCHOOL_KID_F;
            break;
        case TRAINER_ROGUE_SCHOOL_KID_M:
            outTrainer->trainerClass = TRAINER_CLASS_SCHOOL_KID;
            outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE;
            outTrainer->trainerPic = TRAINER_PIC_SCHOOL_KID_M;
            break;
            
        case TRAINER_ROGUE_TUBER_F:
            outTrainer->trainerClass = TRAINER_CLASS_TUBER_F;
            outTrainer->encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE;
            outTrainer->trainerPic = TRAINER_PIC_TUBER_F;
            break;
        case TRAINER_ROGUE_TUBER_M:
            outTrainer->trainerClass = TRAINER_CLASS_TUBER_M;
            outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE;
            outTrainer->trainerPic = TRAINER_PIC_TUBER_M;
            break;

        case TRAINER_ROGUE_POSH_F:
            outTrainer->trainerClass = TRAINER_CLASS_AROMA_LADY;
            outTrainer->encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE;
            outTrainer->trainerPic = TRAINER_PIC_AROMA_LADY;
            break;
        case TRAINER_ROGUE_POSH_M:
            outTrainer->trainerClass = TRAINER_CLASS_GENTLEMAN;
            outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE;
            outTrainer->trainerPic = TRAINER_PIC_GENTLEMAN;
            break;
            
        case TRAINER_ROGUE_MAGMA_F:
            outTrainer->trainerClass = TRAINER_CLASS_TEAM_MAGMA;
            outTrainer->encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE;
            outTrainer->trainerPic = TRAINER_PIC_MAGMA_GRUNT_F;
            break;
        case TRAINER_ROGUE_MAGMA_M:
            outTrainer->trainerClass = TRAINER_CLASS_TEAM_MAGMA;
            outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE;
            outTrainer->trainerPic = TRAINER_PIC_MAGMA_GRUNT_M;
            break;

        case TRAINER_ROGUE_AQUA_F:
            outTrainer->trainerClass = TRAINER_CLASS_TEAM_AQUA;
            outTrainer->encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE;
            outTrainer->trainerPic = TRAINER_PIC_AQUA_GRUNT_F;
            break;
        case TRAINER_ROGUE_AQUA_M:
            outTrainer->trainerClass = TRAINER_CLASS_TEAM_AQUA;
            outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE;
            outTrainer->trainerPic = TRAINER_PIC_AQUA_GRUNT_M;
            break;
    }

    // TODO
}

static u16 SanitizeItemId(u16 itemId)
{
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

void Rogue_ModifyItem(u16 itemId, struct Item* outItem)
{
}
#else

extern const u8 gText_ItemLinkCable[];
extern const u8 gText_ItemQuestLog[];
extern const u8 gText_ItemQuestLogDesc[];

extern const u32 *const gItemIconTable[][2];

const u8* Rogue_GetItemName(u16 itemId)
{
    itemId = SanitizeItemId(itemId);

    switch(itemId)
    {
        case ITEM_LINK_CABLE:
            return gText_ItemLinkCable;
        
        case ITEM_QUEST_LOG:
            return gText_ItemQuestLog;
    }

    return gItems[itemId].name;
}

const void* Rogue_GetItemIconPicOrPalette(u16 itemId, u8 which)
{
    switch(itemId)
    {
        case ITEM_QUEST_LOG:
            return which == 0 ? gItemIcon_FameChecker : gItemIconPalette_FameChecker;
    }

    return gItemIconTable[itemId][which];
}

void Rogue_ModifyItem(u16 itemId, struct Item* outItem)
{
    itemId = SanitizeItemId(itemId);
    memcpy(outItem, &gItems[itemId], sizeof(struct Item));

    // Behaviour edits
    //
    switch(itemId)
    {
        case ITEM_QUEST_LOG: // Quest Log
            outItem->fieldUseFunc = ItemUseOutOfBattle_QuestLog;
            outItem->description = gText_ItemQuestLogDesc;
            break;
    }

    // Price Edits
    //
    // Range edits
    if(itemId >= ITEM_HP_UP && itemId <= ITEM_PP_MAX)
    {
        outItem->price = 4000;
    }

#ifdef ROGUE_EXPANSION
    if(itemId >= ITEM_X_ATTACK && itemId <= ITEM_GUARD_SPEC)
#else
    if(itemId >= ITEM_GUARD_SPEC && itemId <= ITEM_X_SPECIAL)
#endif
    {
        outItem->price = 1500;
    }

    if(outItem->fieldUseFunc == ItemUseOutOfBattle_EvolutionStone)
    {
        outItem->price = 2100;
    }

    // Hold items set price (Ignore berries)
    if(outItem->holdEffect != 0 && !(itemId >= FIRST_BERRY_INDEX && itemId <= LAST_BERRY_INDEX))
    {
        outItem->price = 500;
    }

#ifdef ROGUE_EXPANSION
    if(itemId >= ITEM_LEVEL_BALL && itemId <= ITEM_CHERISH_BALL)
    {
        outItem->price = 2500;
    }

    if(itemId >= ITEM_FIRE_STONE && itemId <= ITEM_RIBBON_SWEET)
    {
        outItem->price = 2100;
    }

    if(itemId >= ITEM_RED_NECTAR && itemId <= ITEM_PURPLE_NECTAR)
    {
        outItem->price = 2100;
    }

    if(itemId >= ITEM_RED_ORB && itemId <= ITEM_DIANCITE)
    {
        outItem->price = 5000;
    }

    if(itemId >= ITEM_NORMALIUM_Z && itemId <= ITEM_ULTRANECROZIUM_Z)
    {
        outItem->price = 5000;
    }

    if(itemId >= ITEM_DOUSE_DRIVE && itemId <= ITEM_CHILL_DRIVE)
    {
        outItem->price = 1000;
    }

    if(itemId >= ITEM_ROTOM_CATALOG && itemId <= ITEM_REINS_OF_UNITY)
    {
        outItem->price = 5000;
    }

    if(itemId >= ITEM_LONELY_MINT && itemId <= ITEM_SERIOUS_MINT)
    {
        outItem->price = 1500;
    }
#endif

    // Individual items
    switch(itemId)
    {
        // Evo item prices
        case ITEM_LINK_CABLE:
            outItem->price = 2100;
            outItem->holdEffect = 0;//HOLD_EFFECT_NONE;
            break;

        case ITEM_NUGGET:
            outItem->price = 1000;
            outItem->holdEffect = 0;
            break;

        case ITEM_PEARL:
            outItem->price = 1500;
            outItem->holdEffect = 0;
            break;

        case ITEM_BIG_PEARL:
            outItem->price = 2000;
            outItem->holdEffect = 0;
            break;

        case ITEM_STARDUST:
            outItem->price = 3000;
            outItem->holdEffect = 0;
            break;

        case ITEM_STAR_PIECE:
            outItem->price = 4000;
            outItem->holdEffect = 0;
            break;
    
#ifdef ROGUE_EXPANSION
        case ITEM_ABILITY_PATCH:
            outItem->price = 10000;
            break;

        case ITEM_RUSTED_SWORD:
        case ITEM_RUSTED_SHIELD:
            outItem->price = 5000;
            break;
#endif

        case ITEM_KINGS_ROCK:
        case ITEM_DEEP_SEA_TOOTH:
        case ITEM_DEEP_SEA_SCALE:
        case ITEM_METAL_COAT:
        case ITEM_DRAGON_SCALE:
#ifdef ROGUE_EXPANSION
        case ITEM_UPGRADE:
#else
        case ITEM_UP_GRADE:
#endif
            outItem->price = 2100;
            break;

        case ITEM_RARE_CANDY:
            outItem->price = 1000;
            break;

#ifdef ROGUE_EXPANSION
        case ITEM_SPORT_BALL:
        case ITEM_PARK_BALL:
#endif
        case ITEM_SAFARI_BALL:
            outItem->price = 0;
            break;

        case ITEM_MASTER_BALL:
            outItem->price = 50000;
            break;
    }
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
    return gRogueBake_EggSpecies[species];

#else
    u16 e, s, evo, spe;
    bool8 found;
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

            for (evo = 0; evo < EVOS_PER_MON; evo++)
            {
                Rogue_ModifyEvolution(spe, evo, &evolution);

#ifdef ROGUE_EXPANSION
                if(evolution.method != EVO_MEGA_EVOLUTION &&
                    evolution.method != EVO_MOVE_MEGA_EVOLUTION &&
                    evolution.method != EVO_PRIMAL_REVERSION
                )
#endif
                {
                    if (evolution.targetSpecies == species)
                    {
                        species = spe;
                        found = TRUE;
                        break;
                    }
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

u8 Rogue_GetEvolutionCount(u16 species)
{
#ifdef ROGUE_BAKE_VALID
    return gRogueBake_EvolutionCount[species];
    
#else
    u16 s, e;
    struct Evolution evo;

    for (e = 0; e < EVOS_PER_MON; e++)
    {
        Rogue_ModifyEvolution(species, e, &evo);

        s = evo.targetSpecies;

#ifdef ROGUE_EXPANSION
        if(evo.method != EVO_MEGA_EVOLUTION &&
            evo.method != EVO_MOVE_MEGA_EVOLUTION &&
            evo.method != EVO_PRIMAL_REVERSION
        )
#endif
        {
            if (s != SPECIES_NONE)
            {
                return 1 + Rogue_GetEvolutionCount(s);
            }
        }
    }

    return 0;
#endif
}