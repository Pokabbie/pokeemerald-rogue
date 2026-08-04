#include "global.h"
#include "bg.h"
#include "menu.h"
#include "text_window.h"
#include "constants/rgb.h"
#include "scanline_effect.h"
#include "task.h"
#include "gpu_regs.h"
#include "decompress.h"
#include "intro.h"
#include "main.h"
#include "malloc.h"
#include "menu_helpers.h"
#include "palette.h"
#include "sound.h"
#include "sprite.h"
#include "util.h"
#include "emulator_check.h"
#include "config/emulator_check.h"
#include "constants/songs.h"


static void   EmulatorCheck_Init(MainCallback callback);
static void   EmulatorCheck_VBlankCB(void);
static bool32 EmulatorCheck_InitBgs(void);
static bool32 EmulatorCheck_LoadGraphics(void);
static void   EmulatorCheck_InitWindows(void);
static void   EmulatorCheck_PrintErrorText(void);
static void   Task_EmulatorCheckWaitFadeIn(u8 taskId);
static void   Task_EmulatorCheckMainInput(u8 taskId);
static void   Task_EmulatorCheckWaitFadeAndExitGracefully(u8 taskId);
static void   EmulatorCheck_FreeResources(void);
static void   Task_ScrollBG0(u8 taskId);
static void   Task_PulseStripePalette(u8 taskId);
static void   LoadSprites(void);
static void   EmulatorCheck_SetupCB(void);
static void   EmulatorCheck_MainCB(void);

#define TIMER_COUNTUP     0x04

struct EmulatorCheckData
{
    MainCallback savedCallback;
    u8 loadState;
    u8 bg1TilemapBuffer[BG_SCREEN_SIZE];
};
static struct EmulatorCheckData *sEmulatorCheckData;

// window IDs
#define WIN_ERROR_MSG  0
#define WIN_BOTTOM_MSG 1

// BG layers: BG0 = scrolling background (direct hardware write), BG1 = text window
static const struct BgTemplate sBgTemplates[] = {
    {
        .bg            = 0, // scrolling bg layer
        .charBaseIndex = 0,
        .mapBaseIndex  = 8,
        .screenSize    = 0,
        .paletteMode   = 0,
        .priority      = 2,
        .baseTile      = 0,
    },
    {
        .bg            = 1, // text layer
        .charBaseIndex = 2,
        .mapBaseIndex  = 24,
        .screenSize    = 3,
        .paletteMode   = 0,
        .priority      = 1,
        .baseTile      = 0,
    },
};

#define WIN_WIDTH  23
#define WIN_HEIGHT 10

static const struct WindowTemplate sWindowTemplates[] = {
    [WIN_ERROR_MSG] = {
        .bg          = 1,
        .tilemapLeft = 1,
        .tilemapTop  = 5,
        .width       = WIN_WIDTH,
        .height      = WIN_HEIGHT,
        .paletteNum  = 14,
        .baseBlock   = 1,
    },
    [WIN_BOTTOM_MSG] = {
        .bg          = 1,
        .tilemapLeft = 1,
        .tilemapTop  = 16,
        .width       = 17,
        .height      = 2,
        .paletteNum  = 15,
        .baseBlock   = 1 + WIN_WIDTH * WIN_HEIGHT,
    },
    DUMMY_WIN_TEMPLATE,
};

static const u16 sTextBoxMessagePal[] = {
    RGB(0,  0,  0),  // [0] black
    RGB(31, 31, 31), // [1] white     – text foreground
    RGB(14, 14, 14), // [2] mid grey  – text shadow
    RGB(7,  7,  7),  // [3] dark grey – box background
};

static const u16 sTextBoxBottomPal[] = {
    RGB(0,  0,  0),  // [0] black
    RGB(31, 31, 31), // [1] white     – text foreground
    RGB(14, 14, 14), // [2] mid grey  – text shadow
    RGB(7,  7,  7),  // [3] dark grey – box background
};

static const u8 sTextColors_ErrorMsg[] = {TEXT_COLOR_TRANSPARENT, 1, 2};
static const u8 sTextColors_Bottom[] = {TEXT_COLOR_TRANSPARENT, 1, 2};
static const u8 sText_ErrorMessage[] = _(
    "Inaccurate emulator detected!\nPlease use mGBA or one of these:"
    VIABLE_MGBA_RA_IOS
    VIABLE_MGBA_RA_ANDROID
    VIABLE_LEMUROID
    VIABLE_PIZZABOY
);
static const u8 sText_BottomMessage[] = _("Press START to continue.");

