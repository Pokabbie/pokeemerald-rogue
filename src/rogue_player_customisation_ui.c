#include "gba/types.h"
#include "gba/defines.h"
#include "global.h"
#include "constants/trainers.h"

#include "main.h"
#include "bg.h"
#include "text_window.h"
#include "window.h"
#include "characters.h"
#include "palette.h"
#include "task.h"
#include "overworld.h"
#include "malloc.h"
#include "gba/macro.h"
#include "menu_helpers.h"
#include "menu.h"
#include "scanline_effect.h"
#include "sprite.h"
#include "constants/rgb.h"
#include "decompress.h"
#include "constants/songs.h"
#include "field_effect.h"
#include "sound.h"
#include "string_util.h"
#include "trainer_pokemon_sprites.h"
#include "pokemon_icon.h"
#include "graphics.h"
#include "data.h"
#include "pokedex.h"

#include "rogue_player_customisation_ui.h"

struct RoguePlayerUIState
{
    u8 loadState;
    u8 sMode;
    u8 trainerFrontSpriteId;


    u8 sMonIconSpriteId;
    u16 sMonIconDexNum;
};

enum WindowIds
{
    WIN_TITLE,
    WIN_INFO_PANEL
};

enum
{
    BG_PAL_ID_BACKGROUND,
    BG_PAL_ID_WINDOW_FRAMES
};

extern const struct PokedexEntry gPokedexEntries[];

static EWRAM_DATA struct RoguePlayerUIState *sRoguePlayerUISavedState = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;

#define MON_ICON_X   39
#define MON_ICON_Y   36
#define MODE_KANTO   0
#define MODE_JOHTO   1
#define MODE_HOENN   2
#define MODE_SINNOH  3
#define MODE_UNOVA   4
#define MODE_KALOS   5
#define MODE_ALOLA   6
static const u16 sDexRanges[7][2] = {
    [MODE_KANTO]   = {1, 151},
    [MODE_JOHTO]   = {152, 251},
    [MODE_HOENN]   = {252, 386},
    [MODE_SINNOH]  = {387, 493},
    [MODE_UNOVA]   = {494, 649},
    [MODE_KALOS]   = {650, 721},
    [MODE_ALOLA]   = {722, 809}
};
static const u8 sModeNameKanto[] =  _("Kanto");
static const u8 sModeNameJohto[] =  _("Johto");
static const u8 sModeNameHoenn[] =  _("Hoenn");
static const u8 sModeNameSinnoh[] = _("Sinnoh");
static const u8 sModeNameUnova[] =  _("Unova");
static const u8 sModeNameKalos[] =  _("Kalos");
static const u8 sModeNameAlola[] =  _("Alola");
static const u8 *const sModeNames[7] = {
    [MODE_KANTO]   = sModeNameKanto,
    [MODE_JOHTO]   = sModeNameJohto,
    [MODE_HOENN]   = sModeNameHoenn,
    [MODE_SINNOH]  = sModeNameSinnoh,
    [MODE_UNOVA]   = sModeNameUnova,
    [MODE_KALOS]   = sModeNameKalos,
    [MODE_ALOLA]   = sModeNameAlola
};

static const u16 sModeBgColors[] = {
    [MODE_KANTO]   = 0x6a93,
    [MODE_JOHTO]   = 0x527a,
    [MODE_HOENN]   = 0x4f55,
    [MODE_SINNOH]  = 0x4b7c,
    [MODE_UNOVA]   = 0x5ef7,
    [MODE_KALOS]   = 0x76fb,
    [MODE_ALOLA]   = 0x471f,
};

static const struct BgTemplate sRoguePlayerUIBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .priority = 1
    }, 
    {
        .bg = 1,
        .charBaseIndex = 3,
        .mapBaseIndex = 30,
        .priority = 2
    },
};

static const struct WindowTemplate sRoguePlayerUIWindowTemplates[] = 
{
    [WIN_TITLE] = 
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 13,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 1,
    },
    [WIN_INFO_PANEL] =
    {
        .bg = 0,
        .tilemapLeft = 16,
        .tilemapTop = 1,
        .width = 13,
        .height = 18,
        .paletteNum = 15,
        .baseBlock = 1 + (13 * 2),
    }
};

