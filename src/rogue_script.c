#include "global.h"
#include "constants/battle.h"
#include "constants/rogue.h"

#include "event_data.h"
#include "pokemon.h"
#include "random.h"

#include "rogue_baked.h"
#include "rogue_controller.h"
#include "rogue_query.h"

#include "rogue_script.h"


void Rogue_RandomisePartyMon(void)
{
    u8 monIdx = gSpecialVar_0x8004;

    if(monIdx == 255)
    {
        // Entire team
        u8 i;
        u16 queryCount;
        u16 species;
        u16 heldItem;
        u8 targetlevel = GetMonData(&gPlayerParty[0], MON_DATA_LEVEL);

        // Query for the current route type
        RogueQuery_Clear();

        RogueQuery_SpeciesIsValid();
        RogueQuery_SpeciesExcludeCommon();
        RogueQuery_TransformToEggSpecies();

        // Evolve the species to just below the wild encounter level
        RogueQuery_EvolveSpeciesAndKeepPreEvo(targetlevel, TRUE);

        queryCount = RogueQuery_UncollapsedSpeciesSize();

        for(i = 0; i < gPlayerPartyCount; ++i)
        {
            targetlevel = GetMonData(&gPlayerParty[i], MON_DATA_LEVEL);
            heldItem = GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM);

            species = RogueQuery_AtUncollapsedIndex(Random() % queryCount);

            ZeroMonData(&gPlayerParty[i]);
            CreateMon(&gPlayerParty[i], species, targetlevel, USE_RANDOM_IVS, 0, 0, OT_ID_PLAYER_ID, 0);

            SetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM, &heldItem);
        }
    }
    else
    {
        u16 queryCount;
        u16 species;
        u8 targetlevel = GetMonData(&gPlayerParty[monIdx], MON_DATA_LEVEL);
        u16 heldItem = GetMonData(&gPlayerParty[monIdx], MON_DATA_HELD_ITEM);

        // Query for the current route type
        RogueQuery_Clear();

        RogueQuery_SpeciesIsValid();
        RogueQuery_SpeciesExcludeCommon();
        RogueQuery_TransformToEggSpecies();

        // Evolve the species to just below the wild encounter level
        RogueQuery_EvolveSpeciesAndKeepPreEvo(targetlevel, TRUE);

        queryCount = RogueQuery_UncollapsedSpeciesSize();
        species = RogueQuery_AtUncollapsedIndex(Random() % queryCount);

        ZeroMonData(&gPlayerParty[monIdx]);
        CreateMon(&gPlayerParty[monIdx], species, targetlevel, USE_RANDOM_IVS, 0, 0, OT_ID_PLAYER_ID, 0);

        SetMonData(&gPlayerParty[monIdx], MON_DATA_HELD_ITEM, &heldItem);
    }
}

void Rogue_AlterMonIVs(void)
{
    const u16 delta = 10;

    u16 statId;
    u16 ivAmount;
    u16 monIdx = gSpecialVar_0x8004;
    u16 statOp = gSpecialVar_0x8005;

    if(monIdx == 255)
    {
        // Entire team
        u8 i;

        for(i = 0; i < gPlayerPartyCount; ++i)
        {
            for(statId = MON_DATA_HP_IV; statId <= MON_DATA_SPDEF_IV; ++statId)
            {
                ivAmount = GetMonData(&gPlayerParty[i], statId);

                if(statOp == 0)
                {
                    ivAmount += delta;
                    ivAmount = min(31, ivAmount);
                }
                else
                {
                    if(ivAmount < delta)
                        ivAmount = 0;
                    else
                        ivAmount -= delta;
                }

                SetMonData(&gPlayerParty[i], statId, &ivAmount);
                CalculateMonStats(&gPlayerParty[i]);
            }
        }
    }
    else
    {
        // Modify just 1 mon
        for(statId = MON_DATA_HP_IV; statId <= MON_DATA_SPDEF_IV; ++statId)
        {
            ivAmount = GetMonData(&gPlayerParty[monIdx], statId);

            if(statOp == 0)
            {
                ivAmount += delta;
                ivAmount = min(31, ivAmount);
            }
            else
            {
                if(ivAmount < delta)
                    ivAmount = 0;
                else
                    ivAmount -= delta;
            }

            SetMonData(&gPlayerParty[monIdx], statId, &ivAmount);
            CalculateMonStats(&gPlayerParty[monIdx]);
        }
    }
}

