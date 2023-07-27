#include "gba/types.h"
#include "gba/defines.h"
#include "global.h"
#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
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
#include "event_object_movement.h"
#include "sound.h"
#include "string_util.h"
#include "international_string_util.h"
#include "trainer_pokemon_sprites.h"
#include "pokemon_icon.h"
#include "graphics.h"
#include "data.h"
#include "pokedex.h"

#include "rogue_player_customisation.h"
#include "rogue_player_customisation_ui.h"

typedef bool8 (*MenuItemInputCallback)();
typedef void (*MenuItemDrawCallback)(u8);

struct RoguePlayerUIState
{
    struct TrainerSpriteInfo trainerFrontSprite;
    u16 trainerObjectEventGfx;

    u8 trainerObjectUpSpriteId;
    u8 trainerObjectUpAnimIdx;
    u8 trainerObjectDownSpriteId;
    u8 trainerObjectDownAnimIdx;
    u8 trainerObjectSideSpriteId;
    u8 trainerObjectSideAnimIdx;

    u8 loadState;

    u8 currentOptionIdx;
};

struct RoguePlayerUIEntry
{
    const char text[16];
    MenuItemInputCallback processInput;
    MenuItemDrawCallback drawChoices;
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

enum
{
    UI_ENTRY_OUTFIT,
    UI_ENTRY_APPEARANCE,
    UI_ENTRY_CLOTHES_COLOUR,