#define FREE_BLOCK_START (1 + (13 * 2) + (13 * 18))

static const u32 sRoguePlayerUITiles[] = INCBIN_U32("graphics/rogue_players/customise_view.4bpp.lz");
static const u32 sRoguePlayerUITilemap[] = INCBIN_U32("graphics/rogue_players/customise_view.bin.lz");
static const u16 sRoguePlayerUIPalette[] = INCBIN_U16("graphics/rogue_players/customise_view.gbapal");

enum FontColor
{
    FONT_BLACK,
    FONT_WHITE,
    FONT_RED,
    FONT_BLUE,
};
static const u8 sRoguePlayerUIWindowFontColors[][3] = 
{
    [FONT_BLACK]  = {TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY,  TEXT_COLOR_LIGHT_GRAY},
    [FONT_WHITE]  = {TEXT_COLOR_WHITE, TEXT_COLOR_WHITE,      TEXT_COLOR_DARK_GRAY},
    [FONT_RED]    = {TEXT_COLOR_WHITE, TEXT_COLOR_RED,        TEXT_COLOR_LIGHT_GRAY},
    [FONT_BLUE]   = {TEXT_COLOR_WHITE, TEXT_COLOR_BLUE,       TEXT_COLOR_LIGHT_GRAY},
};

static void RoguePlayerUI_PreSetupCB(void);
static void RoguePlayerUI_SetupCB(void);
static void RoguePlayerUI_MainCB(void);
static void RoguePlayerUI_VBlankCB(void);

static void Task_RoguePlayerUIWaitFadeIn(u8 taskId);
static void Task_RoguePlayerUIMain(u8 taskId);
static void Task_RoguePlayerUIWaitFadeAndBail(u8 taskId);
static void Task_RoguePlayerUIWaitFadeAndExitGracefully(u8 taskId);

void RoguePlayerUI_Init(MainCallback callback);
static bool8 RoguePlayerUI_InitBgs(void);
static void RoguePlayerUI_FadeAndBail(void);
static bool8 RoguePlayerUI_LoadGraphics(void);
static void RoguePlayerUI_InitWindows(void);
static void RoguePlayerUI_PrintUiButtonHints(u8 windowId, u8 colorIdx);
static void RoguePlayerUI_PrintUiMonInfo(u8 windowId, u8 colorIdx);
static void RoguePlayerUI_DrawTrainerSprites(u16 dexNum);
static void RoguePlayerUI_FreeResources(void);
static void DrawBgWindowFrames(void);

void CB2_InitPlayerCustomisationMenu()
{
    sRoguePlayerUISavedState = AllocZeroed(sizeof(struct RoguePlayerUIState));
    if (sRoguePlayerUISavedState == NULL)
    {
        SetMainCallback2(gMain.savedCallback);
        return;
    }

    sRoguePlayerUISavedState = AllocZeroed(sizeof(struct RoguePlayerUIState));
    sRoguePlayerUISavedState->loadState = 0;
    
    SetMainCallback2(RoguePlayerUI_PreSetupCB);
}

static void RoguePlayerUI_PreSetupCB(void)
{
    if (!gPaletteFade.active)
    {
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(RoguePlayerUI_SetupCB);
    }
}

