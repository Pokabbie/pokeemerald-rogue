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
 *     is the appeal and jam hearts, which are sprites. QuestMenuShowHideHearts is called
 *     while reloading the contest display to control them.
 * DoQuestMenuMain: MENU_STATE_FADE_TO_BLACK
 * DoQuestMenuMain: MENU_STATE_WAIT_FOR_FADE
 *   - Go to MENU_STATE_IDLE_BATTLE_MODE
 *
 * DoQuestMenuMain: MENU_STATE_SETUP_BATTLE_MODE
 * DoQuestMenuMain: MENU_STATE_IDLE_BATTLE_MODE
 *   - If the player selected a move (pressed A), go to MENU_STATE_PRINT_TEACH_MOVE_PROMPT.
 *   - If the player cancelled (pressed B), go to MENU_STATE_PRINT_GIVE_UP_PROMPT.
 *   - If the player pressed left or right, swap the move display window to contest mode,
 *     and go to MENU_STATE_SETUP_CONTEST_MODE.
 *
 * DoQuestMenuMain: MENU_STATE_SETUP_CONTEST_MODE
 * DoQuestMenuMain: MENU_STATE_IDLE_CONTEST_MODE
 *   - If the player selected a move, go to MENU_STATE_PRINT_TEACH_MOVE_PROMPT.
 *   - If the player cancelled, go to MENU_STATE_PRINT_GIVE_UP_PROMPT
 *   - If the player pressed left or right, swap the move display window to battle mode,
 *     and go to MENU_STATE_SETUP_BATTLE_MODE.
 *
 * DoQuestMenuMain: MENU_STATE_PRINT_TEACH_MOVE_PROMPT
 * DoQuestMenuMain: MENU_STATE_TEACH_MOVE_CONFIRM
 *   - Wait for the player to confirm.
 *   - If cancelled, go to either MENU_STATE_SETUP_BATTLE_MODE or MENU_STATE_SETUP_CONTEST_MODE.
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
 *   - If the player cancels, go to either MENU_STATE_SETUP_BATTLE_MODE or
 *     MENU_STATE_SETUP_CONTEST_MODE.
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
#define MENU_STATE_SETUP_BATTLE_MODE 3
#define MENU_STATE_IDLE_BATTLE_MODE 4
#define MENU_STATE_SETUP_CONTEST_MODE 5
#define MENU_STATE_IDLE_CONTEST_MODE 6
// State 7 is skipped.
#define MENU_STATE_PRINT_TEACH_MOVE_PROMPT 8
#define MENU_STATE_TEACH_MOVE_CONFIRM 9
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

#define MAX_QUESTS_TO_SHOW (ROGUE_QUEST_COUNT)

extern const u8 gText_QuestLogBack[];

static EWRAM_DATA struct
{
    u8 state;
    u8 heartSpriteIds[16];                               /*0x001*/
    u16 questsToDisplay[MAX_QUESTS_TO_SHOW];               /*0x01A*/
    u8 partyMon;                                         /*0x044*/
    u8 moveSlot;                                         /*0x045*/
    struct ListMenuItem menuItems[MAX_QUESTS_TO_SHOW];  /*0x0E8*/
    u8 numMenuChoices;                                   /*0x110*/
    u8 numToShowAtOnce;                                  /*0x111*/
    u8 moveListMenuTask;                                 /*0x112*/
    u8 moveListScrollArrowTask;                          /*0x113*/
    u8 moveDisplayArrowTask;                             /*0x114*/
    u16 scrollOffset;                                    /*0x116*/
} *sQuestMenuStruct = {0};

static EWRAM_DATA struct {
    u16 listOffset;
    u16 listRow;
    bool8 showContestInfo;
} sQuestMenuMenuSate = {0};

static const u16 sQuestMenuPaletteData[] = INCBIN_U16("graphics/interface/ui_learn_move.gbapal");

// The arrow sprites in this spritesheet aren't used. The scroll-arrow system provides its own
// arrow sprites.
static const u8 sQuestMenuSpriteSheetData[] = INCBIN_U8("graphics/interface/ui_learn_move.4bpp");

