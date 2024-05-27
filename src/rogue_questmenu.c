#include "global.h"
#include "constants/songs.h"

#include "palette.h"
#include "main.h"
#include "malloc.h"
#include "field_screen_effect.h"
#include "gpu_regs.h"
#include "scanline_effect.h"
#include "task.h"
#include "malloc.h"
#include "decompress.h"
#include "bg.h"
#include "window.h"
#include "script.h"
#include "sound.h"
#include "strings.h"
#include "string_util.h"
#include "text.h"
#include "international_string_util.h"
#include "item_icon.h"
#include "overworld.h"
#include "menu.h"
#include "pokedex.h"
#include "pokemon_icon.h"
#include "constants/rgb.h"

#include "rogue_controller.h"
#include "rogue_gifts.h"
#include "rogue_popup.h"
#include "rogue_quest.h"
#include "rogue_questmenu.h"

#define SCROLL_ITEMS_IN_VIEW 8
#define QUEST_SPRITE_CAPACITY 32

enum {
    TAG_REWARD_ICON_POKEMON_SHINY = 100,
    TAG_REWARD_ICON_POKEMON_CUSTOM,
    TAG_REWARD_ICON_SHOP_ITEM,
    TAG_REWARD_ICON_MONEY,
    TAG_REWARD_ICON_ITEM,
};

typedef void (*QuestMenuCallback)();
typedef void (*QuestMenuCallbackParam)(u8);

extern const u8 gText_DexNational[];
extern const u8 gText_DexHoenn[];
extern const u8 gText_PokedexQuest[];

static void CB2_InitQuestMenu(void);
static void MainCB2(void);
static void SetupPage(u8 page);
static void Task_QuestFadeIn(u8);
static void Task_QuestHandleInput(u8);
static void Task_QuestFadeOut(u8);
static void InitQuestBg(void);
static void InitQuestWindows(void);
static void ClearQuestWindows(void);
static bool8 HandleScrollBehaviour();

static void Setup_FrontPage();
static void Setup_IndexPage();
static void Setup_QuestPage();
static void Setup_QuestBoard();

static void HandleInput_FrontPage(u8 taskId);
static void HandleInput_IndexPage(u8 taskId);
static void HandleInput_QuestPage(u8 taskId);

static void Draw_FrontPage();
static void Draw_IndexPage();
static void Draw_QuestPage();

enum
{
    PAGE_BOOK_FRONT,
    PAGE_BOOK_INDEX,

    PAGE_BOOK_ALL_PINNED,

    PAGE_BOOK_MAIN_TODO,
    PAGE_BOOK_MAIN_ACTIVE,
    PAGE_BOOK_MAIN_INACTIVE,
    PAGE_BOOK_MAIN_COMPLETE,

    PAGE_BOOK_CHALLENGE_TODO,
    PAGE_BOOK_CHALLENGE_ACTIVE,
    PAGE_BOOK_CHALLENGE_INACTIVE,
    PAGE_BOOK_CHALLENGE_COMPLETE,

    PAGE_BOOK_MON_MASTERY_TODO,
    PAGE_BOOK_MON_MASTERY_ACTIVE,
    PAGE_BOOK_MON_MASTERY_INACTIVE,
    PAGE_BOOK_MON_MASTERY_COMPLETE,

    PAGE_QUEST_BOARD, // new quests
    PAGE_COUNT,

    PAGE_NONE = PAGE_COUNT,
};

enum
{
    WIN_LEFT_PAGE,
    WIN_RIGHT_PAGE,
    WIN_COUNT,
};

struct QuestMenuData
{
    struct MenuOption* menuOptionsBuffer;
    u8 backgroundTilemapBuffer[BG_SCREEN_SIZE];
    //u8 textTilemapBuffer[BG_SCREEN_SIZE];
    u8 sprites[QUEST_SPRITE_CAPACITY];
    u32 questListConstIncludeFlags;
    u32 questListStateIncludeFlags;
    u32 questListConstExcludeFlags;
    u32 questListStateExcludeFlags;
    u16 scrollListHead;
    u16 scrollListOffset;
    u16 scrollListCount;
    u16 previousScrollListHead;
    u16 previousScrollListOffset;
    u8 currentPage;
    u8 previousPage;
    u8 menuOptionsBufferCount;
    u8 alphabeticalSort : 1;
};

struct PageData
{
    u32 const* tilemap;
    QuestMenuCallback setupCallback;
    QuestMenuCallbackParam inputCallback;
    QuestMenuCallback drawCallback;
};

struct MenuOption
{
    u8 const* text;
    QuestMenuCallbackParam callback;
    u8 param;
};

static u32 const sFrontpageTilemap[] = INCBIN_U32("graphics/rogue_quest/front_page.bin.lz");
static u32 const sIndexTilemap[] = INCBIN_U32("graphics/rogue_quest/index_page.bin.lz");
static u32 const sInnerTilemap[] = INCBIN_U32("graphics/rogue_quest/inner_page.bin.lz");
static u32 const sQuestboardTilemap[] = INCBIN_U32("graphics/rogue_quest/quest_board.bin.lz");

static u32 const sQuestTiles[] = INCBIN_U32("graphics/rogue_quest/tiles.4bpp.lz");
static u16 const sQuestPalette[] = INCBIN_U16("graphics/rogue_quest/tiles.gbapal");

static const struct PageData sPageData[PAGE_COUNT] =
{
    [PAGE_BOOK_FRONT] = 
    {
        .tilemap = sFrontpageTilemap,
        .setupCallback = Setup_FrontPage,
        .inputCallback = HandleInput_FrontPage,
        .drawCallback = Draw_FrontPage,
    },
    [PAGE_BOOK_INDEX] = 
    {
        .tilemap = sIndexTilemap,
        .setupCallback = Setup_IndexPage,
        .inputCallback = HandleInput_IndexPage,
        .drawCallback = Draw_IndexPage,
    },

    [PAGE_BOOK_ALL_PINNED] = 
    {
        .tilemap = sInnerTilemap,
        .setupCallback = Setup_QuestPage,
        .inputCallback = HandleInput_QuestPage,
        .drawCallback = Draw_QuestPage,
    },

    [PAGE_BOOK_MAIN_TODO] = 
    {
        .tilemap = sInnerTilemap,
        .setupCallback = Setup_QuestPage,
        .inputCallback = HandleInput_QuestPage,
        .drawCallback = Draw_QuestPage,
    },
    [PAGE_BOOK_MAIN_ACTIVE] = 
    {
        .tilemap = sInnerTilemap,
        .setupCallback = Setup_QuestPage,
        .inputCallback = HandleInput_QuestPage,
        .drawCallback = Draw_QuestPage,
    },
    [PAGE_BOOK_MAIN_INACTIVE] = 
    {
        .tilemap = sInnerTilemap,
        .setupCallback = Setup_QuestPage,
        .inputCallback = HandleInput_QuestPage,
        .drawCallback = Draw_QuestPage,
    },
    [PAGE_BOOK_MAIN_COMPLETE] = 
    {
        .tilemap = sInnerTilemap,
        .setupCallback = Setup_QuestPage,
        .inputCallback = HandleInput_QuestPage,
        .drawCallback = Draw_QuestPage,
    },

    [PAGE_BOOK_CHALLENGE_TODO] = 
    {
        .tilemap = sInnerTilemap,
        .setupCallback = Setup_QuestPage,
        .inputCallback = HandleInput_QuestPage,
        .drawCallback = Draw_QuestPage,
    },
    [PAGE_BOOK_CHALLENGE_ACTIVE] = 
    {
        .tilemap = sInnerTilemap,
        .setupCallback = Setup_QuestPage,
        .inputCallback = HandleInput_QuestPage,
        .drawCallback = Draw_QuestPage,
    },
    [PAGE_BOOK_CHALLENGE_INACTIVE] = 
    {
        .tilemap = sInnerTilemap,
        .setupCallback = Setup_QuestPage,
        .inputCallback = HandleInput_QuestPage,
        .drawCallback = Draw_QuestPage,
    },
    [PAGE_BOOK_CHALLENGE_COMPLETE] = 
    {
        .tilemap = sInnerTilemap,
        .setupCallback = Setup_QuestPage,
        .inputCallback = HandleInput_QuestPage,
        .drawCallback = Draw_QuestPage,
    },

    [PAGE_BOOK_MON_MASTERY_TODO] = 
    {
        .tilemap = sInnerTilemap,
        .setupCallback = Setup_QuestPage,
        .inputCallback = HandleInput_QuestPage,
        .drawCallback = Draw_QuestPage,
    },
    [PAGE_BOOK_MON_MASTERY_ACTIVE] = 
    {
        .tilemap = sInnerTilemap,
        .setupCallback = Setup_QuestPage,
        .inputCallback = HandleInput_QuestPage,
        .drawCallback = Draw_QuestPage,
    },
    [PAGE_BOOK_MON_MASTERY_INACTIVE] = 
    {
        .tilemap = sInnerTilemap,
        .setupCallback = Setup_QuestPage,
        .inputCallback = HandleInput_QuestPage,
        .drawCallback = Draw_QuestPage,
    },
    [PAGE_BOOK_MON_MASTERY_COMPLETE] = 
    {
        .tilemap = sInnerTilemap,
        .setupCallback = Setup_QuestPage,
        .inputCallback = HandleInput_QuestPage,
        .drawCallback = Draw_QuestPage,
    },

    [PAGE_QUEST_BOARD] = 
    {
        .tilemap = sQuestboardTilemap,
        .setupCallback = Setup_QuestBoard,
        .inputCallback = HandleInput_QuestPage,
        .drawCallback = Draw_QuestPage,
    }
};

