#include "global.h"
#include "main.h"
#include "battle_main.h"
#include "data.h"
#include "event_data.h"
#include "field_effect.h"
#include "field_specials.h"
#include "item.h"
#include "list_menu.h"
#include "malloc.h"
#include "menu.h"
#include "palette.h"
#include "party_menu.h"
#include "pokemon_summary_screen.h"
#include "script.h"
#include "script_menu.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "constants/field_specials.h"
#include "constants/items.h"
#include "constants/rgb.h"
#include "constants/script_menu.h"
#include "constants/songs.h"

#include "rogue_controller.h"
#include "rogue_gifts.h"
#include "rogue_hub.h"
#include "rogue_pokedex.h"

#include "data/script_menu.h"

static EWRAM_DATA u8 sProcessInputDelay = 0;

static u8 sLilycoveSSTidalSelections[SSTIDAL_SELECTION_COUNT];

static void Task_HandleMultichoiceInput(u8 taskId);
static void Task_HandleYesNoInput(u8 taskId);
static void Task_HandleMultichoiceGridInput(u8 taskId);
static void DrawMultichoiceMenu(u8 left, u8 top, u8 multichoiceId, bool8 ignoreBPress, u8 cursorPos);
static void InitMultichoiceCheckWrap(bool8 ignoreBPress, u8 count, u8 windowId, u8 multichoiceId);
static void DrawLinkServicesMultichoiceMenu(u8 multichoiceId);
static void CreatePCMultichoice(void);
static void CreateLilycoveSSTidalMultichoice(void);
static bool8 IsPicboxClosed(void);
static void CreateStartMenuForPokenavTutorial(void);
static void InitMultichoiceNoWrap(bool8 ignoreBPress, u8 unusedCount, u8 windowId, u8 multichoiceId);

bool8 ScriptMenu_Multichoice(u8 left, u8 top, u8 multichoiceId, bool8 ignoreBPress)
{
    if (FuncIsActiveTask(Task_HandleMultichoiceInput) == TRUE)
    {
        return FALSE;
    }
    else
    {
        gSpecialVar_Result = 0xFF;
        DrawMultichoiceMenu(left, top, multichoiceId, ignoreBPress, 0);
        return TRUE;
    }
}

static void MultichoiceLists_GetList(u8 list, struct MenuAction* dest, u8* outCount)
{
    if(list >= MULTI_DYNAMIC_CALLBACK_START)
    {
        sMultichoiceCallback[list](dest, outCount, MULTICHOICE_LIST_CAPACITY);
    }
    else
    {
        u8 count;
        count = sMultichoiceLists[list].count;
        memcpy(dest, sMultichoiceLists[list].list, sizeof(struct MenuAction) * count);

        *outCount = count;
    }
}

u8 ScriptMenu_MultichoiceLength(u8 multichoiceId)
{
    u8 count;
    struct MenuAction actions[MULTICHOICE_LIST_CAPACITY];
    MultichoiceLists_GetList(multichoiceId, &actions[0], &count);
    return count;
}

bool8 ScriptMenu_MultichoiceWithDefault(u8 left, u8 top, u8 multichoiceId, bool8 ignoreBPress, u8 defaultChoice)
{
    if (FuncIsActiveTask(Task_HandleMultichoiceInput) == TRUE)
    {
        return FALSE;
    }
    else
    {
        gSpecialVar_Result = 0xFF;
        DrawMultichoiceMenu(left, top, multichoiceId, ignoreBPress, defaultChoice);
        return TRUE;
    }
}

// Unused
static u16 GetLengthWithExpandedPlayerName(const u8 *str)
{
    u16 length = 0;

    while (*str != EOS)
    {
        if (*str == PLACEHOLDER_BEGIN)
        {
            str++;
            if (*str == PLACEHOLDER_ID_PLAYER)
            {
                length += StringLength(gSaveBlock2Ptr->playerName);
                str++;
            }
        }
        else
        {
            str++;
            length++;
        }
    }

    return length;
}

static void DrawMultichoiceMenu(u8 left, u8 top, u8 multichoiceId, bool8 ignoreBPress, u8 cursorPos)
{
    int i;
    u8 windowId;
    u8 count;
    struct MenuAction actions[MULTICHOICE_LIST_CAPACITY];
    int width = 0;
    u8 newWidth;

    MultichoiceLists_GetList(multichoiceId, &actions[0], &count);

    for (i = 0; i < count; i++)
    {
        StringExpandPlaceholders(gStringVar4, actions[i].text);
        width = DisplayTextAndGetWidth(gStringVar4, width);
    }

    newWidth = ConvertPixelWidthToTileWidth(width);
    left = ScriptMenu_AdjustLeftCoordFromWidth(left, newWidth);
    windowId = CreateWindowFromRect(left, top, newWidth, count * 2);
    SetStandardWindowBorderStyle(windowId, 0);
    PrintMenuTable(windowId, count, &actions[0]);
    InitMenuInUpperLeftCornerNormal(windowId, count, cursorPos);
    ScheduleBgCopyTilemapToVram(0);
    InitMultichoiceCheckWrap(ignoreBPress, count, windowId, multichoiceId);
}

#define tLeft           data[0]
#define tTop            data[1]
#define tRight          data[2]
#define tBottom         data[3]
#define tIgnoreBPress   data[4]
#define tDoWrap         data[5]
#define tWindowId       data[6]
#define tMultichoiceId  data[7]

static void InitMultichoiceCheckWrap(bool8 ignoreBPress, u8 count, u8 windowId, u8 multichoiceId)
{
    u8 i;
    u8 taskId;
    sProcessInputDelay = 2;

    for (i = 0; i < ARRAY_COUNT(sLinkServicesMultichoiceIds); i++)
    {
        if (sLinkServicesMultichoiceIds[i] == multichoiceId)
        {
            sProcessInputDelay = 12;
        }
    }

    taskId = CreateTask(Task_HandleMultichoiceInput, 80);

    gTasks[taskId].tIgnoreBPress = ignoreBPress;

    if (count > 3)
        gTasks[taskId].tDoWrap = TRUE;
    else
        gTasks[taskId].tDoWrap = FALSE;

    gTasks[taskId].tWindowId = windowId;
    gTasks[taskId].tMultichoiceId = multichoiceId;

    DrawLinkServicesMultichoiceMenu(multichoiceId);
}

