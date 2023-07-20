#include "global.h"
#include "option_menu.h"
#include "main.h"
#include "menu.h"
#include "scanline_effect.h"
#include "palette.h"
#include "sprite.h"
#include "task.h"
#include "bg.h"
#include "gpu_regs.h"
#include "window.h"
#include "text.h"
#include "text_window.h"
#include "international_string_util.h"
#include "script.h"
#include "strings.h"
#include "string_util.h"
#include "gba/m4a_internal.h"
#include "constants/rgb.h"

#include "rogue_settings.h"

extern const u8 gText_16Spaces[];
extern const u8 gText_DifficultySettings[];
extern const u8 gText_DifficultyArrowLeft[];
extern const u8 gText_DifficultyArrowRight[];

extern const u8 gText_DifficultyDoesntAffectReward[];
extern const u8 gText_DifficultyRewardLevel[];

extern const u8 gText_DifficultyPreset[];
extern const u8 gText_DifficultyPresetEasy[];
extern const u8 gText_DifficultyPresetMedium[];
extern const u8 gText_DifficultyPresetHard[];
extern const u8 gText_DifficultyPresetBrutal[];
extern const u8 gText_DifficultyPresetCustom[];

extern const u8 gText_DifficultyEnabled[];
extern const u8 gText_DifficultyDisabled[];

extern const u8 gText_DifficultyToggles[];
extern const u8 gText_DifficultySliders[];

extern const u8 gText_DifficultyExpAll[];
extern const u8 gText_DifficultyOverLvl[];
extern const u8 gText_DifficultyEVGain[];
extern const u8 gText_DifficultyOverworldMons[];
extern const u8 gText_DifficultyBagWipe[];
extern const u8 gText_DifficultySwitchMode[];

extern const u8 gText_DifficultyTrainers[];
extern const u8 gText_DifficultyItems[];
extern const u8 gText_DifficultyLegendaries[];

extern const u8 gText_DifficultyPresetDesc[];
extern const u8 gText_DifficultyCustomDesc[];
extern const u8 gText_DifficultyExpAllDesc[];
extern const u8 gText_DifficultyOverLvlDesc[];
extern const u8 gText_DifficultyEVGainDesc[];
extern const u8 gText_DifficultyOverworldMonsDesc[];
extern const u8 gText_DifficultyBagWipeDesc[];
extern const u8 gText_DifficultySwitchModeDesc[];

extern const u8 gText_DifficultyTrainersDesc[];
extern const u8 gText_DifficultyItemsDesc[];
extern const u8 gText_DifficultyLegendariesDesc[];

// Task data
enum
{
    TD_MENUSELECTION,
    TD_MENUSELECTION_TOP,
    TD_SUBMENU,
    TD_PREVIOUS_MENUSELECTION,
    TD_PREVIOUS_MENUSELECTION_TOP,
};

// Menu items
enum
{
    MENUITEM_DIFFICULTY_PRESET,

    MENUITEM_MENU_TOGGLES,
    MENUITEM_MENU_SLIDERS,

    MENUITEM_MENU_TOGGLE_EXP_ALL,
    MENUITEM_MENU_TOGGLE_OVER_LVL,
    MENUITEM_MENU_TOGGLE_EV_GAIN,
    MENUITEM_MENU_TOGGLE_OVERWORLD_MONS,
    MENUITEM_MENU_TOGGLE_BAG_WIPE,
    MENUITEM_MENU_TOGGLE_SWITCH_MODE,

    MENUITEM_MENU_SLIDER_TRAINER,
    MENUITEM_MENU_SLIDER_ITEM,
    MENUITEM_MENU_SLIDER_LEGENDARY,

    MENUITEM_CANCEL,
};

enum
{
    SUBMENUITEM_NONE,
    SUBMENUITEM_TOGGLES,
    SUBMENUITEM_SLIDERS,
    SUBMENUITEM_COUNT,
};

// Window Ids
enum
{
    WIN_TEXT_OPTION,
    WIN_OPTIONS
};

#define MAX_MENUITEM_COUNT 16
#define MAX_MENUITEM_TO_DISPLAY 5
#define YPOS_SPACING      16

