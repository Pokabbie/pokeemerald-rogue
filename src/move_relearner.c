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
#include "money.h"
#include "overworld.h"
#include "palette.h"
#include "party_menu.h"
#include "pokemon_icon.h"
#include "pokemon_summary_screen.h"
#include "script.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings.h"
#include "international_string_util.h"
#include "task.h"
#include "constants/rgb.h"
#include "constants/songs.h"

#include "daycare.h"
#include "party_menu.h"

#include "rogue_baked.h"
#include "rogue_controller.h"

/*
 * Move relearner state machine
 * ------------------------
 *
 * Entry point: TeachMoveRelearnerMove
 *
 * TeachMoveRelearnerMove
 * Task_WaitForFadeOut
 * CB2_InitLearnMove
 *   - Creates moveDisplayArrowTask to listen to right/left buttons.
 *   - Creates moveListScrollArrowTask to listen to up/down buttons.
 *   - Whenever the selected move changes (and once on init), the MoveRelearnerCursorCallback
 *     is called (see sMoveRelearnerMovesListTemplate). That callback will reload the contest
 *     display and battle display windows for the new move. Both are always loaded in
 *     memory, but only the currently active one is copied to VRAM. The exception to this
 *     is the appeal and jam hearts, which are sprites. MoveRelearnerShowHideHearts is called
 *     while reloading the contest display to control them.
 * DoMoveRelearnerMain: MENU_STATE_FADE_TO_BLACK
 * DoMoveRelearnerMain: MENU_STATE_WAIT_FOR_FADE
 *   - Go to MENU_STATE_IDLE_BATTLE_MODE
 *
 * DoMoveRelearnerMain: MENU_STATE_SETUP_BATTLE_MODE
 * DoMoveRelearnerMain: MENU_STATE_IDLE_BATTLE_MODE
 *   - If the player selected a move (pressed A), go to MENU_STATE_PRINT_TEACH_MOVE_PROMPT.
 *   - If the player cancelled (pressed B), go to MENU_STATE_PRINT_GIVE_UP_PROMPT.
 *   - If the player pressed left or right, swap the move display window to contest mode,
 *     and go to MENU_STATE_SETUP_CONTEST_MODE.
 *
 * DoMoveRelearnerMain: MENU_STATE_SETUP_CONTEST_MODE
 * DoMoveRelearnerMain: MENU_STATE_IDLE_CONTEST_MODE
 *   - If the player selected a move, go to MENU_STATE_PRINT_TEACH_MOVE_PROMPT.
 *   - If the player cancelled, go to MENU_STATE_PRINT_GIVE_UP_PROMPT
 *   - If the player pressed left or right, swap the move display window to battle mode,
 *     and go to MENU_STATE_SETUP_BATTLE_MODE.
 *
 * DoMoveRelearnerMain: MENU_STATE_PRINT_TEACH_MOVE_PROMPT
 * DoMoveRelearnerMain: MENU_STATE_TEACH_MOVE_CONFIRM
 *   - Wait for the player to confirm.
 *   - If cancelled, go to either MENU_STATE_SETUP_BATTLE_MODE or MENU_STATE_SETUP_CONTEST_MODE.
 *   - If confirmed and the pokemon had an empty move slot, set VAR_0x8004 to TRUE and go to
 *     MENU_STATE_PRINT_TEXT_THEN_FANFARE.
 *   - If confirmed and the pokemon doesn't have an empty move slot, go to
 *     MENU_STATE_PRINT_TRYING_TO_LEARN_PROMPT.
 *
 * DoMoveRelearnerMain: MENU_STATE_PRINT_TRYING_TO_LEARN_PROMPT
 * DoMoveRelearnerMain: MENU_STATE_WAIT_FOR_TRYING_TO_LEARN
 * DoMoveRelearnerMain: MENU_STATE_CONFIRM_DELETE_OLD_MOVE
 *   - If the player confirms, go to MENU_STATE_PRINT_WHICH_MOVE_PROMPT.
 *   - If the player cancels, go to MENU_STATE_PRINT_STOP_TEACHING
 *
 * DoMoveRelearnerMain: MENU_STATE_PRINT_STOP_TEACHING
 * DoMoveRelearnerMain: MENU_STATE_WAIT_FOR_STOP_TEACHING
 * DoMoveRelearnerMain: MENU_STATE_CONFIRM_STOP_TEACHING
 *   - If the player confirms, go to MENU_STATE_CHOOSE_SETUP_STATE.
 *   - If the player cancels, go back to MENU_STATE_PRINT_TRYING_TO_LEARN_PROMPT.
 *
 * DoMoveRelearnerMain: MENU_STATE_PRINT_WHICH_MOVE_PROMPT
 * DoMoveRelearnerMain: MENU_STATE_SHOW_MOVE_SUMMARY_SCREEN
 *   - Go to ShowSelectMovePokemonSummaryScreen. When done, control returns to
 *     CB2_InitLearnMoveReturnFromSelectMove.
 *
 * DoMoveRelearnerMain: MENU_STATE_DOUBLE_FANFARE_FORGOT_MOVE
 * DoMoveRelearnerMain: MENU_STATE_PRINT_TEXT_THEN_FANFARE
 * DoMoveRelearnerMain: MENU_STATE_WAIT_FOR_FANFARE
 * DoMoveRelearnerMain: MENU_STATE_WAIT_FOR_A_BUTTON
 * DoMoveRelearnerMain: MENU_STATE_FADE_AND_RETURN
 * DoMoveRelearnerMain: MENU_STATE_RETURN_TO_FIELD
 *   - Clean up and go to CB2_ReturnToField.
 *
 * DoMoveRelearnerMain: MENU_STATE_PRINT_GIVE_UP_PROMPT
 * DoMoveRelearnerMain: MENU_STATE_GIVE_UP_CONFIRM
 *   - If the player confirms, go to MENU_STATE_FADE_AND_RETURN, and set VAR_0x8004 to FALSE.
 *   - If the player cancels, go to either MENU_STATE_SETUP_BATTLE_MODE or
 *     MENU_STATE_SETUP_CONTEST_MODE.
 *
 * CB2_InitLearnMoveReturnFromSelectMove:
 *   - Do most of the same stuff as CB2_InitLearnMove.
 * DoMoveRelearnerMain: MENU_STATE_FADE_FROM_SUMMARY_SCREEN
 * DoMoveRelearnerMain: MENU_STATE_TRY_OVERWRITE_MOVE
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

#define MAX_RELEARNER_MOVES 255

#define TEACH_STATE_RELEARN       0
#define TEACH_STATE_EGG_MOVES     1
#define TEACH_STATE_TUTOR_MOVES   2

static EWRAM_DATA struct
{
    u8 state;
    //u8 heartSpriteIds[16];
    u8 partySpriteIds[PARTY_SIZE];
    u8 typeSpriteIds[2];
    u16 movesToLearn[MAX_RELEARNER_MOVES];
    u8 partyMon;
    u8 moveSlot;
    struct ListMenuItem menuItems[MAX_RELEARNER_MOVES];  
    u8 numMenuChoices;
    u8 numMenuHiddenChoices;
    u8 numToShowAtOnce;
    u8 moveListMenuTask;
    u8 moveListScrollArrowTask;
    u8 moveDisplayArrowTask;
    u16 scrollOffset;
} *sMoveRelearnerStruct = {0};

static EWRAM_DATA struct {
    u16 listOffset;
    u16 listRow;
    bool8 showContestInfo;
    u8 teachMoveState;
    bool8 inPartyMenu : 1;
} sMoveRelearnerMenuSate = {0};

static const u16 sMoveRelearnerPaletteData[] = INCBIN_U16("graphics/interface/ui_learn_move.gbapal");

// The arrow sprites in this spritesheet aren't used. The scroll-arrow system provides its own
// arrow sprites.
static const u8 sMoveRelearnerSpriteSheetData[] = INCBIN_U8("graphics/interface/ui_learn_move.4bpp");

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

static const struct SpriteSheet sMoveRelearnerSpriteSheet =
{
    .data = sMoveRelearnerSpriteSheetData,
    .size = 0x180,
    .tag = 5525
};

static const struct SpritePalette sMoveRelearnerPalette =
{
    .data = sMoveRelearnerPaletteData,
    .tag = 5526
};

static const struct ScrollArrowsTemplate sDisplayModeArrowsTemplate =
{
    .firstArrowType = SCROLL_ARROW_LEFT,
    .firstX = 7,
    .firstY = 16,
    .secondArrowType = SCROLL_ARROW_RIGHT,
    .secondX = 137,
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

static const struct BgTemplate sMoveRelearnerMenuBackgroundTemplates[] =
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

void LoadMoveTypesSpritesheetAndPalette();
u8 CreateMonTypeIcon(u16 typeId, u8 x, u8 y);
void DestroyMonTypIcon(u8 spriteId);

#ifdef ROGUE_EXPANSION
void LoadMoveSplitSpritesheetAndPalette();
u8 CreateMoveSplitIcon(u32 split, u8 x, u8 y);
void DestroyMoveSplitIcon(u8 spriteId);
#endif

static void DoMoveRelearnerMain(void);
static void CreateLearnableMovesList(void);
static u8 LoadMoveRelearnerMovesList(const struct ListMenuItem *items, u16 numChoices);
static void MoveRelearnerCursorCallback(s32 itemIndex, bool8 onInit, struct ListMenu *list);
static void MoveRelearnerLoadBattleMoveDescription(u32 chosenMove);
static void MoveRelearnerMenuLoadContestMoveDescription(u32 chosenMove);
static void CreateUISprites(void);
static void CB2_MoveRelearnerMain(void);
static void Task_WaitForFadeOut(u8 taskId);
static void Task_WaitForFadeOutFromPartyMenu(u8 taskId);
static void CB2_InitLearnMove(void);
static void CB2_InitLearnMoveReturnFromSelectMove(void);
static void InitMoveRelearnerBackgroundLayers(void);
static void AddScrollArrows(void);
static void HandleInput(u8);
static void ShowTeachMoveText(u8);
static s32 GetCurrentSelectedMove(void);
static void FreeMoveRelearnerResources(void);
static void RemoveScrollArrows(void);
static void SetupDisplayedSprite(bool8);

static const struct ListMenuTemplate sMoveRelearnerMovesListTemplate =
{
    .items = NULL,
    .moveCursorFunc = MoveRelearnerCursorCallback,
    .itemPrintFunc = NULL,
    .totalItems = 0,
    .maxShowed = 0,
    .windowId = 2,
    .header_X = 0,
    .item_X = 8,
    .cursor_X = 0,
    .upText_Y = 1,
    .cursorPal = 2,
    .fillValue = 1,
    .cursorShadowPal = 3,
    .lettersSpacing = 0,
    .itemVerticalPadding = 0,
    .scrollMultiple = LIST_MULTIPLE_SCROLL_L_R,
    .fontId = FONT_NORMAL,
    .cursorKind = 0
};

static void VBlankCB_MoveRelearner(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void TeachMoveSetContextRelearnMove(void)
{
    sMoveRelearnerMenuSate.teachMoveState = TEACH_STATE_RELEARN;
}

void TeachMoveSetContextEggMove(void)
{
    sMoveRelearnerMenuSate.teachMoveState = TEACH_STATE_EGG_MOVES;
}

void TeachMoveSetContextTutorMove(void)
{
    sMoveRelearnerMenuSate.teachMoveState = TEACH_STATE_TUTOR_MOVES;
}

void TeachMoveFromContext(void)
{
    sMoveRelearnerMenuSate.inPartyMenu = FALSE;

    LockPlayerFieldControls();
    CreateTask(Task_WaitForFadeOut, 10);
    // Fade to black
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
}

void TeachMoveFromContextFromTask(u8 taskId)
{
    sMoveRelearnerMenuSate.inPartyMenu = TRUE;

    // no fade needed (expected to already be ready when this is called)
    gTasks[taskId].func = Task_WaitForFadeOutFromPartyMenu;
}

// Script arguments: The pokemon to teach is in VAR_0x8004
// RogueNote: legacy path
void TeachMoveRelearnerMove(void)
{
    sMoveRelearnerMenuSate.teachMoveState = TEACH_STATE_RELEARN;

    LockPlayerFieldControls();
    CreateTask(Task_WaitForFadeOut, 10);
    // Fade to black
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
}

static void GatherLearnableMoves(struct Pokemon* mon)
{
    if(sMoveRelearnerMenuSate.teachMoveState == TEACH_STATE_EGG_MOVES)
    {
        sMoveRelearnerStruct->numMenuChoices = GetEggMoves(mon, sMoveRelearnerStruct->movesToLearn);
    }
    else if(sMoveRelearnerMenuSate.teachMoveState == TEACH_STATE_TUTOR_MOVES)
    {
        sMoveRelearnerStruct->numMenuChoices = GetTutorMoves(mon, sMoveRelearnerStruct->movesToLearn, ARRAY_COUNT(sMoveRelearnerStruct->movesToLearn));
    }
    else // TEACH_STATE_RELEARN
    {
        u16 temp;
        u8 i;
        sMoveRelearnerStruct->numMenuChoices = GetMoveRelearnerMoves(mon, sMoveRelearnerStruct->movesToLearn);

        // Flip order, so the most recently learn moves are first
        for(i = 0; i <= sMoveRelearnerStruct->numMenuChoices / 2; ++i)
        {
            if(i < sMoveRelearnerStruct->numMenuChoices - 1 - i)
            {
                SWAP(sMoveRelearnerStruct->movesToLearn[i], sMoveRelearnerStruct->movesToLearn[sMoveRelearnerStruct->numMenuChoices - 1 - i], temp);
            }
        }
    }

    Rogue_ModifyTutorMoves(mon, sMoveRelearnerMenuSate.teachMoveState, &sMoveRelearnerStruct->numMenuChoices, &sMoveRelearnerStruct->numMenuHiddenChoices, sMoveRelearnerStruct->movesToLearn);
}

u8 GetNumberOfRelearnableMovesForContext(struct Pokemon* mon)
{
    if(sMoveRelearnerMenuSate.teachMoveState == TEACH_STATE_EGG_MOVES)
    {
        return sMoveRelearnerStruct->numMenuChoices = GetEggMoves(mon, sMoveRelearnerStruct->movesToLearn);
    }
    else if(sMoveRelearnerMenuSate.teachMoveState == TEACH_STATE_TUTOR_MOVES)
    {
        return sMoveRelearnerStruct->numMenuChoices = GetTutorMoves(mon, sMoveRelearnerStruct->movesToLearn, ARRAY_COUNT(sMoveRelearnerStruct->movesToLearn));
    }
    else // TEACH_STATE_RELEARN
    {
        return sMoveRelearnerStruct->numMenuChoices = GetMoveRelearnerMoves(mon, sMoveRelearnerStruct->movesToLearn);
    }
}

static bool8 DoesTeachingCostMoney()
{
    return !sMoveRelearnerMenuSate.inPartyMenu;
}

static u8 GetMoveStatsYOffset(u8 yEnabledOffset)
{
    // Move block higher up if we don't need the space for the cost
    return DoesTeachingCostMoney() ? yEnabledOffset : (yEnabledOffset - 8);
}

static bool8 HasEnoughMoneyToTeach(u16 chosenMove)
{
    u32 cost = Rogue_CalculateMovePrice(chosenMove);
    u32 playerMoney = GetMoney(&gSaveBlock1Ptr->money);
    return playerMoney >= cost;
}

static bool8 DoesTeachingMaintainPP()
{
    return sMoveRelearnerMenuSate.inPartyMenu;
}

static void Task_WaitForFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(CB2_InitLearnMove);
        gFieldCallback = FieldCB_ContinueScriptHandleMusic;
        DestroyTask(taskId);
    }
}

static void Task_WaitForFadeOutFromPartyMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(CB2_InitLearnMove);
        DestroyTask(taskId);
    }
}

static void CB2_InitLearnMove(void)
{
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetTasks();
    ClearScheduledBgCopiesToVram();
    sMoveRelearnerStruct = AllocZeroed(sizeof(*sMoveRelearnerStruct));
    sMoveRelearnerStruct->partyMon = gSpecialVar_0x8004;
    SetVBlankCallback(VBlankCB_MoveRelearner);

    gSpecialVar_0x8006 = FALSE;

    InitMoveRelearnerBackgroundLayers();
    InitMoveRelearnerWindows(FALSE);

    sMoveRelearnerMenuSate.listOffset = 0;
    sMoveRelearnerMenuSate.listRow = 0;
    sMoveRelearnerMenuSate.showContestInfo = FALSE;

    CreateLearnableMovesList();

    LoadSpriteSheet(&sMoveRelearnerSpriteSheet);
    LoadSpritePalette(&sMoveRelearnerPalette);
    LoadMonIconPalettes();
    LoadMoveTypesSpritesheetAndPalette();
#ifdef ROGUE_EXPANSION
    LoadMoveSplitSpritesheetAndPalette();
#endif
    CreateUISprites();

    // set via sMoveRelearnerMovesListTemplate
    sMoveRelearnerStruct->moveListMenuTask = ListMenuInit(&gMultiuseListMenuTemplate, sMoveRelearnerMenuSate.listOffset, sMoveRelearnerMenuSate.listRow);
    FillPalette(RGB_BLACK, 0, 2);
    SetMainCallback2(CB2_MoveRelearnerMain);
}

static void CB2_InitLearnMoveReturnFromSelectMove(void)
{
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetTasks();
    ClearScheduledBgCopiesToVram();
    sMoveRelearnerStruct = AllocZeroed(sizeof(*sMoveRelearnerStruct));
    sMoveRelearnerStruct->state = MENU_STATE_FADE_FROM_SUMMARY_SCREEN;
    sMoveRelearnerStruct->partyMon = gSpecialVar_0x8004;
    sMoveRelearnerStruct->moveSlot = gSpecialVar_0x8005;
    SetVBlankCallback(VBlankCB_MoveRelearner);

    gSpecialVar_0x8006 = FALSE;

    InitMoveRelearnerBackgroundLayers();
    InitMoveRelearnerWindows(sMoveRelearnerMenuSate.showContestInfo);
    CreateLearnableMovesList();

    LoadSpriteSheet(&sMoveRelearnerSpriteSheet);
    LoadSpritePalette(&sMoveRelearnerPalette);
    LoadMonIconPalettes();
    LoadMoveTypesSpritesheetAndPalette();
#ifdef ROGUE_EXPANSION
    LoadMoveSplitSpritesheetAndPalette();
#endif
    CreateUISprites();

    if(sMoveRelearnerStruct->moveSlot != MAX_MON_MOVES)
    {
        // We just taught a move so reset the list to the top
        //sMoveRelearnerMenuSate.listOffset = 0;
        //sMoveRelearnerMenuSate.listRow = 0;
    }

    // set via sMoveRelearnerMovesListTemplate
    sMoveRelearnerStruct->moveListMenuTask = ListMenuInit(&gMultiuseListMenuTemplate, sMoveRelearnerMenuSate.listOffset, sMoveRelearnerMenuSate.listRow);
    FillPalette(RGB_BLACK, 0, 2);
    SetMainCallback2(CB2_MoveRelearnerMain);
}

static void InitMoveRelearnerBackgroundLayers(void)
{
    ResetVramOamAndBgCntRegs();
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sMoveRelearnerMenuBackgroundTemplates, ARRAY_COUNT(sMoveRelearnerMenuBackgroundTemplates));
    ResetAllBgsCoordinates();
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 |
                                  DISPCNT_OBJ_1D_MAP |
                                  DISPCNT_OBJ_ON);
    ShowBg(0);
    ShowBg(1);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
}

static void CB2_MoveRelearnerMain(void)
{
    DoMoveRelearnerMain();
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void FormatAndPrintText(const u8 *src)
{
    StringExpandPlaceholders(gStringVar4, src);
    MoveRelearnerPrintText(gStringVar4);
}

static void BufferMonNickname(u8* str)
{
    u8 nickname[POKEMON_NAME_LENGTH + 1];
    GetMonData(&gPlayerParty[sMoveRelearnerStruct->partyMon], MON_DATA_NICKNAME, nickname);
    StringCopy_Nickname(str, nickname);
}

// See the state machine doc at the top of the file.
static void DoMoveRelearnerMain(void)
{
    switch (sMoveRelearnerStruct->state)
    {
    case MENU_STATE_FADE_TO_BLACK:
        sMoveRelearnerStruct->state++;
        SetupDisplayedSprite(FALSE);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        break;
    case MENU_STATE_WAIT_FOR_FADE:
        if (!gPaletteFade.active)
        {
            sMoveRelearnerStruct->state = MENU_STATE_IDLE_BATTLE_MODE;
        }
        break;
    case MENU_STATE_UNREACHABLE:
        sMoveRelearnerStruct->state++;
        break;
    case MENU_STATE_SETUP_BATTLE_MODE:

        SetupDisplayedSprite(FALSE);
        sMoveRelearnerStruct->state++;
        AddScrollArrows();
        break;
    case MENU_STATE_IDLE_BATTLE_MODE:
        HandleInput(FALSE);
        break;
    case MENU_STATE_SETUP_CONTEST_MODE:
        ShowTeachMoveText(FALSE);
        sMoveRelearnerStruct->state++;
        AddScrollArrows();
        break;
    case MENU_STATE_IDLE_CONTEST_MODE:
        HandleInput(TRUE);
        break;
    case MENU_STATE_PRINT_TEACH_MOVE_PROMPT:
        if (!MoveRelearnerRunTextPrinters())
        {
            if(DoesTeachingCostMoney() && !HasEnoughMoneyToTeach(GetCurrentSelectedMove()))
            {
                if (JOY_NEW(A_BUTTON | B_BUTTON | DPAD_ANY | R_BUTTON | L_BUTTON ))
                {
                    PlaySE(SE_SELECT);
                    sMoveRelearnerStruct->state = MENU_STATE_SETUP_BATTLE_MODE;
                }
            }
            else
            {
                MoveRelearnerCreateYesNoMenu();
                sMoveRelearnerStruct->state++;
            }
        }
        break;
    case MENU_STATE_TEACH_MOVE_CONFIRM:
        {
            s8 selection = Menu_ProcessInputNoWrapClearOnChoose();

            if (selection == 0)
            {
                if (GiveMoveToMon(&gPlayerParty[sMoveRelearnerStruct->partyMon], GetCurrentSelectedMove()) != MON_HAS_MAX_MOVES)
                {
                    BufferMonNickname(gStringVar1);
                    FormatAndPrintText(gText_MoveRelearnerPkmnLearnedMove);
                    gSpecialVar_0x8006 = TRUE;
                    sMoveRelearnerStruct->state = MENU_STATE_PRINT_TEXT_THEN_FANFARE;
                }
                else
                {
                    sMoveRelearnerStruct->state = MENU_STATE_PRINT_WHICH_MOVE_PROMPT;
                }
            }
            else if (selection == MENU_B_PRESSED || selection == 1)
            {
                if (sMoveRelearnerMenuSate.showContestInfo == FALSE)
                {
                    sMoveRelearnerStruct->state = MENU_STATE_SETUP_BATTLE_MODE;
                }
                else if (sMoveRelearnerMenuSate.showContestInfo == TRUE)
                {
                    sMoveRelearnerStruct->state = MENU_STATE_SETUP_CONTEST_MODE;
                }
            }
        }
        break;
    case MENU_STATE_PRINT_GIVE_UP_PROMPT:
        // Skip prompt
        sMoveRelearnerStruct->state = MENU_STATE_FADE_AND_RETURN;
        //if (!MoveRelearnerRunTextPrinters())
        //{
        //    MoveRelearnerCreateYesNoMenu();
        //    sMoveRelearnerStruct->state++;
        //}
        break;
    case MENU_STATE_GIVE_UP_CONFIRM:
        {
            s8 selection = Menu_ProcessInputNoWrapClearOnChoose();

            if (selection == 0)
            {
                gSpecialVar_0x8006 = FALSE;
                sMoveRelearnerStruct->state = MENU_STATE_FADE_AND_RETURN;
            }
            else if (selection == -1 || selection == 1)
            {
                if (sMoveRelearnerMenuSate.showContestInfo == FALSE)
                {
                    sMoveRelearnerStruct->state = MENU_STATE_SETUP_BATTLE_MODE;
                }
                else if (sMoveRelearnerMenuSate.showContestInfo == TRUE)
                {
                    sMoveRelearnerStruct->state = MENU_STATE_SETUP_CONTEST_MODE;
                }
            }
        }
        break;
    case MENU_STATE_PRINT_TRYING_TO_LEARN_PROMPT:
        FormatAndPrintText(gText_MoveRelearnerPkmnTryingToLearnMove);
        sMoveRelearnerStruct->state++;
        break;
    case MENU_STATE_WAIT_FOR_TRYING_TO_LEARN:
        if (!MoveRelearnerRunTextPrinters())
        {
            MoveRelearnerCreateYesNoMenu();
            sMoveRelearnerStruct->state = MENU_STATE_CONFIRM_DELETE_OLD_MOVE;
        }
        break;
    case MENU_STATE_CONFIRM_DELETE_OLD_MOVE:
        {
            s8 var = Menu_ProcessInputNoWrapClearOnChoose();

            if (var == 0)
            {
                FormatAndPrintText(gText_MoveRelearnerWhichMoveToForget);
                sMoveRelearnerStruct->state = MENU_STATE_PRINT_WHICH_MOVE_PROMPT;
            }
            else if (var == -1 || var == 1)
            {
                sMoveRelearnerStruct->state = MENU_STATE_PRINT_STOP_TEACHING;
            }
        }
        break;
    case MENU_STATE_PRINT_STOP_TEACHING:
        sMoveRelearnerStruct->state = MENU_STATE_CHOOSE_SETUP_STATE;
        //StringCopy(gStringVar2, gMoveNames[GetCurrentSelectedMove()]);
        //FormatAndPrintText(gText_MoveRelearnerStopTryingToTeachMove);
        //sMoveRelearnerStruct->state++;
        break;
    case MENU_STATE_WAIT_FOR_STOP_TEACHING:
        if (!MoveRelearnerRunTextPrinters())
        {
            MoveRelearnerCreateYesNoMenu();
            sMoveRelearnerStruct->state++;
        }
        break;
    case MENU_STATE_CONFIRM_STOP_TEACHING:
        {
            s8 var = Menu_ProcessInputNoWrapClearOnChoose();

            if (var == 0)
            {
                sMoveRelearnerStruct->state = MENU_STATE_CHOOSE_SETUP_STATE;
            }
            else if (var == MENU_B_PRESSED || var == 1)
            {
                // What's the point? It gets set to MENU_STATE_PRINT_TRYING_TO_LEARN_PROMPT, anyway.
                if (sMoveRelearnerMenuSate.showContestInfo == FALSE)
                {
                    sMoveRelearnerStruct->state = MENU_STATE_SETUP_BATTLE_MODE;
                }
                else if (sMoveRelearnerMenuSate.showContestInfo == TRUE)
                {
                    sMoveRelearnerStruct->state = MENU_STATE_SETUP_CONTEST_MODE;
                }
                sMoveRelearnerStruct->state = MENU_STATE_PRINT_WHICH_MOVE_PROMPT; // RogueNote: this may not be correct?
            }
        }
        break;
    case MENU_STATE_CHOOSE_SETUP_STATE:
        if (!MoveRelearnerRunTextPrinters())
        {
            FillWindowPixelBuffer(3, 0x11);
            if (sMoveRelearnerMenuSate.showContestInfo == FALSE)
            {
                sMoveRelearnerStruct->state = MENU_STATE_SETUP_BATTLE_MODE;
            }
            else if (sMoveRelearnerMenuSate.showContestInfo == TRUE)
            {
                sMoveRelearnerStruct->state = MENU_STATE_SETUP_CONTEST_MODE;
            }
        }
        break;
    case MENU_STATE_PRINT_WHICH_MOVE_PROMPT:
        if (!MoveRelearnerRunTextPrinters())
        {
            sMoveRelearnerStruct->state = MENU_STATE_SHOW_MOVE_SUMMARY_SCREEN;
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        }
        break;
    case MENU_STATE_SHOW_MOVE_SUMMARY_SCREEN:
        if (!gPaletteFade.active)
        {
            // Remember mon to return to
            gSpecialVar_0x8004 = sMoveRelearnerStruct->partyMon;
            ShowSelectMovePokemonSummaryScreen(gPlayerParty, sMoveRelearnerStruct->partyMon, gPlayerPartyCount - 1, CB2_InitLearnMoveReturnFromSelectMove, GetCurrentSelectedMove());
            FreeMoveRelearnerResources();
        }
        break;
    case 21:
        if (!MoveRelearnerRunTextPrinters())
        {
            sMoveRelearnerStruct->state = MENU_STATE_FADE_AND_RETURN;
        }
        break;
    case 22:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        break;
    case MENU_STATE_FADE_AND_RETURN:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        sMoveRelearnerStruct->state++;
        break;
    case MENU_STATE_RETURN_TO_FIELD:
        if (!gPaletteFade.active)
        {
            FreeMoveRelearnerResources();

            if(sMoveRelearnerMenuSate.inPartyMenu)
            {
                ReturnToPartyMenuSubMenu();
            }
            else
            {
                SetMainCallback2(CB2_ReturnToField);
            }
        }
        break;
    case MENU_STATE_FADE_FROM_SUMMARY_SCREEN:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        sMoveRelearnerStruct->state++;
        if (sMoveRelearnerMenuSate.showContestInfo == FALSE)
        {
            SetupDisplayedSprite(TRUE);
        }
        else if (sMoveRelearnerMenuSate.showContestInfo == TRUE)
        {
            ShowTeachMoveText(TRUE);
        }
        RemoveScrollArrows();
        CopyWindowToVram(3, COPYWIN_GFX);
        break;
    case MENU_STATE_TRY_OVERWRITE_MOVE:
        if (!gPaletteFade.active)
        {
            if (sMoveRelearnerStruct->moveSlot == MAX_MON_MOVES)
            {
                sMoveRelearnerStruct->state = MENU_STATE_PRINT_STOP_TEACHING;
            }
            else
            {
                u32 startPpPerc;
                u16 moveId = GetMonData(&gPlayerParty[sMoveRelearnerStruct->partyMon], MON_DATA_MOVE1 + sMoveRelearnerStruct->moveSlot);

                // Log PP
                if(DoesTeachingMaintainPP())
                {
                    startPpPerc = GetMonData(&gPlayerParty[sMoveRelearnerStruct->partyMon], MON_DATA_PP1 + sMoveRelearnerStruct->moveSlot);
                    startPpPerc = (startPpPerc * 100) / (u32)gBattleMoves[moveId].pp;
                }

                StringCopy(gStringVar3, gMoveNames[moveId]);
                RemoveMonPPBonus(&gPlayerParty[sMoveRelearnerStruct->partyMon], sMoveRelearnerStruct->moveSlot);
                SetMonMoveSlot(&gPlayerParty[sMoveRelearnerStruct->partyMon], GetCurrentSelectedMove(), sMoveRelearnerStruct->moveSlot);

                // Maintain PP %
                if(DoesTeachingMaintainPP())
                {
                    startPpPerc = (startPpPerc * (u32)gBattleMoves[GetCurrentSelectedMove()].pp) / 100;
                    SetMonData(&gPlayerParty[sMoveRelearnerStruct->partyMon], MON_DATA_PP1 + sMoveRelearnerStruct->moveSlot, &startPpPerc);
                }

                StringCopy(gStringVar2, gMoveNames[GetCurrentSelectedMove()]);
                FormatAndPrintText(gText_MoveRelearnerAndPoof);
                sMoveRelearnerStruct->state = MENU_STATE_DOUBLE_FANFARE_FORGOT_MOVE;
                gSpecialVar_0x8006 = TRUE;
            }
        }
        break;
    case MENU_STATE_DOUBLE_FANFARE_FORGOT_MOVE:
        if (!MoveRelearnerRunTextPrinters())
        {
            BufferMonNickname(gStringVar1);
            FormatAndPrintText(gText_MoveRelearnerPkmnForgotMoveAndLearnedNew);
            sMoveRelearnerStruct->state = MENU_STATE_PRINT_TEXT_THEN_FANFARE;
            //PlayFanfare(MUS_LEVEL_UP);
        }
        break;
    case MENU_STATE_PRINT_TEXT_THEN_FANFARE:
        if (!MoveRelearnerRunTextPrinters())
        {
            //PlayFanfare(MUS_LEVEL_UP);
            sMoveRelearnerStruct->state = MENU_STATE_WAIT_FOR_FANFARE;
        }
        break;
    case MENU_STATE_WAIT_FOR_FANFARE:
        if (IsFanfareTaskInactive())
        {
            sMoveRelearnerStruct->state = MENU_STATE_WAIT_FOR_A_BUTTON;
        }
        break;
    case MENU_STATE_WAIT_FOR_A_BUTTON:
        if (JOY_NEW(A_BUTTON))
        {
            if(DoesTeachingCostMoney())
            {
                u32 cost = Rogue_CalculateMovePrice(GetCurrentSelectedMove());
                RemoveMoney(&gSaveBlock1Ptr->money, cost);
                PlaySE(SE_VEND);
            }
            else
            {
                PlaySE(SE_SELECT);
            }

            // Recreate list to remove move we just taught
            CreateLearnableMovesList();
            DestroyListMenuTask(sMoveRelearnerStruct->moveListMenuTask, &sMoveRelearnerMenuSate.listOffset, &sMoveRelearnerMenuSate.listRow);

            if(sMoveRelearnerMenuSate.listOffset != 0)
                --sMoveRelearnerMenuSate.listOffset;
            //sMoveRelearnerMenuSate.listOffset = 0;
            //sMoveRelearnerMenuSate.listRow = 0;
            sMoveRelearnerStruct->moveListMenuTask = ListMenuInit(&gMultiuseListMenuTemplate, sMoveRelearnerMenuSate.listOffset, sMoveRelearnerMenuSate.listRow);
            
            // Reshow move screen
            sMoveRelearnerStruct->state = MENU_STATE_PRINT_STOP_TEACHING;
            //sMoveRelearnerStruct->state = MENU_STATE_FADE_AND_RETURN;
        }
        break;
    }
}

static void FreeMoveRelearnerResources(void)
{
    RemoveScrollArrows();
    DestroyListMenuTask(sMoveRelearnerStruct->moveListMenuTask, &sMoveRelearnerMenuSate.listOffset, &sMoveRelearnerMenuSate.listRow);
    FreeAllWindowBuffers();
    FREE_AND_SET_NULL(sMoveRelearnerStruct);
    ResetSpriteData();
    FreeAllSpritePalettes();
}

// Note: The hearts are already made invisible by MoveRelearnerShowHideHearts,
// which is called whenever the cursor in either list changes.
static void SetupDisplayedSprite(bool8 onlyHideSprites)
{
    s32 i;

    //for (i = 0; i < 16; i++)
    //{
    //    gSprites[sMoveRelearnerStruct->heartSpriteIds[i]].invisible = TRUE;
    //}

    if (!onlyHideSprites)
    {
        BufferMonNickname(gStringVar1);
        StringExpandPlaceholders(gStringVar4, gText_TeachWhichMoveToPkmn);
        FillWindowPixelBuffer(3, 0x11);
        AddTextPrinterParameterized(3, FONT_NORMAL, gStringVar4, 0, 1, 0, NULL);
    }
}

static void HandleInput(bool8 showContest)
{
    s32 itemId = ListMenu_ProcessInput(sMoveRelearnerStruct->moveListMenuTask);
    ListMenuGetScrollAndRow(sMoveRelearnerStruct->moveListMenuTask, &sMoveRelearnerMenuSate.listOffset, &sMoveRelearnerMenuSate.listRow);

    switch (itemId)
    {
    case LIST_NOTHING_CHOSEN:
        if (!(JOY_NEW(DPAD_LEFT | DPAD_RIGHT)) && !GetLRKeysPressed())
        {
            break;
        }

        // Cycle through mons
        PlaySE(SE_SELECT);
        sMoveRelearnerStruct->state = MENU_STATE_SETUP_BATTLE_MODE;
        sMoveRelearnerMenuSate.showContestInfo = FALSE;

        if(JOY_NEW(DPAD_RIGHT))
        {
            sMoveRelearnerStruct->partyMon = (sMoveRelearnerStruct->partyMon + 1) % gPlayerPartyCount;
        }
        else // JOY_NEW(DPAD_LEFT)
        {
            if(sMoveRelearnerStruct->partyMon == 0)
                sMoveRelearnerStruct->partyMon = gPlayerPartyCount - 1;
            else
                --sMoveRelearnerStruct->partyMon;
        }

        CreateLearnableMovesList();
        DestroyListMenuTask(sMoveRelearnerStruct->moveListMenuTask, &sMoveRelearnerMenuSate.listOffset, &sMoveRelearnerMenuSate.listRow);
        sMoveRelearnerMenuSate.listOffset = 0;
        sMoveRelearnerMenuSate.listRow = 0;
        sMoveRelearnerStruct->moveListMenuTask = ListMenuInit(&gMultiuseListMenuTemplate, sMoveRelearnerMenuSate.listOffset, sMoveRelearnerMenuSate.listRow);

        //MoveRelearnerShowHideHearts(GetCurrentSelectedMove());
        //MoveRelearnerLoadBattleMoveDescription(GetCurrentSelectedMove());

        PutWindowTilemap(0);
        ScheduleBgCopyTilemapToVram(1);
        break;

    case LIST_CANCEL:
        PlaySE(SE_SELECT);
        RemoveScrollArrows();
        sMoveRelearnerStruct->state = MENU_STATE_PRINT_GIVE_UP_PROMPT;
        StringExpandPlaceholders(gStringVar4, gText_MoveRelearnerGiveUp);
        MoveRelearnerPrintText(gStringVar4);
        break;

    default:
        if(DoesTeachingCostMoney() && !HasEnoughMoneyToTeach(itemId))
        {
            PlaySE(SE_FAILURE);
            StringCopy(gStringVar4, gText_MoveRelearnerNotEnoughMoney);
            MoveRelearnerPrintText(gStringVar4);
            sMoveRelearnerStruct->state = MENU_STATE_PRINT_TEACH_MOVE_PROMPT;
        }
        else
        {
            PlaySE(SE_SELECT);
            RemoveScrollArrows();
            sMoveRelearnerStruct->state = MENU_STATE_PRINT_TEACH_MOVE_PROMPT;
            StringCopy(gStringVar2, gMoveNames[itemId]);
            StringExpandPlaceholders(gStringVar4, gText_MoveRelearnerTeachMoveConfirm);
            MoveRelearnerPrintText(gStringVar4);
        }
        break;
    }
}

static s32 GetCurrentSelectedMove(void)
{
    return sMoveRelearnerStruct->menuItems[sMoveRelearnerMenuSate.listRow + sMoveRelearnerMenuSate.listOffset].id;
}

// Theory: This used to make the heart sprites visible again (i.e.
// this was the inverse of HideHeartsAndShowTeachMoveText), but the
// code was commented out. The bool argument would have been named
// "justShowHearts." The code for showing/hiding the heards was moved
// to MoveRelearnerShowHideHearts, which is called whenever a new move is
// selected and whenever the display mode changes.
static void ShowTeachMoveText(bool8 shouldDoNothingInstead)
{
    if (shouldDoNothingInstead == FALSE)
    {
        BufferMonNickname(gStringVar1);
        StringExpandPlaceholders(gStringVar4, gText_TeachWhichMoveToPkmn);
        FillWindowPixelBuffer(3, 0x11);
        AddTextPrinterParameterized(3, FONT_NORMAL, gStringVar4, 0, 1, 0, NULL);
    }
}

static void CreateUISprites(void)
{
    int i;

    sMoveRelearnerStruct->moveDisplayArrowTask = TASK_NONE;
    sMoveRelearnerStruct->moveListScrollArrowTask = TASK_NONE;
    AddScrollArrows();

    sMoveRelearnerStruct->typeSpriteIds[0] = SPRITE_NONE;
    sMoveRelearnerStruct->typeSpriteIds[1] = SPRITE_NONE;

    for(i = 0; i < PARTY_SIZE; ++i)
    {
        if(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE)
        {
            sMoveRelearnerStruct->partySpriteIds[i] = CreateMonIcon(
                GetMonData(&gPlayerParty[i], MON_DATA_SPECIES), 
                SpriteCB_MonIcon, 
                28, 16, 0, 
                GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY), 
                TRUE
            );

            gSprites[sMoveRelearnerStruct->partySpriteIds[i]].invisible = TRUE;
        }
        else
        {
            sMoveRelearnerStruct->partySpriteIds[i] = SPRITE_NONE;
        }
    }

    // These are the appeal hearts.
    //for (i = 0; i < 8; i++)
    //{
    //    sMoveRelearnerStruct->heartSpriteIds[i] = CreateSprite(&sConstestMoveHeartSprite, (i - (i / 4) * 4) * 8 + 104, (i / 4) * 8 + 36, 0);
    //}
//
    //// These are the jam harts.
    //// The animation is used to toggle between full/empty heart sprites.
    //for (i = 0; i < 8; i++)
    //{
    //    sMoveRelearnerStruct->heartSpriteIds[i + 8] = CreateSprite(&sConstestMoveHeartSprite, (i - (i / 4) * 4) * 8 + 104, (i / 4) * 8 + 52, 0);
    //    StartSpriteAnim(&gSprites[sMoveRelearnerStruct->heartSpriteIds[i + 8]], 2);
    //}
//
    //for (i = 0; i < 16; i++)
    //{
    //    gSprites[sMoveRelearnerStruct->heartSpriteIds[i]].invisible = TRUE;
    //}
}

static void AddScrollArrows(void)
{
    if (sMoveRelearnerStruct->moveDisplayArrowTask == TASK_NONE)
    {
        sMoveRelearnerStruct->moveDisplayArrowTask = AddScrollIndicatorArrowPair(&sDisplayModeArrowsTemplate, &sMoveRelearnerStruct->scrollOffset);
    }

    if (sMoveRelearnerStruct->moveListScrollArrowTask == TASK_NONE)
    {
        gTempScrollArrowTemplate = sMoveListScrollArrowsTemplate;
        gTempScrollArrowTemplate.fullyDownThreshold = sMoveRelearnerStruct->numMenuChoices - sMoveRelearnerStruct->numToShowAtOnce;
        sMoveRelearnerStruct->moveListScrollArrowTask = AddScrollIndicatorArrowPair(&gTempScrollArrowTemplate, &sMoveRelearnerMenuSate.listOffset);
    }
}

static void RemoveScrollArrows(void)
{
    if (sMoveRelearnerStruct->moveDisplayArrowTask != TASK_NONE)
    {
        RemoveScrollIndicatorArrowPair(sMoveRelearnerStruct->moveDisplayArrowTask);
        sMoveRelearnerStruct->moveDisplayArrowTask = TASK_NONE;
    }

    if (sMoveRelearnerStruct->moveListScrollArrowTask != TASK_NONE)
    {
        RemoveScrollIndicatorArrowPair(sMoveRelearnerStruct->moveListScrollArrowTask);
        sMoveRelearnerStruct->moveListScrollArrowTask = TASK_NONE;
    }
}

static void CreateLearnableMovesList(void)
{
    s32 i;

    GatherLearnableMoves(&gPlayerParty[sMoveRelearnerStruct->partyMon]);

    for (i = 0; i < sMoveRelearnerStruct->numMenuChoices; i++)
    {
        sMoveRelearnerStruct->menuItems[i].name = gMoveNames[sMoveRelearnerStruct->movesToLearn[i]];
        sMoveRelearnerStruct->menuItems[i].id = sMoveRelearnerStruct->movesToLearn[i];
    }

    BufferMonNickname(gStringVar1);

    if(sMoveRelearnerStruct->numMenuHiddenChoices != 0)
    {
        // STR_VARs are used heavily here so it's easiest to just hard code these
        u8 index;
        const u8* const hiddenMoveTexts[] = 
        {
            gText_HiddenMoves1,
            gText_HiddenMoves2,
            gText_HiddenMoves3,
            gText_HiddenMoves4,
            gText_HiddenMoves5,
            gText_HiddenMoves6,
            gText_HiddenMoves7,
            gText_HiddenMoves8,
            gText_HiddenMoves9,
            gText_HiddenMoves10,
            gText_HiddenMoves11,
            gText_HiddenMoves12,
            gText_HiddenMoves13,
            gText_HiddenMoves14,
            gText_HiddenMoves15,
            gText_HiddenMoves16,
            gText_HiddenMoves17,
            gText_HiddenMoves18,
            gText_HiddenMoves19,
            gText_HiddenMoves20,
            gText_HiddenMoves20plus,
        };

        index = min(sMoveRelearnerStruct->numMenuHiddenChoices - 1, ARRAY_COUNT(hiddenMoveTexts) - 1);

        sMoveRelearnerStruct->menuItems[sMoveRelearnerStruct->numMenuChoices].name = hiddenMoveTexts[index];
        sMoveRelearnerStruct->menuItems[sMoveRelearnerStruct->numMenuChoices].id = LIST_CANCEL;
        sMoveRelearnerStruct->numMenuChoices++;
    }

    sMoveRelearnerStruct->menuItems[sMoveRelearnerStruct->numMenuChoices].name = gText_Cancel;
    sMoveRelearnerStruct->menuItems[sMoveRelearnerStruct->numMenuChoices].id = LIST_CANCEL;
    sMoveRelearnerStruct->numMenuChoices++;
    sMoveRelearnerStruct->numToShowAtOnce = LoadMoveRelearnerMovesList(sMoveRelearnerStruct->menuItems, sMoveRelearnerStruct->numMenuChoices);
}

static u8 LoadMoveRelearnerMovesList(const struct ListMenuItem *items, u16 numChoices)
{
    gMultiuseListMenuTemplate = sMoveRelearnerMovesListTemplate;
    gMultiuseListMenuTemplate.totalItems = numChoices;
    gMultiuseListMenuTemplate.items = items;

    if (numChoices < 6)
        gMultiuseListMenuTemplate.maxShowed = numChoices;
    else
        gMultiuseListMenuTemplate.maxShowed = 6;

    return gMultiuseListMenuTemplate.maxShowed;
}

static void MoveRelearnerCursorCallback(s32 itemIndex, bool8 onInit, struct ListMenu *list)
{
    if (onInit != TRUE)
        PlaySE(SE_SELECT);
    MoveRelearnerLoadBattleMoveDescription(itemIndex);
    MoveRelearnerMenuLoadContestMoveDescription(itemIndex);
}

static void MoveRelearnerLoadBattleMoveDescription(u32 chosenMove)
{
    s32 x;
    const struct BattleMove *move;
    u8 buffer[32];
    const u8 *str;
    u8 const yStatHeight = GetMoveStatsYOffset(38); // 24
    u8 const yPriceHeight = 24; // 52
    u8 const yDescHeight = 65;

    FillWindowPixelBuffer(0, PIXEL_FILL(1));

    // Print mon name
    //str = gText_MoveRelearnerBattleMoves;
    BufferMonNickname(gStringVar1);
    x = GetStringCenterAlignXOffset(FONT_NORMAL, gStringVar1, 128);
    AddTextPrinterParameterized(0, FONT_NORMAL, gStringVar1, x, 1, TEXT_SKIP_DRAW, NULL);

    if (chosenMove == LIST_CANCEL)
    {
        CopyWindowToVram(0, COPYWIN_GFX);
        return;
    }

    // Type
    move = &gBattleMoves[chosenMove];
    //str = gTypeNames[move->type];
    //AddTextPrinterParameterized(0, FONT_NORMAL, str, 4, 25, TEXT_SKIP_DRAW, NULL);

    // PP
    x = 4;
    AddTextPrinterParameterized(0, FONT_NORMAL, gText_MoveRelearnerPP, x, yStatHeight, TEXT_SKIP_DRAW, NULL);

    x += GetStringWidth(FONT_NORMAL, gText_MoveRelearnerPP, 0);
    ConvertIntToDecimalStringN(buffer, move->pp, STR_CONV_MODE_LEFT_ALIGN, 2);
    AddTextPrinterParameterized(0, FONT_NORMAL, buffer, x, yStatHeight, TEXT_SKIP_DRAW, NULL);

    // Power
    x = 40;
    AddTextPrinterParameterized(0, FONT_NORMAL, gText_MoveRelearnerPower, x, yStatHeight, TEXT_SKIP_DRAW, NULL);

    if (move->power < 2)
    {
        str = gText_ThreeDashes;
    }
    else
    {
        ConvertIntToDecimalStringN(buffer, move->power, STR_CONV_MODE_LEFT_ALIGN, 3);
        str = buffer;
    }
    x += GetStringWidth(FONT_NORMAL, gText_MoveRelearnerPower, 0);
    AddTextPrinterParameterized(0, FONT_NORMAL, str, x, yStatHeight, TEXT_SKIP_DRAW, NULL);

    // Accuracy
    x = 85;
    AddTextPrinterParameterized(0, FONT_NORMAL, gText_MoveRelearnerAccuracy, x, yStatHeight, TEXT_SKIP_DRAW, NULL);

    if (move->accuracy == 0)
    {
        str = gText_ThreeDashes;
    }
    else
    {
        ConvertIntToDecimalStringN(buffer, move->accuracy, STR_CONV_MODE_LEFT_ALIGN, 3);
        str = buffer;
    }
    x += GetStringWidth(FONT_NORMAL, gText_MoveRelearnerAccuracy, 0);
    AddTextPrinterParameterized(0, FONT_NORMAL, str, x, yStatHeight, TEXT_SKIP_DRAW, NULL);

    // Price
    if(DoesTeachingCostMoney())
    {
        u32 cost = Rogue_CalculateMovePrice(chosenMove);
        u32 playerMoney = GetMoney(&gSaveBlock1Ptr->money);

        ConvertIntToDecimalStringN(gStringVar1, cost, STR_CONV_MODE_LEFT_ALIGN, 6);
        ConvertIntToDecimalStringN(gStringVar2, playerMoney, STR_CONV_MODE_LEFT_ALIGN, 6);
        StringExpandPlaceholders(gStringVar4, (playerMoney >= cost) ? gText_MoveRelearnerCostCanBuy : gText_MoveRelearnerCostCannotBuy);
        x = 0;
        AddTextPrinterParameterized(0, FONT_SMALL_NARROW, gStringVar4, x, yPriceHeight, TEXT_SKIP_DRAW, NULL);
    }


    // Description
    str = gMoveDescriptionPointers[chosenMove - 1];
    AddTextPrinterParameterized(0, FONT_NARROW, str, 0, yDescHeight, 0, NULL);
}

static void MoveRelearnerMenuLoadContestMoveDescription(u32 chosenMove)
{
    s32 x;
    const u8 *str;
    const struct ContestMove *move;

    MoveRelearnerShowHideHearts(chosenMove);
    FillWindowPixelBuffer(1, PIXEL_FILL(1));
    str = gText_MoveRelearnerContestMovesTitle;
    x = GetStringCenterAlignXOffset(FONT_NORMAL, str, 0x80);
    AddTextPrinterParameterized(1, FONT_NORMAL, str, x, 1, TEXT_SKIP_DRAW, NULL);

    str = gText_MoveRelearnerAppeal;
    x = GetStringRightAlignXOffset(FONT_NORMAL, str, 0x5C);
    AddTextPrinterParameterized(1, FONT_NORMAL, str, x, 0x19, TEXT_SKIP_DRAW, NULL);

    str = gText_MoveRelearnerJam;
    x = GetStringRightAlignXOffset(FONT_NORMAL, str, 0x5C);
    AddTextPrinterParameterized(1, FONT_NORMAL, str, x, 0x29, TEXT_SKIP_DRAW, NULL);

    if (chosenMove == MENU_NOTHING_CHOSEN)
    {
        CopyWindowToVram(1, COPYWIN_GFX);
        return;
    }

    move = &gContestMoves[chosenMove];
    str = gContestMoveTypeTextPointers[move->contestCategory];
    AddTextPrinterParameterized(1, FONT_NORMAL, str, 4, 0x19, TEXT_SKIP_DRAW, NULL);

    str = gContestEffectDescriptionPointers[move->effect];
    AddTextPrinterParameterized(1, FONT_NARROW, str, 0, 0x41, TEXT_SKIP_DRAW, NULL);

    CopyWindowToVram(1, COPYWIN_GFX);
}

void MoveRelearnerShowHideHearts(s32 moveId)
{
    u8 i;
    
    for(i = 0; i < PARTY_SIZE; ++i)
    {
        if(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE)
        {
            gSprites[sMoveRelearnerStruct->partySpriteIds[i]].invisible = (sMoveRelearnerStruct->partyMon != i);
        }
    }

    // Create type sprites
    if(sMoveRelearnerStruct->typeSpriteIds[0] != SPRITE_NONE)
    {
        DestroyMonTypIcon(sMoveRelearnerStruct->typeSpriteIds[0]);
        sMoveRelearnerStruct->typeSpriteIds[0] = SPRITE_NONE;
    }

#ifdef ROGUE_EXPANSION
    if(sMoveRelearnerStruct->typeSpriteIds[1] != SPRITE_NONE)
    {
        DestroyMoveSplitIcon(sMoveRelearnerStruct->typeSpriteIds[1]);
        sMoveRelearnerStruct->typeSpriteIds[1] = SPRITE_NONE;
    }
#endif

    if(moveId != LIST_CANCEL)
    {
        u8 type = gBattleMoves[moveId].type;

        if(moveId == MOVE_HIDDEN_POWER)
            type = CalcMonHiddenPowerType(&gPlayerParty[sMoveRelearnerStruct->partyMon]);

        sMoveRelearnerStruct->typeSpriteIds[0] = CreateMonTypeIcon(type, 12, GetMoveStatsYOffset(59));
#ifdef ROGUE_EXPANSION
        sMoveRelearnerStruct->typeSpriteIds[1] = CreateMoveSplitIcon(gBattleMoves[moveId].split, 44, GetMoveStatsYOffset(59));
#endif
    }

    //sMoveRelearnerStruct->typeSpriteIds[1] = SPRITE_NONE;

    //u16 numHearts;
    //u16 i;

    //if (!sMoveRelearnerMenuSate.showContestInfo || moveId == LIST_CANCEL)
    //{
    //    for (i = 0; i < 16; i++)
    //    {
    //        gSprites[sMoveRelearnerStruct->heartSpriteIds[i]].invisible = TRUE;
    //    }
    //}
    //else
    //{
    //    numHearts = (u8)(gContestEffects[gContestMoves[moveId].effect].appeal / 10);
//
    //    if (numHearts == 0xFF)
    //    {
    //        numHearts = 0;
    //    }
//
    //    for (i = 0; i < 8; i++)
    //    {
    //        if (i < numHearts)
    //        {
    //            StartSpriteAnim(&gSprites[sMoveRelearnerStruct->heartSpriteIds[i]], 1);
    //        }
    //        else
    //        {
    //            StartSpriteAnim(&gSprites[sMoveRelearnerStruct->heartSpriteIds[i]], 0);
    //        }
    //        gSprites[sMoveRelearnerStruct->heartSpriteIds[i]].invisible = FALSE;
    //    }
//
    //    numHearts = (u8)(gContestEffects[gContestMoves[moveId].effect].jam / 10);
//
    //    if (numHearts == 0xFF)
    //    {
    //        numHearts = 0;
    //    }
//
    //    for (i = 0; i < 8; i++)
    //    {
    //        if (i < numHearts)
    //        {
    //            StartSpriteAnim(&gSprites[sMoveRelearnerStruct->heartSpriteIds[i + 8]], 3);
    //        }
    //        else
    //        {
    //            StartSpriteAnim(&gSprites[sMoveRelearnerStruct->heartSpriteIds[i + 8]], 2);
    //        }
    //        gSprites[sMoveRelearnerStruct->heartSpriteIds[i + 8]].invisible = FALSE;
    //    }
    //}
}