static void Task_HandleMultichoiceInput(u8 taskId)
{
    s8 selection;
    s16 *data = gTasks[taskId].data;

    if (!gPaletteFade.active)
    {
        if (sProcessInputDelay)
        {
            sProcessInputDelay--;
        }
        else
        {
            if (!tDoWrap)
                selection = Menu_ProcessInputNoWrap();
            else
                selection = Menu_ProcessInput();

            if (JOY_NEW(DPAD_UP | DPAD_DOWN))
            {
                DrawLinkServicesMultichoiceMenu(tMultichoiceId);
            }

            if (selection != MENU_NOTHING_CHOSEN)
            {
                if (selection == MENU_B_PRESSED)
                {
                    if (tIgnoreBPress)
                        return;
                    PlaySE(SE_SELECT);
                    gSpecialVar_Result = MULTI_B_PRESSED;
                }
                else
                {
                    gSpecialVar_Result = selection;
                }
                ClearToTransparentAndRemoveWindow(tWindowId);
                DestroyTask(taskId);
                ScriptContext_Enable();
            }
        }
    }
}

bool8 ScriptMenu_YesNo(u8 left, u8 top)
{
    u8 taskId;

    if (FuncIsActiveTask(Task_HandleYesNoInput) == TRUE)
    {
        return FALSE;
    }
    else
    {
        gSpecialVar_Result = 0xFF;
        DisplayYesNoMenuDefaultYes();
        taskId = CreateTask(Task_HandleYesNoInput, 0x50);
        return TRUE;
    }
}

// Unused
bool8 IsScriptActive(void)
{
    if (gSpecialVar_Result == 0xFF)
        return FALSE;
    else
        return TRUE;
}

static void Task_HandleYesNoInput(u8 taskId)
{
    if (gTasks[taskId].tRight < 5)
    {
        gTasks[taskId].tRight++;
        return;
    }

    switch (Menu_ProcessInputNoWrapClearOnChoose())
    {
    case MENU_NOTHING_CHOSEN:
        return;
    case MENU_B_PRESSED:
    case 1:
        PlaySE(SE_SELECT);
        gSpecialVar_Result = 0;
        break;
    case 0:
        gSpecialVar_Result = 1;
        break;
    }

    DestroyTask(taskId);
    ScriptContext_Enable();
}

bool8 ScriptMenu_MultichoiceGrid(u8 left, u8 top, u8 multichoiceId, bool8 ignoreBPress, u8 columnCount)
{
    if (FuncIsActiveTask(Task_HandleMultichoiceGridInput) == TRUE)
    {
        return FALSE;
    }
    else
    {
        u8 taskId;
        u8 rowCount, newWidth;
        int i, width;
        u8 listCount;
        struct MenuAction actions[MULTICHOICE_LIST_CAPACITY];

        gSpecialVar_Result = 0xFF;
        width = 0;
        
        MultichoiceLists_GetList(multichoiceId, &actions[0], &listCount);

        for (i = 0; i < listCount; i++)
        {
            StringExpandPlaceholders(gStringVar4, actions[i].text);
            width = DisplayTextAndGetWidth(gStringVar4, width);
        }

        newWidth = ConvertPixelWidthToTileWidth(width);

        left = ScriptMenu_AdjustLeftCoordFromWidth(left, columnCount * newWidth);
        rowCount = listCount / columnCount;

        taskId = CreateTask(Task_HandleMultichoiceGridInput, 80);

        gTasks[taskId].tIgnoreBPress = ignoreBPress;
        gTasks[taskId].tWindowId = CreateWindowFromRect(left, top, columnCount * newWidth, rowCount * 2);
        SetStandardWindowBorderStyle(gTasks[taskId].tWindowId, 0);
        PrintMenuGridTable(gTasks[taskId].tWindowId, newWidth * 8, columnCount, rowCount, &actions[0]);
        InitMenuActionGrid(gTasks[taskId].tWindowId, newWidth * 8, columnCount, rowCount, 0);
        CopyWindowToVram(gTasks[taskId].tWindowId, COPYWIN_FULL);
        return TRUE;
    }
}

static void Task_HandleMultichoiceGridInput(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    s8 selection = Menu_ProcessGridInput();

    switch (selection)
    {
    case MENU_NOTHING_CHOSEN:
        return;
    case MENU_B_PRESSED:
        if (tIgnoreBPress)
            return;
        PlaySE(SE_SELECT);
        gSpecialVar_Result = MULTI_B_PRESSED;
        break;
    default:
        gSpecialVar_Result = selection;
        break;
    }

    ClearToTransparentAndRemoveWindow(tWindowId);
    DestroyTask(taskId);
    ScriptContext_Enable();
}

#undef tWindowId

bool16 ScriptMenu_CreatePCMultichoice(void)
{
    if (FuncIsActiveTask(Task_HandleMultichoiceInput) == TRUE)
    {
        return FALSE;
    }
    else
    {
        gSpecialVar_Result = 0xFF;
        CreatePCMultichoice();
        return TRUE;
    }
}

static void CreatePCMultichoice(void)
{
    u8 y = 8;
    u32 pixelWidth = 0;
    u8 width;
    u8 numChoices;
    u8 windowId;
    int i;

    for (i = 0; i < ARRAY_COUNT(sPCNameStrings); i++)
    {
        pixelWidth = DisplayTextAndGetWidth(sPCNameStrings[i], pixelWidth);
    }

    if (FlagGet(FLAG_SYS_GAME_CLEAR))
    {
        pixelWidth = DisplayTextAndGetWidth(gText_HallOfFame, pixelWidth);
    }

    width = ConvertPixelWidthToTileWidth(pixelWidth);

    // Include Hall of Fame option if player is champion
    if (FlagGet(FLAG_SYS_GAME_CLEAR))
    {
        numChoices = 4;
        windowId = CreateWindowFromRect(0, 0, width, 8);
        SetStandardWindowBorderStyle(windowId, 0);
        AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_HallOfFame, y, 33, TEXT_SKIP_DRAW, NULL);
        AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_LogOff, y, 49, TEXT_SKIP_DRAW, NULL);
    }
    else
    {
        numChoices = 3;
        windowId = CreateWindowFromRect(0, 0, width, 6);
        SetStandardWindowBorderStyle(windowId, 0);
        AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_LogOff, y, 33, TEXT_SKIP_DRAW, NULL);
    }

    // Change PC name if player has met Lanette
    //if (FlagGet(FLAG_SYS_PC_LANETTE))
    //    AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_LanettesPC, y, 1, TEXT_SKIP_DRAW, NULL);
    //else
        AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_SomeonesPC, y, 1, TEXT_SKIP_DRAW, NULL);

    StringExpandPlaceholders(gStringVar4, gText_PlayersPC);
    PrintPlayerNameOnWindow(windowId, gStringVar4, y, 17);
    InitMenuInUpperLeftCornerNormal(windowId, numChoices, 0);
    CopyWindowToVram(windowId, COPYWIN_FULL);
    InitMultichoiceCheckWrap(FALSE, numChoices, windowId, MULTI_PC);
}

