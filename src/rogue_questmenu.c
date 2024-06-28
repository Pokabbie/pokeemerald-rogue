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
#include "rogue_pokedex.h"
#include "rogue_quest.h"
#include "rogue_questmenu.h"

#define SCROLL_ITEMS_IN_VIEW 8
#define QUEST_SPRITE_CAPACITY 32

enum {
    TAG_REWARD_ICON_POKEMON_SHINY = 100,
    TAG_REWARD_ICON_POKEMON_CUSTOM,
    TAG_REWARD_ICON_SHOP_ITEM,
    TAG_REWARD_ICON_DIFFICULTY_HARD,
    TAG_REWARD_ICON_DIFFICULTY_BRUTAL,
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
static void Setup_MasteryLandingPage();
static void Setup_MasteryTrackerPage();
static void Setup_PlayerStatsPage();

static void HandleInput_FrontPage(u8 taskId);
static void HandleInput_IndexPage(u8 taskId);
static void HandleInput_QuestPage(u8 taskId);
static void HandleInput_MasteryLandingPage(u8 taskId);
static void HandleInput_MasteryTrackerPage(u8 taskId);
static void HandleInput_PlayerStatsPage(u8 taskId);

static void Draw_FrontPage();
static void Draw_IndexPage();
static void Draw_QuestPage();
static void Draw_MasteryLandingPage();
static void Draw_MasteryTrackerPage();
static void Draw_PlayerStatsPage();

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

    PAGE_BOOK_MON_MASTERY_LANDING,
    PAGE_BOOK_MON_MASTERY_TRACKER,
    PAGE_BOOK_MON_MASTERY_TODO,
    PAGE_BOOK_MON_MASTERY_ACTIVE,
    PAGE_BOOK_MON_MASTERY_INACTIVE,
    PAGE_BOOK_MON_MASTERY_COMPLETE,

    PAGE_BOOK_PLAYER_STATS,

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
    u8 exitOnMonMasteryLanding : 1;
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
static u32 const sLinedTilemap[] = INCBIN_U32("graphics/rogue_quest/lined_page.bin.lz");
static u32 const sQuestboardTilemap[] = INCBIN_U32("graphics/rogue_quest/quest_board.bin.lz");

static u32 const sQuestTiles[] = INCBIN_U32("graphics/rogue_quest/tiles.4bpp.lz");
static u16 const sQuestPalette_Blue[] = INCBIN_U16("graphics/rogue_quest/pal_blue.gbapal");
static u16 const sQuestPalette_Green[] = INCBIN_U16("graphics/rogue_quest/pal_green.gbapal");
static u16 const sQuestPalette_Gold[] = INCBIN_U16("graphics/rogue_quest/pal_gold.gbapal");

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

