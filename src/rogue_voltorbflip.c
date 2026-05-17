#include "global.h"
#include "constants/songs.h"

#include "palette.h"
#include "main.h"
#include "gpu_regs.h"
#include "scanline_effect.h"
#include "task.h"
#include "malloc.h"
#include "decompress.h"
#include "event_data.h"
#include "bg.h"
#include "window.h"
#include "sound.h"
#include "strings.h"
#include "string_util.h"
#include "text.h"
#include "overworld.h"
#include "menu.h"
#include "pokedex.h"
#include "constants/rgb.h"
#include "random.h"

#include "rogue_voltorbflip.h"

#define FLIP_VERTICAL (0x08 << 8)
#define FLIP_HORIZONTAL (0x04 << 8)

#define BOARD_WIDTH 5
#define BOARD_HEIGHT 5
#define TOTAL_CARD_COUNT (BOARD_WIDTH * BOARD_HEIGHT)

#define COORDS_TO_CARD(x, y) (y * BOARD_WIDTH + x)

#define MAX_VOLTORB_FLIP_LEVEL 8
#define MAX_VOLTORB_LEVEL_VARIANTS 5

enum
{
    CARD_GFX_HIDDEN,
    CARD_GFX_SHOW_1,
    CARD_GFX_SHOW_2,
    CARD_GFX_SHOW_3,
    CARD_GFX_SHOW_VOLTORB,
    CARD_GFX_COUNT
};

enum
{
    CARD_VALUE_1,
    CARD_VALUE_2,
    CARD_VALUE_3,
    CARD_VALUE_VOLTORB,
    CARD_VALUE_COUNT
};

enum
{
    GAME_STATE_IN_PROGRESS,
    GAME_STATE_WIN,
    GAME_STATE_LOSE,
};

enum
{
    WIN_INFO_RED_X,
    WIN_INFO_GREEN_X,
    WIN_INFO_YELLOW_X,
    WIN_INFO_BLUE_X,
    WIN_INFO_PURPLE_X,

    WIN_INFO_RED_Y,
    WIN_INFO_GREEN_Y,
    WIN_INFO_YELLOW_Y,
    WIN_INFO_BLUE_Y,
    WIN_INFO_PURPLE_Y,

    WIN_COUNT,
};

struct VoltorbSpawnCounts
{
    u8 x2Count;
    u8 x3Count;
    u8 voltorbCount;
};

struct VoltorbCardState
{
    u8 value : 4;
    u8 isShown: 1;
    u8 note1 : 1;
    u8 note2 : 1;
    u8 note3 : 1;
    u8 noteVoltorb : 1;
};

struct VoltorbFlipState
{
    struct VoltorbCardState cardStates[TOTAL_CARD_COUNT];
    u8 pointerSprite;
    u8 outlineSprite;
    u8 cursorX;
    u8 cursorY;
    u8 cursorWriteValue;
};

extern const u8 gText_DexNational[];
extern const u8 gText_DexHoenn[];
extern const u8 gText_PokedexDiploma[];

static void MainCB2(void);
static void Task_VoltorbFlipFadeIn(u8);
static void Task_VoltorbFlipWaitForKeyPress(u8);
static void Task_VoltorbFlipVictory(u8);
static void Task_VoltorbFlipLoss(u8);
static void Task_VoltorbFlipFadeOut(u8);
static void DisplayVoltorbFlipText(void);
static void DrawBoardCardTiles(void);
static void InitVoltorbFlipBg(void);
static void InitVoltorbFlipSprites(void);
static void InitVoltorbFlipWindow(void);
static void PrintVoltorbFlipText(u8, u8, u8 const*, u8, u8);

static void UpdateVoltorbFlipSprites();
static void ResetVoltorbFlipCards(u8 level);
static void ShowAllCards();
static u8 CalculateBoardState();

static void DrawCardTiles(u8 x, u8 y);
static void DrawNoteTiles();

EWRAM_DATA static u8 *sVoltorbFlipTilemapPtr = NULL;
EWRAM_DATA static struct VoltorbFlipState *sVoltorbFlipState = NULL;

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static const u16 sVoltorbFlipPalettes[][16] =
{
    INCBIN_U16("graphics/rogue_voltorbflip/gameboard.gbapal"),
};

static const u32 sVoltorbFlipTilemap[] = INCBIN_U32("graphics/rogue_voltorbflip/gameboard.bin.lz");
static const u32 sVoltorbFlipTiles[] = INCBIN_U32("graphics/rogue_voltorbflip/gameboard.4bpp.lz");

static const u8 sVoltorbFlipSpriteSheetData[] = INCBIN_U8("graphics/rogue_voltorbflip/sprites.4bpp");
static const u16 sVoltorbFlipPaletteSpriteData[] = INCBIN_U16("graphics/rogue_voltorbflip/sprites.gbapal");


