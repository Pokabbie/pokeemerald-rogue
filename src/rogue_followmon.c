#include "global.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
//#include "constants/abilities.h"
//#include "constants/heal_locations.h"
//#include "constants/hold_effects.h"
//#include "constants/items.h"
//#include "constants/layouts.h"
#include "constants/rogue.h"
//#include "data.h"
//
//#include "battle.h"
//#include "battle_setup.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "fieldmap.h"
#include "follow_me.h"
//#include "item.h"
//#include "item_use.h"
//#include "money.h"
//#include "overworld.h"
//#include "pokedex.h"
#include "pokemon.h"
//#include "random.h"
//#include "strings.h"
//#include "string_util.h"
//#include "text.h"

#include "rogue.h"

//#include "rogue_query.h"
//#include "rogue_baked.h"
//#include "rogue_campaign.h"
//#include "rogue_controller.h"

#include "rogue_followmon.h"

extern const struct ObjectEventGraphicsInfo *const gObjectEventMonGraphicsInfoPointers[NUM_SPECIES];
extern const struct ObjectEventGraphicsInfo *const gObjectEventShinyMonGraphicsInfoPointers[NUM_SPECIES];

static u16 MonSpeciesToFollowSpecies(u16 species, bool8 isShiny)
{
    if(species == SPECIES_NONE)
        return SPECIES_NONE;

    return species + (isShiny ? FOLLOWMON_SHINY_OFFSET : 0);
}

static u16 MonToFollowSpecies(struct Pokemon* mon)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES);
    return MonSpeciesToFollowSpecies(species, IsMonShiny(mon));
}

static u16 GetPartnerFollowSpecies()
{
    return MonToFollowSpecies(&gPlayerParty[0]);
}

const struct ObjectEventGraphicsInfo *GetFollowMonObjectEventInfo(u16 graphicsId)
{
    u16 species;

    if(graphicsId >= OBJ_EVENT_GFX_FOLLOW_MON_0 && graphicsId <= OBJ_EVENT_GFX_FOLLOW_MON_F)
    {
        u16 varNo = graphicsId - OBJ_EVENT_GFX_FOLLOW_MON_0;
        species = MonSpeciesToFollowSpecies(VarGet(VAR_FOLLOW_MON_0 + varNo), FALSE);
    }
    else // OBJ_EVENT_GFX_FOLLOW_MON_PARTNER
    {
        species = GetPartnerFollowSpecies();
    }

    if(species >= FOLLOWMON_SHINY_OFFSET)
    {
        species -= FOLLOWMON_SHINY_OFFSET;

        // Return the shiny gfx if we have one
        if(species < NUM_SPECIES && gObjectEventShinyMonGraphicsInfoPointers[species])
            return gObjectEventShinyMonGraphicsInfoPointers[species];
    }

    // Return the normal gfx if we have one
    if(species < NUM_SPECIES && gObjectEventMonGraphicsInfoPointers[species])
        return gObjectEventMonGraphicsInfoPointers[species];

    // Return a fallback sprite
    return gObjectEventMonGraphicsInfoPointers[SPECIES_NONE];
}

void SetupFollowParterMonObjectEvent()
{
    bool8 shouldFollowMonBeVisible = TRUE;

    if(GetPartnerFollowSpecies() == SPECIES_NONE)
        shouldFollowMonBeVisible = FALSE;

    if(shouldFollowMonBeVisible)
    {
        if(!PlayerHasFollower())
        {
            u8 localId = 63;
            u8 objectEventId = SpawnSpecialObjectEventParameterized2(
                OBJ_EVENT_GFX_FOLLOW_MON_PARTNER,
                MOVEMENT_TYPE_FACE_DOWN,
                localId,
                gSaveBlock1Ptr->pos.x + MAP_OFFSET,
                gSaveBlock1Ptr->pos.y + MAP_OFFSET,
                3,
                NULL
            );

            SetUpFollowerSprite(localId, FOLLOWER_FLAG_CAN_LEAVE_ROUTE | FOLLOWER_FLAG_FOLLOW_DURING_SCRIPT); // Check FOLLOWER_FLAG_ALL
            gSaveBlock2Ptr->follower.scriptId = FOLLOW_SCRIPT_ID_FOLLOW_MON;
        }
    }
    else
    {
        if(PlayerHasFollower())
            DestroyFollower();
    }
}

void ResetFollowParterMonObjectEvent()
{
    if(PlayerHasFollower())
        DestroyFollower();
}