    [PAGE_BOOK_MON_MASTERY_LANDING] = 
    {
        .tilemap = sIndexTilemap,
        .setupCallback = Setup_MasteryLandingPage,
        .inputCallback = HandleInput_MasteryLandingPage,
        .drawCallback = Draw_MasteryLandingPage,
    },
    [PAGE_BOOK_MON_MASTERY_TRACKER] = 
    {
        .tilemap = sLinedTilemap,
        .setupCallback = Setup_MasteryTrackerPage,
        .inputCallback = HandleInput_MasteryTrackerPage,
        .drawCallback = Draw_MasteryTrackerPage,
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

    [PAGE_BOOK_PLAYER_STATS] = 
    {
        .tilemap = sLinedTilemap,
        .setupCallback = Setup_PlayerStatsPage,
        .inputCallback = HandleInput_PlayerStatsPage,
        .drawCallback = Draw_PlayerStatsPage,
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

static u8 const sText_MonMastery[] = _("{PKMN} Mastery");
static u8 const sText_MonMasteryTracker[] = _("{PKMN} Tracker");
static u8 const sText_MonMasteryTodo[] = _("Quests·{FONT_SMALL_NARROW}{COLOR BLUE}To-Do");
static u8 const sText_MonMasteryComplete[] = _("Quests·{FONT_SMALL_NARROW}{COLOR GREEN}Done");
static u8 const sText_MonMasteryActive[] = _("Mastery·{FONT_SMALL_NARROW}{COLOR BLUE}Active");
static u8 const sText_MonMasteryInactive[] = _("Mastery·{FONT_SMALL_NARROW}{COLOR RED}Inactive");

static u8 const sText_Stats[] = _("{FONT_SMALL_NARROW}Adventure Stats");

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
static u8 const sText_MarkerCompleteEasy[] = _("{COLOR GREEN}·Complete {COLOR GREEN}{SHADOW LIGHT_GREEN}Easy{COLOR GREEN}{SHADOW LIGHT_GRAY}·");
static u8 const sText_MarkerCompleteAverage[] = _("{COLOR GREEN}·Complete {COLOR GREEN}{SHADOW LIGHT_GRAY}Average{COLOR GREEN}{SHADOW LIGHT_GRAY}·");
static u8 const sText_MarkerCompleteHard[] = _("{COLOR GREEN}·Complete {COLOR RED}{SHADOW LIGHT_GRAY}Hard{COLOR GREEN}{SHADOW LIGHT_GRAY}·");
static u8 const sText_MarkerCompleteBrutal[] = _("{COLOR GREEN}·Complete {COLOR RED}{SHADOW LIGHT_RED}Brutal{COLOR GREEN}{SHADOW LIGHT_GRAY}·");
static u8 const sText_MarkerRewards[] = _("{COLOR DARK_GRAY}Rewards");

static u8 const sText_PkmnMastery[] = _("{PKMN} Mastery");

// Index Page
static u8 const sText_Index_InProgressPerc[] = _("{COLOR BLUE}{STR_VAR_1}%");
static u8 const sText_Index_FinishedPerc[] = _("{COLOR GREEN}{STR_VAR_1}%");
static u8 const sText_Index_ActiveCount[] = _("{COLOR BLUE}{STR_VAR_1} / {STR_VAR_2}");
static u8 const sText_Index_NoneActiveCount[] = _("{COLOR RED}{STR_VAR_1} / {STR_VAR_2}");

static u8 const sText_Index_Main[] = _("Main");
static u8 const sText_Index_Challenge[] = _("Challenge");
static u8 const sText_Index_Mastery[] = _("Mastery");
static u8 const sText_Index_Total[] = _("Total");
static u8 const sText_Index_ActiveQuests[] = _("Active Quests");
static u8 const sText_Index_ChallengeDifficulty[] = _("Challenges");
static u8 const sText_Index_Easy[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Easy");
static u8 const sText_Index_Average[] = _("{COLOR GREEN}{SHADOW LIGHT_GRAY}Average");
static u8 const sText_Index_Hard[] = _("{COLOR RED}{SHADOW LIGHT_GRAY}Hard");
static u8 const sText_Index_Brutal[] = _("{COLOR RED}{SHADOW LIGHT_RED}Brutal");
static u8 const sText_EasyStar[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}");
static u8 const sText_AverageStar[] = _("{COLOR GREEN}{SHADOW LIGHT_GRAY}");
static u8 const sText_HardStar[] = _("{COLOR RED}{SHADOW LIGHT_GRAY}");
static u8 const sText_BrutalStar[] = _("{COLOR RED}{SHADOW LIGHT_RED}");
static u8 const sText_Index_Quests[] = _("Quests");

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

void Rogue_OpenMonMasteryMenu(RogueQuestMenuCallback callback)
{
    OpenQuestMenu(callback, PAGE_BOOK_MON_MASTERY_LANDING);
    sQuestMenuData->exitOnMonMasteryLanding = TRUE;
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

    if(Rogue_Use200PercEffects())
        LoadPalette(sQuestPalette_Gold, 0, 1 * 32);
    else if(Rogue_Use100PercEffects())
        LoadPalette(sQuestPalette_Green, 0, 1 * 32);
    else
        LoadPalette(sQuestPalette_Blue, 0, 1 * 32);

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

    if(page == PAGE_BOOK_INDEX)
    {
        // Redirect these pages
        switch (sQuestMenuData->currentPage)
        {
        case PAGE_BOOK_MON_MASTERY_TODO:
        case PAGE_BOOK_MON_MASTERY_COMPLETE:
            page = PAGE_BOOK_MON_MASTERY_LANDING;
            break;
        }
    }
    
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
            u8 highestComplete = RogueQuest_GetHighestCompleteDifficulty(questId);
            u8* strPtr = gStringVar4;

            *strPtr = 0xFF;

            if(RogueQuest_GetStateFlag(questId, QUEST_STATE_HAS_COMPLETE))
            {
                if(RogueQuest_GetConstFlag(questId, QUEST_CONST_IS_CHALLENGE))
                {
                    switch (RogueQuest_GetHighestCompleteDifficulty(questId))
                    {
                    case DIFFICULTY_LEVEL_EASY:
                        strPtr = StringAppend(strPtr, sText_EasyStar);
                        break;
                    case DIFFICULTY_LEVEL_AVERAGE:
                        strPtr = StringAppend(strPtr, sText_AverageStar);
                        break;
                    case DIFFICULTY_LEVEL_HARD:
                        strPtr = StringAppend(strPtr, sText_HardStar);
                        break;
                    case DIFFICULTY_LEVEL_BRUTAL:
                        strPtr = StringAppend(strPtr, sText_BrutalStar);
                        break;
                    }
                }
                else
                {
                    strPtr = StringAppend(strPtr, sText_AverageStar);
                }
            }

            StringAppend(strPtr, RogueQuest_GetTitle(questId));

            AddTextPrinterParameterized4(WIN_RIGHT_PAGE, FONT_NARROW, 8, 4 + 16 * i, 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);
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
    u8* txtPtr;
    u8 const color[3] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY};
    u16 trainerId = (gSaveBlock2Ptr->playerTrainerId[1] << 8) | gSaveBlock2Ptr->playerTrainerId[0];

    FillWindowPixelBuffer(WIN_RIGHT_PAGE, PIXEL_FILL(0));

    // Trainer name
    AddTextPrinterParameterized4(WIN_RIGHT_PAGE, FONT_SMALL_NARROW, 4, 111 + 9, 0, 0, color, TEXT_SKIP_DRAW, gSaveBlock2Ptr->playerName);

    // Trainer number
    ConvertIntToDecimalStringN(gStringVar4, trainerId, STR_CONV_MODE_LEADING_ZEROS, 5);
    AddTextPrinterParameterized4(WIN_RIGHT_PAGE, FONT_SMALL_NARROW, 4, 111 + 18, 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);


    PutWindowTilemap(WIN_RIGHT_PAGE);
    CopyWindowToVram(WIN_RIGHT_PAGE, COPYWIN_FULL);
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
        .text = sText_MonMastery,
        .callback = SetupPage,
        .param = PAGE_BOOK_MON_MASTERY_LANDING,
    },

    {
        .text = sText_Stats,
        .callback = SetupPage,
        .param = PAGE_BOOK_PLAYER_STATS,
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
        .text = sText_MonMastery,
        .callback = SetupPage,
        .param = PAGE_BOOK_MON_MASTERY_LANDING,
    },

    {
        .text = sText_Stats,
        .callback = SetupPage,
        .param = PAGE_BOOK_PLAYER_STATS,
    },

    //{
    //    .text = sText_MonMasteryActive,
    //    .callback = SetupPage,
    //    .param = PAGE_BOOK_MON_MASTERY_ACTIVE,
    //},
    //{
    //    .text = sText_MonMasteryInactive,
    //    .callback = SetupPage,
    //    .param = PAGE_BOOK_MON_MASTERY_INACTIVE,
    //},

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
 
    case PAGE_BOOK_MON_MASTERY_LANDING:
    case PAGE_BOOK_MON_MASTERY_TRACKER:
    case PAGE_BOOK_MON_MASTERY_TODO:
    case PAGE_BOOK_MON_MASTERY_ACTIVE:
    case PAGE_BOOK_MON_MASTERY_INACTIVE:
    case PAGE_BOOK_MON_MASTERY_COMPLETE:
        return RogueQuest_HasUnlockedMonMasteries();

    case PAGE_BOOK_PLAYER_STATS:
        return RogueQuest_HasCollectedRewards(QUEST_ID_TO_ADVENTUREEMARK);
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
    u8 const* textMainQuest = (RogueQuest_HasUnlockedChallenges() || RogueQuest_HasUnlockedMonMasteries()) ? sText_Index_Main : sText_Index_Total;
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
        if(RogueQuest_HasUnlockedMonMasteries())
        {
            ++y;
            AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, sText_Index_ChallengeDifficulty);
            ++y;

            // Easy
            AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, sText_Index_Easy);
            BufferQuestPercValueFor(str, RogueQuest_GetQuestCompletePercAtDifficultyFor(QUEST_CONST_IS_CHALLENGE, DIFFICULTY_LEVEL_EASY), 100);

            x = GetStringRightAlignXOffset(FONT_SMALL_NARROW, str, sQuestWinTemplates[WIN_LEFT_PAGE].width * 8);
            AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, x, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, str);
            ++y;