void ScriptMenu_DisplayPCStartupPrompt(void)
{
    LoadMessageBoxAndFrameGfx(0, TRUE);
    AddTextPrinterParameterized2(0, FONT_NORMAL, gText_WhichPCShouldBeAccessed, 0, NULL, 2, 1, 3);
}

bool8 ScriptMenu_CreateLilycoveSSTidalMultichoice(void)
{
    if (FuncIsActiveTask(Task_HandleMultichoiceInput) == TRUE)
    {
        return FALSE;
    }
    else
    {
        gSpecialVar_Result = 0xFF;
        CreateLilycoveSSTidalMultichoice();
        return TRUE;
    }
}

// gSpecialVar_0x8004 is 1 if the Sailor was shown multiple event tickets at the same time
// otherwise gSpecialVar_0x8004 is 0
static void CreateLilycoveSSTidalMultichoice(void)
{
    u8 selectionCount = 0;
    u8 count;
    u32 pixelWidth;
    u8 width;
    u8 windowId;
    u8 i;
    u32 j;

    for (i = 0; i < SSTIDAL_SELECTION_COUNT; i++)
    {
        sLilycoveSSTidalSelections[i] = 0xFF;
    }

    GetFontAttribute(FONT_NORMAL, FONTATTR_MAX_LETTER_WIDTH);

    if (gSpecialVar_0x8004 == 0)
    {
        sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_SLATEPORT;
        selectionCount++;

        if (FlagGet(FLAG_MET_SCOTT_ON_SS_TIDAL) == TRUE)
        {
            sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_BATTLE_FRONTIER;
            selectionCount++;
        }
    }

    if (CheckBagHasItem(ITEM_EON_TICKET, 1) == TRUE && FlagGet(FLAG_ENABLE_SHIP_SOUTHERN_ISLAND) == TRUE)
    {
        if (gSpecialVar_0x8004 == 0)
        {
            sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_SOUTHERN_ISLAND;
            selectionCount++;
        }

        if (gSpecialVar_0x8004 == 1 && FlagGet(FLAG_SHOWN_EON_TICKET) == FALSE)
        {
            sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_SOUTHERN_ISLAND;
            selectionCount++;
            FlagSet(FLAG_SHOWN_EON_TICKET);
        }
    }

    if (CheckBagHasItem(ITEM_MYSTIC_TICKET, 1) == TRUE && FlagGet(FLAG_ENABLE_SHIP_NAVEL_ROCK) == TRUE)
    {
        if (gSpecialVar_0x8004 == 0)
        {
            sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_NAVEL_ROCK;
            selectionCount++;
        }

        if (gSpecialVar_0x8004 == 1 && FlagGet(FLAG_SHOWN_MYSTIC_TICKET) == FALSE)
        {
            sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_NAVEL_ROCK;
            selectionCount++;
            FlagSet(FLAG_SHOWN_MYSTIC_TICKET);
        }
    }

    if (CheckBagHasItem(ITEM_AURORA_TICKET, 1) == TRUE && FlagGet(FLAG_ENABLE_SHIP_BIRTH_ISLAND) == TRUE)
    {
        if (gSpecialVar_0x8004 == 0)
        {
            sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_BIRTH_ISLAND;
            selectionCount++;
        }

        if (gSpecialVar_0x8004 == 1 && FlagGet(FLAG_SHOWN_AURORA_TICKET) == FALSE)
        {
            sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_BIRTH_ISLAND;
            selectionCount++;
            FlagSet(FLAG_SHOWN_AURORA_TICKET);
        }
    }

    if (CheckBagHasItem(ITEM_OLD_SEA_MAP, 1) == TRUE && FlagGet(FLAG_ENABLE_SHIP_FARAWAY_ISLAND) == TRUE)
    {
        if (gSpecialVar_0x8004 == 0)
        {
            sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_FARAWAY_ISLAND;
            selectionCount++;
        }

        if (gSpecialVar_0x8004 == 1 && FlagGet(FLAG_SHOWN_OLD_SEA_MAP) == FALSE)
        {
            sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_FARAWAY_ISLAND;
            selectionCount++;
            FlagSet(FLAG_SHOWN_OLD_SEA_MAP);
        }
    }

    sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_EXIT;
    selectionCount++;

    if (gSpecialVar_0x8004 == 0 && FlagGet(FLAG_MET_SCOTT_ON_SS_TIDAL) == TRUE)
    {
        count = selectionCount;
    }

    count = selectionCount;
    if (count == SSTIDAL_SELECTION_COUNT)
    {
        gSpecialVar_0x8004 = SCROLL_MULTI_SS_TIDAL_DESTINATION;
        ShowScrollableMultichoice();
    }
    else
    {
        pixelWidth = 0;

        for (j = 0; j < SSTIDAL_SELECTION_COUNT; j++)
        {
            u8 selection = sLilycoveSSTidalSelections[j];
            if (selection != 0xFF)
            {
                pixelWidth = DisplayTextAndGetWidth(sLilycoveSSTidalDestinations[selection], pixelWidth);
            }
        }

        width = ConvertPixelWidthToTileWidth(pixelWidth);
        windowId = CreateWindowFromRect(MAX_MULTICHOICE_WIDTH - width, (6 - count) * 2, width, count * 2);
        SetStandardWindowBorderStyle(windowId, 0);

        for (selectionCount = 0, i = 0; i < SSTIDAL_SELECTION_COUNT; i++)
        {
            if (sLilycoveSSTidalSelections[i] != 0xFF)
            {
                AddTextPrinterParameterized(windowId, FONT_NORMAL, sLilycoveSSTidalDestinations[sLilycoveSSTidalSelections[i]], 8, selectionCount * 16 + 1, TEXT_SKIP_DRAW, NULL);
                selectionCount++;
            }
        }

        InitMenuInUpperLeftCornerNormal(windowId, count, count - 1);
        CopyWindowToVram(windowId, COPYWIN_FULL);
        InitMultichoiceCheckWrap(FALSE, count, windowId, MULTI_SSTIDAL_LILYCOVE);
    }
}