static const u32 sErrorScreen_Gfx[] = INCBIN_U32( "graphics/intro/emulator_check/background.4bpp.lz");
static const u32 sErrorScreen_Map[] = INCBIN_U32( "graphics/intro/emulator_check/background.bin.lz");
static const u16 sErrorScreen_Pal[] = INCBIN_U16("graphics/intro/emulator_check/background.gbapal");

static const u32 sWarningLeft_Gfx[]  = INCBIN_U32("graphics/intro/emulator_check/warning_left.4bpp.lz");
static const u32 sWarningRight_Gfx[] = INCBIN_U32("graphics/intro/emulator_check/warning_right.4bpp.lz");
static const u16 sWarning_Pal[]      = INCBIN_U16("graphics/intro/emulator_check/warning.gbapal");

static const u32 sDizzyEgg_Gfx[] = INCBIN_U32("graphics/intro/emulator_check/dizzy_egg.4bpp.lz");
static const u16 sDizzyEgg_Pal[] = INCBIN_U16("graphics/intro/emulator_check/dizzy_egg.gbapal");

static const u32 sPikachu_Gfx[]  = INCBIN_U32("graphics/intro/emulator_check/pikachu.4bpp.lz");
static const u16 sPikachu_Pal[]  = INCBIN_U16("graphics/intro/emulator_check/pikachu.gbapal");

static const u32 sPorygon_Gfx[]  = INCBIN_U32("graphics/intro/emulator_check/porygon.4bpp.lz");
static const u16 sPorygon_Pal[]  = INCBIN_U16("graphics/intro/emulator_check/porygon.gbapal");

// using dummy tag IDs since these are only loaded once during this check UI and will be freed afterwards
#define TAG_EMUCHECK_WARNING_LEFT  0
#define TAG_EMUCHECK_WARNING_RIGHT 1
#define TAG_EMUCHECK_DIZZY_EGG     2
#define TAG_EMUCHECK_PIKACHU       3
#define TAG_EMUCHECK_PORYGON       4

static const struct CompressedSpriteSheet sSpriteSheets[] = {
    { sWarningLeft_Gfx,  0x400, TAG_EMUCHECK_WARNING_LEFT  },
    { sWarningRight_Gfx, 0x400, TAG_EMUCHECK_WARNING_RIGHT },
    { sDizzyEgg_Gfx,     0x800, TAG_EMUCHECK_DIZZY_EGG     },
    { sPikachu_Gfx,      0x800, TAG_EMUCHECK_PIKACHU       },
    { sPorygon_Gfx,      0x800, TAG_EMUCHECK_PORYGON       },
};

static const struct SpritePalette sSpritePalettes[] = {
    { sWarning_Pal,  TAG_EMUCHECK_WARNING_LEFT  },
    { sWarning_Pal,  TAG_EMUCHECK_WARNING_RIGHT },
    { sDizzyEgg_Pal, TAG_EMUCHECK_DIZZY_EGG     },
    { sPikachu_Pal,  TAG_EMUCHECK_PIKACHU       },
    { sPorygon_Pal,  TAG_EMUCHECK_PORYGON       },
    {},
};

static const struct OamData sOamData_WarningLeft = {
    .size       = SPRITE_SIZE(64x32),
    .shape      = SPRITE_SHAPE(64x32),
    .priority   = 0,
    .paletteNum = 0,
};

static const struct OamData sOamData_WarningRight = {
    .size       = SPRITE_SIZE(64x32),
    .shape      = SPRITE_SHAPE(64x32),
    .priority   = 0,
    .paletteNum = 0,
};

static const struct OamData sOamData_DizzyEgg = {
    .size       = SPRITE_SIZE(64x64),
    .shape      = SPRITE_SHAPE(64x64),
    .priority   = 0,
    .paletteNum = 1,
};

static const struct OamData sOamData_Pikachu = {
    .size       = SPRITE_SIZE(64x64),
    .shape      = SPRITE_SHAPE(64x64),
    .priority   = 0,
    .paletteNum = 2,
};

static const struct OamData sOamData_Porygon = {
    .size       = SPRITE_SIZE(64x64),
    .shape      = SPRITE_SHAPE(64x64),
    .priority   = 0,
    .paletteNum = 3,
};