            // Average
            AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, sText_Index_Average);
            BufferQuestPercValueFor(str, RogueQuest_GetQuestCompletePercAtDifficultyFor(QUEST_CONST_IS_CHALLENGE, DIFFICULTY_LEVEL_AVERAGE), 100);

            x = GetStringRightAlignXOffset(FONT_SMALL_NARROW, str, sQuestWinTemplates[WIN_LEFT_PAGE].width * 8);
            AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, x, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, str);
            ++y;

            // Hard
            AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, sText_Index_Hard);
            BufferQuestPercValueFor(str, RogueQuest_GetQuestCompletePercAtDifficultyFor(QUEST_CONST_IS_CHALLENGE, DIFFICULTY_LEVEL_HARD), 100);

            x = GetStringRightAlignXOffset(FONT_SMALL_NARROW, str, sQuestWinTemplates[WIN_LEFT_PAGE].width * 8);
            AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, x, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, str);
            ++y;

            // Brutal
            AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, sText_Index_Brutal);
            BufferQuestPercValueFor(str, RogueQuest_GetQuestCompletePercAtDifficultyFor(QUEST_CONST_IS_CHALLENGE, DIFFICULTY_LEVEL_BRUTAL), 100);

            x = GetStringRightAlignXOffset(FONT_SMALL_NARROW, str, sQuestWinTemplates[WIN_LEFT_PAGE].width * 8);
            AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, x, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, str);
            ++y;
        }

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
extern const u32 gItemIcon_RogueHardLock[];
extern const u32 gItemIcon_RogueBrutalLock[];
extern const u32 gItemIconPalette_RogueStatusLock[];

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

                // Attach difficulty tag above main sprite
                {
                    if(reward->requiredDifficulty >= DIFFICULTY_LEVEL_BRUTAL)
                    {
                        sQuestMenuData->sprites[spriteIdx] = AddIconSprite(TAG_REWARD_ICON_DIFFICULTY_BRUTAL, TAG_REWARD_ICON_DIFFICULTY_HARD, gItemIcon_RogueBrutalLock, gItemIconPalette_RogueStatusLock);
                        groupedSpriteIndex[spriteIdx] = currentSpriteGroup;
                        spriteLayering[spriteIdx] = 0;
                        ++spriteIdx;
                    }
                    else if(reward->requiredDifficulty >= DIFFICULTY_LEVEL_HARD)
                    {
                        sQuestMenuData->sprites[spriteIdx] = AddIconSprite(TAG_REWARD_ICON_DIFFICULTY_HARD, TAG_REWARD_ICON_DIFFICULTY_HARD, gItemIcon_RogueHardLock, gItemIconPalette_RogueStatusLock);
                        groupedSpriteIndex[spriteIdx] = currentSpriteGroup;
                        spriteLayering[spriteIdx] = 0;
                        ++spriteIdx;
                    }
                }

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
                    // Display the custom popup icon here for consistency
                    if(reward->customPopup->speciesIcon != SPECIES_NONE)
                    {
                        LoadMonIconPalette(reward->customPopup->speciesIcon);
                        sQuestMenuData->sprites[spriteIdx] = CreateMonIcon(
                            reward->customPopup->speciesIcon,
                            SpriteCallbackDummy,
                            0, 0,
                            0,
                            0,
                            MON_MALE
                        );

                        gSprites[sQuestMenuData->sprites[spriteIdx]].x2 = -4;
                        gSprites[sQuestMenuData->sprites[spriteIdx]].y2 = -8;
                    }
                    else
                    {
                        currentTag = TAG_REWARD_ICON_ITEM + reward->customPopup->itemIcon;
                        sQuestMenuData->sprites[spriteIdx] = AddItemIconSprite(currentTag, currentTag, reward->customPopup->itemIcon);
                    }

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