void GetLilycoveSSTidalSelection(void)
{
    if (gSpecialVar_Result != MULTI_B_PRESSED)
    {
        gSpecialVar_Result = sLilycoveSSTidalSelections[gSpecialVar_Result];
    }
}

#define tState       data[0]
#define tMonSpecies  data[1]
#define tMonSpriteId data[2]
#define tWindowX     data[3]
#define tWindowY     data[4]
#define tWindowId    data[5]

static void Task_PokemonPicWindow(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->tState)
    {
    case 0:
        task->tState++;
        break;
    case 1:
        // Wait until state is advanced by ScriptMenu_HidePokemonPic
        break;
    case 2:
        FreeResourcesAndDestroySprite(&gSprites[task->tMonSpriteId], task->tMonSpriteId);
        task->tState++;
        break;
    case 3:
        ClearToTransparentAndRemoveWindow(task->tWindowId);
        DestroyTask(taskId);
        break;
    }
}

bool8 ScriptMenu_ShowPokemonPic(u16 species, u8 x, u8 y, bool8 isObscured)
{
    u8 taskId;
    u8 spriteId;

    if (FindTaskIdByFunc(Task_PokemonPicWindow) != TASK_NONE)
    {
        return FALSE;
    }
    else
    {
        spriteId = CreateMonSprite_PicBox(species, x * 8 + 40, y * 8 + 40, 0);
        taskId = CreateTask(Task_PokemonPicWindow, 0x50);
        gTasks[taskId].tWindowId = CreateWindowFromRect(x, y, 8, 8);
        gTasks[taskId].tState = 0;
        gTasks[taskId].tMonSpecies = species;
        gTasks[taskId].tMonSpriteId = spriteId;
        gSprites[spriteId].callback = SpriteCallbackDummy;
        gSprites[spriteId].oam.priority = 0;

        if(isObscured)
        {
            // black out palette
            TintPalette_StompColour(&gPlttBufferUnfaded[OBJ_PLTT_ID(gSprites[spriteId].oam.paletteNum)], 16, RGB(1, 1, 1));
            TintPalette_StompColour(&gPlttBufferFaded[OBJ_PLTT_ID(gSprites[spriteId].oam.paletteNum)], 16, RGB(1, 1, 1));
        }

        SetStandardWindowBorderStyle(gTasks[taskId].tWindowId, 1);
        ScheduleBgCopyTilemapToVram(0);
        return TRUE;
    }
}

bool8 (*ScriptMenu_HidePokemonPic(void))(void)
{
    u8 taskId = FindTaskIdByFunc(Task_PokemonPicWindow);

    if (taskId == TASK_NONE)
        return NULL;
    gTasks[taskId].tState++;
    return IsPicboxClosed;
}

static bool8 IsPicboxClosed(void)
{
    if (FindTaskIdByFunc(Task_PokemonPicWindow) == TASK_NONE)
        return TRUE;
    else
        return FALSE;
}

#undef tState
#undef tMonSpecies
#undef tMonSpriteId
#undef tWindowX
#undef tWindowY
#undef tWindowId

u8 CreateWindowFromRect(u8 x, u8 y, u8 width, u8 height)
{
    struct WindowTemplate template = CreateWindowTemplate(0, x + 1, y + 1, width, height, 15, 100);
    u8 windowId = AddWindow(&template);
    PutWindowTilemap(windowId);
    return windowId;
}

void ClearToTransparentAndRemoveWindow(u8 windowId)
{
    ClearStdWindowAndFrameToTransparent(windowId, TRUE);
    RemoveWindow(windowId);
}

static void DrawLinkServicesMultichoiceMenu(u8 multichoiceId)
{
    switch (multichoiceId)
    {
    case MULTI_WIRELESS_NO_BERRY:
        FillWindowPixelBuffer(0, PIXEL_FILL(1));
        AddTextPrinterParameterized2(0, FONT_NORMAL, sWirelessOptionsNoBerryCrush[Menu_GetCursorPos()], 0, NULL, 2, 1, 3);
        break;
    case MULTI_CABLE_CLUB_WITH_RECORD_MIX:
        FillWindowPixelBuffer(0, PIXEL_FILL(1));
        AddTextPrinterParameterized2(0, FONT_NORMAL, sCableClubOptions_WithRecordMix[Menu_GetCursorPos()], 0, NULL, 2, 1, 3);
        break;
    case MULTI_WIRELESS_NO_RECORD:
        FillWindowPixelBuffer(0, PIXEL_FILL(1));
        AddTextPrinterParameterized2(0, FONT_NORMAL, sWirelessOptions_NoRecordMix[Menu_GetCursorPos()], 0, NULL, 2, 1, 3);
        break;
    case MULTI_WIRELESS_ALL_SERVICES:
        FillWindowPixelBuffer(0, PIXEL_FILL(1));
        AddTextPrinterParameterized2(0, FONT_NORMAL, sWirelessOptions_AllServices[Menu_GetCursorPos()], 0, NULL, 2, 1, 3);
        break;
    case MULTI_WIRELESS_NO_RECORD_BERRY:
        FillWindowPixelBuffer(0, PIXEL_FILL(1));
        AddTextPrinterParameterized2(0, FONT_NORMAL, sWirelessOptions_NoRecordMixBerryCrush[Menu_GetCursorPos()], 0, NULL, 2, 1, 3);
        break;
    case MULTI_CABLE_CLUB_NO_RECORD_MIX:
        FillWindowPixelBuffer(0, PIXEL_FILL(1));
        AddTextPrinterParameterized2(0, FONT_NORMAL, sCableClubOptions_NoRecordMix[Menu_GetCursorPos()], 0, NULL, 2, 1, 3);
        break;
    }
}

