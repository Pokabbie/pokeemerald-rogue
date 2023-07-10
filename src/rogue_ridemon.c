#include "global.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/songs.h"

#include "bike.h"
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
    s8 playerX;
    s8 playerY;
    s8 monX;
    s8 monY;
    u8 playerRendersInFront : 1;
};

struct RideMonInfo
{
    u8 flags;
    struct RideMonSpriteInfo spriteInfo[RIDE_SPRITE_DIR_COUNT];
};

#include "data/rogue_ridemon_infos.h"

struct TestData
{
    u8 spriteId;
#ifdef ROGUE_DEBUG
    bool8 useDebugInfo;
    bool8 debugSetter;
    u8 debugMoveSpeed;
    struct RideMonInfo debugInfo;
#endif
};

struct RideMonData
{
    u8 rideFrameCounter;
    u8 flyingHeight;
    bool8 desiredFlyingState;
};

EWRAM_DATA struct TestData sTestData = {0};

EWRAM_DATA struct RideMonData sRideMonData = {0};


void Rogue_RideMonInit()
{
    sRideMonData.rideFrameCounter = 0;
    sRideMonData.desiredFlyingState = 0;
    sRideMonData.flyingHeight = 0;

    sTestData.spriteId = SPRITE_NONE;
#ifdef ROGUE_DEBUG
    sTestData.useDebugInfo = FALSE;
    sTestData.debugSetter = TRUE;
    sTestData.debugMoveSpeed = RIDE_MOVEMENT_SLOW;
#endif
}

static void UpdateRideSpriteInternal(u8 mountSpriteId, u8 riderSpriteId, const struct RideMonInfo* rideInfo);

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

    if (gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_RIDING)
    {
        SetPlayerAvatarTransitionFlags(PLAYER_AVATAR_FLAG_ON_FOOT);
        //Overworld_ClearSavedMusic();
        //Overworld_PlaySpecialMapMusic();
        //PlaySE(SE_MUD_BALL);
    }
    else
    {
        SetPlayerAvatarTransitionFlags(PLAYER_AVATAR_FLAG_RIDING);
        //Overworld_SetSavedMusic(MUS_CYCLING);
        //Overworld_ChangeMusicTo(MUS_CYCLING);
        PlaySE(SE_M_DETECT); // detect
    }

    // Delete existing sprite
    if(sTestData.spriteId != SPRITE_NONE)
    {
        DestroySprite(&gSprites[sTestData.spriteId]);
        sTestData.spriteId = SPRITE_NONE;
    }

    Rogue_CreateDestroyRideMonSprites();

    if(!forWarp)
        SetupFollowParterMonObjectEvent();
}

void Rogue_CreateDestroyRideMonSprites()
{
    // By this point either the sprite is deleted or has been previously reset, so isn't ours
    sTestData.spriteId = SPRITE_NONE;

    if(TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_RIDING))
    {
        s16 playerX, playerY;
        PlayerGetDestCoords(&playerX, &playerY);

        sTestData.spriteId = CreateObjectGraphicsSpriteInObjectEventSpace(OBJ_EVENT_GFX_FOLLOW_MON_PARTNER, SpriteCallbackDummy, playerX, playerY, 0);
        gSprites[sTestData.spriteId].oam.priority = 2;
        StartSpriteAnim(&gSprites[sTestData.spriteId], ANIM_STD_GO_SOUTH);
    }
}

void Rogue_FreeRideMonSprites()
{
    // Assuming we're already free from calling code
    sTestData.spriteId = SPRITE_NONE;
}

