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
#include "strings.h"
#include "international_string_util.h"
#include "trainer_pokemon_sprites.h"
#include "pokemon_icon.h"
#include "graphics.h"
#include "data.h"
#include "pokedex.h"

#include "rogue_player_customisation.h"
#include "rogue_player_customisation_ui.h"
#include "rogue_timeofday.h"

#define TOTAL_UI_PAGE_ENTRIES 9
#define MAX_UI_PAGE_DEPTH 4

typedef bool8 (*MenuItemInputCallback)(u8, u8);
typedef void (*MenuItemDrawCallback)(u8, u8);

enum
{
    COLOUR_EDIT_FAST,
    COLOUR_EDIT_ACCURATE,
};

struct RoguePlayerUIState
{
    struct TrainerSpriteInfo trainerFrontSprite;
    u16 currentPageEntries[TOTAL_UI_PAGE_ENTRIES];
    u8 stackCurrentPageIdx[MAX_UI_PAGE_DEPTH];
    u8 stackCurrentOptionIdx[MAX_UI_PAGE_DEPTH];
    u8 outfitStyleR[PLAYER_OUTFIT_STYLE_COUNT];
    u8 outfitStyleG[PLAYER_OUTFIT_STYLE_COUNT];
    u8 outfitStyleB[PLAYER_OUTFIT_STYLE_COUNT];
    u16 trainerObjectEventGfx;

    u8 trainerObjectUpSpriteId;
    u8 trainerObjectUpAnimIdx;
    u8 trainerObjectDownSpriteId;
    u8 trainerObjectDownAnimIdx;
    u8 trainerObjectSideSpriteId;
    u8 trainerObjectSideAnimIdx;

    u8 loadState;
    u8 pageStackDepth;

    u8 currentPageIdx;
    u8 currentOptionIdx;

    u8 colourEditMode;
};


struct RoguePlayerUIEntry
{
    const u8 text[24];
    MenuItemInputCallback processInput;
    MenuItemDrawCallback drawChoices;
    union
    {
        u8 val8[4];
        u16 val16[2];
        u32 val32;
    } userData;
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
    UI_ENTRY_BACK,
    UI_ENTRY_EXIT,
    UI_ENTRY_OUTFIT,
    UI_ENTRY_RANDOMISE_EVERYTHING,
    UI_ENTRY_RANDOMISE_COLOURS,

    UI_ENTRY_EDIT_APPEARANCE,
    UI_ENTRY_EDIT_PRIMARY,
    UI_ENTRY_EDIT_SECONDARY,

    UI_ENTRY_APPEARANCE_COLOUR,
    UI_ENTRY_APPEARANCE_R,
    UI_ENTRY_APPEARANCE_G,
    UI_ENTRY_APPEARANCE_B,

    UI_ENTRY_PRIMARY_COLOUR,
    UI_ENTRY_PRIMARY_R,
    UI_ENTRY_PRIMARY_G,
    UI_ENTRY_PRIMARY_B,

    UI_ENTRY_SECONDARY_COLOUR,
    UI_ENTRY_SECONDARY_R,
    UI_ENTRY_SECONDARY_G,
    UI_ENTRY_SECONDARY_B,

    UI_ENTRY_COUNT
};

enum
{
    UI_PAGE_MAIN,
    UI_PAGE_EDIT_APPEARANCE,
    UI_PAGE_EDIT_PRIMARY,
    UI_PAGE_EDIT_SECONDARY,
    UI_PAGE_COUNT,
};

enum
{
    COLOUR_CHANNEL_R,
    COLOUR_CHANNEL_G,
    COLOUR_CHANNEL_B,
};

static EWRAM_DATA struct RoguePlayerUIState *sPlayerOutfitUIState = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;