static const struct VoltorbSpawnCounts sVoltorbSpawnCounts[MAX_VOLTORB_FLIP_LEVEL][MAX_VOLTORB_LEVEL_VARIANTS] = 
{
    {
        { .x2Count = 3, .x3Count = 1, .voltorbCount = 6, },
        { .x2Count = 0, .x3Count = 3, .voltorbCount = 6, },
        { .x2Count = 5, .x3Count = 0, .voltorbCount = 6, },
        { .x2Count = 2, .x3Count = 2, .voltorbCount = 6, },
        { .x2Count = 4, .x3Count = 1, .voltorbCount = 6, },
    },
    {
        { .x2Count = 1, .x3Count = 3, .voltorbCount = 7, },
        { .x2Count = 6, .x3Count = 0, .voltorbCount = 7, },
        { .x2Count = 3, .x3Count = 2, .voltorbCount = 7, },
        { .x2Count = 0, .x3Count = 4, .voltorbCount = 7, },
        { .x2Count = 5, .x3Count = 1, .voltorbCount = 7, },
    },
    {
        { .x2Count = 2, .x3Count = 3, .voltorbCount = 8, },
        { .x2Count = 7, .x3Count = 0, .voltorbCount = 8, },
        { .x2Count = 4, .x3Count = 2, .voltorbCount = 8, },
        { .x2Count = 1, .x3Count = 4, .voltorbCount = 8, },
        { .x2Count = 6, .x3Count = 1, .voltorbCount = 8, },
    },
    {
        { .x2Count = 3, .x3Count = 3, .voltorbCount = 8, },
        { .x2Count = 0, .x3Count = 5, .voltorbCount = 8, },
        { .x2Count = 8, .x3Count = 0, .voltorbCount = 10, },
        { .x2Count = 5, .x3Count = 2, .voltorbCount = 10, },
        { .x2Count = 2, .x3Count = 4, .voltorbCount = 10, },
    },
    {
        { .x2Count = 7, .x3Count = 1, .voltorbCount = 10, },
        { .x2Count = 4, .x3Count = 3, .voltorbCount = 10, },
        { .x2Count = 1, .x3Count = 5, .voltorbCount = 10, },
        { .x2Count = 9, .x3Count = 0, .voltorbCount = 10, },
        { .x2Count = 6, .x3Count = 2, .voltorbCount = 10, },
    },
    {
        { .x2Count = 3, .x3Count = 4, .voltorbCount = 10, },
        { .x2Count = 0, .x3Count = 6, .voltorbCount = 10, },
        { .x2Count = 8, .x3Count = 1, .voltorbCount = 10, },
        { .x2Count = 5, .x3Count = 3, .voltorbCount = 10, },
        { .x2Count = 2, .x3Count = 5, .voltorbCount = 10, },
    },
    {
        { .x2Count = 7, .x3Count = 2, .voltorbCount = 10, },
        { .x2Count = 4, .x3Count = 4, .voltorbCount = 10, },
        { .x2Count = 1, .x3Count = 6, .voltorbCount = 10, },
        { .x2Count = 9, .x3Count = 1, .voltorbCount = 10, },
        { .x2Count = 6, .x3Count = 3, .voltorbCount = 10, },
    },
    {
        { .x2Count = 0, .x3Count = 7, .voltorbCount = 10, },
        { .x2Count = 8, .x3Count = 2, .voltorbCount = 10, },
        { .x2Count = 5, .x3Count = 4, .voltorbCount = 10, },
        { .x2Count = 2, .x3Count = 6, .voltorbCount = 10, },
        { .x2Count = 7, .x3Count = 3, .voltorbCount = 10, },
    },
};

static const u16 sCardGfxTiles[CARD_GFX_COUNT][9] = 
{
    [CARD_GFX_HIDDEN] = 
    {
        0x06, 0x07, 0x06 | FLIP_HORIZONTAL,
        0x10, 0x11, 0x10 | FLIP_HORIZONTAL,
        0x06 | FLIP_VERTICAL, 0x07 | FLIP_VERTICAL, 0x06 | FLIP_HORIZONTAL | FLIP_VERTICAL,
    },

    [CARD_GFX_SHOW_1] = 
    {
        0x25, 0x26, 0x25 | FLIP_HORIZONTAL,
        0x30, 0x47, 0x30 | FLIP_HORIZONTAL,
        0x25 | FLIP_VERTICAL, 0x26 | FLIP_VERTICAL, 0x25 | FLIP_HORIZONTAL | FLIP_VERTICAL,
    },
    [CARD_GFX_SHOW_2] = 
    {
        0x27, 0x28, 0x27 | FLIP_HORIZONTAL,
        0x32, 0x48, 0x32 | FLIP_HORIZONTAL,
        0x27 | FLIP_VERTICAL, 0x28 | FLIP_VERTICAL, 0x27 | FLIP_HORIZONTAL | FLIP_VERTICAL,
    },
    [CARD_GFX_SHOW_3] = 
    {
        0x27, 0x28, 0x27 | FLIP_HORIZONTAL,
        0x32, 0x49, 0x32 | FLIP_HORIZONTAL,
        0x27 | FLIP_VERTICAL, 0x28 | FLIP_VERTICAL, 0x27 | FLIP_HORIZONTAL | FLIP_VERTICAL,
    },
    [CARD_GFX_SHOW_VOLTORB] = 
    {
        0x43, 0x44, 0x43 | FLIP_HORIZONTAL,
        0x4A, 0x4B, 0x4A | FLIP_HORIZONTAL,
        0x4F, 0x50, 0x4F | FLIP_HORIZONTAL,
    },
};

