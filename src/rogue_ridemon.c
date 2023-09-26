#include "global.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
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

#include "rogue_followmon.h"
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
    u8 riderLocalId;
    u8 riderSpriteId;
    u8 monSpriteId;
};

struct RideMonData
{
    struct RideObjectEvent playerObject;
    struct RogueRideMonState playerRideState;
    u8 rideFrameCounter;
    u8 recentRideIndex;
};

EWRAM_DATA struct RideMonData sRideMonData = {0};

void ResetRideObject(struct RideObjectEvent* rideObject)
{
    rideObject->riderLocalId = 0;
    rideObject->riderSpriteId = SPRITE_NONE;
    rideObject->monSpriteId = SPRITE_NONE;
}

void Rogue_RideMonInit()
{
    sRideMonData.rideFrameCounter = 0;
    sRideMonData.playerRideState.flyingState = 0;
    sRideMonData.playerRideState.flyingHeight = 0;
    sRideMonData.playerRideState.monGfx = SPECIES_NONE;
    sRideMonData.recentRideIndex = 0;
    
    ResetRideObject(&sRideMonData.playerObject);
}

struct RogueRideMonState* Rogue_GetPlayerRideMonStatePtr()
{
    return &sRideMonData.playerRideState;
}

static void UpdateRideMonSprites();
static void UpdateRideSpriteInternal(u8 mountSpriteId, u8 riderSpriteId, const struct RideMonInfo* rideInfo);
static bool8 AdjustFlyingAnimation(u8 objectEventId);

static u16 GetCurrentRideMonSpecies();
static const struct RideMonInfo* GetRideMonInfoForSpecies(u16 species);

static bool8 IsValidMonToRideNow(struct Pokemon* mon)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES);

    if(species != SPECIES_NONE)
    {
        const struct RideMonInfo* rideInfo = GetRideMonInfoForSpecies(species);

        if(rideInfo && (rideInfo->flags & RIDE_MON_FLAG_CAN_RIDE) != 0)
            return TRUE;
    }

    return FALSE;
}

static bool8 CalculateRideSpecies(s8 dir)
{
    u8 counter;
    s8 monIdx;

    // Loop through mons from last riden
    sRideMonData.recentRideIndex = min(sRideMonData.recentRideIndex, gPlayerPartyCount - 1);
    sRideMonData.playerRideState.monGfx = SPECIES_NONE;

    for(counter = 0; counter < gPlayerPartyCount; ++counter)
    {
        monIdx = sRideMonData.recentRideIndex;

        if(dir != 0)
            monIdx += (1 + counter) * dir;
        else
            monIdx = counter;

        while(monIdx < 0)
            monIdx += gPlayerPartyCount;
        
        monIdx %= gPlayerPartyCount;

        if(IsValidMonToRideNow(&gPlayerParty[monIdx]))
        {
            sRideMonData.recentRideIndex = monIdx;
            sRideMonData.playerRideState.monGfx = FollowMon_GetMonGraphics(&gPlayerParty[monIdx]);
            return TRUE;
        }
    }

    return FALSE;
}

static bool8 CalculateInitialRideSpecies()
{
    u8 counter;
    u8 monIdx;

    sRideMonData.recentRideIndex = min(sRideMonData.recentRideIndex, gPlayerPartyCount - 1);

    // Try to ride the same species we were previously riding
    for(counter = 0; counter < gPlayerPartyCount; ++counter)
    {
        monIdx = (sRideMonData.recentRideIndex + counter) % gPlayerPartyCount;

        if(IsValidMonToRideNow(&gPlayerParty[monIdx]) && FollowMon_GetMonGraphics(&gPlayerParty[monIdx]) == sRideMonData.playerRideState.monGfx)
        {
            sRideMonData.recentRideIndex = monIdx;
            return TRUE;
        }
    }

    // Can't ride the mon we were previously riding, so try to pick next avaliable
    return CalculateRideSpecies(0);
}

