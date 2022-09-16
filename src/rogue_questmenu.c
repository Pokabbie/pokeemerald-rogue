#include "global.h"
#include "main.h"
#include "battle.h"
#include "bg.h"
#include "contest_effect.h"
#include "data.h"
#include "event_data.h"
#include "field_screen_effect.h"
#include "gpu_regs.h"
#include "move_relearner.h"
#include "list_menu.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "menu_specialized.h"
#include "overworld.h"
#include "palette.h"
#include "pokemon_summary_screen.h"
#include "script.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "constants/rgb.h"
#include "constants/songs.h"

#include "daycare.h"
#include "party_menu.h"

#include "rogue.h"
#include "rogue_controller.h"
#include "rogue_quest.h"
#include "rogue_questmenu.h"

/*
 * Move relearner state machine
 * ------------------------
 *
 * Entry point: TeachQuestMenuMove
 *
 * TeachQuestMenuMove
 * Task_WaitForFadeOut
 * CB2_InitQuestMenu
 *   - Creates moveDisplayArrowTask to listen to right/left buttons.
 *   - Creates moveListScrollArrowTask to listen to up/down buttons.
 *   - Whenever the selected move changes (and once on init), the QuestMenuCursorCallback
 *     is called (see sQuestMenuMovesListTemplate). That callback will reload the contest
 *     display and battle display windows for the new move. Both are always loaded in
 *     memory, but only the currently active one is copied to VRAM. The exception to this
 *     is the appeal and jam hearts, which are sprites. QuestMenuShowPinSprites is called
 *     while reloading the contest display to control them.
 * DoQuestMenuMain: MENU_STATE_FADE_TO_BLACK
 * DoQuestMenuMain: MENU_STATE_WAIT_FOR_FADE
 *   - Go to MENU_STATE_IDLE_BATTLE_MODE
 *
 * DoQuestMenuMain: MENU_STATE_SETUP_QUEST_DESC
 * DoQuestMenuMain: MENU_STATE_IDLE_BATTLE_MODE
 *   - If the player selected a move (pressed A), go to MENU_STATE_PRINT_PIN_QUEST_PROMPT.
 *   - If the player cancelled (pressed B), go to MENU_STATE_PRINT_GIVE_UP_PROMPT.
 *   - If the player pressed left or right, swap the move display window to contest mode,
 *     and go to MENU_STATE_SETUP_QUEST_REWARD.
 *
 * DoQuestMenuMain: MENU_STATE_SETUP_QUEST_REWARD
 * DoQuestMenuMain: MENU_STATE_IDLE_CONTEST_MODE
 *   - If the player selected a move, go to MENU_STATE_PRINT_PIN_QUEST_PROMPT.
 *   - If the player cancelled, go to MENU_STATE_PRINT_GIVE_UP_PROMPT
 *   - If the player pressed left or right, swap the move display window to battle mode,
 *     and go to MENU_STATE_SETUP_QUEST_DESC.
 *
 * DoQuestMenuMain: MENU_STATE_PRINT_PIN_QUEST_PROMPT
 * DoQuestMenuMain: MENU_STATE_PIN_QUEST_CONFIRM
 *   - Wait for the player to confirm.
 *   - If cancelled, go to either MENU_STATE_SETUP_QUEST_DESC or MENU_STATE_SETUP_QUEST_REWARD.
 *   - If confirmed and the pokemon had an empty move slot, set VAR_0x8004 to TRUE and go to
 *     MENU_STATE_PRINT_TEXT_THEN_FANFARE.
 *   - If confirmed and the pokemon doesn't have an empty move slot, go to
 *     MENU_STATE_PRINT_TRYING_TO_LEARN_PROMPT.
 *
 * DoQuestMenuMain: MENU_STATE_PRINT_TRYING_TO_LEARN_PROMPT
 * DoQuestMenuMain: MENU_STATE_WAIT_FOR_TRYING_TO_LEARN
 * DoQuestMenuMain: MENU_STATE_CONFIRM_DELETE_OLD_MOVE
 *   - If the player confirms, go to MENU_STATE_PRINT_WHICH_MOVE_PROMPT.
 *   - If the player cancels, go to MENU_STATE_PRINT_STOP_TEACHING
 *
 * DoQuestMenuMain: MENU_STATE_PRINT_STOP_TEACHING
 * DoQuestMenuMain: MENU_STATE_WAIT_FOR_STOP_TEACHING
 * DoQuestMenuMain: MENU_STATE_CONFIRM_STOP_TEACHING
 *   - If the player confirms, go to MENU_STATE_CHOOSE_SETUP_STATE.
 *   - If the player cancels, go back to MENU_STATE_PRINT_TRYING_TO_LEARN_PROMPT.
 *
 * DoQuestMenuMain: MENU_STATE_PRINT_WHICH_MOVE_PROMPT
 * DoQuestMenuMain: MENU_STATE_SHOW_MOVE_SUMMARY_SCREEN
 *   - Go to ShowSelectMovePokemonSummaryScreen. When done, control returns to
 *     CB2_InitQuestMenuReturnFromSelectMove.
 *
 * DoQuestMenuMain: MENU_STATE_DOUBLE_FANFARE_FORGOT_MOVE
 * DoQuestMenuMain: MENU_STATE_PRINT_TEXT_THEN_FANFARE
 * DoQuestMenuMain: MENU_STATE_WAIT_FOR_FANFARE
 * DoQuestMenuMain: MENU_STATE_WAIT_FOR_A_BUTTON
 * DoQuestMenuMain: MENU_STATE_FADE_AND_RETURN
 * DoQuestMenuMain: MENU_STATE_RETURN_TO_FIELD
 *   - Clean up and go to CB2_ReturnToField.
 *
 * DoQuestMenuMain: MENU_STATE_PRINT_GIVE_UP_PROMPT
 * DoQuestMenuMain: MENU_STATE_GIVE_UP_CONFIRM
 *   - If the player confirms, go to MENU_STATE_FADE_AND_RETURN, and set VAR_0x8004 to FALSE.
 *   - If the player cancels, go to either MENU_STATE_SETUP_QUEST_DESC or
 *     MENU_STATE_SETUP_QUEST_REWARD.
 *
 * CB2_InitQuestMenuReturnFromSelectMove:
 *   - Do most of the same stuff as CB2_InitQuestMenu.
 * DoQuestMenuMain: MENU_STATE_FADE_FROM_SUMMARY_SCREEN
 * DoQuestMenuMain: MENU_STATE_TRY_OVERWRITE_MOVE
 *   - If any of the pokemon's existing moves were chosen, overwrite the move and
 *     go to MENU_STATE_DOUBLE_FANFARE_FORGOT_MOVE and set VAR_0x8004 to TRUE.
 *   - If the chosen move is the one the player selected before the summary screen,
 *     go to MENU_STATE_PRINT_STOP_TEACHING.
 *
 */