static void RoguePlayerUI_SetupCB(void)
{
    u8 taskId;
    switch (gMain.state)
    {
    case 0:
        DmaClearLarge16(3, (void *)VRAM, VRAM_SIZE, 0x1000);
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
        if (RoguePlayerUI_InitBgs())
        {
            sRoguePlayerUISavedState->loadState = 0;
            gMain.state++;
        }
        else
        {
            RoguePlayerUI_FadeAndBail();
            return;
        }
        break;
    case 3:
        if (RoguePlayerUI_LoadGraphics() == TRUE)
        {
            gMain.state++;
        }
        break;
    case 4:
        RoguePlayerUI_InitWindows();
        gMain.state++;
        break;
    case 5:
        sRoguePlayerUISavedState->sMode = MODE_KANTO;
        sRoguePlayerUISavedState->sMonIconDexNum = sDexRanges[sRoguePlayerUISavedState->sMode][0];

        RoguePlayerUI_PrintUiButtonHints(WIN_TITLE, FONT_WHITE);
        RoguePlayerUI_PrintUiMonInfo(WIN_INFO_PANEL, FONT_BLACK);

        taskId = CreateTask(Task_RoguePlayerUIWaitFadeIn, 0);
        gMain.state++;
        break;
    case 6:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    case 7:
        SetVBlankCallback(RoguePlayerUI_VBlankCB);
        SetMainCallback2(RoguePlayerUI_MainCB);
        break;
    }
}

static void RoguePlayerUI_MainCB(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void RoguePlayerUI_VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void Task_RoguePlayerUIWaitFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        // Wait for fade to spawn in sprite, as we had some corruption doing it before fade
        RoguePlayerUI_DrawTrainerSprites(sRoguePlayerUISavedState->sMonIconDexNum);

        gTasks[taskId].func = Task_RoguePlayerUIMain;
    }
}

static void Task_RoguePlayerUIMain(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_PC_OFF);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_RoguePlayerUIWaitFadeAndExitGracefully;
    }
    if (JOY_REPEAT(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        FreeAndDestroyTrainerPicSprite(sRoguePlayerUISavedState->trainerFrontSpriteId);
        if (sRoguePlayerUISavedState->sMonIconDexNum < sDexRanges[sRoguePlayerUISavedState->sMode][1])
        {
            sRoguePlayerUISavedState->sMonIconDexNum++;
        }
        else
        {
            sRoguePlayerUISavedState->sMonIconDexNum = sDexRanges[sRoguePlayerUISavedState->sMode][0];
        }
        RoguePlayerUI_DrawTrainerSprites(sRoguePlayerUISavedState->sMonIconDexNum);
        RoguePlayerUI_PrintUiMonInfo(WIN_INFO_PANEL, FONT_BLACK);
    }
    if (JOY_REPEAT(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        FreeAndDestroyTrainerPicSprite(sRoguePlayerUISavedState->trainerFrontSpriteId);
        if (sRoguePlayerUISavedState->sMonIconDexNum > sDexRanges[sRoguePlayerUISavedState->sMode][0])
        {
            sRoguePlayerUISavedState->sMonIconDexNum--;
        }
        else
        {
            sRoguePlayerUISavedState->sMonIconDexNum = sDexRanges[sRoguePlayerUISavedState->sMode][1];
        }
        RoguePlayerUI_DrawTrainerSprites(sRoguePlayerUISavedState->sMonIconDexNum);
        RoguePlayerUI_PrintUiMonInfo(WIN_INFO_PANEL, FONT_BLACK);
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (sRoguePlayerUISavedState->sMode == MODE_ALOLA)
        {
            sRoguePlayerUISavedState->sMode = MODE_KANTO;
        }
        else
        {
            sRoguePlayerUISavedState->sMode++;
        }
        sRoguePlayerUISavedState->sMonIconDexNum = sDexRanges[sRoguePlayerUISavedState->sMode][0];
        FreeAndDestroyTrainerPicSprite(sRoguePlayerUISavedState->trainerFrontSpriteId);

        RoguePlayerUI_DrawTrainerSprites(sRoguePlayerUISavedState->sMonIconDexNum);
        RoguePlayerUI_PrintUiButtonHints(WIN_TITLE, FONT_WHITE);
        RoguePlayerUI_PrintUiMonInfo(WIN_INFO_PANEL, FONT_BLACK);

        //LoadPalette(&sModeBgColors[sRoguePlayerUISavedState->sMode], BG_PLTT_ID(0) + 2, 2);
    }
}

static void Task_RoguePlayerUIWaitFadeAndBail(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(gMain.savedCallback);
        RoguePlayerUI_FreeResources();
        DestroyTask(taskId);
    }
}

