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
#include "rogue_charms.h"
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

extern const u8 gText_TrainerName_TateLiza[];
extern const u8 gText_TrainerName_Anabel[];

extern const u8 gText_TrainerName_Brock[];
extern const u8 gText_TrainerName_Misty[];
extern const u8 gText_TrainerName_LtSurge[];
extern const u8 gText_TrainerName_Erika[];
extern const u8 gText_TrainerName_Koga[];
extern const u8 gText_TrainerName_Sabrina[];
extern const u8 gText_TrainerName_Blaine[];
extern const u8 gText_TrainerName_Giovanni[];

extern const u8 gText_TrainerName_Lorelei[];
extern const u8 gText_TrainerName_Bruno[];
extern const u8 gText_TrainerName_Agatha[];
extern const u8 gText_TrainerName_Lance[];

extern const u8 gText_TrainerName_Blue[];
extern const u8 gText_TrainerName_ProfOak[];

extern const u8 gText_TrainerName_Brendan[];
extern const u8 gText_TrainerName_May[];
extern const u8 gText_TrainerName_Red[];
extern const u8 gText_TrainerName_Leaf[];

extern const u8 gText_TrainerName_Falkner[];
extern const u8 gText_TrainerName_Bugsy[];
extern const u8 gText_TrainerName_Whitney[];
extern const u8 gText_TrainerName_Morty[];
extern const u8 gText_TrainerName_Chuck[];
extern const u8 gText_TrainerName_Jasmine[];
extern const u8 gText_TrainerName_Pryce[];
extern const u8 gText_TrainerName_Clair[];

extern const u8 gText_TrainerName_Will[];
extern const u8 gText_TrainerName_Karen[];