#define MENU_STATE_FADE_TO_BLACK 0
#define MENU_STATE_WAIT_FOR_FADE 1
#define MENU_STATE_UNREACHABLE 2
#define MENU_STATE_SETUP_QUEST_DESC 3
#define MENU_STATE_IDLE_BATTLE_MODE 4
#define MENU_STATE_SETUP_QUEST_REWARD 5
#define MENU_STATE_IDLE_CONTEST_MODE 6
// State 7 is skipped.
#define MENU_STATE_PRINT_PIN_QUEST_PROMPT 8
#define MENU_STATE_PIN_QUEST_CONFIRM 9
// States 10 and 11 are skipped.
#define MENU_STATE_PRINT_GIVE_UP_PROMPT 12
#define MENU_STATE_GIVE_UP_CONFIRM 13
#define MENU_STATE_FADE_AND_RETURN 14
#define MENU_STATE_RETURN_TO_FIELD 15
#define MENU_STATE_PRINT_TRYING_TO_LEARN_PROMPT 16
#define MENU_STATE_WAIT_FOR_TRYING_TO_LEARN 17
#define MENU_STATE_CONFIRM_DELETE_OLD_MOVE 18
#define MENU_STATE_PRINT_WHICH_MOVE_PROMPT 19
#define MENU_STATE_SHOW_MOVE_SUMMARY_SCREEN 20
// States 21, 22, and 23 are skipped.
#define MENU_STATE_PRINT_STOP_TEACHING 24
#define MENU_STATE_WAIT_FOR_STOP_TEACHING 25
#define MENU_STATE_CONFIRM_STOP_TEACHING 26
#define MENU_STATE_CHOOSE_SETUP_STATE 27
#define MENU_STATE_FADE_FROM_SUMMARY_SCREEN 28
#define MENU_STATE_TRY_OVERWRITE_MOVE 29
#define MENU_STATE_DOUBLE_FANFARE_FORGOT_MOVE 30
#define MENU_STATE_PRINT_TEXT_THEN_FANFARE 31
#define MENU_STATE_WAIT_FOR_FANFARE 32
#define MENU_STATE_WAIT_FOR_A_BUTTON 33

// The different versions of hearts are selected using animation
// commands.
#define APPEAL_HEART_EMPTY 0
#define APPEAL_HEART_FULL 1
#define JAM_HEART_EMPTY 2
#define JAM_HEART_FULL 3

#define MENU_PAGE_OVERVIEW 0
#define MENU_PAGE_PINNED_QUESTS 1
#define MENU_PAGE_ACTIVE_QUESTS 2
#define MENU_PAGE_INACTIVE_QUESTS 3
#define MENU_PAGE_COMPLETED_QUESTS 4
#define MENU_PAGE_TODO_QUESTS 5
#define MENU_PAGE_REPEATABLE_QUESTS 6
#define MENU_PAGE_NEW_QUESTS 7

#define MAX_QUESTS_TO_SHOW (QUEST_COUNT)

extern const u8 gText_QuestLogBack[];
extern const u8 gText_QuestLogTitleOverview[];
extern const u8 gText_QuestLogTitlePinned[];
extern const u8 gText_QuestLogTitleActive[];
extern const u8 gText_QuestLogTitleInactive[];
extern const u8 gText_QuestLogTitleComplete[];
extern const u8 gText_QuestLogTitleTodo[];
extern const u8 gText_QuestLogTitleRepeatable[];
extern const u8 gText_QuestLogTitleNew[];

extern const u8 gText_QuestLogPromptOverview[];
extern const u8 gText_QuestLogPromptCategory[];
extern const u8 gText_QuestLogPromptPinQuest[];
extern const u8 gText_QuestLogPromptUnpinQuest[];

static EWRAM_DATA struct
{
    u8 state;
    u8 currentPage;
    u8 pinSpriteIds[2];                               /*0x001*/
    u16 optionsToDisplay[MAX_QUESTS_TO_SHOW];               /*0x01A*/
    u8 partyMon;                                         /*0x044*/
    u8 moveSlot;                                         /*0x045*/
    struct ListMenuItem menuItems[MAX_QUESTS_TO_SHOW];  /*0x0E8*/
    u8 numMenuChoices;                                   /*0x110*/
    u8 numToShowAtOnce;                                  /*0x111*/
    u8 listMenuTask;                                     /*0x112*/
    u8 moveListScrollArrowTask;                          /*0x113*/
    u8 moveDisplayArrowTask;                             /*0x114*/
    u16 scrollOffset;                                    /*0x116*/
} *sQuestMenuStruct = {0};

static EWRAM_DATA struct {
    u16 listOffset;
    u16 listRow;
    u16 prevListOffset;
    u16 prevListRow;
    bool8 showQuestRewards;
} sQuestMenuMenuSate = {0};