static const struct BgTemplate sQuestBgTemplates[2] =
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

static const struct WindowTemplate sQuestWinTemplates[WIN_COUNT + 1] =
{
    [WIN_LEFT_PAGE] = {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 1,
        .width = 11,
        .height = 18,
        .paletteNum = 15,
        .baseBlock = 1,
    },
    [WIN_RIGHT_PAGE] = {
        .bg = 0,
        .tilemapLeft = 17,
        .tilemapTop = 1,
        .width = 11,
        .height = 18,
        .paletteNum = 15,
        .baseBlock = 199,
    },
    [WIN_COUNT] = DUMMY_WIN_TEMPLATE,
};

static u8 const sText_EarlyGameTodo[] = _("·{COLOR BLUE}To-Do");
static u8 const sText_EarlyGameComplete[] = _("·{COLOR GREEN}Done");
static u8 const sText_EarlyGameActive[] = _("·{COLOR BLUE}Active");
static u8 const sText_EarlyGameInactive[] = _("·{COLOR RED}Inactive");

static u8 const sText_QuestsTodo[] = _("Main·{FONT_SMALL_NARROW}{COLOR BLUE}To-Do");
static u8 const sText_QuestsComplete[] = _("Main·{FONT_SMALL_NARROW}{COLOR GREEN}Done");
static u8 const sText_QuestsActive[] = _("Main·{FONT_SMALL_NARROW}{COLOR BLUE}Active");
static u8 const sText_QuestsInactive[] = _("Main·{FONT_SMALL_NARROW}{COLOR RED}Inactive");

static u8 const sText_ChallengesTodo[] = _("Challenge·{FONT_SMALL_NARROW}{COLOR BLUE}To-Do");
static u8 const sText_ChallengesComplete[] = _("Challenge·{FONT_SMALL_NARROW}{COLOR GREEN}Done");
static u8 const sText_ChallengesActive[] = _("Challenge·{FONT_SMALL_NARROW}{COLOR BLUE}Active");
static u8 const sText_ChallengesInactive[] = _("Challenge·{FONT_SMALL_NARROW}{COLOR RED}Inactiv");

static u8 const sText_MonMasteryTodo[] = _("Mastery·{FONT_SMALL_NARROW}{COLOR BLUE}To-Do");
static u8 const sText_MonMasteryComplete[] = _("Mastery·{FONT_SMALL_NARROW}{COLOR GREEN}Done");
static u8 const sText_MonMasteryActive[] = _("Mastery·{FONT_SMALL_NARROW}{COLOR BLUE}Active");
static u8 const sText_MonMasteryInactive[] = _("Mastery·{FONT_SMALL_NARROW}{COLOR RED}Inactive");

static u8 const sText_Pinned[] = _("·Pinned Quests·");
static u8 const sText_InProgress[] = _("In Progress…");
static u8 const sText_Inactive[] = _("Inactive");
static u8 const sText_Todo[] = _("To-do");
static u8 const sText_Complete[] = _("Complete");
static u8 const sText_Back[] = _("Back");
static u8 const sText_Progress[] = _("Progress");
static u8 const sText_AButtonPin[] = _("{COLOR LIGHT_GRAY}{SHADOW DARK_GRAY}{A_BUTTON} Pin  {SELECT_BUTTON} Sort");

static u8 const sText_MarkerInProgress[] = _("{COLOR BLUE}·In Progress·");
static u8 const sText_MarkerInactive[] = _("{COLOR RED}·Inactive·");
static u8 const sText_MarkerPendingRewards[] = _("{COLOR GREEN}·Ready to Collect!·");
static u8 const sText_MarkerComplete[] = _("{COLOR GREEN}·Complete·");
static u8 const sText_MarkerCompleteEasy[] = _("{COLOR GREEN}·Complete Easy·");
static u8 const sText_MarkerCompleteAverage[] = _("{COLOR GREEN}·Complete Average·");
static u8 const sText_MarkerCompleteHard[] = _("{COLOR GREEN}·Complete Hard·");
static u8 const sText_MarkerCompleteBrutal[] = _("{COLOR GREEN}·Complete Brutal·");
static u8 const sText_MarkerRewards[] = _("{COLOR DARK_GRAY}Rewards");

// Index Page
static u8 const sText_Index_InProgressPerc[] = _("{COLOR BLUE}{STR_VAR_1}%");
static u8 const sText_Index_FinishedPerc[] = _("{COLOR GREEN}{STR_VAR_1}%");
static u8 const sText_Index_ActiveCount[] = _("{COLOR BLUE}{STR_VAR_1} / {STR_VAR_2}");
static u8 const sText_Index_NoneActiveCount[] = _("{COLOR RED}{STR_VAR_1} / {STR_VAR_2}");

static u8 const sText_Index_Quest[] = _("Main");
static u8 const sText_Index_Challenge[] = _("Challenge");
static u8 const sText_Index_Mastery[] = _("Mastery");
static u8 const sText_Index_Total[] = _("Total");
static u8 const sText_Index_ActiveQuests[] = _("Active Quests");

static u8 const sText_Index_PendingRewards[] = _("{COLOR GREEN}Rewards ready to\nbe Collected!");


EWRAM_DATA static struct QuestMenuData* sQuestMenuData = NULL;

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void OpenQuestMenu(RogueQuestMenuCallback callback, u8 page)
{
    RogueQuest_OnTrigger(QUEST_TRIGGER_MISC_UPDATE);

    gMain.savedCallback = callback;
    LockPlayerFieldControls();

    
    sQuestMenuData = AllocZeroed(sizeof(struct QuestMenuData));
    sQuestMenuData->currentPage = page;

    SetMainCallback2(CB2_InitQuestMenu);
    gFieldCallback = FieldCB_ContinueScriptHandleMusic;
}

void Rogue_OpenQuestMenu(RogueQuestMenuCallback callback, bool8 viewQuestBook)
{
    OpenQuestMenu(callback, viewQuestBook ? PAGE_BOOK_FRONT : PAGE_QUEST_BOARD);
}

static void CB2_InitQuestMenu(void)
{
    AGB_ASSERT(sQuestMenuData != NULL);

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

    // TODO - Init quest menu data

    InitQuestBg();
    InitQuestWindows();

    LoadPalette(sQuestPalette, 0, 1 * 32);

    DecompressAndCopyTileDataToVram(1, sQuestTiles, 0, 0, 0);
    while (FreeTempTileDataBuffersIfPossible())
        ;

    SetupPage(sQuestMenuData->currentPage);

    BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
    EnableInterrupts(1);
    SetVBlankCallback(VBlankCB);
    SetMainCallback2(MainCB2);

    CreateTask(Task_QuestFadeIn, 0);
}