// Mastery Custom Pages
//

static struct MenuOption const sMasteryLandingOptions[] = 
{
    {
        .text = sText_MonMasteryTracker,
        .callback = SetupPage,
        .param = PAGE_BOOK_MON_MASTERY_TRACKER,
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
        .param = PAGE_BOOK_INDEX,
    },
};


static void Setup_MasteryLandingPage()
{
    sQuestMenuData->scrollListCount = ARRAY_COUNT(sMasteryLandingOptions);
}

static void HandleInput_MasteryLandingPage(u8 taskId)
{
    if(HandleScrollBehaviour())
        Draw_MasteryLandingPage();

    if (JOY_NEW(A_BUTTON))
    {
        AGB_ASSERT(GetCurrentListIndex() < ARRAY_COUNT(sMasteryLandingOptions));

        sMasteryLandingOptions[GetCurrentListIndex()].callback(sMasteryLandingOptions[GetCurrentListIndex()].param);
    }

    if (JOY_NEW(B_BUTTON))
    {
        if(sQuestMenuData->exitOnMonMasteryLanding)
        {
            RogueQuest_ClearNewUnlockQuests();
            StartFadeAndExit(taskId);
        }
        else
        {
            SetupPage(PAGE_BOOK_INDEX);
        }
    }
}


static u8 const sText_Mastery_LandingDesc[] = _("Enter the Hall of\nFame with any {PKMN}\nfrom it's Evolution\nline to Complete\nit's Mastery!\n\nSpecific {PKMN} have\nRewards/Quests\nassociated with\nthem.");