// Based on GetOnOffBike
void Rogue_GetOnOffRideMon(bool8 forWarp)
{
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
        if(CalculateInitialRideSpecies())
        {
            SetPlayerAvatarTransitionFlags(PLAYER_AVATAR_FLAG_RIDING);

            if(IsCryPlaying())
                StopCry();
            PlayCry_Normal(GetCurrentRideMonSpecies(), 0);
        }
        else
        {
            PlaySE(SE_FAILURE);
        }
    }

    // Delete existing sprite
    if(sRideMonData.playerObject.monSpriteId != SPRITE_NONE)
    {
        DestroySprite(&gSprites[sRideMonData.playerObject.monSpriteId]);
        sRideMonData.playerObject.monSpriteId = SPRITE_NONE;
    }

    if(!forWarp)
        SetupFollowParterMonObjectEvent();
}

static bool8 CanCycleRideMons()
{
    return !Rogue_IsRideMonFlying() && !Rogue_IsRideMonSwimming();
}

bool8 Rogue_HandleRideMonInput()
{
    if(TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_RIDING))
    {
        if(JOY_NEW(L_BUTTON))
        {
            if(CanCycleRideMons() && CalculateRideSpecies(-1))
            {
                if(IsCryPlaying())
                    StopCry();
                PlayCry_Normal(GetCurrentRideMonSpecies(), 0);

                // Dealloc sprite, so it can get remade
                if(sRideMonData.playerObject.monSpriteId != SPRITE_NONE)
                {
                    DestroySprite(&gSprites[sRideMonData.playerObject.monSpriteId]);
                    sRideMonData.playerObject.monSpriteId = SPRITE_NONE;
                }
            }
            else
            {
                PlaySE(SE_WALL_HIT);
            }
        }
        else if(JOY_NEW(R_BUTTON))
        {
            if(CanCycleRideMons() && CalculateRideSpecies(1))
            {
                if(IsCryPlaying())
                    StopCry();
                PlayCry_Normal(GetCurrentRideMonSpecies(), 0);

                // Dealloc sprite, so it can get remade
                if(sRideMonData.playerObject.monSpriteId != SPRITE_NONE)
                {
                    DestroySprite(&gSprites[sRideMonData.playerObject.monSpriteId]);
                    sRideMonData.playerObject.monSpriteId = SPRITE_NONE;
                }
            }
            else
            {
                PlaySE(SE_WALL_HIT);
            }
        }
    }

    // Never block input from here
    return FALSE;
}

void Rogue_UpdateRideMons()
{
    sRideMonData.playerObject.riderLocalId = gPlayerAvatar.objectEventId;
    sRideMonData.playerObject.riderSpriteId = gPlayerAvatar.spriteId;

    UpdateRideMonSprites();
}

void Rogue_DestroyRideMonSprites()
{
    sRideMonData.playerObject.monSpriteId = SPRITE_NONE;
}