static const struct SpriteTemplate sSpriteTemplate_WarningLeft = {
    .tileTag    = TAG_EMUCHECK_WARNING_LEFT,
    .paletteTag = TAG_EMUCHECK_WARNING_LEFT,
    .oam        = &sOamData_WarningLeft,
    .anims      = gDummySpriteAnimTable,
    .images     = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback   = SpriteCallbackDummy,
};

static const struct SpriteTemplate sSpriteTemplate_WarningRight = {
    .tileTag    = TAG_EMUCHECK_WARNING_RIGHT,
    .paletteTag = TAG_EMUCHECK_WARNING_RIGHT,
    .oam        = &sOamData_WarningRight,
    .anims      = gDummySpriteAnimTable,
    .images     = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback   = SpriteCallbackDummy,
};

static const struct SpriteTemplate sSpriteTemplate_DizzyEgg = {
    .tileTag    = TAG_EMUCHECK_DIZZY_EGG,
    .paletteTag = TAG_EMUCHECK_DIZZY_EGG,
    .oam        = &sOamData_DizzyEgg,
    .anims      = gDummySpriteAnimTable,
    .images     = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback   = SpriteCallbackDummy,
};

static const struct SpriteTemplate sSpriteTemplate_Pikachu = {
    .tileTag    = TAG_EMUCHECK_PIKACHU,
    .paletteTag = TAG_EMUCHECK_PIKACHU,
    .oam        = &sOamData_Pikachu,
    .anims      = gDummySpriteAnimTable,
    .images     = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback   = SpriteCallbackDummy,
};

static const struct SpriteTemplate sSpriteTemplate_Porygon = {
    .tileTag    = TAG_EMUCHECK_PORYGON,
    .paletteTag = TAG_EMUCHECK_PORYGON,
    .oam        = &sOamData_Porygon,
    .anims      = gDummySpriteAnimTable,
    .images     = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback   = SpriteCallbackDummy,
};


// this file's functions

bool32 IsInaccurateEmulator(void)
{
    // return TRUE; // for testing of the error screen, comment for actual detection

#if (ACTIVE_EMU_CHECKS & EMU_CHECK_REG_IMC)
    {
        vu32 *const reg_imc = (vu32 *)0x04000800;
        const u32 reg_imc_reset_value = 0x0D000020u;

        if (*reg_imc != reg_imc_reset_value)
            return TRUE;
    }
#endif
#if (ACTIVE_EMU_CHECKS & EMU_CHECK_TIMER_CASCADE)
    {
        u16 result;
        REG_TM2CNT_H = 0;                            // stop TM2
        REG_TM3CNT_H = 0;                            // stop TM3
        REG_TM2CNT_L = 0xFFFF;                       // one tick to overflow
        REG_TM3CNT_L = 0;                            // TM3 starts at 0
        REG_TM3CNT_H = TIMER_ENABLE | TIMER_COUNTUP; // TM3 counts TM2 overflows
        REG_TM2CNT_H = TIMER_ENABLE;                 // start TM2 (F/1 prescaler)

        // TM2 overflows within a few cycles; TM3 should now be >= 1
        result = REG_TM3CNT_L;

        // clean up and restore sound timers to avoid breaking m4a on some emulators
        REG_TM2CNT_H = 0;
        REG_TM3CNT_H = 0;

        if (result == 0)
            return TRUE;
    }
#endif
#if (ACTIVE_EMU_CHECKS & EMU_CHECK_SOUNDCNT_H)
    {
        u16 result;
        vu16 *const soundcnt_x = (vu16 *)0x04000084;
        vu16 *const soundcnt_h = (vu16 *)0x04000082;
        u16 savedX = *soundcnt_x;
        u16 savedH = *soundcnt_h;
        
        *soundcnt_x = 0x0080;  // enable master sound
        *soundcnt_h = 0xFFFF;
        result = *soundcnt_h;
        *soundcnt_h = savedH;
        *soundcnt_x = savedX;
        if (result != 0x770F)
            return TRUE;
    }
#endif
#if (ACTIVE_EMU_CHECKS & EMU_CHECK_KEYCNT)
    {
        vu16 *const keycnt = (vu16 *)0x04000132;
        const u16 keycnt_expected = 0xC3FF;
        u16 saved = *keycnt;
        u16 result;

        *keycnt = 0xFFFF;
        result = *keycnt;
        *keycnt = saved;
        if (result != keycnt_expected)
            return TRUE;
    }
#endif
#if (ACTIVE_EMU_CHECKS & EMU_CHECK_SOUNDCNT_L)
    {
        vu16 *const soundcnt_x = (vu16 *)0x04000084;
        vu16 *const soundcnt_l = (vu16 *)0x04000080;
        u16 savedX = *soundcnt_x;
        u16 savedL = *soundcnt_l;
        u16 result;

        *soundcnt_x = 0x0080;  // enable master sound
        *soundcnt_l = 0xFFFF;
        result = *soundcnt_l;
        *soundcnt_l = savedL;
        *soundcnt_x = savedX;
        if (result != 0xFF77)
            return TRUE;
    }
#endif

    return FALSE;
}