static const struct BgTemplate sVoltorbFlipBgTemplates[2] =
{
    {
        .bg = 0,
        .charBaseIndex = 1,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0,
    },
    {
        .bg = 1,
        .charBaseIndex = 0,
        .mapBaseIndex = 6,
        .screenSize = 1,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0,
    },
};

static const struct WindowTemplate sVoltorbFlipWinTemplates[WIN_COUNT + 1] =
{
    [WIN_INFO_RED_X] =
    {
        .bg = 0,
        .tilemapLeft = 18,
        .tilemapTop = 1,
        .width = 3,
        .height = 3,
        .paletteNum = 15,
        .baseBlock = 1,
    },
    [WIN_INFO_GREEN_X] =
    {
        .bg = 0,
        .tilemapLeft = 18,
        .tilemapTop = 1 + 3 * 1,
        .width = 3,
        .height = 3,
        .paletteNum = 15,
        .baseBlock = 10,
    },
    [WIN_INFO_YELLOW_X] =
    {
        .bg = 0,
        .tilemapLeft = 18,
        .tilemapTop = 1 + 3 * 2,
        .width = 3,
        .height = 3,
        .paletteNum = 15,
        .baseBlock = 19,
    },
    [WIN_INFO_BLUE_X] =
    {
        .bg = 0,
        .tilemapLeft = 18,
        .tilemapTop = 1 + 3 * 3,
        .width = 3,
        .height = 3,
        .paletteNum = 15,
        .baseBlock = 28,
    },
    [WIN_INFO_PURPLE_X] =
    {
        .bg = 0,
        .tilemapLeft = 18,
        .tilemapTop = 1 + 3 * 4,
        .width = 3,
        .height = 3,
        .paletteNum = 15,
        .baseBlock = 37,
    },
    
    [WIN_INFO_RED_Y] =
    {
        .bg = 0,
        .tilemapLeft = 3,
        .tilemapTop = 16,
        .width = 3,
        .height = 3,
        .paletteNum = 15,
        .baseBlock = 46,
    },
    [WIN_INFO_GREEN_Y] =
    {
        .bg = 0,
        .tilemapLeft = 3 + 3 * 1,
        .tilemapTop = 16,
        .width = 3,
        .height = 3,
        .paletteNum = 15,
        .baseBlock = 55,
    },
    [WIN_INFO_YELLOW_Y] =
    {
        .bg = 0,
        .tilemapLeft = 3 + 3 * 2,
        .tilemapTop = 16,
        .width = 3,
        .height = 3,
        .paletteNum = 15,
        .baseBlock = 64,
    },
    [WIN_INFO_BLUE_Y] =
    {
        .bg = 0,
        .tilemapLeft = 3 + 3 * 3,
        .tilemapTop = 16,
        .width = 3,
        .height = 3,
        .paletteNum = 15,
        .baseBlock = 73,
    },
    [WIN_INFO_PURPLE_Y] =
    {
        .bg = 0,
        .tilemapLeft = 3 + 3 * 4,
        .tilemapTop = 16,
        .width = 3,
        .height = 3,
        .paletteNum = 15,
        .baseBlock = 82,
    },
    [WIN_COUNT] = DUMMY_WIN_TEMPLATE,
};

#define SPRITE_SHEET_TILE_TAG 5525
#define SPRITE_SHEET_PAL_TAG 5526

// TODO - Move to CompressedSpriteSheet
static const struct SpriteSheet sVoltorbFlipSpriteSheet =
{
    .data = sVoltorbFlipSpriteSheetData,
    .size = 2048,
    .tag = SPRITE_SHEET_TILE_TAG
};

static const struct SpritePalette sVoltorbFlipPalette =
{
    .data = sVoltorbFlipPaletteSpriteData,
    .tag = SPRITE_SHEET_PAL_TAG
};

static const struct OamData sPointerSpriteOamData =
{
    .x = 0,
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = 0,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x32),
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x32),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 1,
    .affineParam = 0,
};