// this file's functions
static void Task_OptionMenuFadeIn(u8 taskId);
static void Task_OptionMenuProcessInput(u8 taskId);
static void Task_OptionMenuSave(u8 taskId);
static void Task_OptionMenuFadeOut(u8 taskId);
static void HighlightOptionMenuItem(u8 selection, u8 topIndex);
static void DrawDescriptionOptionMenuText(u8 submenu, u8 selection);
static void DrawOptionMenuTexts(u8 submenu, u8 topIndex);
static void DrawBgWindowFrames(void);
static u8 GetMenuItemValue(u8 menuItem);
static void SetMenuItemValue(u8 menuItem, u8 value);

static void ArrowRight_DrawChoices(u8 menuOffset, u8 selection);
static void ArrowLeft_DrawChoices(u8 menuOffset, u8 selection);
static u8 Slider_ProcessInput(u8 menuOffset, u8 selection);
static void Slider_DrawChoices(u8 menuOffset, u8 selection);
static u8 Toggle_ProcessInput(u8 menuOffset, u8 selection);
static void Toggle_DrawChoices(u8 menuOffset, u8 selection);
static u8 Empty_ProcessInput(u8 menuOffset, u8 selection);
static void Empty_DrawChoices(u8 menuOffset, u8 selection);

EWRAM_DATA static bool8 sArrowPressed = FALSE;

static const u16 sOptionMenuText_Pal[] = INCBIN_U16("graphics/interface/option_menu_text2.gbapal"); // <- inserts extra greens
// note: this is only used in the Japanese release
static const u8 sEqualSignGfx[] = INCBIN_U8("graphics/interface/option_menu_equals_sign.4bpp");

typedef u8 (*MenuItemInputCallback)(u8, u8);
typedef void (*MenuItemDrawCallback)(u8, u8);

struct MenuEntry
{
    const u8 * itemName;
    const u8 * itemDesc;
    MenuItemInputCallback processInput;
    MenuItemDrawCallback drawChoices;
};

struct MenuEntries
{
    const u8 menuOptions[MAX_MENUITEM_COUNT];
};

