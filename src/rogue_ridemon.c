#include "global.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/layouts.h"
#include "constants/songs.h"

#include "bike.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "fieldmap.h"
#include "field_effect_helpers.h"
#include "field_player_avatar.h"
#include "follow_me.h"
#include "metatile_behavior.h"
#include "sound.h"

#include "rogue_baked.h"
#include "rogue_controller.h"
#include "rogue_debug.h"
#include "rogue_followmon.h"
#include "rogue_multiplayer.h"
#include "rogue_popup.h"
#include "rogue_ridemon.h"

enum
{
    RIDE_SPRITE_DIR_UP,
    RIDE_SPRITE_DIR_DOWN,
    RIDE_SPRITE_DIR_SIDE,
    RIDE_SPRITE_DIR_COUNT
};

enum
{
    RIDER_SHOW_BEHIND,
    RIDER_SHOW_INFRONT,
};

enum
{
    RIDE_MOVEMENT_SLOW,
    RIDE_MOVEMENT_ACCELERATE_AVERAGE,
    RIDE_MOVEMENT_AVERAGE,
    RIDE_MOVEMENT_ACCELERATE_FAST,
    RIDE_MOVEMENT_FAST,
};

#define RIDE_MON_FLAG_NONE          (0)
#define RIDE_MON_FLAG_CAN_RIDE      (1 << 0)
#define RIDE_MON_FLAG_CAN_SWIM      (1 << 1)
#define RIDE_MON_FLAG_CAN_CLIMB     (1 << 2)
#define RIDE_MON_FLAG_CAN_FLY       (1 << 3)

#define RIDE_FLY_HEIGHT 12   // 16

#define RIDE_OBJECT_PLAYER              0   // Reserved for the player
#define RIDE_OBJECT_COUNT               (1 + NET_PLAYER_CAPACITY)


struct RideMonSpriteInfo
{
    // Y has larger ranges than X
    s8 playerY;
    s8 monY;
    s8 playerX;
    s8 monX : 7;
    u8 playerRendersInFront : 1;
};

struct RideMonInfo
{
    struct RideMonSpriteInfo spriteInfo[RIDE_SPRITE_DIR_COUNT];
    u8 flags;
};

#include "data/rogue_ridemon_infos.h"

struct RideObjectEvent
{
    struct RogueRideMonState state;
    u8 riderObjectEventId;
    u8 riderSpriteId;
    u8 monSpriteId;
    u8 isActive : 1;
};

struct RideMonData
{
    struct RideObjectEvent rideObjects[RIDE_OBJECT_COUNT];
    u8 rideFrameCounter;
    u8 recentRideIndex;
};

EWRAM_DATA struct RideMonData sRideMonData = {0};

void ResetRideObject(struct RideObjectEvent* rideObject)
{
    rideObject->isActive = FALSE;
    rideObject->riderObjectEventId = 0;
    rideObject->riderSpriteId = SPRITE_NONE;
    rideObject->monSpriteId = SPRITE_NONE;

    rideObject->state.desiredRideSpecies = SPECIES_NONE;
    rideObject->state.flyingState = 0;
    rideObject->state.flyingHeight = 0;
    rideObject->state.whistleType = RIDE_WHISTLE_BASIC;
    rideObject->state.monGfx = SPECIES_NONE;
}

void Rogue_RideMonInit()
{
    u8 i;

    sRideMonData.rideFrameCounter = 0;
    sRideMonData.recentRideIndex = 0;
    
    for(i = 0; i < RIDE_OBJECT_COUNT; ++i)
        ResetRideObject(&sRideMonData.rideObjects[i]);
}

struct RogueRideMonState* Rogue_GetPlayerRideMonStatePtr()
{
    return &sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state;
}

static void UpdateRideMonSprites(u8 rideObjectId, struct RideObjectEvent* rideObject);
static void UpdateRideSpriteInternal(struct RideObjectEvent* rideObject, const struct RideMonInfo* rideInfo);
static bool8 AdjustFlyingAnimation(struct RideObjectEvent* rideObject);

static u16 GetCurrentRideMonSpecies();
static const struct RideMonInfo* GetRideMonInfoForSpecies(u16 species);

static bool8 IsValidSpeciesToRideNow(u16 species)
{
    if(species >= FOLLOWMON_SHINY_OFFSET)
        species -= FOLLOWMON_SHINY_OFFSET;

    if(species != SPECIES_NONE)
    {
        const struct RideMonInfo* rideInfo = GetRideMonInfoForSpecies(species);

        if(rideInfo && (rideInfo->flags & RIDE_MON_FLAG_CAN_RIDE) != 0)
            return TRUE;
    }

    return FALSE;
}

static bool8 IsValidMonToRideNow(struct Pokemon* mon)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES);
    return IsValidSpeciesToRideNow(species);
}

static u8 GetRideOptionCountFor(u8 whistleType)
{
    u8 count;

    if(whistleType == RIDE_WHISTLE_GOLD)
    {
        count = 1 + (FlagGet(FLAG_SYS_RIDING_ACCESS_DAYCARE) ? Rogue_GetCurrentDaycareSlotCount() : 0);
    }
    else // RIDE_WHISTLE_BASIC
    {
        count = gPlayerPartyCount;
    }

    return count;
}

static u8 GetRideOptionCount()
{
    return GetRideOptionCountFor(sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.whistleType);
}

static u16 GetRideOptionGfxFor(u8 whistleType, u8 slot)
{
    if(whistleType == RIDE_WHISTLE_GOLD)
    {
        if(slot == 0)
        {
            return VarGet(VAR_ROGUE_REGISTERED_RIDE_MON);
        }
        else
        {
            struct BoxPokemon* mon = Rogue_GetDaycareBoxMon(slot - 1);
            return FollowMon_GetBoxMonGraphics(mon);
        }
    }
    else // RIDE_WHISTLE_BASIC
    {
        if(IsValidMonToRideNow(&gPlayerParty[slot]))
        {
            return FollowMon_GetMonGraphics(&gPlayerParty[slot]);
        }
    }

    return SPECIES_NONE;
}

static u16 GetRideOptionGfx(u8 slot)
{
    return GetRideOptionGfxFor(sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.whistleType, slot);
}

