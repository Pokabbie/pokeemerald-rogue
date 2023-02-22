#include "global.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/rogue.h"

#include "event_data.h"
#include "event_object_movement.h"
#include "fieldmap.h"
#include "follow_me.h"
#include "pokemon.h"
#include "script.h"

#include "rogue.h"
#include "rogue_followmon.h"

static EWRAM_DATA bool8 sPendingFollowMonInteraction = FALSE;

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
        species = VarGet(VAR_FOLLOW_MON_0 + varNo);
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

void FollowMon_SetGraphics(u16 id, u16 species, bool8 isShiny)
{
    u16 gfxSpecies = MonSpeciesToFollowSpecies(species, isShiny);
    VarSet(VAR_FOLLOW_MON_0 + id, gfxSpecies);
}

static bool8 IsFollowMonObject(struct ObjectEvent* object)
{
    if(object->graphicsId >= OBJ_EVENT_GFX_FOLLOW_MON_0 && object->graphicsId <= OBJ_EVENT_GFX_FOLLOW_MON_F)
    {
        return TRUE;
    }

    if(object->graphicsId >= OBJ_EVENT_GFX_VAR_FIRST && object->graphicsId <= OBJ_EVENT_GFX_VAR_LAST)
    {
        u16 gfxId = VarGet(VAR_OBJ_GFX_ID_0 + (object->graphicsId - OBJ_EVENT_GFX_VAR_FIRST));
        if(gfxId >= OBJ_EVENT_GFX_FOLLOW_MON_0 && gfxId <= OBJ_EVENT_GFX_FOLLOW_MON_F)
        {
            return TRUE;
        }
    }

    return FALSE;
}

static bool8 AreElevationsCompatible(u8 a, u8 b)
{
    if (a == 0 || b == 0)
        return TRUE;

    if (a != b)
        return FALSE;

    return TRUE;
}

bool8 FollowMon_IsCollisionExempt(struct ObjectEvent* obstacle, struct ObjectEvent* collider)
{
    struct ObjectEvent* player = &gObjectEvents[gPlayerAvatar.objectEventId];
    
    if (collider == player)
    {
        // Player can walk on top of follow mon
        if(IsFollowMonObject(obstacle))
        {
            sPendingFollowMonInteraction = TRUE;
            return TRUE;
        }
    }
    else if(obstacle == player)
    {
        // Follow mon can walk onto player
        if(IsFollowMonObject(collider))
        {
            sPendingFollowMonInteraction = TRUE;
            return TRUE;
        }
    }

    return FALSE;
}

bool8 FollowMon_ProcessMonInteraction()
{
    if(sPendingFollowMonInteraction)
    {
        u8 i;
        struct ObjectEvent *curObject;
        struct ObjectEvent* player = &gObjectEvents[gPlayerAvatar.objectEventId];
    
        sPendingFollowMonInteraction = FALSE;
        
        for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
        {
            curObject = &gObjectEvents[i];
            if (curObject->active && curObject != player && IsFollowMonObject(curObject))
            {
                if ((curObject->currentCoords.x == player->currentCoords.x && curObject->currentCoords.y == player->currentCoords.y) || (curObject->previousCoords.x == player->currentCoords.x && curObject->previousCoords.y == player->currentCoords.y))
                {
                    if (AreElevationsCompatible(curObject->currentElevation, player->currentElevation))
                    {
                        // There is a valid collision so exectute the attached script
                        const u8* script = GetObjectEventScriptPointerByObjectEventId(i);

                        if(script != NULL)
                        {
                            VarSet(VAR_LAST_TALKED, curObject->localId);
                            ScriptContext1_SetupScript(script);
                            return TRUE;
                        }
                    }
                }
            }
        }
    }

    return FALSE;
}

void FollowMon_GetSpeciesFromLastInteracted(u16* species, bool8* isShiny)
{
    struct ObjectEvent *curObject;
    u8 lastTalkedId = VarGet(VAR_LAST_TALKED);
    u8 objEventId = GetObjectEventIdByLocalIdAndMap(lastTalkedId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);

    *species = FALSE;
    *isShiny = FALSE;

    if(objEventId < OBJECT_EVENTS_COUNT)
    {
        curObject = &gObjectEvents[objEventId];
        if(IsFollowMonObject(curObject))
        {
            u16 varNo = curObject->graphicsId - OBJ_EVENT_GFX_FOLLOW_MON_0;
            u16 gfxSpecies = VarGet(VAR_FOLLOW_MON_0 + varNo);

            if(gfxSpecies >= FOLLOWMON_SHINY_OFFSET)
            {
                *species = gfxSpecies - FOLLOWMON_SHINY_OFFSET;
                *isShiny = TRUE;
            }
            else
            {
                *species = gfxSpecies;
                *isShiny = FALSE;
            }
        }
    }
}