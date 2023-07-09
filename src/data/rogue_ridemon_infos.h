#include "constants/species.h"

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

static const struct RideMonInfo sRideMonInfo[NUM_SPECIES] = 
{
    // Gen 1
    [SPECIES_VENUSAUR] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_CLIMB,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -2, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -10, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -10, -4, 3, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_CHARIZARD] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -6, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -12, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -7, -10, 3, 1, RIDER_SHOW_BEHIND },
        }
    },
    [SPECIES_BLASTOISE] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_SWIM,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -6, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -12, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -7, -10, 3, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_PIDGEOT] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -6, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -12, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -5, -10, 3, 1, RIDER_SHOW_BEHIND },
        }
    },
    [SPECIES_FEAROW] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -8, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -12, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -5, -12, 3, 1, RIDER_SHOW_BEHIND },
        }
    },

    [SPECIES_LAPRAS] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_SWIM,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -4, 0, 2, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -6, 0, 2, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -3, -4, 6, 2, RIDER_SHOW_INFRONT },
        }
    }
};