extern const u8 gText_TrainerName_Kate[];
extern const u8 gText_TrainerName_Raven[];
extern const u8 gText_TrainerName_Erma[];
extern const u8 gText_TrainerName_Tails[];
extern const u8 gText_TrainerName_Pokabbie[];


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
            case(EVO_SPECIFIC_MON_IN_PARTY):
                outEvo->method = EVO_LEVEL;
                outEvo->param = 20;
                break;

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
                outEvo->method = EVO_LEVEL_ITEM;
                outEvo->param = ITEM_LINK_CABLE;
                break;

            case(EVO_LEVEL_RAIN):
            case(EVO_LEVEL_DARK_TYPE_MON_IN_PARTY):
                outEvo->method = EVO_LEVEL;
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

        // Apply evo curse
        if(IsCurseActive(EFFECT_EVERSTONE_EVOS))
        {
            if(outEvo->method == EVO_LEVEL)
            {
                outEvo->method = EVO_LEVEL_ITEM;
                outEvo->param = ITEM_LINK_CABLE;
            }
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
        case TRAINER_ROGUE_BOSS_MIRROR:
        case TRAINER_ROGUE_MINI_BOSS_MIRROR:
            return gSaveBlock2Ptr->playerName;

        case TRAINER_ROGUE_MINI_BOSS_RIVAL:
        {
            switch(gSaveBlock2Ptr->playerGender)
            {
                case(STYLE_EMR_BRENDAN):
                    return gText_TrainerName_May;
                case(STYLE_EMR_MAY):
                    return gText_TrainerName_Brendan;

                case(STYLE_RED):
                    return gText_TrainerName_Leaf;
                case(STYLE_LEAF):
                    return gText_TrainerName_Red;
            };
            break;
        }

        case TRAINER_ROGUE_BOSS_TATE_AND_LIZA:
            return gText_TrainerName_TateLiza;

        case TRAINER_ROGUE_BOSS_ANABEL:
            return gText_TrainerName_Anabel;

        case TRAINER_ROGUE_BOSS_BROCK:
            return gText_TrainerName_Brock;
        case TRAINER_ROGUE_BOSS_MISTY:
            return gText_TrainerName_Misty;
        case TRAINER_ROGUE_BOSS_LTSURGE:
            return gText_TrainerName_LtSurge;
        case TRAINER_ROGUE_BOSS_ERIKA:
            return gText_TrainerName_Erika;
        case TRAINER_ROGUE_BOSS_KOGA:
        case TRAINER_ROGUE_BOSS_JOHTO_KOGA:
            return gText_TrainerName_Koga;
        case TRAINER_ROGUE_BOSS_SABRINA:
            return gText_TrainerName_Sabrina;
        case TRAINER_ROGUE_BOSS_BLAINE:
            return gText_TrainerName_Blaine;
        case TRAINER_ROGUE_BOSS_GIOVANNI:
            return gText_TrainerName_Giovanni;

        case TRAINER_ROGUE_BOSS_LORELEI:
            return gText_TrainerName_Lorelei;
        case TRAINER_ROGUE_BOSS_BRUNO:
        case TRAINER_ROGUE_BOSS_JOHTO_BRUNO:
            return gText_TrainerName_Bruno;
        case TRAINER_ROGUE_BOSS_AGATHA:
            return gText_TrainerName_Agatha;
        case TRAINER_ROGUE_BOSS_LANCE:
        case TRAINER_ROGUE_BOSS_JOHTO_LANCE:
            return gText_TrainerName_Lance;

        case TRAINER_ROGUE_BOSS_BLUE:
            return gText_TrainerName_Blue;
        case TRAINER_ROGUE_BOSS_PROFOAK:
            return gText_TrainerName_ProfOak;
            

        case TRAINER_ROGUE_BOSS_FALKNER:
            return gText_TrainerName_Falkner;
        case TRAINER_ROGUE_BOSS_BUGSY:
            return gText_TrainerName_Bugsy;
        case TRAINER_ROGUE_BOSS_WHITNEY:
            return gText_TrainerName_Whitney;
        case TRAINER_ROGUE_BOSS_MORTY:
            return gText_TrainerName_Morty;
        case TRAINER_ROGUE_BOSS_CHUCK:
            return gText_TrainerName_Chuck;
        case TRAINER_ROGUE_BOSS_JASMINE:
            return gText_TrainerName_Jasmine;
        case TRAINER_ROGUE_BOSS_PRYCE:
            return gText_TrainerName_Pryce;
        case TRAINER_ROGUE_BOSS_CLAIR:
            return gText_TrainerName_Clair;

        case TRAINER_ROGUE_BOSS_WILL:
            return gText_TrainerName_Will;
        case TRAINER_ROGUE_BOSS_KAREN:
            return gText_TrainerName_Karen;

        case TRAINER_ROGUE_BOSS_RED:
            return gText_TrainerName_Red;

        case TRAINER_ROGUE_BOSS_KATE:
            return gText_TrainerName_Kate;

        case TRAINER_ROGUE_BOSS_RAVEN:
            return gText_TrainerName_Raven;

        case TRAINER_ROGUE_BOSS_ERMA:
            return gText_TrainerName_Erma;

        case TRAINER_ROGUE_BOSS_TAILS:
            return gText_TrainerName_Tails;

        case TRAINER_ROGUE_BOSS_POKABBIE:
            return gText_TrainerName_Pokabbie;
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
        //case TRAINER_ROGUE_BOSS_LUCY:
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
        case TRAINER_ROGUE_BOSS_WATTSON:
        case TRAINER_ROGUE_BOSS_NORMAN:
        case TRAINER_ROGUE_BOSS_JUAN:
        case TRAINER_ROGUE_BOSS_SIDNEY:
        case TRAINER_ROGUE_BOSS_DRAKE:
        case TRAINER_ROGUE_BOSS_WALLACE:
        case TRAINER_ROGUE_BOSS_STEVEN:
        //case TRAINER_ROGUE_BOSS_BRANDON:
        //case TRAINER_ROGUE_BOSS_TUCKER:
        //case TRAINER_ROGUE_BOSS_SPENSER:
            if(gRogueRun.currentDifficulty >= 12)
                outTrainer->trainerClass = TRAINER_CLASS_CHAMPION;
            else if(gRogueRun.currentDifficulty >= 8)
                outTrainer->trainerClass = TRAINER_CLASS_ELITE_FOUR;
            else
                outTrainer->trainerClass = TRAINER_CLASS_LEADER;

            outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE;
            break;

        // Kanto Ver.
        case TRAINER_ROGUE_BOSS_MISTY:
        case TRAINER_ROGUE_BOSS_ERIKA:
        case TRAINER_ROGUE_BOSS_SABRINA:
        case TRAINER_ROGUE_BOSS_LORELEI:
        case TRAINER_ROGUE_BOSS_AGATHA:
        case TRAINER_ROGUE_BOSS_MIRROR:
            if(gRogueRun.currentDifficulty >= 12)
                outTrainer->trainerClass = TRAINER_CLASS_CHAMPION;
            else if(gRogueRun.currentDifficulty >= 8)
                outTrainer->trainerClass = TRAINER_CLASS_ELITE_FOUR;
            else
                outTrainer->trainerClass = TRAINER_CLASS_LEADER;

            outTrainer->encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE;
            outTrainer->partyFlags = F_TRAINER_PARTY_KANTO_MUS;
            break;

        case TRAINER_ROGUE_BOSS_BROCK:
        case TRAINER_ROGUE_BOSS_LTSURGE:
        case TRAINER_ROGUE_BOSS_KOGA:
        case TRAINER_ROGUE_BOSS_BLAINE:
        case TRAINER_ROGUE_BOSS_GIOVANNI:
        case TRAINER_ROGUE_BOSS_BRUNO:
        case TRAINER_ROGUE_BOSS_LANCE:
        case TRAINER_ROGUE_BOSS_BLUE:
        case TRAINER_ROGUE_BOSS_PROFOAK:
            if(gRogueRun.currentDifficulty >= 12)
                outTrainer->trainerClass = TRAINER_CLASS_CHAMPION;
            else if(gRogueRun.currentDifficulty >= 8)
                outTrainer->trainerClass = TRAINER_CLASS_ELITE_FOUR;
            else
                outTrainer->trainerClass = TRAINER_CLASS_LEADER;

            outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE;
            outTrainer->partyFlags = F_TRAINER_PARTY_KANTO_MUS;
            break;

        // Johto Ver.
        case TRAINER_ROGUE_BOSS_WHITNEY:
        case TRAINER_ROGUE_BOSS_JASMINE:
        case TRAINER_ROGUE_BOSS_CLAIR:
        case TRAINER_ROGUE_BOSS_KAREN:
            if(gRogueRun.currentDifficulty >= 12)
                outTrainer->trainerClass = TRAINER_CLASS_CHAMPION;
            else if(gRogueRun.currentDifficulty >= 8)
                outTrainer->trainerClass = TRAINER_CLASS_ELITE_FOUR;
            else
                outTrainer->trainerClass = TRAINER_CLASS_LEADER;

            outTrainer->encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE;
            outTrainer->partyFlags = F_TRAINER_PARTY_JOHTO_MUS;
            break;

        case TRAINER_ROGUE_BOSS_FALKNER:
        case TRAINER_ROGUE_BOSS_BUGSY:
        case TRAINER_ROGUE_BOSS_MORTY:
        case TRAINER_ROGUE_BOSS_CHUCK:
        case TRAINER_ROGUE_BOSS_PRYCE:
        case TRAINER_ROGUE_BOSS_RED:
        case TRAINER_ROGUE_BOSS_WILL:
        case TRAINER_ROGUE_BOSS_JOHTO_KOGA:
        case TRAINER_ROGUE_BOSS_JOHTO_BRUNO:
        case TRAINER_ROGUE_BOSS_JOHTO_LANCE:
            if(gRogueRun.currentDifficulty >= 12)
                outTrainer->trainerClass = TRAINER_CLASS_CHAMPION;
            else if(gRogueRun.currentDifficulty >= 8)
                outTrainer->trainerClass = TRAINER_CLASS_ELITE_FOUR;
            else
                outTrainer->trainerClass = TRAINER_CLASS_LEADER;

            outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE;
            outTrainer->partyFlags = F_TRAINER_PARTY_JOHTO_MUS;
            break;

        // Kanto Ver.
        case TRAINER_ROGUE_BOSS_KATE:
        case TRAINER_ROGUE_BOSS_RAVEN:
        case TRAINER_ROGUE_BOSS_POKABBIE:
            if(gRogueRun.currentDifficulty >= 12)
                outTrainer->trainerClass = TRAINER_CLASS_CHAMPION;
            else if(gRogueRun.currentDifficulty >= 8)
                outTrainer->trainerClass = TRAINER_CLASS_ELITE_FOUR;
            else
                outTrainer->trainerClass = TRAINER_CLASS_LEADER;

            outTrainer->encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE;
            outTrainer->partyFlags = F_TRAINER_PARTY_SINNOH_MUS;
            break;

        // Glitch Ver.
        case TRAINER_ROGUE_BOSS_ERMA:
        case TRAINER_ROGUE_BOSS_TAILS:
            if(gRogueRun.currentDifficulty >= 12)
                outTrainer->trainerClass = TRAINER_CLASS_CHAMPION;
            else if(gRogueRun.currentDifficulty >= 8)
                outTrainer->trainerClass = TRAINER_CLASS_ELITE_FOUR;
            else
                outTrainer->trainerClass = TRAINER_CLASS_LEADER;

            outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE;
            outTrainer->partyFlags = F_TRAINER_PARTY_SINNOH_MUS;
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
        case TRAINER_ROGUE_BOSS_MIRROR:

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
            
        case TRAINER_ROGUE_MINI_BOSS_RIVAL:
            outTrainer->trainerClass = TRAINER_CLASS_RIVAL;

            switch(gSaveBlock2Ptr->playerGender)
            {    
                case(STYLE_EMR_BRENDAN):
                    outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_FEMALE;
                    outTrainer->trainerPic = TRAINER_PIC_MAY;
                    break;
                case(STYLE_EMR_MAY):
                    outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE;
                    outTrainer->trainerPic = TRAINER_PIC_BRENDAN;
                    break;

                case(STYLE_RED):
                    outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_FEMALE;
                    outTrainer->trainerPic = TRAINER_PIC_LEAF;
                    break;
                case(STYLE_LEAF):
                    outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE;
                    outTrainer->trainerPic = TRAINER_PIC_RED;
                    break;
            };
            break;

        // Kanto
        //
        case TRAINER_ROGUE_BOSS_BROCK:
            outTrainer->trainerPic = TRAINER_PIC_KANTO_BROCK;
            break;
        case TRAINER_ROGUE_BOSS_MISTY:
            outTrainer->trainerPic = TRAINER_PIC_KANTO_MISTY;
            break;
        case TRAINER_ROGUE_BOSS_LTSURGE:
            outTrainer->trainerPic = TRAINER_PIC_KANTO_LTSURGE;
            break;
        case TRAINER_ROGUE_BOSS_ERIKA:
            outTrainer->trainerPic = TRAINER_PIC_KANTO_ERIKA;
            break;
        case TRAINER_ROGUE_BOSS_KOGA:
            outTrainer->trainerPic = TRAINER_PIC_KANTO_KOGA;
            break;
        case TRAINER_ROGUE_BOSS_SABRINA:
            outTrainer->trainerPic = TRAINER_PIC_KANTO_SABRINA;
            break;
        case TRAINER_ROGUE_BOSS_BLAINE:
            outTrainer->trainerPic = TRAINER_PIC_KANTO_BLAINE;
            break;
        case TRAINER_ROGUE_BOSS_GIOVANNI:
            outTrainer->trainerPic = TRAINER_PIC_KANTO_GIOVANNI;
            break;

        case TRAINER_ROGUE_BOSS_LORELEI:
            outTrainer->trainerPic = TRAINER_PIC_KANTO_LORELEI;
            break;
        case TRAINER_ROGUE_BOSS_BRUNO:
            outTrainer->trainerPic = TRAINER_PIC_KANTO_BRUNO;
            break;
        case TRAINER_ROGUE_BOSS_AGATHA:
            outTrainer->trainerPic = TRAINER_PIC_KANTO_AGATHA;
            break;
        case TRAINER_ROGUE_BOSS_LANCE:
            outTrainer->trainerPic = TRAINER_PIC_KANTO_LANCE;
            break;

        case TRAINER_ROGUE_BOSS_BLUE:
            outTrainer->trainerPic = TRAINER_PIC_KANTO_BLUE;
            break;
        case TRAINER_ROGUE_BOSS_PROFOAK:
            outTrainer->trainerPic = TRAINER_PIC_KANTO_PROFOAK;
            break;

        // Johto
        //
        case TRAINER_ROGUE_BOSS_FALKNER:
            outTrainer->trainerPic = TRAINER_PIC_JOHTO_FALKNER;
            break;
        case TRAINER_ROGUE_BOSS_BUGSY:
            outTrainer->trainerPic = TRAINER_PIC_JOHTO_BUGSY;
            break;
        case TRAINER_ROGUE_BOSS_WHITNEY:
            outTrainer->trainerPic = TRAINER_PIC_JOHTO_WHITNEY;
            break;
        case TRAINER_ROGUE_BOSS_MORTY:
            outTrainer->trainerPic = TRAINER_PIC_JOHTO_MORTY;
            break;
        case TRAINER_ROGUE_BOSS_CHUCK:
            outTrainer->trainerPic = TRAINER_PIC_JOHTO_CHUCK;
            break;
        case TRAINER_ROGUE_BOSS_JASMINE:
            outTrainer->trainerPic = TRAINER_PIC_JOHTO_JASMINE;
            break;
        case TRAINER_ROGUE_BOSS_PRYCE:
            outTrainer->trainerPic = TRAINER_PIC_JOHTO_PRYCE;
            break;
        case TRAINER_ROGUE_BOSS_CLAIR:
            outTrainer->trainerPic = TRAINER_PIC_JOHTO_CLAIR;
            break;

        case TRAINER_ROGUE_BOSS_WILL:
            outTrainer->trainerPic = TRAINER_PIC_JOHTO_WILL;
            break;
        case TRAINER_ROGUE_BOSS_JOHTO_KOGA:
            outTrainer->trainerPic = TRAINER_PIC_KANTO_KOGA;
            break;
        case TRAINER_ROGUE_BOSS_JOHTO_BRUNO:
            outTrainer->trainerPic = TRAINER_PIC_KANTO_BRUNO;
            break;
        case TRAINER_ROGUE_BOSS_KAREN:
            outTrainer->trainerPic = TRAINER_PIC_JOHTO_KAREN;
            break;

        case TRAINER_ROGUE_BOSS_JOHTO_LANCE:
            outTrainer->trainerPic = TRAINER_PIC_KANTO_LANCE;
            break;

        case TRAINER_ROGUE_BOSS_RED:
            outTrainer->trainerPic = TRAINER_PIC_JOHTO_RED;
            break;


        case TRAINER_ROGUE_BOSS_KATE:
            outTrainer->trainerPic = TRAINER_PIC_GLITCH_KATE;
            break;

        case TRAINER_ROGUE_BOSS_RAVEN:
            outTrainer->trainerPic = TRAINER_PIC_POKEFAN_F;
            break;

        case TRAINER_ROGUE_BOSS_ERMA:
            outTrainer->trainerPic = TRAINER_PIC_GLITCH_ERMA;
            break;

        case TRAINER_ROGUE_BOSS_TAILS:
            outTrainer->trainerPic = TRAINER_PIC_GLITCH_TAILS;
            break;

        case TRAINER_ROGUE_BOSS_POKABBIE:
            outTrainer->trainerPic = TRAINER_PIC_POKABBIE;
            break;

        // Extra trainer
        case TRAINER_ROGUE_BOSS_ANABEL:
            outTrainer->trainerPic = TRAINER_PIC_SALON_MAIDEN_ANABEL;
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
            outTrainer->encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_MAGMA;
            outTrainer->trainerPic = TRAINER_PIC_MAGMA_GRUNT_F;
            break;
        case TRAINER_ROGUE_MAGMA_M:
            outTrainer->trainerClass = TRAINER_CLASS_TEAM_MAGMA;
            outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE | TRAINER_ENCOUNTER_MUSIC_MAGMA;
            outTrainer->trainerPic = TRAINER_PIC_MAGMA_GRUNT_M;
            break;

        case TRAINER_ROGUE_AQUA_F:
            outTrainer->trainerClass = TRAINER_CLASS_TEAM_AQUA;
            outTrainer->encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_AQUA;
            outTrainer->trainerPic = TRAINER_PIC_AQUA_GRUNT_F;
            break;
        case TRAINER_ROGUE_AQUA_M:
            outTrainer->trainerClass = TRAINER_CLASS_TEAM_AQUA;
            outTrainer->encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE | TRAINER_ENCOUNTER_MUSIC_AQUA;
            outTrainer->trainerPic = TRAINER_PIC_AQUA_GRUNT_M;
            break;
    }
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