static bool8 RoguePlayerUI_EntryOpenPage_ProcessInput(u8, u8);
static bool8 RoguePlayerUI_EntryOutfit_ProcessInput(u8, u8);
static void RoguePlayerUI_EntryOutfit_DrawChoices(u8, u8);
static bool8 RoguePlayerUI_EntryRandomise_ProcessInput(u8, u8);
static bool8 RoguePlayerUI_EntryClothesStyleRGB_ProcessInput(u8, u8);
static void RoguePlayerUI_EntryClothesStyleRGB_DrawChoices(u8, u8);
static bool8 RoguePlayerUI_EntryClothesStylePreset_ProcessInput(u8, u8);
static void RoguePlayerUI_EntryClothesStylePreset_DrawChoices(u8, u8);

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
    [UI_ENTRY_BACK] = 
    {
        .text = _("Back"),
        .processInput = NULL,
        .drawChoices = NULL,
    },
    [UI_ENTRY_EXIT] = 
    {
        .text = _("Save & Exit"),
        .processInput = NULL,
        .drawChoices = NULL,
    },

    [UI_ENTRY_OUTFIT] = 
    {
        .text = _("Outfit"),
        .processInput = RoguePlayerUI_EntryOutfit_ProcessInput,
        .drawChoices = RoguePlayerUI_EntryOutfit_DrawChoices,
    },

    [UI_ENTRY_RANDOMISE_EVERYTHING] = 
    {
        .text = _("Randomise Everything"),
        .processInput = RoguePlayerUI_EntryRandomise_ProcessInput,
        .drawChoices = NULL,
        .userData = 
        {
            .val32 = TRUE
        }
    },
    [UI_ENTRY_RANDOMISE_COLOURS] = 
    {
        .text = _("Randomise Colours"),
        .processInput = RoguePlayerUI_EntryRandomise_ProcessInput,
        .drawChoices = NULL,
        .userData = 
        {
            .val32 = FALSE
        }
    },

    [UI_ENTRY_EDIT_APPEARANCE] = 
    {
        .text = _("Appearance…"),
        .processInput = RoguePlayerUI_EntryOpenPage_ProcessInput,
        .drawChoices = NULL,
        .userData = 
        {
            .val32 = UI_PAGE_EDIT_APPEARANCE
        }
    },
    [UI_ENTRY_EDIT_PRIMARY] = 
    {
        .text = _("Primary Colour…"),
        .processInput = RoguePlayerUI_EntryOpenPage_ProcessInput,
        .drawChoices = NULL,
        .userData = 
        {
            .val32 = UI_PAGE_EDIT_PRIMARY
        }
    },
    [UI_ENTRY_EDIT_SECONDARY] = 
    {
        .text = _("Secondary Colour…"),
        .processInput = RoguePlayerUI_EntryOpenPage_ProcessInput,
        .drawChoices = NULL,
        .userData = 
        {
            .val32 = UI_PAGE_EDIT_SECONDARY
        }
    },

    [UI_ENTRY_APPEARANCE_COLOUR] = 
    {
        .text = _("Colour"),
        .processInput = RoguePlayerUI_EntryClothesStylePreset_ProcessInput,
        .drawChoices = RoguePlayerUI_EntryClothesStylePreset_DrawChoices,
        .userData = 
        {
            .val8 = 
            {
                PLAYER_OUTFIT_STYLE_APPEARANCE
            }
        }
    },
    [UI_ENTRY_APPEARANCE_R] = 
    {
        .text = _("R"),
        .processInput = RoguePlayerUI_EntryClothesStyleRGB_ProcessInput,
        .drawChoices = RoguePlayerUI_EntryClothesStyleRGB_DrawChoices,
        .userData = 
        {
            .val8 = 
            {
                PLAYER_OUTFIT_STYLE_APPEARANCE,
                COLOUR_CHANNEL_R,
            }
        }
    },
    [UI_ENTRY_APPEARANCE_G] = 
    {
        .text = _("G"),
        .processInput = RoguePlayerUI_EntryClothesStyleRGB_ProcessInput,
        .drawChoices = RoguePlayerUI_EntryClothesStyleRGB_DrawChoices,
        .userData = 
        {
            .val8 = 
            {
                PLAYER_OUTFIT_STYLE_APPEARANCE,
                COLOUR_CHANNEL_G,
            }
        }
    },
    [UI_ENTRY_APPEARANCE_B] = 
    {
        .text = _("B"),
        .processInput = RoguePlayerUI_EntryClothesStyleRGB_ProcessInput,
        .drawChoices = RoguePlayerUI_EntryClothesStyleRGB_DrawChoices,
        .userData = 
        {
            .val8 = 
            {
                PLAYER_OUTFIT_STYLE_APPEARANCE,
                COLOUR_CHANNEL_B,
            }
        }
    },

    [UI_ENTRY_PRIMARY_COLOUR] = 
    {
        .text = _("Colour"),
        .processInput = RoguePlayerUI_EntryClothesStylePreset_ProcessInput,
        .drawChoices = RoguePlayerUI_EntryClothesStylePreset_DrawChoices,
        .userData = 
        {
            .val8 = 
            {
                PLAYER_OUTFIT_STYLE_PRIMARY
            }
        }
    },
    [UI_ENTRY_PRIMARY_R] = 
    {
        .text = _("R"),
        .processInput = RoguePlayerUI_EntryClothesStyleRGB_ProcessInput,
        .drawChoices = RoguePlayerUI_EntryClothesStyleRGB_DrawChoices,
        .userData =  
        {
            .val8 = 
            {
                PLAYER_OUTFIT_STYLE_PRIMARY,
                COLOUR_CHANNEL_R,
            }
        }
    },
    [UI_ENTRY_PRIMARY_G] = 
    {
        .text = _("G"),
        .processInput = RoguePlayerUI_EntryClothesStyleRGB_ProcessInput,
        .drawChoices = RoguePlayerUI_EntryClothesStyleRGB_DrawChoices,
        .userData =  
        {
            .val8 = 
            {
                PLAYER_OUTFIT_STYLE_PRIMARY,
                COLOUR_CHANNEL_G,
            }
        }
    },
    [UI_ENTRY_PRIMARY_B] = 
    {
        .text = _("B"),
        .processInput = RoguePlayerUI_EntryClothesStyleRGB_ProcessInput,
        .drawChoices = RoguePlayerUI_EntryClothesStyleRGB_DrawChoices,
        .userData =  
        {
            .val8 = 
            {
                PLAYER_OUTFIT_STYLE_PRIMARY,
                COLOUR_CHANNEL_B,
            }
        }
    },
    
    [UI_ENTRY_SECONDARY_COLOUR] = 
    {
        .text = _("Colour"),
        .processInput = RoguePlayerUI_EntryClothesStylePreset_ProcessInput,
        .drawChoices = RoguePlayerUI_EntryClothesStylePreset_DrawChoices,
        .userData = 
        {
            .val8 = 
            {
                PLAYER_OUTFIT_STYLE_SECONDARY
            }
        }
    },
    [UI_ENTRY_SECONDARY_R] = 
    {
        .text = _("R"),
        .processInput = RoguePlayerUI_EntryClothesStyleRGB_ProcessInput,
        .drawChoices = RoguePlayerUI_EntryClothesStyleRGB_DrawChoices,
        .userData =  
        {
            .val8 = 
            {
                PLAYER_OUTFIT_STYLE_SECONDARY,
                COLOUR_CHANNEL_R,
            }
        }
    },
    [UI_ENTRY_SECONDARY_G] = 
    {
        .text = _("G"),
        .processInput = RoguePlayerUI_EntryClothesStyleRGB_ProcessInput,
        .drawChoices = RoguePlayerUI_EntryClothesStyleRGB_DrawChoices,
        .userData =  
        {
            .val8 = 
            {
                PLAYER_OUTFIT_STYLE_SECONDARY,
                COLOUR_CHANNEL_G,
            }
        }
    },
    [UI_ENTRY_SECONDARY_B] = 
    {
        .text = _("B"),
        .processInput = RoguePlayerUI_EntryClothesStyleRGB_ProcessInput,
        .drawChoices = RoguePlayerUI_EntryClothesStyleRGB_DrawChoices,
        .userData =  
        {
            .val8 = 
            {
                PLAYER_OUTFIT_STYLE_SECONDARY,
                COLOUR_CHANNEL_B,
            }
        }
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

static void RoguePlayerUI_OpenPage(u8 pageId);
static bool8 RoguePlayerUI_ClosePage();
static bool8 RoguePlayerUI_HasSubpageOpen();

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

static void RefreshUIOutfitStylesFromSource()
{
    u8 i;
    for(i = 0; i < PLAYER_OUTFIT_STYLE_COUNT; ++i)
    {
        u16 colour = RoguePlayer_GetOutfitStyle(i);
        sPlayerOutfitUIState->outfitStyleR[i] = RGB_CONVERT_TO_UI_RANGE(GET_R(colour));
        sPlayerOutfitUIState->outfitStyleG[i] = RGB_CONVERT_TO_UI_RANGE(GET_G(colour));
        sPlayerOutfitUIState->outfitStyleB[i] = RGB_CONVERT_TO_UI_RANGE(GET_B(colour));
    }
}

void CB2_InitPlayerCustomisationMenu()
{
    sPlayerOutfitUIState = AllocZeroed(sizeof(struct RoguePlayerUIState));
    if (sPlayerOutfitUIState == NULL)
    {
        SetMainCallback2(gMain.savedCallback);
        return;
    }

    RogueToD_SetTempDisableTimeVisuals(TRUE);

    sPlayerOutfitUIState = AllocZeroed(sizeof(struct RoguePlayerUIState));
    
    sPlayerOutfitUIState->loadState = 0;
    sPlayerOutfitUIState->pageStackDepth = 0;
    
    sPlayerOutfitUIState->trainerFrontSprite.spriteId = SPRITE_NONE;
    sPlayerOutfitUIState->trainerFrontSprite.tileTag = TAG_NONE;

    sPlayerOutfitUIState->trainerObjectUpSpriteId = SPRITE_NONE;
    sPlayerOutfitUIState->trainerObjectDownSpriteId = SPRITE_NONE;
    sPlayerOutfitUIState->trainerObjectSideSpriteId = SPRITE_NONE;

    RefreshUIOutfitStylesFromSource();

    RoguePlayerUI_OpenPage(UI_PAGE_MAIN);

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
            sPlayerOutfitUIState->loadState = 0;
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

        CreateTask(Task_RoguePlayerUIWaitFadeIn, 0);
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

static void RoguePlayerUI_RefreshPageEntries()
{
    u8 i;
    bool8 anySupported;

    for(i = 0; i < TOTAL_UI_PAGE_ENTRIES; ++i)
        sPlayerOutfitUIState->currentPageEntries[i] = UI_ENTRY_COUNT;

    i = 0;

    switch (sPlayerOutfitUIState->currentPageIdx)
    {
    case UI_PAGE_MAIN:
        sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_OUTFIT;
        //sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_RANDOMISE_EVERYTHING;

        anySupported = 
            RoguePlayer_SupportsOutfitStyle(PLAYER_OUTFIT_STYLE_APPEARANCE) ||
            RoguePlayer_SupportsOutfitStyle(PLAYER_OUTFIT_STYLE_PRIMARY) ||
            RoguePlayer_SupportsOutfitStyle(PLAYER_OUTFIT_STYLE_SECONDARY);

        if(RoguePlayer_SupportsOutfitStyle(PLAYER_OUTFIT_STYLE_APPEARANCE))
            sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_EDIT_APPEARANCE;

        if(RoguePlayer_SupportsOutfitStyle(PLAYER_OUTFIT_STYLE_PRIMARY))
            sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_EDIT_PRIMARY;

        if(RoguePlayer_SupportsOutfitStyle(PLAYER_OUTFIT_STYLE_SECONDARY))
            sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_EDIT_SECONDARY;

        if(anySupported)
            sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_RANDOMISE_COLOURS;

        sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_EXIT;
        break;

    case UI_PAGE_EDIT_APPEARANCE:
        sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_APPEARANCE_COLOUR;
        sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_APPEARANCE_R;
        sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_APPEARANCE_G;
        sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_APPEARANCE_B;
        sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_BACK;
        break;

    case UI_PAGE_EDIT_PRIMARY:
        sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_PRIMARY_COLOUR;
        sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_PRIMARY_R;
        sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_PRIMARY_G;
        sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_PRIMARY_B;
        sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_BACK;
        break;

    case UI_PAGE_EDIT_SECONDARY:
        sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_SECONDARY_COLOUR;
        sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_SECONDARY_R;
        sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_SECONDARY_G;
        sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_SECONDARY_B;
        sPlayerOutfitUIState->currentPageEntries[i++] = UI_ENTRY_BACK;
        break;
    }
}

static void RoguePlayerUI_OpenPage(u8 pageId)
{
    sPlayerOutfitUIState->stackCurrentPageIdx[sPlayerOutfitUIState->pageStackDepth] = sPlayerOutfitUIState->currentPageIdx;
    sPlayerOutfitUIState->stackCurrentOptionIdx[sPlayerOutfitUIState->pageStackDepth] = sPlayerOutfitUIState->currentOptionIdx;
    ++sPlayerOutfitUIState->pageStackDepth;

    sPlayerOutfitUIState->currentPageIdx = pageId;
    sPlayerOutfitUIState->currentOptionIdx = 0;
    RoguePlayerUI_RefreshPageEntries();
}

static bool8 RoguePlayerUI_ClosePage()
{
    if(sPlayerOutfitUIState->pageStackDepth > 1)
    {
        --sPlayerOutfitUIState->pageStackDepth;
        sPlayerOutfitUIState->currentPageIdx = sPlayerOutfitUIState->stackCurrentPageIdx[sPlayerOutfitUIState->pageStackDepth];
        sPlayerOutfitUIState->currentOptionIdx = sPlayerOutfitUIState->stackCurrentOptionIdx[sPlayerOutfitUIState->pageStackDepth];

        RoguePlayerUI_RefreshPageEntries();
        return TRUE;
    }

    return FALSE;
}

static bool8 RoguePlayerUI_HasSubpageOpen()
{
    return sPlayerOutfitUIState->pageStackDepth > 1;
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

static bool8 CanExitWithB()
{
    if(RoguePlayerUI_HasSubpageOpen())
        return TRUE;

    // In tutorial section, we cannot exit root using B button
    if(gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(ROGUE_INTRO) && gSaveBlock1Ptr->location.mapNum  == MAP_NUM(ROGUE_INTRO))
        return FALSE;

    return TRUE;
}

static void Task_RoguePlayerUIMain(u8 taskId)
{
    u8 startPageIdx = sPlayerOutfitUIState->currentPageIdx;
    u8 startOptionIdx = sPlayerOutfitUIState->currentOptionIdx;

    if ((CanExitWithB() && JOY_NEW(B_BUTTON)) || (JOY_NEW(A_BUTTON) && (sPlayerOutfitUIState->currentPageEntries[sPlayerOutfitUIState->currentOptionIdx] == UI_ENTRY_BACK || sPlayerOutfitUIState->currentPageEntries[sPlayerOutfitUIState->currentOptionIdx] == UI_ENTRY_EXIT)))
    {
        if(!RoguePlayerUI_ClosePage())
        {
            PlaySE(SE_PC_OFF);
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            gTasks[taskId].func = Task_RoguePlayerUIWaitFadeAndExitGracefully;
            return;
        }
        else
        {
            PlaySE(SE_SELECT);
            RoguePlayerUI_DrawTrainerSprites();
            RoguePlayerUI_PrintTitleText();
            RoguePlayerUI_PrintMenuText();
            return;
        }
    }

    else if (JOY_NEW(DPAD_UP))
    {
        if(sPlayerOutfitUIState->currentOptionIdx == 0)
            sPlayerOutfitUIState->currentOptionIdx = 0; // todo - loop
        else
            --sPlayerOutfitUIState->currentOptionIdx;
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        ++sPlayerOutfitUIState->currentOptionIdx;
    }

    else if (JOY_NEW(L_BUTTON))
    {
        sPlayerOutfitUIState->currentOptionIdx = 0;
    }
    else if (JOY_NEW(R_BUTTON))
    {
        sPlayerOutfitUIState->currentOptionIdx = TOTAL_UI_PAGE_ENTRIES;
    }

    else if (JOY_NEW(START_BUTTON))
    {
        // Jump to bottom option
        while(sPlayerOutfitUIState->currentPageEntries[sPlayerOutfitUIState->currentOptionIdx] != UI_ENTRY_COUNT)
        {
            ++sPlayerOutfitUIState->currentOptionIdx;
        }
        --sPlayerOutfitUIState->currentOptionIdx;
    }

    // Clamp
    if(sPlayerOutfitUIState->currentOptionIdx >= TOTAL_UI_PAGE_ENTRIES || sPlayerOutfitUIState->currentPageEntries[sPlayerOutfitUIState->currentOptionIdx] == UI_ENTRY_COUNT)
    {
        sPlayerOutfitUIState->currentOptionIdx = startOptionIdx;
    }

    if(startPageIdx != sPlayerOutfitUIState->currentPageIdx)
    {
        PlaySE(SE_SELECT);
        RoguePlayerUI_PrintTitleText();
        RoguePlayerUI_PrintMenuText();
    }
    else if(sPlayerOutfitUIState->currentOptionIdx != startOptionIdx)
    {
        PlaySE(SE_SELECT);
        RoguePlayerUI_PrintMenuText();
    }

    // Process callback
    {
        u8 currentEntryIdx = sPlayerOutfitUIState->currentPageEntries[sPlayerOutfitUIState->currentOptionIdx];

        if(sRoguePlayerUIEntries[currentEntryIdx].processInput != NULL && sRoguePlayerUIEntries[currentEntryIdx].processInput(currentEntryIdx, sPlayerOutfitUIState->currentOptionIdx))
        {
            // Special sound effect here to not break ears
            if(sRoguePlayerUIEntries[currentEntryIdx].processInput == RoguePlayerUI_EntryClothesStyleRGB_ProcessInput)
                PlaySE(SE_BALL);
            else
                PlaySE(SE_SELECT);

            RoguePlayerUI_DrawTrainerSprites();
            RoguePlayerUI_PrintTitleText();
            RoguePlayerUI_PrintMenuText();
        }
        else
        {
            // Draw UI hint only for colour editing pages :S
            switch (sPlayerOutfitUIState->currentPageIdx)
            {
                case UI_PAGE_EDIT_APPEARANCE:
                case UI_PAGE_EDIT_PRIMARY:
                case UI_PAGE_EDIT_SECONDARY:
                
                if(JOY_NEW(R_BUTTON))
                {
                    if(sPlayerOutfitUIState->colourEditMode == COLOUR_EDIT_FAST)
                        sPlayerOutfitUIState->colourEditMode = COLOUR_EDIT_ACCURATE;
                    else
                        sPlayerOutfitUIState->colourEditMode = COLOUR_EDIT_FAST;
                        
                    PlaySE(SE_SELECT);
                    RoguePlayerUI_DrawTrainerSprites();
                    RoguePlayerUI_PrintTitleText();
                    RoguePlayerUI_PrintMenuText();
                }
            }
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
    switch (sPlayerOutfitUIState->loadState)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, sRoguePlayerUITiles, 0, 0, 0);
        sPlayerOutfitUIState->loadState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            LZDecompressWram(sRoguePlayerUITilemap, sBg1TilemapBuffer);
            sPlayerOutfitUIState->loadState++;
        }
        LoadBgTiles(0, GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->tiles, 0x120, FREE_BLOCK_START);
        break;
    case 2:
        LoadPalette(GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->pal, BG_PLTT_ID(BG_PAL_ID_WINDOW_FRAMES), PLTT_SIZE_4BPP);

        LoadPalette(sRoguePlayerUIPalette, BG_PLTT_ID(BG_PAL_ID_BACKGROUND), PLTT_SIZE_4BPP);
        LoadPalette(gMessageBox_Pal, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        sPlayerOutfitUIState->loadState++;
    default:
        sPlayerOutfitUIState->loadState = 0;
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

static const u8 sText_RoguePlayerUITitle_Main[] = _("Player Outfit");
static const u8 sText_RoguePlayerUITitle_EditAppearance[] = _("Appearance");
static const u8 sText_RoguePlayerUITitle_EditPrimary[] = _("Primary Colour");
static const u8 sText_RoguePlayerUITitle_EditSecondary[] = _("Secondary Colour");

static void RoguePlayerUI_PrintTitleText()
{
    const u8* str = NULL;

    switch (sPlayerOutfitUIState->currentPageIdx)
    {
    case UI_PAGE_MAIN:
        str = sText_RoguePlayerUITitle_Main;
        break;

    case UI_PAGE_EDIT_APPEARANCE:
        str = sText_RoguePlayerUITitle_EditAppearance;
        break;
    case UI_PAGE_EDIT_PRIMARY:
        str = sText_RoguePlayerUITitle_EditPrimary;
        break;
    case UI_PAGE_EDIT_SECONDARY:
        str = sText_RoguePlayerUITitle_EditSecondary;
        break;
    }

    FillWindowPixelBuffer(WIN_TITLE, PIXEL_FILL(TEXT_COLOR_WHITE));
    AddTextPrinterParameterized4(WIN_TITLE, FONT_NORMAL, 0, 0, 0, 0, sRoguePlayerUIWindowFontColors[FONT_BLUE], TEXT_SKIP_DRAW, str);
    CopyWindowToVram(WIN_TITLE, COPYWIN_GFX);
}


static void AddMenuNameText(u8 menuOffset, const u8* text)
{
    u8 font = FONT_BLUE;

    if(menuOffset == sPlayerOutfitUIState->currentOptionIdx)
    {
        font = FONT_GREEN;
        AddTextPrinterParameterized4(WIN_INFO_PANEL, FONT_NARROW, 0, YPOS_SPACING * menuOffset, 0, 0, sRoguePlayerUIWindowFontColors[font], TEXT_SKIP_DRAW, gText_SelectorArrow);
    }

    AddTextPrinterParameterized4(WIN_INFO_PANEL, FONT_NARROW, 10, YPOS_SPACING * menuOffset, 0, 0, sRoguePlayerUIWindowFontColors[font], TEXT_SKIP_DRAW, text);
}

static void AddMenuValueText(u8 menuOffset, s8 offset, const u8* text)
{
    u8 font = FONT_BLACK;

    if(menuOffset == sPlayerOutfitUIState->currentOptionIdx)
    {
        font = FONT_BRIGHT_GREEN;
    }

    AddTextPrinterParameterized4(WIN_INFO_PANEL, 
        FONT_SHORT, 
        50 + GetStringCenterAlignXOffset(FONT_SHORT, text, 56) + offset, 
        YPOS_SPACING * menuOffset, 
        0, 0, 
        sRoguePlayerUIWindowFontColors[font], 
        TEXT_SKIP_DRAW, 
        text
    );
}

static const u8 sText_RoguePlayerUIEditFast[] = _("Fast");
static const u8 sText_RoguePlayerUIEditAccurate[] = _("Accurate");
static const u8 sText_RoguePlayerUIEditPrompt[] = _("{R_BUTTON} Mode: {STR_VAR_1}");
static const u8 sText_RoguePlayerMissingBacksprites[] = _("{COLOR LIGHT_BLUE}{SHADOW LIGHT_GRAY}(Missing back sprites)");

static void RoguePlayerUI_PrintMenuText()
{
    u8 i;

    FillWindowPixelBuffer(WIN_INFO_PANEL, PIXEL_FILL(TEXT_COLOR_WHITE));

    for(i = 0; i < TOTAL_UI_PAGE_ENTRIES; ++i)
    {
        u8 currentEntryIdx = sPlayerOutfitUIState->currentPageEntries[i];

        if(currentEntryIdx == UI_ENTRY_COUNT)
            break;

        AddMenuNameText(i, sRoguePlayerUIEntries[currentEntryIdx].text);
        if(sRoguePlayerUIEntries[currentEntryIdx].drawChoices != NULL)
            sRoguePlayerUIEntries[currentEntryIdx].drawChoices(currentEntryIdx, i);
    }

    // Draw UI hint
    switch (sPlayerOutfitUIState->currentPageIdx)
    {
    case UI_PAGE_EDIT_APPEARANCE:
    case UI_PAGE_EDIT_PRIMARY:
    case UI_PAGE_EDIT_SECONDARY:
        if(sPlayerOutfitUIState->colourEditMode == COLOUR_EDIT_FAST)
            StringCopy(gStringVar1, sText_RoguePlayerUIEditFast);
        else
            StringCopy(gStringVar1, sText_RoguePlayerUIEditAccurate);

        StringExpandPlaceholders(gStringVar2, sText_RoguePlayerUIEditPrompt);

        AddTextPrinterParameterized4(
            WIN_INFO_PANEL, 
            FONT_NARROW, 
            0, YPOS_SPACING * (TOTAL_UI_PAGE_ENTRIES - 1), 
            0, 0, 
            sRoguePlayerUIWindowFontColors[FONT_BLACK], 
            TEXT_SKIP_DRAW, 
            gStringVar2
        );
        break;

    case UI_PAGE_MAIN:
        if(RoguePlayer_GetTrainerBackPic() == TRAINER_BACK_PIC_NONE)
        {
            AddTextPrinterParameterized4(
                WIN_INFO_PANEL, 
                FONT_NARROW, 
                0, YPOS_SPACING * (TOTAL_UI_PAGE_ENTRIES - 1), 
                0, 0, 
                sRoguePlayerUIWindowFontColors[FONT_BLACK], 
                TEXT_SKIP_DRAW, 
                sText_RoguePlayerMissingBacksprites
            );
        }
        break;
    }

    CopyWindowToVram(WIN_INFO_PANEL, COPYWIN_GFX);
}

static void RoguePlayerUI_DrawTrainerSprites()
{
    const u8 xObjectSpacing = 32;

    // Free any sprites we previously had open
    RoguePlayerUI_FreeTrainerSprites();

    //sPlayerOutfitUIState->trainerFrontSpriteId = CreateTrainerPicSprite(
    //    TRAINER_PIC_MAY,
    //    TRUE,
    //    64, 92,
    //    0,
    //    TAG_NONE
    //);

    sPlayerOutfitUIState->trainerFrontSprite = CreateTrainerSprite(
        RoguePlayer_GetTrainerFrontPic(),
        64, 98,
        0, 
        gDecompressionBuffer
    );

    sPlayerOutfitUIState->trainerObjectEventGfx = OBJ_EVENT_GFX_PLAYER_NORMAL;// RoguePlayer_GetPlayerObjectGfx(PLAYER_AVATAR_STATE_NORMAL);

    if(sPlayerOutfitUIState->trainerObjectDownSpriteId == SPRITE_NONE)
    {
        sPlayerOutfitUIState->trainerObjectDownSpriteId = CreateObjectGraphicsSprite(sPlayerOutfitUIState->trainerObjectEventGfx, SpriteCallbackDummy, 64, 38, 0);

        StartSpriteAnim(&gSprites[sPlayerOutfitUIState->trainerObjectDownSpriteId], ANIM_STD_GO_SOUTH);
        SeekSpriteAnim(&gSprites[sPlayerOutfitUIState->trainerObjectDownSpriteId], sPlayerOutfitUIState->trainerObjectDownAnimIdx);
    }

    if(sPlayerOutfitUIState->trainerObjectUpSpriteId == SPRITE_NONE)
    {
        sPlayerOutfitUIState->trainerObjectUpSpriteId = CreateObjectGraphicsSprite(sPlayerOutfitUIState->trainerObjectEventGfx, SpriteCallbackDummy, 64 - xObjectSpacing, 38, 0);

        StartSpriteAnim(&gSprites[sPlayerOutfitUIState->trainerObjectUpSpriteId], ANIM_STD_GO_NORTH);
        SeekSpriteAnim(&gSprites[sPlayerOutfitUIState->trainerObjectUpSpriteId], sPlayerOutfitUIState->trainerObjectUpAnimIdx);
    }

    if(sPlayerOutfitUIState->trainerObjectSideSpriteId == SPRITE_NONE)
    {
        sPlayerOutfitUIState->trainerObjectSideSpriteId = CreateObjectGraphicsSprite(sPlayerOutfitUIState->trainerObjectEventGfx, SpriteCallbackDummy, 64 + xObjectSpacing, 38, 0);
    
        StartSpriteAnim(&gSprites[sPlayerOutfitUIState->trainerObjectSideSpriteId], ANIM_STD_GO_EAST);
        SeekSpriteAnim(&gSprites[sPlayerOutfitUIState->trainerObjectSideSpriteId], sPlayerOutfitUIState->trainerObjectSideAnimIdx);
    }
}

// As we load above directly into gDecompressionBuffer, we don't actually store the active tiles within the sprite
// so they won't get unloaded unless we manually do it here
#define SAFE_TRAINER_SPRITE_DELETE(trainerInfo) if(trainerInfo.spriteId != SPRITE_NONE) { FreeSpriteTilesByTag(trainerInfo.tileTag); FreeAndDestroyTrainerPicSprite(trainerInfo.spriteId); trainerInfo.spriteId = SPRITE_NONE; }
#define SAFE_OBJECT_SPRITE_DELETE(sprite, animIdx) if(sprite != SPRITE_NONE) { animIdx = gSprites[sprite].animCmdIndex; DestroySprite(&gSprites[sprite]); sprite = SPRITE_NONE; }

static void RoguePlayerUI_FreeTrainerSprites()
{
    SAFE_TRAINER_SPRITE_DELETE(sPlayerOutfitUIState->trainerFrontSprite);

    // Free the object palette here ourself
    if(sPlayerOutfitUIState->trainerObjectDownSpriteId != SPRITE_NONE)
    {
        const struct ObjectEventGraphicsInfo *graphicsInfo = GetObjectEventGraphicsInfo(sPlayerOutfitUIState->trainerObjectEventGfx);
        if(graphicsInfo->paletteTag != TAG_NONE)
        {
            FreeSpritePaletteByTag(graphicsInfo->paletteTag);
        }
    }

    SAFE_OBJECT_SPRITE_DELETE(sPlayerOutfitUIState->trainerObjectDownSpriteId, sPlayerOutfitUIState->trainerObjectDownAnimIdx);
    SAFE_OBJECT_SPRITE_DELETE(sPlayerOutfitUIState->trainerObjectUpSpriteId, sPlayerOutfitUIState->trainerObjectUpAnimIdx);
    SAFE_OBJECT_SPRITE_DELETE(sPlayerOutfitUIState->trainerObjectSideSpriteId, sPlayerOutfitUIState->trainerObjectSideAnimIdx);
}

#undef SAFE_TRAINER_SPRITE_DELETE
#undef SAFE_OBJECT_SPRITE_DELETE

static void RoguePlayerUI_FreeResources(void)
{
    if (sPlayerOutfitUIState != NULL)
    {
        Free(sPlayerOutfitUIState);
    }
    if (sBg1TilemapBuffer != NULL)
    {
        Free(sBg1TilemapBuffer);
    }
    
    RogueToD_SetTempDisableTimeVisuals(FALSE);
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

static bool8 RoguePlayerUI_EntryOpenPage_ProcessInput(u8 entryIdx, u8 menuOffset)
{
    if(JOY_NEW(A_BUTTON))
    {
        RoguePlayerUI_OpenPage(sRoguePlayerUIEntries[entryIdx].userData.val8[0]);
        return TRUE;
    }

    return FALSE;
}

static bool8 RoguePlayerUI_EntryOutfit_ProcessInput(u8 entryIdx, u8 menuOffset)
{
    const u16 outfitCount = RoguePlayer_GetOutfitCount();
    u16 outfitId = RoguePlayer_GetOutfitId();

    if(JOY_REPEAT(DPAD_LEFT))
    {
        do
        {
            if(outfitId == 0)
                outfitId = outfitCount - 1;
            else
                --outfitId;
        }
        while(!RoguePlayer_HasUnlockedOutfitId(outfitId));

        RoguePlayer_SetOutfitId(outfitId);
        RoguePlayerUI_RefreshPageEntries();
        return TRUE;
    }
    else if(JOY_REPEAT(DPAD_RIGHT) || JOY_NEW(A_BUTTON))
    {
        do
        {
            outfitId = (outfitId + 1) % outfitCount;
        }
        while(!RoguePlayer_HasUnlockedOutfitId(outfitId));

        RoguePlayer_SetOutfitId(outfitId);
        RoguePlayerUI_RefreshPageEntries();
        return TRUE;
    }

    return FALSE;
}

static void RoguePlayerUI_EntryOutfit_DrawChoices(u8 entryIdx, u8 menuOffset)
{
    const u8* name = RoguePlayer_GetOutfitName();
    AddMenuValueText(menuOffset, 0, name);
}

static bool8 RoguePlayerUI_EntryRandomise_ProcessInput(u8 entryIdx, u8 menuOffset)
{
    if(JOY_NEW(A_BUTTON))
    {
        bool8 randomiseOutfitId = sRoguePlayerUIEntries[entryIdx].userData.val8[0];
        RoguePlayer_RandomiseOutfit(randomiseOutfitId != 0);
        RoguePlayerUI_RefreshPageEntries();
        return TRUE;
    }

    return FALSE;
}

static u8 GetCurrentOutfitStyleRGBChannel(u8 entryIdx)
{
    u8 outfitStyle = sRoguePlayerUIEntries[entryIdx].userData.val8[0];
    u8 colourChannel = sRoguePlayerUIEntries[entryIdx].userData.val8[1];

    switch (colourChannel)
    {
    case COLOUR_CHANNEL_R:
        return sPlayerOutfitUIState->outfitStyleR[outfitStyle];

    case COLOUR_CHANNEL_G:
        return sPlayerOutfitUIState->outfitStyleG[outfitStyle];

    case COLOUR_CHANNEL_B:
        return sPlayerOutfitUIState->outfitStyleB[outfitStyle];
    }

    return 0;
}

static void SetCurrentOutfitStyleRGBChannel(u8 entryIdx, u8 value)
{
    u16 colour;
    u8 outfitStyle = sRoguePlayerUIEntries[entryIdx].userData.val8[0];
    u8 colourChannel = sRoguePlayerUIEntries[entryIdx].userData.val8[1];
    
    switch (colourChannel)
    {
    case COLOUR_CHANNEL_R:
        sPlayerOutfitUIState->outfitStyleR[outfitStyle] = value;
        break;

    case COLOUR_CHANNEL_G:
        sPlayerOutfitUIState->outfitStyleG[outfitStyle] = value;
        break;

    case COLOUR_CHANNEL_B:
        sPlayerOutfitUIState->outfitStyleB[outfitStyle] = value;
        break;
    }

    colour = RGB(
        RGB_CONVERT_FROM_UI_RANGE(sPlayerOutfitUIState->outfitStyleR[outfitStyle]),
        RGB_CONVERT_FROM_UI_RANGE(sPlayerOutfitUIState->outfitStyleG[outfitStyle]),
        RGB_CONVERT_FROM_UI_RANGE(sPlayerOutfitUIState->outfitStyleB[outfitStyle])
    );
    RoguePlayer_SetOutfitStyle(outfitStyle, colour);
}


static bool8 RoguePlayerUI_EntryClothesStyleRGB_ProcessInput(u8 entryIdx, u8 menuOffset)
{
    u8 currValue, prevValue;
    currValue = prevValue = GetCurrentOutfitStyleRGBChannel(entryIdx);

    if(sPlayerOutfitUIState->colourEditMode == COLOUR_EDIT_ACCURATE)
    {
        // Accurate mode: Only increment on press
        if(JOY_NEW(DPAD_LEFT))
        {
            if(currValue != 0)
                --currValue;
        }
        else if(JOY_NEW(DPAD_RIGHT))
        {
            if(currValue < RGB_MAX_UI_VALUE)
                currValue = (currValue + 1);
        }
    }
    else
    {
        // Fast mode: increment on hold
        if(JOY_HELD(DPAD_LEFT))
        {
            if(currValue != 0)
                --currValue;
        }
        else if(JOY_HELD(DPAD_RIGHT))
        {
            if(currValue < RGB_MAX_UI_VALUE)
                currValue = (currValue + 1);
        }
    }

    if(prevValue != currValue)
    {
        SetCurrentOutfitStyleRGBChannel(entryIdx, currValue);
        return TRUE;
    }

    return FALSE;
}

//static const u8 sText_RoguePlayerZero[] = _(" 0%");
//static const u8 sText_RoguePlayerPlusValue[] = _("+{STR_VAR_1}%");
//static const u8 sText_RoguePlayerMinusValue[] = _("-{STR_VAR_1}%");

static const u8 sText_RoguePlayerValuePercentage[] = _("{STR_VAR_1}%");

static const u8 sText_RoguePlayerRed[] = _("{COLOR RED}{SHADOW LIGHT_RED}");
static const u8 sText_RoguePlayerGreen[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}");
static const u8 sText_RoguePlayerBlue[] = _("{COLOR BLUE}{SHADOW LIGHT_BLUE}");

static const u8 sText_RoguePlayerEmptyRed[] = _("{COLOR LIGHT_RED}{SHADOW LIGHT_GRAY}");
static const u8 sText_RoguePlayerEmptyGreen[] = _("{COLOR LIGHT_GREEN}{SHADOW LIGHT_GRAY}");
static const u8 sText_RoguePlayerEmptyBlue[] = _("{COLOR LIGHT_BLUE}{SHADOW LIGHT_GRAY}");

static const u8 sText_RoguePlayerFullCell[] = _("{EMOJI_SQUARE}");
static const u8 sText_RoguePlayerEmptyCell[] = _("{EMOJI_MINUS}");

static void RoguePlayerUI_EntryClothesStyleRGB_DrawChoices(u8 entryIdx, u8 menuOffset)
{
    u16 value = GetCurrentOutfitStyleRGBChannel(entryIdx);

    if(RGB_MAX_UI_VALUE <= 10)
    {
        u8 i, colourChannel;
        u8* str = &gStringVar1[0];

        // Place circles
        *str = 0xFF;
        
        colourChannel = sRoguePlayerUIEntries[entryIdx].userData.val8[1];

        // Full cell colour
        switch (colourChannel)
        {
        case COLOUR_CHANNEL_R:
            str = StringAppend(str, sText_RoguePlayerRed);
            break;

        case COLOUR_CHANNEL_G:
            str = StringAppend(str, sText_RoguePlayerGreen);
            break;

        case COLOUR_CHANNEL_B:
            str = StringAppend(str, sText_RoguePlayerBlue);
            break;
        }

        for(i = 0; i <= RGB_MAX_UI_VALUE; ++i)
        {
            if(value > i)
                str = StringAppend(str, sText_RoguePlayerFullCell);
            else
            {
                if(value == i)
                {
                    // Empty cell colour
                    switch (colourChannel)
                    {
                    case COLOUR_CHANNEL_R:
                        str = StringAppend(str, sText_RoguePlayerEmptyRed);
                        break;

                    case COLOUR_CHANNEL_G:
                        str = StringAppend(str, sText_RoguePlayerEmptyGreen);
                        break;

                    case COLOUR_CHANNEL_B:
                        str = StringAppend(str, sText_RoguePlayerEmptyBlue);
                        break;
                    }
                }
                str = StringAppend(str, sText_RoguePlayerEmptyCell);
            }
        }
        AddMenuValueText(menuOffset, -25, gStringVar1);
    }
    else
    {
        //value = (value * 100) / 31;
        ConvertIntToDecimalStringN(gStringVar1, value, STR_CONV_MODE_LEFT_ALIGN, 3);
        //StringExpandPlaceholders(gStringVar2, sText_RoguePlayerValuePercentage);
        AddMenuValueText(menuOffset, 0, gStringVar1);
    }
}

static bool8 RoguePlayerUI_EntryClothesStylePreset_ProcessInput(u8 entryIdx, u8 menuOffset)
{
    u8 outfitStyle = sRoguePlayerUIEntries[entryIdx].userData.val8[0];

    // Accurate mode: Only increment on press
    if(JOY_NEW(DPAD_LEFT))
    {
        RoguePlayer_IncrementOutfitStyleByName(outfitStyle, -1);
        RefreshUIOutfitStylesFromSource();
        return TRUE;
    }
    else if(JOY_NEW(DPAD_RIGHT) || JOY_NEW(A_BUTTON))
    {
        RoguePlayer_IncrementOutfitStyleByName(outfitStyle, 1);
        RefreshUIOutfitStylesFromSource();
        return TRUE;
    }

    return FALSE;
}

static void RoguePlayerUI_EntryClothesStylePreset_DrawChoices(u8 entryIdx, u8 menuOffset)
{
    u8 outfitStyle = sRoguePlayerUIEntries[entryIdx].userData.val8[0];
    const u8* name = RoguePlayer_GetOutfitStyleName(outfitStyle);

    AddMenuValueText(menuOffset, 0, name);
}