static const struct MenuEntry sOptionMenuItems[] =
{
    [MENUITEM_DIFFICULTY_PRESET] = 
    {
        .itemName = gText_DifficultyPreset,
        .itemDesc = gText_DifficultyPresetDesc,
        .processInput = Slider_ProcessInput,
        .drawChoices = Slider_DrawChoices
    },

    [MENUITEM_MENU_TOGGLES] = 
    {
        .itemName = gText_DifficultyToggles,
        .itemDesc = gText_DifficultyCustomDesc,
        .processInput = Empty_ProcessInput,
        .drawChoices = ArrowRight_DrawChoices
    },
    [MENUITEM_MENU_SLIDERS] = 
    {
        .itemName = gText_DifficultySliders,
        .itemDesc = gText_DifficultyCustomDesc,
        .processInput = Empty_ProcessInput,
        .drawChoices = ArrowRight_DrawChoices
    },


    [MENUITEM_MENU_TOGGLE_EXP_ALL] = 
    {
        .itemName = gText_DifficultyExpAll,
        .itemDesc = gText_DifficultyExpAllDesc,
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
    [MENUITEM_MENU_TOGGLE_OVER_LVL] = 
    {
        .itemName = gText_DifficultyOverLvl,
        .itemDesc = gText_DifficultyOverLvlDesc,
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
    [MENUITEM_MENU_TOGGLE_EV_GAIN] = 
    {
        .itemName = gText_DifficultyEVGain,
        .itemDesc = gText_DifficultyEVGainDesc,
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
    [MENUITEM_MENU_TOGGLE_OVERWORLD_MONS] = 
    {
        .itemName = gText_DifficultyOverworldMons,
        .itemDesc = gText_DifficultyOverworldMonsDesc,
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
    [MENUITEM_MENU_TOGGLE_BAG_WIPE] = 
    {
        .itemName = gText_DifficultyBagWipe,
        .itemDesc = gText_DifficultyBagWipeDesc,
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
    [MENUITEM_MENU_TOGGLE_SWITCH_MODE] = 
    {
        .itemName = gText_DifficultySwitchMode,
        .itemDesc = gText_DifficultySwitchModeDesc,
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },

    [MENUITEM_MENU_SLIDER_TRAINER] = 
    {
        .itemName = gText_DifficultyTrainers,
        .itemDesc = gText_DifficultyTrainersDesc,
        .processInput = Slider_ProcessInput,
        .drawChoices = Slider_DrawChoices
    },
    [MENUITEM_MENU_SLIDER_ITEM] = 
    {
        .itemName = gText_DifficultyItems,
        .itemDesc = gText_DifficultyItemsDesc,
        .processInput = Slider_ProcessInput,
        .drawChoices = Slider_DrawChoices
    },
    [MENUITEM_MENU_SLIDER_LEGENDARY] = 
    {
        .itemName = gText_DifficultyLegendaries,
        .itemDesc = gText_DifficultyLegendariesDesc,
        .processInput = Slider_ProcessInput,
        .drawChoices = Slider_DrawChoices
    },

    [MENUITEM_CANCEL] = 
    {
        .itemName = gText_OptionMenuCancel,
        .processInput = Empty_ProcessInput,
        .drawChoices = Empty_DrawChoices
    },
};

static const struct MenuEntries sOptionMenuEntries[SUBMENUITEM_COUNT] =
{
    [SUBMENUITEM_NONE] = 
    {
        .menuOptions = 
        {
            MENUITEM_DIFFICULTY_PRESET,
            MENUITEM_MENU_TOGGLES,
            MENUITEM_MENU_SLIDERS,
            MENUITEM_CANCEL
        }
    },
    [SUBMENUITEM_TOGGLES] = 
    {
        .menuOptions = 
        {
            MENUITEM_MENU_TOGGLE_EXP_ALL,
            MENUITEM_MENU_TOGGLE_OVER_LVL,
            MENUITEM_MENU_TOGGLE_EV_GAIN,
            MENUITEM_MENU_TOGGLE_OVERWORLD_MONS,
            MENUITEM_MENU_TOGGLE_BAG_WIPE,
            MENUITEM_MENU_TOGGLE_SWITCH_MODE,
            MENUITEM_CANCEL
        }
    },
    [SUBMENUITEM_SLIDERS] = 
    {
        .menuOptions = 
        {
            MENUITEM_MENU_SLIDER_TRAINER,
            MENUITEM_MENU_SLIDER_ITEM,
            MENUITEM_MENU_SLIDER_LEGENDARY,
            MENUITEM_CANCEL
        }
    },
};

static const struct WindowTemplate sOptionMenuWinTemplates[] =
{
    {
        .bg = 1,
        .tilemapLeft = 1,
        .tilemapTop = 13,
        .width = 28,
        .height = 6,
        .paletteNum = 1,
        .baseBlock = 2
    },
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 28,
        .height = 10,
        .paletteNum = 1,
        .baseBlock = 170
    },
    //{
    //    .bg = 0,
    //    .tilemapLeft = 2,
    //    .tilemapTop = 5,
    //    .width = 26,
    //    .height = 14,
    //    .paletteNum = 1,
    //    .baseBlock = 0x36
    //},
    DUMMY_WIN_TEMPLATE
};

#define FREE_BLOCK_START 450

static const struct BgTemplate sOptionMenuBgTemplates[] =
{
   {
       .bg = 1,
       .charBaseIndex = 1,
       .mapBaseIndex = 30,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 0,
       .baseTile = 0
   },
   {
       .bg = 0,
       .charBaseIndex = 1,
       .mapBaseIndex = 31,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 1,
       .baseTile = 0
   }
};

static const u16 sOptionMenuBg_Pal[] = {RGB(17, 18, 31)};

// code
static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void CB2_InitDifficultyConfigMenu(void)
{
    switch (gMain.state)
    {
    default:
    case 0:
        SetVBlankCallback(NULL);
        gMain.state++;
        break;
    case 1:
        DmaClearLarge16(3, (void*)(VRAM), VRAM_SIZE, 0x1000);
        DmaClear32(3, OAM, OAM_SIZE);
        DmaClear16(3, PLTT, PLTT_SIZE);
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sOptionMenuBgTemplates, ARRAY_COUNT(sOptionMenuBgTemplates));
        ChangeBgX(0, 0, BG_COORD_SET);
        ChangeBgY(0, 0, BG_COORD_SET);
        ChangeBgX(1, 0, BG_COORD_SET);
        ChangeBgY(1, 0, BG_COORD_SET);
        ChangeBgX(2, 0, BG_COORD_SET);
        ChangeBgY(2, 0, BG_COORD_SET);
        ChangeBgX(3, 0, BG_COORD_SET);
        ChangeBgY(3, 0, BG_COORD_SET);
        InitWindows(sOptionMenuWinTemplates);
        DeactivateAllTextPrinters();
        SetGpuReg(REG_OFFSET_WIN0H, 0);
        SetGpuReg(REG_OFFSET_WIN0V, 0);
        SetGpuReg(REG_OFFSET_WININ, WININ_WIN0_BG0);
        SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG0 | WINOUT_WIN01_BG1 | WINOUT_WIN01_CLR);
        SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG0 | BLDCNT_EFFECT_DARKEN);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 4);
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON | DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
        ShowBg(0);
        ShowBg(1);
        gMain.state++;
        break;
    case 2:
        ResetPaletteFade();
        ScanlineEffect_Stop();
        ResetTasks();
        ResetSpriteData();
        gMain.state++;
        break;
    case 3:
        LoadBgTiles(1, GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->tiles, 0x120, FREE_BLOCK_START);
        gMain.state++;
        break;
    case 4:
        LoadPalette(sOptionMenuBg_Pal, 0, sizeof(sOptionMenuBg_Pal));
        LoadPalette(GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->pal, 0x70, 0x20);
        gMain.state++;
        break;
    case 5:
        LoadPalette(sOptionMenuText_Pal, 16, sizeof(sOptionMenuText_Pal));
        gMain.state++;
        break;
    case 6:
        PutWindowTilemap(0);
        gMain.state++;
        break;
    case 7:
        gMain.state++;
        break;
    case 8:
        PutWindowTilemap(1);
        gMain.state++;
    case 9:
        DrawBgWindowFrames();
        gMain.state++;
        break;
    case 10:
    {
        u8 i;
        u8 taskId = CreateTask(Task_OptionMenuFadeIn, 0);

        gTasks[taskId].data[TD_MENUSELECTION] = 0;
        gTasks[taskId].data[TD_MENUSELECTION_TOP] = 0;
        gTasks[taskId].data[TD_SUBMENU] = 0;

        DrawOptionMenuTexts(gTasks[taskId].data[TD_SUBMENU], 0);
        DrawDescriptionOptionMenuText(gTasks[taskId].data[TD_SUBMENU], gTasks[taskId].data[TD_MENUSELECTION]);
        HighlightOptionMenuItem(gTasks[taskId].data[TD_MENUSELECTION], gTasks[taskId].data[TD_MENUSELECTION_TOP]);

        CopyWindowToVram(WIN_OPTIONS, COPYWIN_FULL);
        gMain.state++;
        break;
    }
    case 11:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0x10, 0, RGB_BLACK);
        SetVBlankCallback(VBlankCB);
        SetMainCallback2(MainCB2);
        return;
    }
}