static void Draw_MasteryLandingPage()
{
    u8 x, y;
    u8 str[32];
    u8 const color[3] = {0, 2, 3};

    FillWindowPixelBuffer(WIN_LEFT_PAGE, PIXEL_FILL(0));

    y = 2;
    AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_NORMAL, 0, 1, 0, 0, color, TEXT_SKIP_DRAW, sText_PkmnMastery);

    // Total
    {
        AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, sText_Index_Total);
        BufferQuestPercValueFor(str, RogueQuest_GetMonMasteryTotalPerc(), 100);

        x = GetStringRightAlignXOffset(FONT_SMALL_NARROW, str, sQuestWinTemplates[WIN_LEFT_PAGE].width * 8);
        AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, x, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, str);
        ++y;
    }

    // Quests
    {
        AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, sText_Index_Quests);
        BufferQuestPercValueFor(str, RogueQuest_GetQuestCompletePercFor(QUEST_CONST_IS_MON_MASTERY), 100);

        x = GetStringRightAlignXOffset(FONT_SMALL_NARROW, str, sQuestWinTemplates[WIN_LEFT_PAGE].width * 8);
        AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, x, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, str);
        ++y;
    }

    // Print Description
    ++y;
    AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 5 + 8 * y, 0, 0, color, TEXT_SKIP_DRAW, sText_Mastery_LandingDesc);
    ++y;

    PutWindowTilemap(WIN_LEFT_PAGE);
    CopyWindowToVram(WIN_LEFT_PAGE, COPYWIN_FULL);

    // Draw scroll list
    DrawGenericScrollList(sMasteryLandingOptions, ARRAY_COUNT(sMasteryLandingOptions));
}

#define TRACKER_ENTRY_WIDTH         2
#define TRACKER_ENTRY_HEIGHT        8
#define TRACKER_ENTRY_PER_PAGE      (TRACKER_ENTRY_WIDTH * TRACKER_ENTRY_HEIGHT)

static void Setup_MasteryTrackerPage()
{
    sQuestMenuData->scrollListOffset = 0;
}

static void HandleInput_MasteryTrackerPage(u8 taskId)
{
    //if(HandleScrollBehaviour())
    //    Draw_MasteryLandingPage();

    if(JOY_REPEAT(DPAD_LEFT))
    {
        if(sQuestMenuData->scrollListOffset == 0)
            sQuestMenuData->scrollListOffset = SPECIES_EGG_EVO_STAGE_COUNT / TRACKER_ENTRY_PER_PAGE;
        else
            --sQuestMenuData->scrollListOffset;

        PlaySE(SE_DEX_SCROLL);
        Draw_MasteryTrackerPage();
    }

    if(JOY_REPEAT(DPAD_RIGHT))
    {
        sQuestMenuData->scrollListOffset++;

        if(sQuestMenuData->scrollListOffset * TRACKER_ENTRY_PER_PAGE >= SPECIES_EGG_EVO_STAGE_COUNT)
            sQuestMenuData->scrollListOffset = 0;

        PlaySE(SE_DEX_SCROLL);
        Draw_MasteryTrackerPage();
    }

    //if (JOY_NEW(A_BUTTON))
    //{
    //    AGB_ASSERT(GetCurrentListIndex() < ARRAY_COUNT(sMasteryLandingOptions));
//
    //    sMasteryLandingOptions[GetCurrentListIndex()].callback(sMasteryLandingOptions[GetCurrentListIndex()].param);
    //}

    if (JOY_NEW(B_BUTTON))
        SetupPage(PAGE_BOOK_MON_MASTERY_LANDING);
}

extern const u16 gRogueBake_EggSpecies[];

static u8 const sText_PageMarker[] = _("{COLOR LIGHT_GRAY}{SHADOW DARK_GRAY}");
static u8 const sText_ChangePageLeft[] = _("{COLOR LIGHT_GRAY}{SHADOW DARK_GRAY}{DPAD_LEFT} back");
static u8 const sText_ChangePageRight[] = _("{COLOR LIGHT_GRAY}{SHADOW DARK_GRAY}next {DPAD_RIGHT}");

