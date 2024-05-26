#include "global.h"
#include "constants/event_objects.h"
#include "palette.h"
#include "main.h"
#include "task.h"
#include "bg.h"
#include "malloc.h"
#include "window.h"
#include "text.h"
#include "menu.h"
#include "international_string_util.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "gpu_regs.h"
#include "m4a.h"
#include "constants/rgb.h"
#include "trainer_pokemon_sprites.h"
#include "starter_choose.h"
#include "event_object_movement.h"
#include "decompress.h"
#include "intro_credits_graphics.h"
#include "sound.h"
#include "trig.h"
#include "graphics.h"
#include "pokedex.h"
#include "event_data.h"
#include "random.h"

#include "rogue_controller.h"
#include "rogue_followmon.h"
#include "rogue_timeofday.h"

// Fade into accurate background colour
#define COLOR_DARK_GREEN (gPlttBufferUnfaded[BG_PLTT_ID(15) + 6]) // RGB(7, 11, 6)
#define COLOR_LIGHT_GREEN (gPlttBufferUnfaded[BG_PLTT_ID(15) + 7]) // RGB(13, 20, 12)

#define TAG_MON_BG 1001

// Positions for the Pokémon images
enum {
    POS_LEFT,
    POS_CENTER,
    POS_RIGHT,
};

enum {
    MODE_NONE,
    MODE_BIKE_SCENE,
    MODE_SHOW_MONS,
};

#define tState data[0]

// Task data for the main Credits tasks
#define tTaskId_BgScenery  data[0] // ID for Task_BicycleBgAnimation (created by CreateBicycleBgAnimationTask)
#define tTaskId_BikeScene  data[1] // ID for Task_BikeScene
#define tTaskId_SceneryPal data[2] // ID for Task_CycleSceneryPalette
#define tEndCredits        data[4]
#define tSceneNum          data[7]
// data[8]-[10] are unused
#define tNextMode          data[11]
#define tTheEndDelay       data[12]
#define tCurrentMode       data[13]
#define tPrintedPage       data[14]
#define tTaskId_UpdatePage data[15]

enum
{
    ROGUE_SPRITE_PLAYER,
    ROGUE_SPRITE_MON_START,
    ROGUE_SPRITE_MON_END = ROGUE_SPRITE_MON_START + 6,
    ROGUE_SPRITE_END
};

struct RogueSpriteData
{
    s16 desiredX;
    u8 spriteIndex;
};

struct RogueCreditsData
{
    struct RogueSpriteData rogueSprites[ROGUE_SPRITE_END];
    struct RoguePartySnapshot previousPartySnapshot;
    struct RoguePartySnapshot currentPartySnapshot;
    u8 currentDisplayPhase;
    u8 updateFrame;
    u8 inEndFade : 1;
    u8 hintAtEvos : 1;
};

struct CreditsData
{
    u16 imgCounter; //how many mon images have been shown
    u16 nextImgPos; //if the next image spawns left/center/right
};

struct CreditsEntry
{
    const u8 *text;
    u16 flags;
};

static EWRAM_DATA s16 UNUSED sUnkVar = 0; // Never read, only set to 0
static EWRAM_DATA u16 sSavedTaskId = 0;
EWRAM_DATA bool8 gHasHallOfFameRecords = 0;
static EWRAM_DATA bool8 sUsedSpeedUp = 0; // Never read
static EWRAM_DATA struct CreditsData *sCreditsData = {0};
static EWRAM_DATA struct RogueCreditsData *sRogueCreditsData = {0};

static const u16 sCredits_Pal[] = INCBIN_U16("graphics/credits/credits.gbapal");
static const u16 sCreditsTheEnd_Pal[] = INCBIN_U16("graphics/credits/the_end.gbapal");
static const u32 sCreditsCopyrightEnd_Gfx[] = INCBIN_U32("graphics/credits/the_end_copyright.4bpp.lz");

static void SpriteCB_CreditsMonBg(struct Sprite *);
static void Task_WaitPaletteFade(u8);
static void Task_CreditsMain(u8);
static void Task_ReadyBikeScene(u8);
static void Task_SetBikeScene(u8);
static void SetupRogueSprites(u8 snapshotIndex);
static void FadeOutRogueSprites(u8 snapshotIndex);
static void UpdateRogueSprites();
static bool8 AreRogueSpritesAnimating();
static void Task_CreditsTheEnd1(u8);
static void Task_CreditsTheEnd2(u8);
static void Task_CreditsTheEnd3(u8);
static void Task_CreditsTheEnd4(u8);
static void Task_CreditsTheEnd5(u8);
static void Task_CreditsTheEnd6(u8);
static void Task_CreditsSoftReset(u8);
static void ResetGpuAndVram(void);
static void Task_UpdatePage(u8);
static u8 CheckChangeScene(u8, u8);
static void Task_CycleSceneryPalette(u8);
static void Task_BikeScene(u8);
static bool8 LoadBikeScene(u8 data, u8);
static void ResetCreditsTasks(u8);
static void LoadTheEndScreen(u16, u16, u16);
static void DrawTheEnd(u16, u16);
static void SpriteCB_Player(struct Sprite *);
static void SpriteCB_Rival(struct Sprite *);

static const u8 sTheEnd_LetterMap_T[] =
{
    0,    1, 0,
    0xFF, 1, 0xFF,
    0xFF, 1, 0xFF,
    0xFF, 1, 0xFF,
    0xFF, 1, 0xFF,
};

static const u8 sTheEnd_LetterMap_H[] =
{
    1, 0xFF, 1,
    1, 0xFF, 1,
    1, 2,    1,
    1, 0xFF, 1,
    1, 0xFF, 1,
};

static const u8 sTheEnd_LetterMap_E[] =
{
    1, 0, 0,
    1, 0xFF, 0xFF,
    1, 2,    2,
    1, 0xFF, 0xFF,
    1, 0x80, 0x80,
};

static const u8 sTheEnd_LetterMap_N[] =
{
    1, 3, 1,
    1, 4, 1,
    1, 5, 1,
    1, 0xC4, 1,
    1, 0xC3, 1,
};

static const u8 sTheEnd_LetterMap_D[] =
{
    1, 6, 7,
    1, 8, 9,
    1, 0xFF, 1,
    1, 0x88, 0x89,
    1, 0x86, 0x87,
};

static const u8 sTheEnd_LetterMap_QMark[] =
{
    1,    0,    1,
    0,    0xFF, 1,
    0xFF, 1,    0,
    0xFF, 0,    0xFF,
    0xFF, 1,    0xFF,
};

static const u8 sTheEnd_LetterMap_EMark[] =
{
    0xFF, 1,    0xFF,
    0xFF, 1,    0xFF,
    0xFF, 1,    0xFF,
    0xFF, 0,    0xFF,
    0xFF, 1,    0xFF,
};

static const u8 sTheEnd_LetterMap_FullStop[] =
{
    0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF,
    0xFF, 1,    0xFF,
};