static bool8 CalculateRideSpecies(s8 dir)
{
    u8 counter;
    s8 monIdx;
    u16 rideOptionGfx;
    u8 rideOptionCount = GetRideOptionCount();

    // Loop through mons from last riden
    sRideMonData.recentRideIndex = min(sRideMonData.recentRideIndex, rideOptionCount - 1);
    sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.desiredRideSpecies = SPECIES_NONE;

    for(counter = 0; counter < rideOptionCount; ++counter)
    {
        monIdx = sRideMonData.recentRideIndex;

        if(dir != 0)
            monIdx += (1 + counter) * dir;
        else
            monIdx = counter;

        while(monIdx < 0)
            monIdx += rideOptionCount;
        
        monIdx %= rideOptionCount;
        rideOptionGfx = GetRideOptionGfx(monIdx);

        if(IsValidSpeciesToRideNow(rideOptionGfx))
        {
            sRideMonData.recentRideIndex = monIdx;
            sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.desiredRideSpecies = rideOptionGfx;
            return TRUE;
        }
    }

    return FALSE;
}

static bool8 CalculateInitialRideSpecies()
{
    u8 counter;
    u8 monIdx;
    u16 rideOptionGfx;
    u8 rideOptionCount = GetRideOptionCount();

    sRideMonData.recentRideIndex = min(sRideMonData.recentRideIndex, rideOptionCount - 1);

    // Try to ride the same species we were previously riding
    for(counter = 0; counter < rideOptionCount; ++counter)
    {
        monIdx = (sRideMonData.recentRideIndex + counter) % rideOptionCount;
        rideOptionGfx = GetRideOptionGfx(monIdx);

        if(IsValidSpeciesToRideNow(rideOptionGfx))
        {
            sRideMonData.recentRideIndex = monIdx;
            sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.desiredRideSpecies = rideOptionGfx;
            return TRUE;
        }
    }

    // Can't ride the mon we were previously riding, so try to pick next avaliable
    return CalculateRideSpecies(0);
}

static void PlayRideMonCry()
{
    if(sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.desiredRideSpecies != sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.monGfx)
    {
        u16 species = sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.desiredRideSpecies;

        if(species >= FOLLOWMON_SHINY_OFFSET)
            species -= FOLLOWMON_SHINY_OFFSET;

        if(IsCryPlaying())
            StopCry();
        PlayCry_Normal(species, 0);
    }
}

// Based on GetOnOffBike
void Rogue_GetOnOffRideMon(u8 whistleType, bool8 forWarp)
{
    FollowMon_ClearCachedPartnerSpecies();

    if(!forWarp)
    {
        if(Rogue_IsRideMonFlying() || Rogue_IsRideMonSwimming())
        {
            // We're not allowed to dismount here
            PlaySE(SE_WALL_HIT);
            return;
        }
    }

    if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_RIDING))
    {
        SetPlayerAvatarTransitionFlags(PLAYER_AVATAR_FLAG_ON_FOOT);
    }
    else
    {
        sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.whistleType = whistleType;

        if(CalculateInitialRideSpecies())
        {
            SetPlayerAvatarTransitionFlags(PLAYER_AVATAR_FLAG_RIDING);
            PlayRideMonCry();
        }
        else
        {
            PlaySE(SE_FAILURE);
        }
    }

    // Delete existing sprite
    //if(sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].monSpriteId != SPRITE_NONE)
    //{
    //    DestroySprite(&gSprites[sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].monSpriteId]);
    //    sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].monSpriteId = SPRITE_NONE;
    //}

    if(!forWarp)
        SetupFollowParterMonObjectEvent();
}

static bool8 CanCycleRideMons()
{
    return !Rogue_IsRideMonFlying() && !Rogue_IsRideMonSwimming();
}

bool8 Rogue_HandleRideMonInput()
{
    if(Rogue_IsRideActive())
    {
        // Cycle through mons, when pressing L or R
        if(sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.whistleType == RIDE_WHISTLE_BASIC || (FlagGet(FLAG_SYS_RIDING_ACCESS_DAYCARE) && sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.whistleType == RIDE_WHISTLE_GOLD))
        {
            if(JOY_NEW(L_BUTTON))
            {
                if(CanCycleRideMons())
                {
                    CalculateRideSpecies(-1);
                    PlayRideMonCry();
                }
                else
                {
                    PlaySE(SE_FAILURE);
                }
            }
            else if(JOY_NEW(R_BUTTON))
            {
                if(CanCycleRideMons())
                {
                    CalculateRideSpecies(1);
                    PlayRideMonCry();
                }
                else
                {
                    PlaySE(SE_FAILURE);
                }
            }
        }
    }

    // Never block input from here
    return FALSE;
}

static bool8 ShouldRideMonBeVisible()
{
    return Rogue_IsRideActive() && !gObjectEvents[gPlayerAvatar.objectEventId].invisible;
}

static void UpdatePlayerRideState()
{
    bool8 wasActive = sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].isActive;

    // Update player ride state now
    sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].riderObjectEventId = gPlayerAvatar.objectEventId;
    sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].riderSpriteId = gPlayerAvatar.spriteId;
    sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].isActive = ShouldRideMonBeVisible();

    if(sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].isActive)
    {
        AGB_ASSERT(sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.desiredRideSpecies != SPECIES_NONE);
    }
    else
    {
        //sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.desiredRideSpecies = SPECIES_NONE;
        sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.monGfx = SPECIES_NONE;

        if(wasActive)
        {
            // Final update to ensure this sprite is destroyed
            UpdateRideMonSprites(RIDE_OBJECT_PLAYER, &sRideMonData.rideObjects[RIDE_OBJECT_PLAYER]);
        }
    }
}

void Rogue_UpdateRideMons()
{
    u8 i;

    // Remove other object ride mons (Player is handled separately to this)
    for(i = 1; i < RIDE_OBJECT_COUNT; ++i)
    {
        if(!sRideMonData.rideObjects[i].isActive && sRideMonData.rideObjects[i].riderObjectEventId != OBJECT_EVENTS_COUNT)
        {
            UpdateRideMonSprites(i, &sRideMonData.rideObjects[i]);
            sRideMonData.rideObjects[i].riderObjectEventId = OBJECT_EVENTS_COUNT;
        }
    }
}

void Rogue_HandleRideMonMovementIfNeeded(u8 objectEventId)
{
    u8 i;

    if(gPlayerAvatar.objectEventId == objectEventId)
    {
        START_TIMER(ROGUE_RIDEMON_UPDATE);
        UpdatePlayerRideState();
        Rogue_UpdateRideMons(); // do global update based on player
        STOP_TIMER(ROGUE_RIDEMON_UPDATE);
    }

    for(i = 0; i < RIDE_OBJECT_COUNT; ++i)
    {
        if(sRideMonData.rideObjects[i].isActive && sRideMonData.rideObjects[i].riderObjectEventId == objectEventId)
        {
            UpdateRideMonSprites(i, &sRideMonData.rideObjects[i]);
            break;
        }
    }
}