static void Draw_MasteryTrackerPage()
{
    u8 x, y;
    u32 i, counter, species;
    u8 const color[3] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY};
    u8 const completedColor[3] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_GREEN, TEXT_COLOR_LIGHT_GRAY};
    //u8 const todoColor[3] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_RED, TEXT_COLOR_LIGHT_GRAY};

    FillWindowPixelBuffer(WIN_LEFT_PAGE, PIXEL_FILL(0));
    FillWindowPixelBuffer(WIN_RIGHT_PAGE, PIXEL_FILL(0));

    // Remove previous sprites if we have any
    FreeAllSpritePalettes();
    ResetSpriteData();

    LoadMonIconPalettes();

    counter = 0;
    
    for(x = 0; x < TRACKER_ENTRY_WIDTH; ++x)
    {
        for(y = 0; y < TRACKER_ENTRY_HEIGHT; ++y)
        {
            i = sQuestMenuData->scrollListOffset * TRACKER_ENTRY_PER_PAGE + counter++;

            if(i >= SPECIES_EGG_EVO_STAGE_COUNT)
                break;

            species = gRogueBake_EggSpecies[i];

            if(RogueQuest_GetMonMasteryFlag(species))
            {
                AddTextPrinterParameterized4(
                    x == 0 ? WIN_LEFT_PAGE : WIN_RIGHT_PAGE,
                    FONT_NORMAL,
                    24,
                    4 + 16 * y,
                    0, 0,
                    completedColor,
                    TEXT_SKIP_DRAW,
                    RoguePokedex_GetSpeciesName(species)
                );

                CreateMonIcon(
                    species,
                    SpriteCallbackDummy,
                    25 + 120 * x,
                    14 + 16 * y,
                    0, 0, 
                    MON_MALE
                );
            }
            else
            {
                AddTextPrinterParameterized4(
                    x == 0 ? WIN_LEFT_PAGE : WIN_RIGHT_PAGE,
                    FONT_NORMAL,
                    24,
                    4 + 16 * y,
                    0, 0,
                    color,
                    TEXT_SKIP_DRAW,
                    gText_ThreeQuestionMarks
                );

                //CreateMissingMonIcon(
                //    SpriteCallbackDummy,
                //    25 + 120 * x,
                //    14 + 16 * y,
                //    0,
                //    0
                //);
            }
        }
    }

    // Page numbers
    {
        u8* str;
        
        str = StringCopy(gStringVar4, sText_PageMarker);
        ConvertIntToDecimalStringN(str, sQuestMenuData->scrollListOffset * 2 + 1, STR_CONV_MODE_RIGHT_ALIGN, 3);
        AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 60, 131, 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);
        
        str = StringCopy(gStringVar4, sText_PageMarker);
        ConvertIntToDecimalStringN(str, sQuestMenuData->scrollListOffset * 2 + 2, STR_CONV_MODE_RIGHT_ALIGN, 3);
        AddTextPrinterParameterized4(WIN_RIGHT_PAGE, FONT_SMALL_NARROW, 4, 131, 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);
    }

    AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 131, 0, 0, color, TEXT_SKIP_DRAW, sText_ChangePageLeft);
    AddTextPrinterParameterized4(WIN_RIGHT_PAGE, FONT_SMALL_NARROW, 57, 131, 0, 0, color, TEXT_SKIP_DRAW, sText_ChangePageRight);

    PutWindowTilemap(WIN_LEFT_PAGE);
    PutWindowTilemap(WIN_RIGHT_PAGE);
    CopyWindowToVram(WIN_LEFT_PAGE, COPYWIN_FULL);
    CopyWindowToVram(WIN_RIGHT_PAGE, COPYWIN_FULL);
}

#undef TRACKER_ENTRY_WIDTH
#undef TRACKER_ENTRY_HEIGHT
#undef TRACKER_ENTRY_PER_PAGE

// Player Stats
//

struct DisplayStat
{
    u8 const* name;
    u8 statId;
};

static u8 const sStatName_TotalRuns[] = _("Total Adventures");
static u8 const sStatName_TotalWins[] = _("Wins");
static u8 const sStatName_TotalCurrentWinStreak[] = _("Current Win Streak");
static u8 const sStatName_TotalLongestWinStreak[] = _("Longest Win Streak");
static u8 const sStatName_TotalLosses[] = _("Wipes");
static u8 const sStatName_TotalCurrentLossStreak[] = _("Current Wipe Streak");
static u8 const sStatName_TotalLongestLossStreak[] = _("Longest Wipe Streak");

static u8 const sStatName_FirstHoF[] = _("First HoF Time");
static u8 const sStatName_FastestHoF[] = _("Fastest HoF Time");
static u8 const sStatName_SlowestHoF[] = _("Slowest HoF Time");

static u8 const sStatName_TotalBadges[] = _("Total Badges");
static u8 const sStatName_GymBadges[] = _("Gym Badges");
static u8 const sStatName_EliteBadges[] = _("Elite Badges");
static u8 const sStatName_ChampionBadges[] = _("Champion Badges");

static u8 const sStatName_TotalBattles[] = _("Total Battles");
static u8 const sStatName_WildBattles[] = _("Wild Battles");
static u8 const sStatName_TrainerBattles[] = _("Trainer Battles");
static u8 const sStatName_RivalBattles[] = _("Rival Battles");

static u8 const sStatName_PokemonCaught[] = _("Pokémon Caught");
static u8 const sStatName_ShinyCaught[] = _("Shinies Caught");
static u8 const sStatName_LegendsCaught[] = _("Legends Caught");
static u8 const sStatName_RoamersCaught[] = _("Roamers Caught");

static u8 const sStatName_RandoTradeTotal[] = _("Randoman {PKMN}");
static u8 const sStatName_RandoTradeParty[] = _("Party Trades");
static u8 const sStatName_RandoTradeSingle[] = _("Single Trades");

