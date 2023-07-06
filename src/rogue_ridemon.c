#include "global.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/songs.h"

#include "bike.h"
#include "event_object_movement.h"
#include "fieldmap.h"
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
    RIDER_SPRITE_SITTING,
    RIDER_SPRITE_GRABBING,
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
    u8 riderSpriteMode : 1; // RIDER_SPRITE_
    struct RideMonSpriteInfo spriteInfo[RIDE_SPRITE_DIR_COUNT];
};

static const struct RideMonInfo sRideMonInfo[NUM_SPECIES] = 
{
    [SPECIES_LAPRAS] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_SWIM,
        .riderSpriteMode = RIDER_SPRITE_SITTING,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -4, 0, 2, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -6, 0, 2, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -3, -4, 6, 2, RIDER_SHOW_INFRONT },
        }
    }
};


struct TestData
{
    u8 spriteId;
#ifdef ROGUE_DEBUG
    bool8 useDebugInfo;
    bool8 debugSetter;
    struct RideMonInfo debugInfo;
#endif
};

EWRAM_DATA struct TestData sTestData = {0};


void Rogue_RideMonInit()
{
    sTestData.spriteId = SPRITE_NONE;
#ifdef ROGUE_DEBUG
    sTestData.useDebugInfo = FALSE;
    sTestData.debugSetter = TRUE;
#endif
}

static void UpdateRideSpriteInternal(u8 mountSpriteId, u8 riderSpriteId, const struct RideMonInfo* rideInfo);

// Based on GetOnOffBike
void Rogue_GetOnOffRideMon()
{
    if (gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_RIDING)
    {
        SetPlayerAvatarTransitionFlags(PLAYER_AVATAR_FLAG_ON_FOOT);
        //Overworld_ClearSavedMusic();
        //Overworld_PlaySpecialMapMusic();
    }
    else
    {
        SetPlayerAvatarTransitionFlags(PLAYER_AVATAR_FLAG_RIDING);
        //Overworld_SetSavedMusic(MUS_CYCLING);
        //Overworld_ChangeMusicTo(MUS_CYCLING);
    }

    Rogue_CreateDestroyRideMonSprites();
    SetupFollowParterMonObjectEvent();
}

void Rogue_CreateDestroyRideMonSprites()
{
    if(TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_RIDING))
    {
        if(sTestData.spriteId == SPRITE_NONE)
        {
            s16 playerX, playerY;
            PlayerGetDestCoords(&playerX, &playerY);

            sTestData.spriteId = CreateObjectGraphicsSpriteInObjectEventSpace(OBJ_EVENT_GFX_FOLLOW_MON_PARTNER, SpriteCallbackDummy, playerX, playerY, 0);
            gSprites[sTestData.spriteId].oam.priority = 2;
            StartSpriteAnim(&gSprites[sTestData.spriteId], ANIM_STD_GO_SOUTH);
        }
    }
    else
    {
        if(sTestData.spriteId != SPRITE_NONE)
        {
            DestroySprite(&gSprites[sTestData.spriteId]);
            sTestData.spriteId = SPRITE_NONE;
        }
    }
}

void Rogue_UpdateRideMonSprites()
{
    if(sTestData.spriteId != SPRITE_NONE)
    {
        const struct RideMonInfo* rideInfo = &sRideMonInfo[SPECIES_LAPRAS];

#ifdef ROGUE_DEBUG
        if(sTestData.useDebugInfo)
        {
            if(sTestData.debugSetter)
            {
                sTestData.debugSetter = FALSE;
                memcpy(&sTestData.debugInfo, rideInfo, sizeof(struct RideMonInfo));
            }

            rideInfo = &sTestData.debugInfo;
        }
#endif

        UpdateRideSpriteInternal(sTestData.spriteId, gPlayerAvatar.spriteId, rideInfo);
    }
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
}

//
// Movement code
//

static u8 CheckMovementInputOnRideMon(u8);
static void PlayerOnRideMonNotMoving(u8, u16);
static void PlayerOnRideMonTurningInPlace(u8, u16);
static void PlayerOnRideMonMoving(u8, u16);

static u8 CheckForPlayerAvatarCollision(u8 direction);
static void PlayerOnRideMonCollide(u8);
static void PlayCollisionSoundIfNotFacingWarp(u8 direction);

static void (*const sPlayerOnRideMonFuncs[])(u8, u16) =
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
    sPlayerOnRideMonFuncs[CheckMovementInputOnRideMon(direction)](direction, heldKeys);
}

s16 RideMonGetPlayerSpeed()
{
    if(gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_DASH)
        return PLAYER_SPEED_FASTER;

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

static void PlayerOnRideMonNotMoving(u8 direction, u16 heldKeys)
{
    PlayerFaceDirection(GetPlayerFacingDirection());
}

static void PlayerOnRideMonTurningInPlace(u8 direction, u16 heldKeys)
{
    PlayerTurnInPlace(direction);
}

static void PlayerOnRideMonMoving(u8 direction, u16 heldKeys)
{
    u8 collision = CheckForPlayerAvatarCollision(direction);

    if (collision)
    {
        if (collision == COLLISION_LEDGE_JUMP)
        {
            PlayerJumpLedge(direction);
            return;
        }
        else
        {
            u8 adjustedCollision = collision - COLLISION_STOP_SURFING;
            if (adjustedCollision > 3)
                PlayerOnRideMonCollide(direction);
            return;
        }
    }

    //if (gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_SURFING)
    //{
    //    // same speed as running
    //    PlayerWalkFast(direction);
    //    return;
    //}

    // The running speed should be kept in sync with above RideMonGetPlayerSpeed
    if (GetPlayerSpritingState(heldKeys) && IsRunningDisallowed(gObjectEvents[gPlayerAvatar.objectEventId].currentMetatileBehavior) == 0 && !FollowerComingThroughDoor())
    {
        gPlayerAvatar.flags |= PLAYER_AVATAR_FLAG_DASH;
        PlayerWalkFaster(direction);
        return;
    }
    else
    {
        PlayerWalkFast(direction);
    }

    //PlayerWalkNormal(direction);
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

static void PlayerOnRideMonCollide(u8 direction)
{
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