bool8 Rogue_IsActiveRideMonObject(u8 objectEventId)
{
    u8 i;

    if(gPlayerAvatar.objectEventId == objectEventId)
    {
        if(sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].isActive)
            return TRUE;
    }

    for(i = 1; i < RIDE_OBJECT_COUNT; ++i)
    {
        if(sRideMonData.rideObjects[i].isActive && sRideMonData.rideObjects[i].riderObjectEventId == objectEventId)
            return TRUE;
    }

    return FALSE;
}

void Rogue_SetupRideObject(u8 rideObjectId, u8 objectEventId, u16 rideSpecies, bool8 isFlying)
{
    AGB_ASSERT(rideObjectId < RIDE_OBJECT_COUNT);
    sRideMonData.rideObjects[rideObjectId].isActive = TRUE;
    sRideMonData.rideObjects[rideObjectId].riderSpriteId = gObjectEvents[objectEventId].spriteId;
    sRideMonData.rideObjects[rideObjectId].riderObjectEventId = objectEventId;
    sRideMonData.rideObjects[rideObjectId].state.flyingState = isFlying;
    sRideMonData.rideObjects[rideObjectId].state.desiredRideSpecies = rideSpecies;
}

void Rogue_ClearRideObject(u8 rideObjectId)
{
    AGB_ASSERT(rideObjectId < RIDE_OBJECT_COUNT);
    sRideMonData.rideObjects[rideObjectId].isActive = FALSE;
    sRideMonData.rideObjects[rideObjectId].riderSpriteId = SPRITE_NONE;
    sRideMonData.rideObjects[rideObjectId].state.desiredRideSpecies = SPECIES_NONE;

    // Don't clear here, clear above when sprite is removed
    //sRideMonData.rideObjects[rideObjectId].riderObjectEventId = OBJECT_EVENTS_COUNT;
}

void Rogue_OnResetRideMonSprites()
{
    u8 i;

    for(i = 0; i < RIDE_OBJECT_COUNT; ++i)
    {
        sRideMonData.rideObjects[i].riderSpriteId = SPRITE_NONE;
        sRideMonData.rideObjects[i].monSpriteId = SPRITE_NONE;
    }
}

u16 Rogue_GetRideMonSpeciesGfx(u8 rideObject)
{
    AGB_ASSERT(rideObject < RIDE_OBJECT_COUNT);
    return sRideMonData.rideObjects[rideObject].state.monGfx;
}

static u8 CalculateMovementModeForInternal(u16 species);

static u16 ToRideSpecies(u16 species)
{
#ifdef ROGUE_EXPANSION
    // If we don't have valid ride flag then check the base mon ride info
    // this handles stuff like arceus and other forms which are very similar
    if(sRideMonInfo[species].flags & RIDE_MON_FLAG_CAN_RIDE)
    {
        return species;
    }

    return GET_BASE_SPECIES_ID(species);
#else
    return species;
#endif
}

static const struct RideMonInfo* GetRideMonInfoForSpecies(u16 species)
{
    const struct RideMonInfo* rideInfo = &sRideMonInfo[ToRideSpecies(species)];

#ifdef ROGUE_DEBUG
    //if(sTestData.useDebugInfo)
    //{
    //    if(sTestData.debugSetter)
    //    {
    //        sTestData.debugSetter = FALSE;
    //        sTestData.debugMoveSpeed = CalculateMovementModeForInternal(species);
    //        memcpy(&sTestData.debugInfo, rideInfo, sizeof(struct RideMonInfo));
    //    }
    //    rideInfo = &sTestData.debugInfo;
    //}
#endif

    return rideInfo;
}

bool8 Rogue_IsValidRideSpecies(u16 species)
{
    const struct RideMonInfo* rideInfo = GetRideMonInfoForSpecies(species);

     if(rideInfo != NULL && (rideInfo->flags & RIDE_MON_FLAG_CAN_RIDE) != 0)
    {
        return TRUE;
    }

    return FALSE;
}

bool8 Rogue_IsValidRideClimbSpecies(u16 species)
{
    const struct RideMonInfo* rideInfo = GetRideMonInfoForSpecies(species);

     if(rideInfo != NULL && Rogue_IsValidRideSpecies(species) && (rideInfo->flags & RIDE_MON_FLAG_CAN_CLIMB) != 0)
    {
        return TRUE;
    }

    return FALSE;
}

bool8 Rogue_IsValidRideSwimSpecies(u16 species)
{
    const struct RideMonInfo* rideInfo = GetRideMonInfoForSpecies(species);

     if(rideInfo != NULL && Rogue_IsValidRideSpecies(species) && (rideInfo->flags & RIDE_MON_FLAG_CAN_SWIM) != 0)
    {
        return TRUE;
    }

    return FALSE;
}

bool8 Rogue_IsValidRideFlySpecies(u16 species)
{
    const struct RideMonInfo* rideInfo = GetRideMonInfoForSpecies(species);

     if(rideInfo != NULL && Rogue_IsValidRideSpecies(species) && (rideInfo->flags & RIDE_MON_FLAG_CAN_FLY) != 0)
    {
        return TRUE;
    }

    return FALSE;
}

static u8 UNUSED CalculateMovementModeForInternal(u16 species)
{
    u8 speed = gRogueSpeciesInfo[species].baseSpeed;
    
    if(speed <= 70)
        return RIDE_MOVEMENT_SLOW;

    if(speed <= 90)
        return RIDE_MOVEMENT_ACCELERATE_AVERAGE;
    
    if(speed <= 100)
        return RIDE_MOVEMENT_AVERAGE;

    if(speed <= 110)
        return RIDE_MOVEMENT_ACCELERATE_FAST;

    return RIDE_MOVEMENT_FAST;
}

static u8 CalculateMovementModeFor(u16 species)
{
    u8 moveSpeed = CalculateMovementModeForInternal(species);
    
#ifdef ROGUE_DEBUG
    //if(sTestData.useDebugInfo)
    //{
    //    return sTestData.debugMoveSpeed;
    //}
#endif

    return moveSpeed;
}

u8 Rogue_GetRideSpeciesSpeedStars(u16 species)
{
    return CalculateMovementModeFor(species) + 1;
}

static u16 GetCurrentRideMonSpecies()
{
    if(sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.monGfx >= FOLLOWMON_SHINY_OFFSET)
        return sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.monGfx - FOLLOWMON_SHINY_OFFSET;

    return sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.monGfx;
}

static const struct RideMonInfo* GetCurrentRideMonInfo()
{
    return GetRideMonInfoForSpecies(GetCurrentRideMonSpecies());
}

static bool8 IsRideObjectFlying(struct RideObjectEvent* rideObject)
{
    return rideObject->state.flyingState || rideObject->state.flyingHeight != 0;
}