#include "data/credits.h"

static const struct BgTemplate sBackgroundTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 2,
        .mapBaseIndex = 28,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    },
};
static const struct WindowTemplate sWindowTemplates[] =
{
    {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 5, // 9
        .width = DISPLAY_TILE_WIDTH,
        .height = 12,
        .paletteNum = 8,
        .baseBlock = 1
    },
    DUMMY_WIN_TEMPLATE,
};
static const u8 sMonSpritePos[][2] =
{
    {104, 36},
    {120, 36},
    {136, 36},
};

static const union AnimCmd sAnim_Player_Slow[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(64, 8),
    ANIMCMD_FRAME(128, 8),
    ANIMCMD_FRAME(192, 8),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sAnim_Player_Fast[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(64, 4),
    ANIMCMD_FRAME(128, 4),
    ANIMCMD_FRAME(192, 4),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sAnim_Player_LookBack[] =
{
    ANIMCMD_FRAME(256, 4),
    ANIMCMD_FRAME(320, 4),
    ANIMCMD_FRAME(384, 4),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_Player_LookForward[] =
{
    ANIMCMD_FRAME(384, 30),
    ANIMCMD_FRAME(320, 30),
    ANIMCMD_FRAME(256, 30),
    ANIMCMD_FRAME(256, 30),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_Player[] =
{
    sAnim_Player_Slow,
    sAnim_Player_Fast,
    sAnim_Player_LookBack,
    sAnim_Player_LookForward,
};

static const union AnimCmd sAnim_Rival_Slow[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(64, 8),
    ANIMCMD_FRAME(128, 8),
    ANIMCMD_FRAME(192, 8),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sAnim_Rival_Fast[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(64, 4),
    ANIMCMD_FRAME(128, 4),
    ANIMCMD_FRAME(192, 4),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sAnim_Rival_Still[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_Rival[] =
{
    sAnim_Rival_Slow,
    sAnim_Rival_Fast,
    sAnim_Rival_Still,
};

#define MONBG_OFFSET (MON_PIC_SIZE * 3)
static const struct SpriteSheet sSpriteSheet_MonBg[] = {
    { gDecompressionBuffer, MONBG_OFFSET, TAG_MON_BG },
    {},
};
static const struct SpritePalette sSpritePalette_MonBg[] = {
    { (const u16 *)&gDecompressionBuffer[MONBG_OFFSET], TAG_MON_BG },
    {},
};

static const struct OamData sOamData_MonBg =
{
    .y = DISPLAY_HEIGHT,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x64),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(64x64),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sAnim_MonBg_Yellow[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_MonBg_Red[] =
{
    ANIMCMD_FRAME(64, 8),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_MonBg_Blue[] =
{
    ANIMCMD_FRAME(128, 8),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_MonBg[] =
{
    [POS_LEFT]   = sAnim_MonBg_Yellow,
    [POS_CENTER] = sAnim_MonBg_Red,
    [POS_RIGHT]  = sAnim_MonBg_Blue,
};

static const struct SpriteTemplate sSpriteTemplate_CreditsMonBg =
{
    .tileTag = TAG_MON_BG,
    .paletteTag = TAG_MON_BG,
    .oam = &sOamData_MonBg,
    .anims = sAnims_MonBg,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_CreditsMonBg,
};

static void VBlankCB_Credits(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void CB2_Credits(void)
{
    RunTasks();
    AnimateSprites();

    if ((JOY_HELD(B_BUTTON))
     && gHasHallOfFameRecords
     && gTasks[sSavedTaskId].func == Task_CreditsMain)
    {
        u8 i;

        for(i = 1; i < 11; ++i)
        {
            // Speed up credits
            VBlankCB_Credits();
            RunTasks();
            AnimateSprites();
        }

        sUsedSpeedUp = TRUE;
    }
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void InitCreditsBgsAndWindows(void)
{
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sBackgroundTemplates, ARRAY_COUNT(sBackgroundTemplates));
    SetBgTilemapBuffer(0, AllocZeroed(BG_SCREEN_SIZE));
    LoadPalette(sCredits_Pal, BG_PLTT_ID(8), 2 * PLTT_SIZE_4BPP);
    InitWindows(sWindowTemplates);
    DeactivateAllTextPrinters();
    PutWindowTilemap(0);
    CopyWindowToVram(0, COPYWIN_FULL);
    ShowBg(0);
}

static void FreeCreditsBgsAndWindows(void)
{
    void *ptr;
    FreeAllWindowBuffers();
    ptr = GetBgTilemapBuffer(0);
    if (ptr)
        Free(ptr);
}

static void PrintCreditsText(const u8 *string, u8 y, bool8 isTitle)
{
    u8 x;
    u8 color[3];

    color[0] = TEXT_COLOR_TRANSPARENT;

    if (isTitle == TRUE)
    {
        color[1] = TEXT_COLOR_LIGHT_GRAY;
        color[2] = TEXT_COLOR_RED;
    }
    else
    {
        color[1] = TEXT_COLOR_WHITE;
        color[2] = TEXT_COLOR_DARK_GRAY;
    }

    x = GetStringCenterAlignXOffsetWithLetterSpacing(FONT_NORMAL, string, DISPLAY_WIDTH, 1);
    AddTextPrinterParameterized4(0, FONT_NORMAL, x, y, 1, 0, color, TEXT_SKIP_DRAW, string);
}

#define tMainTaskId data[1]

void CB2_StartCreditsSequence(void)
{
    u8 taskId;
    s16 bikeTaskId;
    u8 pageTaskId;

    RogueToD_SetTempDisableTimeVisuals(TRUE);

    ResetGpuAndVram();
    SetVBlankCallback(NULL);
    InitHeap(gHeap, HEAP_SIZE);
    ResetPaletteFade();
    ResetTasks();
    InitCreditsBgsAndWindows();

    taskId = CreateTask(Task_WaitPaletteFade, 0);

    gTasks[taskId].tEndCredits = FALSE;
    gTasks[taskId].tSceneNum = SCENE_OCEAN_MORNING;
    gTasks[taskId].tNextMode = MODE_NONE;
    gTasks[taskId].tCurrentMode = MODE_BIKE_SCENE;

    while (TRUE)
    {
        if (LoadBikeScene(SCENE_OCEAN_MORNING, taskId))
            break;
    }

    bikeTaskId = gTasks[taskId].tTaskId_BikeScene;
    gTasks[bikeTaskId].tState = 40;

    //SetGpuReg(REG_OFFSET_BG0VOFS, 0xFFFC);

    pageTaskId = CreateTask(Task_UpdatePage, 0);

    gTasks[pageTaskId].tMainTaskId = taskId;
    gTasks[taskId].tTaskId_UpdatePage = pageTaskId;

    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
    EnableInterrupts(INTR_FLAG_VBLANK);
    SetVBlankCallback(VBlankCB_Credits);
    m4aSongNumStart(MUS_HG_CREDITS);
    SetMainCallback2(CB2_Credits);
    sUsedSpeedUp = FALSE;
    sCreditsData = AllocZeroed(sizeof(struct CreditsData));

    sCreditsData->imgCounter = 0;
    sCreditsData->nextImgPos = POS_LEFT;

    sSavedTaskId = taskId;
}

static void Task_WaitPaletteFade(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_CreditsMain;
}

static void Task_CreditsMain(u8 taskId)
{
    u16 mode;

    if (gTasks[taskId].tEndCredits)
    {
        s16 bikeTaskId = gTasks[taskId].tTaskId_BikeScene;
        gTasks[bikeTaskId].tState = 30;

        gTasks[taskId].tTheEndDelay = 256;
        gTasks[taskId].func = Task_CreditsTheEnd1;
        return;
    }

    sUnkVar = 0;
    mode = gTasks[taskId].tNextMode;

    if (gTasks[taskId].tNextMode == MODE_BIKE_SCENE)
    {
        // Start a bike cutscene
        gTasks[taskId].tCurrentMode = mode;
        gTasks[taskId].tNextMode = MODE_NONE;
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_ReadyBikeScene;
    }
    //else if (gTasks[taskId].tNextMode == MODE_SHOW_MONS)
    //{
    //    // Start a Pokémon interlude
    //    gTasks[taskId].tCurrentMode = mode;
    //    gTasks[taskId].tNextMode = MODE_NONE;
    //    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    //    gTasks[taskId].func = Task_ReadyShowMons;
    //}
}

static void Task_ReadyBikeScene(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        ResetCreditsTasks(taskId);
        gTasks[taskId].func = Task_SetBikeScene;
    }
}

static void Task_SetBikeScene(u8 taskId)
{
    SetVBlankCallback(NULL);

    if (LoadBikeScene(gTasks[taskId].tSceneNum, taskId))
    {
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        EnableInterrupts(INTR_FLAG_VBLANK);
        SetVBlankCallback(VBlankCB_Credits);
        gTasks[taskId].func = Task_WaitPaletteFade;
    }
}

static void Task_CreditsTheEnd1(u8 taskId)
{
    // Stop scrolling bg
    if (gTasks[taskId].tTaskId_BgScenery != 0)
    {
        DestroyTask(gTasks[taskId].tTaskId_BgScenery);
        gTasks[taskId].tTaskId_BgScenery = 0;
    }

    UpdateRogueSprites();

    if (gTasks[taskId].tTheEndDelay)
    {
        gTasks[taskId].tTheEndDelay--;
        return;
    }

    BeginNormalPaletteFade(PALETTES_ALL, 12, 0, 16, RGB_BLACK);
    gTasks[taskId].func = Task_CreditsTheEnd2;
}

static void Task_CreditsTheEnd2(u8 taskId)
{
    UpdateRogueSprites();
    
    if (!gPaletteFade.active)
    {
        FREE_AND_SET_NULL(sRogueCreditsData);
        ResetCreditsTasks(taskId);
        gTasks[taskId].func = Task_CreditsTheEnd3;
    }
}

#define tDelay data[0]

static void Task_CreditsTheEnd3(u8 taskId)
{
    ResetGpuAndVram();
    ResetPaletteFade();
    LoadTheEndScreen(0, 0x3800, BG_PLTT_ID(0));
    ResetSpriteData();
    FreeAllSpritePalettes();
    BeginNormalPaletteFade(PALETTES_ALL, 8, 16, 0, RGB_BLACK);

    SetGpuReg(REG_OFFSET_BG0CNT, BGCNT_PRIORITY(0)
                               | BGCNT_CHARBASE(0)
                               | BGCNT_SCREENBASE(7)
                               | BGCNT_16COLOR
                               | BGCNT_TXT256x256);
    EnableInterrupts(INTR_FLAG_VBLANK);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0
                                | DISPCNT_OBJ_1D_MAP
                                | DISPCNT_BG0_ON);

    gTasks[taskId].tDelay = 0; //235; //set this to 215 to actually show "THE END" in time to the last song beat
    gTasks[taskId].func = Task_CreditsTheEnd4;
}

static void Task_CreditsTheEnd4(u8 taskId)
{
    if (gTasks[taskId].tDelay)
    {
        gTasks[taskId].tDelay--;
        return;
    }

    BeginNormalPaletteFade(PALETTES_ALL, 6, 0, 16, RGB_BLACK);
    gTasks[taskId].func = Task_CreditsTheEnd5;
}

static void Task_CreditsTheEnd5(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        FadeOutBGM(8);

        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0, RGB_BLACK);
        gTasks[taskId].tDelay = 7200;
        gTasks[taskId].func = Task_CreditsTheEnd6;
    }
}

static void Task_CreditsTheEnd6(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if(gTasks[taskId].tDelay <= 6990)
        {
            if (gTasks[taskId].tDelay == 0 || gMain.newKeys)
            {
                FadeOutBGM(4);
                BeginNormalPaletteFade(PALETTES_ALL, 8, 0, 16, RGB_WHITEALPHA);
                RogueToD_SetTempDisableTimeVisuals(FALSE);
                gTasks[taskId].func = Task_CreditsSoftReset;
                return;
            }
        }

        if (gTasks[taskId].tDelay == 6990) // 6840)
        {
            DrawTheEnd(0x3800, 0);
            m4aSongNumStart(MUS_HG_END);
        }

        gTasks[taskId].tDelay--;
    }
}

#undef tDelay

static void Task_CreditsSoftReset(u8 taskId)
{
    if (!gPaletteFade.active)
        SoftReset(RESET_ALL);
}

static void ResetGpuAndVram(void)
{
    SetGpuReg(REG_OFFSET_DISPCNT, 0);

    SetGpuReg(REG_OFFSET_BG3HOFS, 0);
    SetGpuReg(REG_OFFSET_BG3VOFS, 0);
    SetGpuReg(REG_OFFSET_BG2HOFS, 0);
    SetGpuReg(REG_OFFSET_BG2VOFS, 0);
    SetGpuReg(REG_OFFSET_BG1HOFS, 0);
    SetGpuReg(REG_OFFSET_BG1VOFS, 0);
    SetGpuReg(REG_OFFSET_BG0HOFS, 0);
    SetGpuReg(REG_OFFSET_BG0VOFS, 0);

    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_BLDALPHA, 0);
    SetGpuReg(REG_OFFSET_BLDY, 0);

    DmaFill16(3, 0, (void *)VRAM, VRAM_SIZE);
    DmaFill32(3, 0, (void *)OAM, OAM_SIZE);
    DmaFill16(3, 0, (void *)(PLTT + 2), PLTT_SIZE - 2);
}

#define tCurrentPage    data[2]
#define tCurrentIndex   data[3]
#define tDelay          data[4]

static u16 CalculateTotalPageCount()
{
    u16 i, entryCount, pageCount;

    pageCount = 0;
    entryCount = 0;

    for(i = 0; i < ARRAY_COUNT(sCreditsEntryPointerTable); ++i)
    {
        if((sCreditsEntryPointerTable[i].flags & CREDITS_FLAG_BREAK) != 0)
        {
            entryCount = 0;
            ++pageCount;
        }
        else if(++entryCount >= ENTRIES_PER_PAGE)
        {
            entryCount = 0;
            ++pageCount;
        }
    }

    return pageCount;
}

static void Task_UpdatePage(u8 taskId)
{
    int i;

    UpdateRogueSprites();

    switch (gTasks[taskId].tState)
    {
    case 0:
    case 6:
    case 7:
    case 8:
    case 9:
    default:
        if (!gPaletteFade.active)
        {
            gTasks[taskId].tState = 1;
            gTasks[taskId].tDelay = 72;
            gTasks[gTasks[taskId].tMainTaskId].tPrintedPage = FALSE;
            sUnkVar = 0;
        }
        return;
    case 1:
        if (gTasks[taskId].tDelay != 0)
        {
            gTasks[taskId].tDelay--;
            return;
        }
        gTasks[taskId].tState++;
        return;
    case 2:
        if (gTasks[gTasks[taskId].tMainTaskId].func == Task_CreditsMain)
        {
            if (gTasks[taskId].tCurrentIndex < ARRAY_COUNT(sCreditsEntryPointerTable))
            {
                // Print text for this Credits page
                u16 entryIdx, entryCount;
                u16 entryIndices[ENTRIES_PER_PAGE];

                entryCount = 0;
                
                // Don't start printing on a break
                while((sCreditsEntryPointerTable[gTasks[taskId].tCurrentIndex].flags & CREDITS_FLAG_BREAK) != 0)
                {
                    gTasks[taskId].tCurrentIndex++;
                }

                for (i = 0; i < ENTRIES_PER_PAGE; i++)
                {
                    entryIdx = gTasks[taskId].tCurrentIndex++;
                    entryIndices[entryCount++] = entryIdx;

                    if((sCreditsEntryPointerTable[entryIdx].flags & CREDITS_FLAG_BREAK) != 0)
                        break;
                }

                for (i = 0; i < entryCount; i++)
                {
                    entryIdx = entryIndices[i];

                    PrintCreditsText(
                        sCreditsEntryPointerTable[entryIdx].text,
                        5 + i * 16,
                        ((sCreditsEntryPointerTable[entryIdx].flags & CREDITS_FLAG_TITLE) != 0)
                    );
                }
                CopyWindowToVram(0, COPYWIN_GFX);

                gTasks[taskId].tCurrentPage++;
                gTasks[taskId].tState++;

                if(sRogueCreditsData != NULL)
                {
                    // 2 phases per difficulty (enter and exit)
                    u16 totalPageCount = CalculateTotalPageCount() - 1;
                    u16 displayPhase = ((gTasks[taskId].tCurrentPage + 1) * gRogueRun.partySnapshotCount * 2) / totalPageCount;

                    DebugPrintf("Phase %d, Page %d / %d", displayPhase, gTasks[taskId].tCurrentPage, totalPageCount);

                    displayPhase = min(displayPhase, gRogueRun.partySnapshotCount * 2);

                    if(sRogueCreditsData->currentDisplayPhase != displayPhase)
                    {
                        u8 desiredSnapshotIndex = displayPhase / 2;
                        u8 currentSnapshotIndex= sRogueCreditsData->currentDisplayPhase / 2;

                        sRogueCreditsData->currentDisplayPhase = displayPhase;

                        if(desiredSnapshotIndex != currentSnapshotIndex)
                        {
                            DebugPrintf("    SetupRogueSprites %d", desiredSnapshotIndex);
                            SetupRogueSprites(desiredSnapshotIndex);
                        }
                        else
                        {
                            DebugPrintf("    FadeOutRogueSprites %d", desiredSnapshotIndex);
                            FadeOutRogueSprites(desiredSnapshotIndex);
                        }
                    }
                }

                gTasks[gTasks[taskId].tMainTaskId].tPrintedPage = TRUE;

                if (gTasks[gTasks[taskId].tMainTaskId].tCurrentMode == MODE_BIKE_SCENE)
                    BeginNormalPaletteFade(0x300, 0, 16, 0, COLOR_LIGHT_GREEN);
                else // MODE_SHOW_MONS
                    BeginNormalPaletteFade(0x300, 0, 16, 0, COLOR_DARK_GREEN);
                return;
            }

            // Reached final page of Credits, end task
            gTasks[taskId].tState = 10;
            FadeOutRogueSprites(255);
            return;
        }
        gTasks[gTasks[taskId].tMainTaskId].tPrintedPage = FALSE;
        return;
    case 3:
        if (!gPaletteFade.active)
        {
            gTasks[taskId].tDelay = 150;
            gTasks[taskId].tState++;
        }
        return;
    case 4:
        if (gTasks[taskId].tDelay != 0)
        {
            gTasks[taskId].tDelay--;
            return;
        }

        if (CheckChangeScene((u8)gTasks[taskId].tCurrentPage, (u8)gTasks[taskId].tMainTaskId))
        {
            gTasks[taskId].tState++;
            return;
        }
        gTasks[taskId].tState++;
        if (gTasks[gTasks[taskId].tMainTaskId].tCurrentMode == MODE_BIKE_SCENE)
            BeginNormalPaletteFade(0x300, 0, 0, 16, COLOR_LIGHT_GREEN);
        else // MODE_SHOW_MONS
            BeginNormalPaletteFade(0x300, 0, 0, 16, COLOR_DARK_GREEN);
        return;
    case 5:
        if (!gPaletteFade.active)
        {
            // Still more Credits pages to show, return to state 2 to print
            FillWindowPixelBuffer(0, PIXEL_FILL(0));
            CopyWindowToVram(0, COPYWIN_GFX);
            gTasks[taskId].tState = 2;
        }
        return;
    case 10:
        if(AreRogueSpritesAnimating())
        {
            gTasks[gTasks[taskId].tMainTaskId].tEndCredits = TRUE;
            DestroyTask(taskId);
            FreeCreditsBgsAndWindows();
            FREE_AND_SET_NULL(sCreditsData);
        }
        return;
    }
}

#undef tDelay

#define PAGE_INTERVAL 3 // 9 scenes (5 bike scenes, 4 Pokémon interludes)

static u8 CheckChangeScene(u8 page, u8 taskId)
{
    // Starts with bike + ocean + morning (SCENE_OCEAN_MORNING)

    //if (page == PAGE_INTERVAL * 1)
    //{
    //    // Pokémon interlude
    //    gTasks[taskId].tNextMode = MODE_SHOW_MONS;
    //}
//
    //if (page == PAGE_INTERVAL * 2)
    //{
    //    // Bike + ocean + sunset
    //    gTasks[taskId].tSceneNum = SCENE_OCEAN_SUNSET;
    //    gTasks[taskId].tNextMode = MODE_BIKE_SCENE;
    //}
//
    //if (page == PAGE_INTERVAL * 3)
    //{
    //    // Pokémon interlude
    //    gTasks[taskId].tNextMode = MODE_SHOW_MONS;
    //}
//
    //if (page == PAGE_INTERVAL * 4)
    //{
    //    // Bike + forest + sunset
    //    gTasks[taskId].tSceneNum = SCENE_FOREST_RIVAL_ARRIVE;
    //    gTasks[taskId].tNextMode = MODE_BIKE_SCENE;
    //}
//
    //if (page == PAGE_INTERVAL * 5)
    //{
    //    // Pokémon interlude
    //    gTasks[taskId].tNextMode = MODE_SHOW_MONS;
    //}
//
    //if (page == PAGE_INTERVAL * 6)
    //{
    //    // Bike + forest + sunset
    //    gTasks[taskId].tSceneNum = SCENE_FOREST_CATCH_RIVAL;
    //    gTasks[taskId].tNextMode = MODE_BIKE_SCENE;
    //}
//
    //if (page == PAGE_INTERVAL * 7)
    //{
    //    // Pokémon interlude
    //    gTasks[taskId].tNextMode = MODE_SHOW_MONS;
    //}
//
    //if (page == PAGE_INTERVAL * 8)
    //{
    //    // Bike + town + night
    //    gTasks[taskId].tSceneNum = SCENE_CITY_NIGHT;
    //    gTasks[taskId].tNextMode = MODE_BIKE_SCENE;
    //}
//
    //if(gTasks[taskId].tCurrentIndex >= ARRAY_COUNT(sCreditsEntryPointerTable))
    //{
    //    gTasks[taskId].tSceneNum = SCENE_CITY_NIGHT;
    //    gTasks[taskId].tNextMode = MODE_BIKE_SCENE;
    //}

    if (gTasks[taskId].tNextMode != MODE_NONE)
    {
        // Returns true if changed
        return TRUE;
    }

    return FALSE;
}

#define tDelay data[3]

#undef tMainTaskId
#undef tDelay

#define tDelay  data[4]
#define tSinIdx data[5]

static void Task_BikeScene(u8 taskId)
{
    switch (gTasks[taskId].tState)
    {
    case 0:
        gIntroCredits_MovingSceneryVOffset = Sin((gTasks[taskId].tSinIdx >> 1) & 0x7F, 12);
        gTasks[taskId].tSinIdx++;
        break;
    case 1:
        if (gIntroCredits_MovingSceneryVOffset != 0)
        {
            gIntroCredits_MovingSceneryVOffset = Sin((gTasks[taskId].tSinIdx >> 1) & 0x7F, 12);
            gTasks[taskId].tSinIdx++;
        }
        else
        {
            gTasks[taskId].tSinIdx = 0;
            gTasks[taskId].tState++;
        }
        break;
    case 2:
        if (gTasks[taskId].tSinIdx < 64)
        {
            gTasks[taskId].tSinIdx++;
            gIntroCredits_MovingSceneryVOffset = Sin(gTasks[taskId].tSinIdx & 0x7F, 20);
        }
        else
        {
            gTasks[taskId].tState++;
        }
        break;
    case 3:
        gTasks[taskId].tDelay = 120;
        gTasks[taskId].tState++;
        break;
    case 4:
        if (gTasks[taskId].tDelay != 0)
        {
            gTasks[taskId].tDelay--;
        }
        else
        {
            gTasks[taskId].tSinIdx = 64;
            gTasks[taskId].tState++;
        }
        break;
    case 5:
        if (gTasks[taskId].tSinIdx > 0)
        {
            gTasks[taskId].tSinIdx--;
            gIntroCredits_MovingSceneryVOffset = Sin(gTasks[taskId].tSinIdx & 0x7F, 20);
        }
        else
        {
            gTasks[taskId].tState++;
        }
        break;
    case 6:
        gTasks[taskId].tState = 50;
        break;
    case 10:
        gTasks[taskId].tState = 50;
        break;
    case 20:
        gTasks[taskId].tState = 50;
        break;
    case 30:
        gTasks[taskId].tState = 50;
        break;
    case 50:
        gTasks[taskId].tState = 0;
        break;
    }
}

#define TIMER_STOP  0x7FFF
#define tTimer      data[1]
#define tMainTaskId data[2]

static void Task_CycleSceneryPalette(u8 taskId)
{
    s16 bikeTaskId;

    switch (gTasks[taskId].tState)
    {
    default:
    case SCENE_OCEAN_MORNING:
        if (gTasks[taskId].tTimer != TIMER_STOP)
        {
            if (gTasks[gTasks[gTasks[taskId].tMainTaskId].tTaskId_UpdatePage].tCurrentPage == 2)
            {
                gTasks[gTasks[gTasks[taskId].tMainTaskId].tTaskId_BikeScene].tState = 20;
                gTasks[taskId].tTimer = TIMER_STOP;
            }
        }
        CycleSceneryPalette(0);
        break;
    case SCENE_OCEAN_SUNSET:
        CycleSceneryPalette(0);
        break;
    case SCENE_FOREST_RIVAL_ARRIVE:
        if (gTasks[taskId].tTimer != TIMER_STOP)
        {
            bikeTaskId = gTasks[gTasks[taskId].tMainTaskId].tTaskId_BikeScene;

            // Floor to multiple of 128
            if ((gTasks[bikeTaskId].tSinIdx & -128) == 640)
            {
                gTasks[bikeTaskId].tState = 1;
                gTasks[taskId].tTimer = TIMER_STOP;
            }
        }
        CycleSceneryPalette(1);
        break;
    case SCENE_FOREST_CATCH_RIVAL:
        if (gTasks[taskId].tTimer != TIMER_STOP)
        {

            if (gTasks[taskId].tTimer == 584)
            {
                gTasks[gTasks[gTasks[taskId].tMainTaskId].tTaskId_BikeScene].tState = 10;
                gTasks[taskId].tTimer = TIMER_STOP;
            }
            else
            {
                gTasks[taskId].tTimer++;
            }
        }
        CycleSceneryPalette(1);
        break;
    case SCENE_CITY_NIGHT:
        CycleSceneryPalette(2);
        break;
    }
}

struct MonSpriteTemplate
{
    s16 x;
    s16 y;
    u8 subpriority;
};

#define GFX_EGG_SPECIES(species) (Rogue_GetEggSpecies(species >= FOLLOWMON_SHINY_OFFSET ? (species - FOLLOWMON_SHINY_OFFSET) : species))

static void UpdateDisplayedSnapshots(u8 snapshotIndex)
{
    memcpy(&sRogueCreditsData->previousPartySnapshot, &sRogueCreditsData->currentPartySnapshot, sizeof(sRogueCreditsData->previousPartySnapshot));

    if(snapshotIndex < gRogueRun.partySnapshotCount)
    {
        // Copy whilst maintaining index so pokemon don't jump around party slots
        u8 i, j;
        u8 useMonIndices[PARTY_SIZE] = {0};

        memset(&sRogueCreditsData->currentPartySnapshot, 0, sizeof(sRogueCreditsData->currentPartySnapshot));

        // Copy the mons we previously were showing so they are in the same slot
        for(i = 0; i < PARTY_SIZE; ++i)
        {
            if(sRogueCreditsData->previousPartySnapshot.partyPersonalities[i] != 0)
            {
                for(j = 0; j < PARTY_SIZE; ++j)
                {
                    if(
                        GFX_EGG_SPECIES(gRogueRun.partySnapshots[snapshotIndex].partySpeciesGfx[j]) == GFX_EGG_SPECIES(sRogueCreditsData->previousPartySnapshot.partySpeciesGfx[i]) &&
                        gRogueRun.partySnapshots[snapshotIndex].partyPersonalities[j] == sRogueCreditsData->previousPartySnapshot.partyPersonalities[i]
                    )
                    {
                        // Found the same mon
                        sRogueCreditsData->currentPartySnapshot.partySpeciesGfx[i] = gRogueRun.partySnapshots[snapshotIndex].partySpeciesGfx[j];
                        sRogueCreditsData->currentPartySnapshot.partyPersonalities[i] = gRogueRun.partySnapshots[snapshotIndex].partyPersonalities[j];

                        useMonIndices[j] = TRUE;
                        break;
                    }
                }
            }
        }

        // Put new mons into any free slot
        for(i = 0; i < PARTY_SIZE; ++i)
        {
            if(useMonIndices[i] || gRogueRun.partySnapshots[snapshotIndex].partySpeciesGfx[i] == SPECIES_NONE)
                continue;

            // Find free slot and insert
            for(j = 0; j < PARTY_SIZE; ++j)
            {
                if(sRogueCreditsData->currentPartySnapshot.partySpeciesGfx[j] == SPECIES_NONE)
                {
                    sRogueCreditsData->currentPartySnapshot.partySpeciesGfx[j] = gRogueRun.partySnapshots[snapshotIndex].partySpeciesGfx[i];
                    sRogueCreditsData->currentPartySnapshot.partyPersonalities[j] = gRogueRun.partySnapshots[snapshotIndex].partyPersonalities[i];
                    break;
                }
            }
        }
    }
}

static void SetupRogueSprites(u8 snapshotIndex)
{
    struct MonSpriteTemplate const cMonTemplates[PARTY_SIZE] = 
    {
        { 72 - 16 * 1, 72 - 8 * 1, 4 },
        { 72 - 16 * 1, 72 + 8 * 1, 2 },
        { 72 - 16 * 2, 72 - 8 * 2, 5 },
        { 72 - 16 * 2, 72 + 8 * 2, 1 },
        { 72 - 16 * 3, 72 - 8 * 3, 6 },
        { 72 - 16 * 3, 72 + 8 * 3, 0 },
    };

    u8 i;
    u8 spriteId;
    
    // Setup player on initial load
    if(snapshotIndex == 0)
    {
        AGB_ASSERT(sRogueCreditsData == NULL);

        sRogueCreditsData = AllocZeroed(sizeof(struct RogueCreditsData));

        for(spriteId = 0; spriteId < ROGUE_SPRITE_END; ++spriteId)
            sRogueCreditsData->rogueSprites[spriteId].spriteIndex = SPRITE_NONE;

        UpdateDisplayedSnapshots(snapshotIndex);

        spriteId = CreateObjectGraphicsSprite(OBJ_EVENT_GFX_PLAYER_NORMAL, SpriteCallbackDummy, 72, 72, 3);
        gSprites[spriteId].oam.priority = 1;
        gSprites[spriteId].x2 = 0;
        StartSpriteAnim(&gSprites[spriteId], ANIM_STD_GO_EAST);

        sRogueCreditsData->rogueSprites[ROGUE_SPRITE_PLAYER].spriteIndex = spriteId;
        sRogueCreditsData->rogueSprites[ROGUE_SPRITE_PLAYER].desiredX = 0;
    }

    sRogueCreditsData->hintAtEvos = FALSE;

    // Destroy all the mon sprites we don't need
    for(i = 0; i < PARTY_SIZE; ++i)
    {
        if(sRogueCreditsData->rogueSprites[ROGUE_SPRITE_MON_START + i].spriteIndex != SPRITE_NONE)
        {
            // Sprites have gone off screen to left
            if(sRogueCreditsData->rogueSprites[ROGUE_SPRITE_MON_START + i].desiredX < 0 || sRogueCreditsData->previousPartySnapshot.partySpeciesGfx[i] != sRogueCreditsData->currentPartySnapshot.partySpeciesGfx[i])
            {
                DestroySprite(&gSprites[sRogueCreditsData->rogueSprites[ROGUE_SPRITE_MON_START + i].spriteIndex]);
                sRogueCreditsData->rogueSprites[ROGUE_SPRITE_MON_START + i].spriteIndex = SPRITE_NONE;
            }
        }
    }

    // Spawn in new sprites
    for(i = 0; i < PARTY_SIZE; ++i)
    {
        if(sRogueCreditsData->rogueSprites[ROGUE_SPRITE_MON_START + i].spriteIndex == SPRITE_NONE && sRogueCreditsData->currentPartySnapshot.partySpeciesGfx[i] != SPECIES_NONE)
        {
            FollowMon_SetGraphicsRaw(i, sRogueCreditsData->currentPartySnapshot.partySpeciesGfx[i]);

            spriteId = CreateObjectGraphicsSprite(
                    OBJ_EVENT_GFX_FOLLOW_MON_0 + i, 
                    SpriteCallbackDummy, 
                    cMonTemplates[i].x, cMonTemplates[i].y - (FollowMon_IsLargeGfx(sRogueCreditsData->currentPartySnapshot.partySpeciesGfx[i]) ? 16 : 0), 
                    cMonTemplates[i].subpriority
            );
            gSprites[spriteId].disableAnimOffsets = TRUE;
            gSprites[spriteId].oam.priority = 1;
            StartSpriteAnim(&gSprites[spriteId], ANIM_STD_GO_EAST);

            if(snapshotIndex == 0 || sRogueCreditsData->previousPartySnapshot.partyPersonalities[i] == sRogueCreditsData->currentPartySnapshot.partyPersonalities[i])
                gSprites[spriteId].x2 = 0;
            else
                gSprites[spriteId].x2 = 232; // place off screen indicating caught on adventure

            sRogueCreditsData->rogueSprites[ROGUE_SPRITE_MON_START + i].spriteIndex = spriteId;
            sRogueCreditsData->rogueSprites[ROGUE_SPRITE_MON_START + i].desiredX = 0;
        }
    }

    // Instantly copy current snapshot over previous to avoid pop/jump bug
    if(snapshotIndex == 0)
    {
        memcpy(&sRogueCreditsData->previousPartySnapshot, &sRogueCreditsData->currentPartySnapshot, sizeof(sRogueCreditsData->previousPartySnapshot));
    }
}

static void FadeOutRogueSprites(u8 snapshotIndex)
{
    u8 i;

    sRogueCreditsData->hintAtEvos = TRUE;
    UpdateDisplayedSnapshots(snapshotIndex);

    if(snapshotIndex == 255) // final display
    {
        sRogueCreditsData->inEndFade = TRUE;
        sRogueCreditsData->rogueSprites[ROGUE_SPRITE_PLAYER].desiredX = DISPLAY_WIDTH;
    }

    for(i = 0; i < PARTY_SIZE; ++i)
    {
        if(sRogueCreditsData->rogueSprites[ROGUE_SPRITE_MON_START + i].spriteIndex != SPRITE_NONE)
        {
            if(GFX_EGG_SPECIES(sRogueCreditsData->currentPartySnapshot.partySpeciesGfx[i]) != GFX_EGG_SPECIES(sRogueCreditsData->previousPartySnapshot.partySpeciesGfx[i]) || sRogueCreditsData->currentPartySnapshot.partyPersonalities[i] != sRogueCreditsData->previousPartySnapshot.partyPersonalities[i])
            {
                if(snapshotIndex == 255) // final display
                    sRogueCreditsData->rogueSprites[ROGUE_SPRITE_MON_START + i].desiredX = -1; // have it stand still
                else
                    sRogueCreditsData->rogueSprites[ROGUE_SPRITE_MON_START + i].desiredX = -DISPLAY_WIDTH; // number doesn't matter just get it off screen!
            }
            else if(snapshotIndex == 255) // final display
            {
                sRogueCreditsData->rogueSprites[ROGUE_SPRITE_MON_START + i].desiredX = DISPLAY_WIDTH;
            }
        }
    }
}

#undef GFX_EGG_SPECIES

static void UpdateRogueSprites()
{
    u8 i;
    u8 frame = sRogueCreditsData->updateFrame++;

    if(sRogueCreditsData == NULL)
        return;


    for(i = 0; i < ROGUE_SPRITE_END; ++i)
    {
        if(sRogueCreditsData->rogueSprites[i].spriteIndex != SPRITE_NONE)
        {
            u8 spriteId = sRogueCreditsData->rogueSprites[i].spriteIndex;

            if(sRogueCreditsData->inEndFade)
            {
                // If falling off screen do it slower
                if(gSprites[spriteId].x2 < 0)
                {
                    if(frame % 4)
                        continue;
                }
                else
                {
                    if(frame % 2)
                        continue;
                }
            }
            else
            {

                // If falling off screen do it slower
                if(gSprites[spriteId].x2 < 0)
                {
                    if(frame % 2)
                        continue;
                }
            }

            if(gSprites[spriteId].x2 > sRogueCreditsData->rogueSprites[i].desiredX)
                --gSprites[spriteId].x2;
            else if(gSprites[spriteId].x2 < sRogueCreditsData->rogueSprites[i].desiredX)
                ++gSprites[spriteId].x2;

            // Bobbing anim
            if(i >= ROGUE_SPRITE_MON_START && i <= ROGUE_SPRITE_MON_END)
            {
                s16 bobbingAnim = ((gSprites[spriteId].animCmdIndex % 2) ? 0 : -1);

                if(sRogueCreditsData->hintAtEvos && i >= ROGUE_SPRITE_MON_START && i <= ROGUE_SPRITE_MON_END)
                {
                    // Jump to indicate an evo
                    if(
                        sRogueCreditsData->previousPartySnapshot.partySpeciesGfx[i - ROGUE_SPRITE_MON_START] != sRogueCreditsData->currentPartySnapshot.partySpeciesGfx[i - ROGUE_SPRITE_MON_START] &&
                        sRogueCreditsData->previousPartySnapshot.partyPersonalities[i - ROGUE_SPRITE_MON_START] == sRogueCreditsData->currentPartySnapshot.partyPersonalities[i - ROGUE_SPRITE_MON_START]
                    )
                        bobbingAnim = (bobbingAnim == 0 ? -2 : 0); // flip to make clearer
                }

                gSprites[spriteId].y2 = bobbingAnim;
            }
        }
    }
}

static bool8 AreRogueSpritesAnimating()
{
    u8 i;

    if(sRogueCreditsData == NULL)
        return FALSE;

    for(i = 0; i < ROGUE_SPRITE_END; ++i)
    {
        if(sRogueCreditsData->rogueSprites[i].spriteIndex != SPRITE_NONE)
        {
            u8 spriteId = sRogueCreditsData->rogueSprites[i].spriteIndex;

            if(gSprites[spriteId].x2 != sRogueCreditsData->rogueSprites[i].desiredX)
                return TRUE;
        }
    }

    return FALSE;
}

static void SetBikeScene(u8 scene, u8 taskId)
{
    gTasks[taskId].tTaskId_BgScenery = CreateBicycleBgAnimationTask(0, 0x800, 0x20, 8);

    gTasks[taskId].tTaskId_SceneryPal = CreateTask(Task_CycleSceneryPalette, 0);
    gTasks[gTasks[taskId].tTaskId_SceneryPal].tState = scene;
    gTasks[gTasks[taskId].tTaskId_SceneryPal].tTimer = 0;
    gTasks[gTasks[taskId].tTaskId_SceneryPal].tMainTaskId = taskId;

    gTasks[taskId].tTaskId_BikeScene = CreateTask(Task_BikeScene, 0);
    gTasks[gTasks[taskId].tTaskId_BikeScene].tState = 0;
    gTasks[gTasks[taskId].tTaskId_BikeScene].data[1] = taskId; // data[1] is never read
    gTasks[gTasks[taskId].tTaskId_BikeScene].tDelay = 0;

    if (scene == SCENE_FOREST_RIVAL_ARRIVE)
        gTasks[gTasks[taskId].tTaskId_BikeScene].tSinIdx = 69;

    SetupRogueSprites(0);
}

#undef tTimer
#undef tDelay
#undef tSinIdx

static bool8 LoadBikeScene(u8 scene, u8 taskId)
{
    switch (gMain.state)
    {
    default:
    case 0:
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        SetGpuReg(REG_OFFSET_BG3HOFS, 8);
        SetGpuReg(REG_OFFSET_BG3VOFS, 0);
        SetGpuReg(REG_OFFSET_BG2HOFS, 0);
        SetGpuReg(REG_OFFSET_BG2VOFS, 0);
        SetGpuReg(REG_OFFSET_BG1HOFS, 0);
        SetGpuReg(REG_OFFSET_BG1VOFS, 0);
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        ResetSpriteData();
        FreeAllSpritePalettes();
        gMain.state = 1;
        break;
    case 1:
        gIntroCredits_MovingSceneryVBase = 34;
        gIntroCredits_MovingSceneryVOffset = 0;
        LoadCreditsSceneGraphics(scene);
        gMain.state++;
        break;
    case 2:
        gMain.state++;
        break;
    case 3:
        SetBikeScene(scene, taskId);
        SetCreditsSceneBgCnt(scene);
        gMain.state = 0;
        return TRUE;
    }
    return FALSE;
}

static void ResetCreditsTasks(u8 taskId)
{
    // Destroy Task_BicycleBgAnimation, if running
    if (gTasks[taskId].tTaskId_BgScenery != 0)
    {
        DestroyTask(gTasks[taskId].tTaskId_BgScenery);
        gTasks[taskId].tTaskId_BgScenery = 0;
    }

    // Destroy Task_BikeScene, if running
    if (gTasks[taskId].tTaskId_BikeScene != 0)
    {
        DestroyTask(gTasks[taskId].tTaskId_BikeScene);
        gTasks[taskId].tTaskId_BikeScene = 0;
    }

    // Destroy Task_CycleSceneryPalette, if running
    if (gTasks[taskId].tTaskId_SceneryPal != 0)
    {
        DestroyTask(gTasks[taskId].tTaskId_SceneryPal);
        gTasks[taskId].tTaskId_SceneryPal = 0;
    }

    gIntroCredits_MovingSceneryState = INTROCRED_SCENERY_DESTROY;
}

static void LoadTheEndScreen(u16 tileOffsetLoad, u16 tileOffsetWrite, u16 palOffset)
{
    u16 baseTile;
    u16 i;

    LZ77UnCompVram(sCreditsCopyrightEnd_Gfx, (void *)(VRAM + tileOffsetLoad));
    LoadPalette(sCreditsTheEnd_Pal, palOffset, sizeof(sCreditsTheEnd_Pal));

    baseTile = (palOffset / 16) << 12;

    for (i = 0; i < 32 * 32; i++)
        ((u16 *) (VRAM + tileOffsetWrite))[i] = baseTile + 1;
}

static u16 GetLetterMapTile(u8 baseTiles)
{
    u16 out = (baseTiles & 0x3F) + 80;

    if (baseTiles == 0xFF)
        return 1;

    if (baseTiles & (1 << 7))
        out |= 1 << 11;
    if (baseTiles & (1 << 6))
        out |= 1 << 10;

    return out;
}

static void DrawLetterMapTiles(const u8 baseTiles[], u8 baseX, u8 baseY, u16 offset, u16 palette)
{
    u8 y, x;
    const u16 tileOffset = (palette / 16) << 12;

    for (y = 0; y < 5; y++)
    {
        for (x = 0; x < 3; x++)
            ((u16 *) (VRAM + offset + (baseY + y) * 64))[baseX + x] = tileOffset + GetLetterMapTile(baseTiles[y * 3 + x]);
    }
}

static void DrawTheEnd(u16 offset, u16 palette)
{
    u16 pos;
    u16 baseTile = (palette / 16) << 12;

    for (pos = 0; pos < 32 * 32; pos++)
        ((u16 *) (VRAM + offset))[pos] = baseTile + 1;

    DrawLetterMapTiles(sTheEnd_LetterMap_T, 1, 7, offset, palette);
    DrawLetterMapTiles(sTheEnd_LetterMap_H, 5, 7, offset, palette);
    DrawLetterMapTiles(sTheEnd_LetterMap_E, 9, 7, offset, palette);
    DrawLetterMapTiles(sTheEnd_LetterMap_E, 14, 7, offset, palette);
    DrawLetterMapTiles(sTheEnd_LetterMap_N, 18, 7, offset, palette);
    DrawLetterMapTiles(sTheEnd_LetterMap_D, 22, 7, offset, palette);

    // RogueNote: Todo - Swap this out based on percentage
    if(Rogue_UseFinalQuestEffects())
        DrawLetterMapTiles(sTheEnd_LetterMap_FullStop, 26, 7, offset, palette);
    else
        DrawLetterMapTiles(sTheEnd_LetterMap_QMark, 26, 7, offset, palette);
}

#define sState data[0]

static void UNUSED SpriteCB_Player(struct Sprite *sprite)
{
    if (gIntroCredits_MovingSceneryState != INTROCRED_SCENERY_NORMAL)
    {
        DestroySprite(sprite);
        return;
    }

    switch (sprite->sState)
    {
    case 0:
        StartSpriteAnimIfDifferent(sprite, 0);
        break;
    case 1:
        StartSpriteAnimIfDifferent(sprite, 1);
        if (sprite->x > -32)
            sprite->x--;
        break;
    case 2:
        StartSpriteAnimIfDifferent(sprite, 2);
        break;
    case 3:
        StartSpriteAnimIfDifferent(sprite, 3);
        break;
    case 4:
        StartSpriteAnimIfDifferent(sprite, 0);
        if (sprite->x > DISPLAY_WIDTH / 2)
            sprite->x--;
        break;
    case 5:
        StartSpriteAnimIfDifferent(sprite, 0);
        if (sprite->x > -32)
            sprite->x--;
        break;
    }
}

static void UNUSED SpriteCB_Rival(struct Sprite *sprite)
{
    if (gIntroCredits_MovingSceneryState != INTROCRED_SCENERY_NORMAL)
    {
        DestroySprite(sprite);
        return;
    }

    switch (sprite->sState)
    {
    case 0:
        sprite->y2 = 0;
        StartSpriteAnimIfDifferent(sprite, 0);
        break;
    case 1:
        if (sprite->x > 200)
            StartSpriteAnimIfDifferent(sprite, 1);
        else
            StartSpriteAnimIfDifferent(sprite, 2);
        if (sprite->x > -32)
            sprite->x -= 2;
        sprite->y2 = -gIntroCredits_MovingSceneryVOffset;
        break;
    case 2:
        sprite->data[7]++;
        StartSpriteAnimIfDifferent(sprite, 0);
        if ((sprite->data[7] & 3) == 0)
            sprite->x++;
        break;
    case 3:
        StartSpriteAnimIfDifferent(sprite, 0);
        if (sprite->x > -32)
            sprite->x--;
        break;
    }
}

#define sPosition data[1]
#define sSpriteId data[6]

#define sMonSpriteId data[0]

static void SpriteCB_CreditsMonBg(struct Sprite *sprite)
{
    if (gSprites[sprite->sMonSpriteId].data[0] == 10
     || gIntroCredits_MovingSceneryState != INTROCRED_SCENERY_NORMAL)
    {
        DestroySprite(sprite);
        return;
    }

    // Copy sprite data from the associated Pokémon
    sprite->invisible = gSprites[sprite->sMonSpriteId].invisible;
    sprite->oam.objMode = gSprites[sprite->sMonSpriteId].oam.objMode;
    sprite->oam.affineMode = gSprites[sprite->sMonSpriteId].oam.affineMode;
    sprite->oam.matrixNum = gSprites[sprite->sMonSpriteId].oam.matrixNum;
    sprite->x = gSprites[sprite->sMonSpriteId].x;
    sprite->y = gSprites[sprite->sMonSpriteId].y;
}