static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void SetupPage(u8 page)
{
    u16 prevScrollListHead = sQuestMenuData->scrollListHead;
    u16 prevScrollListOffset = sQuestMenuData->scrollListOffset;
    
    if(sQuestMenuData->previousPage == page)
    {
        sQuestMenuData->scrollListHead = sQuestMenuData->previousScrollListHead;
        sQuestMenuData->scrollListOffset = sQuestMenuData->previousScrollListOffset;
        sQuestMenuData->scrollListCount = 0;
    }
    else
    {
        sQuestMenuData->scrollListHead = 0;
        sQuestMenuData->scrollListOffset = 0;
        sQuestMenuData->scrollListCount = 0;
    }

    sQuestMenuData->previousPage = sQuestMenuData->currentPage;
    sQuestMenuData->previousScrollListHead = prevScrollListHead;
    sQuestMenuData->previousScrollListOffset = prevScrollListOffset;

    sQuestMenuData->currentPage = page;

    sQuestMenuData->questListConstIncludeFlags = QUEST_CONST_NONE;
    sQuestMenuData->questListConstExcludeFlags = QUEST_CONST_NONE;
    sQuestMenuData->questListStateIncludeFlags = QUEST_STATE_NONE;
    sQuestMenuData->questListStateExcludeFlags = QUEST_STATE_NONE;

    //FreeAllWindowBuffers();
    ClearQuestWindows();

    // Free sprites
    FreeAllSpritePalettes();
    ResetSpriteData();
    {
        u8 i;

        for(i = 0; i < QUEST_SPRITE_CAPACITY; ++i)
            sQuestMenuData->sprites[i] = SPRITE_NONE;
    }
    

    if(sPageData[page].setupCallback != NULL)
        sPageData[page].setupCallback();

    LZDecompressWram(sPageData[page].tilemap, sQuestMenuData->backgroundTilemapBuffer);
    CopyBgTilemapBufferToVram(1);

    if(sPageData[page].drawCallback != NULL)
        sPageData[page].drawCallback();

    ResetTempTileDataBuffers();
}

static void Task_QuestFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gTasks[taskId].func = Task_QuestHandleInput;
    }
}

extern const u8 gText_MysteryGiftCantUse[];
extern const u8 gText_MainMenuMysteryEvents[];
extern const u8 gText_MainMenuOption[];

static void StartFadeAndExit(u8 taskId)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    gTasks[taskId].func = Task_QuestFadeOut;
}

static void Task_QuestHandleInput(u8 taskId)
{
    if(sQuestMenuData->currentPage != PAGE_NONE)
    {
        if(sPageData[sQuestMenuData->currentPage].inputCallback != NULL)
        {
            sPageData[sQuestMenuData->currentPage].inputCallback(taskId);
        }
        else
        {
            // Fallback behaviour
            if (JOY_NEW(B_BUTTON))
                StartFadeAndExit(taskId);
        }
    }
}

static void Task_QuestFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if(sQuestMenuData->menuOptionsBuffer)
            Free(sQuestMenuData->menuOptionsBuffer);

        Free(sQuestMenuData);
        sQuestMenuData = NULL;

        FreeAllWindowBuffers();
        DestroyTask(taskId);
        //SetMainCallback2(CB2_ReturnToFieldFadeFromBlack);
        SetMainCallback2(gMain.savedCallback);
    }
}

static void InitQuestBg(void)
{
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sQuestBgTemplates, ARRAY_COUNT(sQuestBgTemplates));
    //SetBgTilemapBuffer(0, sQuestMenuData->textTilemapBuffer);
    SetBgTilemapBuffer(1, sQuestMenuData->backgroundTilemapBuffer);

    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
    ShowBg(0);
    ShowBg(1);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_BLDALPHA, 0);
    SetGpuReg(REG_OFFSET_BLDY, 0);
}

static void InitQuestWindows(void)
{
    FreeAllWindowBuffers();
    InitWindows(sQuestWinTemplates);
    DeactivateAllTextPrinters();
    LoadPalette(gStandardMenuPalette, 0xF0, 0x20);

    ClearQuestWindows();
}

static void ClearQuestWindows(void)
{
    u8 i;

    //CpuFastFill8(PIXEL_FILL(0), sQuestMenuData->textTilemapBuffer, ARRAY_COUNT(sQuestMenuData->textTilemapBuffer));

    for(i = 0; i < WIN_COUNT; ++i)
    {
        FillWindowPixelBuffer(i, PIXEL_FILL(0));
        PutWindowTilemap(i);
        CopyWindowToVram(i, COPYWIN_FULL);
    }
}

static u16 GetCurrentListIndex()
{
    return sQuestMenuData->scrollListHead + sQuestMenuData->scrollListOffset;
}

static bool8 IsQuestIndexVisible(u16 questIndex)
{
    u16 questId = RogueQuest_GetOrderedQuest(questIndex, sQuestMenuData->alphabeticalSort);

    if(!RogueQuest_IsQuestVisible(questId))
        return FALSE;

    if(sQuestMenuData->questListConstIncludeFlags != QUEST_CONST_NONE)
    {
        if(RogueQuest_GetConstFlag(questId, sQuestMenuData->questListConstIncludeFlags) == FALSE)
            return FALSE;
    }

    if(sQuestMenuData->questListConstExcludeFlags != QUEST_CONST_NONE)
    {
        if(RogueQuest_GetConstFlag(questId, sQuestMenuData->questListConstExcludeFlags) == TRUE)
            return FALSE;
    }

    if(sQuestMenuData->questListStateIncludeFlags != QUEST_STATE_NONE)
    {
        if(RogueQuest_GetStateFlag(questId, sQuestMenuData->questListStateIncludeFlags) == FALSE)
            return FALSE;
    }

    if(sQuestMenuData->questListStateExcludeFlags != QUEST_STATE_NONE)
    {
        if(RogueQuest_GetStateFlag(questId, sQuestMenuData->questListStateExcludeFlags) == TRUE)
            return FALSE;
    }

    return TRUE;
}

static void IterateNextVisibleQuestIndex(u16* questIndex)
{
    while(*questIndex < QUEST_ID_COUNT)
    {
        ++*questIndex;

        if(*questIndex == QUEST_ID_COUNT || IsQuestIndexVisible(*questIndex))
            return;
    }
}

static u16 GetFirstVisibleQuestIndex()
{
    u16 questIndex = 0;

    if(IsQuestIndexVisible(questIndex))
        return questIndex;

    IterateNextVisibleQuestIndex(&questIndex);
    return questIndex;
}

static u16 CalculateVisibleQuestCount()
{
    u16 count = 0;
    u16 questIndex = GetFirstVisibleQuestIndex();

    while(questIndex != QUEST_ID_COUNT)
    {
        ++count;
        IterateNextVisibleQuestIndex(&questIndex);
    }

    return count;
}

static u16 GetCurrentListQuestIndex()
{
    u16 i;
    u16 questIndex = GetFirstVisibleQuestIndex();
    u16 selectedIndex = GetCurrentListIndex();

    for(i = 0; i < selectedIndex; ++i)
        IterateNextVisibleQuestIndex(&questIndex);

    return questIndex;
}