static void Task_OptionMenuFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_OptionMenuProcessInput;
}

static void Task_OptionMenuProcessInput(u8 taskId)
{
    bool8 submenuChanged = FALSE;
    u8 menuSelection = gTasks[taskId].data[TD_MENUSELECTION];
    u8 menuSelectionTop = gTasks[taskId].data[TD_MENUSELECTION_TOP];
    u8 submenuSelection = gTasks[taskId].data[TD_SUBMENU];
    u8 menuItem = sOptionMenuEntries[submenuSelection].menuOptions[menuSelection];

    if (JOY_NEW(B_BUTTON) || (JOY_NEW(A_BUTTON) && menuItem == MENUITEM_CANCEL))
    {
        if(submenuSelection != SUBMENUITEM_NONE)
        {
            submenuSelection = SUBMENUITEM_NONE;
            submenuChanged = TRUE;
        }
        else
            gTasks[taskId].func = Task_OptionMenuSave;
    }
    else if(JOY_NEW(A_BUTTON) && submenuSelection == SUBMENUITEM_NONE)
    {
        switch (menuItem)
        {
        case MENUITEM_MENU_TOGGLES:
            submenuSelection = SUBMENUITEM_TOGGLES;
            submenuChanged = TRUE;
            break;

        case MENUITEM_MENU_SLIDERS:
            submenuSelection = SUBMENUITEM_SLIDERS;
            submenuChanged = TRUE;
            break;
        }
    }
    else if (JOY_NEW(DPAD_UP))
    {
        if(menuSelection != 0)
        {
            menuSelection--;
            DrawDescriptionOptionMenuText(submenuSelection, menuSelection);

            if(menuSelection < menuSelectionTop)
            {
                menuSelectionTop = menuSelection;
                DrawOptionMenuTexts(submenuSelection, menuSelectionTop);
            }
        }

        HighlightOptionMenuItem(menuSelection, menuSelectionTop);
        gTasks[taskId].data[TD_MENUSELECTION] = menuSelection;
        gTasks[taskId].data[TD_MENUSELECTION_TOP] = menuSelectionTop;
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        if(menuItem != MENUITEM_CANCEL)
        {
            menuSelection++;
            DrawDescriptionOptionMenuText(submenuSelection, menuSelection);

            if(menuSelection >= menuSelectionTop + MAX_MENUITEM_TO_DISPLAY)
            {
                menuSelectionTop = menuSelection - MAX_MENUITEM_TO_DISPLAY + 1;
                DrawOptionMenuTexts(submenuSelection, menuSelectionTop);
            }
        }

        HighlightOptionMenuItem(menuSelection, menuSelectionTop);
        gTasks[taskId].data[TD_MENUSELECTION] = menuSelection;
        gTasks[taskId].data[TD_MENUSELECTION_TOP] = menuSelectionTop;
    }
    else if(menuItem != MENUITEM_CANCEL)
    {
        u8 currOption;
        u8 prevOption;

        prevOption = GetMenuItemValue(menuItem);

        currOption = sOptionMenuItems[menuItem].processInput(menuSelection, prevOption);

        if(prevOption != currOption)
        {
            sOptionMenuItems[menuItem].drawChoices(menuSelection - menuSelectionTop, currOption);

            SetMenuItemValue(menuItem, currOption);

            // Update the description
            DrawDescriptionOptionMenuText(submenuSelection, menuSelection);
        }

        if (sArrowPressed)
        {
            sArrowPressed = FALSE;
            CopyWindowToVram(WIN_OPTIONS, COPYWIN_GFX);
            //CopyWindowToVram(WIN_OPTIONS, COPYWIN_FULL);
        }
    }

    if(submenuChanged)
    {
        if(submenuSelection == SUBMENUITEM_NONE)
        {
            menuSelection = gTasks[taskId].data[TD_PREVIOUS_MENUSELECTION];
            menuSelectionTop = gTasks[taskId].data[TD_PREVIOUS_MENUSELECTION_TOP];
        }
        else
        {
            gTasks[taskId].data[TD_PREVIOUS_MENUSELECTION] = menuSelection;
            gTasks[taskId].data[TD_PREVIOUS_MENUSELECTION_TOP] = menuSelectionTop;
            menuSelection = 0;
        }
        
        gTasks[taskId].data[TD_MENUSELECTION] = menuSelection;
        gTasks[taskId].data[TD_MENUSELECTION_TOP] = menuSelectionTop;
        gTasks[taskId].data[TD_SUBMENU] = submenuSelection;

        DrawOptionMenuTexts(submenuSelection, menuSelectionTop);
        DrawDescriptionOptionMenuText(submenuSelection, menuSelection);
        HighlightOptionMenuItem(menuSelection, menuSelectionTop);
    }
}

