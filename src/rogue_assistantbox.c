#include "global.h"
#include "palette.h"
#include "random.h"
#include "main.h"
#include "gpu_regs.h"
#include "scanline_effect.h"
#include "sound.h"
#include "task.h"
#include "malloc.h"
#include "decompress.h"
#include "bg.h"
#include "window.h"
#include "string_util.h"
#include "text.h"
#include "text_window.h"
#include "overworld.h"
#include "menu.h"
#include "pokedex.h"
#include "pokemon_storage_system.h"
#include "constants/rgb.h"
#include "constants/songs.h"

#include "rogue_assistant.h"
#include "rogue_assistantbox.h"


#define SPRITE_SHEET_TILE_TAG 5525
#define SPRITE_SHEET_PAL_TAG 5526

enum PaletteIds
{
    BG_PAL_ID_BACKGROUND,
    BG_PAL_ID_WINDOW_FRAMES,
    BG_PAL_ID_STD_MENU = 15,
};

static const u8 sGenericSpriteSheetData[] = INCBIN_U8("graphics/rogue_assistantbox/sprites.4bpp");
static const u16 sGenericPaletteSpriteData[] = INCBIN_U16("graphics/rogue_assistantbox/sprites.gbapal");

// TODO - Move to CompressedSpriteSheet
static const struct SpriteSheet sGenericSpriteSheet =
{
    .data = sGenericSpriteSheetData,
    .size = 512 * 8,
    .tag = SPRITE_SHEET_TILE_TAG
};

static const struct SpritePalette sGenericPalette =
{
    .data = sGenericPaletteSpriteData,
    .tag = SPRITE_SHEET_PAL_TAG
};

static const struct OamData sGenericSpriteOamData =
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
    .priority = 1,
    .paletteNum = 1,
    .affineParam = 0,
};