static void Task_RoguePlayerUIWaitFadeAndExitGracefully(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(gMain.savedCallback);
        RoguePlayerUI_FreeResources();
        DestroyTask(taskId);
    }
}

#define TILEMAP_BUFFER_SIZE (1024 * 2)
static bool8 RoguePlayerUI_InitBgs(void)
{
    ResetAllBgsCoordinates();

    sBg1TilemapBuffer = Alloc(TILEMAP_BUFFER_SIZE);
    if (sBg1TilemapBuffer == NULL)
    {
        return FALSE;
    }

    memset(sBg1TilemapBuffer, 0, TILEMAP_BUFFER_SIZE);
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sRoguePlayerUIBgTemplates, NELEMS(sRoguePlayerUIBgTemplates));
    SetBgTilemapBuffer(1, sBg1TilemapBuffer);

    ScheduleBgCopyTilemapToVram(1);

    ShowBg(0);
    ShowBg(1);

    return TRUE;
}
#undef TILEMAP_BUFFER_SIZE

static void RoguePlayerUI_FadeAndBail(void)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_RoguePlayerUIWaitFadeAndBail, 0);

    SetVBlankCallback(RoguePlayerUI_VBlankCB);
    SetMainCallback2(RoguePlayerUI_MainCB);
}

static bool8 RoguePlayerUI_LoadGraphics(void)
{
    switch (sRoguePlayerUISavedState->loadState)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, sRoguePlayerUITiles, 0, 0, 0);
        sRoguePlayerUISavedState->loadState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            LZDecompressWram(sRoguePlayerUITilemap, sBg1TilemapBuffer);
            sRoguePlayerUISavedState->loadState++;
        }
        LoadBgTiles(0, GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->tiles, 0x120, FREE_BLOCK_START);
        break;
    case 2:
        LoadPalette(GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->pal, BG_PLTT_ID(BG_PAL_ID_WINDOW_FRAMES), PLTT_SIZE_4BPP);

        LoadPalette(sRoguePlayerUIPalette, BG_PLTT_ID(BG_PAL_ID_BACKGROUND), PLTT_SIZE_4BPP);
        //LoadPalette(&sModeBgColors[MODE_KANTO], BG_PLTT_ID(0) + 2, 2);
        LoadPalette(gMessageBox_Pal, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        sRoguePlayerUISavedState->loadState++;
    default:
        sRoguePlayerUISavedState->loadState = 0;
        return TRUE;
    }
    return FALSE;
}

static void RoguePlayerUI_InitWindows(void)
{
    InitWindows(sRoguePlayerUIWindowTemplates);
    DeactivateAllTextPrinters();
    ScheduleBgCopyTilemapToVram(0);
    FillWindowPixelBuffer(WIN_TITLE, PIXEL_FILL(TEXT_COLOR_WHITE));
    FillWindowPixelBuffer(WIN_INFO_PANEL, PIXEL_FILL(TEXT_COLOR_WHITE));
    PutWindowTilemap(WIN_TITLE);
    PutWindowTilemap(WIN_INFO_PANEL);
    DrawBgWindowFrames();
    CopyWindowToVram(WIN_TITLE, 3);
    CopyWindowToVram(WIN_INFO_PANEL, 3);
}

static const u8 sText_RoguePlayerUIDex[] = _("DEX");
static const u8 sText_RoguePlayerUIButtonHint1[] = _("{DPAD_UPDOWN}Change POKéMON");
static const u8 sText_RoguePlayerUIButtonHint2[] = _("{A_BUTTON}Mode: {STR_VAR_1}");
static const u8 sText_RoguePlayerUIButtonHint3[] = _("{B_BUTTON}Exit");

