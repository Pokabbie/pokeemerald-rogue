#include "global.h"
#include "constants/event_objects.h"
#include "constants/trainers.h"

#include "global.fieldmap.h"

#include "rogue_player_customisation.h"


struct PlayerOutfit
{
    u16 trainerFrontPic;
    u16 trainerBackPic;
    u16 objectEventGfx[PLAYER_AVATAR_STATE_COUNT];
};

enum 
{
    PLAYER_OUTFIT_BRENDAN,
    PLAYER_OUTFIT_MAY,
    PLAYER_OUTFIT_RED,
    PLAYER_OUTFIT_LEAF,
    PLAYER_OUTFIT_ETHAN,
    PLAYER_OUTFIT_LYRA,
    
    PLAYER_OUTFIT_TEST,

    PLAYER_OUTFIT_COUNT,
};

static const struct PlayerOutfit sPlayerOutfits[PLAYER_OUTFIT_COUNT] =
{
    [PLAYER_OUTFIT_BRENDAN] =
    {
        .trainerFrontPic = TRAINER_PIC_BRENDAN,
        .trainerBackPic = TRAINER_BACK_PIC_BRENDAN,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL] = OBJ_EVENT_GFX_BRENDAN_NORMAL,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING] = OBJ_EVENT_GFX_BRENDAN_RIDING,
            [PLAYER_AVATAR_STATE_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE, // <- todo remove this
        }
    },
    [PLAYER_OUTFIT_MAY] =
    {
        .trainerFrontPic = TRAINER_PIC_MAY,
        .trainerBackPic = TRAINER_BACK_PIC_MAY,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL] = OBJ_EVENT_GFX_MAY_NORMAL,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING] = OBJ_EVENT_GFX_MAY_RIDING,
            [PLAYER_AVATAR_STATE_FIELD_MOVE] = OBJ_EVENT_GFX_MAY_FIELD_MOVE, // <- todo remove this
        }
    },

    [PLAYER_OUTFIT_RED] =
    {
        .trainerFrontPic = TRAINER_PIC_RED,
        .trainerBackPic = TRAINER_BACK_PIC_MAY,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL] = OBJ_EVENT_GFX_RED,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING] = OBJ_EVENT_GFX_RED_RIDING,
            [PLAYER_AVATAR_STATE_FIELD_MOVE] = OBJ_EVENT_GFX_RED_FIELD_MOVE, // <- todo remove this
        }
    },
    [PLAYER_OUTFIT_LEAF] =
    {
        .trainerFrontPic = TRAINER_PIC_LEAF,
        .trainerBackPic = TRAINER_BACK_PIC_LEAF,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL] = OBJ_EVENT_GFX_LEAF,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING] = OBJ_EVENT_GFX_LEAF_RIDING,
            [PLAYER_AVATAR_STATE_FIELD_MOVE] = OBJ_EVENT_GFX_LEAF_FIELD_MOVE, // <- todo remove this
        }
    },
    
    [PLAYER_OUTFIT_ETHAN] =
    {
        .trainerFrontPic = TRAINER_PIC_ETHAN,
        .trainerBackPic = TRAINER_BACK_PIC_ETHAN,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL] = OBJ_EVENT_GFX_ETHAN,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING] = OBJ_EVENT_GFX_ETHAN_RIDING,
            [PLAYER_AVATAR_STATE_FIELD_MOVE] = OBJ_EVENT_GFX_ETHAN_FIELD_MOVE, // <- todo remove this
        }
    },
    [PLAYER_OUTFIT_LYRA] =
    {
        .trainerFrontPic = TRAINER_PIC_LYRA,
        .trainerBackPic = TRAINER_BACK_PIC_LYRA,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL] = OBJ_EVENT_GFX_LYRA,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING] = OBJ_EVENT_GFX_LYRA_RIDING,
            [PLAYER_AVATAR_STATE_FIELD_MOVE] = OBJ_EVENT_GFX_LYRA_FIELD_MOVE, // <- todo remove this
        }
    },
    [PLAYER_OUTFIT_TEST] =
    {
        .trainerFrontPic = TRAINER_PIC_MAGMA_GRUNT_F,
        .trainerBackPic = TRAINER_BACK_PIC_WALLY,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL] = OBJ_EVENT_GFX_MAGMA_MEMBER_F,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING] = OBJ_EVENT_GFX_MAGMA_MEMBER_F,
            [PLAYER_AVATAR_STATE_FIELD_MOVE] = OBJ_EVENT_GFX_LYRA_FIELD_MOVE, // <- todo remove this
        }
    }
};

static const struct PlayerOutfit* GetCurrentOutfit()
{
    // TODO - Should probably have a missing no outfit?
    return &sPlayerOutfits[min(RoguePlayer_GetOutfitId(), PLAYER_OUTFIT_COUNT - 1)];
}

void RoguePlayer_SetOutfitId(u16 outfit)
{
    gSaveBlock2Ptr->playerGender = outfit;
}

u16 RoguePlayer_GetOutfitId()
{
    return gSaveBlock2Ptr->playerGender;
}

u16 RoguePlayer_GetOutfitCount()
{
    return PLAYER_OUTFIT_COUNT;
}

u16 RoguePlayer_GetTrainerFrontPic()
{
    return GetCurrentOutfit()->trainerFrontPic;
}

u16 RoguePlayer_GetTrainerBackPic()
{
    return GetCurrentOutfit()->trainerBackPic;
}

u16 RoguePlayer_GetObjectGfx(u8 state)
{
    u16 gfx = GetCurrentOutfit()->objectEventGfx[state];
    return gfx;
}