static const union AnimCmd sPointerSprite_PointerFrame[] =
{
    ANIMCMD_FRAME(0, 5, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sPointerSprite_PencilFrame[] =
{
    ANIMCMD_FRAME(16, 5, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sPointerSprite_PointerOutlineFrame[] =
{
    ANIMCMD_FRAME(32, 5, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sPointerSprite_PencilOutlineFrame[] =
{
    ANIMCMD_FRAME(48, 5, FALSE, FALSE),
    ANIMCMD_END
};

enum
{
    POINTER_ANIM_POINTER,
    POINTER_ANIM_PENCIL,
    POINTER_ANIM_POINTER_OUTLINE,
    POINTER_ANIM_PENCIL_OUTLINE,
    POINTER_ANIM_COUNT,
};

static const union AnimCmd *const sPencilAnimationCommands[POINTER_ANIM_COUNT] =
{
    [POINTER_ANIM_POINTER] = sPointerSprite_PointerFrame,
    [POINTER_ANIM_PENCIL] = sPointerSprite_PencilFrame,
    [POINTER_ANIM_POINTER_OUTLINE] = sPointerSprite_PointerOutlineFrame,
    [POINTER_ANIM_PENCIL_OUTLINE] = sPointerSprite_PencilOutlineFrame,
};

static const struct SpriteTemplate sPointerSprite =
{
    .tileTag = SPRITE_SHEET_TILE_TAG,
    .paletteTag = SPRITE_SHEET_PAL_TAG,
    .oam = &sPointerSpriteOamData,
    .anims = sPencilAnimationCommands,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

//static const struct OamData sPointerSpriteOamData =
//{
//    .x = 0,
//    .y = 0,
//    .affineMode = ST_OAM_AFFINE_OFF,
//    .objMode = ST_OAM_OBJ_NORMAL,
//    .mosaic = 0,
//    .bpp = ST_OAM_4BPP,
//    .shape = SPRITE_SHAPE(32x32),
//    .matrixNum = 0,
//    .size = SPRITE_SIZE(32x32),
//    .tileNum = 0,
//    .priority = 0,
//    .paletteNum = 1,
//    .affineParam = 0,
//};

void CB2_ShowVoltorbFlip(void)
{
    AGB_ASSERT(sVoltorbFlipTilemapPtr == NULL);
    AGB_ASSERT(sVoltorbFlipState == NULL);

    SetVBlankCallback(NULL);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0);
    SetGpuReg(REG_OFFSET_BG3CNT, 0);
    SetGpuReg(REG_OFFSET_BG2CNT, 0);
    SetGpuReg(REG_OFFSET_BG1CNT, 0);
    SetGpuReg(REG_OFFSET_BG0CNT, 0);
    SetGpuReg(REG_OFFSET_BG3HOFS, 0);
    SetGpuReg(REG_OFFSET_BG3VOFS, 0);
    SetGpuReg(REG_OFFSET_BG2HOFS, 0);
    SetGpuReg(REG_OFFSET_BG2VOFS, 0);
    SetGpuReg(REG_OFFSET_BG1HOFS, 0);
    SetGpuReg(REG_OFFSET_BG1VOFS, 0);
    SetGpuReg(REG_OFFSET_BG0HOFS, 0);
    SetGpuReg(REG_OFFSET_BG0VOFS, 0);
    // why doesn't this one use the dma manager either?
    DmaFill16(3, 0, VRAM, VRAM_SIZE);
    DmaFill32(3, 0, OAM, OAM_SIZE);
    DmaFill16(3, 0, PLTT, PLTT_SIZE);
    ScanlineEffect_Stop();
    ResetTasks();
    ResetSpriteData();
    ResetPaletteFade();
    FreeAllSpritePalettes();
    LoadPalette(sVoltorbFlipPalettes, 0, ARRAY_COUNT(sVoltorbFlipPalettes) * 32);
    sVoltorbFlipTilemapPtr = Alloc(0x1000);
    sVoltorbFlipState = AllocZeroed(sizeof(struct VoltorbFlipState));

    sVoltorbFlipState->cursorX = BOARD_WIDTH / 2;
    sVoltorbFlipState->cursorY = BOARD_HEIGHT / 2;
    sVoltorbFlipState->cursorWriteValue = 0;

    InitVoltorbFlipBg();
    InitVoltorbFlipSprites();
    InitVoltorbFlipWindow();
    ResetTempTileDataBuffers();
    DecompressAndCopyTileDataToVram(1, &sVoltorbFlipTiles, 0, 0, 0);
    while (FreeTempTileDataBuffersIfPossible())
        ;
    LZDecompressWram(sVoltorbFlipTilemap, sVoltorbFlipTilemapPtr);
    CopyBgTilemapBufferToVram(1);
    BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
    EnableInterrupts(1);
    SetVBlankCallback(VBlankCB);
    SetMainCallback2(MainCB2);

    ResetVoltorbFlipCards(VarGet(VAR_0x8004));

    CreateTask(Task_VoltorbFlipFadeIn, 0);
}

static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void Task_VoltorbFlipFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_VoltorbFlipWaitForKeyPress;
}

static void Task_VoltorbFlipWaitForKeyPress(u8 taskId)
{
    u8 gameState = CalculateBoardState();
    AGB_ASSERT(sVoltorbFlipState != NULL);

    if (JOY_NEW(B_BUTTON))
    {
        VarSet(VAR_RESULT, FALSE);

        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_VoltorbFlipFadeOut;
        return;
    }

    if(gameState == GAME_STATE_LOSE)
    {
        VarSet(VAR_RESULT, FALSE);

        gSprites[sVoltorbFlipState->outlineSprite].invisible = TRUE;
        gSprites[sVoltorbFlipState->pointerSprite].invisible = TRUE;
        gTasks[taskId].func = Task_VoltorbFlipLoss;
        gTasks[taskId].data[0] = 0;
    }
    else if(gameState == GAME_STATE_WIN)
    {
        ShowAllCards();
        VarSet(VAR_RESULT, TRUE);

        gSprites[sVoltorbFlipState->outlineSprite].invisible = TRUE;
        gSprites[sVoltorbFlipState->pointerSprite].invisible = TRUE;
        gTasks[taskId].func = Task_VoltorbFlipVictory;
        gTasks[taskId].data[0] = 0;
        return;
    }

    if (JOY_NEW(DPAD_LEFT))
    {
        if(sVoltorbFlipState->cursorX > 0)
        {
            PlaySE(SE_DEX_SCROLL);
            --sVoltorbFlipState->cursorX;
        }
    }
    if (JOY_NEW(DPAD_RIGHT))
    {
        if(sVoltorbFlipState->cursorX < BOARD_WIDTH - 1)
        {
            PlaySE(SE_DEX_SCROLL);
            ++sVoltorbFlipState->cursorX;
        }
    }

    if (JOY_NEW(DPAD_UP))
    {
        if(sVoltorbFlipState->cursorY > 0)
        {
            PlaySE(SE_DEX_SCROLL);
            --sVoltorbFlipState->cursorY;
        }
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        if(sVoltorbFlipState->cursorY < BOARD_HEIGHT - 1)
        {
            PlaySE(SE_DEX_SCROLL);
            ++sVoltorbFlipState->cursorY;
        }
    }

    if (JOY_NEW(L_BUTTON))
    {
        if(sVoltorbFlipState->cursorWriteValue > 0)
        {
            PlaySE(SE_SELECT);
            --sVoltorbFlipState->cursorWriteValue;
            DrawNoteTiles();
        }
    }
    if (JOY_NEW(R_BUTTON))
    {
        if(sVoltorbFlipState->cursorWriteValue < CARD_VALUE_COUNT)
        {
            PlaySE(SE_SELECT);
            ++sVoltorbFlipState->cursorWriteValue;
            DrawNoteTiles();
        }
    }

    if (JOY_NEW(A_BUTTON))
    {
        u8 cardIdx = COORDS_TO_CARD(sVoltorbFlipState->cursorX, sVoltorbFlipState->cursorY);

        if(!sVoltorbFlipState->cardStates[cardIdx].isShown)
        {
            switch (sVoltorbFlipState->cursorWriteValue)
            {
            case 0:
                if(sVoltorbFlipState->cardStates[cardIdx].value == CARD_VALUE_VOLTORB)
                    PlaySE(SE_M_EXPLOSION);
                else
                    PlaySE(SE_PIN);

                sVoltorbFlipState->cardStates[cardIdx].isShown = TRUE;
                break;

            case 1 + CARD_VALUE_1:
                PlaySE(SE_BALL);
                sVoltorbFlipState->cardStates[cardIdx].note1 = !sVoltorbFlipState->cardStates[cardIdx].note1;
                break;

            case 1 + CARD_VALUE_2:
                PlaySE(SE_BALL);
                sVoltorbFlipState->cardStates[cardIdx].note2 = !sVoltorbFlipState->cardStates[cardIdx].note2;
                break;

            case 1 + CARD_VALUE_3:
                PlaySE(SE_BALL);
                sVoltorbFlipState->cardStates[cardIdx].note3 = !sVoltorbFlipState->cardStates[cardIdx].note3;
                break;

            case 1 + CARD_VALUE_VOLTORB:
                PlaySE(SE_BALL);
                sVoltorbFlipState->cardStates[cardIdx].noteVoltorb = !sVoltorbFlipState->cardStates[cardIdx].noteVoltorb;
                break;
            }

            gSprites[sVoltorbFlipState->pointerSprite].y2 = 12;
            DrawCardTiles(sVoltorbFlipState->cursorX, sVoltorbFlipState->cursorY);
        }
    }

    UpdateVoltorbFlipSprites();
}

static void Task_VoltorbFlipVictory(u8 taskId)
{
    if(gTasks[taskId].data[0] == 0)
    {
        PlayFanfare(MUS_OBTAIN_ITEM);
        ++gTasks[taskId].data[0];
    }
    else
    {
        if(WaitFanfare(FALSE))
        {
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            gTasks[taskId].func = Task_VoltorbFlipFadeOut;
        }
    }
}

static void Task_VoltorbFlipLoss(u8 taskId)
{
    if(!IsSEPlaying())
    {
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_VoltorbFlipFadeOut;
    }
}

static void Task_VoltorbFlipFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        Free(sVoltorbFlipTilemapPtr);
        Free(sVoltorbFlipState);
        sVoltorbFlipTilemapPtr = NULL;
        sVoltorbFlipState = NULL;
        FreeAllWindowBuffers();
        DestroyTask(taskId);
        SetMainCallback2(CB2_ReturnToFieldContinueScript);
    }
}

static void ResetVoltorbFlipCards(u8 level)
{
    u8 i, idx;
    u8 variant = Random() % MAX_VOLTORB_LEVEL_VARIANTS;
    AGB_ASSERT(sVoltorbFlipState != NULL);

    if(level != 0)
        level = min(level, MAX_VOLTORB_FLIP_LEVEL) - 1;

    // Reset
    for(i = 0; i < TOTAL_CARD_COUNT; ++i)
    {
        memset(&sVoltorbFlipState->cardStates[i], 0, sizeof(struct VoltorbCardState));
        sVoltorbFlipState->cardStates[i].value = CARD_VALUE_1;
    }

    // Place voltorbs
    for(i = 0; i < sVoltorbSpawnCounts[level][variant].voltorbCount;)
    {
        idx = Random() % TOTAL_CARD_COUNT;
        if(sVoltorbFlipState->cardStates[idx].value == CARD_VALUE_1)
        {
            sVoltorbFlipState->cardStates[idx].value = CARD_VALUE_VOLTORB;
            ++i;
        }
    }

    // x2
    for(i = 0; i < sVoltorbSpawnCounts[level][variant].x2Count;)
    {
        idx = Random() % TOTAL_CARD_COUNT;
        if(sVoltorbFlipState->cardStates[idx].value == CARD_VALUE_1)
        {
            sVoltorbFlipState->cardStates[idx].value = CARD_VALUE_2;
            ++i;
        }
    }

    // x3
    for(i = 0; i < sVoltorbSpawnCounts[level][variant].x3Count;)
    {
        idx = Random() % TOTAL_CARD_COUNT;
        if(sVoltorbFlipState->cardStates[idx].value == CARD_VALUE_1)
        {
            sVoltorbFlipState->cardStates[idx].value = CARD_VALUE_3;
            ++i;
        }
    }

    DisplayVoltorbFlipText();
    DrawBoardCardTiles();
    DrawNoteTiles();
}

static u8 CalculateBoardState()
{
    u8 i;
    bool8 inProgress = FALSE;

    for(i = 0; i < TOTAL_CARD_COUNT; ++i)
    {
        if(sVoltorbFlipState->cardStates[i].isShown)
        {
            if(sVoltorbFlipState->cardStates[i].value == CARD_VALUE_VOLTORB)
                return GAME_STATE_LOSE;
        }
        else
        {
            if(sVoltorbFlipState->cardStates[i].value == CARD_VALUE_2 || sVoltorbFlipState->cardStates[i].value == CARD_VALUE_3)
                inProgress = TRUE;
        }
    }

    return inProgress ? GAME_STATE_IN_PROGRESS : GAME_STATE_WIN;
}

static void ShowAllCards()
{
    u8 x, y;

    for(x = 0; x < BOARD_WIDTH; ++x)
    for(y = 0; y < BOARD_HEIGHT; ++y)
    {
        sVoltorbFlipState->cardStates[COORDS_TO_CARD(x, y)].isShown = TRUE;
        DrawCardTiles(x, y);
    }
}

static const u8 sTextWhite[] = _("{COLOR WHITE}{SHADOW DARK_GRAY}");
static const u8 sTextGrey[] = _("{COLOR LIGHT_GRAY}{SHADOW DARK_GRAY}");

static void PrintRowInfo(u8 window, u8 totalValue, u8 totalVoltorbs)
{
    u8 text[16];
    FillWindowPixelBuffer(window, PIXEL_FILL(0));

    // Top number
    ConvertUIntToDecimalStringN(gStringVar4, totalValue, STR_CONV_MODE_RIGHT_ALIGN, 2);
    StringCopy(text, sTextWhite);
    StringAppend(text, gStringVar4);
    PrintVoltorbFlipText(window, FONT_SMALL_NARROW, text, 10, 0);
    
    // Bottom number
    ConvertUIntToDecimalStringN(gStringVar4, totalVoltorbs, STR_CONV_MODE_RIGHT_ALIGN, 1);
    StringCopy(text, sTextGrey);
    StringAppend(text, gStringVar4);
    PrintVoltorbFlipText(window, FONT_SMALL_NARROW, text, 15, 8);

    PutWindowTilemap(window);
    CopyWindowToVram(window, COPYWIN_FULL);
}

static u8 CountRow(u8 y, bool8 countValue)
{
    u8 x;
    u8 total = 0;
    for(x = 0; x < BOARD_WIDTH; ++x)
    {
        if(countValue)
        {
            if(sVoltorbFlipState->cardStates[COORDS_TO_CARD(x, y)].value != CARD_VALUE_VOLTORB)
            {
                total += sVoltorbFlipState->cardStates[COORDS_TO_CARD(x, y)].value + 1;
            }
        }
        else
        {
            if(sVoltorbFlipState->cardStates[COORDS_TO_CARD(x, y)].value == CARD_VALUE_VOLTORB)
            {
                total += 1;
            }
        }
    }

    return total;
}

static u8 CountColumn(u8 x, bool8 countValue)
{
    u8 y;
    u8 total = 0;
    for(y = 0; y < BOARD_HEIGHT; ++y)
    {
        if(countValue)
        {
            if(sVoltorbFlipState->cardStates[COORDS_TO_CARD(x, y)].value != CARD_VALUE_VOLTORB)
            {
                total += sVoltorbFlipState->cardStates[COORDS_TO_CARD(x, y)].value + 1;
            }
        }
        else
        {
            if(sVoltorbFlipState->cardStates[COORDS_TO_CARD(x, y)].value == CARD_VALUE_VOLTORB)
            {
                total += 1;
            }
        }
    }

    return total;
}

static void DisplayVoltorbFlipText(void)
{
    u32 i;

    for(i = 0; i < BOARD_HEIGHT; ++i)
    {
        PrintRowInfo(
            WIN_INFO_RED_X + i, 
            CountRow(i, TRUE),
            CountRow(i, FALSE)
        );
    }

    for(i = 0; i < BOARD_WIDTH; ++i)
    {
        PrintRowInfo(
            WIN_INFO_RED_Y + i, 
            CountColumn(i, TRUE),
            CountColumn(i, FALSE)
        );
    }
}

static void DrawBoardCardTiles(void)
{
    u8 x, y;
    for(x = 0; x < BOARD_WIDTH; ++x)
    {
        for(y = 0; y < BOARD_HEIGHT; ++y)
            DrawCardTiles(x, y);
    }
}

static void InitVoltorbFlipBg(void)
{
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sVoltorbFlipBgTemplates, ARRAY_COUNT(sVoltorbFlipBgTemplates));
    SetBgTilemapBuffer(1, sVoltorbFlipTilemapPtr);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
    ShowBg(0);
    ShowBg(1);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_BLDALPHA, 0);
    SetGpuReg(REG_OFFSET_BLDY, 0);
}

static void UpdateVoltorbFlipSprites()
{
    AGB_ASSERT(sVoltorbFlipState != NULL);
    gSprites[sVoltorbFlipState->outlineSprite].x = 24 + 16 + 24 * sVoltorbFlipState->cursorX;
    gSprites[sVoltorbFlipState->outlineSprite].y = 8 + 16 + 24 * sVoltorbFlipState->cursorY;

    gSprites[sVoltorbFlipState->pointerSprite].x = 24 + 12 + 24 * sVoltorbFlipState->cursorX;
    gSprites[sVoltorbFlipState->pointerSprite].y = 8 + 0 + 24 * sVoltorbFlipState->cursorY;

    if(gSprites[sVoltorbFlipState->pointerSprite].y2 > 0)
    {
        if(gSprites[sVoltorbFlipState->pointerSprite].y2 < 2)
            gSprites[sVoltorbFlipState->pointerSprite].y2 = 0;
        else
            gSprites[sVoltorbFlipState->pointerSprite].y2 -= 2;
    }

    if(sVoltorbFlipState->cursorWriteValue == 0)
    {
        StartSpriteAnimIfDifferent(&gSprites[sVoltorbFlipState->outlineSprite], POINTER_ANIM_POINTER_OUTLINE);
        StartSpriteAnimIfDifferent(&gSprites[sVoltorbFlipState->pointerSprite], POINTER_ANIM_POINTER);
    }
    else
    {
        StartSpriteAnimIfDifferent(&gSprites[sVoltorbFlipState->outlineSprite], POINTER_ANIM_PENCIL_OUTLINE);
        StartSpriteAnimIfDifferent(&gSprites[sVoltorbFlipState->pointerSprite], POINTER_ANIM_PENCIL);
    }
}

static void InitVoltorbFlipSprites(void)
{
    AGB_ASSERT(sVoltorbFlipState != NULL);

    LoadSpriteSheet(&sVoltorbFlipSpriteSheet);
    LoadSpritePalette(&sVoltorbFlipPalette);

    sVoltorbFlipState->outlineSprite = CreateSprite(&sPointerSprite, 0, 0, 1);
    sVoltorbFlipState->pointerSprite = CreateSprite(&sPointerSprite, 24 + 12, 8 + 0, 0);
    UpdateVoltorbFlipSprites();
}

static void InitVoltorbFlipWindow(void)
{
    InitWindows(sVoltorbFlipWinTemplates);
    DeactivateAllTextPrinters();
    LoadPalette(gStandardMenuPalette, 0xF0, 0x20);

    FillWindowPixelBuffer(WIN_INFO_RED_X, PIXEL_FILL(0));
    PutWindowTilemap(WIN_INFO_RED_X);
}

static void PrintVoltorbFlipText(u8 win, u8 font, u8 const* text, u8 x, u8 y)
{
    u8 color[3] = {0, 2, 3};

    AddTextPrinterParameterized4(win, font, x, y, 0, 0, color, TEXT_SKIP_DRAW, text);
}

//static void A()
//{
//    FillWindowPixelBuffer(WIN_TITLE_COUNTERS, PIXEL_FILL(0));
//    PutWindowTilemap(WIN_TITLE_COUNTERS);
//
//    CopyWindowToVram(WIN_TITLE_COUNTERS, COPYWIN_FULL);
//}

static void DrawCardTiles(u8 x, u8 y)
{
    u8 const xOffset = 3;
    u8 const yOffset = 1;
    struct VoltorbCardState* state = &sVoltorbFlipState->cardStates[COORDS_TO_CARD(x, y)];
    u8 i, gfx;

    AGB_ASSERT(sVoltorbFlipState != NULL);

    if(state->isShown)
        gfx = CARD_GFX_SHOW_1 + state->value;
    else
        gfx = CARD_GFX_HIDDEN;

    for(i = 0; i < 9; ++i)
        FillBgTilemapBufferRect_Palette0(1, sCardGfxTiles[gfx][i], xOffset + x * 3 + (i % 3), yOffset + y * 3 + (i / 3), 1, 1);

    if(!state->isShown)
    {
        if(state->noteVoltorb)
            FillBgTilemapBufferRect_Palette0(1, 0x23, xOffset + x * 3 + 0, yOffset + y * 3 + 0, 1, 1);

        if(state->note1)
            FillBgTilemapBufferRect_Palette0(1, 0x24, xOffset + x * 3 + 2, yOffset + y * 3 + 0, 1, 1);

        if(state->note2)
            FillBgTilemapBufferRect_Palette0(1, 0x3A, xOffset + x * 3 + 0, yOffset + y * 3 + 2, 1, 1);

        if(state->note3)
            FillBgTilemapBufferRect_Palette0(1, 0x3B, xOffset + x * 3 + 2, yOffset + y * 3 + 2, 1, 1);
    }

    ScheduleBgCopyTilemapToVram(1);
}


static void DrawNoteTiles()
{
    AGB_ASSERT(sVoltorbFlipState != NULL);

    FillBgTilemapBufferRect_Palette0(1, 
            (sVoltorbFlipState->cursorWriteValue == 0) ? 0x38 : 0x39,
            22 + 0, 5, 
            1, 1
    );

    FillBgTilemapBufferRect_Palette0(1, 
            (sVoltorbFlipState->cursorWriteValue == 1 + CARD_VALUE_1) ? 0x37 : 0x39,
            22 + 1, 5, 
            1, 1
    );

    FillBgTilemapBufferRect_Palette0(1, 
            (sVoltorbFlipState->cursorWriteValue == 1 + CARD_VALUE_2) ? 0x37 : 0x39,
            22 + 2, 5, 
            1, 1
    );

    FillBgTilemapBufferRect_Palette0(1, 
            (sVoltorbFlipState->cursorWriteValue == 1 + CARD_VALUE_3) ? 0x37 : 0x39,
            22 + 3, 5, 
            1, 1
    );

    FillBgTilemapBufferRect_Palette0(1, 
            (sVoltorbFlipState->cursorWriteValue == 1 + CARD_VALUE_VOLTORB) ? 0x37 : 0x39,
            22 + 4, 5, 
            1, 1
    );

    ScheduleBgCopyTilemapToVram(1);
}