bool16 ScriptMenu_CreateStartMenuForPokenavTutorial(void)
{
    if (FuncIsActiveTask(Task_HandleMultichoiceInput) == TRUE)
    {
        return FALSE;
    }
    else
    {
        gSpecialVar_Result = 0xFF;
        CreateStartMenuForPokenavTutorial();
        return TRUE;
    }
}

static void CreateStartMenuForPokenavTutorial(void)
{
    u8 windowId = CreateWindowFromRect(21, 0, 7, 18);
    SetStandardWindowBorderStyle(windowId, 0);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_MenuOptionPokedex, 8, 9, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_MenuOptionPokemon, 8, 25, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_MenuOptionBag, 8, 41, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_MenuOptionPokenav, 8, 57, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, gSaveBlock2Ptr->playerName, 8, 73, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_MenuOptionSave, 8, 89, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_MenuOptionOption, 8, 105, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_MenuOptionExit, 8, 121, TEXT_SKIP_DRAW, NULL);
    InitMenuNormal(windowId, FONT_NORMAL, 0, 9, 16, ARRAY_COUNT(MultichoiceList_ForcedStartMenu), 0);
    InitMultichoiceNoWrap(FALSE, ARRAY_COUNT(MultichoiceList_ForcedStartMenu), windowId, MULTI_FORCED_START_MENU);
    CopyWindowToVram(windowId, COPYWIN_FULL);
}

#define tWindowId       data[6]

static void InitMultichoiceNoWrap(bool8 ignoreBPress, u8 unusedCount, u8 windowId, u8 multichoiceId)
{
    u8 taskId;
    sProcessInputDelay = 2;
    taskId = CreateTask(Task_HandleMultichoiceInput, 80);
    gTasks[taskId].tIgnoreBPress = ignoreBPress;
    gTasks[taskId].tDoWrap = 0;
    gTasks[taskId].tWindowId = windowId;
    gTasks[taskId].tMultichoiceId = multichoiceId;
}

#undef tLeft
#undef tTop
#undef tRight
#undef tBottom
#undef tIgnoreBPress
#undef tDoWrap
#undef tWindowId
#undef tMultichoiceId

static int DisplayTextAndGetWidthInternal(const u8 *str)
{
    u8 temp[64];
    StringExpandPlaceholders(temp, str);
    return GetStringWidth(FONT_NORMAL, temp, 0);
}

int DisplayTextAndGetWidth(const u8 *str, int prevWidth)
{
    int width = DisplayTextAndGetWidthInternal(str);
    if (width < prevWidth)
    {
        width = prevWidth;
    }
    return width;
}

int ConvertPixelWidthToTileWidth(int width)
{
    return (((width + 9) / 8) + 1) > MAX_MULTICHOICE_WIDTH ? MAX_MULTICHOICE_WIDTH : (((width + 9) / 8) + 1);
}

int ScriptMenu_AdjustLeftCoordFromWidth(int left, int width)
{
    int adjustedLeft = left;

    if (left + width > MAX_MULTICHOICE_WIDTH)
    {
        if (MAX_MULTICHOICE_WIDTH - width < 0)
        {
            adjustedLeft = 0;
        }
        else
        {
            adjustedLeft = MAX_MULTICHOICE_WIDTH - width;
        }
    }

    return adjustedLeft;
}

// Multichoice lists
//
static void Task_ScrollingMultichoiceInput(u8 taskId);

static const struct ListMenuTemplate sMultichoiceListTemplate =
{
    .header_X = 0,
    .item_X = 8,
    .cursor_X = 0,
    .upText_Y = 1,
    .cursorPal = 2,
    .fillValue = 1,
    .cursorShadowPal = 3,
    .lettersSpacing = 1,
    .itemVerticalPadding = 0,
    .scrollMultiple = LIST_MULTIPLE_SCROLL_L_R,
    .fontId = 1,
    .cursorKind = 0
};

static EWRAM_DATA struct ListMenuItem* sDynamicScrollingMultichoiceList = NULL;
static EWRAM_DATA u16 sDynamicScrollingMultichoiceCount = 0;
#ifdef ROGUE_DEBUG
static EWRAM_DATA u16 sDynamicScrollingMultichoiceCapacity = 0;
#endif

// 0x8004 = set id
// 0x8005 = window X
// 0x8006 = window y
// 0x8007 = showed at once
// 0x8008 = Allow B press
static void ScriptMenu_ScrollingMultichoiceInternal(const struct ListMenuItem *list, u16 listCount, bool8 hasSetSize)
{
    int i, windowId, taskId, width = 0;
    int left = gSpecialVar_0x8005;
    int top = gSpecialVar_0x8006;
    int maxShowed = gSpecialVar_0x8007;

    if(!hasSetSize)
    {
        maxShowed = min(maxShowed, listCount);
    }

    for (i = 0; i < listCount; i++)
        width = DisplayTextAndGetWidth(list[i].name, width);

    width = ConvertPixelWidthToTileWidth(width);
    left = ScriptMenu_AdjustLeftCoordFromWidth(left, width);
    windowId = CreateWindowFromRect(left, top, width, maxShowed * 2);
    SetStandardWindowBorderStyle(windowId, 0);
    CopyWindowToVram(windowId, 3);

    gMultiuseListMenuTemplate = sMultichoiceListTemplate;
    gMultiuseListMenuTemplate.windowId = windowId;
    gMultiuseListMenuTemplate.items = list;
    gMultiuseListMenuTemplate.totalItems = listCount;
    gMultiuseListMenuTemplate.maxShowed = maxShowed;

    taskId = CreateTask(Task_ScrollingMultichoiceInput, 0);
    gTasks[taskId].data[0] = ListMenuInit(&gMultiuseListMenuTemplate, 0, 0);
    gTasks[taskId].data[1] = gSpecialVar_0x8008;
    gTasks[taskId].data[2] = windowId;
}

void ScriptMenu_ScrollingMultichoice(void)
{
    int setId = gSpecialVar_0x8004;
    ScriptMenu_ScrollingMultichoiceInternal(sScrollingMultichoiceLists[setId].list, sScrollingMultichoiceLists[setId].count, TRUE);
}