static const u8* const sQuestMenuPageTitles[] =
{
    [MENU_PAGE_OVERVIEW] = gText_QuestLogTitleOverview,
    [MENU_PAGE_PINNED_QUESTS] = gText_QuestLogTitlePinned,
    [MENU_PAGE_ACTIVE_QUESTS] = gText_QuestLogTitleActive,
    [MENU_PAGE_INACTIVE_QUESTS] = gText_QuestLogTitleInactive,
    [MENU_PAGE_COMPLETED_QUESTS] = gText_QuestLogTitleComplete,
    [MENU_PAGE_TODO_QUESTS] = gText_QuestLogTitleTodo,
    [MENU_PAGE_REPEATABLE_QUESTS] = gText_QuestLogTitleRepeatable,
    [MENU_PAGE_NEW_QUESTS] = gText_QuestLogTitleNew,
};

static const u16 sQuestMenuPaletteData[] = INCBIN_U16("graphics/interface/ui_learn_move.gbapal");

// The arrow sprites in this spritesheet aren't used. The scroll-arrow system provides its own
// arrow sprites.
static const u8 sQuestMenuSpriteSheetData[] = INCBIN_U8("graphics/interface/ui_learn_move.4bpp");


static const struct OamData sQuestPinOamData =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = 0,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(8x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(8x8),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const struct OamData sUnusedOam1 =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = 0,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(8x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(8x16),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const struct OamData sUnusedOam2 =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = 0,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(16x8),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const struct SpriteSheet sQuestMenuSpriteSheet =
{
    .data = sQuestMenuSpriteSheetData,
    .size = 0x180,
    .tag = 5525
};

static const struct SpritePalette sQuestMenuPalette =
{
    .data = sQuestMenuPaletteData,
    .tag = 5526
};

static const struct ScrollArrowsTemplate sDisplayModeArrowsTemplate =
{
    .firstArrowType = SCROLL_ARROW_LEFT,
    .firstX = 27,
    .firstY = 16,
    .secondArrowType = SCROLL_ARROW_RIGHT,
    .secondX = 117,
    .secondY = 16,
    .fullyUpThreshold = -1,
    .fullyDownThreshold = -1,
    .tileTag = 5325,
    .palTag = 5325,
    .palNum = 0,
};

static const struct ScrollArrowsTemplate sMoveListScrollArrowsTemplate =
{
    .firstArrowType = SCROLL_ARROW_UP,
    .firstX = 192,
    .firstY = 8,
    .secondArrowType = SCROLL_ARROW_DOWN,
    .secondX = 192,
    .secondY = 104,
    .fullyUpThreshold = 0,
    .fullyDownThreshold = 0,
    .tileTag = 5425,
    .palTag = 5425,
    .palNum = 0,
};

static const union AnimCmd sHeartSprite_AppealEmptyFrame[] =
{
    ANIMCMD_FRAME(8, 5, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sHeartSprite_AppealFullFrame[] =
{
    ANIMCMD_FRAME(9, 5, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sHeartSprite_JamEmptyFrame[] =
{
    ANIMCMD_FRAME(10, 5, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sHeartSprite_JamFullFrame[] =
{
    ANIMCMD_FRAME(11, 5, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sHeartSpriteAnimationCommands[] =
{
    [APPEAL_HEART_EMPTY] = sHeartSprite_AppealEmptyFrame,
    [APPEAL_HEART_FULL] = sHeartSprite_AppealFullFrame,
    [JAM_HEART_EMPTY] = sHeartSprite_JamEmptyFrame,
    [JAM_HEART_FULL] = sHeartSprite_JamFullFrame,
};

static const struct SpriteTemplate sQuestPinSprite =
{
    .tileTag = 5525,
    .paletteTag = 5526,
    .oam = &sQuestPinOamData,
    .anims = sHeartSpriteAnimationCommands,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const struct BgTemplate sQuestMenuMenuBackgroundTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0,
    },
    {
        .bg = 1,
        .charBaseIndex = 0,
        .mapBaseIndex = 30,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0,
    },
};

static void DoQuestMenuMain(void);
static void CreateOptionsDisplayList(void);
static void CreateUISprites(void);
static void CB2_QuestMenuMain(void);
static void CB2_InitQuestMenu(void);
static void CB2_InitQuestMenuReturnFromSelectMove(void);
static void InitQuestMenuBackgroundLayers(void);
static void AddScrollArrows(void);
static void HandleInput(u8);
static void ShowTeachMoveText(u8);
static s32 GetCurrentOptionMove(void);
static void FreeQuestMenuResources(void);
static void RemoveScrollArrows(void);
static void QuestMenuShowPinSprites(s32 moveId);

static void PrintPromptForCurrentPage();
static void RecreateOptionsForPage(u8 pageId);

static void VBlankCB_QuestMenu(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

bool8 Rogue_IsQuestMenuOverviewActive(void)
{
    return sQuestMenuStruct->currentPage == MENU_PAGE_OVERVIEW;
}

void Rogue_OpenQuestMenu(RogueQuestMenuCallback callback)
{
    ScriptContext2_Enable();
    SetMainCallback2(CB2_InitQuestMenu);
    gFieldCallback = FieldCB_ContinueScriptHandleMusic;
}

static void GatherOptionsToDisplay()
{
    u16 i;
    struct RogueQuestState state;

    switch(sQuestMenuStruct->currentPage)
    {
        case MENU_PAGE_PINNED_QUESTS:
            sQuestMenuStruct->numMenuChoices = 0;
            for(i = QUEST_FIRST; i < QUEST_COUNT; ++i)
            {
                if(GetQuestState(i, &state) && state.isPinned)
                {
                    sQuestMenuStruct->optionsToDisplay[sQuestMenuStruct->numMenuChoices++] = i;
                }
            }
            break;

        case MENU_PAGE_ACTIVE_QUESTS:
            sQuestMenuStruct->numMenuChoices = 0;
            for(i = QUEST_FIRST; i < QUEST_COUNT; ++i)
            {
                if(GetQuestState(i, &state) && state.isValid)
                {
                    sQuestMenuStruct->optionsToDisplay[sQuestMenuStruct->numMenuChoices++] = i;
                }
            }
            break;

        case MENU_PAGE_INACTIVE_QUESTS:
            sQuestMenuStruct->numMenuChoices = 0;
            for(i = QUEST_FIRST; i < QUEST_COUNT; ++i)
            {
                if(GetQuestState(i, &state) && !state.isValid)
                {
                    sQuestMenuStruct->optionsToDisplay[sQuestMenuStruct->numMenuChoices++] = i;
                }
            }
            break;

        case MENU_PAGE_COMPLETED_QUESTS:
            sQuestMenuStruct->numMenuChoices = 0;
            for(i = QUEST_FIRST; i < QUEST_COUNT; ++i)
            {
                if(GetQuestState(i, &state) && state.isCompleted)
                {
                    sQuestMenuStruct->optionsToDisplay[sQuestMenuStruct->numMenuChoices++] = i;
                }
            }
            break;

        case MENU_PAGE_TODO_QUESTS:
            sQuestMenuStruct->numMenuChoices = 0;
            for(i = QUEST_FIRST; i < QUEST_COUNT; ++i)
            {
                if(GetQuestState(i, &state) && !state.isCompleted)
                {
                    sQuestMenuStruct->optionsToDisplay[sQuestMenuStruct->numMenuChoices++] = i;
                }
            }
            break;

        case MENU_PAGE_REPEATABLE_QUESTS:
            sQuestMenuStruct->numMenuChoices = 0;
            for(i = QUEST_FIRST; i < QUEST_COUNT; ++i)
            {
                if(GetQuestState(i, &state) && IsQuestRepeatable(i))
                {
                    sQuestMenuStruct->optionsToDisplay[sQuestMenuStruct->numMenuChoices++] = i;
                }
            }
            break;

        case MENU_PAGE_NEW_QUESTS:
            sQuestMenuStruct->numMenuChoices = 0;
            for(i = QUEST_FIRST; i < QUEST_COUNT; ++i)
            {
                if(GetQuestState(i, &state) && state.hasNewMarker)
                {
                    sQuestMenuStruct->optionsToDisplay[sQuestMenuStruct->numMenuChoices++] = i;
                }
            }
            break;
            
         default: // MENU_PAGE_OVERVIEW
            sQuestMenuStruct->numMenuChoices = 0;
            if(Rogue_IsRunActive())
            {
                sQuestMenuStruct->optionsToDisplay[sQuestMenuStruct->numMenuChoices++] = MENU_PAGE_PINNED_QUESTS;

                if(AnyNewQuests())
                    sQuestMenuStruct->optionsToDisplay[sQuestMenuStruct->numMenuChoices++] = MENU_PAGE_NEW_QUESTS;

                sQuestMenuStruct->optionsToDisplay[sQuestMenuStruct->numMenuChoices++] = MENU_PAGE_ACTIVE_QUESTS;
                sQuestMenuStruct->optionsToDisplay[sQuestMenuStruct->numMenuChoices++] = MENU_PAGE_INACTIVE_QUESTS;
                sQuestMenuStruct->optionsToDisplay[sQuestMenuStruct->numMenuChoices++] = MENU_PAGE_COMPLETED_QUESTS;
            }
            else
            {
                sQuestMenuStruct->optionsToDisplay[sQuestMenuStruct->numMenuChoices++] = MENU_PAGE_PINNED_QUESTS;

                if(AnyNewQuests())
                    sQuestMenuStruct->optionsToDisplay[sQuestMenuStruct->numMenuChoices++] = MENU_PAGE_NEW_QUESTS;

                sQuestMenuStruct->optionsToDisplay[sQuestMenuStruct->numMenuChoices++] = MENU_PAGE_TODO_QUESTS;
                sQuestMenuStruct->optionsToDisplay[sQuestMenuStruct->numMenuChoices++] = MENU_PAGE_REPEATABLE_QUESTS;
                sQuestMenuStruct->optionsToDisplay[sQuestMenuStruct->numMenuChoices++] = MENU_PAGE_COMPLETED_QUESTS;
            }
            break;

    };
}

static void CB2_InitQuestMenu(void)
{
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetTasks();
    ClearScheduledBgCopiesToVram();
    sQuestMenuStruct = AllocZeroed(sizeof(*sQuestMenuStruct));
    sQuestMenuStruct->partyMon = 0;
    sQuestMenuStruct->currentPage = MENU_PAGE_OVERVIEW; 
    SetVBlankCallback(VBlankCB_QuestMenu);

    InitQuestMenuBackgroundLayers();
    InitQuestMenuWindows(FALSE);

    sQuestMenuMenuSate.listOffset = 0;
    sQuestMenuMenuSate.listRow = 0;
    sQuestMenuMenuSate.showQuestRewards = FALSE;

    CreateOptionsDisplayList();

    LoadSpriteSheet(&sQuestMenuSpriteSheet);
    LoadSpritePalette(&sQuestMenuPalette);
    CreateUISprites();

    sQuestMenuStruct->listMenuTask = ListMenuInit(&gMultiuseListMenuTemplate, sQuestMenuMenuSate.listOffset, sQuestMenuMenuSate.listRow);
    FillPalette(RGB_BLACK, 0, 2);
    SetMainCallback2(CB2_QuestMenuMain);

    PrintPromptForCurrentPage();
}

static void CB2_InitQuestMenuReturnFromSelectMove(void)
{
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetTasks();
    ClearScheduledBgCopiesToVram();
    sQuestMenuStruct = AllocZeroed(sizeof(*sQuestMenuStruct));
    sQuestMenuStruct->state = MENU_STATE_FADE_FROM_SUMMARY_SCREEN;
    sQuestMenuStruct->partyMon = gSpecialVar_0x8004;
    sQuestMenuStruct->moveSlot = gSpecialVar_0x8005;
    SetVBlankCallback(VBlankCB_QuestMenu);

    InitQuestMenuBackgroundLayers();
    InitQuestMenuWindows(sQuestMenuMenuSate.showQuestRewards);
    CreateOptionsDisplayList();

    LoadSpriteSheet(&sQuestMenuSpriteSheet);
    LoadSpritePalette(&sQuestMenuPalette);
    CreateUISprites();

    sQuestMenuStruct->listMenuTask = ListMenuInit(&gMultiuseListMenuTemplate, sQuestMenuMenuSate.listOffset, sQuestMenuMenuSate.listRow);
    FillPalette(RGB_BLACK, 0, 2);
    SetMainCallback2(CB2_QuestMenuMain);
}

static void FormatAndPrintText(const u8 *src)
{
    StringExpandPlaceholders(gStringVar4, src);
    FillWindowPixelBuffer(3, 0x11);
    AddTextPrinterParameterized(3, FONT_NORMAL, gStringVar4, 0, 1, 0, NULL);
    //QuestMenuPrintText(gStringVar4);
}

static void PrintPromptForCurrentPage()
{
    if(sQuestMenuStruct->currentPage == MENU_PAGE_OVERVIEW)
    {
        FormatAndPrintText(gText_QuestLogPromptOverview);
    }
    else
    {
        FormatAndPrintText(gText_QuestLogPromptCategory);
    }
}

static void RecreateOptionsForPage(u8 pageId)
{
    PutWindowTilemap(0);
    sQuestMenuStruct->state = MENU_STATE_SETUP_QUEST_DESC;
    sQuestMenuStruct->currentPage = pageId;
    sQuestMenuMenuSate.showQuestRewards = FALSE;
    
    CreateOptionsDisplayList();
    
    DestroyListMenuTask(sQuestMenuStruct->listMenuTask, &sQuestMenuMenuSate.listOffset, &sQuestMenuMenuSate.listRow);

    if(pageId == MENU_PAGE_OVERVIEW)
    {
        // Restore prev menu position
        sQuestMenuMenuSate.listOffset = sQuestMenuMenuSate.prevListOffset;
        sQuestMenuMenuSate.listRow = sQuestMenuMenuSate.prevListRow;
    }
    else
    {
        // Store menu position
        sQuestMenuMenuSate.prevListOffset = sQuestMenuMenuSate.listOffset;
        sQuestMenuMenuSate.prevListRow = sQuestMenuMenuSate.listRow;
        sQuestMenuMenuSate.listOffset = 0;
        sQuestMenuMenuSate.listRow = 0;
    }

    sQuestMenuStruct->listMenuTask = ListMenuInit(&gMultiuseListMenuTemplate, sQuestMenuMenuSate.listOffset, sQuestMenuMenuSate.listRow);

    PrintPromptForCurrentPage();
}

static void InitQuestMenuBackgroundLayers(void)
{
    ResetVramOamAndBgCntRegs();
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sQuestMenuMenuBackgroundTemplates, ARRAY_COUNT(sQuestMenuMenuBackgroundTemplates));
    ResetAllBgsCoordinates();
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 |
                                  DISPCNT_OBJ_1D_MAP |
                                  DISPCNT_OBJ_ON);
    ShowBg(0);
    ShowBg(1);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
}

static void CB2_QuestMenuMain(void)
{
    DoQuestMenuMain();
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

// See the state machine doc at the top of the file.
static void DoQuestMenuMain(void)
{
    switch (sQuestMenuStruct->state)
    {
    case MENU_STATE_FADE_TO_BLACK:
        sQuestMenuStruct->state++;
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        break;
    case MENU_STATE_WAIT_FOR_FADE:
        if (!gPaletteFade.active)
        {
            sQuestMenuStruct->state = MENU_STATE_IDLE_BATTLE_MODE;
        }
        break;
    case MENU_STATE_UNREACHABLE:
        sQuestMenuStruct->state++;
        break;
    case MENU_STATE_SETUP_QUEST_DESC:

        //HideHeartSpritesAndShowTeachMoveText(FALSE);
        sQuestMenuStruct->state++;
        AddScrollArrows();
        break;
    case MENU_STATE_IDLE_BATTLE_MODE:
        HandleInput(FALSE);
        break;
    case MENU_STATE_SETUP_QUEST_REWARD:
        ShowTeachMoveText(FALSE);
        sQuestMenuStruct->state++;
        AddScrollArrows();
        break;
    case MENU_STATE_IDLE_CONTEST_MODE:
        HandleInput(TRUE);
        break;
    case MENU_STATE_PRINT_PIN_QUEST_PROMPT:
        if (!QuestMenuRunTextPrinters())
        {
            QuestMenuCreateYesNoMenu();
            sQuestMenuStruct->state++;
        }
        break;
    case MENU_STATE_PIN_QUEST_CONFIRM:
        {
            struct RogueQuestState state;
            u16 questId = GetCurrentOptionMove();
            s8 selection = Menu_ProcessInputNoWrapClearOnChoose();

            if (selection == 0 && GetQuestState(questId, &state))
            {
                state.isPinned = !state.isPinned;
                SetQuestState(questId, &state);

                QuestMenuShowPinSprites(GetCurrentOptionMove());
            }
            //else if (selection == MENU_B_PRESSED || selection == 1)
            //{
            //}
            
            if(selection != MENU_NOTHING_CHOSEN)
            {
                if (sQuestMenuMenuSate.showQuestRewards == FALSE)
                {
                    sQuestMenuStruct->state = MENU_STATE_SETUP_QUEST_DESC;
                }
                else if (sQuestMenuMenuSate.showQuestRewards == TRUE)
                {
                    sQuestMenuStruct->state = MENU_STATE_SETUP_QUEST_REWARD;
                }
                
                PrintPromptForCurrentPage();
            }
        }
        break;
    case MENU_STATE_PRINT_GIVE_UP_PROMPT:
        if (!QuestMenuRunTextPrinters())
        {
            QuestMenuCreateYesNoMenu();
            sQuestMenuStruct->state++;
        }
        break;
    case MENU_STATE_GIVE_UP_CONFIRM:
        {
            s8 selection = Menu_ProcessInputNoWrapClearOnChoose();

            if (selection == 0)
            {
                gSpecialVar_0x8004 = FALSE;
                sQuestMenuStruct->state = MENU_STATE_FADE_AND_RETURN;
            }
            else if (selection == -1 || selection == 1)
            {
                if (sQuestMenuMenuSate.showQuestRewards == FALSE)
                {
                    sQuestMenuStruct->state = MENU_STATE_SETUP_QUEST_DESC;
                }
                else if (sQuestMenuMenuSate.showQuestRewards == TRUE)
                {
                    sQuestMenuStruct->state = MENU_STATE_SETUP_QUEST_REWARD;
                }
            }
        }
        break;
    case MENU_STATE_PRINT_TRYING_TO_LEARN_PROMPT:
        FormatAndPrintText(gText_MoveRelearnerPkmnTryingToLearnMove);
        sQuestMenuStruct->state++;
        break;
    case MENU_STATE_WAIT_FOR_TRYING_TO_LEARN:
        if (!QuestMenuRunTextPrinters())
        {
            QuestMenuCreateYesNoMenu();
            sQuestMenuStruct->state = MENU_STATE_CONFIRM_DELETE_OLD_MOVE;
        }
        break;
    case MENU_STATE_CONFIRM_DELETE_OLD_MOVE:
        {
            s8 var = Menu_ProcessInputNoWrapClearOnChoose();

            if (var == 0)
            {
                FormatAndPrintText(gText_MoveRelearnerWhichMoveToForget);
                sQuestMenuStruct->state = MENU_STATE_PRINT_WHICH_MOVE_PROMPT;
            }
            else if (var == -1 || var == 1)
            {
                sQuestMenuStruct->state = MENU_STATE_PRINT_STOP_TEACHING;
            }
        }
        break;
    case MENU_STATE_PRINT_STOP_TEACHING:
        StringCopy(gStringVar2, gMoveNames[GetCurrentOptionMove()]);
        FormatAndPrintText(gText_MoveRelearnerStopTryingToTeachMove);
        sQuestMenuStruct->state++;
        break;
    case MENU_STATE_WAIT_FOR_STOP_TEACHING:
        if (!QuestMenuRunTextPrinters())
        {
            QuestMenuCreateYesNoMenu();
            sQuestMenuStruct->state++;
        }
        break;
    case MENU_STATE_CONFIRM_STOP_TEACHING:
        {
            s8 var = Menu_ProcessInputNoWrapClearOnChoose();

            if (var == 0)
            {
                sQuestMenuStruct->state = MENU_STATE_CHOOSE_SETUP_STATE;
            }
            else if (var == MENU_B_PRESSED || var == 1)
            {
                // What's the point? It gets set to MENU_STATE_PRINT_TRYING_TO_LEARN_PROMPT, anyway.
                if (sQuestMenuMenuSate.showQuestRewards == FALSE)
                {
                    sQuestMenuStruct->state = MENU_STATE_SETUP_QUEST_DESC;
                }
                else if (sQuestMenuMenuSate.showQuestRewards == TRUE)
                {
                    sQuestMenuStruct->state = MENU_STATE_SETUP_QUEST_REWARD;
                }
                sQuestMenuStruct->state = MENU_STATE_PRINT_TRYING_TO_LEARN_PROMPT;
            }
        }
        break;
    case MENU_STATE_CHOOSE_SETUP_STATE:
        if (!QuestMenuRunTextPrinters())
        {
            FillWindowPixelBuffer(3, 0x11);
            if (sQuestMenuMenuSate.showQuestRewards == FALSE)
            {
                sQuestMenuStruct->state = MENU_STATE_SETUP_QUEST_DESC;
            }
            else if (sQuestMenuMenuSate.showQuestRewards == TRUE)
            {
                sQuestMenuStruct->state = MENU_STATE_SETUP_QUEST_REWARD;
            }
        }
        break;
    case MENU_STATE_PRINT_WHICH_MOVE_PROMPT:
        if (!QuestMenuRunTextPrinters())
        {
            sQuestMenuStruct->state = MENU_STATE_SHOW_MOVE_SUMMARY_SCREEN;
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        }
        break;
    case MENU_STATE_SHOW_MOVE_SUMMARY_SCREEN:
        if (!gPaletteFade.active)
        {
            ShowSelectMovePokemonSummaryScreen(gPlayerParty, sQuestMenuStruct->partyMon, gPlayerPartyCount - 1, CB2_InitQuestMenuReturnFromSelectMove, GetCurrentOptionMove());
            FreeQuestMenuResources();
        }
        break;
    case 21:
        if (!QuestMenuRunTextPrinters())
        {
            sQuestMenuStruct->state = MENU_STATE_FADE_AND_RETURN;
        }
        break;
    case 22:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        break;
    case MENU_STATE_FADE_AND_RETURN:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        sQuestMenuStruct->state++;
        break;
    case MENU_STATE_RETURN_TO_FIELD:
        if (!gPaletteFade.active)
        {
            FreeQuestMenuResources();
            SetMainCallback2(CB2_ReturnToField);
        }
        break;
    case MENU_STATE_FADE_FROM_SUMMARY_SCREEN:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        sQuestMenuStruct->state++;
        if (sQuestMenuMenuSate.showQuestRewards == FALSE)
        {
            //HideHeartSpritesAndShowTeachMoveText(TRUE);
        }
        else if (sQuestMenuMenuSate.showQuestRewards == TRUE)
        {
            ShowTeachMoveText(TRUE);
        }
        RemoveScrollArrows();
        CopyWindowToVram(3, COPYWIN_GFX);
        break;
    case MENU_STATE_TRY_OVERWRITE_MOVE:
        if (!gPaletteFade.active)
        {
            if (sQuestMenuStruct->moveSlot == MAX_MON_MOVES)
            {
                sQuestMenuStruct->state = MENU_STATE_PRINT_STOP_TEACHING;
            }
            else
            {
                u16 moveId = GetMonData(&gPlayerParty[sQuestMenuStruct->partyMon], MON_DATA_MOVE1 + sQuestMenuStruct->moveSlot);

                StringCopy(gStringVar3, gMoveNames[moveId]);
                RemoveMonPPBonus(&gPlayerParty[sQuestMenuStruct->partyMon], sQuestMenuStruct->moveSlot);
                SetMonMoveSlot(&gPlayerParty[sQuestMenuStruct->partyMon], GetCurrentOptionMove(), sQuestMenuStruct->moveSlot);
                StringCopy(gStringVar2, gMoveNames[GetCurrentOptionMove()]);
                FormatAndPrintText(gText_MoveRelearnerAndPoof);
                sQuestMenuStruct->state = MENU_STATE_DOUBLE_FANFARE_FORGOT_MOVE;
                gSpecialVar_0x8004 = TRUE;
            }
        }
        break;
    case MENU_STATE_DOUBLE_FANFARE_FORGOT_MOVE:
        if (!QuestMenuRunTextPrinters())
        {
            FormatAndPrintText(gText_MoveRelearnerPkmnForgotMoveAndLearnedNew);
            sQuestMenuStruct->state = MENU_STATE_PRINT_TEXT_THEN_FANFARE;
            PlayFanfare(MUS_LEVEL_UP);
        }
        break;
    case MENU_STATE_PRINT_TEXT_THEN_FANFARE:
        if (!QuestMenuRunTextPrinters())
        {
            PlayFanfare(MUS_LEVEL_UP);
            sQuestMenuStruct->state = MENU_STATE_WAIT_FOR_FANFARE;
        }
        break;
    case MENU_STATE_WAIT_FOR_FANFARE:
        if (IsFanfareTaskInactive())
        {
            sQuestMenuStruct->state = MENU_STATE_WAIT_FOR_A_BUTTON;
        }
        break;
    case MENU_STATE_WAIT_FOR_A_BUTTON:
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            sQuestMenuStruct->state = MENU_STATE_FADE_AND_RETURN;
        }
        break;
    }
}

static void FreeQuestMenuResources(void)
{
    RemoveScrollArrows();
    DestroyListMenuTask(sQuestMenuStruct->listMenuTask, &sQuestMenuMenuSate.listOffset, &sQuestMenuMenuSate.listRow);
    FreeAllWindowBuffers();
    FREE_AND_SET_NULL(sQuestMenuStruct);
    ResetSpriteData();
    FreeAllSpritePalettes();
}

static void HandleInput(bool8 showRewards)
{
    struct RogueQuestState state;
    s32 itemId = ListMenu_ProcessInput(sQuestMenuStruct->listMenuTask);
    ListMenuGetScrollAndRow(sQuestMenuStruct->listMenuTask, &sQuestMenuMenuSate.listOffset, &sQuestMenuMenuSate.listRow);

    switch (itemId)
    {
    case LIST_NOTHING_CHOSEN:
        if (!(JOY_NEW(DPAD_LEFT | DPAD_RIGHT)) && !GetLRKeysPressed())
        {
            break;
        }

        if(Rogue_IsQuestMenuOverviewActive())
        {
            // Can't change screen
            break;
        }

        PlaySE(SE_SELECT);

        if (showRewards == FALSE)
        {
            PutWindowTilemap(1);
            sQuestMenuStruct->state = MENU_STATE_SETUP_QUEST_REWARD;
            sQuestMenuMenuSate.showQuestRewards = TRUE;
        }
        else
        {
            PutWindowTilemap(0);
            sQuestMenuStruct->state = MENU_STATE_SETUP_QUEST_DESC;
            sQuestMenuMenuSate.showQuestRewards = FALSE;
        }

        ScheduleBgCopyTilemapToVram(1);
        break;
    case LIST_CANCEL:
        PlaySE(SE_SELECT);
        
        if(Rogue_IsQuestMenuOverviewActive())
        {
            RemoveScrollArrows();
            sQuestMenuStruct->state = MENU_STATE_FADE_AND_RETURN; //MENU_STATE_PRINT_GIVE_UP_PROMPT;
        }
        else
        {
            RecreateOptionsForPage(MENU_PAGE_OVERVIEW);
        }
        break;
    default:
        PlaySE(SE_SELECT);

        if(Rogue_IsQuestMenuOverviewActive())
        {
            RecreateOptionsForPage(itemId);

            ScheduleBgCopyTilemapToVram(1);
        }
        else if(GetQuestState(itemId, &state))
        {
            // Goes to YesNo menu
            RemoveScrollArrows();
            sQuestMenuStruct->state = MENU_STATE_PRINT_PIN_QUEST_PROMPT;

            if(state.isPinned)
                FormatAndPrintText(gText_QuestLogPromptUnpinQuest);
            else
                FormatAndPrintText(gText_QuestLogPromptPinQuest);
        }
        break;
    }

    QuestMenuShowPinSprites(GetCurrentOptionMove());
}

static s32 GetCurrentOptionMove(void)
{
    return sQuestMenuStruct->menuItems[sQuestMenuMenuSate.listRow + sQuestMenuMenuSate.listOffset].id;
}

// Theory: This used to make the heart sprites visible again (i.e.
// this was the inverse of HideHeartsAndShowTeachMoveText), but the
// code was commented out. The bool argument would have been named
// "justShowHearts." The code for showing/hiding the heards was moved
// to QuestMenuShowPinSprites, which is called whenever a new move is
// selected and whenever the display mode changes.
static void ShowTeachMoveText(bool8 shouldDoNothingInstead)
{
    //if (shouldDoNothingInstead == FALSE)
    //{
    //    StringExpandPlaceholders(gStringVar4, gText_TeachWhichMoveToPkmn);
    //    FillWindowPixelBuffer(3, 0x11);
    //    AddTextPrinterParameterized(3, FONT_NORMAL, gStringVar4, 0, 1, 0, NULL);
    //}
}

static void CreateUISprites(void)
{
    sQuestMenuStruct->moveDisplayArrowTask = TASK_NONE;
    sQuestMenuStruct->moveListScrollArrowTask = TASK_NONE;
    AddScrollArrows();

    // Unpin sprite
    sQuestMenuStruct->pinSpriteIds[0] = CreateSprite(&sQuestPinSprite, 15, 15, 0);
    StartSpriteAnim(&gSprites[sQuestMenuStruct->pinSpriteIds[0]], 2);

    // Pin sprite
    sQuestMenuStruct->pinSpriteIds[1] = CreateSprite(&sQuestPinSprite, 15, 15, 0);
    StartSpriteAnim(&gSprites[sQuestMenuStruct->pinSpriteIds[1]], 1);

    
    gSprites[sQuestMenuStruct->pinSpriteIds[0]].invisible = TRUE;
    gSprites[sQuestMenuStruct->pinSpriteIds[1]].invisible = TRUE;
}

static void AddScrollArrows(void)
{
    if (sQuestMenuStruct->moveDisplayArrowTask == TASK_NONE)
    {
        sQuestMenuStruct->moveDisplayArrowTask = AddScrollIndicatorArrowPair(&sDisplayModeArrowsTemplate, &sQuestMenuStruct->scrollOffset);
    }

    if (sQuestMenuStruct->moveListScrollArrowTask == TASK_NONE)
    {
        gTempScrollArrowTemplate = sMoveListScrollArrowsTemplate;
        gTempScrollArrowTemplate.fullyDownThreshold = sQuestMenuStruct->numMenuChoices - sQuestMenuStruct->numToShowAtOnce;
        sQuestMenuStruct->moveListScrollArrowTask = AddScrollIndicatorArrowPair(&gTempScrollArrowTemplate, &sQuestMenuMenuSate.listOffset);
    }
}

static void RemoveScrollArrows(void)
{
    if (sQuestMenuStruct->moveDisplayArrowTask != TASK_NONE)
    {
        RemoveScrollIndicatorArrowPair(sQuestMenuStruct->moveDisplayArrowTask);
        sQuestMenuStruct->moveDisplayArrowTask = TASK_NONE;
    }

    if (sQuestMenuStruct->moveListScrollArrowTask != TASK_NONE)
    {
        RemoveScrollIndicatorArrowPair(sQuestMenuStruct->moveListScrollArrowTask);
        sQuestMenuStruct->moveListScrollArrowTask = TASK_NONE;
    }
}

static void CreateOptionsDisplayList(void)
{
    s32 i;
    u8 nickname[POKEMON_NAME_LENGTH + 1];

    GatherOptionsToDisplay();

    if(Rogue_IsQuestMenuOverviewActive())
    {
        for (i = 0; i < sQuestMenuStruct->numMenuChoices; i++)
        {
            sQuestMenuStruct->menuItems[i].name = sQuestMenuPageTitles[sQuestMenuStruct->optionsToDisplay[i]];
            sQuestMenuStruct->menuItems[i].id = sQuestMenuStruct->optionsToDisplay[i];
        }
    }
    else
    {
        for (i = 0; i < sQuestMenuStruct->numMenuChoices; i++)
        {
            sQuestMenuStruct->menuItems[i].name = gRogueQuests[sQuestMenuStruct->optionsToDisplay[i]].title;
            sQuestMenuStruct->menuItems[i].id = sQuestMenuStruct->optionsToDisplay[i];
        }
    }

    GetMonData(&gPlayerParty[sQuestMenuStruct->partyMon], MON_DATA_NICKNAME, nickname);
    StringCopy_Nickname(gStringVar1, nickname);
    sQuestMenuStruct->menuItems[sQuestMenuStruct->numMenuChoices].name = gText_QuestLogBack;
    sQuestMenuStruct->menuItems[sQuestMenuStruct->numMenuChoices].id = LIST_CANCEL;
    sQuestMenuStruct->numMenuChoices++;
    sQuestMenuStruct->numToShowAtOnce = LoadQuestMenuMovesList(sQuestMenuStruct->menuItems, sQuestMenuStruct->numMenuChoices);
}

static void QuestMenuShowPinSprites(s32 questId)
{
    u8 i;

    if(Rogue_IsQuestMenuOverviewActive())
    {
        for (i = 0; i < ARRAY_COUNT(sQuestMenuStruct->pinSpriteIds); i++)
        {
            gSprites[sQuestMenuStruct->pinSpriteIds[i]].invisible = TRUE;
        }
    }
    else
    {
        struct RogueQuestState state;
        bool8 isPinned = FALSE;

        if(GetQuestState(questId, &state))
        {
            isPinned = state.isPinned;
            if(state.hasNewMarker)
            {
                // This quest has now been viewed
                state.hasNewMarker = FALSE;
                SetQuestState(questId, &state);
            }
        }

        if(isPinned)
        {
            gSprites[sQuestMenuStruct->pinSpriteIds[0]].invisible = TRUE;
            gSprites[sQuestMenuStruct->pinSpriteIds[1]].invisible = FALSE;
        }
        else
        {
            gSprites[sQuestMenuStruct->pinSpriteIds[0]].invisible = FALSE;
            gSprites[sQuestMenuStruct->pinSpriteIds[1]].invisible = TRUE;
        }
    }
}