static const union AnimCmd sGenericSprite_PointerFrame[] =
{
    ANIMCMD_FRAME(16 * 0, 5, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sGenericSprite_PointerGrabbingFrame[] =
{
    ANIMCMD_FRAME(16 * 1, 5, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sGenericSprite_PointerBoxEmpty[] =
{
    ANIMCMD_FRAME(16 * 2, 5, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sGenericSprite_PointerBoxFilled0[] =
{
    ANIMCMD_FRAME(16 * 3, 5, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sGenericSprite_PointerBoxFilled1[] =
{
    ANIMCMD_FRAME(16 * 4, 5, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sGenericSprite_PointerBoxFilled2[] =
{
    ANIMCMD_FRAME(16 * 5, 5, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sGenericSprite_PointerBoxFilled3[] =
{
    ANIMCMD_FRAME(16 * 6, 5, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sGenericSprite_PointerBoxFilled4[] =
{
    ANIMCMD_FRAME(16 * 7, 5, FALSE, FALSE),
    ANIMCMD_END
};

enum
{
    SPRITE_ANIM_POINTER,
    SPRITE_ANIM_POINTER_GRABBING,
    SPRITE_ANIM_BOX_EMPTY,
    SPRITE_ANIM_BOX_FILLED_0,
    SPRITE_ANIM_BOX_FILLED_1,
    SPRITE_ANIM_BOX_FILLED_2,
    SPRITE_ANIM_BOX_FILLED_3,
    SPRITE_ANIM_BOX_FILLED_4,
    SPRITE_ANIM_COUNT,
};

static const union AnimCmd *const sGenericAnimationCommands[SPRITE_ANIM_COUNT] =
{
    [SPRITE_ANIM_POINTER] = sGenericSprite_PointerFrame,
    [SPRITE_ANIM_POINTER_GRABBING] = sGenericSprite_PointerGrabbingFrame,
    [SPRITE_ANIM_BOX_EMPTY] = sGenericSprite_PointerBoxEmpty,
    [SPRITE_ANIM_BOX_FILLED_0] = sGenericSprite_PointerBoxFilled0,
    [SPRITE_ANIM_BOX_FILLED_1] = sGenericSprite_PointerBoxFilled1,
    [SPRITE_ANIM_BOX_FILLED_2] = sGenericSprite_PointerBoxFilled2,
    [SPRITE_ANIM_BOX_FILLED_3] = sGenericSprite_PointerBoxFilled3,
    [SPRITE_ANIM_BOX_FILLED_4] = sGenericSprite_PointerBoxFilled4,
};

static void GenericBoxSpriteCallback(struct Sprite *sprite);
static void GenericPointerSpriteCallback(struct Sprite *sprite);
static void SetGenericSpriteDesiredPosition(u8 spriteId, u8 x, u8 y);
static void SnapGenericSpriteToPosition(u8 spriteId);

static const struct SpriteTemplate sGenericBoxSprite =
{
    .tileTag = SPRITE_SHEET_TILE_TAG,
    .paletteTag = SPRITE_SHEET_PAL_TAG,
    .oam = &sGenericSpriteOamData,
    .anims = sGenericAnimationCommands,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = GenericBoxSpriteCallback
};

static const struct SpriteTemplate sGenericPointerSprite =
{
    .tileTag = SPRITE_SHEET_TILE_TAG,
    .paletteTag = SPRITE_SHEET_PAL_TAG,
    .oam = &sGenericSpriteOamData,
    .anims = sGenericAnimationCommands,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = GenericPointerSpriteCallback
};

static const struct BgTemplate sAssitantBoxBgTemplates[2] =
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

enum
{
    WIN_DESC,
    WIN_COUNT
};

static const struct WindowTemplate sAssitantBoxWinTemplates[WIN_COUNT + 1] =
{
    [WIN_DESC] = 
    {
        .bg = 0,
        .tilemapLeft = 5,
        .tilemapTop = 18,
        .width = 20,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 1,
    },
    DUMMY_WIN_TEMPLATE,
};

#define FREE_BLOCK_START (1 + (20 * 16))

#define TOTAL_BOX_ROW_COUNT 6
#define INVALID_IDX (255)

STATIC_ASSERT((TOTAL_BOXES_COUNT * TOTAL_BOX_ROW_COUNT) == ASSISTANT_HOME_TOTAL_BOXES, SizeOfAssistantBoxesMatchExpectedUICounts);

#define COORD_TO_IDX(x, y) (y * TOTAL_BOXES_COUNT + x)

struct AssistantBoxState
{
    u8 cursorX;
    u8 cursorY;
    u8 pointerSprite;
    u8 currentHoldIdx;
    u8 boxSprites[TOTAL_BOXES_COUNT * TOTAL_BOX_ROW_COUNT];
};

EWRAM_DATA static struct AssistantBoxState *sAssistantBoxState = NULL;

static void MainCB2(void);
static void Task_AssitantBoxFadeIn(u8);
static void Task_AssitantBoxWaitForReady(u8);
static void Task_AssitantBoxErrorWaitForExit(u8);
static void Task_AssitantBoxWaitForKeyPress(u8);
static void Task_AssitantBoxFadeOut(u8);
static void DisplayAssitantActiveBoxText(void);
static void DisplayAssitantLoadingText(void);
static void DisplayAssitantDisconnectedText(void);
static void InitAssitantBoxBg(void);
static void InitAssitantBoxWindow(void);
static void InitAssitantBoxSprites(void);
static void PrintAssitantBoxText(u8 const*, u8, u8);
static void DrawBgWindowFrames(void);

EWRAM_DATA static u8 *sAssitantBoxTilemapPtr = NULL;

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static const u16 sAssitantBoxPalettes[][16] =
{
    INCBIN_U16("graphics/rogue_assistantbox/background.gbapal"),
    //INCBIN_U16("graphics/diploma/hoenn.gbapal"),
};

static const u32 sAssitantBoxTilemap[] = INCBIN_U32("graphics/rogue_assistantbox/background.bin.lz");
static const u32 sAssitantBoxTiles[] = INCBIN_U32("graphics/rogue_assistantbox/background.4bpp.lz");

void CB2_ShowAssitantBoxView(void)
{
    RogueBox_OpenConnection();

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
    LoadPalette(sAssitantBoxPalettes, BG_PLTT_ID(BG_PAL_ID_BACKGROUND), sizeof(sAssitantBoxPalettes));
    sAssitantBoxTilemapPtr = malloc(0x1000);
    sAssistantBoxState = AllocZeroed(sizeof(struct AssistantBoxState));
    sAssistantBoxState->currentHoldIdx = INVALID_IDX;

    InitAssitantBoxBg();
    InitAssitantBoxSprites();
    InitAssitantBoxWindow();
    ResetTempTileDataBuffers();
    DecompressAndCopyTileDataToVram(1, &sAssitantBoxTiles, 0, 0, 0);
    while (FreeTempTileDataBuffersIfPossible())
        ;
    LZDecompressWram(sAssitantBoxTilemap, sAssitantBoxTilemapPtr);
    LoadBgTiles(0, GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->tiles, 0x120, FREE_BLOCK_START);
    LoadPalette(GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->pal, BG_PLTT_ID(BG_PAL_ID_WINDOW_FRAMES), PLTT_SIZE_4BPP);
    CopyBgTilemapBufferToVram(1);

    DisplayAssitantLoadingText();
    BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
    EnableInterrupts(1);
    SetVBlankCallback(VBlankCB);
    SetMainCallback2(MainCB2);
    CreateTask(Task_AssitantBoxFadeIn, 0);
}

static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();

    // Custom anim
    if(sAssistantBoxState != NULL)
    {
        if(sAssistantBoxState->currentHoldIdx != INVALID_IDX)
        {
            u8 boxSprite = sAssistantBoxState->boxSprites[sAssistantBoxState->currentHoldIdx];

            gSprites[boxSprite].x = gSprites[sAssistantBoxState->pointerSprite].x;
            gSprites[boxSprite].y = gSprites[sAssistantBoxState->pointerSprite].y - 4;
        }
    }

    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void Task_AssitantBoxFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_AssitantBoxWaitForReady;
}

static void Task_AssitantBoxWaitForReady(u8 taskId)
{
    if(JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        
        // Exit
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_AssitantBoxFadeOut;
            PlaySE(SE_PC_OFF);
    }
    else if(RogueBox_IsConnectedAndReady())
    {
        PlaySE(SE_PC_ON);
        DisplayAssitantActiveBoxText();
        gTasks[taskId].func = Task_AssitantBoxWaitForKeyPress;
    }
}

static void Task_AssitantBoxErrorWaitForExit(u8 taskId)
{
    if(JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);

        // Exit
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_AssitantBoxFadeOut;
            PlaySE(SE_PC_OFF);
    }
}

static void Task_AssitantBoxWaitForKeyPress(u8 taskId)
{
    u8 prevCurX = sAssistantBoxState->cursorX;
    u8 prevCurY = sAssistantBoxState->cursorY;
    u8 prevHoldIdx = sAssistantBoxState->currentHoldIdx;
    bool32 doSnap = FALSE;

    AGB_ASSERT(sAssistantBoxState);

    if(!RogueBox_IsConnectedAndReady())
    {
        DisplayAssitantDisconnectedText();
        gTasks[taskId].func = Task_AssitantBoxErrorWaitForExit;
        return;
    }

    if(JOY_REPEAT(DPAD_LEFT))
    {
        if(sAssistantBoxState->cursorX == 0)
        {
            sAssistantBoxState->cursorX = TOTAL_BOXES_COUNT - 1;
            doSnap = TRUE;
        }
        else
            --sAssistantBoxState->cursorX;
        
    }
    if(JOY_REPEAT(DPAD_RIGHT))
    {
        if(sAssistantBoxState->cursorX == TOTAL_BOXES_COUNT - 1)
        {
            sAssistantBoxState->cursorX = 0;
            doSnap = TRUE;
        }
        else
            ++sAssistantBoxState->cursorX;
    }
    if(JOY_REPEAT(DPAD_UP))
    {
        if(sAssistantBoxState->cursorY == 0)
        {
            sAssistantBoxState->cursorY = TOTAL_BOX_ROW_COUNT - 1;
            doSnap = TRUE;
        }
        else
            --sAssistantBoxState->cursorY;
    }
    if(JOY_REPEAT(DPAD_DOWN))
    {
        if(sAssistantBoxState->cursorY == TOTAL_BOX_ROW_COUNT - 1)
        {
            sAssistantBoxState->cursorY = 0;
            doSnap = TRUE;
        }
        else
            ++sAssistantBoxState->cursorY;
    }

    if(prevCurX != sAssistantBoxState->cursorX || prevCurY != sAssistantBoxState->cursorY)
    {
        PlaySE(SE_SELECT);
        DisplayAssitantActiveBoxText();
    }

    if(JOY_NEW(A_BUTTON))
    {
        u8 hoverIdx = COORD_TO_IDX(sAssistantBoxState->cursorX, sAssistantBoxState->cursorY);
        PlaySE(SE_SELECT);

        if(sAssistantBoxState->currentHoldIdx == hoverIdx)
        {
            // Place box back down
            sAssistantBoxState->currentHoldIdx = INVALID_IDX;
        }
        else if(sAssistantBoxState->currentHoldIdx != INVALID_IDX)
        {
            u8 topSprite = sAssistantBoxState->boxSprites[sAssistantBoxState->currentHoldIdx];
            u8 bottomSprite = sAssistantBoxState->boxSprites[hoverIdx];

            // Swap both boxes
            RogueBox_SwapBoxes(sAssistantBoxState->currentHoldIdx, hoverIdx);

            // Offset to play a little place anim
            gSprites[bottomSprite].y -= 24;
        }
        else
        {
            // Pickup new box
            sAssistantBoxState->currentHoldIdx = hoverIdx;
        }

        AGB_ASSERT(sAssistantBoxState->currentHoldIdx != INVALID_IDX);

        if(RogueBox_GetCountInBox(sAssistantBoxState->currentHoldIdx) == 0)
        {
            // Place box back, as no need to pickup empty boxes
            sAssistantBoxState->currentHoldIdx = INVALID_IDX;
        }
    }

    if(JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        if(sAssistantBoxState->currentHoldIdx != INVALID_IDX)
        {
            // Release current box
            sAssistantBoxState->currentHoldIdx = INVALID_IDX;
        }
        else
        {
            // Exit
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            gTasks[taskId].func = Task_AssitantBoxFadeOut;
            PlaySE(SE_PC_OFF);
        }
    }

    // Update Cursor
    SetGenericSpriteDesiredPosition(sAssistantBoxState->pointerSprite, sAssistantBoxState->cursorX, sAssistantBoxState->cursorY);

    if(doSnap)
        SnapGenericSpriteToPosition(sAssistantBoxState->pointerSprite);

    if(sAssistantBoxState->currentHoldIdx == INVALID_IDX)
        StartSpriteAnimIfDifferent(&gSprites[sAssistantBoxState->pointerSprite], SPRITE_ANIM_POINTER);
    else
        StartSpriteAnimIfDifferent(&gSprites[sAssistantBoxState->pointerSprite], SPRITE_ANIM_POINTER_GRABBING);


    // Update held box
    if(prevHoldIdx != sAssistantBoxState->currentHoldIdx)
    {
        // Place in front
        if(sAssistantBoxState->currentHoldIdx != INVALID_IDX)
            gSprites[sAssistantBoxState->boxSprites[sAssistantBoxState->currentHoldIdx]].subpriority = 0;
        
        // Place in background
        if(prevHoldIdx != INVALID_IDX)
            gSprites[sAssistantBoxState->boxSprites[prevHoldIdx]].subpriority = 2;
    }
}

static void Task_AssitantBoxFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        ResetSpriteData();
        Free(sAssitantBoxTilemapPtr);
        Free(sAssistantBoxState);
        sAssistantBoxState = NULL;
        FreeAllWindowBuffers();
        DestroyTask(taskId);

        RogueBox_CloseConnection();
        SetMainCallback2(CB2_ReturnToFieldContinueScript);
    }
}

static u8 const sText_Loading[] = _("Loading data…");
static u8 const sText_Error[] = _("Disconnected…");
static u8 const sText_BtoExit[] = _("{B_BUTTON} Exit");

static u8 const sText_PC[] = _("PC - ");
static u8 const sText_Storage[] = _("Storage - ");
static u8 const sText_Pkmn[] = _("{PKMN}");

static void DisplayAssitantLoadingText(void)
{
    u8* ptr;
    u8 hoverBoxIdx = COORD_TO_IDX(sAssistantBoxState->cursorX, sAssistantBoxState->cursorY);

    AGB_ASSERT(sAssistantBoxState != NULL);
    SetGpuReg(REG_OFFSET_BG1HOFS, 0);

    FillWindowPixelBuffer(WIN_DESC, PIXEL_FILL(TEXT_COLOR_WHITE));

    PrintAssitantBoxText(sText_Loading, 0, 1);
    PrintAssitantBoxText(sText_BtoExit, 120, 1);

    PutWindowTilemap(0);
    CopyWindowToVram(0, COPYWIN_FULL);
}

static void DisplayAssitantDisconnectedText(void)
{
    u8* ptr;
    u8 hoverBoxIdx = COORD_TO_IDX(sAssistantBoxState->cursorX, sAssistantBoxState->cursorY);

    AGB_ASSERT(sAssistantBoxState != NULL);
    SetGpuReg(REG_OFFSET_BG1HOFS, 0);

    FillWindowPixelBuffer(WIN_DESC, PIXEL_FILL(TEXT_COLOR_WHITE));

    PrintAssitantBoxText(sText_Error, 0, 1);
    PrintAssitantBoxText(sText_BtoExit, 120, 1);

    PutWindowTilemap(0);
    CopyWindowToVram(0, COPYWIN_FULL);
}

static void DisplayAssitantActiveBoxText(void)
{
    u8* ptr;
    u8 hoverBoxIdx = COORD_TO_IDX(sAssistantBoxState->cursorX, sAssistantBoxState->cursorY);

    AGB_ASSERT(sAssistantBoxState != NULL);
    SetGpuReg(REG_OFFSET_BG1HOFS, 0);

    FillWindowPixelBuffer(WIN_DESC, PIXEL_FILL(TEXT_COLOR_WHITE));

    // Source + Name
    if(RogueBox_IsLocalBox(hoverBoxIdx))
        ptr = StringCopy(gStringVar4, sText_PC);
    else
        ptr = StringCopy(gStringVar4, sText_Storage);

    ptr = StringCopyN(ptr, RogueBox_GetName(hoverBoxIdx), BOX_NAME_LENGTH);
    PrintAssitantBoxText(gStringVar4, 0, 1);

    // Count in back
    ptr = ConvertIntToDecimalStringN(gStringVar4, RogueBox_GetCountInBox(hoverBoxIdx), STR_CONV_MODE_RIGHT_ALIGN, 2);
    ptr = StringAppend(ptr, sText_Pkmn);
    PrintAssitantBoxText(gStringVar4, 128, 1);

    PutWindowTilemap(0);
    CopyWindowToVram(0, COPYWIN_FULL);
}

static void InitAssitantBoxBg(void)
{
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sAssitantBoxBgTemplates, ARRAY_COUNT(sAssitantBoxBgTemplates));
    SetBgTilemapBuffer(1, sAssitantBoxTilemapPtr);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
    ShowBg(0);
    ShowBg(1);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_BLDALPHA, 0);
    SetGpuReg(REG_OFFSET_BLDY, 0);
}


static void InitAssitantBoxSprites(void)
{
    u32 x, y;

    AGB_ASSERT(sAssistantBoxState != NULL);

    LoadSpriteSheet(&sGenericSpriteSheet);
    LoadSpritePalette(&sGenericPalette);

    for(x = 0; x < TOTAL_BOXES_COUNT; ++x)
    {
        for(y = 0; y < TOTAL_BOX_ROW_COUNT; ++y)
        {
            u8 sprite = CreateSprite(&sGenericBoxSprite, 0, 0, 2);
            SetGenericSpriteDesiredPosition(sprite, x, y);
            SnapGenericSpriteToPosition(sprite);

            StartSpriteAnimIfDifferent(&gSprites[sprite], SPRITE_ANIM_BOX_EMPTY);
            sAssistantBoxState->boxSprites[COORD_TO_IDX(x, y)] = sprite;
        }
    }

    sAssistantBoxState->pointerSprite = CreateSprite(&sGenericPointerSprite, 0, 0, 0);
    StartSpriteAnimIfDifferent(&gSprites[sAssistantBoxState->pointerSprite], SPRITE_ANIM_POINTER);
}

static void InitAssitantBoxWindow(void)
{
    InitWindows(sAssitantBoxWinTemplates);
    DeactivateAllTextPrinters();
    LoadPalette(gStandardMenuPalette, BG_PLTT_ID(BG_PAL_ID_STD_MENU), PLTT_SIZE_4BPP);
    FillWindowPixelBuffer(WIN_DESC, PIXEL_FILL(TEXT_COLOR_WHITE));
    PutWindowTilemap(WIN_DESC);
    DrawBgWindowFrames();
}

static void PrintAssitantBoxText(u8 const* text, u8 var1, u8 var2)
{
    u8 color[3] = {0, 2, 3};

    AddTextPrinterParameterized4(0, FONT_NORMAL, var1, var2, 0, 0, color, TEXT_SKIP_DRAW, text);
}

static void SetGenericSpriteDesiredPosition(u8 spriteId, u8 x, u8 y)
{
    if(spriteId != SPRITE_NONE)
    {
        gSprites[spriteId].data[0] = 12 + 24 * x;
        gSprites[spriteId].data[1] = 12 + 24 * y;
        gSprites[spriteId].data[2] = x;
        gSprites[spriteId].data[3] = y;
    }
}

static void SnapGenericSpriteToPosition(u8 spriteId)
{
    if(spriteId != SPRITE_NONE)
    {
        gSprites[spriteId].x = gSprites[spriteId].data[0];
        gSprites[spriteId].y = gSprites[spriteId].data[1];
    }
}

static void AnimateGenericToDesiredCoords(struct Sprite *sprite, u8 speed)
{
    u8 i;
    bool8 loop = TRUE;

    for(i = 0; loop && i < 4; ++i)
    {
        loop = FALSE;

        if(sprite->x < sprite->data[0])
        {
            ++sprite->x;
            loop = TRUE;
        }
        else if(sprite->x > sprite->data[0])
        {
            --sprite->x;
            loop = TRUE;
        }

        if(sprite->y < sprite->data[1])
        {
            ++sprite->y;
            loop = TRUE;
        }
        else if(sprite->y > sprite->data[1])
        {
            --sprite->y;
            loop = TRUE;
        }
    }
}

static void GenericBoxSpriteCallback(struct Sprite *sprite)
{
    u8 boxIdx = COORD_TO_IDX(sprite->data[2], sprite->data[3]);
    u8 monCount = RogueBox_GetCountInBox(boxIdx);
    //AnimateGenericToDesiredCoords(sprite, 8);

    // Snap to coords
    sprite->x = sprite->data[0];
    sprite->y = sprite->data[1];

    if(monCount >= 30)
        StartSpriteAnimIfDifferent(sprite, SPRITE_ANIM_BOX_FILLED_4);
    else if(monCount >= 23)
        StartSpriteAnimIfDifferent(sprite, SPRITE_ANIM_BOX_FILLED_3);
    else if(monCount >= 17)
        StartSpriteAnimIfDifferent(sprite, SPRITE_ANIM_BOX_FILLED_3);
    else if(monCount >= 11)
        StartSpriteAnimIfDifferent(sprite, SPRITE_ANIM_BOX_FILLED_2);
    else if(monCount >= 5)
        StartSpriteAnimIfDifferent(sprite, SPRITE_ANIM_BOX_FILLED_1);
    else if(monCount > 0)
        StartSpriteAnimIfDifferent(sprite, SPRITE_ANIM_BOX_FILLED_0);
    else
        StartSpriteAnimIfDifferent(sprite, SPRITE_ANIM_BOX_EMPTY);
}

static void GenericPointerSpriteCallback(struct Sprite *sprite)
{
    AnimateGenericToDesiredCoords(sprite, 4);
}

#define TILE_TOP_CORNER_L (FREE_BLOCK_START + 0x1A2 - 0x1A2)
#define TILE_TOP_EDGE     (FREE_BLOCK_START + 0x1A3 - 0x1A2)
#define TILE_TOP_CORNER_R (FREE_BLOCK_START + 0x1A4 - 0x1A2)
#define TILE_LEFT_EDGE    (FREE_BLOCK_START + 0x1A5 - 0x1A2)
#define TILE_RIGHT_EDGE   (FREE_BLOCK_START + 0x1A7 - 0x1A2)
#define TILE_BOT_CORNER_L (FREE_BLOCK_START + 0x1A8 - 0x1A2)
#define TILE_BOT_EDGE     (FREE_BLOCK_START + 0x1A9 - 0x1A2)
#define TILE_BOT_CORNER_R (FREE_BLOCK_START + 0x1AA - 0x1A2)

static void DrawBgWindowFrames(void)
{
    u8 i, left, right, top, bottom, bg;
    bool8 leftValid, rightValid, topValid, bottomValid;

    for(i = 0; i < ARRAY_COUNT(sAssitantBoxWinTemplates); ++i)
    {
        bg = sAssitantBoxWinTemplates[i].bg;

        left = sAssitantBoxWinTemplates[i].tilemapLeft - 1;
        right = sAssitantBoxWinTemplates[i].tilemapLeft + sAssitantBoxWinTemplates[i].width;
        top = sAssitantBoxWinTemplates[i].tilemapTop - 1;
        bottom = sAssitantBoxWinTemplates[i].tilemapTop + sAssitantBoxWinTemplates[i].height;

        leftValid = left <= 29 && left <= right;
        rightValid = right <= 29 && right >= left;
        topValid = top <= 19 && top <= bottom;
        bottomValid = bottom <= 19;// && bottom >= bottom;

        //                     bg, tile,              x, y, width, height, palNum
        // Draw title window frame
        if(topValid && leftValid)
            FillBgTilemapBufferRect(bg, TILE_TOP_CORNER_L, left, top, 1, 1, BG_PAL_ID_WINDOW_FRAMES);

        if(topValid && rightValid)
            FillBgTilemapBufferRect(bg, TILE_TOP_CORNER_R, right, top, 1, 1, BG_PAL_ID_WINDOW_FRAMES);
            
        if(bottomValid && leftValid)
            FillBgTilemapBufferRect(bg, TILE_BOT_CORNER_L, left, bottom, 1, 1, BG_PAL_ID_WINDOW_FRAMES);

        if(bottomValid && rightValid)
            FillBgTilemapBufferRect(bg, TILE_BOT_CORNER_R, right, bottom, 1, 1, BG_PAL_ID_WINDOW_FRAMES);


        if(topValid)
            FillBgTilemapBufferRect(bg, TILE_TOP_EDGE, left + 1, top, (right - left - 1), 1, BG_PAL_ID_WINDOW_FRAMES);

        if(bottomValid)
            FillBgTilemapBufferRect(bg, TILE_BOT_EDGE, left + 1, bottom, (right - left - 1), 1, BG_PAL_ID_WINDOW_FRAMES);

        if(leftValid)
            FillBgTilemapBufferRect(bg, TILE_LEFT_EDGE, left, top + 1, 1, (bottom - top - 1), BG_PAL_ID_WINDOW_FRAMES);

        if(rightValid)
            FillBgTilemapBufferRect(bg, TILE_RIGHT_EDGE, right, top + 1, 1, (bottom - top - 1), BG_PAL_ID_WINDOW_FRAMES);

        //CopyBgTilemapBufferToVram(bg);
    }
}