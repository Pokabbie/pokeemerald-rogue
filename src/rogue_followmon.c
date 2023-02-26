#include "global.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/rogue.h"

#include "event_data.h"
#include "event_object_movement.h"
#include "fieldmap.h"
#include "field_player_avatar.h"
#include "follow_me.h"
#include "metatile_behavior.h"
#include "pokemon.h"
#include "random.h"
#include "script.h"

#include "rogue.h"
#include "rogue_controller.h"
#include "rogue_followmon.h"

struct FollowMonData
{
    bool8 pendingInterction;
    u16 spawnCountdown;
    u16 spawnSlot;
};

static EWRAM_DATA struct FollowMonData sFollowMonData = { 0 };

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
        if(!FollowMon_IsPartnerMonActive())
        {
            u16 localId = OBJ_EVENT_ID_FOLLOWER;
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
        if(FollowMon_IsPartnerMonActive())
            DestroyFollower();
    }
}

void ResetFollowParterMonObjectEvent()
{
    if(FollowMon_IsPartnerMonActive())
        DestroyFollower();
}

void FollowMon_SetGraphics(u16 id, u16 species, bool8 isShiny)
{
    u16 gfxSpecies = MonSpeciesToFollowSpecies(species, isShiny);
    VarSet(VAR_FOLLOW_MON_0 + id, gfxSpecies);
}

bool8 FollowMon_IsPartnerMonActive()
{
    // TODO - If we use other partners, gonna have to check object too
    return PlayerHasFollower();
}

bool8 FollowMon_IsMonObject(struct ObjectEvent* object, bool8 ignorePartnerMon)
{
    u16 localId = object->localId;
    u16 graphicsId = object->graphicsId;

    if(localId >= OBJ_EVENT_ID_FOLLOW_MON_FIRST && localId <= OBJ_EVENT_ID_FOLLOW_MON_LAST)
    {
        // Fast check
        return TRUE;
    }

    // Check gfx id
    if(graphicsId >= OBJ_EVENT_GFX_VAR_FIRST && graphicsId <= OBJ_EVENT_GFX_VAR_LAST)
    {
        graphicsId = VarGet(VAR_OBJ_GFX_ID_0 + (object->graphicsId - OBJ_EVENT_GFX_VAR_FIRST));
    }

    if(object->graphicsId >= OBJ_EVENT_GFX_FOLLOW_MON_FIRST && object->graphicsId <= OBJ_EVENT_GFX_FOLLOW_MON_LAST)
    {
        if(!ignorePartnerMon || graphicsId != OBJ_EVENT_GFX_FOLLOW_MON_PARTNER )
            return TRUE;
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
        if(FollowMon_IsMonObject(obstacle, TRUE))
        {
            sFollowMonData.pendingInterction = TRUE;
            return TRUE;
        }
    }
    else if(obstacle == player)
    {
        // Follow mon can walk onto player
        if(FollowMon_IsMonObject(collider, TRUE))
        {
            sFollowMonData.pendingInterction = TRUE;
            return TRUE;
        }
    }

    return FALSE;
}

bool8 FollowMon_ProcessMonInteraction()
{
    if(sFollowMonData.pendingInterction)
    {
        u8 i;
        struct ObjectEvent *curObject;
        struct ObjectEvent* player = &gObjectEvents[gPlayerAvatar.objectEventId];
    
        sFollowMonData.pendingInterction = FALSE;
        
        for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
        {
            curObject = &gObjectEvents[i];
            if (curObject->active && curObject != player && FollowMon_IsMonObject(curObject, TRUE))
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
        if(FollowMon_IsMonObject(curObject, TRUE))
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

static u16 NextSpawnMonSlot()
{
    u16 slot;
    u16 species;
    u8 level; // ignore
    u32 personality; // ignore

    sFollowMonData.spawnSlot = (sFollowMonData.spawnSlot + 1) % 6; // Care with increasing slot count as it can cause lag
    slot = sFollowMonData.spawnSlot;

    Rogue_CreateWildMon(0, &species, &level, &personality);

    FollowMon_SetGraphics(slot, species, (Random() % Rogue_GetShinyOdds()) == 0);

    RemoveObjectEventByLocalIdAndMap(OBJ_EVENT_ID_FOLLOW_MON_FIRST + slot, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
    return slot;
}

static bool8 TrySelectTile(s16* outX, s16* outY)
{
    u16 tileBehavior;
    s16 playerX, playerY;
    s16 x, y;

    // Select a random tile in [-7, -4] [7, 4] range
    // Make sure is not directly next to player
    do
    {
        x = (s16)(Random() % 15) - 7;
        y = (s16)(Random() % 9) - 4;
    }
    while (abs(x) <= 2 && abs(y) <= 2);
    
    PlayerGetDestCoords(&playerX, &playerY);
    x += playerX;
    y += playerY;
    tileBehavior = MapGridGetMetatileBehaviorAt(x, y);

    //bool8 MetatileBehavior_IsLandWildEncounter(u8);
    //bool8 MetatileBehavior_IsWaterWildEncounter(u8);
    if(MetatileBehavior_IsLandWildEncounter(tileBehavior) && !MapGridIsImpassableAt(x, y))
    {
        *outX = x;
        *outY = y;
        return TRUE;
    }

    return FALSE;
}

void FollowMon_OverworldCB()
{
    if(!Rogue_AreWildMonEnabled())
    {
        return;
    }

    if(sFollowMonData.spawnCountdown == 0)
    {
        s16 x, y;

        if(TrySelectTile(&x, &y))
        {
            u16 spawnSlot = NextSpawnMonSlot();

            if(spawnSlot != 0xF)
            {
                u8 localId = OBJ_EVENT_ID_FOLLOW_MON_FIRST + spawnSlot;
                u8 objectEventId = SpawnSpecialObjectEventParameterized(
                    OBJ_EVENT_GFX_FOLLOW_MON_0 + spawnSlot,
                    MOVEMENT_TYPE_WANDER_AROUND,
                    localId,
                    x,
                    y,
                    MapGridGetElevationAt(x, y)
                );

                gObjectEvents[objectEventId].rangeX = 8;
                gObjectEvents[objectEventId].rangeY = 8;

                // TODO - Spawn faster if running or cycling
                sFollowMonData.spawnCountdown = 60 * (3 + Random() % 3);
            }
        }
    }
    else
    {
        --sFollowMonData.spawnCountdown;
    }
}