static void Task_OptionMenuSave(u8 taskId)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
    gTasks[taskId].func = Task_OptionMenuFadeOut;
}

static void Task_OptionMenuFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DestroyTask(taskId);
        FreeAllWindowBuffers();
        SetMainCallback2(gMain.savedCallback);
        // EnableBothScriptContexts(); <- handled in savedCallback
    }
}

static void HighlightOptionMenuItem(u8 index, u8 topIndex)
{
    u16 left = max(1, sOptionMenuWinTemplates[WIN_OPTIONS].tilemapLeft) - 1;
    u16 top = max(1, sOptionMenuWinTemplates[WIN_OPTIONS].tilemapTop) - 1;

    index -= topIndex;

    SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(16 * left, DISPLAY_WIDTH - 16 * left));
    SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE((index + top) * 16 + 8, (index + top + 1) * 16 + 8));
}

static void DrawOptionMenuChoice(const u8 *text, u8 x, u8 y, u8 style)
{
    u8 dst[16];
    u16 i;

    for (i = 0; *text != EOS && i <= 14; i++)
        dst[i] = *(text++);

    if (style != 0)
    {
        dst[2] = 4;
        dst[5] = 5;
    }

    dst[i] = EOS;
    AddTextPrinterParameterized(WIN_OPTIONS, FONT_NORMAL, dst, x, y + 1, TEXT_SKIP_DRAW, NULL);
}