static u8 const sStatName_ReleasedPokemon[] = _("Pokémon Released");
static u8 const sStatName_FaintedPokemon[] = _("Pokémon Fainted");
static u8 const sStatName_EvolvedPokemon[] = _("Pokémon Evolved");

static u8 const sStatFormat_HofTime[] = _("{STR_VAR_1}:{STR_VAR_2}:{STR_VAR_3}");


static struct DisplayStat const sDisplayStats[] =
{
    { sStatName_TotalRuns, GAME_STAT_TOTAL_RUNS },
    { sStatName_TotalWins, GAME_STAT_RUN_WINS },
    { sStatName_TotalCurrentWinStreak, GAME_STAT_CURRENT_RUN_WIN_STREAK },
    { sStatName_TotalLongestWinStreak, GAME_STAT_LONGEST_RUN_WIN_STREAK },

    {},
    { sStatName_TotalLosses, GAME_STAT_RUN_LOSSES },
    { sStatName_TotalCurrentLossStreak, GAME_STAT_CURRENT_RUN_LOSS_STREAK },
    { sStatName_TotalLongestLossStreak, GAME_STAT_LONGEST_RUN_LOSS_STREAK },


    { sStatName_TotalBadges, GAME_STAT_TOTAL_BADGES },
    { sStatName_GymBadges, GAME_STAT_GYM_BADGES },
    { sStatName_EliteBadges, GAME_STAT_ELITE_BADGES },
    { sStatName_ChampionBadges, GAME_STAT_CHAMPION_BADGES },

    { sStatName_FirstHoF, GAME_STAT_FIRST_HOF_PLAY_TIME },
    { sStatName_FastestHoF, GAME_STAT_FASTEST_HOF_PLAY_TIME },
    { sStatName_SlowestHoF, GAME_STAT_SLOWEST_HOF_PLAY_TIME },
    {},


    { sStatName_TotalBattles, GAME_STAT_TOTAL_BATTLES },
    { sStatName_WildBattles, GAME_STAT_WILD_BATTLES },
    { sStatName_TrainerBattles, GAME_STAT_TRAINER_BATTLES },
    { sStatName_RivalBattles, GAME_STAT_RIVAL_BATTLES },

    { sStatName_PokemonCaught, GAME_STAT_POKEMON_CAUGHT },
    { sStatName_ShinyCaught, GAME_STAT_SHINIES_CAUGHT },
    { sStatName_LegendsCaught, GAME_STAT_LEGENDS_CAUGHT },
    { sStatName_RoamersCaught, GAME_STAT_ROAMERS_CAUGHT },


    { sStatName_ReleasedPokemon, GAME_STAT_POKEMON_RELEASED },
    { sStatName_FaintedPokemon, GAME_STAT_POKEMON_FAINTED },
    { sStatName_EvolvedPokemon, GAME_STAT_EVOLVED_POKEMON },
    {},

    { sStatName_RandoTradeTotal, GAME_STAT_RANDO_TRADE_TOTAL_PKMN },
    { sStatName_RandoTradeParty, GAME_STAT_RANDO_TRADE_PARTY },
    { sStatName_RandoTradeSingle, GAME_STAT_RANDO_TRADE_SINGLE },

    // todo - Maybe fix these up and add them?
    //{ NULL, GAME_STAT_ITEMS_FOUND },
    //{ NULL, GAME_STAT_ITEMS_BOUGHT },
    //{ NULL, GAME_STAT_MOVES_BOUGHT },
    //{ NULL, GAME_STAT_MONEY_SPENT },
};

#define STATS_ENTRY_WIDTH         2
#define STATS_ENTRY_HEIGHT        4
#define STATS_ENTRY_PER_PAGE      (STATS_ENTRY_WIDTH * STATS_ENTRY_HEIGHT)

static void Setup_PlayerStatsPage()
{
    sQuestMenuData->scrollListOffset = 0;
}

static void HandleInput_PlayerStatsPage(u8 taskId)
{
    //if(HandleScrollBehaviour())
    //    Draw_MasteryLandingPage();

    if(JOY_REPEAT(DPAD_LEFT))
    {
        if(sQuestMenuData->scrollListOffset == 0)
            sQuestMenuData->scrollListOffset = ARRAY_COUNT(sDisplayStats) / STATS_ENTRY_PER_PAGE;
        else
            --sQuestMenuData->scrollListOffset;

        PlaySE(SE_DEX_SCROLL);
        Draw_PlayerStatsPage();
    }

    if(JOY_REPEAT(DPAD_RIGHT))
    {
        sQuestMenuData->scrollListOffset++;

        if(sQuestMenuData->scrollListOffset * STATS_ENTRY_PER_PAGE >= ARRAY_COUNT(sDisplayStats))
            sQuestMenuData->scrollListOffset = 0;

        PlaySE(SE_DEX_SCROLL);
        Draw_PlayerStatsPage();
    }

    if (JOY_NEW(B_BUTTON))
        SetupPage(PAGE_BOOK_INDEX);
}