void ScriptMenu_ScrollingMultichoiceDynamicBegin(u16 capacity)
{
    AGB_ASSERT(sDynamicScrollingMultichoiceList == NULL);
    sDynamicScrollingMultichoiceList = malloc(sizeof(struct ListMenuItem) * capacity);
    sDynamicScrollingMultichoiceCount = 0;
#ifdef ROGUE_DEBUG
    sDynamicScrollingMultichoiceCapacity = capacity;
#endif
}

void ScriptMenu_ScrollingMultichoiceDynamicAppendOption(u8 const* str, u16 value)
{
    AGB_ASSERT(sDynamicScrollingMultichoiceList != NULL);
#ifdef ROGUE_DEBUG
    AGB_ASSERT(sDynamicScrollingMultichoiceCount < sDynamicScrollingMultichoiceCapacity);
#endif

    sDynamicScrollingMultichoiceList[sDynamicScrollingMultichoiceCount].name = str;
    sDynamicScrollingMultichoiceList[sDynamicScrollingMultichoiceCount].id = value;
    sDynamicScrollingMultichoiceCount++;
}

void ScriptMenu_ScrollingMultichoiceDynamicEnd(void)
{
    AGB_ASSERT(sDynamicScrollingMultichoiceList != NULL);
    ScriptMenu_ScrollingMultichoiceInternal(sDynamicScrollingMultichoiceList, sDynamicScrollingMultichoiceCount, FALSE);
}

static void Task_ScrollingMultichoiceInput(u8 taskId)
{
    bool32 done = FALSE;
    s32 input = ListMenu_ProcessInput(gTasks[taskId].data[0]);

    switch (input)
    {
    case LIST_HEADER:
    case LIST_NOTHING_CHOSEN:
        break;
    case LIST_CANCEL:
        if (gTasks[taskId].data[1])
        {
            gSpecialVar_Result = 0x7F;
            done = TRUE;
        }
        break;
    default:
        gSpecialVar_Result = input;
        done = TRUE;
        break;
    }

    if (done)
    {
        DestroyListMenuTask(gTasks[taskId].data[0], NULL, NULL);
        ClearStdWindowAndFrame(gTasks[taskId].data[2], TRUE);
        RemoveWindow(gTasks[taskId].data[2]);
        ScriptContext_Enable();
        DestroyTask(taskId);

        if(sDynamicScrollingMultichoiceList != NULL)
        {
            Free(sDynamicScrollingMultichoiceList);
            sDynamicScrollingMultichoiceList = NULL;
        }
    }
}

static u8 CreateWindowFromRectWithBaseBlockOffset(u8 x, u8 y, u8 width, u8 height, u16 baseBlockOffset)
{
    struct WindowTemplate template = CreateWindowTemplate(0, x + 1, y + 1, width, height, 15, 100 + baseBlockOffset);
    u8 windowId = AddWindow(&template);
    PutWindowTilemap(windowId);
    return windowId;
}

static void Task_DisplayTextInWindowInput(u8 taskId)
{
    if(JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        u8 windowId = gTasks[taskId].data[0];

        ClearStdWindowAndFrame(windowId, TRUE);
        RemoveWindow(windowId);

        ScriptContext_Enable();
        DestroyTask(taskId);
    }
}

void ScriptMenu_DisplayTextInWindow(const u8* str, u8 x, u8 y, u8 width, u8 height)
{
    u8 taskId;
    u8 windowId = CreateWindowFromRectWithBaseBlockOffset(x, y, width, height, 8 * 8);
    SetStandardWindowBorderStyle(windowId, 0);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, str, 2, 0, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(windowId, COPYWIN_FULL);

    taskId = CreateTask(Task_DisplayTextInWindowInput, 0);
    gTasks[taskId].data[0] = windowId;
}

static u8 const sText_UniqueMonTitle[] = _("{STR_VAR_1} {FONT_SMALL_NARROW}{COLOR BLUE}({STR_VAR_2})");
static u8 const sText_UniqueMonTitleRare[] = _("{STR_VAR_1} {FONT_SMALL_NARROW}{COLOR RED}({STR_VAR_2})");
static u8 const sText_UniqueMonAbility[] = _("A/ {COLOR GREEN}{STR_VAR_1}");
static u8 const sText_UniqueMonMove[] = _(" -{STR_VAR_1}");

static void PrintUniqueMonInfoToWindow(u8 windowId)
{
    u8 i, line;
    u16 species = RogueGift_GetDynamicUniqueMon(gSpecialVar_0x8004)->species;
    u32 customMonId = RogueGift_GetDynamicUniqueMon(gSpecialVar_0x8004)->customMonId;
    u8 rarity = RogueGift_GetCustomMonRarity(customMonId);

    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    SetStandardWindowBorderStyle(windowId, 0);

    // Title
    StringCopy(gStringVar1, RoguePokedex_GetSpeciesName(species));
    StringCopy(gStringVar2, RogueGift_GetRarityName(rarity));
    StringExpandPlaceholders(gStringVar4, rarity >= UNIQUE_RARITY_EPIC ? sText_UniqueMonTitleRare : sText_UniqueMonTitle);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, gStringVar4, 2, 0, TEXT_SKIP_DRAW, NULL);

    line = 0;

    // Ability
    if(RogueGift_GetCustomMonAbilityCount(customMonId) != 0)
    {
        u16 ability = RogueGift_GetCustomMonAbility(customMonId, 0);

        StringCopy(gStringVar1, gAbilityNames[ability]);
        StringExpandPlaceholders(gStringVar4, sText_UniqueMonAbility);
        AddTextPrinterParameterized(windowId, FONT_SMALL, gStringVar4, 2, 13 + 13 * (line++), TEXT_SKIP_DRAW, NULL);
    }

    // Moves
    for(i = 0; i < RogueGift_GetCustomMonMoveCount(customMonId); ++i)
    {
        u16 moveId = RogueGift_GetCustomMonMove(customMonId, i);
        
        StringCopy(gStringVar1, gMoveNames[moveId]);
        StringExpandPlaceholders(gStringVar4, sText_UniqueMonMove);
        AddTextPrinterParameterized(windowId, FONT_SMALL, gStringVar4, 2, 13 + 13 * (line++), TEXT_SKIP_DRAW, NULL);
    }

    CopyWindowToVram(windowId, COPYWIN_FULL);
}