void RunEmulatorCheckUI(MainCallback callback)
{
    if (!gPaletteFade.active)
    {
        EmulatorCheck_Init(callback);
    }
}

static void EmulatorCheck_Init(MainCallback callback)
{
    sEmulatorCheckData = AllocZeroed(sizeof(struct EmulatorCheckData));
    sEmulatorCheckData->savedCallback = callback;

    SetMainCallback2(EmulatorCheck_SetupCB);
}

static void EmulatorCheck_VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static bool32 EmulatorCheck_InitBgs(void)
{
    ResetAllBgsCoordinates();
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sBgTemplates, NELEMS(sBgTemplates));

    SetBgTilemapBuffer(1, sEmulatorCheckData->bg1TilemapBuffer);
    ScheduleBgCopyTilemapToVram(1);

    ShowBg(0);
    ShowBg(1);

    return TRUE;
}

static bool32 EmulatorCheck_LoadGraphics(void)
{
    switch (sEmulatorCheckData->loadState)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(0, sErrorScreen_Gfx, 0, 0, 0);
        sEmulatorCheckData->loadState++;
        break;
    case 1:
        LZDecompressVram(sErrorScreen_Map, (void *)BG_SCREEN_ADDR(8));
        sEmulatorCheckData->loadState++;
        break;
    case 2:
        LoadPalette(sErrorScreen_Pal, BG_PLTT_ID(3), sizeof(sErrorScreen_Pal));
        LoadPalette(sTextBoxMessagePal, BG_PLTT_ID(14), sizeof(sTextBoxMessagePal));
        LoadPalette(sTextBoxBottomPal, BG_PLTT_ID(15), sizeof(sTextBoxBottomPal));
        sEmulatorCheckData->loadState++;
        break;
    case 3:
        LoadSprites();
        sEmulatorCheckData->loadState = 0;
        return TRUE;
    default:
        sEmulatorCheckData->loadState = 0;
        return TRUE;
    }
    return FALSE;
}

static void EmulatorCheck_InitWindows(void)
{
    InitWindows(sWindowTemplates);
    DeactivateAllTextPrinters();
    ScheduleBgCopyTilemapToVram(0);

    // main text    
    PutWindowTilemap(WIN_ERROR_MSG);
    CopyWindowToVram(WIN_ERROR_MSG, 3);

    // optional continue text
    if (ALLOW_BOOT_CONTINUATION)
    {
        PutWindowTilemap(WIN_BOTTOM_MSG);
        CopyWindowToVram(WIN_BOTTOM_MSG, 3);
    }
}

static void EmulatorCheck_PrintErrorText(void)
{
    // main text
    FillWindowPixelBuffer(WIN_ERROR_MSG, PIXEL_FILL(0));
    AddTextPrinterParameterized3(WIN_ERROR_MSG, FONT_SMALL, 3, 1, sTextColors_ErrorMsg, TEXT_SKIP_DRAW, sText_ErrorMessage);
    CopyWindowToVram(WIN_ERROR_MSG, COPYWIN_GFX);

    // optional continue text
    if (ALLOW_BOOT_CONTINUATION)
    {
        FillWindowPixelBuffer(WIN_BOTTOM_MSG, PIXEL_FILL(0));
        AddTextPrinterParameterized3(WIN_BOTTOM_MSG, FONT_NORMAL, 3, 1, sTextColors_Bottom, TEXT_SKIP_DRAW, sText_BottomMessage);
        CopyWindowToVram(WIN_BOTTOM_MSG, COPYWIN_GFX);
    }
}

static void Task_EmulatorCheckWaitFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gTasks[taskId].func = Task_EmulatorCheckMainInput;
    }
}

static void Task_EmulatorCheckMainInput(u8 taskId)
{
    if (JOY_NEW(START_BUTTON) && ALLOW_BOOT_CONTINUATION)
    {
        PlaySE(SE_PC_OFF);
        FadeOutBGM(4);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_EmulatorCheckWaitFadeAndExitGracefully;
    }
    else if (gMain.newKeysRaw != 0)
        PlaySE(SE_BOO);
}