static bool8 HandleScrollBehaviour()
{
    u16 prevIndex = GetCurrentListIndex();

    AGB_ASSERT(sQuestMenuData->scrollListCount != 0);

    if (JOY_REPEAT(DPAD_UP) || JOY_REPEAT(L_BUTTON | DPAD_LEFT))
    {
        u8 i;
        u8 loopCount = JOY_REPEAT(L_BUTTON | DPAD_LEFT) ? 7 : 1;

        for(i = 0; i < loopCount; ++i)
        {
            if(sQuestMenuData->scrollListHead + sQuestMenuData->scrollListOffset == 0)
            {
                // Snap to top before looping
                if(i != 0)
                    break;

                if(sQuestMenuData->scrollListCount <= SCROLL_ITEMS_IN_VIEW)
                {
                    sQuestMenuData->scrollListOffset = sQuestMenuData->scrollListCount - 1;
                    sQuestMenuData->scrollListHead = 0;
                }
                else
                {
                    sQuestMenuData->scrollListOffset = SCROLL_ITEMS_IN_VIEW - 1;
                    sQuestMenuData->scrollListHead = sQuestMenuData->scrollListCount - SCROLL_ITEMS_IN_VIEW;
                }

                break;
            }
            else
            {
                if(sQuestMenuData->scrollListOffset != 0)
                    --sQuestMenuData->scrollListOffset;
                else
                    --sQuestMenuData->scrollListHead;
            }
        }
    }
    else if (JOY_REPEAT(DPAD_DOWN) || JOY_REPEAT(R_BUTTON | DPAD_RIGHT))
    {
        u8 i;
        u8 loopCount = JOY_REPEAT(R_BUTTON | DPAD_RIGHT) ? 7 : 1;

        for(i = 0; i < loopCount; ++i)
        {
            if(sQuestMenuData->scrollListHead + sQuestMenuData->scrollListOffset + 1 >= sQuestMenuData->scrollListCount)
            {
                // Snap to bottome before looping
                if(i != 0)
                    break;

                sQuestMenuData->scrollListOffset = 0;
                sQuestMenuData->scrollListHead = 0;

                break;
            }
            else
            {
                ++sQuestMenuData->scrollListOffset;

                if(sQuestMenuData->scrollListOffset >= SCROLL_ITEMS_IN_VIEW)
                {
                    --sQuestMenuData->scrollListOffset;
                    ++sQuestMenuData->scrollListHead;
                }
            }
        }
    }

    if (prevIndex != GetCurrentListIndex())
    {
        PlaySE(SE_DEX_SCROLL);
        return TRUE;
    }

    return FALSE;
}

static void DrawGenericScrollList(struct MenuOption const* options, u16 count)
{
    u16 j, index;
    u8 const color[3] = {0, 2, 3};

    FillWindowPixelBuffer(WIN_RIGHT_PAGE, PIXEL_FILL(0));

    // draw elements in view
    for(j = 0; j < SCROLL_ITEMS_IN_VIEW; ++j)
    {
        // Skip to head of list
        index = sQuestMenuData->scrollListHead + j;

        if(index >= count)
        {
            // Draw nothing as have reached end
            break;
        }

        if(sQuestMenuData->scrollListOffset == j)
        {
            // Prepend pointer for current selected item
            AddTextPrinterParameterized4(WIN_RIGHT_PAGE, FONT_NARROW, 0, 4 + 16 * j, 0, 0, color, TEXT_SKIP_DRAW, gText_SelectorArrow);
        }

        AddTextPrinterParameterized4(WIN_RIGHT_PAGE, FONT_NARROW, 8, 4 + 16 * j, 0, 0, color, TEXT_SKIP_DRAW, options[j].text);
    }

    PutWindowTilemap(WIN_RIGHT_PAGE);
    CopyWindowToVram(WIN_RIGHT_PAGE, COPYWIN_FULL);
}

static void DrawQuestScrollList()
{
    u16 questIndex;
    u16 i;
    u8 const color[3] = {0, 2, 3};

    FillWindowPixelBuffer(WIN_RIGHT_PAGE, PIXEL_FILL(0));

    i = 0;
    questIndex = GetFirstVisibleQuestIndex();

    // Skip to head of list
    for(i = 0; i < sQuestMenuData->scrollListHead; ++i)
        IterateNextVisibleQuestIndex(&questIndex);

    // draw elements in view
    for(i = 0; i < SCROLL_ITEMS_IN_VIEW; ++i)
    {
        if(sQuestMenuData->scrollListOffset == i)
        {
            // Prepend pointer for current selected item
            AddTextPrinterParameterized4(WIN_RIGHT_PAGE, FONT_NARROW, 0, 4 + 16 * i, 0, 0, color, TEXT_SKIP_DRAW, gText_SelectorArrow);
        }

        if(questIndex == QUEST_ID_COUNT)
        {
            AddTextPrinterParameterized4(WIN_RIGHT_PAGE, FONT_NARROW, 8, 4 + 16 * i, 0, 0, color, TEXT_SKIP_DRAW, sText_Back);
            break;
        }
        else
        {
            u16 questId = RogueQuest_GetOrderedQuest(questIndex, sQuestMenuData->alphabeticalSort);
            AddTextPrinterParameterized4(WIN_RIGHT_PAGE, FONT_NARROW, 8, 4 + 16 * i, 0, 0, color, TEXT_SKIP_DRAW, RogueQuest_GetTitle(questId));

            IterateNextVisibleQuestIndex(&questIndex);
        }
    }

    AddTextPrinterParameterized4(WIN_RIGHT_PAGE, FONT_SMALL_NARROW, 0, 3 + 16 * 8, 0, 0, color, TEXT_SKIP_DRAW, sText_AButtonPin);

    PutWindowTilemap(WIN_RIGHT_PAGE);
    CopyWindowToVram(WIN_RIGHT_PAGE, COPYWIN_FULL);
}

// Front page
//

static void Setup_FrontPage()
{

}

static void HandleInput_FrontPage(u8 taskId)
{
    if (JOY_NEW(A_BUTTON | START_BUTTON))
        SetupPage(PAGE_BOOK_INDEX);

    if (JOY_NEW(B_BUTTON))
        StartFadeAndExit(taskId);
}

static void Draw_FrontPage()
{

}

// Index page
//
static struct MenuOption const sMenuOptionsHub[] = 
{
    {
        .text = sText_Pinned,
        .callback = SetupPage,
        .param = PAGE_BOOK_ALL_PINNED,
    },
    {
        .text = sText_QuestsTodo,
        .callback = SetupPage,
        .param = PAGE_BOOK_MAIN_TODO,
    },
    {
        .text = sText_QuestsComplete,
        .callback = SetupPage,
        .param = PAGE_BOOK_MAIN_COMPLETE,
    },

    {
        .text = sText_ChallengesTodo,
        .callback = SetupPage,
        .param = PAGE_BOOK_CHALLENGE_TODO,
    },
    {
        .text = sText_ChallengesComplete,
        .callback = SetupPage,
        .param = PAGE_BOOK_CHALLENGE_COMPLETE,
    },

    {
        .text = sText_MonMasteryTodo,
        .callback = SetupPage,
        .param = PAGE_BOOK_MON_MASTERY_TODO,
    },
    {
        .text = sText_MonMasteryComplete,
        .callback = SetupPage,
        .param = PAGE_BOOK_MON_MASTERY_COMPLETE,
    },

    {
        .text = sText_Back,
        .callback = SetupPage,
        .param = PAGE_BOOK_FRONT,
    },
};

static struct MenuOption const sMenuOptionsAdventure[] = 
{
    {
        .text = sText_Pinned,
        .callback = SetupPage,
        .param = PAGE_BOOK_ALL_PINNED,
    },

    {
        .text = sText_QuestsActive,
        .callback = SetupPage,
        .param = PAGE_BOOK_MAIN_ACTIVE,
    },
    {
        .text = sText_QuestsInactive,
        .callback = SetupPage,
        .param = PAGE_BOOK_MAIN_INACTIVE,
    },

    {
        .text = sText_ChallengesActive,
        .callback = SetupPage,
        .param = PAGE_BOOK_CHALLENGE_ACTIVE,
    },
    {
        .text = sText_ChallengesInactive,
        .callback = SetupPage,
        .param = PAGE_BOOK_CHALLENGE_INACTIVE,
    },

    {
        .text = sText_MonMasteryActive,
        .callback = SetupPage,
        .param = PAGE_BOOK_MON_MASTERY_ACTIVE,
    },
    {
        .text = sText_MonMasteryInactive,
        .callback = SetupPage,
        .param = PAGE_BOOK_MON_MASTERY_INACTIVE,
    },