static const struct RideMonInfo* GetRideObjectRideMonInfo(struct RideObjectEvent* rideObject)
{
    u16 monGfx = rideObject->state.monGfx;

    if(monGfx >= FOLLOWMON_SHINY_OFFSET)
        monGfx -= FOLLOWMON_SHINY_OFFSET;

    return GetRideMonInfoForSpecies(monGfx);
}

static void UpdateRideMonSprites(u8 rideObjectId, struct RideObjectEvent* rideObject)
{
    if(rideObject->isActive)
    {
        AGB_ASSERT(rideObject->state.desiredRideSpecies != SPECIES_NONE);

        if(rideObjectId != RIDE_OBJECT_PLAYER)
        {
            // Non-player riders have their reflection hidden always
            gObjectEvents[rideObject->riderObjectEventId].hideReflection = TRUE;
        }

        if(rideObject->monSpriteId != SPRITE_NONE && rideObject->state.monGfx != rideObject->state.desiredRideSpecies)
        {
            // Species changed so dealloc sprite ready to make new sprite
            DestroySprite(&gSprites[rideObject->monSpriteId]);
            rideObject->monSpriteId = SPRITE_NONE;
        }

        // Alloc sprite
        if(rideObject->monSpriteId == SPRITE_NONE)
        {
            s16 spriteX = gObjectEvents[rideObject->riderObjectEventId].currentCoords.x;
            s16 spriteY = gObjectEvents[rideObject->riderObjectEventId].currentCoords.y;

            rideObject->state.monGfx = rideObject->state.desiredRideSpecies;

            if(rideObjectId == RIDE_OBJECT_PLAYER)
                FollowMon_ClearCachedPartnerSpecies();

            if(rideObjectId)
            {
                rideObject->monSpriteId = CreateObjectGraphicsSpriteInObjectEventSpace(OBJ_EVENT_GFX_FOLLOW_MON_PARTNER, SpriteCallbackDummy, spriteX, spriteY, 0);
                gSprites[rideObject->monSpriteId].disableAnimOffsets = TRUE;
            }
            else
            {
                u16 gfxId = rideObjectId - 1;
                u16 species = rideObject->state.monGfx;
                bool8 isShiny = FALSE;

                if(species >= FOLLOWMON_SHINY_OFFSET)
                {
                    species -= FOLLOWMON_SHINY_OFFSET;
                    isShiny = TRUE;
                }
                
                FollowMon_SetGraphics(gfxId, species, isShiny);
                rideObject->monSpriteId = CreateObjectGraphicsSpriteInObjectEventSpace(OBJ_EVENT_GFX_FOLLOW_MON_0 + gfxId, SpriteCallbackDummy, spriteX, spriteY, 0);
                gSprites[rideObject->monSpriteId].disableAnimOffsets = TRUE;
            }

            gSprites[rideObject->monSpriteId].oam.priority = 2;
            StartSpriteAnim(&gSprites[rideObject->monSpriteId], ANIM_STD_GO_SOUTH);
            
            // Handle returning to the screen after flying
            if(IsRideObjectFlying(rideObject))
            {
                SetShadowFieldEffectVisible(&gObjectEvents[rideObject->riderObjectEventId], TRUE);
                gObjectEvents[rideObject->riderObjectEventId].hideReflection = TRUE;
            }

            // If we're attempting to ride a mon, but for whatever reason we no longer can (e.g. released mon we were riding) unmount here
            //if(CalculateInitialRideSpecies())
            //{
            //    s16 playerX, playerY;
            //    PlayerGetDestCoords(&playerX, &playerY);
//
            //    rideObject->monSpriteId = CreateObjectGraphicsSpriteInObjectEventSpace(OBJ_EVENT_GFX_FOLLOW_MON_PARTNER, SpriteCallbackDummy, playerX, playerY, 0);
            //    gSprites[rideObject->monSpriteId].oam.priority = 2;
            //    StartSpriteAnim(&gSprites[rideObject->playerObject.monSpriteId], ANIM_STD_GO_SOUTH);
            //    
            //    // Handle returning to the screen after flying
            //    if(Rogue_IsRideMonFlying())
            //    {
            //        SetShadowFieldEffectVisible(&gObjectEvents[gPlayerAvatar.objectEventId], TRUE);
            //        gObjectEvents[gPlayerAvatar.objectEventId].hideReflection = TRUE;
            //    }
            //}
            //else
            //{
            //    // Force demount here
            //    Rogue_GetOnOffRideMon(sRideMonData.playerRideState.whistleType, FALSE);
            //}
        }

        if(rideObject->monSpriteId != SPRITE_NONE)
        {
            const struct RideMonInfo* rideInfo = GetRideObjectRideMonInfo(rideObject);

            if(rideInfo != NULL)
            {
                // Execute fly animation here, for non-player 
                // (Player runs code below in movement actions)
                if(rideObjectId != RIDE_OBJECT_PLAYER)
                    AdjustFlyingAnimation(rideObject);

                UpdateRideSpriteInternal(rideObject, rideInfo);
            }
        }
    }
    else
    {
        // Dealloc sprite
        if(rideObject->monSpriteId != SPRITE_NONE)
        {
            DestroySprite(&gSprites[rideObject->monSpriteId]);

            if(rideObjectId != RIDE_OBJECT_PLAYER)
            {
                // Restore non-player riders reflections
                gObjectEvents[rideObject->riderObjectEventId].hideReflection = FALSE;
            }

            rideObject->monSpriteId = SPRITE_NONE;
            rideObject->riderObjectEventId = OBJECT_EVENTS_COUNT;
        }
    }
}

void Rogue_OnRideMonWarp()
{
    // dismount when warping
    // Rogue_GetOnOffRideMon(TRUE);

    if(Rogue_IsRideMonFlying())
    {
        // Instantly snap to ground
        sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.flyingState = FALSE;
        sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.flyingHeight = 0;
        AdjustFlyingAnimation(&sRideMonData.rideObjects[RIDE_OBJECT_PLAYER]);
    }
}

u8 Rogue_GetRideMonSprite(struct ObjectEvent* objectEvent)
{
    if(objectEvent->localId == OBJ_EVENT_ID_PLAYER)
    {
        return sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].monSpriteId;
    }

    return SPRITE_NONE;
}