static void Task_EmulatorCheckWaitFadeAndExitGracefully(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sEmulatorCheckData->savedCallback);
        EmulatorCheck_FreeResources();
        DestroyTask(taskId);
    }
}

static void EmulatorCheck_FreeResources(void)
{
    FREE_AND_SET_NULL(sEmulatorCheckData);
    FreeAllWindowBuffers();
    ResetSpriteData();
}

#define tScrollPos data[0]
#define tTimer     data[1]

static void Task_ScrollBG0(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    tTimer++;
    if (tTimer > ANIM_SPEED)
    {
        tTimer = 0;
        tScrollPos++;
        SetGpuReg(REG_OFFSET_BG0HOFS, -(tScrollPos * 2));
    }
}

#undef tScrollPos
#undef tTimer

#if FADING_STRIPES
#define tPhase data[0]
#define tDir   data[1]

static void Task_PulseStripePalette(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u8 coeff;

    tPhase++;
    if (tPhase >= PULSE_FRAMES)
    {
        tPhase = 0;
        tDir ^= 1;
    }

    coeff = (u8)((tPhase * PULSE_MAX_BLEND) / PULSE_FRAMES);
    if (tDir != 0)
        coeff = PULSE_MAX_BLEND - coeff;

    BlendPalette(BG_PLTT_ID(3), 16, coeff, RGB_BLACK);
}

#undef tPhase
#undef tDir
#endif // FADING_STRIPES

static void LoadSprites(void)
{
    LoadCompressedSpriteSheet(&sSpriteSheets[0]);
    LoadCompressedSpriteSheet(&sSpriteSheets[1]);
    LoadCompressedSpriteSheet(&sSpriteSheets[2]);
    LoadCompressedSpriteSheet(&sSpriteSheets[3]);
    LoadCompressedSpriteSheet(&sSpriteSheets[4]);
    LoadSpritePalettes(sSpritePalettes);

    CreateSprite(&sSpriteTemplate_WarningLeft, WARNING_X, WARNING_Y + 16, 0);
    CreateSprite(&sSpriteTemplate_WarningRight, WARNING_X + 64, WARNING_Y + 16, 0);
    if (SPRITE_MODE == MODE_EXPANSION)
        CreateSprite(&sSpriteTemplate_DizzyEgg, DIZZY_EGG_X, DIZZY_EGG_Y + 32, 0);
    if (SPRITE_MODE == MODE_VANILLA)
        CreateSprite(&sSpriteTemplate_Pikachu, PIKACHU_X, PIKACHU_Y + 32, 0);
    CreateSprite(&sSpriteTemplate_Porygon, PORYGON_X, PORYGON_Y + 32, 0);
}

static void EmulatorCheck_SetupCB(void)
{
    switch (gMain.state)
    {
    case 0:
        SetVBlankHBlankCallbacksToNull();
        ClearScheduledBgCopiesToVram();
        gMain.state++;
        break;
    case 1:
        ScanlineEffect_Stop();
        FreeAllSpritePalettes();
        ResetPaletteFade();
        ResetSpriteData();
        ResetTasks();
        gMain.state++;
        break;
    case 2:
        if (EmulatorCheck_InitBgs())
        {
            sEmulatorCheckData->loadState = 0;
            gMain.state++;
        }
        break;
    case 3:
        if (EmulatorCheck_LoadGraphics() == TRUE)
            gMain.state++;
        break;
    case 4:
        EmulatorCheck_InitWindows();
        gMain.state++;
        break;
    case 5:
        EmulatorCheck_PrintErrorText();
        CreateTask(Task_EmulatorCheckWaitFadeIn, 0);
        gMain.state++;
        break;
    case 6:
        // BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    case 7:
        PlayBGM(ERROR_MUSIC);
        CreateTask(Task_ScrollBG0, 1);
#if FADING_STRIPES
        CreateTask(Task_PulseStripePalette, 1);
#endif
        gMain.state++;
        break;
    case 8:
        TransferPlttBuffer();
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG1_ON);
        gMain.state = 9;
        break;
    case 9:
        SetVBlankCallback(EmulatorCheck_VBlankCB);
        SetMainCallback2(EmulatorCheck_MainCB);
        break;
    }
}

static void EmulatorCheck_MainCB(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}