    {
        .text = sText_Back,
        .callback = SetupPage,
        .param = PAGE_BOOK_FRONT,
    },
};

static bool8 IsIndexPageVisible(u8 page)
{
    switch (page)
    {
    case PAGE_BOOK_CHALLENGE_TODO:
    case PAGE_BOOK_CHALLENGE_ACTIVE:
    case PAGE_BOOK_CHALLENGE_INACTIVE:
    case PAGE_BOOK_CHALLENGE_COMPLETE:
        return RogueQuest_HasUnlockedChallenges();
 
    case PAGE_BOOK_MON_MASTERY_TODO:
    case PAGE_BOOK_MON_MASTERY_ACTIVE:
    case PAGE_BOOK_MON_MASTERY_INACTIVE:
    case PAGE_BOOK_MON_MASTERY_COMPLETE:
        return RogueQuest_HasUnlockedMonMasteries();
    }

    return TRUE;
}

static void OverrideIndexPageOption(struct MenuOption* option)
{
    // Remove the prefix when we haven't unlocked any other categories
    switch (option->param)
    {
    case PAGE_BOOK_MAIN_TODO:
        if(!RogueQuest_HasUnlockedChallenges() && !RogueQuest_HasUnlockedMonMasteries())
            option->text = sText_EarlyGameTodo;
        break;

    case PAGE_BOOK_MAIN_ACTIVE:
        if(!RogueQuest_HasUnlockedChallenges() && !RogueQuest_HasUnlockedMonMasteries())
            option->text = sText_EarlyGameActive;
        break;

    case PAGE_BOOK_MAIN_INACTIVE:
        if(!RogueQuest_HasUnlockedChallenges() && !RogueQuest_HasUnlockedMonMasteries())
            option->text = sText_EarlyGameInactive;
        break;

    case PAGE_BOOK_MAIN_COMPLETE:
        if(!RogueQuest_HasUnlockedChallenges() && !RogueQuest_HasUnlockedMonMasteries())
            option->text = sText_EarlyGameComplete;
        break;
    }
}

static void EnsureMenuOptionsBufferIsValid()
{
    if(sQuestMenuData->menuOptionsBuffer == NULL)
    {
        u16 i;
        struct MenuOption const* baseMenu = Rogue_IsRunActive() ? sMenuOptionsAdventure : sMenuOptionsHub;
        u16 baseCount = Rogue_IsRunActive() ? ARRAY_COUNT(sMenuOptionsAdventure) : ARRAY_COUNT(sMenuOptionsHub);

        sQuestMenuData->menuOptionsBuffer = Alloc(sizeof(struct MenuOption) * baseCount);
        sQuestMenuData->menuOptionsBufferCount = 0;

        for(i = 0; i < baseCount; ++i)
        {
            if(IsIndexPageVisible(baseMenu[i].param))
            {
                memcpy(&sQuestMenuData->menuOptionsBuffer[sQuestMenuData->menuOptionsBufferCount++], &baseMenu[i], sizeof(struct MenuOption));
                OverrideIndexPageOption(&sQuestMenuData->menuOptionsBuffer[sQuestMenuData->menuOptionsBufferCount - 1]);
            }
        }
    }
}

static struct MenuOption const* GetIndexMenuOptionsPtr()
{
    EnsureMenuOptionsBufferIsValid();
    return sQuestMenuData->menuOptionsBuffer;
}

static u16 GetIndexMenuOptionsCount()
{
    EnsureMenuOptionsBufferIsValid();
    return sQuestMenuData->menuOptionsBufferCount;
}

static void Setup_IndexPage()
{
    sQuestMenuData->scrollListCount = GetIndexMenuOptionsCount();
}

static void HandleInput_IndexPage(u8 taskId)
{
    if(HandleScrollBehaviour())
        Draw_IndexPage();

    if (JOY_NEW(A_BUTTON))
    {
        struct MenuOption const* menuOptions = GetIndexMenuOptionsPtr();
        AGB_ASSERT(GetCurrentListIndex() < GetIndexMenuOptionsCount());

        menuOptions[GetCurrentListIndex()].callback(menuOptions[GetCurrentListIndex()].param);
    }

    if (JOY_NEW(B_BUTTON))
        SetupPage(PAGE_BOOK_FRONT);
}

static void BufferQuestPercValueFor(u8* str, u8 perc, u8 total)
{
    ConvertUIntToDecimalStringN(gStringVar1, perc, STR_CONV_MODE_LEFT_ALIGN, 3);
    if(perc >= total)
        StringExpandPlaceholders(str, sText_Index_FinishedPerc);
    else
        StringExpandPlaceholders(str, sText_Index_InProgressPerc);
}

static void BufferActiveCountFor(u8* str, u16 active, u16 inactive)
{
    ConvertUIntToDecimalStringN(gStringVar1, active, STR_CONV_MODE_LEFT_ALIGN, 3);
    ConvertUIntToDecimalStringN(gStringVar2, active + inactive, STR_CONV_MODE_LEFT_ALIGN, 3);
    if(active != 0)
        StringExpandPlaceholders(str, sText_Index_ActiveCount);
    else
        StringExpandPlaceholders(str, sText_Index_NoneActiveCount);
}

static void Draw_IndexPage()
{
    u8 x, y;
    u8 str[32];
    u8 const* textMainQuest = (RogueQuest_HasUnlockedChallenges() || RogueQuest_HasUnlockedMonMasteries()) ? sText_Index_Quest : sText_Index_Total;
    u8 const color[3] = {0, 2, 3};

    // Draw current quest info
    FillWindowPixelBuffer(WIN_LEFT_PAGE, PIXEL_FILL(0));

    y = 2;
    AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_NORMAL, 0, 1, 0, 0, color, TEXT_SKIP_DRAW, sText_Progress);

    // Main Quests
    {
        AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, textMainQuest);
        BufferQuestPercValueFor(str, RogueQuest_GetQuestCompletePercFor(QUEST_CONST_IS_MAIN_QUEST), 100);

        x = GetStringRightAlignXOffset(FONT_SMALL_NARROW, str, sQuestWinTemplates[WIN_LEFT_PAGE].width * 8);
        AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, x, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, str);
        ++y;
    }

    // Challenges
    if(RogueQuest_HasUnlockedChallenges())
    {
        AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, sText_Index_Challenge);
        BufferQuestPercValueFor(str, RogueQuest_GetQuestCompletePercFor(QUEST_CONST_IS_CHALLENGE), 100);

        x = GetStringRightAlignXOffset(FONT_SMALL_NARROW, str, sQuestWinTemplates[WIN_LEFT_PAGE].width * 8);
        AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, x, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, str);
        ++y;
    }

    // Mon Mastery
    if(RogueQuest_HasUnlockedMonMasteries())
    {
        AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, sText_Index_Mastery);
        BufferQuestPercValueFor(str, RogueQuest_GetQuestCompletePercFor(QUEST_CONST_IS_MON_MASTERY), 100);

        x = GetStringRightAlignXOffset(FONT_SMALL_NARROW, str, sQuestWinTemplates[WIN_LEFT_PAGE].width * 8);
        AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, x, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, str);
        ++y;
    }

    // Total
    if(RogueQuest_HasUnlockedChallenges() || RogueQuest_HasUnlockedMonMasteries())
    {
        ++y;
        AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, sText_Index_Total);
        BufferQuestPercValueFor(str, RogueQuest_GetDisplayCompletePerc(), 120);

        x = GetStringRightAlignXOffset(FONT_SMALL_NARROW, str, sQuestWinTemplates[WIN_LEFT_PAGE].width * 8);
        AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, x, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, str);
        ++y;
    }

    if(Rogue_IsRunActive())
    {
        u16 active, inactive;
        ++y;
        ++y;

        AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_NARROW, 0, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, sText_Index_ActiveQuests);
        ++y;
        ++y;
        
        // Main Quests
        {
            AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, textMainQuest);
            RogueQuest_GetQuestCountsFor(QUEST_CONST_IS_MAIN_QUEST, &active, &inactive);
            BufferActiveCountFor(str, active, inactive);

            x = GetStringRightAlignXOffset(FONT_SMALL_NARROW, str, sQuestWinTemplates[WIN_LEFT_PAGE].width * 8);
            AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, x, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, str);
            ++y;
        }
        
        // Challenges
        if(RogueQuest_HasUnlockedChallenges())
        {
            AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, sText_Index_Challenge);
            RogueQuest_GetQuestCountsFor(QUEST_CONST_IS_CHALLENGE, &active, &inactive);
            BufferActiveCountFor(str, active, inactive);

            x = GetStringRightAlignXOffset(FONT_SMALL_NARROW, str, sQuestWinTemplates[WIN_LEFT_PAGE].width * 8);
            AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, x, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, str);
            ++y;
        }
        
        // Mon Mastery
        if(RogueQuest_HasUnlockedMonMasteries())
        {
            AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, sText_Index_Mastery);
            RogueQuest_GetQuestCountsFor(QUEST_CONST_IS_MON_MASTERY, &active, &inactive);
            BufferActiveCountFor(str, active, inactive);

            x = GetStringRightAlignXOffset(FONT_SMALL_NARROW, str, sQuestWinTemplates[WIN_LEFT_PAGE].width * 8);
            AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, x, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, str);
            ++y;
        }
    }
    else
    {
        // Pending rewards to collect
        if(RogueQuest_HasAnyPendingRewards())
            AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 8 * 14, 0, 0, color, TEXT_SKIP_DRAW, sText_Index_PendingRewards);
    }

    PutWindowTilemap(WIN_LEFT_PAGE);
    CopyWindowToVram(WIN_LEFT_PAGE, COPYWIN_FULL);

    // Draw scroll list
    DrawGenericScrollList(GetIndexMenuOptionsPtr(), GetIndexMenuOptionsCount());
}