bool8 Rogue_CanRideMonInvJumpLedge()
{
    if(FlagGet(FLAG_SYS_RIDING_LEDGE_JUMP))
    {
        const struct RideMonInfo* rideInfo = GetCurrentRideMonInfo();

        if(rideInfo != NULL && (rideInfo->flags & RIDE_MON_FLAG_CAN_CLIMB) != 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}

bool8 Rogue_CanRideMonSwim()
{
    if(FlagGet(FLAG_SYS_RIDING_SURF))
    {
        const struct RideMonInfo* rideInfo = GetCurrentRideMonInfo();

        if(rideInfo != NULL && (rideInfo->flags & RIDE_MON_FLAG_CAN_SWIM) != 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}

bool8 Rogue_CanRideMonFly()
{
    if(FlagGet(FLAG_SYS_RIDING_FLY))
    {
        const struct RideMonInfo* rideInfo = GetCurrentRideMonInfo();

        if(rideInfo != NULL && (rideInfo->flags & RIDE_MON_FLAG_CAN_FLY) != 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}

static bool8 IsUsableSwimOrFlySpecies(u16 species)
{
    if(FlagGet(FLAG_SYS_RIDING_SURF) && Rogue_IsValidRideSwimSpecies(species))
        return TRUE;
        
    if(FlagGet(FLAG_SYS_RIDING_FLY) && Rogue_IsValidRideFlySpecies(species))
    {
        if(Rogue_GetRemainingFlightCharges() != 0)
            return TRUE;
    }

    return FALSE;
}

bool8 Rogue_HasUsableSwimOrFlyCharge()
{
    // Check Party first
    u32 count = GetRideOptionCountFor(RIDE_WHISTLE_BASIC);
    u32 i;

    for(i = 0; i < count; ++i)
    {
        u16 species = GetRideOptionGfxFor(RIDE_WHISTLE_BASIC, i);

        if(species >= FOLLOWMON_SHINY_OFFSET)
            species -= FOLLOWMON_SHINY_OFFSET;

        if(species != SPECIES_NONE && IsUsableSwimOrFlySpecies(species))
            return TRUE;
    }

    // TODO - Only run if we have this in our bag?
    // (We should always have this in our bag anyway and we can't fill any of the slots until we get it anyway)
    {
        count = GetRideOptionCountFor(RIDE_WHISTLE_GOLD);

        for(i = 0; i < count; ++i)
        {
            u16 species = GetRideOptionGfxFor(RIDE_WHISTLE_GOLD, i);

            if(species >= FOLLOWMON_SHINY_OFFSET)
                species -= FOLLOWMON_SHINY_OFFSET;

            if(species != SPECIES_NONE && IsUsableSwimOrFlySpecies(species))
                return TRUE;
        }
    }

    return FALSE;
}

#define IN_AREA(x0, y0, x1, y1) (playerX >= x0 && playerX <= x1 && playerY >= y0 && playerY <= y1)

static bool8 IsInSoftlockZone()
{
    s16 playerX, playerY;
    PlayerGetDestCoords(&playerX, &playerY);
    playerX -= MAP_OFFSET;
    playerY -= MAP_OFFSET;

    switch (gMapHeader.mapLayoutId)
    {
    case LAYOUT_ROGUE_ROUTE_MOUNTAIN0:
        if(IN_AREA(3, 113, 7, 119))
            return TRUE;
        if(IN_AREA(7, 119, 13, 120))
            return TRUE;
        break;

    case LAYOUT_ROGUE_ROUTE_MOUNTAIN1:
        if(IN_AREA(12, 25, 15, 28))
            return TRUE;
        break;

    case LAYOUT_ROGUE_ROUTE_WATER_FRONT1:
        if(IN_AREA(24, 63, 30, 64))
            return TRUE;
        if(IN_AREA(0, 86, 9, 89))
            return TRUE;
        break;

    case LAYOUT_ROGUE_ROUTE_URBAN0:
        if(IN_AREA(32, 46, 34, 48))
            return TRUE;
        if(IN_AREA(22, 23, 24, 26))
            return TRUE;
        break;


    case LAYOUT_ROGUE_ROUTE_JOHTO_FIELD1:
        if(IN_AREA(20, 101, 29, 101))
            return TRUE;
        break;

    case LAYOUT_ROGUE_ROUTE_JOHTO_WATER_FRONT0:
        if(IN_AREA(20, 16, 25, 22))
            return TRUE;
        break;

    case LAYOUT_ROGUE_ROUTE_SINNOH_212:
        if(IN_AREA(27, 21, 29, 24))
            return TRUE;
        break;

    case LAYOUT_ROGUE_ROUTE_SINNOH_214:
        if(IN_AREA(25, 4, 27, 5))
            return TRUE;
        break;
    }

    return FALSE;
}

bool8 Rogue_ShouldRunRidemonTrappedScript()
{
    if(FlagGet(FLAG_SYS_RIDING_SURF) || FlagGet(FLAG_SYS_RIDING_FLY))
    {
        if(Rogue_IsRideMonSwimming() || Rogue_IsRideMonFlying())
            return FALSE;

        // If we're in softlockable locations, we need to warp back to safety
        if(IsInSoftlockZone() && !Rogue_HasUsableSwimOrFlyCharge())
            return TRUE;
    }

    return FALSE;
}

u32 Rogue_GetRemainingFlightCharges()
{
    if(Rogue_IsRunActive())
        return VarGet(VAR_ROGUE_REMAINING_FLIGHT_CHARGES);
    else
        return 999;
}

u32 Rogue_GetMaxFlightCharges()
{
    if(FlagGet(FLAG_SYS_RIDING_FLY_INFINITE_CHARGES))
        return 999;
    else if(FlagGet(FLAG_SYS_RIDING_FLY_3_CHARGES))
        return 3;
    else if(FlagGet(FLAG_SYS_RIDING_FLY_2_CHARGES))
        return 2;
    else
        return 1;
}

void Rogue_DecreaseFlightCharges()
{
    if(Rogue_IsRunActive() && !FlagGet(FLAG_SYS_RIDING_FLY_INFINITE_CHARGES))
    {
        u16 charges = VarGet(VAR_ROGUE_REMAINING_FLIGHT_CHARGES);
        if(charges != 0)
        {
            VarSet(VAR_ROGUE_REMAINING_FLIGHT_CHARGES, charges - 1);

            Rogue_PushPopup_FlightChargeUsed(Rogue_GetRemainingFlightCharges(), Rogue_GetMaxFlightCharges());
        }
    }
}

void Rogue_RefillFlightCharges(bool8 createPopup)
{
    if(Rogue_GetRemainingFlightCharges() != Rogue_GetMaxFlightCharges())
    {
        VarSet(VAR_ROGUE_REMAINING_FLIGHT_CHARGES, Rogue_GetMaxFlightCharges());

        if(createPopup)
            Rogue_PushPopup_FlightChargeRefilled(Rogue_GetMaxFlightCharges());
    }
}

bool8 Rogue_IsRideActive()
{
    return TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_RIDING);
}

bool8 Rogue_IsRideMonSwimming()
{
    if (Rogue_IsRideActive())
    {
        s16 x, y;
        u16 tileBehavior;

        if(Rogue_IsRideMonFlying())
            return FALSE;

        PlayerGetDestCoords(&x, &y);
        tileBehavior = MapGridGetMetatileBehaviorAt(x, y);

        if (MetatileBehavior_IsSurfableWaterOrUnderwater(tileBehavior) && !MapGridIsImpassableAt(x, y))
            return TRUE;
        if (MetatileBehavior_IsBridgeOverWaterNoEdge(tileBehavior) == TRUE)
            return TRUE;
    }

    return FALSE;
}

bool8 Rogue_IsRideMonFlying()
{
    if (Rogue_IsRideActive())
    {
        return IsRideObjectFlying(&sRideMonData.rideObjects[RIDE_OBJECT_PLAYER]);
    }

    return FALSE;
}

bool8 Rogue_RideMonIsCollisionExempt(struct ObjectEvent* obstacle, struct ObjectEvent* collider)
{
    struct ObjectEvent* player = &gObjectEvents[gPlayerAvatar.objectEventId];

    if(Rogue_IsRideMonFlying())
    {
        // Player always collides with the rival
        if(collider == player)
        {
            struct ObjectEventTemplate* template = GetBaseTemplateForObjectEvent(obstacle);

            if(template->flagId == FLAG_ROGUE_RIVAL_DISABLED)
                return FALSE;
        }

        // Regular objects will never collide
        return (obstacle == player || collider == player);
    }

    return FALSE;
}

static void UpdateRideSpriteInternal(struct RideObjectEvent* rideObject, const struct RideMonInfo* rideInfo)
{
    s8 xFlip;
    const struct RideMonSpriteInfo* rideSpriteInfo;
    struct Sprite* mountSprite = &gSprites[rideObject->monSpriteId];
    struct Sprite* riderSprite = &gSprites[rideObject->riderSpriteId];
    s16 bobbingAnim = ((mountSprite->animCmdIndex % 2) ? 0 : -1);
    u8 facingDirection = gObjectEvents[rideObject->riderObjectEventId].facingDirection;
    u16 species = rideObject->state.monGfx;
    u8 rideSpeed;

    if(species >= FOLLOWMON_SHINY_OFFSET)
        species -= FOLLOWMON_SHINY_OFFSET;

    rideSpeed = CalculateMovementModeFor(species);


    AGB_ASSERT(rideObject->monSpriteId != SPRITE_NONE);
    AGB_ASSERT(rideObject->riderSpriteId != SPRITE_NONE);

    // Fix stairs directions
    switch (facingDirection)
    {
    case DIR_SOUTHWEST:
    case DIR_NORTHWEST:
        facingDirection = DIR_WEST;
        break;

    case DIR_SOUTHEAST:
    case DIR_NORTHEAST:
        facingDirection = DIR_EAST;
        break;
    }

    switch (facingDirection)
    {
    case DIR_NORTH:
        xFlip = 1;
        rideSpriteInfo = &rideInfo->spriteInfo[RIDE_SPRITE_DIR_UP];
        break;

    case DIR_EAST:
        xFlip = 1;
        rideSpriteInfo = &rideInfo->spriteInfo[RIDE_SPRITE_DIR_SIDE];
        break;

    case DIR_SOUTH:
        xFlip = 1;
        rideSpriteInfo = &rideInfo->spriteInfo[RIDE_SPRITE_DIR_DOWN];
        break;

    //case DIR_WEST:
    default:
        xFlip = -1;
        rideSpriteInfo = &rideInfo->spriteInfo[RIDE_SPRITE_DIR_SIDE];
        break;
    };

    {
        u16 idleAnim = ANIM_STD_FACE_SOUTH;
        u16 movingAnim = ANIM_STD_FACE_SOUTH;

        if(rideSpeed > RIDE_MOVEMENT_ACCELERATE_AVERAGE)
        {
            idleAnim = ANIM_STD_GO_SOUTH;
            movingAnim = ANIM_STD_GO_SOUTH;
        }

        if(rideSpeed == RIDE_MOVEMENT_FAST)
        {
            movingAnim = ANIM_STD_GO_FAST_SOUTH;
        }

        if(rideObject->state.flyingHeight != 0)
        {
            // Increase anims by a phase so it's faster when in the air
            idleAnim += 4;
            movingAnim = idleAnim;
        }

        switch (gObjectEvents[rideObject->riderObjectEventId].movementActionId)
        {
        case MOVEMENT_ACTION_FACE_DOWN:
        case MOVEMENT_ACTION_FACE_UP:
        case MOVEMENT_ACTION_FACE_LEFT:
        case MOVEMENT_ACTION_FACE_RIGHT:
            StartSpriteAnimIfDifferent(mountSprite, idleAnim + facingDirection - DIR_SOUTH);
            //riderSprite->x2 = 0;
            //riderSprite->y2 = 0;
            break;

        case MOVEMENT_ACTION_JUMP_2_DOWN:
        case MOVEMENT_ACTION_JUMP_2_UP:
        case MOVEMENT_ACTION_JUMP_2_LEFT:
        case MOVEMENT_ACTION_JUMP_2_RIGHT:
        case MOVEMENT_ACTION_JUMP_DOWN:
        case MOVEMENT_ACTION_JUMP_UP:
        case MOVEMENT_ACTION_JUMP_LEFT:
        case MOVEMENT_ACTION_JUMP_RIGHT:
            StartSpriteAnimIfDifferent(mountSprite, idleAnim + facingDirection - DIR_SOUTH);
            // Don't reset xy2 for these movement actions
            break;

        case MOVEMENT_ACTION_JUMP_IN_PLACE_DOWN:
        case MOVEMENT_ACTION_JUMP_IN_PLACE_UP:
        case MOVEMENT_ACTION_JUMP_IN_PLACE_LEFT:
        case MOVEMENT_ACTION_JUMP_IN_PLACE_RIGHT:
        case MOVEMENT_ACTION_JUMP_IN_PLACE_DOWN_UP:
        case MOVEMENT_ACTION_JUMP_IN_PLACE_UP_DOWN:
        case MOVEMENT_ACTION_JUMP_IN_PLACE_LEFT_RIGHT:
        case MOVEMENT_ACTION_JUMP_IN_PLACE_RIGHT_LEFT:
            StartSpriteAnimIfDifferent(mountSprite, idleAnim + facingDirection - DIR_SOUTH);
            // Don't reset xy2 for these movement actions
            break;

        default:
            StartSpriteAnimIfDifferent(mountSprite, movingAnim + facingDirection - DIR_SOUTH);
            break;
        }
    }

    // Move mount sprite
    mountSprite->x = riderSprite->x;
    mountSprite->y = riderSprite->y;
    mountSprite->x2 = riderSprite->x2 + rideSpriteInfo->monX * xFlip;
    mountSprite->y2 = riderSprite->y2 + rideSpriteInfo->monY + bobbingAnim;

    mountSprite->subpriority = riderSprite->subpriority;

    mountSprite->oam.x = riderSprite->oam.x;
    mountSprite->oam.y = riderSprite->oam.y;
    mountSprite->oam.priority = riderSprite->oam.priority;

    if(rideSpriteInfo->playerRendersInFront)
        mountSprite->subpriority = riderSprite->subpriority + 1;
    else if(riderSprite->subpriority != 0)
        mountSprite->subpriority = riderSprite->subpriority - 1;

    // Move player
    riderSprite->x2 += rideSpriteInfo->playerX * xFlip;
    riderSprite->y2 += rideSpriteInfo->playerY + bobbingAnim;

    // Offset both for flying anim
    if(rideObject->state.flyingHeight != 0)
    {
        riderSprite->y2 -= rideObject->state.flyingHeight;
        mountSprite->y2 -= rideObject->state.flyingHeight;
    }
}

//
// Movement code
//

static u8 CheckMovementInputOnRideMon(u8);
static void PlayerOnRideMonNotMoving(u8, u16, u16);
static void PlayerOnRideMonTurningInPlace(u8, u16, u16);
static void PlayerOnRideMonMoving(u8, u16, u16);

static bool8 PlayerOnRideMonAdjustFlyingState(u8, u16, u16);

static u8 CheckForPlayerAvatarCollision(u8 direction);
static u8 CheckForPlayerLandingCollision();
static void PlayerOnRideMonCollide(u8);
static void PlayCollisionSoundIfNotFacingWarp(u8 direction);

static void (*const sPlayerOnRideMonFuncs[])(u8, u16, u16) =
{
    [NOT_MOVING]     = PlayerOnRideMonNotMoving,
    [TURN_DIRECTION] = PlayerOnRideMonTurningInPlace,
    [MOVING]         = PlayerOnRideMonMoving,
};

static bool8 (*const sArrowWarpMetatileBehaviorChecks3[])(u8) =  //Duplicate of sArrowWarpMetatileBehaviorChecks
{
    [DIR_SOUTH - 1] = MetatileBehavior_IsSouthArrowWarp,
    [DIR_NORTH - 1] = MetatileBehavior_IsNorthArrowWarp,
    [DIR_WEST - 1]  = MetatileBehavior_IsWestArrowWarp,
    [DIR_EAST - 1]  = MetatileBehavior_IsEastArrowWarp,
};

void MovePlayerOnRideMon(u8 direction, u16 newKeys, u16 heldKeys)
{
    if(PlayerOnRideMonAdjustFlyingState(direction, newKeys, heldKeys))
        return;

    if(Rogue_IsRideMonFlying())
    {
        SetShadowFieldEffectVisible(&gObjectEvents[gPlayerAvatar.objectEventId], TRUE);
        gObjectEvents[gPlayerAvatar.objectEventId].hideReflection = TRUE;
    }

    sPlayerOnRideMonFuncs[CheckMovementInputOnRideMon(direction)](direction, newKeys, heldKeys);
}

s16 RideMonGetPlayerSpeed()
{

    //RIDE_MOVEMENT_SLOW,
    //RIDE_MOVEMENT_AVERAGE,
    //RIDE_MOVEMENT_FAST,
    //RIDE_MOVEMENT_FASTEST,

    //if(gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_DASH)
    //    return PLAYER_SPEED_FASTER;

    return PLAYER_SPEED_FAST;
}

static u8 CheckMovementInputOnRideMon(u8 direction)
{
    if (direction == DIR_NONE)
        return gPlayerAvatar.runningState = NOT_MOVING;
    else if (direction != GetPlayerMovementDirection() && gPlayerAvatar.runningState != MOVING)
        return gPlayerAvatar.runningState = TURN_DIRECTION;
    else
        return gPlayerAvatar.runningState = MOVING;
}

void ForceRunRidemonTrappedCheck();

static void PlayerOnRideMonNotMoving(u8 direction, u16 newKeys, u16 heldKeys)
{
    if(newKeys & B_BUTTON && (Rogue_IsRideMonFlying() || Rogue_CanRideMonFly()))
    {
        // Toggle between flying modes
        bool8 desiredFlyState = !sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.flyingState;

        if(desiredFlyState)
        {
            if(Rogue_GetRemainingFlightCharges() == 0)
            {
                // No charges remaining
                //Rogue_PushPopup_FlightChargeUsed(0, Rogue_GetMaxFlightCharges());
                PlaySE(SE_FAILURE);
                return;
            }

            // We got here, so it means we're allowed to take off
            Rogue_DecreaseFlightCharges();
        }
        else
        {
            if(CheckForPlayerLandingCollision())
            {
                // We're not allowed to land here
                PlaySE(SE_FAILURE);
                return;
            }

            if(Rogue_GetRemainingFlightCharges() == 0)
            {
                // Run the soft lock check
                ForceRunRidemonTrappedCheck();
            }
        }

        sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.flyingState = desiredFlyState;
        PlaySE(sRideMonData.rideObjects[RIDE_OBJECT_PLAYER].state.flyingState ? SE_M_FLY : SE_M_WING_ATTACK);
    }
    else
    {
        sRideMonData.rideFrameCounter = 0;
        PlayerFaceDirection(GetPlayerFacingDirection());
    }
}

static void PlayerOnRideMonTurningInPlace(u8 direction, u16 newKeys, u16 heldKeys)
{
    PlayerTurnInPlace(direction);
}

static void PlayerOnRideMonMoving(u8 direction, u16 newKeys, u16 heldKeys)
{
    u8 collision = CheckForPlayerAvatarCollision(direction);
    const struct RideMonInfo* rideInfo = GetCurrentRideMonInfo();

    if(rideInfo != NULL)
    {
        u8 frameIdx;

        if(collision == COLLISION_START_SWIMMING || collision == COLLISION_STOP_SWIMMING)
        {
            if(Rogue_IsRideMonFlying())
            {
                collision = COLLISION_NONE;
            }
        }

        if (collision)
        {
            sRideMonData.rideFrameCounter = 0;

            if(collision == COLLISION_START_SWIMMING || collision == COLLISION_STOP_SWIMMING)
            {
                PlayerJumpLedgeShort(direction);
            }
            else if (collision == COLLISION_LEDGE_JUMP)
            {
                PlayerJumpLedge(direction);
            }
            else
            {
                u8 adjustedCollision = collision - COLLISION_STOP_SURFING;
                if (adjustedCollision > 3)
                    PlayerOnRideMonCollide(direction);
            }
            
            return;
        }

        // RogueNote
        //MOVE_SPEED_NORMAL, // walking                                     PlayerWalkNormal
        //MOVE_SPEED_FAST_1, // running / surfing / sliding (ice tile)      PlayerWalkFast
        //MOVE_SPEED_FAST_2, // water current / acro bike                   PlayerRideWaterCurrent
        //MOVE_SPEED_FASTER, // mach bike's max speed                       PlayerWalkFaster
        //MOVE_SPEED_FASTEST,                                               ??

        switch (CalculateMovementModeFor(GetCurrentRideMonSpecies()))
        {
        case RIDE_MOVEMENT_SLOW:
            PlayerWalkFast(direction);
            break;

        case RIDE_MOVEMENT_ACCELERATE_AVERAGE:
            frameIdx = sRideMonData.rideFrameCounter / 4;

            if(sRideMonData.rideFrameCounter == 0)
                PlayerWalkNormal(direction);
            else if(frameIdx == 0)
                PlayerWalkFast(direction);
            else
                PlayerRideWaterCurrent(direction);
            break;

        case RIDE_MOVEMENT_AVERAGE:
            PlayerRideWaterCurrent(direction);
            break;

        case RIDE_MOVEMENT_ACCELERATE_FAST:
            frameIdx = (sRideMonData.rideFrameCounter - 1) / 3;

            if(sRideMonData.rideFrameCounter == 0)
                PlayerWalkFast(direction);
            else if(frameIdx == 0)
                PlayerRideWaterCurrent(direction);
            else
                PlayerWalkFaster(direction);
            break;

        case RIDE_MOVEMENT_FAST:
            PlayerWalkFaster(direction);
            break;

        default:
            //AGB_ASSERT(false);
            break;
        }

        if(sRideMonData.rideFrameCounter < 255)
            ++sRideMonData.rideFrameCounter;
    }
}

static bool8 PlayerOnRideMonAdjustFlyingState(u8 direction, u16 newKeys, u16 heldKeys)
{
    return AdjustFlyingAnimation(&sRideMonData.rideObjects[RIDE_OBJECT_PLAYER]);
}

static bool8 AdjustFlyingAnimation(struct RideObjectEvent* rideObject)
{
    AGB_ASSERT(rideObject->riderObjectEventId != OBJECT_EVENTS_COUNT);

    if(rideObject->state.flyingState)
    {
        if(rideObject->state.flyingHeight < RIDE_FLY_HEIGHT)
        {
            if(rideObject->state.flyingHeight == 0)
            {
                SetShadowFieldEffectVisible(&gObjectEvents[rideObject->riderObjectEventId], TRUE);
                gObjectEvents[rideObject->riderObjectEventId].hideReflection = TRUE;
            }

            ++rideObject->state.flyingHeight;
            return TRUE;
        }
    }
    else
    {
        if(rideObject->state.flyingHeight > 0)
        {
            --rideObject->state.flyingHeight;

            if(rideObject->state.flyingHeight == 0)
            {
                SetShadowFieldEffectVisible(&gObjectEvents[rideObject->riderObjectEventId], FALSE);
                gObjectEvents[rideObject->riderObjectEventId].hideReflection = FALSE;
            }
            return TRUE;
        }
    }

    return FALSE;
}

static u8 CheckForPlayerAvatarCollision(u8 direction)
{
    s16 x, y;
    struct ObjectEvent *playerObjEvent = &gObjectEvents[gPlayerAvatar.objectEventId];

    x = playerObjEvent->currentCoords.x;
    y = playerObjEvent->currentCoords.y;
    MoveCoords(direction, &x, &y);
    return CheckForObjectEventCollision(playerObjEvent, x, y, direction, MapGridGetMetatileBehaviorAt(x, y));
}


static bool8 AreElevationsCompatible(u8 a, u8 b)
{
    if (a == 0 || b == 0)
        return TRUE;

    if (a != b)
        return FALSE;

    return TRUE;
}

static bool8 CheckNonPlayerObjectAt(struct ObjectEvent *playerObjEvent, s16 x, s16 y)
{
    u8 i;
    struct ObjectEvent *curObject;

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        curObject = &gObjectEvents[i];
        if (curObject->active && curObject != playerObjEvent)
        {
            // check for collision if curObject is active, not the object in question, and not exempt from collisions
            if ((curObject->currentCoords.x == x && curObject->currentCoords.y == y) || (curObject->previousCoords.x == x && curObject->previousCoords.y == y))
            {
                if (AreElevationsCompatible(playerObjEvent->currentElevation, curObject->currentElevation))
                    return TRUE;
            }
        }
    }
    return FALSE;
}

static u8 CheckForPlayerLandingCollision()
{
    s16 x, y;
    struct ObjectEvent *playerObjEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    const struct RideMonInfo* rideInfo = GetCurrentRideMonInfo();

    x = playerObjEvent->currentCoords.x;
    y = playerObjEvent->currentCoords.y;

    if(MapGridIsImpassableAt(x, y))
    {
        return TRUE;
    }
    
    if(!(rideInfo->flags & RIDE_MON_FLAG_CAN_SWIM))
    {
        u32 tileBehaviour = MapGridGetMetatileBehaviorAt(x, y);

        // Can't land on water
        if(MetatileBehavior_IsSurfableWaterOrUnderwater(tileBehaviour))
            return TRUE;
    }

    if(CheckNonPlayerObjectAt(playerObjEvent, x, y))
    {
        return TRUE;
    }

    return FALSE;
}

static void PlayerOnRideMonCollide(u8 direction)
{
    sRideMonData.rideFrameCounter = 0;

    PlayCollisionSoundIfNotFacingWarp(direction);
    PlayerSetAnimId(GetWalkInPlaceSlowMovementAction(direction), COPY_MOVE_WALK);
}

static void PlayCollisionSoundIfNotFacingWarp(u8 direction)
{
    s16 x, y;
    u8 metatileBehavior = gObjectEvents[gPlayerAvatar.objectEventId].currentMetatileBehavior;

    if (!sArrowWarpMetatileBehaviorChecks3[direction - 1](metatileBehavior))
    {
        // Check if walking up into a door
        if (direction == DIR_NORTH)
        {
            PlayerGetDestCoords(&x, &y);
            MoveCoords(direction, &x, &y);
            if (MetatileBehavior_IsWarpDoor(MapGridGetMetatileBehaviorAt(x, y)))
                return;
        }
        PlaySE(SE_WALL_HIT);
    }
}