    UI_ENTRY_COUNT
};

static EWRAM_DATA struct RoguePlayerUIState *sRoguePlayerUISavedState = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;

static bool8 RoguePlayerUI_EntryOutfit_ProcessInput();
static void RoguePlayerUI_EntryOutfit_DrawChoices(u8);
static bool8 RoguePlayerUI_EntryAppearance_ProcessInput();
static void RoguePlayerUI_EntryAppearance_DrawChoices(u8);
static bool8 RoguePlayerUI_EntryClothesColour_ProcessInput();
static void RoguePlayerUI_EntryClothesColour_DrawChoices(u8);

#define YPOS_SPACING      16

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

static const struct RoguePlayerUIEntry sRoguePlayerUIEntries[UI_ENTRY_COUNT] = 
{
    [UI_ENTRY_OUTFIT] = 
    {
        .text = _("Outfit"),
        .processInput = RoguePlayerUI_EntryOutfit_ProcessInput,
        .drawChoices = RoguePlayerUI_EntryOutfit_DrawChoices,
    },
    [UI_ENTRY_APPEARANCE] = 
    {
        .text = _("Appearance"),
        .processInput = RoguePlayerUI_EntryAppearance_ProcessInput,
        .drawChoices = RoguePlayerUI_EntryAppearance_DrawChoices,
    },
    [UI_ENTRY_CLOTHES_COLOUR] = 
    {
        .text = _("Outfix Colour"),
        .processInput = RoguePlayerUI_EntryClothesColour_ProcessInput,
        .drawChoices = RoguePlayerUI_EntryClothesColour_DrawChoices,
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
    FONT_GREEN,
    FONT_BRIGHT_GREEN,
};
static const u8 sRoguePlayerUIWindowFontColors[][3] = 
{
    [FONT_BLACK]        = {TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY,  TEXT_COLOR_LIGHT_GRAY},
    [FONT_WHITE]        = {TEXT_COLOR_WHITE, TEXT_COLOR_WHITE,      TEXT_COLOR_DARK_GRAY},
    [FONT_RED]          = {TEXT_COLOR_WHITE, TEXT_COLOR_RED,        TEXT_COLOR_LIGHT_GRAY},
    [FONT_BLUE]         = {TEXT_COLOR_WHITE, TEXT_COLOR_BLUE,       TEXT_COLOR_LIGHT_GRAY},
    [FONT_GREEN]        = {TEXT_COLOR_WHITE, TEXT_COLOR_GREEN,      TEXT_COLOR_LIGHT_GRAY},
    [FONT_BRIGHT_GREEN] = {TEXT_COLOR_WHITE, TEXT_COLOR_GREEN,      TEXT_COLOR_LIGHT_GREEN},
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
static void RoguePlayerUI_PrintTitleText();
static void RoguePlayerUI_PrintMenuText();
static void RoguePlayerUI_DrawTrainerSprites();
static void RoguePlayerUI_FreeTrainerSprites();
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

    sRoguePlayerUISavedState->trainerFrontSprite.spriteId = SPRITE_NONE;
    sRoguePlayerUISavedState->trainerFrontSprite.tileTag = TAG_NONE;

    sRoguePlayerUISavedState->trainerObjectUpSpriteId = SPRITE_NONE;
    sRoguePlayerUISavedState->trainerObjectDownSpriteId = SPRITE_NONE;
    sRoguePlayerUISavedState->trainerObjectSideSpriteId = SPRITE_NONE;
    
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
        RoguePlayerUI_PrintTitleText();
        RoguePlayerUI_PrintMenuText();

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
        RoguePlayerUI_DrawTrainerSprites();

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

    else if (JOY_NEW(DPAD_UP))
    {
        PlaySE(SE_SELECT);

        if(sRoguePlayerUISavedState->currentOptionIdx == 0)
            sRoguePlayerUISavedState->currentOptionIdx = 0; // todo - loop
        else
            --sRoguePlayerUISavedState->currentOptionIdx;
        
        RoguePlayerUI_PrintMenuText();
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);

        ++sRoguePlayerUISavedState->currentOptionIdx; // todo - clamp

        RoguePlayerUI_PrintMenuText();
    }

    //else if (JOY_NEW(DPAD_LEFT))
    //{
    //    PlaySE(SE_SELECT);
    //    RoguePlayerUI_FreeTrainerSprites();
//
    //    // TODO - Move
    //    gSaveBlock2Ptr->playerStyle0 = (gSaveBlock2Ptr->playerStyle0 + 1) % 4;
//
    //    RoguePlayerUI_DrawTrainerSprites(0);
    //    RoguePlayerUI_PrintMenuText();
    //}
    //else if (JOY_NEW(DPAD_RIGHT))
    //{
    //    PlaySE(SE_SELECT);
    //    RoguePlayerUI_FreeTrainerSprites();
//
    //    // TODO - Move
    //    gSaveBlock2Ptr->playerStyle1 = (gSaveBlock2Ptr->playerStyle1 + 1) % 6;
//
    //    RoguePlayerUI_DrawTrainerSprites();
    //    RoguePlayerUI_PrintMenuText();
    //}

    if(sRoguePlayerUISavedState->currentOptionIdx < UI_ENTRY_COUNT)
    {
        if(sRoguePlayerUIEntries[sRoguePlayerUISavedState->currentOptionIdx].processInput())
        {
            PlaySE(SE_SELECT);
            RoguePlayerUI_DrawTrainerSprites();
            RoguePlayerUI_PrintMenuText();
        }
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

static const u8 sText_RoguePlayerUITitle[] = _("TITLE HERE");

static void RoguePlayerUI_PrintTitleText()
{
    FillWindowPixelBuffer(WIN_TITLE, PIXEL_FILL(TEXT_COLOR_WHITE));
    AddTextPrinterParameterized4(WIN_TITLE, FONT_NORMAL, 0, 0, 0, 0, sRoguePlayerUIWindowFontColors[FONT_BLUE], TEXT_SKIP_DRAW, sText_RoguePlayerUITitle);
    CopyWindowToVram(WIN_TITLE, COPYWIN_GFX);
}


static void AddMenuNameText(u8 menuOffset, const char* text)
{
    u8 font = FONT_BLUE;

    if(menuOffset == sRoguePlayerUISavedState->currentOptionIdx)
    {
        font = FONT_GREEN;
    }

    AddTextPrinterParameterized4(WIN_INFO_PANEL, FONT_NARROW, 0, YPOS_SPACING * menuOffset, 0, 0, sRoguePlayerUIWindowFontColors[font], TEXT_SKIP_DRAW, text);
}

static void AddMenuValueText(u8 menuOffset, const char* text)
{
    u8 font = FONT_BLACK;

    if(menuOffset == sRoguePlayerUISavedState->currentOptionIdx)
    {
        font = FONT_BRIGHT_GREEN;
    }

    AddTextPrinterParameterized4(WIN_INFO_PANEL, 
        FONT_SHORT, 
        50 + GetStringCenterAlignXOffset(FONT_SHORT, text, 56), 
        YPOS_SPACING * menuOffset, 
        0, 0, 
        sRoguePlayerUIWindowFontColors[font], 
        TEXT_SKIP_DRAW, 
        text
    );
}

static void RoguePlayerUI_PrintMenuText()
{
    u8 i, y;

    FillWindowPixelBuffer(WIN_INFO_PANEL, PIXEL_FILL(TEXT_COLOR_WHITE));

    for(i = 0; i < UI_ENTRY_COUNT; ++i)
    {
        y = i;
        AddMenuNameText(y, sRoguePlayerUIEntries[i].text);
        sRoguePlayerUIEntries[i].drawChoices(y);
    }

    CopyWindowToVram(WIN_INFO_PANEL, COPYWIN_GFX);
}

static void RoguePlayerUI_DrawTrainerSprites()
{
    const u8 xObjectSpacing = 32;

    // Free any sprites we previously had open
    RoguePlayerUI_FreeTrainerSprites();

    //sRoguePlayerUISavedState->trainerFrontSpriteId = CreateTrainerPicSprite(
    //    TRAINER_PIC_MAY,
    //    TRUE,
    //    64, 92,
    //    0,
    //    TAG_NONE
    //);

    sRoguePlayerUISavedState->trainerFrontSprite = CreateTrainerSprite(
        RoguePlayer_GetTrainerFrontPic(),
        64, 92,
        0, 
        gDecompressionBuffer
    );

    sRoguePlayerUISavedState->trainerObjectEventGfx = OBJ_EVENT_GFX_PLAYER_NORMAL;// RoguePlayer_GetPlayerObjectGfx(PLAYER_AVATAR_STATE_NORMAL);

    if(sRoguePlayerUISavedState->trainerObjectDownSpriteId == SPRITE_NONE)
    {
        sRoguePlayerUISavedState->trainerObjectDownSpriteId = CreateObjectGraphicsSprite(sRoguePlayerUISavedState->trainerObjectEventGfx, SpriteCallbackDummy, 64, 38, 0);

        StartSpriteAnim(&gSprites[sRoguePlayerUISavedState->trainerObjectDownSpriteId], ANIM_STD_GO_SOUTH);
        SeekSpriteAnim(&gSprites[sRoguePlayerUISavedState->trainerObjectDownSpriteId], sRoguePlayerUISavedState->trainerObjectDownAnimIdx);
    }

    if(sRoguePlayerUISavedState->trainerObjectUpSpriteId == SPRITE_NONE)
    {
        sRoguePlayerUISavedState->trainerObjectUpSpriteId = CreateObjectGraphicsSprite(sRoguePlayerUISavedState->trainerObjectEventGfx, SpriteCallbackDummy, 64 - xObjectSpacing, 38, 0);

        StartSpriteAnim(&gSprites[sRoguePlayerUISavedState->trainerObjectUpSpriteId], ANIM_STD_GO_NORTH);
        SeekSpriteAnim(&gSprites[sRoguePlayerUISavedState->trainerObjectUpSpriteId], sRoguePlayerUISavedState->trainerObjectUpAnimIdx);
    }

    if(sRoguePlayerUISavedState->trainerObjectSideSpriteId == SPRITE_NONE)
    {
        sRoguePlayerUISavedState->trainerObjectSideSpriteId = CreateObjectGraphicsSprite(sRoguePlayerUISavedState->trainerObjectEventGfx, SpriteCallbackDummy, 64 + xObjectSpacing, 38, 0);
    
        StartSpriteAnim(&gSprites[sRoguePlayerUISavedState->trainerObjectSideSpriteId], ANIM_STD_GO_EAST);
        SeekSpriteAnim(&gSprites[sRoguePlayerUISavedState->trainerObjectSideSpriteId], sRoguePlayerUISavedState->trainerObjectSideAnimIdx);
    }
}

// As we load above directly into gDecompressionBuffer, we don't actually store the active tiles within the sprite
// so they won't get unloaded unless we manually do it here
#define SAFE_TRAINER_SPRITE_DELETE(trainerInfo) if(trainerInfo.spriteId != SPRITE_NONE) { FreeSpriteTilesByTag(trainerInfo.tileTag); FreeAndDestroyTrainerPicSprite(trainerInfo.spriteId); trainerInfo.spriteId = SPRITE_NONE; }
#define SAFE_OBJECT_SPRITE_DELETE(sprite, animIdx) if(sprite != SPRITE_NONE) { animIdx = gSprites[sprite].animCmdIndex; DestroySprite(&gSprites[sprite]); sprite = SPRITE_NONE; }

static void RoguePlayerUI_FreeTrainerSprites()
{
    SAFE_TRAINER_SPRITE_DELETE(sRoguePlayerUISavedState->trainerFrontSprite);

    // Free the object palette here ourself
    if(sRoguePlayerUISavedState->trainerObjectDownSpriteId != SPRITE_NONE)
    {
        const struct ObjectEventGraphicsInfo *graphicsInfo = GetObjectEventGraphicsInfo(sRoguePlayerUISavedState->trainerObjectEventGfx);
        if(graphicsInfo->paletteTag != TAG_NONE)
        {
            FreeSpritePaletteByTag(graphicsInfo->paletteTag);
        }
    }

    SAFE_OBJECT_SPRITE_DELETE(sRoguePlayerUISavedState->trainerObjectDownSpriteId, sRoguePlayerUISavedState->trainerObjectDownAnimIdx);
    SAFE_OBJECT_SPRITE_DELETE(sRoguePlayerUISavedState->trainerObjectUpSpriteId, sRoguePlayerUISavedState->trainerObjectUpAnimIdx);
    SAFE_OBJECT_SPRITE_DELETE(sRoguePlayerUISavedState->trainerObjectSideSpriteId, sRoguePlayerUISavedState->trainerObjectSideAnimIdx);
}

#undef SAFE_TRAINER_SPRITE_DELETE
#undef SAFE_OBJECT_SPRITE_DELETE

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

static bool8 RoguePlayerUI_EntryOutfit_ProcessInput()
{
    const u16 outfitCount = RoguePlayer_GetOutfitCount();
    u16 outfitId = RoguePlayer_GetOutfitId();

    if(JOY_NEW(DPAD_LEFT))
    {
        if(outfitId == 0)
            outfitId = outfitCount - 1;
        else
            --outfitId;

        RoguePlayer_SetOutfitId(outfitId);
        return TRUE;
    }
    else if(JOY_NEW(DPAD_RIGHT))
    {
        outfitId = (outfitId + 1) % outfitCount;
        RoguePlayer_SetOutfitId(outfitId);

        return TRUE;
    }

    return FALSE;
}

static void RoguePlayerUI_EntryOutfit_DrawChoices(u8 menuOffset)
{
    ConvertUIntToDecimalStringN(gStringVar1, RoguePlayer_GetOutfitId(), STR_CONV_MODE_LEFT_ALIGN, 2);
    AddMenuValueText(menuOffset, gStringVar1);
}

static bool8 RoguePlayerUI_EntryAppearance_ProcessInput()
{
    if(JOY_NEW(DPAD_LEFT))
    {
        if(gSaveBlock2Ptr->playerStyle0 == 0)
            gSaveBlock2Ptr->playerStyle0 = 3;
        else
            --gSaveBlock2Ptr->playerStyle0;
        return TRUE;
    }
    else if(JOY_NEW(DPAD_RIGHT))
    {
        gSaveBlock2Ptr->playerStyle0 = (gSaveBlock2Ptr->playerStyle0 + 1) % 4;
        return TRUE;
    }

    return FALSE;
}

static void RoguePlayerUI_EntryAppearance_DrawChoices(u8 menuOffset)
{
    ConvertUIntToDecimalStringN(gStringVar1, gSaveBlock2Ptr->playerStyle0, STR_CONV_MODE_LEFT_ALIGN, 2);
    AddMenuValueText(menuOffset, gStringVar1);
}


static bool8 RoguePlayerUI_EntryClothesColour_ProcessInput()
{
    if(JOY_NEW(DPAD_LEFT))
    {
        if(gSaveBlock2Ptr->playerStyle1 == 0)
            gSaveBlock2Ptr->playerStyle1 = 5;
        else
            --gSaveBlock2Ptr->playerStyle1;
        return TRUE;
    }
    else if(JOY_NEW(DPAD_RIGHT))
    {
        gSaveBlock2Ptr->playerStyle1 = (gSaveBlock2Ptr->playerStyle1 + 1) % 6;
        return TRUE;
    }

    return FALSE;
}

static void RoguePlayerUI_EntryClothesColour_DrawChoices(u8 menuOffset)
{
    ConvertUIntToDecimalStringN(gStringVar1, gSaveBlock2Ptr->playerStyle1, STR_CONV_MODE_LEFT_ALIGN, 2);
    AddMenuValueText(menuOffset, gStringVar1);
}