static void ArrowRight_DrawChoices(u8 menuOffset, u8 selection)
{
    DrawOptionMenuChoice(gText_DifficultyArrowRight, 104, menuOffset * YPOS_SPACING, 0);
}

static void ArrowLeft_DrawChoices(u8 menuOffset, u8 selection)
{
    DrawOptionMenuChoice(gText_DifficultyArrowLeft, 104, menuOffset * YPOS_SPACING, 0);
}

static u8 Slider_ProcessInput(u8 menuOffset, u8 selection)
{
    if (JOY_NEW(DPAD_RIGHT))
    {
        if (selection < DIFFICULTY_LEVEL_BRUTAL)
            selection++;
        //else
        //    selection = DIFFICULTY_LEVEL_EASY;

        sArrowPressed = TRUE;
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        if (selection != DIFFICULTY_LEVEL_EASY)
            selection--;
        //else
        //    selection = DIFFICULTY_LEVEL_BRUTAL;

        sArrowPressed = TRUE;
    }
    return selection;
}

static void Slider_DrawChoices(u8 menuOffset, u8 selection)
{
    const u8* text;
    u8 style = 0;

    // Hack to wipe tiles????
    DrawOptionMenuChoice(gText_16Spaces, 104, menuOffset * YPOS_SPACING, 0);

    switch (selection)
    {
    case DIFFICULTY_LEVEL_EASY:
        text = gText_DifficultyPresetEasy;
        break;

    case DIFFICULTY_LEVEL_MEDIUM:
        text = gText_DifficultyPresetMedium;
        break;

    case DIFFICULTY_LEVEL_HARD:
        text = gText_DifficultyPresetHard;
        break;

    case DIFFICULTY_LEVEL_BRUTAL:
        text = gText_DifficultyPresetBrutal;
        break;
    
    default:
        text = gText_DifficultyPresetCustom;
        break;
    }

    DrawOptionMenuChoice(text, 104, menuOffset * YPOS_SPACING, style);
}

static u8 Toggle_ProcessInput(u8 menuOffset, u8 selection)
{
    if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT))
    {
        selection ^= 1;
        sArrowPressed = TRUE;
    }

    return selection;
}

static void Toggle_DrawChoices(u8 menuOffset, u8 selection)
{
    // Hack to wipe tiles????
    DrawOptionMenuChoice(gText_16Spaces, 104, menuOffset * YPOS_SPACING, 0);

    if(selection == 0)
        DrawOptionMenuChoice(gText_DifficultyDisabled, 104, menuOffset * YPOS_SPACING, 0);
    else
        DrawOptionMenuChoice(gText_DifficultyEnabled, 104, menuOffset * YPOS_SPACING, 0);
}

static u8 Empty_ProcessInput(u8 menuOffset, u8 selection)
{

}

static void Empty_DrawChoices(u8 menuOffset, u8 selection)
{

}