extern const u8 gText_ItemShoppingCharm[];
extern const u8 gText_ItemFlinchCharm[];
extern const u8 gText_ItemCritCharm[];
extern const u8 gText_ItemShedSkinCharm[];
extern const u8 gText_ItemWildIVCharm[];
extern const u8 gText_ItemCatchingCharm[];
extern const u8 gText_ItemGraceCharm[];

extern const u8 gText_ItemShoppingCurse[];
extern const u8 gText_ItemFlinchCurse[];
extern const u8 gText_ItemCritCurse[];
extern const u8 gText_ItemShedSkinCurse[];
extern const u8 gText_ItemWildIVCurse[];
extern const u8 gText_ItemCatchingCurse[];
extern const u8 gText_ItemGraceCurse[];
extern const u8 gText_ItemPartyCurse[];
extern const u8 gText_ItemEverstoneCurse[];


extern const u8 sItemShoppingCharmDesc[];
extern const u8 sItemFlinchCharmDesc[];
extern const u8 sItemCritCharmDesc[];
extern const u8 sItemShedSkinCharmDesc[];
extern const u8 sItemWildIVCharmDesc[];
extern const u8 sItemCatchingCharmDesc[];
extern const u8 sItemGraceCharmDesc[];

extern const u8 sItemShoppingCurseDesc[];
extern const u8 sItemFlinchCurseDesc[];
extern const u8 sItemCritCurseDesc[];
extern const u8 sItemShedSkinCurseDesc[];
extern const u8 sItemWildIVCurseDesc[];
extern const u8 sItemCatchingCurseDesc[];
extern const u8 sItemGraceCurseDesc[];
extern const u8 sItemPartyCurseDesc[];
extern const u8 sItemEverstoneCurseDesc[];