void ScriptMenu_DisplayUniqueMonInfo()
{
    u8 taskId;
    u8 windowId = CreateWindowFromRectWithBaseBlockOffset(12, 1, 14, 10, 8 * 8);

    PrintUniqueMonInfoToWindow(windowId);

    taskId = CreateTask(Task_DisplayTextInWindowInput, 0);
    gTasks[taskId].data[0] = windowId;
    gTasks[taskId].data[1] = RogueGift_GetDynamicUniqueMon(gSpecialVar_0x8004)->countDown;
}

static u8 const sText_PresetMonAbility_Has[] = _("Ability/ {COLOR GREEN}{STR_VAR_1}");
static u8 const sText_PresetMonAbility_Missing[] = _("Ability/ {COLOR RED}{STR_VAR_1}");
static u8 const sText_PresetMonItem_Has[] = _("Item/ {COLOR GREEN}{STR_VAR_1}");
static u8 const sText_PresetMonItem_Missing[] = _("Item/ {COLOR RED}{STR_VAR_1}");
static u8 const sText_PresetMonNature_Has[] = _("Nature/ {COLOR GREEN}{STR_VAR_1}");
static u8 const sText_PresetMonNature_Missing[] = _("Nature/ {COLOR RED}{STR_VAR_1}");
static u8 const sText_PresetMonMove_Has[] = _(" -{COLOR GREEN}{STR_VAR_1}");
static u8 const sText_PresetMonMove_Missing[] = _(" -{COLOR RED}{STR_VAR_1}");
static u8 const sText_PresetMonNoData[] = _("No recommendations for\nthis Pokémon.\n\n\n(This Pokémon may need\nto evolve in order to\nget recomendations)");

static void PrintRecommendedMonSetToWindow(u8 windowId, struct Pokemon* mon, struct RoguePokemonCompetitiveSet const* preset)
{
    u8 i, line;
    gSpecialVar_Result = GetMonData(mon, MON_DATA_SPECIES, NULL);

    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    SetStandardWindowBorderStyle(windowId, 0);

    line = 0;

    if(preset != NULL)
    {
        // Ability
        if(preset->ability == ITEM_NONE)
        {
            StringCopyN(gStringVar1, gText_None, ABILITY_NAME_LENGTH);
            StringExpandPlaceholders(gStringVar4, sText_PresetMonAbility_Has);
            AddTextPrinterParameterized(windowId, FONT_SMALL_NARROW, gStringVar4, 2, 12 * (line++), TEXT_SKIP_DRAW, NULL);
        }
        else
        {
            StringCopyN(gStringVar1, gAbilityNames[preset->ability], ABILITY_NAME_LENGTH);
            StringExpandPlaceholders(gStringVar4, GetMonAbility(mon) == preset->ability ? sText_PresetMonAbility_Has : sText_PresetMonAbility_Missing);
            AddTextPrinterParameterized(windowId, FONT_SMALL_NARROW, gStringVar4, 2, 12 * (line++), TEXT_SKIP_DRAW, NULL);
        }

        // Item
        if(preset->heldItem == ITEM_NONE)
        {
            StringCopyN(gStringVar1, gText_None, ITEM_NAME_LENGTH);
            StringExpandPlaceholders(gStringVar4, sText_PresetMonItem_Has);
            AddTextPrinterParameterized(windowId, FONT_SMALL_NARROW, gStringVar4, 2, 12 * (line++), TEXT_SKIP_DRAW, NULL);
        }
        else
        {
            StringCopyN(gStringVar1, ItemId_GetName(preset->heldItem), ITEM_NAME_LENGTH);
            StringExpandPlaceholders(gStringVar4, GetMonData(mon, MON_DATA_HELD_ITEM) == preset->heldItem ? sText_PresetMonItem_Has : sText_PresetMonItem_Missing);
            AddTextPrinterParameterized(windowId, FONT_SMALL_NARROW, gStringVar4, 2, 12 * (line++), TEXT_SKIP_DRAW, NULL);
        }

        // Nature
        StringCopy(gStringVar1, gNatureNamePointers[preset->nature]);
        StringExpandPlaceholders(gStringVar4, GetNature(mon) == preset->nature ? sText_PresetMonNature_Has : sText_PresetMonNature_Missing);
        AddTextPrinterParameterized(windowId, FONT_SMALL_NARROW, gStringVar4, 2, 12 * (line++), TEXT_SKIP_DRAW, NULL);

        // Moves
        for(i = 0; i < MAX_MON_MOVES; ++i)
        {
            u16 moveId = preset->moves[i];

            if(moveId != MOVE_NONE)
            {
                StringCopy(gStringVar1, gMoveNames[moveId]);
                StringExpandPlaceholders(gStringVar4, MonKnowsMove(mon, moveId) ? sText_PresetMonMove_Has : sText_PresetMonMove_Missing);
                AddTextPrinterParameterized(windowId, FONT_SMALL_NARROW, gStringVar4, 2, 12 * (line++), TEXT_SKIP_DRAW, NULL);
            }
        }
    }
    else
    {
        AddTextPrinterParameterized(windowId, FONT_SMALL, sText_PresetMonNoData, 2, 12 * (line++), TEXT_SKIP_DRAW, NULL);
    }

    CopyWindowToVram(windowId, COPYWIN_FULL);
}

static void Task_DisplayRecommendedMonSetInput(u8 taskId)
{
    u8 windowId = gTasks[taskId].data[0];

    if(JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        gSpecialVar_0x8004 = PARTY_SIZE;

        ClearStdWindowAndFrame(windowId, TRUE);
        RemoveWindow(windowId);

        ScriptContext_Enable();
        DestroyTask(taskId);
    }
    else if(JOY_NEW(DPAD_LEFT))
    {
        if(gSpecialVar_0x8004 == 0)
            gSpecialVar_0x8004 = gPlayerPartyCount - 1;
        else
            --gSpecialVar_0x8004; 

        ClearStdWindowAndFrame(windowId, TRUE);
        RemoveWindow(windowId);

        ScriptContext_Enable();
        DestroyTask(taskId);
    }
    else if(JOY_NEW(DPAD_RIGHT))
    {
        gSpecialVar_0x8004 = (gSpecialVar_0x8004 + 1 ) % gPlayerPartyCount;

        ClearStdWindowAndFrame(windowId, TRUE);
        RemoveWindow(windowId);

        ScriptContext_Enable();
        DestroyTask(taskId);
    }
}