// Quest page
//

static void Setup_QuestPage()
{
    switch (sQuestMenuData->currentPage)
    {
    case PAGE_BOOK_ALL_PINNED:
        sQuestMenuData->questListStateIncludeFlags = QUEST_STATE_PINNED;
        break;


    case PAGE_BOOK_MAIN_TODO:
        sQuestMenuData->questListConstIncludeFlags = QUEST_CONST_IS_MAIN_QUEST;
        sQuestMenuData->questListStateExcludeFlags = QUEST_STATE_HAS_COMPLETE;
        break;

    case PAGE_BOOK_MAIN_ACTIVE:
        sQuestMenuData->questListConstIncludeFlags = QUEST_CONST_IS_MAIN_QUEST;
        sQuestMenuData->questListStateIncludeFlags = QUEST_STATE_ACTIVE;
        break;

    case PAGE_BOOK_MAIN_INACTIVE:
        sQuestMenuData->questListConstIncludeFlags = QUEST_CONST_IS_MAIN_QUEST;
        sQuestMenuData->questListStateExcludeFlags = QUEST_STATE_ACTIVE;
        break;

    case PAGE_BOOK_MAIN_COMPLETE:
        sQuestMenuData->questListConstIncludeFlags = QUEST_CONST_IS_MAIN_QUEST;
        sQuestMenuData->questListStateIncludeFlags = QUEST_STATE_HAS_COMPLETE;
        break;


    case PAGE_BOOK_CHALLENGE_TODO:
        sQuestMenuData->questListConstIncludeFlags = QUEST_CONST_IS_CHALLENGE;
        sQuestMenuData->questListStateExcludeFlags = QUEST_STATE_HAS_COMPLETE;
        break;

    case PAGE_BOOK_CHALLENGE_ACTIVE:
        sQuestMenuData->questListConstIncludeFlags = QUEST_CONST_IS_CHALLENGE;
        sQuestMenuData->questListStateIncludeFlags = QUEST_STATE_ACTIVE;
        break;

    case PAGE_BOOK_CHALLENGE_INACTIVE:
        sQuestMenuData->questListConstIncludeFlags = QUEST_CONST_IS_CHALLENGE;
        sQuestMenuData->questListStateExcludeFlags = QUEST_STATE_ACTIVE;
        break;

    case PAGE_BOOK_CHALLENGE_COMPLETE:
        sQuestMenuData->questListConstIncludeFlags = QUEST_CONST_IS_CHALLENGE;
        sQuestMenuData->questListStateIncludeFlags = QUEST_STATE_HAS_COMPLETE;
        break;


    case PAGE_BOOK_MON_MASTERY_TODO:
        sQuestMenuData->questListConstIncludeFlags = QUEST_CONST_IS_MON_MASTERY;
        sQuestMenuData->questListStateExcludeFlags = QUEST_STATE_HAS_COMPLETE;
        break;

    case PAGE_BOOK_MON_MASTERY_ACTIVE:
        sQuestMenuData->questListConstIncludeFlags = QUEST_CONST_IS_MON_MASTERY;
        sQuestMenuData->questListStateIncludeFlags = QUEST_STATE_ACTIVE;
        break;

    case PAGE_BOOK_MON_MASTERY_INACTIVE:
        sQuestMenuData->questListConstIncludeFlags = QUEST_CONST_IS_MON_MASTERY;
        sQuestMenuData->questListStateExcludeFlags = QUEST_STATE_ACTIVE;
        break;

    case PAGE_BOOK_MON_MASTERY_COMPLETE:
        sQuestMenuData->questListConstIncludeFlags = QUEST_CONST_IS_MON_MASTERY;
        sQuestMenuData->questListStateIncludeFlags = QUEST_STATE_HAS_COMPLETE;
        break;
    
    default:
        AGB_ASSERT(FALSE);
        break;
    }

    sQuestMenuData->scrollListCount = CalculateVisibleQuestCount() + 1;
}

static void Setup_QuestBoard()
{
    sQuestMenuData->questListStateIncludeFlags = QUEST_STATE_NEW_UNLOCK;
    sQuestMenuData->scrollListCount = CalculateVisibleQuestCount() + 1;
}

static void HandleInput_QuestPage(u8 taskId)
{
    if(HandleScrollBehaviour())
        Draw_QuestPage();

    if (JOY_NEW(A_BUTTON))
    {
        u16 questIndex = GetCurrentListQuestIndex();

        if(questIndex == QUEST_ID_COUNT)
        {
            // Exit
            if(sQuestMenuData->currentPage == PAGE_QUEST_BOARD)
            {
                RogueQuest_ClearNewUnlockQuests();
                StartFadeAndExit(taskId);
            }
            else
                SetupPage(PAGE_BOOK_INDEX);
            return;
        }
        else
        {
            // Toggle pinned
            u16 questId = RogueQuest_GetOrderedQuest(questIndex, sQuestMenuData->alphabeticalSort);
            RogueQuest_SetStateFlag(questId, QUEST_STATE_PINNED, !RogueQuest_GetStateFlag(questId, QUEST_STATE_PINNED));
            Draw_QuestPage();
            PlaySE(SE_CLICK);
        }
    }

    if (JOY_NEW(B_BUTTON))
    {
        if(sQuestMenuData->currentPage == PAGE_QUEST_BOARD)
        {
            RogueQuest_ClearNewUnlockQuests();
            StartFadeAndExit(taskId);
        }
        else
            SetupPage(PAGE_BOOK_INDEX);
    }

    if(JOY_NEW(SELECT_BUTTON))
    {
        sQuestMenuData->alphabeticalSort ^= 1;
        Draw_QuestPage();
        PlaySE(SE_SWITCH);
    }
}

#define TILE_BOOK_PIN_ACTIVE    0x5E
#define TILE_BOOK_PIN_NONE      0x18
#define TILE_BOARD_PIN_ACTIVE   0x60
#define TILE_BOARD_PIN_NONE     0x51

extern const u32 gItemIcon_RogueStatusStar[];
extern const u32 gItemIcon_RogueStatusCustom[];
extern const u32 gItemIcon_RogueStatusShop[];
extern const u32 gItemIcon_RogueStatusMoney[];
extern const u32 gItemIconPalette_RogueStatusStarCustom[];