extern const u8 gText_ItemPlaceholderDesc[];
extern const u8 gText_ItemQuestLogDesc[];
extern const u8 gText_EscapeRopeDesc[];
extern const u8 gText_ItemLinkCableDesc[];

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


        case ITEM_SHOP_PRICE_CHARM:
            return gText_ItemShoppingCharm;

        case ITEM_FLINCH_CHARM:
            return gText_ItemFlinchCharm;

        case ITEM_CRIT_CHARM:
            return gText_ItemCritCharm;

        case ITEM_SHED_SKIN_CHARM:
            return gText_ItemShedSkinCharm;

        case ITEM_WILD_IV_CHARM:
            return gText_ItemWildIVCharm;

        case ITEM_CATCHING_CHARM:
            return gText_ItemCatchingCharm;

        case ITEM_GRACE_CHARM:
            return gText_ItemGraceCharm;


        case ITEM_SHOP_PRICE_CURSE:
            return gText_ItemShoppingCurse;

        case ITEM_FLINCH_CURSE:
            return gText_ItemFlinchCurse;

        case ITEM_CRIT_CURSE:
            return gText_ItemCritCurse;

        case ITEM_SHED_SKIN_CURSE:
            return gText_ItemShedSkinCurse;

        case ITEM_WILD_IV_CURSE:
            return gText_ItemWildIVCurse;

        case ITEM_CATCHING_CURSE:
            return gText_ItemCatchingCurse;

        case ITEM_GRACE_CURSE:
            return gText_ItemGraceCurse;

        case ITEM_PARTY_CURSE:
            return gText_ItemPartyCurse;

        case ITEM_EVERSTONE_CURSE:
            return gText_ItemEverstoneCurse;
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

    if(itemId >= FIRST_ITEM_CHARM && itemId <= LAST_ITEM_CHARM)
    {
        // Charm icon
        return which == 0 ? gItemIcon_RogueCharm : gItemIconPalette_RogueCharm;
    }

    if(itemId >= FIRST_ITEM_CURSE && itemId <= LAST_ITEM_CURSE)
    {
        // Curse icon
        return which == 0 ? gItemIcon_RogueCurse : gItemIconPalette_RogueCurse;
    }

    return gItemIconTable[itemId][which];
}