static u32 CalculatePresetDisplayScore(struct Pokemon* mon, struct RoguePokemonCompetitiveSet const* preset)
{
    u8 i;
    u32 score = 0;
    u32 temp;

#ifdef ROGUE_EXPANSION
    if(GetNature(mon) == preset->nature)
        score += 3;

    if(GetMonAbility(mon) == preset->ability)
        score += 3;
#else
    // Rate much higher, as cannot change in Vanilla
    if(GetNature(mon) == preset->nature)
        score += 6;

    if(GetMonAbility(mon) == preset->ability)
        score += 6;
#endif

    temp = GetMonData(mon, MON_DATA_HELD_ITEM);
    if(temp == preset->heldItem)
        score += 1;

#ifdef ROGUE_EXPANSION
    if(temp >= ITEM_VENUSAURITE && temp <= ITEM_DIANCITE && !IsMegaEvolutionEnabled())
    {
        return 1;
    }

    if(temp >= ITEM_NORMALIUM_Z && temp <= ITEM_ULTRANECROZIUM_Z && !IsZMovesEnabled())
    {
        return 1;
    }
#endif

    for(i = 0; i < MAX_MON_MOVES; ++i)
    {
        u16 moveId = preset->moves[i];

        if(moveId != MOVE_NONE)
        {
            if(MonKnowsMove(mon, moveId))
                moveId += 2;
        }
    }

    return score;
}

static struct RoguePokemonCompetitiveSet const* SelectMonPreset(struct Pokemon* mon)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES, NULL);

    if(gRoguePokemonProfiles[species].competitiveSetCount != 0)
    {
        u16 i;
        u16 bestIdx = 0;
        u32 bestScore = CalculatePresetDisplayScore(mon, &gRoguePokemonProfiles[species].competitiveSets[0]);

        for(i = 1; i < gRoguePokemonProfiles[species].competitiveSetCount; ++i)
        {
            u32 score = CalculatePresetDisplayScore(mon, &gRoguePokemonProfiles[species].competitiveSets[i]);

            if(score > bestScore)
            {
                bestIdx = i;
                bestScore = score;
            }
        }

        return &gRoguePokemonProfiles[species].competitiveSets[bestIdx];
    }

    return NULL;
}

void ScriptMenu_DisplayRecommendedMonSet()
{
    u8 taskId;
    struct Pokemon* mon = &gPlayerParty[gSpecialVar_0x8004];
    struct RoguePokemonCompetitiveSet const* preset = SelectMonPreset(mon);
    u8 windowId = CreateWindowFromRectWithBaseBlockOffset(12, 1, 14, 11, 8 * 8);

    PrintRecommendedMonSetToWindow(windowId, mon, preset);

    taskId = CreateTask(Task_DisplayRecommendedMonSetInput, 0);
    gTasks[taskId].data[0] = windowId;
}

static void Task_ShowItemDescriptionInput(u8 taskId)
{
}

static u8 const sText_ItemName[] = _("{COLOR BLUE}{STR_VAR_1}");

static void PrintItemDescriptionToWindow(u8 windowId, u16 itemId)
{
    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    SetStandardWindowBorderStyle(windowId, 0);

    StringCopy(gStringVar1, ItemId_GetName(itemId));
    StringExpandPlaceholders(gStringVar4, sText_ItemName);

    gTextFlags.replaceScrollWithNewLine = TRUE;
    AddTextPrinterParameterized(windowId, FONT_NORMAL, gStringVar4, 0, 0, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, ItemId_GetDescription(itemId), 0, 14, TEXT_SKIP_DRAW, NULL);
    gTextFlags.replaceScrollWithNewLine = FALSE;

    CopyWindowToVram(windowId, COPYWIN_FULL);
}

void ScriptMenu_ShowItemDescription()
{
    u8 taskId;
    u8 windowId = CreateWindowFromRect(1, 4, 13, 8);

    PrintItemDescriptionToWindow(windowId, gSpecialVar_0x8004);

    taskId = CreateTask(Task_ShowItemDescriptionInput, 0);
    gTasks[taskId].data[0] = windowId;
}

void ScriptMenu_HideItemDescription()
{
    u8 taskId = FindTaskIdByFunc(Task_ShowItemDescriptionInput);

    if (taskId == TASK_NONE)
        return;

    ClearStdWindowAndFrame(gTasks[taskId].data[0], TRUE);
    RemoveWindow(gTasks[taskId].data[0]);
    DestroyTask(taskId);
}

static u8 const sText_RogueAssistant[] = _("{COLOR BLUE}Rogue Assistant");
static u8 const sText_RogueAssistantInfo[] = _("Download from:\n{COLOR BLUE}https://rogue.assist.pokabbie.com\n\n{COLOR RED}Never download from other links!");

static void PrintRogueAssistantNoticToWindow(u8 windowId)
{
    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    SetStandardWindowBorderStyle(windowId, 0);

    AddTextPrinterParameterized(windowId, FONT_NORMAL, sText_RogueAssistant, 0, 0, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_NARROW, sText_RogueAssistantInfo, 0, 14, TEXT_SKIP_DRAW, NULL);

    CopyWindowToVram(windowId, COPYWIN_FULL);
}

static void Task_ShowRogueAssistantNoticeInput(u8 taskId)
{
}

void ScriptMenu_ShowRogueAssistantNotice()
{
    u8 taskId;
    u8 windowId = CreateWindowFromRect(4, 1, 20, 10);

    PrintRogueAssistantNoticToWindow(windowId);

    taskId = CreateTask(Task_ShowRogueAssistantNoticeInput, 0);
    gTasks[taskId].data[0] = windowId;
}

void ScriptMenu_HideRogueAssistantNotice()
{
    u8 taskId = FindTaskIdByFunc(Task_ShowRogueAssistantNoticeInput);

    if (taskId == TASK_NONE)
        return;

    ClearStdWindowAndFrame(gTasks[taskId].data[0], TRUE);
    RemoveWindow(gTasks[taskId].data[0]);
    DestroyTask(taskId);
}