u16 Rogue_GetRideMonSpeciesGfx()
{
    return sRideMonData.playerRideState.monGfx;
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

static u8 CalculateMovementModeForInternal(u16 species)
{
    u8 speed = gBaseStats[species].baseSpeed;
    
    if(speed <= 30)
        return RIDE_MOVEMENT_SLOW;

    if(speed <= 50)
        return RIDE_MOVEMENT_ACCELERATE_AVERAGE;
    
    if(speed <= 90)
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

static u16 GetCurrentRideMonSpecies()
{
    if(sRideMonData.playerRideState.monGfx >= FOLLOWMON_SHINY_OFFSET)
        return sRideMonData.playerRideState.monGfx -FOLLOWMON_SHINY_OFFSET;

    return sRideMonData.playerRideState.monGfx;
}

static const struct RideMonInfo* GetCurrentRideMonInfo()
{
    return GetRideMonInfoForSpecies(GetCurrentRideMonSpecies());
}

static bool8 ShouldRideMonBeVisible()
{
    return TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_RIDING) && !gObjectEvents[gPlayerAvatar.objectEventId].invisible;
}

static void UpdateRideMonSprites()
{
    if(ShouldRideMonBeVisible())
    {
        // Alloc sprite
        if(sRideMonData.playerObject.monSpriteId == SPRITE_NONE)
        {
            // If we're attempting to ride a mon, but for whatever reason we no longer can (e.g. released mon we were riding) unmount here
            if(CalculateInitialRideSpecies())
            {
                s16 playerX, playerY;
                PlayerGetDestCoords(&playerX, &playerY);

                sRideMonData.playerObject.monSpriteId = CreateObjectGraphicsSpriteInObjectEventSpace(OBJ_EVENT_GFX_FOLLOW_MON_PARTNER, SpriteCallbackDummy, playerX, playerY, 0);
                gSprites[sRideMonData.playerObject.monSpriteId].oam.priority = 2;
                StartSpriteAnim(&gSprites[sRideMonData.playerObject.monSpriteId], ANIM_STD_GO_SOUTH);
                
                // Handle returning to the screen after flying
                if(Rogue_IsRideMonFlying())
                {
                    SetShadowFieldEffectVisible(&gObjectEvents[gPlayerAvatar.objectEventId], TRUE);
                    gObjectEvents[gPlayerAvatar.objectEventId].hideReflection = TRUE;
                }
            }
            else
            {
                // Force demount here
                Rogue_GetOnOffRideMon(FALSE);
            }
        }
    }
    else
    {
        // Dealloc sprite
        if(sRideMonData.playerObject.monSpriteId != SPRITE_NONE)
        {
            DestroySprite(&gSprites[sRideMonData.playerObject.monSpriteId]);
            sRideMonData.playerObject.monSpriteId = SPRITE_NONE;
        }
    }

    if(sRideMonData.playerObject.monSpriteId != SPRITE_NONE)
    {
        const struct RideMonInfo* rideInfo = GetCurrentRideMonInfo();

        if(rideInfo != NULL)
        {
            UpdateRideSpriteInternal(sRideMonData.playerObject.monSpriteId, gPlayerAvatar.spriteId, rideInfo);
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
        sRideMonData.playerRideState.flyingState = FALSE;
        sRideMonData.playerRideState.flyingHeight = 0;
        AdjustFlyingAnimation(gPlayerAvatar.objectEventId);
    }
}

u8 Rogue_GetRideMonSprite(struct ObjectEvent* objectEvent)
{
    if(objectEvent == &gObjectEvents[gPlayerAvatar.objectEventId])
    {
        return sRideMonData.playerObject.monSpriteId;
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

bool8 Rogue_IsRideMonSwimming()
{
    if (gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_RIDING)
    {
        s16 x, y;
        u16 tileBehavior;

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
    if (gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_RIDING)
    {
        return sRideMonData.playerRideState.flyingState || sRideMonData.playerRideState.flyingHeight != 0;
    }

    return FALSE;
}

static void UpdateRideSpriteInternal(u8 mountSpriteId, u8 riderSpriteId, const struct RideMonInfo* rideInfo)
{
    s8 xFlip;
    const struct RideMonSpriteInfo* rideSpriteInfo;

    struct Sprite* mountSprite = &gSprites[sRideMonData.playerObject.monSpriteId];
    struct Sprite* riderSprite = &gSprites[gPlayerAvatar.spriteId];

    switch (GetPlayerFacingDirection())
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

    mountSprite->x = riderSprite->x;
    mountSprite->y = riderSprite->y;
    mountSprite->x2 = riderSprite->x2 + rideSpriteInfo->monX * xFlip;
    mountSprite->y2 = riderSprite->y2 + rideSpriteInfo->monY;

    mountSprite->subpriority = riderSprite->subpriority;

    mountSprite->oam.x = riderSprite->oam.x;
    mountSprite->oam.y = riderSprite->oam.y;
    mountSprite->oam.priority = riderSprite->oam.priority;

    switch (PlayerGetCopyableMovement())
    {
    case COPY_MOVE_WALK:
        StartSpriteAnimIfDifferent(mountSprite, ANIM_STD_GO_SOUTH + GetPlayerFacingDirection() - DIR_SOUTH);
        break;

    case COPY_MOVE_WALK_FAST:
        StartSpriteAnimIfDifferent(mountSprite, ANIM_STD_GO_FAST_SOUTH + GetPlayerFacingDirection() - DIR_SOUTH);
        break;

    case COPY_MOVE_WALK_FASTER:
        StartSpriteAnimIfDifferent(mountSprite, ANIM_STD_GO_FASTER_SOUTH + GetPlayerFacingDirection() - DIR_SOUTH);
        break;

    // We always want to face the correct direction
    default: // COPY_MOVE_FACE
        StartSpriteAnimIfDifferent(mountSprite, ANIM_STD_FACE_SOUTH + GetPlayerFacingDirection() - DIR_SOUTH);
        break;
    }

    if(rideSpriteInfo->playerRendersInFront)
        mountSprite->subpriority++;
    else
        mountSprite->subpriority--;

    // Move player up here?
    riderSprite->x2 += + rideSpriteInfo->playerX * xFlip;
    riderSprite->y2 += + rideSpriteInfo->playerY;

    if(sRideMonData.playerRideState.flyingHeight != 0)
    {
        mountSprite->y2 -= sRideMonData.playerRideState.flyingHeight;
        riderSprite->y2 -= sRideMonData.playerRideState.flyingHeight;
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

static void PlayerOnRideMonNotMoving(u8 direction, u16 newKeys, u16 heldKeys)
{
    if(newKeys & B_BUTTON && (Rogue_IsRideMonFlying() || Rogue_CanRideMonFly()))
    {
        // Toggle between flying modes
        bool8 desiredFlyState = !sRideMonData.playerRideState.flyingState;

        if(!desiredFlyState)
        {
            if(CheckForPlayerLandingCollision())
            {
                // We're not allowed to land here
                PlaySE(SE_FAILURE);
                return;
            }
        }

        sRideMonData.playerRideState.flyingState = desiredFlyState;
        PlaySE(sRideMonData.playerRideState.flyingState ? SE_M_FLY : SE_M_WING_ATTACK);
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

        switch (CalculateMovementModeFor(GetCurrentRideMonSpecies()))
        {
        case RIDE_MOVEMENT_SLOW:
            PlayerWalkNormal(direction);
            break;

        case RIDE_MOVEMENT_AVERAGE:
            PlayerWalkFast(direction);
            break;

        case RIDE_MOVEMENT_FAST:
            PlayerWalkFaster(direction);
            break;

        case RIDE_MOVEMENT_ACCELERATE_AVERAGE:
            frameIdx = sRideMonData.rideFrameCounter / 4;

            if(frameIdx == 0)
                PlayerWalkNormal(direction);
            else
                PlayerWalkFast(direction);
            break;

        case RIDE_MOVEMENT_ACCELERATE_FAST:
            frameIdx = (sRideMonData.rideFrameCounter - 1) / 3;

            if(sRideMonData.rideFrameCounter == 0)
                PlayerWalkNormal(direction);
            else if(frameIdx == 0)
                PlayerWalkFast(direction);
            else
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
    return AdjustFlyingAnimation(gPlayerAvatar.objectEventId);
}

static bool8 AdjustFlyingAnimation(u8 objectEventId)
{
    if(sRideMonData.playerRideState.flyingState)
    {
        if(sRideMonData.playerRideState.flyingHeight < 16)
        {
            if(sRideMonData.playerRideState.flyingHeight == 0)
            {
                SetShadowFieldEffectVisible(&gObjectEvents[objectEventId], TRUE);
                gObjectEvents[objectEventId].hideReflection = TRUE;
            }

            ++sRideMonData.playerRideState.flyingHeight;
            return TRUE;
        }
    }
    else
    {
        if(sRideMonData.playerRideState.flyingHeight > 0)
        {
            --sRideMonData.playerRideState.flyingHeight;

            if(sRideMonData.playerRideState.flyingHeight == 0)
            {
                SetShadowFieldEffectVisible(&gObjectEvents[objectEventId], FALSE);
                gObjectEvents[objectEventId].hideReflection = FALSE;
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