void Rogue_ModifyItem(u16 itemId, struct Item* outItem)
{
    itemId = SanitizeItemId(itemId);
    memcpy(outItem, &gItems[itemId], sizeof(struct Item));

    // Charm/Curse items
    //
    if((itemId >= FIRST_ITEM_CHARM && itemId <= LAST_ITEM_CHARM) || (itemId >= FIRST_ITEM_CURSE && itemId <= LAST_ITEM_CURSE))
    {
        outItem->itemId = itemId;
        outItem->price = 0;
        outItem->description = gText_ItemPlaceholderDesc;
        outItem->importance = 0;
        outItem->registrability = FALSE;
        outItem->pocket = POCKET_KEY_ITEMS;
        outItem->type = ITEM_USE_FIELD;
        outItem->holdEffect = 0;
        outItem->fieldUseFunc = ItemUseOutOfBattle_CannotUse;
    }

    // Custom desc for charms/curses
    switch(itemId)
    {
        case ITEM_SHOP_PRICE_CHARM:
            outItem->description = sItemShoppingCharmDesc;
            break;

        case ITEM_FLINCH_CHARM:
            outItem->description = sItemFlinchCharmDesc;
            break;

        case ITEM_CRIT_CHARM:
            outItem->description = sItemCritCharmDesc;
            break;

        case ITEM_SHED_SKIN_CHARM:
            outItem->description = sItemShedSkinCharmDesc;
            break;

        case ITEM_WILD_IV_CHARM:
            outItem->description = sItemWildIVCharmDesc;
            break;

        case ITEM_CATCHING_CHARM:
            outItem->description = sItemCatchingCharmDesc;
            break;

        case ITEM_GRACE_CHARM:
            outItem->description = sItemGraceCharmDesc;
            break;


        case ITEM_SHOP_PRICE_CURSE:
            outItem->description = sItemShoppingCurseDesc;
            break;

        case ITEM_FLINCH_CURSE:
            outItem->description = sItemFlinchCurseDesc;
            break;

        case ITEM_CRIT_CURSE:
            outItem->description = sItemCritCurseDesc;
            break;

        case ITEM_SHED_SKIN_CURSE:
            outItem->description = sItemShedSkinCurseDesc;
            break;

        case ITEM_WILD_IV_CURSE:
            outItem->description = sItemWildIVCurseDesc;
            break;

        case ITEM_CATCHING_CURSE:
            outItem->description = sItemCatchingCurseDesc;
            break;

        case ITEM_GRACE_CURSE:
            outItem->description = sItemGraceCurseDesc;
            break;

        case ITEM_PARTY_CURSE:
            outItem->description = sItemPartyCurseDesc;
            break;

        case ITEM_EVERSTONE_CURSE:
            outItem->description = sItemEverstoneCurseDesc;
            break;
    }

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
        case ITEM_ESCAPE_ROPE:
            outItem->price = 6000;
            outItem->description = gText_EscapeRopeDesc;
            break;

        // Evo item prices
        case ITEM_LINK_CABLE:
            outItem->price = 2100;
            outItem->holdEffect = 0;//HOLD_EFFECT_NONE;
            outItem->description = gText_ItemLinkCableDesc;
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
        case ITEM_ABILITY_CAPSULE:
            outItem->price = 3000;
            break;

        case ITEM_ABILITY_PATCH:
            outItem->price = 6000;
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