void Rogue_ApplyStatusToMon(void)
{
    u16 statusAilment;
    u16 monIdx = gSpecialVar_0x8004;

    switch(gSpecialVar_0x8005)
    {
        case 0:
            statusAilment = STATUS1_POISON;
            break;

        case 1:
            statusAilment = STATUS1_PARALYSIS;
            break;

        case 2:
            statusAilment = STATUS1_SLEEP;
            break;

        case 3:
            statusAilment = STATUS1_FREEZE;
            break;

        case 4:
            statusAilment = STATUS1_BURN;
            break;
    }

    if(monIdx == 255)
    {
        // Entire team
        u8 i;

        for(i = 0; i < gPlayerPartyCount; ++i)
        {
            SetMonData(&gPlayerParty[i], MON_DATA_STATUS, &statusAilment);
        }
    }
    else
    {
        SetMonData(&gPlayerParty[monIdx], MON_DATA_STATUS, &statusAilment);
    }
}

void Rogue_ReducePartySize(void)
{
    u16 monIdx = gSpecialVar_0x8004;

    if(monIdx < gPlayerPartyCount)
    {
        RemoveMonAtSlot(monIdx, TRUE);
    }

    VarSet(VAR_ROGUE_MAX_PARTY_SIZE, VarGet(VAR_ROGUE_MAX_PARTY_SIZE) - 1);
}

u16 Rogue_GetMonEvoCount(void)
{
    u16 monIdx = gSpecialVar_0x8004;
    u16 species = GetMonData(&gPlayerParty[monIdx], MON_DATA_SPECIES);

    if(species != SPECIES_NONE)
    {
        u16 e;
        struct Evolution evo;
        u16 count = 0;

        for (e = 0; e < EVOS_PER_MON; e++)
        {
            Rogue_ModifyEvolution(species, e, &evo);

            if (evo.targetSpecies != SPECIES_NONE)
            {
#ifdef ROGUE_EXPANSION
                if(evolution.method != EVO_MEGA_EVOLUTION &&
                    evolution.method != EVO_MOVE_MEGA_EVOLUTION &&
                    evolution.method != EVO_PRIMAL_REVERSION
                )
#endif
                {
                    ++count;
                }
            }
        }

        return count;
    }

    return 0;
}

void Rogue_GetMonEvoParams(void)
{
    u16 monIdx = gSpecialVar_0x8004;
    u16 evoIdx = gSpecialVar_0x8005;
    u16 species = GetMonData(&gPlayerParty[monIdx], MON_DATA_SPECIES);

    gSpecialVar_0x8006 = 0;
    gSpecialVar_0x8007 = 0;

    if(species != SPECIES_NONE)
    {        // evoIdx doesn't mean array idx annoyingly as evos can be toggled/changed
        u16 e;
        struct Evolution evo;
        u16 count = 0;

        for (e = 0; e < EVOS_PER_MON; e++)
        {
            Rogue_ModifyEvolution(species, e, &evo);

            if (evo.targetSpecies != SPECIES_NONE)
            {
#ifdef ROGUE_EXPANSION
                if(evolution.method != EVO_MEGA_EVOLUTION &&
                    evolution.method != EVO_MOVE_MEGA_EVOLUTION &&
                    evolution.method != EVO_PRIMAL_REVERSION
                )
#endif
                {
                    if(count++ == evoIdx)
                    {
                        gSpecialVar_0x8006 = evo.method;
                        gSpecialVar_0x8007 = evo.param;
                        return;
                    }
                }
            }
        }
    }
}