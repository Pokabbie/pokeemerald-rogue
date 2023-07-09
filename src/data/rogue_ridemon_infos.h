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
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_FLY,
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
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_FLY,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -6, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -12, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -5, -10, 3, 1, RIDER_SHOW_BEHIND },
        }
    },
    [SPECIES_FEAROW] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_FLY,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -8, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -12, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -5, -12, 3, 1, RIDER_SHOW_BEHIND },
        }
    },
    [SPECIES_ARBOK] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_CLIMB,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -6, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -6, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -3, -6, 2, 1, RIDER_SHOW_BEHIND },
        }
    },
    [SPECIES_NIDOQUEEN] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_CLIMB,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -6, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -6, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -5, -6, 2, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_NIDOKING] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_CLIMB,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -6, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -6, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -5, -6, 2, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_PARASECT] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -6, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -6, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -5, -6, 2, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_PRIMEAPE] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_CLIMB,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -6, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -6, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -5, -6, 2, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_ARCANINE] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -8, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -2, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -2, -8, 2, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_POLIWRATH] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_SWIM,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -8, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -10, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -4, -8, 2, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_ALAKAZAM] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_FLY,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -8, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -10, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -4, -8, 2, 1, RIDER_SHOW_BEHIND },
        }
    },
    [SPECIES_MACHAMP] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_CLIMB,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -8, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -10, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -4, -8, 2, 1, RIDER_SHOW_BEHIND },
        }
    },
    [SPECIES_VICTREEBEL] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -8, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -10, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -4, -8, 2, 1, RIDER_SHOW_BEHIND },
        }
    },
    [SPECIES_TENTACRUEL] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_SWIM,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -8, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -10, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -4, -8, 2, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_GOLEM] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_CLIMB,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -8, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -10, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -4, -8, 2, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_RAPIDASH] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -8, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -10, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -4, -8, 2, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_SLOWBRO] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_SWIM,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -8, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -10, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -4, -8, 2, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_DODRIO] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -4, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -6, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -5, -6, 0, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_DEWGONG] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_SWIM,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -4, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -6, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -5, -6, 0, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_MUK] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -4, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -6, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -5, -6, 0, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_ONIX] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_CLIMB,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -6, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -10, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -10, -8, 2, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_ELECTRODE] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -6, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -10, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -10, -8, 2, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_WEEZING] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_FLY,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -6, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -10, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -10, -8, 2, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_RHYDON] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_CLIMB,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -6, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -10, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -4, -8, 2, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_KANGASKHAN] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_CLIMB,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -6, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -10, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -4, -8, 2, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_SEAKING] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_SWIM,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -6, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -10, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -4, -8, 2, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_STARMIE] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_SWIM,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -6, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -10, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -4, -8, 2, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_TAUROS] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -8, 0, 3, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -9, 0, 3, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_SIDE]  = { 0, -8, 2, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_GYARADOS] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_SWIM,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -8, 0, 3, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -12, 0, 3, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -3, -10, 4, 1, RIDER_SHOW_INFRONT },
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
    },
    [SPECIES_AERODACTYL] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_FLY,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -6, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -8, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -5, -10, 3, 1, RIDER_SHOW_BEHIND },
        }
    },
    [SPECIES_SNORLAX] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_CLIMB,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -6, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -8, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -8, -10, 3, 1, RIDER_SHOW_INFRONT },
        }
    },
    [SPECIES_ARTICUNO] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_FLY,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -6, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -10, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -4, -10, 2, 1, RIDER_SHOW_BEHIND },
        }
    },
    [SPECIES_MOLTRES] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_FLY,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -6, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -10, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -6, -10, 2, 1, RIDER_SHOW_BEHIND },
        }
    },
    [SPECIES_ZAPDOS] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_FLY,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -8, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -10, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -8, -10, 2, 1, RIDER_SHOW_BEHIND },
        }
    },
    [SPECIES_DRAGONITE] = 
    {
        .flags = RIDE_MON_FLAG_CAN_RIDE | RIDE_MON_FLAG_CAN_FLY,
        .spriteInfo = 
        {
            [RIDE_SPRITE_DIR_UP]    = { 0, -8, 0, 1, RIDER_SHOW_INFRONT },
            [RIDE_SPRITE_DIR_DOWN]  = { 0, -10, 0, 1, RIDER_SHOW_BEHIND },
            [RIDE_SPRITE_DIR_SIDE]  = { -6, -10, 2, 1, RIDER_SHOW_BEHIND },
        }
    },
};