static u16 Rogue_GetRideMonSpecies()
{
    return FollowMon_GetPartnerFollowSpecies(FALSE);
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

static const struct RideMonInfo* Rogue_GetRideMonInfo(u16 species)
{
    const struct RideMonInfo* rideInfo = &sRideMonInfo[ToRideSpecies(species)];

#ifdef ROGUE_DEBUG
    if(sTestData.useDebugInfo)
    {
        if(sTestData.debugSetter)
        {
            sTestData.debugSetter = FALSE;
            sTestData.debugMoveSpeed = CalculateMovementModeForInternal(species);
            memcpy(&sTestData.debugInfo, rideInfo, sizeof(struct RideMonInfo));
        }

        rideInfo = &sTestData.debugInfo;
    }
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
    if(sTestData.useDebugInfo)
    {
        return sTestData.debugMoveSpeed;
    }
#endif

    //if(Rogue_IsRideMonSwimming())
    //{
    //    moveSpeed = min(moveSpeed + 1, RIDE_MOVEMENT_FAST);
    //}

    //if(Rogue_IsRideMonFlying() && moveSpeed != RIDE_MOVEMENT_SLOW)
    //{
    //    --moveSpeed;
    //}

    return moveSpeed;
}

static const struct RideMonInfo* GetCurrentRideMonInfo()
{
    return Rogue_GetRideMonInfo(Rogue_GetRideMonSpecies());
}


void Rogue_UpdateRideMonSprites()
{
    if(sTestData.spriteId != SPRITE_NONE)
    {
        const struct RideMonInfo* rideInfo = GetCurrentRideMonInfo();

        if(rideInfo != NULL)
        {
            UpdateRideSpriteInternal(sTestData.spriteId, gPlayerAvatar.spriteId, rideInfo);
        }
    }
}

u8 Rogue_GetRideMonSprite(struct ObjectEvent* objectEvent)
{
    if(objectEvent == &gObjectEvents[gPlayerAvatar.objectEventId])
    {
        return sTestData.spriteId;
    }

    return SPRITE_NONE;
}

bool8 Rogue_CanRideMonInvJumpLedge()
{
    const struct RideMonInfo* rideInfo = GetCurrentRideMonInfo();

    if(rideInfo != NULL && (rideInfo->flags & RIDE_MON_FLAG_CAN_CLIMB) != 0)
    {
        return TRUE;
    }

    return FALSE;
}

bool8 Rogue_CanRideMonSwim()
{
    const struct RideMonInfo* rideInfo = GetCurrentRideMonInfo();

    if(rideInfo != NULL && (rideInfo->flags & RIDE_MON_FLAG_CAN_SWIM) != 0)
    {
        return TRUE;
    }

    return FALSE;
}

bool8 Rogue_CanRideMonFly()
{
    const struct RideMonInfo* rideInfo = GetCurrentRideMonInfo();

    if(rideInfo != NULL && (rideInfo->flags & RIDE_MON_FLAG_CAN_FLY) != 0)
    {
        return TRUE;
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
        return sRideMonData.desiredFlyingState || sRideMonData.flyingHeight != 0;
    }

    return FALSE;
}

static void UpdateRideSpriteInternal(u8 mountSpriteId, u8 riderSpriteId, const struct RideMonInfo* rideInfo)
{
    s8 xFlip;
    const struct RideMonSpriteInfo* rideSpriteInfo;

    struct Sprite* mountSprite = &gSprites[sTestData.spriteId];
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

    if(sRideMonData.flyingHeight != 0)
    {
        mountSprite->y2 -= sRideMonData.flyingHeight;
        riderSprite->y2 -= sRideMonData.flyingHeight;
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
        bool8 desiredFlyState = !sRideMonData.desiredFlyingState;

        if(!desiredFlyState)
        {
            if(CheckForPlayerLandingCollision())
            {
                // We're not allowed to land here
                PlaySE(SE_FAILURE);
                return;
            }
        }

        sRideMonData.desiredFlyingState = desiredFlyState;
        PlaySE(sRideMonData.desiredFlyingState ? SE_M_FLY : SE_M_WING_ATTACK);
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

        switch (CalculateMovementModeFor(Rogue_GetRideMonSpecies()))
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
    if(sRideMonData.desiredFlyingState)
    {
        if(sRideMonData.flyingHeight < 16)
        {
            if(sRideMonData.flyingHeight == 0)
            {
                SetShadowFieldEffectVisible(&gObjectEvents[gPlayerAvatar.objectEventId], TRUE);
                gObjectEvents[gPlayerAvatar.objectEventId].hideReflection = TRUE;
            }

            ++sRideMonData.flyingHeight;
            return TRUE;
        }
    }
    else
    {
        if(sRideMonData.flyingHeight > 0)
        {
            --sRideMonData.flyingHeight;

            if(sRideMonData.flyingHeight == 0)
            {
                SetShadowFieldEffectVisible(&gObjectEvents[gPlayerAvatar.objectEventId], FALSE);
                gObjectEvents[gPlayerAvatar.objectEventId].hideReflection = FALSE;
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