#include "global.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/map_types.h"
#include "constants/rogue.h"
#include "constants/songs.h"

#include "event_data.h"
#include "event_object_movement.h"
#include "fieldmap.h"
#include "field_player_avatar.h"
#include "follow_me.h"
#include "metatile_behavior.h"
#include "pokemon.h"
#include "safari_zone.h"
#include "sound.h"
#include "random.h"
#include "script.h"

#include "rogue.h"
#include "rogue_controller.h"
#include "rogue_followmon.h"
#include "rogue_popup.h"

// Care with increasing slot count as it can cause lag
#define MAX_SPAWN_SLOTS 6

struct FollowMonData
{
    bool8 pendingInterction;
    u8 activeCount;
    u8 encounterChainCount;
    u16 spawnCountdown;
    u16 spawnSlot;
    u16 pendingSpawnAnim;
    u16 encounterChainSpecies;
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
        species = FollowMon_GetPartnerFollowSpecies();
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

    if(FollowMon_GetPartnerFollowSpecies() == SPECIES_NONE)
        shouldFollowMonBeVisible = FALSE;

    // Don't show if on bike or surfing
    if(gPlayerAvatar.flags & (PLAYER_AVATAR_FLAG_MACH_BIKE | PLAYER_AVATAR_FLAG_ACRO_BIKE | PLAYER_AVATAR_FLAG_SURFING | PLAYER_AVATAR_FLAG_UNDERWATER))
        shouldFollowMonBeVisible = FALSE;

    if(shouldFollowMonBeVisible)
    {
        if(!FollowMon_IsPartnerMonActive())
        {
            u16 localId = OBJ_EVENT_ID_FOLLOWER;
            u8 objectEventId = SpawnSpecialObjectEventParameterized(
                OBJ_EVENT_GFX_FOLLOW_MON_PARTNER,
                MOVEMENT_TYPE_FACE_DOWN,
                localId,
                gSaveBlock1Ptr->pos.x + MAP_OFFSET,
                gSaveBlock1Ptr->pos.y + MAP_OFFSET,
                3
            );

            SetUpFollowerSprite(localId, FOLLOWER_FLAG_CAN_LEAVE_ROUTE | FOLLOWER_FLAG_FOLLOW_DURING_SCRIPT); // Check FOLLOWER_FLAG_ALL
            gSaveBlock2Ptr->follower.scriptId = FOLLOW_SCRIPT_ID_FOLLOW_MON;
        }
    }
    else
    {
        ResetFollowParterMonObjectEvent();
    }
}

void ResetFollowParterMonObjectEvent()
{
    if(FollowMon_IsPartnerMonActive())
        DestroyFollower();
}

void UpdateWildEncounterChain(u16 species)
{
    if(sFollowMonData.encounterChainSpecies != species)
    {
        if(sFollowMonData.encounterChainCount > 3)
            Rogue_PushPopup(POPUP_MSG_ENCOUNTER_CHAIN_END, sFollowMonData.encounterChainSpecies);

        sFollowMonData.encounterChainSpecies = species;
        sFollowMonData.encounterChainCount = 0;
    }

    if(species != SPECIES_NONE)
    {
        if(sFollowMonData.encounterChainCount < 255)
            ++sFollowMonData.encounterChainCount;

        if(sFollowMonData.encounterChainCount >= 3)
            Rogue_PushPopup(POPUP_MSG_ENCOUNTER_CHAIN, 0);
    }
}

u16 GetWildChainSpecies()
{
    return sFollowMonData.encounterChainSpecies;
}