static void RoguePlayerUI_PrintUiButtonHints(u8 windowId, u8 colorIdx)
{
    StringCopy(gStringVar1, sModeNames[sRoguePlayerUISavedState->sMode]);
    StringExpandPlaceholders(gStringVar2, sText_RoguePlayerUIButtonHint2);

    FillWindowPixelBuffer(windowId, PIXEL_FILL(TEXT_COLOR_WHITE));
    //AddTextPrinterParameterized4(windowId, FONT_NORMAL, 0, 3, 0, 0, sRoguePlayerUIWindowFontColors[colorIdx], TEXT_SKIP_DRAW, sText_RoguePlayerUIDex);
    AddTextPrinterParameterized4(windowId, FONT_SMALL, 0, 0, 0, 0, sRoguePlayerUIWindowFontColors[colorIdx], TEXT_SKIP_DRAW, sText_RoguePlayerUIButtonHint1);
    AddTextPrinterParameterized4(windowId, FONT_SMALL, 0, 10, 0, 0, sRoguePlayerUIWindowFontColors[colorIdx], TEXT_SKIP_DRAW, gStringVar2);
    AddTextPrinterParameterized4(windowId, FONT_SMALL, 0, 20, 0, 0, sRoguePlayerUIWindowFontColors[colorIdx], TEXT_SKIP_DRAW, sText_RoguePlayerUIButtonHint3);
    CopyWindowToVram(windowId, COPYWIN_GFX);
}

static const u8 sText_RoguePlayerUIMonInfoSpecies[] = _("{NO}{STR_VAR_1} {STR_VAR_2}");
static void RoguePlayerUI_PrintUiMonInfo(u8 windowId, u8 colorIdx)
{
    u16 speciesId = NationalPokedexNumToSpecies(sRoguePlayerUISavedState->sMonIconDexNum);

    ConvertIntToDecimalStringN(gStringVar1, sRoguePlayerUISavedState->sMonIconDexNum, STR_CONV_MODE_LEADING_ZEROS, 3);
    StringCopy(gStringVar2, gSpeciesNames[speciesId]);
    StringExpandPlaceholders(gStringVar3, sText_RoguePlayerUIMonInfoSpecies);
    StringCopy(gStringVar4, gPokedexEntries[sRoguePlayerUISavedState->sMonIconDexNum].description);

    FillWindowPixelBuffer(windowId, PIXEL_FILL(TEXT_COLOR_WHITE));
    AddTextPrinterParameterized4(windowId, FONT_SHORT, 0, 0, 0, 0, sRoguePlayerUIWindowFontColors[colorIdx], TEXT_SKIP_DRAW, gStringVar3);
    AddTextPrinterParameterized4(windowId, FONT_SMALL, 0, 5, 0, 0, sRoguePlayerUIWindowFontColors[colorIdx], TEXT_SKIP_DRAW, gStringVar4);
    CopyWindowToVram(windowId, COPYWIN_GFX);
}

static void RoguePlayerUI_DrawTrainerSprites(u16 dexNum)
{ 
    sRoguePlayerUISavedState->trainerFrontSpriteId = CreateTrainerSprite(
        TRAINER_PIC_MAY,
        64, 92,
        0, 
        gDecompressionBuffer
    );
}

static void RoguePlayerUI_FreeResources(void)
{
    if (sRoguePlayerUISavedState != NULL)
    {
        Free(sRoguePlayerUISavedState);
    }
    if (sBg1TilemapBuffer != NULL)
    {
        Free(sBg1TilemapBuffer);
    }
    FreeAllWindowBuffers();
    ResetSpriteData();
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

    for(i = 0; i < ARRAY_COUNT(sRoguePlayerUIWindowTemplates); ++i)
    {
        bg = sRoguePlayerUIWindowTemplates[i].bg;

        left = sRoguePlayerUIWindowTemplates[i].tilemapLeft - 1;
        right = sRoguePlayerUIWindowTemplates[i].tilemapLeft + sRoguePlayerUIWindowTemplates[i].width;
        top = sRoguePlayerUIWindowTemplates[i].tilemapTop - 1;
        bottom = sRoguePlayerUIWindowTemplates[i].tilemapTop + sRoguePlayerUIWindowTemplates[i].height;

        leftValid = left <= 29 && left <= right;
        rightValid = right <= 29 && right >= left;
        topValid = top <= 19 && top <= bottom;
        bottomValid = bottom <= 19 && bottom >= bottom;

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