static void Draw_PlayerStatsPage()
{
    u8 x, y;
    u32 i, counter;
    u8 const color[3] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY};
    u8 const valueColor[3] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_BLUE, TEXT_COLOR_LIGHT_GRAY};

    FillWindowPixelBuffer(WIN_LEFT_PAGE, PIXEL_FILL(0));
    FillWindowPixelBuffer(WIN_RIGHT_PAGE, PIXEL_FILL(0));

    // Remove previous sprites if we have any
    FreeAllSpritePalettes();
    ResetSpriteData();

    counter = 0;
    
    for(x = 0; x < STATS_ENTRY_WIDTH; ++x)
    {
        for(y = 0; y < STATS_ENTRY_HEIGHT; ++y)
        {
            i = sQuestMenuData->scrollListOffset * STATS_ENTRY_PER_PAGE + counter++;

            if(i >= ARRAY_COUNT(sDisplayStats))
                break;

            // Basically a line break
            if(sDisplayStats[i].name == NULL)
                continue;

            AddTextPrinterParameterized4(
                x == 0 ? WIN_LEFT_PAGE : WIN_RIGHT_PAGE,
                FONT_NARROW,
                0,
                4 + 16 * (y * 2 + 0),
                0, 0,
                color,
                TEXT_SKIP_DRAW,
                sDisplayStats[i].name
            );

            if(sDisplayStats[i].statId == GAME_STAT_FIRST_HOF_PLAY_TIME || sDisplayStats[i].statId == GAME_STAT_FASTEST_HOF_PLAY_TIME || sDisplayStats[i].statId == GAME_STAT_SLOWEST_HOF_PLAY_TIME)
            {
                // Conver to readable time
                u32 hours, minutes, seconds;
                u32 playTime = GetGameStat(sDisplayStats[i].statId);

                if (!GetGameStat(GAME_STAT_ENTERED_HOF))
                    playTime = 0;

                hours = playTime >> 16;
                minutes = (playTime >> 8) & 0xFF;
                seconds = playTime & 0xFF;
                if ((playTime >> 16) > 999)
                {
                    hours = 999;
                    minutes = 59;
                    seconds = 59;
                }
                
                ConvertIntToDecimalStringN(gStringVar1, hours, STR_CONV_MODE_RIGHT_ALIGN, 3);
                ConvertIntToDecimalStringN(gStringVar2, minutes, STR_CONV_MODE_LEADING_ZEROS, 2);
                ConvertIntToDecimalStringN(gStringVar3, seconds, STR_CONV_MODE_LEADING_ZEROS, 2);
                StringExpandPlaceholders(gStringVar4, sStatFormat_HofTime);
            }
            else
            {
                // Just convert to a number
                ConvertIntToDecimalStringN(gStringVar4, GetGameStat(sDisplayStats[i].statId), STR_CONV_MODE_LEFT_ALIGN, 9);
            }
            
            AddTextPrinterParameterized4(
                x == 0 ? WIN_LEFT_PAGE : WIN_RIGHT_PAGE,
                FONT_NARROW,
                8,
                4 + 16 * (y * 2 + 1),
                0, 0,
                valueColor,
                TEXT_SKIP_DRAW,
                gStringVar4
            );
        }
    }

    // Page numbers
    {
        u8* str;
        
        str = StringCopy(gStringVar4, sText_PageMarker);
        ConvertIntToDecimalStringN(str, sQuestMenuData->scrollListOffset * 2 + 1, STR_CONV_MODE_RIGHT_ALIGN, 3);
        AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 60, 131, 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);
        
        str = StringCopy(gStringVar4, sText_PageMarker);
        ConvertIntToDecimalStringN(str, sQuestMenuData->scrollListOffset * 2 + 2, STR_CONV_MODE_RIGHT_ALIGN, 3);
        AddTextPrinterParameterized4(WIN_RIGHT_PAGE, FONT_SMALL_NARROW, 4, 131, 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);
    }

    AddTextPrinterParameterized4(WIN_LEFT_PAGE, FONT_SMALL_NARROW, 0, 131, 0, 0, color, TEXT_SKIP_DRAW, sText_ChangePageLeft);
    AddTextPrinterParameterized4(WIN_RIGHT_PAGE, FONT_SMALL_NARROW, 57, 131, 0, 0, color, TEXT_SKIP_DRAW, sText_ChangePageRight);

    PutWindowTilemap(WIN_LEFT_PAGE);
    PutWindowTilemap(WIN_RIGHT_PAGE);
    CopyWindowToVram(WIN_LEFT_PAGE, COPYWIN_FULL);
    CopyWindowToVram(WIN_RIGHT_PAGE, COPYWIN_FULL);
}