static const struct OamData sHeartSpriteOamData =
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

static const struct SpriteTemplate sConstestMoveHeartSprite =
{
    .tileTag = 5525,
    .paletteTag = 5526,
    .oam = &sHeartSpriteOamData,
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
static void CreateQuestDisplayList(void);
static void CreateUISprites(void);
static void CB2_QuestMenuMain(void);
static void CB2_InitQuestMenu(void);
static void CB2_InitQuestMenuReturnFromSelectMove(void);
static void InitQuestMenuBackgroundLayers(void);
static void AddScrollArrows(void);
static void HandleInput(u8);
static void ShowTeachMoveText(u8);
static s32 GetCurrentSelectedMove(void);
static void FreeQuestMenuResources(void);
static void RemoveScrollArrows(void);
static void QuestMenuShowHideHearts(s32 moveId);
static void HideHeartSpritesAndShowTeachMoveText(bool8);

static void VBlankCB_QuestMenu(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void Rogue_OpenQuestMenu(RogueQuestMenuCallback callback)
{
    ScriptContext2_Enable();
    SetMainCallback2(CB2_InitQuestMenu);
    gFieldCallback = FieldCB_ContinueScriptHandleMusic;
}

static void GatherQuestsToDisplay()
{
    sQuestMenuStruct->numMenuChoices = 2;
    sQuestMenuStruct->questsToDisplay[0] = ROGUE_QUEST_Testing1;
    sQuestMenuStruct->questsToDisplay[1] = ROGUE_QUEST_Testing2;
}

static void CB2_InitQuestMenu(void)
{
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetTasks();
    ClearScheduledBgCopiesToVram();
    sQuestMenuStruct = AllocZeroed(sizeof(*sQuestMenuStruct));
    sQuestMenuStruct->partyMon = 0;
    SetVBlankCallback(VBlankCB_QuestMenu);

    InitQuestMenuBackgroundLayers();
    InitQuestMenuWindows(FALSE);

    sQuestMenuMenuSate.listOffset = 0;
    sQuestMenuMenuSate.listRow = 0;
    sQuestMenuMenuSate.showContestInfo = FALSE;

    CreateQuestDisplayList();

    LoadSpriteSheet(&sQuestMenuSpriteSheet);
    LoadSpritePalette(&sQuestMenuPalette);
    CreateUISprites();

    sQuestMenuStruct->moveListMenuTask = ListMenuInit(&gMultiuseListMenuTemplate, sQuestMenuMenuSate.listOffset, sQuestMenuMenuSate.listRow);
    FillPalette(RGB_BLACK, 0, 2);
    SetMainCallback2(CB2_QuestMenuMain);
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
    InitQuestMenuWindows(sQuestMenuMenuSate.showContestInfo);
    CreateQuestDisplayList();

    LoadSpriteSheet(&sQuestMenuSpriteSheet);
    LoadSpritePalette(&sQuestMenuPalette);
    CreateUISprites();

    sQuestMenuStruct->moveListMenuTask = ListMenuInit(&gMultiuseListMenuTemplate, sQuestMenuMenuSate.listOffset, sQuestMenuMenuSate.listRow);
    FillPalette(RGB_BLACK, 0, 2);
    SetMainCallback2(CB2_QuestMenuMain);
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

static void FormatAndPrintText(const u8 *src)
{
    StringExpandPlaceholders(gStringVar4, src);
    QuestMenuPrintText(gStringVar4);
}

// See the state machine doc at the top of the file.
static void DoQuestMenuMain(void)
{
    switch (sQuestMenuStruct->state)
    {
    case MENU_STATE_FADE_TO_BLACK:
        sQuestMenuStruct->state++;
        HideHeartSpritesAndShowTeachMoveText(FALSE);
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
    case MENU_STATE_SETUP_BATTLE_MODE:

        HideHeartSpritesAndShowTeachMoveText(FALSE);
        sQuestMenuStruct->state++;
        AddScrollArrows();
        break;
    case MENU_STATE_IDLE_BATTLE_MODE:
        HandleInput(FALSE);
        break;
    case MENU_STATE_SETUP_CONTEST_MODE:
        ShowTeachMoveText(FALSE);
        sQuestMenuStruct->state++;
        AddScrollArrows();
        break;
    case MENU_STATE_IDLE_CONTEST_MODE:
        HandleInput(TRUE);
        break;
    case MENU_STATE_PRINT_TEACH_MOVE_PROMPT:
        if (!QuestMenuRunTextPrinters())
        {
            QuestMenuCreateYesNoMenu();
            sQuestMenuStruct->state++;
        }
        break;
    case MENU_STATE_TEACH_MOVE_CONFIRM:
        {
            s8 selection = Menu_ProcessInputNoWrapClearOnChoose();

            if (selection == 0)
            {
                if (GiveMoveToMon(&gPlayerParty[sQuestMenuStruct->partyMon], GetCurrentSelectedMove()) != MON_HAS_MAX_MOVES)
                {
                    FormatAndPrintText(gText_MoveRelearnerPkmnLearnedMove);
                    gSpecialVar_0x8004 = TRUE;
                    sQuestMenuStruct->state = MENU_STATE_PRINT_TEXT_THEN_FANFARE;
                }
                else
                {
                    sQuestMenuStruct->state = MENU_STATE_PRINT_TRYING_TO_LEARN_PROMPT;
                }
            }
            else if (selection == MENU_B_PRESSED || selection == 1)
            {
                if (sQuestMenuMenuSate.showContestInfo == FALSE)
                {
                    sQuestMenuStruct->state = MENU_STATE_SETUP_BATTLE_MODE;
                }
                else if (sQuestMenuMenuSate.showContestInfo == TRUE)
                {
                    sQuestMenuStruct->state = MENU_STATE_SETUP_CONTEST_MODE;
                }
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
                if (sQuestMenuMenuSate.showContestInfo == FALSE)
                {
                    sQuestMenuStruct->state = MENU_STATE_SETUP_BATTLE_MODE;
                }
                else if (sQuestMenuMenuSate.showContestInfo == TRUE)
                {
                    sQuestMenuStruct->state = MENU_STATE_SETUP_CONTEST_MODE;
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
        StringCopy(gStringVar2, gMoveNames[GetCurrentSelectedMove()]);
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
                if (sQuestMenuMenuSate.showContestInfo == FALSE)
                {
                    sQuestMenuStruct->state = MENU_STATE_SETUP_BATTLE_MODE;
                }
                else if (sQuestMenuMenuSate.showContestInfo == TRUE)
                {
                    sQuestMenuStruct->state = MENU_STATE_SETUP_CONTEST_MODE;
                }
                sQuestMenuStruct->state = MENU_STATE_PRINT_TRYING_TO_LEARN_PROMPT;
            }
        }
        break;
    case MENU_STATE_CHOOSE_SETUP_STATE:
        if (!QuestMenuRunTextPrinters())
        {
            FillWindowPixelBuffer(3, 0x11);
            if (sQuestMenuMenuSate.showContestInfo == FALSE)
            {
                sQuestMenuStruct->state = MENU_STATE_SETUP_BATTLE_MODE;
            }
            else if (sQuestMenuMenuSate.showContestInfo == TRUE)
            {
                sQuestMenuStruct->state = MENU_STATE_SETUP_CONTEST_MODE;
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
            ShowSelectMovePokemonSummaryScreen(gPlayerParty, sQuestMenuStruct->partyMon, gPlayerPartyCount - 1, CB2_InitQuestMenuReturnFromSelectMove, GetCurrentSelectedMove());
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
        if (sQuestMenuMenuSate.showContestInfo == FALSE)
        {
            HideHeartSpritesAndShowTeachMoveText(TRUE);
        }
        else if (sQuestMenuMenuSate.showContestInfo == TRUE)
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
                SetMonMoveSlot(&gPlayerParty[sQuestMenuStruct->partyMon], GetCurrentSelectedMove(), sQuestMenuStruct->moveSlot);
                StringCopy(gStringVar2, gMoveNames[GetCurrentSelectedMove()]);
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
    DestroyListMenuTask(sQuestMenuStruct->moveListMenuTask, &sQuestMenuMenuSate.listOffset, &sQuestMenuMenuSate.listRow);
    FreeAllWindowBuffers();
    FREE_AND_SET_NULL(sQuestMenuStruct);
    ResetSpriteData();
    FreeAllSpritePalettes();
}

// Note: The hearts are already made invisible by QuestMenuShowHideHearts,
// which is called whenever the cursor in either list changes.
static void HideHeartSpritesAndShowTeachMoveText(bool8 onlyHideSprites)
{
    s32 i;

    for (i = 0; i < 16; i++)
    {
        gSprites[sQuestMenuStruct->heartSpriteIds[i]].invisible = TRUE;
    }

    if (!onlyHideSprites)
    {
        StringExpandPlaceholders(gStringVar4, gText_TeachWhichMoveToPkmn);
        FillWindowPixelBuffer(3, 0x11);
        AddTextPrinterParameterized(3, FONT_NORMAL, gStringVar4, 0, 1, 0, NULL);
    }
}

static void HandleInput(bool8 showContest)
{
    s32 itemId = ListMenu_ProcessInput(sQuestMenuStruct->moveListMenuTask);
    ListMenuGetScrollAndRow(sQuestMenuStruct->moveListMenuTask, &sQuestMenuMenuSate.listOffset, &sQuestMenuMenuSate.listRow);

    switch (itemId)
    {
    case LIST_NOTHING_CHOSEN:
        if (!(JOY_NEW(DPAD_LEFT | DPAD_RIGHT)) && !GetLRKeysPressed())
        {
            break;
        }

        PlaySE(SE_SELECT);

        if (showContest == FALSE)
        {
            PutWindowTilemap(1);
            sQuestMenuStruct->state = MENU_STATE_SETUP_CONTEST_MODE;
            sQuestMenuMenuSate.showContestInfo = TRUE;
        }
        else
        {
            PutWindowTilemap(0);
            sQuestMenuStruct->state = MENU_STATE_SETUP_BATTLE_MODE;
            sQuestMenuMenuSate.showContestInfo = FALSE;
        }

        ScheduleBgCopyTilemapToVram(1);
        QuestMenuShowHideHearts(GetCurrentSelectedMove());
        break;
    case LIST_CANCEL:
        PlaySE(SE_SELECT);
        RemoveScrollArrows();
        sQuestMenuStruct->state = MENU_STATE_FADE_AND_RETURN; //MENU_STATE_PRINT_GIVE_UP_PROMPT;
        StringExpandPlaceholders(gStringVar4, gText_MoveRelearnerGiveUp);
        QuestMenuPrintText(gStringVar4);
        break;
    default:
        PlaySE(SE_SELECT);
        RemoveScrollArrows();
        sQuestMenuStruct->state = MENU_STATE_PRINT_TEACH_MOVE_PROMPT;
        StringCopy(gStringVar2, gMoveNames[itemId]);
        StringExpandPlaceholders(gStringVar4, gText_MoveRelearnerTeachMoveConfirm);
        QuestMenuPrintText(gStringVar4);
        break;
    }
}

static s32 GetCurrentSelectedMove(void)
{
    return sQuestMenuStruct->menuItems[sQuestMenuMenuSate.listRow + sQuestMenuMenuSate.listOffset].id;
}

// Theory: This used to make the heart sprites visible again (i.e.
// this was the inverse of HideHeartsAndShowTeachMoveText), but the
// code was commented out. The bool argument would have been named
// "justShowHearts." The code for showing/hiding the heards was moved
// to QuestMenuShowHideHearts, which is called whenever a new move is
// selected and whenever the display mode changes.
static void ShowTeachMoveText(bool8 shouldDoNothingInstead)
{
    if (shouldDoNothingInstead == FALSE)
    {
        StringExpandPlaceholders(gStringVar4, gText_TeachWhichMoveToPkmn);
        FillWindowPixelBuffer(3, 0x11);
        AddTextPrinterParameterized(3, FONT_NORMAL, gStringVar4, 0, 1, 0, NULL);
    }
}

static void CreateUISprites(void)
{
    int i;

    sQuestMenuStruct->moveDisplayArrowTask = TASK_NONE;
    sQuestMenuStruct->moveListScrollArrowTask = TASK_NONE;
    AddScrollArrows();

    // These are the appeal hearts.
    for (i = 0; i < 8; i++)
    {
        sQuestMenuStruct->heartSpriteIds[i] = CreateSprite(&sConstestMoveHeartSprite, (i - (i / 4) * 4) * 8 + 104, (i / 4) * 8 + 36, 0);
    }

    // These are the jam harts.
    // The animation is used to toggle between full/empty heart sprites.
    for (i = 0; i < 8; i++)
    {
        sQuestMenuStruct->heartSpriteIds[i + 8] = CreateSprite(&sConstestMoveHeartSprite, (i - (i / 4) * 4) * 8 + 104, (i / 4) * 8 + 52, 0);
        StartSpriteAnim(&gSprites[sQuestMenuStruct->heartSpriteIds[i + 8]], 2);
    }

    for (i = 0; i < 16; i++)
    {
        gSprites[sQuestMenuStruct->heartSpriteIds[i]].invisible = TRUE;
    }
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

static void CreateQuestDisplayList(void)
{
    s32 i;
    u8 nickname[POKEMON_NAME_LENGTH + 1];

    GatherQuestsToDisplay();

    for (i = 0; i < sQuestMenuStruct->numMenuChoices; i++)
    {
        sQuestMenuStruct->menuItems[i].name = gRogueQuests[sQuestMenuStruct->questsToDisplay[i]].title;
        sQuestMenuStruct->menuItems[i].id = sQuestMenuStruct->questsToDisplay[i];
    }

    GetMonData(&gPlayerParty[sQuestMenuStruct->partyMon], MON_DATA_NICKNAME, nickname);
    StringCopy_Nickname(gStringVar1, nickname);
    sQuestMenuStruct->menuItems[sQuestMenuStruct->numMenuChoices].name = gText_QuestLogBack;
    sQuestMenuStruct->menuItems[sQuestMenuStruct->numMenuChoices].id = LIST_CANCEL;
    sQuestMenuStruct->numMenuChoices++;
    sQuestMenuStruct->numToShowAtOnce = LoadQuestMenuMovesList(sQuestMenuStruct->menuItems, sQuestMenuStruct->numMenuChoices);
}

static void QuestMenuShowHideHearts(s32 moveId)
{
    u16 numHearts;
    u16 i;

    if (!sQuestMenuMenuSate.showContestInfo || moveId == LIST_CANCEL)
    {
        for (i = 0; i < 16; i++)
        {
            gSprites[sQuestMenuStruct->heartSpriteIds[i]].invisible = TRUE;
        }
    }
    else
    {
        numHearts = (u8)(gContestEffects[gContestMoves[moveId].effect].appeal / 10);

        if (numHearts == 0xFF)
        {
            numHearts = 0;
        }

        for (i = 0; i < 8; i++)
        {
            if (i < numHearts)
            {
                StartSpriteAnim(&gSprites[sQuestMenuStruct->heartSpriteIds[i]], 1);
            }
            else
            {
                StartSpriteAnim(&gSprites[sQuestMenuStruct->heartSpriteIds[i]], 0);
            }
            gSprites[sQuestMenuStruct->heartSpriteIds[i]].invisible = FALSE;
        }

        numHearts = (u8)(gContestEffects[gContestMoves[moveId].effect].jam / 10);

        if (numHearts == 0xFF)
        {
            numHearts = 0;
        }

        for (i = 0; i < 8; i++)
        {
            if (i < numHearts)
            {
                StartSpriteAnim(&gSprites[sQuestMenuStruct->heartSpriteIds[i + 8]], 3);
            }
            else
            {
                StartSpriteAnim(&gSprites[sQuestMenuStruct->heartSpriteIds[i + 8]], 2);
            }
            gSprites[sQuestMenuStruct->heartSpriteIds[i + 8]].invisible = FALSE;
        }
    }
}