static void DrawDescriptionOptionMenuText(u8 submenu, u8 selection)
{
    u8 text[64];
    u8* str;

    u8 menuItem = sOptionMenuEntries[submenu].menuOptions[selection];

    FillWindowPixelBuffer(WIN_TEXT_OPTION, PIXEL_FILL(1));

    // Element name
    str = StringCopy(text, sOptionMenuItems[menuItem].itemName);
    //str = StringAppend(str, gText_DifficultyDoesntAffectReward); // TODO - hookup hint?

    AddTextPrinterParameterized(WIN_TEXT_OPTION, FONT_NORMAL, text, 8, 1, TEXT_SKIP_DRAW, NULL);

    // Place current reward level
    str = StringCopy(text, gText_DifficultyRewardLevel);
    
    switch (Rogue_GetDifficultyRewardLevel())
    {
    case DIFFICULTY_LEVEL_EASY:
        str = StringAppend(str, gText_DifficultyPresetEasy);
        break;

    case DIFFICULTY_LEVEL_MEDIUM:
        str = StringAppend(str, gText_DifficultyPresetMedium);
        break;

    case DIFFICULTY_LEVEL_HARD:
        str = StringAppend(str, gText_DifficultyPresetHard);
        break;

    case DIFFICULTY_LEVEL_BRUTAL:
        str = StringAppend(str, gText_DifficultyPresetBrutal);
        break;
    
    default:
        // This should never be reached
        str = StringAppend(str, gText_DifficultyPresetCustom);
        break;
    }

    AddTextPrinterParameterized(WIN_TEXT_OPTION, FONT_NORMAL, text, 120, 0, TEXT_SKIP_DRAW, NULL);

    // Element description
    if(sOptionMenuItems[menuItem].itemDesc != NULL)
        AddTextPrinterParameterized(WIN_TEXT_OPTION, FONT_NORMAL, sOptionMenuItems[menuItem].itemDesc, 8, 17, TEXT_SKIP_DRAW, NULL);

    CopyWindowToVram(WIN_TEXT_OPTION, COPYWIN_FULL);
}

static void DrawOptionMenuTexts(u8 submenu, u8 topIndex)
{
    u8 i;
    FillWindowPixelBuffer(WIN_OPTIONS, PIXEL_FILL(1));

    for (i = 0; i < MAX_MENUITEM_TO_DISPLAY; i++)
    {
        u8 menuItem = sOptionMenuEntries[submenu].menuOptions[i + topIndex];

        AddTextPrinterParameterized(WIN_OPTIONS, FONT_NORMAL, sOptionMenuItems[menuItem].itemName, 8, (i * YPOS_SPACING) + 1, TEXT_SKIP_DRAW, NULL);

        if(menuItem == MENUITEM_CANCEL)
            break;
    }

    for (i = 0; i < MAX_MENUITEM_TO_DISPLAY; i++)
    {
        u8 menuItem = sOptionMenuEntries[submenu].menuOptions[i + topIndex];
    
        sOptionMenuItems[menuItem].drawChoices(i, GetMenuItemValue(menuItem));

        if(menuItem == MENUITEM_CANCEL)
            break;
    }

    CopyWindowToVram(WIN_OPTIONS, COPYWIN_FULL);
}

static u8 GetMenuItemValue(u8 menuItem)
{
    switch (menuItem)
    {
    case MENUITEM_DIFFICULTY_PRESET:
        return Rogue_GetDifficultyPreset();

    case MENUITEM_MENU_TOGGLE_EXP_ALL:
        return Rogue_GetConfigToggle(DIFFICULTY_TOGGLE_EXP_ALL);

    case MENUITEM_MENU_TOGGLE_OVER_LVL:
        return Rogue_GetConfigToggle(DIFFICULTY_TOGGLE_OVER_LVL);

    case MENUITEM_MENU_TOGGLE_EV_GAIN:
        return Rogue_GetConfigToggle(DIFFICULTY_TOGGLE_EV_GAIN);

    case MENUITEM_MENU_TOGGLE_OVERWORLD_MONS:
        return Rogue_GetConfigToggle(DIFFICULTY_TOGGLE_OVERWORLD_MONS);

    case MENUITEM_MENU_TOGGLE_BAG_WIPE:
        return Rogue_GetConfigToggle(DIFFICULTY_TOGGLE_BAG_WIPE);

    case MENUITEM_MENU_TOGGLE_SWITCH_MODE:
        return Rogue_GetConfigToggle(DIFFICULTY_TOGGLE_SWITCH_MODE);


    case MENUITEM_MENU_SLIDER_TRAINER:
        return Rogue_GetConfigRange(DIFFICULTY_RANGE_TRAINER);

    case MENUITEM_MENU_SLIDER_ITEM:
        return Rogue_GetConfigRange(DIFFICULTY_RANGE_ITEM);

    case MENUITEM_MENU_SLIDER_LEGENDARY:
        return Rogue_GetConfigRange(DIFFICULTY_RANGE_LEGENDARY);
    }

    return 0;
}