u8 GetWildChainCount()
{
    return sFollowMonData.encounterChainCount;
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

u16 FollowMon_GetPartnerFollowSpecies()
{
    return MonToFollowSpecies(&gPlayerParty[0]);
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

bool8 FollowMon_ShouldAlwaysAnimation(struct ObjectEvent *objectEvent)
{
    // TODO - Should animate in legendary rooms???

    if(!Rogue_IsRunActive() || objectEvent->graphicsId == OBJ_EVENT_GFX_FOLLOW_MON_PARTNER)
    {
        // Only fully animate partner mons
        return TRUE;
    }

    return FALSE;
}

bool8 FollowMon_ShouldAnimationGrass(struct ObjectEvent *objectEvent)
{
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
    
    if(Rogue_IsRunActive() || GetSafariZoneFlag())
    {
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
    }

    return FALSE;
}

bool8 FollowMon_ProcessMonInteraction()
{
    if(Rogue_AreWildMonEnabled() && VarGet(VAR_REPEL_STEP_COUNT) != 0)
    {
        // Never auto trigger battle whilst repel is active
        sFollowMonData.pendingInterction = FALSE;
        return FALSE;
    }

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

    if(lastTalkedId == OBJ_EVENT_ID_FOLLOWER)
    {
        u16 gfxSpecies = FollowMon_GetPartnerFollowSpecies();

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
    else
    {
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
}

static u8 FindObjectEventForGfx(u16 gfxId);

static u16 NextSpawnMonSlot()
{
    u16 slot;
    u16 species;
    u8 level; // ignore
    bool8 isShiny;

    // Attempt to find a free slot first
    for(slot = 0; slot < MAX_SPAWN_SLOTS; ++slot)
    {
        if(FindObjectEventForGfx(OBJ_EVENT_GFX_FOLLOW_MON_0 + slot) == OBJECT_EVENTS_COUNT)
            break;
    }

    // All mon slots are in use
    if(slot == MAX_SPAWN_SLOTS)
    {
        // Cycle through so we remove the oldest mon first
        sFollowMonData.spawnSlot = (sFollowMonData.spawnSlot + 1) % MAX_SPAWN_SLOTS;
        slot = sFollowMonData.spawnSlot;
    }

    if(gPlayerAvatar.flags & (PLAYER_AVATAR_FLAG_SURFING | PLAYER_AVATAR_FLAG_UNDERWATER))
        Rogue_CreateWildMon(1, &species, &level, &isShiny); // WILD_AREA_WATER
    else
        Rogue_CreateWildMon(0, &species, &level, &isShiny);

    FollowMon_SetGraphics(slot, species, isShiny);

    RemoveObjectEventByLocalIdAndMap(OBJ_EVENT_ID_FOLLOW_MON_FIRST + slot, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
    return slot;
}

static bool8 CheckForObjectEventAtLocation(s16 x, s16 y)
{
    u8 i;
    for(i = 0; i < OBJECT_EVENTS_COUNT; ++i)
    {
        if(gObjectEvents[i].active && gObjectEvents[i].currentCoords.x == x && gObjectEvents[i].currentCoords.y == y)
        {
            return TRUE;
        }
    }

    return FALSE;
}

static bool8 TrySelectTile(s16* outX, s16* outY)
{
    u8 tryCount;
    u16 tileBehavior;
    s16 playerX, playerY;
    s16 x, y;
    u8 closeDistance;

    for(tryCount = 0; tryCount < 3; ++tryCount)
    {
        // Spawn further away when surfing
        if(gPlayerAvatar.flags & (PLAYER_AVATAR_FLAG_SURFING | PLAYER_AVATAR_FLAG_UNDERWATER))
            closeDistance = 3;
        else
            closeDistance = 1;

        // Select a random tile in [-7, -4] [7, 4] range
        // Make sure is not directly next to player
        do
        {
            x = (s16)(Random() % 15) - 7;
            y = (s16)(Random() % 9) - 4;
        }
        while (abs(x) <= closeDistance && abs(y) <= closeDistance);
        
        PlayerGetDestCoords(&playerX, &playerY);
        x += playerX;
        y += playerY;
        tileBehavior = MapGridGetMetatileBehaviorAt(x, y);

        if(gPlayerAvatar.flags & (PLAYER_AVATAR_FLAG_SURFING | PLAYER_AVATAR_FLAG_UNDERWATER))
        {
            if(MetatileBehavior_IsWaterWildEncounter(tileBehavior) && !MapGridIsImpassableAt(x, y))
            {
                *outX = x;
                *outY = y;

                if(!CheckForObjectEventAtLocation(x, y))
                    return TRUE;
            }
        }
        else
        {
            if(MetatileBehavior_IsLandWildEncounter(tileBehavior) && !MapGridIsImpassableAt(x, y))
            {
                *outX = x;
                *outY = y;

                if(!CheckForObjectEventAtLocation(x, y))
                    return TRUE;
            }
        }
    }

    return FALSE;
}

// MovementAction_EmoteFollowMonSpawn
bool8 MovementAction_FollowMonGrassSpawn(struct ObjectEvent *objEvent);
bool8 MovementAction_FollowMonWaterSpawn(struct ObjectEvent *objEvent);
bool8 MovementAction_FollowMonCaveSpawn(struct ObjectEvent *objEvent);
bool8 MovementAction_FollowMonShinySpawn(struct ObjectEvent *objEvent);

static u8 FindObjectEventForGfx(u16 gfxId)
{
    u8 i;
    for(i = 0; i < OBJECT_EVENTS_COUNT; ++i)
    {
        if(gObjectEvents[i].active && gObjectEvents[i].graphicsId == gfxId)
        {
            return i;
        }
    }

    return OBJECT_EVENTS_COUNT;
}

static bool8 InShinyAnimationRange(u8 objectEventId)
{
    if(Rogue_AreWildMonEnabled())
    {
        // Wild spawns instantly play animation
        return TRUE;
    }
    else
    {
        // Static spawns only play anim when you're close enough to see it
        s16 x, y;
        PlayerGetDestCoords(&x, &y);

        x = gObjectEvents[objectEventId].currentCoords.x - x;
        y = gObjectEvents[objectEventId].currentCoords.y - y;

        return abs(x) <= 4 && abs(y) <= 4;
    }
}

void FollowMon_OverworldCB()
{
    if(Rogue_AreWildMonEnabled())
    {
        // Speed up spawning
        if(sFollowMonData.activeCount <= 1)
        {
            // Super fast spawn for new things on screen
            sFollowMonData.spawnCountdown = min(sFollowMonData.spawnCountdown, 15);
        }
        else if(sFollowMonData.activeCount <= (MAX_SPAWN_SLOTS - 1))
        {
            // Fast spawn to reach capacity
            sFollowMonData.spawnCountdown = min(sFollowMonData.spawnCountdown, 60);
        }

        if(sFollowMonData.spawnCountdown == 0)
        {
            s16 x, y;

            if(IsSafeToSpawnObjectEvents() && TrySelectTile(&x, &y))
            {
                u16 spawnSlot = NextSpawnMonSlot();

                if(spawnSlot != 0xF)
                {
                    u16 spawnRate;
                    bool8 isShiny = (VarGet(VAR_FOLLOW_MON_0 + spawnSlot) >= FOLLOWMON_SHINY_OFFSET);
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

                    // Slower replacement spawning
                    sFollowMonData.spawnCountdown = 60 * (3 + Random() % 2);
                }
            }
        }
        else
        {
            --sFollowMonData.spawnCountdown;
        }
    }

    // Play spawn animation when player is close enough
    if(sFollowMonData.pendingSpawnAnim != 0)
    {
        u16 spawnSlot;
        u16 gfxId;
        u16 bitFlag;
        bool8 isShiny;
        u8 objectEventId;

        for(gfxId = OBJ_EVENT_GFX_FOLLOW_MON_0; gfxId < OBJ_EVENT_GFX_FOLLOW_MON_LAST; ++gfxId)
        {
            spawnSlot = gfxId - OBJ_EVENT_GFX_FOLLOW_MON_0;
            bitFlag = (1 << spawnSlot);

            if((sFollowMonData.pendingSpawnAnim & bitFlag) != 0)
            {
                objectEventId = FindObjectEventForGfx(gfxId);

                if(objectEventId != OBJECT_EVENTS_COUNT)
                {
                    isShiny = (VarGet(VAR_FOLLOW_MON_0 + spawnSlot) >= FOLLOWMON_SHINY_OFFSET);

                    if(isShiny)
                    {
                        // Play special shiny animation only once in range
                        if(InShinyAnimationRange(objectEventId))
                        {
                            MovementAction_FollowMonShinySpawn(&gObjectEvents[objectEventId]);
                            PlaySE(SE_SHINY);

                            sFollowMonData.pendingSpawnAnim &= ~bitFlag;
                        }
                    }
                    else 
                    {
                        // Instantly play a small animation to ground the spawning a bit
                        if(gPlayerAvatar.flags & (PLAYER_AVATAR_FLAG_SURFING | PLAYER_AVATAR_FLAG_UNDERWATER))
                        {
                            MovementAction_FollowMonWaterSpawn(&gObjectEvents[objectEventId]);
                        }
                        else if(gMapHeader.cave || gMapHeader.mapType == MAP_TYPE_UNDERGROUND)
                        {
                            MovementAction_FollowMonCaveSpawn(&gObjectEvents[objectEventId]);
                        }
                        else
                        {
                            MovementAction_FollowMonGrassSpawn(&gObjectEvents[objectEventId]);
                        }

                        sFollowMonData.pendingSpawnAnim &= ~bitFlag;
                    }
                }
            }
        }
    }
}

bool8 IsSafeToSpawnObjectEvents()
{
    struct ObjectEvent* player = &gObjectEvents[gPlayerAvatar.objectEventId];

    // Only spawn when player is at a valid tile position
    return (player->currentCoords.x == player->previousCoords.x && player->currentCoords.y == player->previousCoords.y);
}

void FollowMon_OnWarp()
{
    sFollowMonData.spawnCountdown = 0;
    sFollowMonData.activeCount = 0;
}

void FollowMon_OnObjectEventSpawned(struct ObjectEvent *objectEvent)
{
    u16 spawnSlot = objectEvent->graphicsId - OBJ_EVENT_GFX_FOLLOW_MON_0;

    if(sFollowMonData.activeCount != 255)
        ++sFollowMonData.activeCount;

    sFollowMonData.pendingSpawnAnim |= (1 << spawnSlot);
}

void FollowMon_OnObjectEventRemoved(struct ObjectEvent *objectEvent)
{
    if(sFollowMonData.activeCount != 0)
        --sFollowMonData.activeCount;
}