static void Draw_QuestPage()
{
    u8 i;
    u8 const color[3] = {0, 2, 3};
    u16 questIndex = GetCurrentListQuestIndex();

    gTextFlags.replaceScrollWithNewLine = TRUE;

    // Draw current quest info
    FillWindowPixelBuffer(WIN_LEFT_PAGE, PIXEL_FILL(0));

    // Remove previous sprites if we have any
    FreeAllSpritePalettes();
    ResetSpriteData();

    if(questIndex != QUEST_ID_COUNT)
    {
        u16 questId = RogueQuest_GetOrderedQuest(questIndex, sQuestMenuData->alphabeticalSort);

        // Place desc/tracking text
        AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_NORMAL, 0, 1, 0, 0, color, TEXT_SKIP_DRAW, RogueQuest_GetTitle(questId));
        AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 16, 0, 0, color, TEXT_SKIP_DRAW, RogueQuest_GetDesc(questId));


        if(RogueQuest_GetStateFlag(questId, QUEST_STATE_ACTIVE))
            AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 16 + 8 * 9, 0, 0, color, TEXT_SKIP_DRAW, sText_MarkerInProgress);

        else if(RogueQuest_GetStateFlag(questId, QUEST_STATE_PENDING_REWARDS))
            AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 16 + 8 * 9, 0, 0, color, TEXT_SKIP_DRAW, sText_MarkerPendingRewards);

        else if(RogueQuest_GetStateFlag(questId, QUEST_STATE_HAS_COMPLETE))
        {
            if(RogueQuest_GetConstFlag(questId, QUEST_CONST_IS_CHALLENGE))
            {
                switch (RogueQuest_GetHighestCompleteDifficulty(questId))
                {
                case DIFFICULTY_LEVEL_EASY:
                    AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 16 + 8 * 9, 0, 0, color, TEXT_SKIP_DRAW, sText_MarkerCompleteEasy);
                    break;
                case DIFFICULTY_LEVEL_AVERAGE:
                    AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 16 + 8 * 9, 0, 0, color, TEXT_SKIP_DRAW, sText_MarkerCompleteAverage);
                    break;
                case DIFFICULTY_LEVEL_HARD:
                    AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 16 + 8 * 9, 0, 0, color, TEXT_SKIP_DRAW, sText_MarkerCompleteHard);
                    break;
                case DIFFICULTY_LEVEL_BRUTAL:
                    AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 16 + 8 * 9, 0, 0, color, TEXT_SKIP_DRAW, sText_MarkerCompleteBrutal);
                    break;
                default:
                    AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 16 + 8 * 9, 0, 0, color, TEXT_SKIP_DRAW, sText_MarkerComplete);
                    break;
                }
            }
            else
            {
                AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 16 + 8 * 9, 0, 0, color, TEXT_SKIP_DRAW, sText_MarkerComplete);
            }
        }
        else if(Rogue_IsRunActive())
            AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 16 + 8 * 9, 0, 0, color, TEXT_SKIP_DRAW, sText_MarkerInactive);

        AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 16 + 8 * 10, 0, 0, color, TEXT_SKIP_DRAW, sText_MarkerRewards);

        // Place sprites
        {
            u8 spriteIdx;
            u16 currentTag;
            struct RogueQuestReward const* reward;
            u16 const rewardCount = RogueQuest_GetRewardCount(questId);
            u8 groupedSpriteIndex[QUEST_SPRITE_CAPACITY];
            u8 spriteLayering[QUEST_SPRITE_CAPACITY];
            u8 currentSpriteGroup;
            bool8 hasDisplayedQuestUnlock = FALSE;

            spriteIdx = 0;

            // Setup reward sprites
            for(i = 0; i < rewardCount; ++i)
            {
                if(spriteIdx >= QUEST_SPRITE_CAPACITY)
                    break;

                reward = RogueQuest_GetReward(questId, i);

                // Don't have any visual indication for this
                if(reward->visiblity == QUEST_REWARD_VISIBLITY_INVISIBLE)
                    continue;

                // Reset to ignore invalid groups
                if(spriteIdx != 0)
                    currentSpriteGroup = groupedSpriteIndex[spriteIdx - 1] + 1;
                else
                    currentSpriteGroup = 0;

                if(reward->visiblity == QUEST_REWARD_VISIBLITY_OBSCURED)
                {
                    // Add a ? icon
                    LoadMonIconPalette(SPECIES_NONE);
                    sQuestMenuData->sprites[spriteIdx] = CreateMissingMonIcon(
                        SpriteCallbackDummy,
                        0, 0,
                        0,
                        0
                    );

                    gSprites[sQuestMenuData->sprites[spriteIdx]].x2 = -4;
                    gSprites[sQuestMenuData->sprites[spriteIdx]].y2 = -8;

                    groupedSpriteIndex[spriteIdx] = currentSpriteGroup;
                    spriteLayering[spriteIdx] = 0;
                    ++spriteIdx;
                }
                else if(reward->customPopup)
                {
                    // Display the custom popup item icon here for consistency
                    currentTag = TAG_REWARD_ICON_ITEM + reward->customPopup->itemIcon;
                    
                    sQuestMenuData->sprites[spriteIdx] = AddItemIconSprite(currentTag, currentTag, reward->customPopup->itemIcon);
                    groupedSpriteIndex[spriteIdx] = currentSpriteGroup;
                    spriteLayering[spriteIdx] = 0;
                    ++spriteIdx;
                }
                else
                {
                    switch (reward->type)
                    {
                    case QUEST_REWARD_ITEM:
                        currentTag = TAG_REWARD_ICON_ITEM + reward->perType.item.item;

                        sQuestMenuData->sprites[spriteIdx] = AddItemIconSprite(currentTag, currentTag, reward->perType.item.item);
                        groupedSpriteIndex[spriteIdx] = currentSpriteGroup;
                        spriteLayering[spriteIdx] = 0;
                        ++spriteIdx;

                        if(reward->perType.item.count >= QUEST_REWARD_MEDIUM_BUILD_AMOUNT)
                        {
                            sQuestMenuData->sprites[spriteIdx] = AddItemIconSprite(currentTag, currentTag, reward->perType.item.item);
                            gSprites[sQuestMenuData->sprites[spriteIdx]].x2 = 3;
                            gSprites[sQuestMenuData->sprites[spriteIdx]].y2 = 1;
                            groupedSpriteIndex[spriteIdx] = currentSpriteGroup;
                            spriteLayering[spriteIdx] = 1;
                            ++spriteIdx;
                        }
                        if(reward->perType.item.count >= QUEST_REWARD_LARGE_BUILD_AMOUNT)
                        {
                            sQuestMenuData->sprites[spriteIdx] = AddItemIconSprite(currentTag, currentTag, reward->perType.item.item);
                            gSprites[sQuestMenuData->sprites[spriteIdx]].x2 = 6;
                            gSprites[sQuestMenuData->sprites[spriteIdx]].y2 = 2;
                            groupedSpriteIndex[spriteIdx] = currentSpriteGroup;
                            spriteLayering[spriteIdx] = 1;
                            ++spriteIdx;
                        }
                        break;

                    case QUEST_REWARD_SHOP_ITEM:
                    
                        {
                            sQuestMenuData->sprites[spriteIdx] = AddIconSprite(TAG_REWARD_ICON_SHOP_ITEM, TAG_REWARD_ICON_SHOP_ITEM, gItemIcon_RogueStatusShop, gItemIconPalette_RogueStatusStarCustom);
                            groupedSpriteIndex[spriteIdx] = currentSpriteGroup;
                            spriteLayering[spriteIdx] = 0;
                            ++spriteIdx;
                        }

                        currentTag = TAG_REWARD_ICON_ITEM + reward->perType.shopItem.item;
                        
                        sQuestMenuData->sprites[spriteIdx] = AddItemIconSprite(currentTag, currentTag, reward->perType.shopItem.item);
                        groupedSpriteIndex[spriteIdx] = currentSpriteGroup;
                        spriteLayering[spriteIdx] = 0;
                        ++spriteIdx;
                        break;

                    case QUEST_REWARD_MONEY:
                        sQuestMenuData->sprites[spriteIdx] = AddIconSprite(TAG_REWARD_ICON_MONEY, TAG_REWARD_ICON_MONEY, gItemIcon_RogueStatusMoney, gItemIconPalette_RogueStatusStarCustom);
                        groupedSpriteIndex[spriteIdx] = currentSpriteGroup;
                        spriteLayering[spriteIdx] = 0;
                        ++spriteIdx;

                        if(reward->perType.money.amount >= QUEST_REWARD_MEDIUM_MONEY)
                        {
                            sQuestMenuData->sprites[spriteIdx] = AddIconSprite(TAG_REWARD_ICON_MONEY, TAG_REWARD_ICON_MONEY, gItemIcon_RogueStatusMoney, gItemIconPalette_RogueStatusStarCustom);
                            gSprites[sQuestMenuData->sprites[spriteIdx]].x2 = 3;
                            gSprites[sQuestMenuData->sprites[spriteIdx]].y2 = 1;
                            groupedSpriteIndex[spriteIdx] = currentSpriteGroup;
                            spriteLayering[spriteIdx] = 1;
                            ++spriteIdx;
                        }
                        if(reward->perType.money.amount >= QUEST_REWARD_LARGE_MONEY)
                        {
                            sQuestMenuData->sprites[spriteIdx] = AddIconSprite(TAG_REWARD_ICON_MONEY, TAG_REWARD_ICON_MONEY, gItemIcon_RogueStatusMoney, gItemIconPalette_RogueStatusStarCustom);
                            gSprites[sQuestMenuData->sprites[spriteIdx]].x2 = 6;
                            gSprites[sQuestMenuData->sprites[spriteIdx]].y2 = 2;
                            groupedSpriteIndex[spriteIdx] = currentSpriteGroup;
                            spriteLayering[spriteIdx] = 1;
                            ++spriteIdx;
                        }
                        break;

                    case QUEST_REWARD_QUEST_UNLOCK:
                        if(!hasDisplayedQuestUnlock)
                        {
                            currentTag = TAG_REWARD_ICON_ITEM + ITEM_QUEST_LOG;

                            sQuestMenuData->sprites[spriteIdx] = AddItemIconSprite(currentTag, currentTag, ITEM_QUEST_LOG);
                            groupedSpriteIndex[spriteIdx] = currentSpriteGroup;
                            spriteLayering[spriteIdx] = 0;
                            ++spriteIdx;

                            // Don't display multiple quest unlock entries (one is enough)
                            hasDisplayedQuestUnlock = TRUE;
                        }
                        break;

                    case QUEST_REWARD_POKEMON:

                        if(reward->perType.pokemon.isShiny)
                        {
                            sQuestMenuData->sprites[spriteIdx] = AddIconSprite(TAG_REWARD_ICON_POKEMON_SHINY, TAG_REWARD_ICON_POKEMON_SHINY, gItemIcon_RogueStatusStar, gItemIconPalette_RogueStatusStarCustom);
                            groupedSpriteIndex[spriteIdx] = currentSpriteGroup;
                            spriteLayering[spriteIdx] = 0;
                            ++spriteIdx;
                        }
                        if(reward->perType.pokemon.customMonId != CUSTOM_MON_NONE)
                        {
                            sQuestMenuData->sprites[spriteIdx] = AddIconSprite(TAG_REWARD_ICON_POKEMON_CUSTOM, TAG_REWARD_ICON_POKEMON_SHINY, gItemIcon_RogueStatusCustom, gItemIconPalette_RogueStatusStarCustom);
                            groupedSpriteIndex[spriteIdx] = currentSpriteGroup;
                            spriteLayering[spriteIdx] = 0;
                            ++spriteIdx;
                        }

                        LoadMonIconPalette(reward->perType.pokemon.species);
                        sQuestMenuData->sprites[spriteIdx] = CreateMonIcon(
                            reward->perType.pokemon.species,
                            SpriteCallbackDummy,
                            0, 0,
                            0,
                            0,
                            MON_MALE
                        );

                        gSprites[sQuestMenuData->sprites[spriteIdx]].x2 = -4;
                        gSprites[sQuestMenuData->sprites[spriteIdx]].y2 = -8;

                        groupedSpriteIndex[spriteIdx] = currentSpriteGroup;
                        spriteLayering[spriteIdx] = 0;
                        ++spriteIdx;
                        break;

                    case QUEST_REWARD_HUB_UPGRADE:
                        currentTag = TAG_REWARD_ICON_ITEM + ITEM_TOWN_MAP;

                        sQuestMenuData->sprites[spriteIdx] = AddItemIconSprite(currentTag, currentTag, ITEM_TOWN_MAP);
                        groupedSpriteIndex[spriteIdx] = currentSpriteGroup;
                        spriteLayering[spriteIdx] = 0;
                        ++spriteIdx;
                        break;
                        
                    case QUEST_REWARD_DECOR:
                    case QUEST_REWARD_DECOR_VARIANT:
                        currentTag = TAG_REWARD_ICON_ITEM + ITEM_BASEMENT_KEY;
                        
                        sQuestMenuData->sprites[spriteIdx] = AddItemIconSprite(currentTag, currentTag, ITEM_BASEMENT_KEY);
                        groupedSpriteIndex[spriteIdx] = currentSpriteGroup;
                        spriteLayering[spriteIdx] = 0;
                        ++spriteIdx;
                        break;
                    }
                }
            }

            if(spriteIdx != 0)
            {
                currentSpriteGroup = groupedSpriteIndex[spriteIdx - 1] + 1;

                for(i = 0; i < QUEST_SPRITE_CAPACITY; ++i)
                {
                    spriteIdx = sQuestMenuData->sprites[i];
                    if(spriteIdx != SPRITE_NONE)
                    {
                        u16 const boxWidth = 74;
                        u8 groupIdx = groupedSpriteIndex[i];

                        gSprites[spriteIdx].x = 24 + 4 + (groupIdx * boxWidth) / currentSpriteGroup + boxWidth / (2 * currentSpriteGroup);
                        gSprites[spriteIdx].y = 8 * 17;
                        gSprites[spriteIdx].subpriority = spriteLayering[i] * QUEST_SPRITE_CAPACITY + i;
                    }
                }
            }
        }
    }

    PutWindowTilemap(WIN_LEFT_PAGE);
    CopyWindowToVram(WIN_LEFT_PAGE, COPYWIN_FULL);

    // TODO - setup sprites

    // Draw scroll list
    DrawQuestScrollList();

    // Draw pins next to quests that have been pinned
    {
        u16 i, tileNum;

        questIndex = GetFirstVisibleQuestIndex();

        for(i = 0; i < sQuestMenuData->scrollListHead; ++i)
            IterateNextVisibleQuestIndex(&questIndex);

        for(i = 0 ; i < SCROLL_ITEMS_IN_VIEW; ++i)
        {
            if(questIndex != QUEST_ID_COUNT && RogueQuest_GetStateFlag(RogueQuest_GetOrderedQuest(questIndex, sQuestMenuData->alphabeticalSort), QUEST_STATE_PINNED))
            {
                tileNum = sQuestMenuData->currentPage == PAGE_QUEST_BOARD ? TILE_BOARD_PIN_ACTIVE : TILE_BOOK_PIN_ACTIVE;
            }
            else
            {
                tileNum = sQuestMenuData->currentPage == PAGE_QUEST_BOARD ? TILE_BOARD_PIN_NONE : TILE_BOOK_PIN_NONE;
            }

            FillBgTilemapBufferRect_Palette0(
                1, 
                tileNum, 
                16, 2 + 2 * i,
                1, 1
            );

            IterateNextVisibleQuestIndex(&questIndex);
        }
    }

    gTextFlags.replaceScrollWithNewLine = FALSE;

    ScheduleBgCopyTilemapToVram(1);
}