static void SetMenuItemValue(u8 menuItem, u8 value)
{
    switch (menuItem)
    {
    case MENUITEM_DIFFICULTY_PRESET:
        Rogue_SetDifficultyPreset(value);
        break;

    case MENUITEM_MENU_TOGGLE_EXP_ALL:
        Rogue_SetConfigToggle(DIFFICULTY_TOGGLE_EXP_ALL, value);
        break;

    case MENUITEM_MENU_TOGGLE_OVER_LVL:
        Rogue_SetConfigToggle(DIFFICULTY_TOGGLE_OVER_LVL, value);
        break;

    case MENUITEM_MENU_TOGGLE_EV_GAIN:
        Rogue_SetConfigToggle(DIFFICULTY_TOGGLE_EV_GAIN, value);
        break;

    case MENUITEM_MENU_TOGGLE_OVERWORLD_MONS:
        Rogue_SetConfigToggle(DIFFICULTY_TOGGLE_OVERWORLD_MONS, value);
        break;

    case MENUITEM_MENU_TOGGLE_BAG_WIPE:
        Rogue_SetConfigToggle(DIFFICULTY_TOGGLE_BAG_WIPE, value);
        break;

    case MENUITEM_MENU_TOGGLE_SWITCH_MODE:
        Rogue_SetConfigToggle(DIFFICULTY_TOGGLE_SWITCH_MODE, value);
        break;


    case MENUITEM_MENU_SLIDER_TRAINER:
        Rogue_SetConfigRange(DIFFICULTY_RANGE_TRAINER, value);
        break;

    case MENUITEM_MENU_SLIDER_ITEM:
        Rogue_SetConfigRange(DIFFICULTY_RANGE_ITEM, value);
        break;

    case MENUITEM_MENU_SLIDER_LEGENDARY:
        Rogue_SetConfigRange(DIFFICULTY_RANGE_LEGENDARY, value);
        break;
    }
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

    for(i = 0; i < ARRAY_COUNT(sOptionMenuWinTemplates) - 1; ++i)
    {
        bg = sOptionMenuWinTemplates[i].bg;

        left = sOptionMenuWinTemplates[i].tilemapLeft - 1;
        right = sOptionMenuWinTemplates[i].tilemapLeft + sOptionMenuWinTemplates[i].width;
        top = sOptionMenuWinTemplates[i].tilemapTop - 1;
        bottom = sOptionMenuWinTemplates[i].tilemapTop + sOptionMenuWinTemplates[i].height;

        leftValid = left <= 29 && left <= right;
        rightValid = right <= 29 && right >= left;
        topValid = top <= 19 && top <= bottom;
        bottomValid = bottom <= 19 && bottom >= bottom;

        //                     bg, tile,              x, y, width, height, palNum
        // Draw title window frame
        if(topValid && leftValid)
            FillBgTilemapBufferRect(bg, TILE_TOP_CORNER_L, left, top, 1, 1, 7);

        if(topValid && rightValid)
            FillBgTilemapBufferRect(bg, TILE_TOP_CORNER_R, right, top, 1, 1, 7);
            
        if(bottomValid && leftValid)
            FillBgTilemapBufferRect(bg, TILE_BOT_CORNER_L, left, bottom, 1, 1, 7);

        if(bottomValid && rightValid)
            FillBgTilemapBufferRect(bg, TILE_BOT_CORNER_R, right, bottom, 1, 1, 7);


        if(topValid)
            FillBgTilemapBufferRect(bg, TILE_TOP_EDGE, left + 1, top, (right - left - 1), 1, 7);

        if(bottomValid)
            FillBgTilemapBufferRect(bg, TILE_BOT_EDGE, left + 1, bottom, (right - left - 1), 1, 7);

        if(leftValid)
            FillBgTilemapBufferRect(bg, TILE_LEFT_EDGE, left, top + 1, 1, (bottom - top - 1), 7);

        if(rightValid)
            FillBgTilemapBufferRect(bg, TILE_RIGHT_EDGE, right, top + 1, 1, (bottom - top - 1), 7);

        CopyBgTilemapBufferToVram(bg);
    }
}
