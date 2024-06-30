#include "global.h"
#include "option_menu.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "event_data.h"
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
#include "sound.h"
#include "strings.h"
#include "string_util.h"
#include "gba/m4a_internal.h"
#include "constants/rgb.h"
#include "constants/songs.h"
#include "constants/weather.h"

#include "rogue_controller.h"
#include "rogue_settings.h"
#include "rogue_quest.h"

#define QUICK_JUMP_AMOUNT 4

extern const u8 gText_16Spaces[];
extern const u8 gText_32Spaces[];
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
extern const u8 gText_DifficultyModeActive[];

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
extern const u8 gText_AdventureCustomDesc[];
extern const u8 gText_DifficultyOverLvlDesc[];

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

static u8 const sMenuName_Back[] = _("Back");
static u8 const sMenuName_SaveAndExit[] = _("Save & Exit");
static u8 const sMenuName_DifficultySubmenu[] = _("Edit Difficulty");
static u8 const sMenuName_AdventureSubmenu[] = _("Adventure");
static u8 const sMenuName_TrainersSubmenu[] = _("Trainers");
static u8 const sMenuName_GameModesSubmenu[] = _("Game Modes");

static u8 const sMenuName_BattleFormat[] = _("Battle Format");
static u8 const sMenuName_BattleFormatSingles[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Singles");
static u8 const sMenuName_BattleFormatDoubles[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Doubles");
static u8 const sMenuName_BattleFormatMixed[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Mixed");

//static u8 const sMenuName_TrainerOrder[] = _("Trainer Order");
//static u8 const sMenuName_TrainerOrderDefault[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Default");
//static u8 const sMenuName_TrainerOrderRainbow[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Rainbow");
//static u8 const sMenuName_TrainerOrderOfficial[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Official");

static u8 const sMenuName_GameMode_Standard[] = _("Standard");
static u8 const sMenuName_GameMode_Rainbow[] = _("Rainbow");
static u8 const sMenuName_GameMode_Official[] = _("Official");
static u8 const sMenuName_GameMode_Gauntlet[] = _("Gauntlet");
static u8 const sMenuName_GameMode_RainbowGauntlet[] = _("Rainbow Gauntlet");

static u8 const sMenuName_Affection[] = _("Affection FX");
static u8 const sMenuName_ReleaseMons[] = _("Release Fainted {PKMN}");
static u8 const sMenuName_TrainerDiversity[] = _("Diverse Trainer {PKMN}");

static u8 const sMenuName_TrainerRogue[] = _("Rogue");
static u8 const sMenuName_TrainerKanto[] = _("Kanto");
static u8 const sMenuName_TrainerJohto[] = _("Johto");
static u8 const sMenuName_TrainerHoenn[] = _("Hoenn");
#ifdef ROGUE_EXPANSION
static u8 const sMenuName_TrainerSinnoh[] = _("Sinnoh");
static u8 const sMenuName_TrainerUnova[] = _("Unova");
static u8 const sMenuName_TrainerKalos[] = _("Kalos");
static u8 const sMenuName_TrainerAlola[] = _("Alola");
static u8 const sMenuName_TrainerGalar[] = _("Galar");
static u8 const sMenuName_TrainerPaldea[] = _("Paldea");
#endif

static u8 const sText_ErrorInvalidSelection[] = _("Error: {COLOR GREEN}{SHADOW LIGHT_GREEN}Invalid selection.");

const u8 sMenuNameDesc_PresetDescription_Easy[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "For those who want a casual experience,\n"
    "comparable to modern {PKMN} games."
);
const u8 sMenuNameDesc_PresetDescription_Average[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Recommended for those who have good {PKMN}\n"
    "knowledge and enjoy a challenge."
);
const u8 sMenuNameDesc_PresetDescription_Hard[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "A punishing experience intended for\n"
    "those who enjoy increased difficulty."
);
const u8 sMenuNameDesc_PresetDescription_Brutal[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "An even more punishing experience than\n"
    "Hard, for those who want to suffer."
);
const u8 sMenuNameDesc_PresetDescription_Custom[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Your rules, your way!"
);
static u8 const* const sMenuNameDesc_PresetDescription[] = 
{
    [DIFFICULTY_LEVEL_EASY] = sMenuNameDesc_PresetDescription_Easy,
    [DIFFICULTY_LEVEL_AVERAGE] = sMenuNameDesc_PresetDescription_Average,
    [DIFFICULTY_LEVEL_HARD] = sMenuNameDesc_PresetDescription_Hard,
    [DIFFICULTY_LEVEL_BRUTAL] = sMenuNameDesc_PresetDescription_Brutal,
    [DIFFICULTY_LEVEL_CUSTOM] = sMenuNameDesc_PresetDescription_Custom,
};


const u8 sMenuNameDesc_DifficultyOverLvlOff[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "{PKMN} will stop gaining lvls once they\n"
    "reach the Level Cap."
);
const u8 sMenuNameDesc_DifficultyOverLvlOn[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "{PKMN} can gain levels over the Level Cap.\n"
    "(After the Level Cap, will Level slowly)"
);
static u8 const* const sMenuNameDesc_DifficultyOverLvl[] = 
{
    sMenuNameDesc_DifficultyOverLvlOff,
    sMenuNameDesc_DifficultyOverLvlOn,
};

static u8 const sMenuNameDesc_TrainersSubmenu[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Enable or Disable groups of Trainers that\n"
    "you would like to encounter."
);

static u8 const sMenuNameDesc_GameModesSubmenu[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Choose custom rule sets or scenarios\n"
    "you to play with."
);

static u8 const sMenuNameDesc_BattleFormatSingles[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Trainer Battles will always be 1v1."
);
static u8 const sMenuNameDesc_BattleFormatDoubles[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Trainer Battles will always be 2v2."
);
static u8 const sMenuNameDesc_BattleFormatMixed[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Trainer Battles will randomly be 1v1 or\n"
    "2v2. (Equal chance for both)"
);
static u8 const* const sMenuNameDesc_BattleFormat[] = 
{
    [BATTLE_FORMAT_SINGLES] = sMenuNameDesc_BattleFormatSingles,
    [BATTLE_FORMAT_DOUBLES] = sMenuNameDesc_BattleFormatDoubles,
    [BATTLE_FORMAT_MIXED] = sMenuNameDesc_BattleFormatMixed,
};

const u8 sMenuNameDesc_Affection[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "{PKMN} with high friendship may have special\n"
    "effects e.g. enduring, extra crits etc."
);

const u8 sMenuNameDesc_ReleaseMonsOn[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "When {PKMN} faint, they will be released.\n"
    "from your party."
);

const u8 sMenuNameDesc_ReleaseMonsOff[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Fainted {PKMN} will remain in your party, but\n"
    "can only be revived by Nurses or Revives."
);
static u8 const* const sMenuNameDesc_ReleaseMons[] = 
{
    sMenuNameDesc_ReleaseMonsOff,
    sMenuNameDesc_ReleaseMonsOn,
};

const u8 sMenuNameDesc_TrainerDiversityOff[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Trainers will mostly stick to their type\n"
    "specialties e.g. Brock has Rock"
);
const u8 sMenuNameDesc_TrainerDiversityOn[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Trainers can have wider type specialties\n"
    "e.g. Brock has a mix of Rock & Steel"
);
static u8 const* const sMenuNameDesc_TrainerDiversity[] = 
{
    sMenuNameDesc_TrainerDiversityOff,
    sMenuNameDesc_TrainerDiversityOn,
};

static u8 const sMenuNameDesc_Rogue[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Enables trainers from the… Rogue\n"
    "region? (Rainbow mode not supported)\n"
);

static u8 const sMenuNameDesc_Kanto[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Enables trainers from the Kanto region.\n"
);

static u8 const sMenuNameDesc_Johto[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Enables trainers from the Johto region.\n"
);

static u8 const sMenuNameDesc_Hoenn[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Enables trainers from the Hoenn region.\n"
);

#ifdef ROGUE_EXPANSION
static u8 const sMenuNameDesc_Sinnoh[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Enables trainers from the Sinnoh region.\n"
);

static u8 const sMenuNameDesc_Unova[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Enables trainers from the Unova region.\n"
);

static u8 const sMenuNameDesc_Kalos[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Enables trainers from the Kalos region.\n"
);

static u8 const sMenuNameDesc_Alola[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Enables trainers from the Alola region.\n"
);

static u8 const sMenuNameDesc_Galar[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Enables trainers from the Galar region.\n"
);

static u8 const sMenuNameDesc_Paldea[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Enables trainers from the Paldea region.\n"
);
#endif

static u8 const sMenuNameDesc_GameMode_Standard[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Typical Adventure with no custom rules."
);
static u8 const sMenuNameDesc_GameMode_Rainbow[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Mighty Trainers appear in any order but\n"
    "will never repeat type specialties.\n"
    "eg. E4 can be Gym Leaders and vice versa"
);
static u8 const sMenuNameDesc_GameMode_Official[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Mighty Trainers appear in the order they\n"
    "appear in their official games.\n"
    "(Disables Challenges)"
);
static u8 const sMenuNameDesc_GameMode_Gauntlet[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Prepare your team and then fight Mighty\n"
    "Trainers back to back without a chance\n"
    "to catch any {PKMN}. (Disables Challenges)"
);
static u8 const sMenuNameDesc_GameMode_RainbowGauntlet[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Combined effects of both Rainbow and\n"
    "Gauntlet modes."
);


static const u8 sText_DifficultyExpAllDescOff[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Only {PKMN} send into battle will be awarded\n"
    "Exp. (Not recommended)"
);
static const u8 sText_DifficultyExpAllDescOn[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "All {PKMN} in the party will be awarded Exp.\n"
    "even if they didn't enter the battle."
);
static u8 const* const sText_DifficultyExpAllDesc[] = 
{
    sText_DifficultyExpAllDescOff,
    sText_DifficultyExpAllDescOn,
};


static const u8 sText_DifficultyOverworldMonsDescOff[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Wild {PKMN} will spawn randomly as you move.\n"
    "(Classic {PKMN} Game style encounters)"
);
static const u8 sText_DifficultyOverworldMonsDescOn[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Wild {PKMN} can be encounted and interacted\n"
    "with in the overworld."
);
static u8 const* const sText_DifficultyOverworldMonsDesc[] = 
{
    sText_DifficultyOverworldMonsDescOff,
    sText_DifficultyOverworldMonsDescOn,
};


const u8 sText_DifficultyEVGainDescOff[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "All {PKMN} will never have EVs."
);
const u8 sText_DifficultyEVGainDescOn[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "{PKMN} gain EVs from Trainer battles based\n"
    "on their nature.(Trainers never have EVs)"
);
static u8 const* const sText_DifficultyEVGainDesc[] = 
{
    sText_DifficultyEVGainDescOff,
    sText_DifficultyEVGainDescOn,
};


const u8 sText_DifficultySwitchModeDescOff[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "After fainting an opposing {PKMN}, you will\n"
    "not be able to switch out until your turn."
);
const u8 sText_DifficultySwitchModeDescOn[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "After fainting an opposing {PKMN} you will be\n"
    "given a chance to switch out immediately."
);
static u8 const* const sText_DifficultySwitchModeDesc[] = 
{
    sText_DifficultySwitchModeDescOff,
    sText_DifficultySwitchModeDescOn,
};


const u8 sText_DifficultyBagWipeDescOff[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "You can take all Meta-Progression\n"
    "into Adventures. (e.g. Items, Day Care)"
);
const u8 sText_DifficultyBagWipeDescOn[] = _(
    "{COLOR GREEN}{SHADOW LIGHT_GREEN}"
    "Only your Partner {PKMN} and Key Items will\n"
    "be taken into runs."
);
static u8 const* const sText_DifficultyBagWipeDesc[] = 
{
    sText_DifficultyBagWipeDescOff,
    sText_DifficultyBagWipeDescOn,
};


#ifdef ROGUE_DEBUG
static u8 const sMenuName_Debug[] = _("DEBUG");

static u8 const sMenuName_DebugToggleInfoPanel[] = _("INFO PANEL");
static u8 const sMenuName_DebugToggleStealTeam[] = _("STEAL TEAM");
static u8 const sMenuName_DebugToggleLvl5[] = _("TRAINER LVL5");
static u8 const sMenuName_DebugToggleAllowSaveScum[] = _("ALLOW SAVE SCUM");
static u8 const sMenuName_DebugToggleInstantCapture[] = _("INSTANT CATCH");
static u8 const sMenuName_DebugToggleTodTintUsePlayerColour[] = _("PLAYER TOD TINT");
static u8 const sMenuName_DebugToggleDebugShops[] = _("DEBUG SHOPS");
static u8 const sMenuName_DebugToggleDebugLegends[] = _("DEBUG LEGENDS");
static u8 const sMenuName_DebugToggleDebugMonQuery[] = _("Dump Mon Query");
static u8 const sMenuName_DebugToggleDebugItemQuery[] = _("Dump Item Query");
static u8 const sMenuName_DebugToggleHideFollower[] = _("Hide Follower");
static u8 const sMenuName_DebugToggleStopWildSpawning[] = _("Stop Wild Spawn");
static u8 const sMenuName_DebugToggleDisableAssistantTimeout[] = _("Disable Assist Timeout");

static u8 const sMenuName_DebugRangeStartDifficulty[] = _("START DIFFICULTY");
static u8 const sMenuName_DebugRangeForcedRoute[] = _("FORCED ROUTE");
static u8 const sMenuName_DebugRangeForcedEvilTeam[] = _("FORCED TEAM");
#endif

// Menu items
enum
{
    MENUITEM_DIFFICULTY_PRESET,

    MENUITEM_MENU_DIFFICULTY_SUBMENU,
    MENUITEM_MENU_ADVENTURE_SUBMENU,
    MENUITEM_MENU_TRAINERS_SUBMENU,
    MENUITEM_MENU_GAME_MODES_SUBMENU,

    MENUITEM_MENU_TOGGLE_EXP_ALL,
    MENUITEM_MENU_TOGGLE_OVER_LVL,
    MENUITEM_MENU_TOGGLE_EV_GAIN,
    MENUITEM_MENU_TOGGLE_OVERWORLD_MONS,
    MENUITEM_MENU_TOGGLE_BAG_WIPE,
    MENUITEM_MENU_TOGGLE_SWITCH_MODE,
    MENUITEM_MENU_TOGGLE_DIVERSE_TRAINERS,
    MENUITEM_MENU_TOGGLE_AFFECTION,
    MENUITEM_MENU_TOGGLE_RELEASE_MONS,

    MENUITEM_MENU_TOGGLE_TRAINER_ROGUE,
    MENUITEM_MENU_TOGGLE_TRAINER_KANTO,
    MENUITEM_MENU_TOGGLE_TRAINER_JOHTO,
    MENUITEM_MENU_TOGGLE_TRAINER_HOENN,
#ifdef ROGUE_EXPANSION
     MENUITEM_MENU_TOGGLE_TRAINER_SINNOH,
     MENUITEM_MENU_TOGGLE_TRAINER_UNOVA,
     MENUITEM_MENU_TOGGLE_TRAINER_KALOS,
     MENUITEM_MENU_TOGGLE_TRAINER_ALOLA,
     MENUITEM_MENU_TOGGLE_TRAINER_GALAR,
     MENUITEM_MENU_TOGGLE_TRAINER_PALDEA,
#endif

    MENUITEM_MENU_SLIDER_TRAINER,
    MENUITEM_MENU_SLIDER_ITEM,
    MENUITEM_MENU_SLIDER_LEGENDARY,
    MENUITEM_MENU_SLIDER_BATTLE_FORMAT,

    MENUITEM_MENU_SLIDER_GAME_MODE_STANDARD,
    MENUITEM_MENU_SLIDER_GAME_MODE_RAINBOW,
    MENUITEM_MENU_SLIDER_GAME_MODE_OFFICIAL,
    MENUITEM_MENU_SLIDER_GAME_MODE_GAUNTLET,
    MENUITEM_MENU_SLIDER_GAME_MODE_RAINBOW_GAUNTLET,

#ifdef ROGUE_DEBUG
    MENUITEM_MENU_DEBUG_SUBMENU,

    MENUITEM_MENU_DEBUG_TOGGLE_INFO_PANEL,
    MENUITEM_MENU_DEBUG_TOGGLE_STEAL_TEAM,
    MENUITEM_MENU_DEBUG_TOGGLE_TRAINER_LVL_5,
    MENUITEM_MENU_DEBUG_TOGGLE_ALLOW_SAVE_SCUM,
    MENUITEM_MENU_DEBUG_TOGGLE_INSTANT_CAPTURE,
    MENUITEM_MENU_DEBUG_TOGGLE_TOD_TINT_USE_PLAYER_COLOUR,
    MENUITEM_MENU_DEBUG_TOGGLE_DEBUG_SHOPS,
    MENUITEM_MENU_DEBUG_TOGGLE_DEBUG_LEGENDS,
    MENUITEM_MENU_DEBUG_TOGGLE_DEBUG_MON_QUERY,
    MENUITEM_MENU_DEBUG_TOGGLE_DEBUG_ITEM_QUERY,
    MENUITEM_MENU_DEBUG_TOGGLE_HIDE_FOLLOWER,
    MENUITEM_MENU_DEBUG_TOGGLE_STOP_WILD_SPAWNING,
    MENUITEM_MENU_DEBUG_TOGGLE_DISABLE_ASSISTANT_TIMEOUT,

    MENUITEM_MENU_DEBUG_RANGE_START_DIFFICULTY,
    MENUITEM_MENU_DEBUG_RANGE_FORCED_ROUTE,
    MENUITEM_MENU_DEBUG_RANGE_FORCED_EVIL_TEAM,
#endif

    MENUITEM_CANCEL,
    MENUITEM_SAVE_AND_EXIT,
};

enum
{
    SUBMENUITEM_NONE,
    SUBMENUITEM_DIFFICULTY,
    SUBMENUITEM_ADVENTURE,
    SUBMENUITEM_TRAINERS,
    SUBMENUITEM_GAME_MODES,
#ifdef ROGUE_DEBUG
    SUBMENUITEM_DEBUG,
#endif
    SUBMENUITEM_COUNT,
};

// Window Ids
enum
{
    WIN_TEXT_OPTION,
    WIN_OPTIONS
};

#define MAX_MENUITEM_COUNT 24
#define MAX_MENUITEM_TO_DISPLAY 5
#define XPOS_TITLES       8
#define XPOS_CHOICES      111
#define YPOS_SPACING      16

struct RogueSettingsMenu
{
    u8 activeSubmenu;
    u8 dynamicMenuOptions[MAX_MENUITEM_COUNT];
};

EWRAM_DATA static struct RogueSettingsMenu *sRogueSettingsMenu = NULL;

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
static u8 Preset_ProcessInput(u8 menuOffset, u8 selection);
static u8 Toggle_ProcessInput(u8 menuOffset, u8 selection);
static void Toggle_DrawChoices(u8 menuOffset, u8 selection);
static u8 Empty_ProcessInput(u8 menuOffset, u8 selection);
static void Empty_DrawChoices(u8 menuOffset, u8 selection);
static u8 BattleFormat_ProcessInput(u8 menuOffset, u8 selection);
static void BattleFormat_DrawChoices(u8 menuOffset, u8 selection);
static u8 GameMode_ProcessInput(u8 menuOffset, u8 selection);
static void GameMode_DrawChoices(u8 menuOffset, u8 selection);

#ifdef ROGUE_DEBUG
static u8 DebugToggle_ProcessInput(u8 menuOffset, u8 selection);
static void DebugToggle_DrawChoices(u8 menuOffset, u8 selection);
static u8 DebugRange_ProcessInput(u8 menuOffset, u8 selection);
static void DebugRange_DrawChoices(u8 menuOffset, u8 selection);
static u8 DebugRange_DifficultySkipProcessInput(u8 menuOffset, u8 selection);
static u8 DebugRange_ForcedRouteProcessInput(u8 menuOffset, u8 selection);
static void DebugRange_ForcedRouteDrawChoices(u8 menuOffset, u8 selection);
static u8 DebugRange_ForcedTeamProcessInput(u8 menuOffset, u8 selection);
#endif

EWRAM_DATA static bool8 sArrowPressed = FALSE;

static const u16 sOptionMenuText_Pal[] = INCBIN_U16("graphics/interface/option_menu_text2.gbapal"); // <- inserts extra greens
// note: this is only used in the Japanese release
static const u8 sEqualSignGfx[] = INCBIN_U8("graphics/interface/option_menu_equals_sign.4bpp");

typedef u8 (*MenuItemInputCallback)(u8, u8);
typedef void (*MenuItemDrawCallback)(u8, u8);

struct MenuEntry
{
    const u8* itemName;
    const u8* singleDesc;
    const u8* const* multiDesc;
    MenuItemInputCallback processInput;
    MenuItemDrawCallback drawChoices;
    u8 descCount;
};

struct MenuEntries
{
    const u8 menuOptions[MAX_MENUITEM_COUNT];
};

#define SINGLE_DESC(text) singleDesc=text, .descCount=1
#define MULTI_DESC(text) multiDesc=text, .descCount=ARRAY_COUNT(text)

static const struct MenuEntry sOptionMenuItems[] =
{
    [MENUITEM_DIFFICULTY_PRESET] = 
    {
        .itemName = gText_DifficultyPreset,
        .MULTI_DESC(sMenuNameDesc_PresetDescription),
        .processInput = Preset_ProcessInput,
        .drawChoices = Slider_DrawChoices
    },

    [MENUITEM_MENU_DIFFICULTY_SUBMENU] = 
    {
        .itemName = sMenuName_DifficultySubmenu,
        .SINGLE_DESC(gText_DifficultyCustomDesc),
        .processInput = Empty_ProcessInput,
        .drawChoices = ArrowRight_DrawChoices
    },
    [MENUITEM_MENU_ADVENTURE_SUBMENU] = 
    {
        .itemName = sMenuName_AdventureSubmenu,
        .SINGLE_DESC(gText_AdventureCustomDesc),
        .processInput = Empty_ProcessInput,
        .drawChoices = ArrowRight_DrawChoices
    },
    [MENUITEM_MENU_TRAINERS_SUBMENU] = 
    {
        .itemName = sMenuName_TrainersSubmenu,
        .SINGLE_DESC(sMenuNameDesc_TrainersSubmenu),
        .processInput = Empty_ProcessInput,
        .drawChoices = ArrowRight_DrawChoices
    },
    [MENUITEM_MENU_GAME_MODES_SUBMENU] = 
    {
        .itemName = sMenuName_GameModesSubmenu,
        .SINGLE_DESC(sMenuNameDesc_GameModesSubmenu),
        .processInput = Empty_ProcessInput,
        .drawChoices = ArrowRight_DrawChoices
    },


    [MENUITEM_MENU_TOGGLE_EXP_ALL] = 
    {
        .itemName = gText_DifficultyExpAll,
        .MULTI_DESC(sText_DifficultyExpAllDesc),
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
    [MENUITEM_MENU_TOGGLE_OVER_LVL] = 
    {
        .itemName = gText_DifficultyOverLvl,
        .MULTI_DESC(sMenuNameDesc_DifficultyOverLvl),
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
    [MENUITEM_MENU_TOGGLE_EV_GAIN] = 
    {
        .itemName = gText_DifficultyEVGain,
        .MULTI_DESC(sText_DifficultyEVGainDesc),
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
    [MENUITEM_MENU_TOGGLE_OVERWORLD_MONS] = 
    {
        .itemName = gText_DifficultyOverworldMons,
        .MULTI_DESC(sText_DifficultyOverworldMonsDesc),
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
    [MENUITEM_MENU_TOGGLE_BAG_WIPE] = 
    {
        .itemName = gText_DifficultyBagWipe,
        .MULTI_DESC(sText_DifficultyBagWipeDesc),
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
    [MENUITEM_MENU_TOGGLE_SWITCH_MODE] = 
    {
        .itemName = gText_DifficultySwitchMode,
        .MULTI_DESC(sText_DifficultySwitchModeDesc),
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
    [MENUITEM_MENU_TOGGLE_DIVERSE_TRAINERS] = 
    {
        .itemName = sMenuName_TrainerDiversity,
        .MULTI_DESC(sMenuNameDesc_TrainerDiversity),
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
    [MENUITEM_MENU_TOGGLE_AFFECTION] = 
    {
        .itemName = sMenuName_Affection,
        .SINGLE_DESC(sMenuNameDesc_Affection),
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
    [MENUITEM_MENU_TOGGLE_RELEASE_MONS] = 
    {
        .itemName = sMenuName_ReleaseMons,
        .MULTI_DESC(sMenuNameDesc_ReleaseMons),
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },

    // Trainers
    //
    [MENUITEM_MENU_TOGGLE_TRAINER_ROGUE] = 
    {
        .itemName = sMenuName_TrainerRogue,
        .SINGLE_DESC(sMenuNameDesc_Rogue),
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
    [MENUITEM_MENU_TOGGLE_TRAINER_KANTO] = 
    {
        .itemName = sMenuName_TrainerKanto,
        .SINGLE_DESC(sMenuNameDesc_Kanto),
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
    [MENUITEM_MENU_TOGGLE_TRAINER_JOHTO] = 
    {
        .itemName = sMenuName_TrainerJohto,
        .SINGLE_DESC(sMenuNameDesc_Johto),
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
    [MENUITEM_MENU_TOGGLE_TRAINER_HOENN] = 
    {
        .itemName = sMenuName_TrainerHoenn,
        .SINGLE_DESC(sMenuNameDesc_Hoenn),
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
#ifdef ROGUE_EXPANSION
    [MENUITEM_MENU_TOGGLE_TRAINER_SINNOH] = 
    {
        .itemName = sMenuName_TrainerSinnoh,
        .SINGLE_DESC(sMenuNameDesc_Sinnoh),
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
    [MENUITEM_MENU_TOGGLE_TRAINER_UNOVA] = 
    {
        .itemName = sMenuName_TrainerUnova,
        .SINGLE_DESC(sMenuNameDesc_Unova),
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
    [MENUITEM_MENU_TOGGLE_TRAINER_KALOS] = 
    {
        .itemName = sMenuName_TrainerKalos,
        .SINGLE_DESC(sMenuNameDesc_Kalos),
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
    [MENUITEM_MENU_TOGGLE_TRAINER_ALOLA] = 
    {
        .itemName = sMenuName_TrainerAlola,
        .SINGLE_DESC(sMenuNameDesc_Alola),
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
    [MENUITEM_MENU_TOGGLE_TRAINER_GALAR] = 
    {
        .itemName = sMenuName_TrainerGalar,
        .SINGLE_DESC(sMenuNameDesc_Galar),
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
    [MENUITEM_MENU_TOGGLE_TRAINER_PALDEA] = 
    {
        .itemName = sMenuName_TrainerPaldea,
        .SINGLE_DESC(sMenuNameDesc_Paldea),
        .processInput = Toggle_ProcessInput,
        .drawChoices = Toggle_DrawChoices
    },
#endif

    [MENUITEM_MENU_SLIDER_TRAINER] = 
    {
        .itemName = gText_DifficultyTrainers,
        .SINGLE_DESC(gText_DifficultyTrainersDesc),
        .processInput = Slider_ProcessInput,
        .drawChoices = Slider_DrawChoices
    },
    [MENUITEM_MENU_SLIDER_ITEM] = 
    {
        .itemName = gText_DifficultyItems,
        .SINGLE_DESC(gText_DifficultyItemsDesc),
        .processInput = Slider_ProcessInput,
        .drawChoices = Slider_DrawChoices
    },
    [MENUITEM_MENU_SLIDER_LEGENDARY] = 
    {
        .itemName = gText_DifficultyLegendaries,
        .SINGLE_DESC(gText_DifficultyLegendariesDesc),
        .processInput = Slider_ProcessInput,
        .drawChoices = Slider_DrawChoices
    },
    [MENUITEM_MENU_SLIDER_BATTLE_FORMAT] = 
    {
        .itemName = sMenuName_BattleFormat,
        .MULTI_DESC(sMenuNameDesc_BattleFormat),
        .processInput = BattleFormat_ProcessInput,
        .drawChoices = BattleFormat_DrawChoices
    },

    [MENUITEM_MENU_SLIDER_GAME_MODE_STANDARD] = 
    {
        .itemName = sMenuName_GameMode_Standard,
        .SINGLE_DESC(sMenuNameDesc_GameMode_Standard),
        .processInput = GameMode_ProcessInput,
        .drawChoices = GameMode_DrawChoices
    },
    [MENUITEM_MENU_SLIDER_GAME_MODE_RAINBOW] = 
    {
        .itemName = sMenuName_GameMode_Rainbow,
        .SINGLE_DESC(sMenuNameDesc_GameMode_Rainbow),
        .processInput = GameMode_ProcessInput,
        .drawChoices = GameMode_DrawChoices
    },
    [MENUITEM_MENU_SLIDER_GAME_MODE_OFFICIAL] = 
    {
        .itemName = sMenuName_GameMode_Official,
        .SINGLE_DESC(sMenuNameDesc_GameMode_Official),
        .processInput = GameMode_ProcessInput,
        .drawChoices = GameMode_DrawChoices
    },
    [MENUITEM_MENU_SLIDER_GAME_MODE_GAUNTLET] = 
    {
        .itemName = sMenuName_GameMode_Gauntlet,
        .SINGLE_DESC(sMenuNameDesc_GameMode_Gauntlet),
        .processInput = GameMode_ProcessInput,
        .drawChoices = GameMode_DrawChoices
    },
    [MENUITEM_MENU_SLIDER_GAME_MODE_RAINBOW_GAUNTLET] = 
    {
        .itemName = sMenuName_GameMode_RainbowGauntlet,
        .SINGLE_DESC(sMenuNameDesc_GameMode_RainbowGauntlet),
        .processInput = GameMode_ProcessInput,
        .drawChoices = GameMode_DrawChoices
    },

#ifdef ROGUE_DEBUG
    [MENUITEM_MENU_DEBUG_SUBMENU] = 
    {
        .itemName = sMenuName_Debug,
        .processInput = Empty_ProcessInput,
        .drawChoices = ArrowRight_DrawChoices
    },

    [MENUITEM_MENU_DEBUG_TOGGLE_INFO_PANEL] = 
    {
        .itemName = sMenuName_DebugToggleInfoPanel,
        .processInput = DebugToggle_ProcessInput,
        .drawChoices = DebugToggle_DrawChoices
    },
    [MENUITEM_MENU_DEBUG_TOGGLE_STEAL_TEAM] = 
    {
        .itemName = sMenuName_DebugToggleStealTeam,
        .processInput = DebugToggle_ProcessInput,
        .drawChoices = DebugToggle_DrawChoices
    },
    [MENUITEM_MENU_DEBUG_TOGGLE_TRAINER_LVL_5] = 
    {
        .itemName = sMenuName_DebugToggleLvl5,
        .processInput = DebugToggle_ProcessInput,
        .drawChoices = DebugToggle_DrawChoices
    },
    [MENUITEM_MENU_DEBUG_TOGGLE_ALLOW_SAVE_SCUM] = 
    {
        .itemName = sMenuName_DebugToggleAllowSaveScum,
        .processInput = DebugToggle_ProcessInput,
        .drawChoices = DebugToggle_DrawChoices
    },
    [MENUITEM_MENU_DEBUG_TOGGLE_INSTANT_CAPTURE] = 
    {
        .itemName = sMenuName_DebugToggleInstantCapture,
        .processInput = DebugToggle_ProcessInput,
        .drawChoices = DebugToggle_DrawChoices
    },
    [MENUITEM_MENU_DEBUG_TOGGLE_TOD_TINT_USE_PLAYER_COLOUR] = 
    {
        .itemName = sMenuName_DebugToggleTodTintUsePlayerColour,
        .processInput = DebugToggle_ProcessInput,
        .drawChoices = DebugToggle_DrawChoices
    },
    [MENUITEM_MENU_DEBUG_TOGGLE_DEBUG_SHOPS] = 
    {
        .itemName = sMenuName_DebugToggleDebugShops,
        .processInput = DebugToggle_ProcessInput,
        .drawChoices = DebugToggle_DrawChoices
    },
    [MENUITEM_MENU_DEBUG_TOGGLE_DEBUG_LEGENDS] = 
    {
        .itemName = sMenuName_DebugToggleDebugLegends,
        .processInput = DebugToggle_ProcessInput,
        .drawChoices = DebugToggle_DrawChoices
    },
    [MENUITEM_MENU_DEBUG_TOGGLE_DEBUG_MON_QUERY] = 
    {
        .itemName = sMenuName_DebugToggleDebugMonQuery,
        .processInput = DebugToggle_ProcessInput,
        .drawChoices = DebugToggle_DrawChoices
    },
    [MENUITEM_MENU_DEBUG_TOGGLE_DEBUG_ITEM_QUERY] = 
    {
        .itemName = sMenuName_DebugToggleDebugItemQuery,
        .processInput = DebugToggle_ProcessInput,
        .drawChoices = DebugToggle_DrawChoices
    },
    [MENUITEM_MENU_DEBUG_TOGGLE_HIDE_FOLLOWER] = 
    {
        .itemName = sMenuName_DebugToggleHideFollower,
        .processInput = DebugToggle_ProcessInput,
        .drawChoices = DebugToggle_DrawChoices
    },
    [MENUITEM_MENU_DEBUG_TOGGLE_STOP_WILD_SPAWNING] = 
    {
        .itemName = sMenuName_DebugToggleStopWildSpawning,
        .processInput = DebugToggle_ProcessInput,
        .drawChoices = DebugToggle_DrawChoices
    },
    [MENUITEM_MENU_DEBUG_TOGGLE_DISABLE_ASSISTANT_TIMEOUT] = 
    {
        .itemName = sMenuName_DebugToggleDisableAssistantTimeout,
        .processInput = DebugToggle_ProcessInput,
        .drawChoices = DebugToggle_DrawChoices
    },

    [MENUITEM_MENU_DEBUG_RANGE_START_DIFFICULTY] = 
    {
        .itemName = sMenuName_DebugRangeStartDifficulty,
        .processInput = DebugRange_DifficultySkipProcessInput,
        .drawChoices = DebugRange_DrawChoices
    },
    [MENUITEM_MENU_DEBUG_RANGE_FORCED_ROUTE] = 
    {
        .itemName = sMenuName_DebugRangeForcedRoute,
        .processInput = DebugRange_ForcedRouteProcessInput,
        .drawChoices = DebugRange_ForcedRouteDrawChoices
    },
    [MENUITEM_MENU_DEBUG_RANGE_FORCED_EVIL_TEAM] = 
    {
        .itemName = sMenuName_DebugRangeForcedEvilTeam,
        .processInput = DebugRange_ForcedTeamProcessInput,
        .drawChoices = DebugRange_DrawChoices
    },

#endif

    [MENUITEM_CANCEL] = 
    {
        .itemName = sMenuName_Back,
        .processInput = Empty_ProcessInput,
        .drawChoices = Empty_DrawChoices
    },
    [MENUITEM_SAVE_AND_EXIT] = 
    {
        .itemName = sMenuName_SaveAndExit,
        .processInput = Empty_ProcessInput,
        .drawChoices = Empty_DrawChoices
    },
};

#undef SINGLE_DESC
#undef MULTI_DESC

static const struct MenuEntries sOptionMenuEntries[SUBMENUITEM_COUNT] =
{
    [SUBMENUITEM_NONE] = 
    {
        .menuOptions = 
        {
            MENUITEM_DIFFICULTY_PRESET,
            MENUITEM_MENU_DIFFICULTY_SUBMENU,
            MENUITEM_MENU_ADVENTURE_SUBMENU,
            MENUITEM_MENU_TRAINERS_SUBMENU,
            MENUITEM_MENU_GAME_MODES_SUBMENU,
#ifdef ROGUE_DEBUG
            MENUITEM_MENU_DEBUG_SUBMENU,
#endif
            MENUITEM_SAVE_AND_EXIT
        }
    },
    [SUBMENUITEM_DIFFICULTY] = 
    {
        .menuOptions = 
        {
            MENUITEM_MENU_SLIDER_TRAINER,
            MENUITEM_MENU_TOGGLE_DIVERSE_TRAINERS,
            //MENUITEM_MENU_SLIDER_ITEM,
            //MENUITEM_MENU_SLIDER_LEGENDARY,
            MENUITEM_MENU_TOGGLE_RELEASE_MONS,
            MENUITEM_MENU_TOGGLE_OVER_LVL,
            MENUITEM_MENU_TOGGLE_EV_GAIN,
#ifdef ROGUE_EXPANSION
            MENUITEM_MENU_TOGGLE_AFFECTION,
#endif
            MENUITEM_MENU_TOGGLE_SWITCH_MODE,
            MENUITEM_MENU_TOGGLE_BAG_WIPE,
            MENUITEM_CANCEL
        }
    },
    [SUBMENUITEM_ADVENTURE] = 
    {
        .menuOptions = 
        {
            MENUITEM_MENU_SLIDER_BATTLE_FORMAT,
            MENUITEM_MENU_TOGGLE_OVERWORLD_MONS,
            MENUITEM_MENU_TOGGLE_EXP_ALL,

            MENUITEM_CANCEL
        }
    },
    
    [SUBMENUITEM_TRAINERS] = 
    {
        .menuOptions = 
        {
            MENUITEM_MENU_TOGGLE_TRAINER_KANTO,
            MENUITEM_MENU_TOGGLE_TRAINER_JOHTO,
            MENUITEM_MENU_TOGGLE_TRAINER_HOENN,
#ifdef ROGUE_EXPANSION
            MENUITEM_MENU_TOGGLE_TRAINER_SINNOH,
            MENUITEM_MENU_TOGGLE_TRAINER_UNOVA,
            MENUITEM_MENU_TOGGLE_TRAINER_KALOS,
            MENUITEM_MENU_TOGGLE_TRAINER_ALOLA,
            MENUITEM_MENU_TOGGLE_TRAINER_GALAR,
            MENUITEM_MENU_TOGGLE_TRAINER_PALDEA,
#endif
            MENUITEM_MENU_TOGGLE_TRAINER_ROGUE,
            MENUITEM_CANCEL
        }
    },
    [SUBMENUITEM_GAME_MODES] = 
    {
        .menuOptions = 
        {
            MENUITEM_MENU_SLIDER_GAME_MODE_STANDARD,
            MENUITEM_MENU_SLIDER_GAME_MODE_RAINBOW,
            MENUITEM_MENU_SLIDER_GAME_MODE_OFFICIAL,
            MENUITEM_MENU_SLIDER_GAME_MODE_GAUNTLET,
            MENUITEM_MENU_SLIDER_GAME_MODE_RAINBOW_GAUNTLET,
            MENUITEM_CANCEL
        }
    },
#ifdef ROGUE_DEBUG
    [SUBMENUITEM_DEBUG] = 
    {
        .menuOptions = 
        {
            MENUITEM_MENU_DEBUG_TOGGLE_INFO_PANEL,
            MENUITEM_MENU_DEBUG_TOGGLE_STEAL_TEAM,
            MENUITEM_MENU_DEBUG_TOGGLE_TRAINER_LVL_5,
            MENUITEM_MENU_DEBUG_TOGGLE_ALLOW_SAVE_SCUM,
            MENUITEM_MENU_DEBUG_TOGGLE_INSTANT_CAPTURE,
            MENUITEM_MENU_DEBUG_TOGGLE_TOD_TINT_USE_PLAYER_COLOUR,
            MENUITEM_MENU_DEBUG_TOGGLE_DEBUG_SHOPS,
            MENUITEM_MENU_DEBUG_TOGGLE_DEBUG_LEGENDS,
            MENUITEM_MENU_DEBUG_TOGGLE_DEBUG_MON_QUERY,
            MENUITEM_MENU_DEBUG_TOGGLE_DEBUG_ITEM_QUERY,
            MENUITEM_MENU_DEBUG_TOGGLE_HIDE_FOLLOWER,
            MENUITEM_MENU_DEBUG_TOGGLE_STOP_WILD_SPAWNING,
            MENUITEM_MENU_DEBUG_TOGGLE_DISABLE_ASSISTANT_TIMEOUT,

            MENUITEM_MENU_DEBUG_RANGE_START_DIFFICULTY,
            MENUITEM_MENU_DEBUG_RANGE_FORCED_ROUTE,
            MENUITEM_MENU_DEBUG_RANGE_FORCED_EVIL_TEAM,

            MENUITEM_CANCEL
        }
    },
#endif
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

void Rogue_OpenDifficultyConfigMenu(RogueDifficultyMenuCallback callback)
{
    gMain.savedCallback = callback;
    SetMainCallback2(CB2_InitDifficultyConfigMenu);
    LockPlayerFieldControls();
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
        u8 taskId = CreateTask(Task_OptionMenuFadeIn, 0);

        AGB_ASSERT(sRogueSettingsMenu == NULL);
        sRogueSettingsMenu = AllocZeroed(sizeof(struct RogueSettingsMenu));
        sRogueSettingsMenu->activeSubmenu = SUBMENUITEM_COUNT;

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

static bool8 IsMenuOptionActive(u8 menuOption)
{
    switch (menuOption)
    {
    case MENUITEM_MENU_TRAINERS_SUBMENU:
    case MENUITEM_MENU_GAME_MODES_SUBMENU:
        return FlagGet(FLAG_ROGUE_MET_POKABBIE);

    case MENUITEM_MENU_TOGGLE_TRAINER_ROGUE:
        return RogueQuest_HasCollectedRewards(QUEST_ID_ONE_LAST_QUEST);
    }

    return TRUE;
}

static u8 GetMenuItemFor(u8 submenu, u8 index)
{
    if(sRogueSettingsMenu->activeSubmenu != submenu)
    {
        u8 i;
        u8 write = 0;

        memset(sRogueSettingsMenu->dynamicMenuOptions, 0, sizeof(sRogueSettingsMenu->dynamicMenuOptions));

        for(i = 0; i < MAX_MENUITEM_COUNT; ++i)
        {
            u8 menuOption = sOptionMenuEntries[submenu].menuOptions[i];

            if(IsMenuOptionActive(menuOption))
            {
                sRogueSettingsMenu->dynamicMenuOptions[write++] = menuOption;
            }
        }

        sRogueSettingsMenu->activeSubmenu = submenu;
    }

    return sRogueSettingsMenu->dynamicMenuOptions[index];
}

static bool8 CanExitWithB(u8 submenuSelection)
{
    if(submenuSelection != SUBMENUITEM_NONE)
        return TRUE;

    // In tutorial section, we cannot exit root using B button
    if(gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(ROGUE_INTRO) && gSaveBlock1Ptr->location.mapNum  == MAP_NUM(ROGUE_INTRO))
        return FALSE;

    return TRUE;
}

static bool8 TryCloseSubmenu(u8 submenuSelection)
{
    if(submenuSelection == SUBMENUITEM_TRAINERS)
    {
        u8 i;
        u32 togglesToCheck[] = 
        {
            CONFIG_TOGGLE_TRAINER_ROGUE,
            CONFIG_TOGGLE_TRAINER_KANTO,
            CONFIG_TOGGLE_TRAINER_JOHTO,
            CONFIG_TOGGLE_TRAINER_HOENN,
#ifdef ROGUE_EXPANSION
            CONFIG_TOGGLE_TRAINER_SINNOH,
            CONFIG_TOGGLE_TRAINER_UNOVA,
            CONFIG_TOGGLE_TRAINER_KALOS,
            CONFIG_TOGGLE_TRAINER_ALOLA,
            CONFIG_TOGGLE_TRAINER_GALAR,
            CONFIG_TOGGLE_TRAINER_PALDEA,
#endif
        };

        for(i = 0; i < ARRAY_COUNT(togglesToCheck); ++i)
        {
            if(Rogue_GetConfigToggle(togglesToCheck[i]) == TRUE)
                return TRUE;
        }

        return FALSE;
    }

    return TRUE;
}

static void Task_OptionMenuProcessInput(u8 taskId)
{
    bool8 submenuChanged = FALSE;
    u8 menuSelection = gTasks[taskId].data[TD_MENUSELECTION];
    u8 menuSelectionTop = gTasks[taskId].data[TD_MENUSELECTION_TOP];
    u8 submenuSelection = gTasks[taskId].data[TD_SUBMENU];
    u8 menuItem = GetMenuItemFor(submenuSelection, menuSelection);

    if ((CanExitWithB(submenuSelection) && JOY_NEW(B_BUTTON)) || (JOY_NEW(A_BUTTON) && (menuItem == MENUITEM_CANCEL || menuItem == MENUITEM_SAVE_AND_EXIT)))
    {
        if(submenuSelection != SUBMENUITEM_NONE)
        {
            if(TryCloseSubmenu(submenuSelection))
            {
                submenuSelection = SUBMENUITEM_NONE;
                submenuChanged = TRUE;
            }
            else
            {
                PlaySE(SE_FAILURE);
                
                FillWindowPixelBuffer(WIN_TEXT_OPTION, PIXEL_FILL(1));
                AddTextPrinterParameterized(WIN_TEXT_OPTION, FONT_NORMAL, sText_ErrorInvalidSelection, 8, 1, TEXT_SKIP_DRAW, NULL);
                CopyWindowToVram(WIN_TEXT_OPTION, COPYWIN_FULL);
            }
        }
        else
            gTasks[taskId].func = Task_OptionMenuSave;
    }
    else if(JOY_NEW(A_BUTTON) && submenuSelection == SUBMENUITEM_NONE)
    {
        switch (menuItem)
        {
        case MENUITEM_MENU_DIFFICULTY_SUBMENU:
            submenuSelection = SUBMENUITEM_DIFFICULTY;
            submenuChanged = TRUE;
            break;

        case MENUITEM_MENU_ADVENTURE_SUBMENU:
            submenuSelection = SUBMENUITEM_ADVENTURE;
            submenuChanged = TRUE;
            break;

        case MENUITEM_MENU_TRAINERS_SUBMENU:
            submenuSelection = SUBMENUITEM_TRAINERS;
            submenuChanged = TRUE;
            break;

        case MENUITEM_MENU_GAME_MODES_SUBMENU:
            submenuSelection = SUBMENUITEM_GAME_MODES;
            submenuChanged = TRUE;
            break;

#ifdef ROGUE_DEBUG
        case MENUITEM_MENU_DEBUG_SUBMENU:
            submenuSelection = SUBMENUITEM_DEBUG;
            submenuChanged = TRUE;
            break;
#endif
        }
    }
    else if (JOY_REPEAT(DPAD_UP | L_BUTTON))
    {
        u8 i;
        u8 repeatAmount = JOY_REPEAT(L_BUTTON) ? QUICK_JUMP_AMOUNT : 1;

        for(i = 0; i < repeatAmount; ++i)
        {
            if(menuSelection != 0)
            {
                menuSelection--;

                if(menuSelection < menuSelectionTop)
                {
                    menuSelectionTop = menuSelection;
                }
            }
            else
                break;
        }

        DrawDescriptionOptionMenuText(submenuSelection, menuSelection);
        DrawOptionMenuTexts(submenuSelection, menuSelectionTop);

        HighlightOptionMenuItem(menuSelection, menuSelectionTop);
        gTasks[taskId].data[TD_MENUSELECTION] = menuSelection;
        gTasks[taskId].data[TD_MENUSELECTION_TOP] = menuSelectionTop;
    }
    else if (JOY_REPEAT(DPAD_DOWN | R_BUTTON))
    {
        u8 i;
        u8 repeatAmount = JOY_REPEAT(R_BUTTON) ? QUICK_JUMP_AMOUNT : 1;
        
        for(i = 0; i < repeatAmount; ++i)
        {
            if(menuItem != MENUITEM_CANCEL && menuItem != MENUITEM_SAVE_AND_EXIT)
            {
                menuSelection++;
                menuItem = GetMenuItemFor(submenuSelection, menuSelection);

                if(menuSelection >= menuSelectionTop + MAX_MENUITEM_TO_DISPLAY)
                {
                    menuSelectionTop = menuSelection - MAX_MENUITEM_TO_DISPLAY + 1;
                }
            }
            else
                break;
        }

        DrawDescriptionOptionMenuText(submenuSelection, menuSelection);
        DrawOptionMenuTexts(submenuSelection, menuSelectionTop);

        HighlightOptionMenuItem(menuSelection, menuSelectionTop);
        gTasks[taskId].data[TD_MENUSELECTION] = menuSelection;
        gTasks[taskId].data[TD_MENUSELECTION_TOP] = menuSelectionTop;
    }
    else if(menuItem != MENUITEM_CANCEL && menuItem != MENUITEM_SAVE_AND_EXIT)
    {
        u8 currOption;
        u8 prevOption;

        prevOption = GetMenuItemValue(menuItem);

        currOption = sOptionMenuItems[menuItem].processInput(menuSelection, prevOption);

        if(prevOption != currOption)
        {
            // Redraw all options in Game Modes, as changing one setting can toggle other settings
            if(submenuSelection == SUBMENUITEM_GAME_MODES)
            {
                SetMenuItemValue(menuItem, currOption);
                DrawOptionMenuTexts(submenuSelection, menuSelectionTop);
            }
            else
            {
                sOptionMenuItems[menuItem].drawChoices(menuSelection - menuSelectionTop, currOption);
                SetMenuItemValue(menuItem, currOption);
            }

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
            menuSelectionTop = 0;
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
        AGB_ASSERT(sRogueSettingsMenu != NULL);
        Free(sRogueSettingsMenu);
        sRogueSettingsMenu = NULL;

        DestroyTask(taskId);
        FreeAllWindowBuffers();
        SetMainCallback2(gMain.savedCallback);
        // ScriptContext_Enable(); <- handled in savedCallback
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
    u8 dst[32];
    u16 i;

    for (i = 0; *text != EOS && i <= (ARRAY_COUNT(dst) - 2); i++)
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
    DrawOptionMenuChoice(gText_DifficultyArrowRight, XPOS_CHOICES, menuOffset * YPOS_SPACING, 0);
}

static void UNUSED ArrowLeft_DrawChoices(u8 menuOffset, u8 selection)
{
    DrawOptionMenuChoice(gText_DifficultyArrowLeft, XPOS_CHOICES, menuOffset * YPOS_SPACING, 0);
}

static bool8 ShouldSkipInput()
{
    if(JOY_NEW((DPAD_RIGHT | DPAD_LEFT | A_BUTTON)) && !Rogue_CanEditConfig())
    {
        PlaySE(SE_FAILURE);
        return TRUE;
    }

    return FALSE;
}

static u8 Slider_ProcessInput(u8 menuOffset, u8 selection)
{
    if(ShouldSkipInput())
        return selection;

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
    DrawOptionMenuChoice(gText_32Spaces, XPOS_CHOICES, menuOffset * YPOS_SPACING, 0);

    switch (selection)
    {
    case DIFFICULTY_LEVEL_EASY:
        text = gText_DifficultyPresetEasy;
        break;

    case DIFFICULTY_LEVEL_AVERAGE:
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

    DrawOptionMenuChoice(text, XPOS_CHOICES, menuOffset * YPOS_SPACING, style);
}

static u8 Preset_ProcessInput(u8 menuOffset, u8 selection)
{
    // Same behaviour as Slider_ProcessInput except for when custom is the option

    if(ShouldSkipInput())
        return selection;

    if(selection == DIFFICULTY_LEVEL_CUSTOM)
    {
        // If we were on custom, first click should be going to the current reward level difficulty
        if (JOY_NEW(DPAD_RIGHT) || JOY_NEW(DPAD_LEFT))
        {
            selection = Rogue_GetDifficultyRewardLevel();
            sArrowPressed = TRUE;
            return selection;
        }
    }

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

static u8 Toggle_ProcessInput(u8 menuOffset, u8 selection)
{
    if(ShouldSkipInput())
        return selection;

    if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT | A_BUTTON ))
    {
        selection ^= 1;
        sArrowPressed = TRUE;
    }

    return selection;
}

static void Toggle_DrawChoices(u8 menuOffset, u8 selection)
{
    // Hack to wipe tiles????
    DrawOptionMenuChoice(gText_32Spaces, XPOS_CHOICES, menuOffset * YPOS_SPACING, 0);

    if(selection == 0)
        DrawOptionMenuChoice(gText_DifficultyDisabled, XPOS_CHOICES, menuOffset * YPOS_SPACING, 0);
    else
        DrawOptionMenuChoice(gText_DifficultyEnabled, XPOS_CHOICES, menuOffset * YPOS_SPACING, 0);
}

static u8 ProcessInputRange(u8 menuOffset, u8 selection, u8 range)
{
    if(ShouldSkipInput())
        return selection;

    if (JOY_NEW(DPAD_RIGHT | A_BUTTON))
    {
        if (selection == range -1)
            selection = 0;
        else
            ++selection;

        sArrowPressed = TRUE;
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        if (selection == 0)
            selection = range -1;
        else
            --selection;

        sArrowPressed = TRUE;
    }
    return selection;
}

static u8 BattleFormat_ProcessInput(u8 menuOffset, u8 selection)
{
    return ProcessInputRange(menuOffset, selection, BATTLE_FORMAT_COUNT);
}

static void BattleFormat_DrawChoices(u8 menuOffset, u8 selection)
{
    const u8* text = NULL;
    u8 style = 0;

    // Hack to wipe tiles????
    DrawOptionMenuChoice(gText_32Spaces, XPOS_CHOICES, menuOffset * YPOS_SPACING, 0);

    switch (selection)
    {
    case BATTLE_FORMAT_SINGLES:
        text = sMenuName_BattleFormatSingles;
        break;

    case BATTLE_FORMAT_DOUBLES:
        text = sMenuName_BattleFormatDoubles;
        break;

    case BATTLE_FORMAT_MIXED:
        text = sMenuName_BattleFormatMixed;
        break;
    }

    DrawOptionMenuChoice(text, XPOS_CHOICES, menuOffset * YPOS_SPACING, style);
}

static u8 GameMode_ProcessInput(u8 menuOffset, u8 selection)
{
    if(ShouldSkipInput())
        return selection;

    if (!selection && JOY_NEW(A_BUTTON))
    {
        selection ^= 1;
        sArrowPressed = TRUE;
    }

    return selection;
}

static void GameMode_DrawChoices(u8 menuOffset, u8 selection)
{
    // Hack to wipe tiles????
    DrawOptionMenuChoice(gText_32Spaces, XPOS_CHOICES, menuOffset * YPOS_SPACING, 0);

    // Only draw enabled

    if(selection != 0)
        DrawOptionMenuChoice(gText_DifficultyModeActive, XPOS_CHOICES, menuOffset * YPOS_SPACING, 0);
}

#ifdef ROGUE_DEBUG

static u8 DebugToggle_ProcessInput(u8 menuOffset, u8 selection)
{
    if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT | A_BUTTON))
    {
        selection ^= 1;
        sArrowPressed = TRUE;
    }

    return selection;
}

static void DebugToggle_DrawChoices(u8 menuOffset, u8 selection)
{
    Toggle_DrawChoices(menuOffset, selection);
}

static u8 DebugRange_ProcessInput(u8 menuOffset, u8 selection)
{
    if (JOY_NEW(DPAD_RIGHT | A_BUTTON))
    {
        selection++;
        sArrowPressed = TRUE;
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        if (selection != 0)
            selection--;

        sArrowPressed = TRUE;
    }
    return selection;
}

static void DebugRange_DrawChoices(u8 menuOffset, u8 selection)
{
    u8 text[16];

    DrawOptionMenuChoice(gText_32Spaces, XPOS_CHOICES, menuOffset * YPOS_SPACING, 0);

    ConvertUIntToDecimalStringN(&text[0], selection, STR_CONV_MODE_LEFT_ALIGN, 3);
    DrawOptionMenuChoice(text, XPOS_CHOICES, menuOffset * YPOS_SPACING, 0);
}

static u8 DebugRange_DifficultySkipProcessInput(u8 menuOffset, u8 selection)
{
    if (JOY_REPEAT(DPAD_RIGHT | A_BUTTON))
    {
        if(selection == ROGUE_MAX_BOSS_COUNT - 1)
            selection = 0;
        else
            selection++;

        sArrowPressed = TRUE;
    }
    if (JOY_REPEAT(DPAD_LEFT))
    {
        if (selection == 0)
            selection = ROGUE_MAX_BOSS_COUNT - 1;
        else
            selection--;

        sArrowPressed = TRUE;
    }
    
    return selection;
}

static u8 DebugRange_ForcedRouteProcessInput(u8 menuOffset, u8 selection)
{
    if (JOY_REPEAT(DPAD_RIGHT | A_BUTTON))
    {
        if(selection == gRogueRouteTable.routeCount)
            selection = 0;
        else
            selection++;

        sArrowPressed = TRUE;
    }
    if (JOY_REPEAT(DPAD_LEFT))
    {
        if (selection == 0)
            selection = gRogueRouteTable.routeCount;
        else
            selection--;

        sArrowPressed = TRUE;
    }
    
    return selection;
}

static void DebugRange_ForcedRouteDrawChoices(u8 menuOffset, u8 selection)
{
    u8 text[16];

    DrawOptionMenuChoice(gText_32Spaces, XPOS_CHOICES, menuOffset * YPOS_SPACING, 0);
    DrawOptionMenuChoice(gText_32Spaces, XPOS_CHOICES + 55, menuOffset * YPOS_SPACING, 0);

    if(selection != 0 && selection <= gRogueRouteTable.routeCount)
    {
        // Offset to remove "Rogue_Route_"
        DrawOptionMenuChoice(&gRogueRouteTable.routes[selection - 1].map.debugName[12], XPOS_CHOICES, menuOffset * YPOS_SPACING, 0);
    }
    else
    {
        ConvertUIntToDecimalStringN(&text[0], selection, STR_CONV_MODE_LEFT_ALIGN, 3);
        DrawOptionMenuChoice(text, XPOS_CHOICES, menuOffset * YPOS_SPACING, 0);
    }
    

}

static u8 DebugRange_ForcedTeamProcessInput(u8 menuOffset, u8 selection)
{
    if (JOY_REPEAT(DPAD_RIGHT | A_BUTTON))
    {
        if(selection == TEAM_NUM_COUNT)
            selection = 0;
        else
            selection++;

        sArrowPressed = TRUE;
    }
    if (JOY_REPEAT(DPAD_LEFT))
    {
        if (selection == 0)
            selection = TEAM_NUM_COUNT;
        else
            selection--;

        sArrowPressed = TRUE;
    }
    
    return selection;
}

#endif

static u8 Empty_ProcessInput(u8 menuOffset, u8 selection)
{
    return 0;
}

static void Empty_DrawChoices(u8 menuOffset, u8 selection)
{

}

static void DrawDescriptionOptionMenuText(u8 submenu, u8 selection)
{
    u8 text[64];
    u8* str;

    u8 menuItem = GetMenuItemFor(submenu, selection);

    FillWindowPixelBuffer(WIN_TEXT_OPTION, PIXEL_FILL(1));

    if(submenu == SUBMENUITEM_GAME_MODES)
    {
        // Use entire box for the mode description

        // Element description
        if(sOptionMenuItems[menuItem].descCount != 0)
        {
            if(sOptionMenuItems[menuItem].descCount > 1)
            {
                u8 value = min(GetMenuItemValue(menuItem), sOptionMenuItems[menuItem].descCount - 1);
                AddTextPrinterParameterized(WIN_TEXT_OPTION, FONT_NORMAL, sOptionMenuItems[menuItem].multiDesc[value], 8, 1, TEXT_SKIP_DRAW, NULL);
            }
            else
            {
                AddTextPrinterParameterized(WIN_TEXT_OPTION, FONT_NORMAL, sOptionMenuItems[menuItem].singleDesc, 8, 1, TEXT_SKIP_DRAW, NULL);
            }
        }
    }
    else
    {
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

        case DIFFICULTY_LEVEL_AVERAGE:
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
        if(sOptionMenuItems[menuItem].descCount != 0)
        {
            if(sOptionMenuItems[menuItem].descCount > 1)
            {
                u8 value = min(GetMenuItemValue(menuItem), sOptionMenuItems[menuItem].descCount - 1);
                AddTextPrinterParameterized(WIN_TEXT_OPTION, FONT_NORMAL, sOptionMenuItems[menuItem].multiDesc[value], 8, 17, TEXT_SKIP_DRAW, NULL);
            }
            else
            {
                AddTextPrinterParameterized(WIN_TEXT_OPTION, FONT_NORMAL, sOptionMenuItems[menuItem].singleDesc, 8, 17, TEXT_SKIP_DRAW, NULL);
            }
        }
    }

    CopyWindowToVram(WIN_TEXT_OPTION, COPYWIN_FULL);
}

static void DrawOptionMenuTexts(u8 submenu, u8 topIndex)
{
    u8 i;
    FillWindowPixelBuffer(WIN_OPTIONS, PIXEL_FILL(1));

    for (i = 0; i < MAX_MENUITEM_TO_DISPLAY; i++)
    {
        u8 menuItem = GetMenuItemFor(submenu, i + topIndex);

        AddTextPrinterParameterized(WIN_OPTIONS, FONT_NORMAL, sOptionMenuItems[menuItem].itemName, XPOS_TITLES, (i * YPOS_SPACING) + 1, TEXT_SKIP_DRAW, NULL);

        if(menuItem == MENUITEM_CANCEL || menuItem == MENUITEM_SAVE_AND_EXIT)
            break;
    }

    for (i = 0; i < MAX_MENUITEM_TO_DISPLAY; i++)
    {
        u8 menuItem = GetMenuItemFor(submenu, i + topIndex);
    
        sOptionMenuItems[menuItem].drawChoices(i, GetMenuItemValue(menuItem));

        if(menuItem == MENUITEM_CANCEL || menuItem == MENUITEM_SAVE_AND_EXIT)
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
        return Rogue_GetConfigToggle(CONFIG_TOGGLE_EXP_ALL);

    case MENUITEM_MENU_TOGGLE_OVER_LVL:
        return Rogue_GetConfigToggle(CONFIG_TOGGLE_OVER_LVL);

    case MENUITEM_MENU_TOGGLE_EV_GAIN:
        return Rogue_GetConfigToggle(CONFIG_TOGGLE_EV_GAIN);

    case MENUITEM_MENU_TOGGLE_OVERWORLD_MONS:
        return Rogue_GetConfigToggle(CONFIG_TOGGLE_OVERWORLD_MONS);

    case MENUITEM_MENU_TOGGLE_BAG_WIPE:
        return Rogue_GetConfigToggle(CONFIG_TOGGLE_BAG_WIPE);

    case MENUITEM_MENU_TOGGLE_SWITCH_MODE:
        return Rogue_GetConfigToggle(CONFIG_TOGGLE_SWITCH_MODE);

    case MENUITEM_MENU_TOGGLE_DIVERSE_TRAINERS:
        return Rogue_GetConfigToggle(CONFIG_TOGGLE_DIVERSE_TRAINERS);

    case MENUITEM_MENU_TOGGLE_AFFECTION:
        return Rogue_GetConfigToggle(CONFIG_TOGGLE_AFFECTION);

    case MENUITEM_MENU_TOGGLE_RELEASE_MONS:
        return Rogue_GetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS);

    // Trainers
    //
    case MENUITEM_MENU_TOGGLE_TRAINER_ROGUE:
        return Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_ROGUE);

    case MENUITEM_MENU_TOGGLE_TRAINER_KANTO:
        return Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_KANTO);

    case MENUITEM_MENU_TOGGLE_TRAINER_JOHTO:
        return Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_JOHTO);

    case MENUITEM_MENU_TOGGLE_TRAINER_HOENN:
        return Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_HOENN);

#ifdef ROGUE_EXPANSION
    case MENUITEM_MENU_TOGGLE_TRAINER_SINNOH:
        return Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_SINNOH);

    case MENUITEM_MENU_TOGGLE_TRAINER_UNOVA:
        return Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_UNOVA);

    case MENUITEM_MENU_TOGGLE_TRAINER_KALOS:
        return Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_KALOS);

    case MENUITEM_MENU_TOGGLE_TRAINER_ALOLA:
        return Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_ALOLA);

    case MENUITEM_MENU_TOGGLE_TRAINER_GALAR:
        return Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_GALAR);

    case MENUITEM_MENU_TOGGLE_TRAINER_PALDEA:
        return Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_PALDEA);
#endif


    case MENUITEM_MENU_SLIDER_TRAINER:
        return Rogue_GetConfigRange(CONFIG_RANGE_TRAINER);

    case MENUITEM_MENU_SLIDER_ITEM:
        return Rogue_GetConfigRange(CONFIG_RANGE_ITEM);

    case MENUITEM_MENU_SLIDER_LEGENDARY:
        return Rogue_GetConfigRange(CONFIG_RANGE_LEGENDARY);

    case MENUITEM_MENU_SLIDER_BATTLE_FORMAT:
        return Rogue_GetConfigRange(CONFIG_RANGE_BATTLE_FORMAT);

    case MENUITEM_MENU_SLIDER_GAME_MODE_STANDARD:
    case MENUITEM_MENU_SLIDER_GAME_MODE_RAINBOW:
    case MENUITEM_MENU_SLIDER_GAME_MODE_OFFICIAL:
    case MENUITEM_MENU_SLIDER_GAME_MODE_GAUNTLET:
    case MENUITEM_MENU_SLIDER_GAME_MODE_RAINBOW_GAUNTLET:
        return Rogue_GetConfigRange(CONFIG_RANGE_GAME_MODE_NUM) == (ROGUE_GAME_MODE_STANDARD + menuItem - MENUITEM_MENU_SLIDER_GAME_MODE_STANDARD);

#ifdef ROGUE_DEBUG
    case MENUITEM_MENU_DEBUG_TOGGLE_INFO_PANEL:
        return RogueDebug_GetConfigToggle(DEBUG_TOGGLE_INFO_PANEL);

    case MENUITEM_MENU_DEBUG_TOGGLE_STEAL_TEAM:
        return RogueDebug_GetConfigToggle(DEBUG_TOGGLE_STEAL_TEAM);

    case MENUITEM_MENU_DEBUG_TOGGLE_TRAINER_LVL_5:
        return RogueDebug_GetConfigToggle(DEBUG_TOGGLE_TRAINER_LVL_5);

    case MENUITEM_MENU_DEBUG_TOGGLE_ALLOW_SAVE_SCUM:
        return RogueDebug_GetConfigToggle(DEBUG_TOGGLE_ALLOW_SAVE_SCUM);

    case MENUITEM_MENU_DEBUG_TOGGLE_INSTANT_CAPTURE:
        return RogueDebug_GetConfigToggle(DEBUG_TOGGLE_INSTANT_CAPTURE);

    case MENUITEM_MENU_DEBUG_TOGGLE_TOD_TINT_USE_PLAYER_COLOUR:
        return RogueDebug_GetConfigToggle(DEBUG_TOGGLE_TOD_TINT_USE_PLAYER_COLOUR);

    case MENUITEM_MENU_DEBUG_TOGGLE_DEBUG_SHOPS:
        return RogueDebug_GetConfigToggle(DEBUG_TOGGLE_DEBUG_SHOPS);

    case MENUITEM_MENU_DEBUG_TOGGLE_DEBUG_LEGENDS:
        return RogueDebug_GetConfigToggle(DEBUG_TOGGLE_DEBUG_LEGENDS);

    case MENUITEM_MENU_DEBUG_TOGGLE_DEBUG_MON_QUERY:
        return RogueDebug_GetConfigToggle(DEBUG_TOGGLE_DEBUG_MON_QUERY);

    case MENUITEM_MENU_DEBUG_TOGGLE_DEBUG_ITEM_QUERY:
        return RogueDebug_GetConfigToggle(DEBUG_TOGGLE_DEBUG_ITEM_QUERY);

    case MENUITEM_MENU_DEBUG_TOGGLE_HIDE_FOLLOWER:
        return RogueDebug_GetConfigToggle(DEBUG_TOGGLE_HIDE_FOLLOWER);

    case MENUITEM_MENU_DEBUG_TOGGLE_STOP_WILD_SPAWNING:
        return RogueDebug_GetConfigToggle(DEBUG_TOGGLE_STOP_WILD_SPAWNING);

    case MENUITEM_MENU_DEBUG_TOGGLE_DISABLE_ASSISTANT_TIMEOUT:
        return RogueDebug_GetConfigToggle(DEBUG_TOGGLE_DISABLE_ASSISTANT_TIMEOUT);


    case MENUITEM_MENU_DEBUG_RANGE_START_DIFFICULTY:
        return RogueDebug_GetConfigRange(DEBUG_RANGE_START_DIFFICULTY);

    case MENUITEM_MENU_DEBUG_RANGE_FORCED_ROUTE:
        return RogueDebug_GetConfigRange(DEBUG_RANGE_FORCED_ROUTE);

    case MENUITEM_MENU_DEBUG_RANGE_FORCED_EVIL_TEAM:
        return RogueDebug_GetConfigRange(DEBUG_RANGE_FORCED_EVIL_TEAM);
#endif
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
        Rogue_SetConfigToggle(CONFIG_TOGGLE_EXP_ALL, value);
        break;

    case MENUITEM_MENU_TOGGLE_OVER_LVL:
        Rogue_SetConfigToggle(CONFIG_TOGGLE_OVER_LVL, value);
        break;

    case MENUITEM_MENU_TOGGLE_EV_GAIN:
        Rogue_SetConfigToggle(CONFIG_TOGGLE_EV_GAIN, value);
        break;

    case MENUITEM_MENU_TOGGLE_OVERWORLD_MONS:
        Rogue_SetConfigToggle(CONFIG_TOGGLE_OVERWORLD_MONS, value);
        break;

    case MENUITEM_MENU_TOGGLE_BAG_WIPE:
        Rogue_SetConfigToggle(CONFIG_TOGGLE_BAG_WIPE, value);
        break;

    case MENUITEM_MENU_TOGGLE_SWITCH_MODE:
        Rogue_SetConfigToggle(CONFIG_TOGGLE_SWITCH_MODE, value);
        break;

    case MENUITEM_MENU_TOGGLE_DIVERSE_TRAINERS:
        Rogue_SetConfigToggle(CONFIG_TOGGLE_DIVERSE_TRAINERS, value);
        break;

    case MENUITEM_MENU_TOGGLE_AFFECTION:
        Rogue_SetConfigToggle(CONFIG_TOGGLE_AFFECTION, value);
        break;

    case MENUITEM_MENU_TOGGLE_RELEASE_MONS:
        Rogue_SetConfigToggle(CONFIG_TOGGLE_RELEASE_MONS, value);
        break;

    // Trainers
    //
    case MENUITEM_MENU_TOGGLE_TRAINER_ROGUE:
        Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_ROGUE, value);
        break;

    case MENUITEM_MENU_TOGGLE_TRAINER_KANTO:
        Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_KANTO, value);
        break;

    case MENUITEM_MENU_TOGGLE_TRAINER_JOHTO:
        Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_JOHTO, value);
        break;

    case MENUITEM_MENU_TOGGLE_TRAINER_HOENN:
        Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_HOENN, value);
        break;

#ifdef ROGUE_EXPANSION
    case MENUITEM_MENU_TOGGLE_TRAINER_SINNOH:
        Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_SINNOH, value);
        break;

    case MENUITEM_MENU_TOGGLE_TRAINER_UNOVA:
        Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_UNOVA, value);
        break;

    case MENUITEM_MENU_TOGGLE_TRAINER_KALOS:
        Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_KALOS, value);
        break;

    case MENUITEM_MENU_TOGGLE_TRAINER_ALOLA:
        Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_ALOLA, value);
        break;

    case MENUITEM_MENU_TOGGLE_TRAINER_GALAR:
        Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_GALAR, value);
        break;

    case MENUITEM_MENU_TOGGLE_TRAINER_PALDEA:
        Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_PALDEA, value);
        break;
#endif

    case MENUITEM_MENU_SLIDER_TRAINER:
        Rogue_SetConfigRange(CONFIG_RANGE_TRAINER, value);
        break;

    case MENUITEM_MENU_SLIDER_ITEM:
        Rogue_SetConfigRange(CONFIG_RANGE_ITEM, value);
        break;

    case MENUITEM_MENU_SLIDER_LEGENDARY:
        Rogue_SetConfigRange(CONFIG_RANGE_LEGENDARY, value);
        break;

    case MENUITEM_MENU_SLIDER_BATTLE_FORMAT:
        Rogue_SetConfigRange(CONFIG_RANGE_BATTLE_FORMAT, value);
        break;

    case MENUITEM_MENU_SLIDER_GAME_MODE_STANDARD:
    case MENUITEM_MENU_SLIDER_GAME_MODE_RAINBOW:
    case MENUITEM_MENU_SLIDER_GAME_MODE_OFFICIAL:
    case MENUITEM_MENU_SLIDER_GAME_MODE_GAUNTLET:
    case MENUITEM_MENU_SLIDER_GAME_MODE_RAINBOW_GAUNTLET:
        if(value != 0)
            Rogue_SetConfigRange(CONFIG_RANGE_GAME_MODE_NUM, ROGUE_GAME_MODE_STANDARD + menuItem - MENUITEM_MENU_SLIDER_GAME_MODE_STANDARD);
        else
            Rogue_SetConfigRange(CONFIG_RANGE_GAME_MODE_NUM, ROGUE_GAME_MODE_STANDARD);
        break;

#ifdef ROGUE_DEBUG
    case MENUITEM_MENU_DEBUG_TOGGLE_INFO_PANEL:
        RogueDebug_SetConfigToggle(DEBUG_TOGGLE_INFO_PANEL, value);
        break;

    case MENUITEM_MENU_DEBUG_TOGGLE_STEAL_TEAM:
        RogueDebug_SetConfigToggle(DEBUG_TOGGLE_STEAL_TEAM, value);
        break;

    case MENUITEM_MENU_DEBUG_TOGGLE_TRAINER_LVL_5:
        RogueDebug_SetConfigToggle(DEBUG_TOGGLE_TRAINER_LVL_5, value);
        break;

    case MENUITEM_MENU_DEBUG_TOGGLE_ALLOW_SAVE_SCUM:
        RogueDebug_SetConfigToggle(DEBUG_TOGGLE_ALLOW_SAVE_SCUM, value);
        break;

    case MENUITEM_MENU_DEBUG_TOGGLE_INSTANT_CAPTURE:
        RogueDebug_SetConfigToggle(DEBUG_TOGGLE_INSTANT_CAPTURE, value);
        break;

    case MENUITEM_MENU_DEBUG_TOGGLE_TOD_TINT_USE_PLAYER_COLOUR:
        RogueDebug_SetConfigToggle(DEBUG_TOGGLE_TOD_TINT_USE_PLAYER_COLOUR, value);
        break;

    case MENUITEM_MENU_DEBUG_TOGGLE_DEBUG_SHOPS:
        RogueDebug_SetConfigToggle(DEBUG_TOGGLE_DEBUG_SHOPS, value);
        break;

    case MENUITEM_MENU_DEBUG_TOGGLE_DEBUG_LEGENDS:
        RogueDebug_SetConfigToggle(DEBUG_TOGGLE_DEBUG_LEGENDS, value);
        break;

    case MENUITEM_MENU_DEBUG_TOGGLE_DEBUG_MON_QUERY:
        RogueDebug_SetConfigToggle(DEBUG_TOGGLE_DEBUG_MON_QUERY, value);
        break;

    case MENUITEM_MENU_DEBUG_TOGGLE_DEBUG_ITEM_QUERY:
        RogueDebug_SetConfigToggle(DEBUG_TOGGLE_DEBUG_ITEM_QUERY, value);
        break;

    case MENUITEM_MENU_DEBUG_TOGGLE_HIDE_FOLLOWER:
        RogueDebug_SetConfigToggle(DEBUG_TOGGLE_HIDE_FOLLOWER, value);
        break;

    case MENUITEM_MENU_DEBUG_TOGGLE_STOP_WILD_SPAWNING:
        RogueDebug_SetConfigToggle(DEBUG_TOGGLE_STOP_WILD_SPAWNING, value);
        break;

    case MENUITEM_MENU_DEBUG_TOGGLE_DISABLE_ASSISTANT_TIMEOUT:
        RogueDebug_SetConfigToggle(DEBUG_TOGGLE_DISABLE_ASSISTANT_TIMEOUT, value);
        break;

    case MENUITEM_MENU_DEBUG_RANGE_START_DIFFICULTY:
        RogueDebug_SetConfigRange(DEBUG_RANGE_START_DIFFICULTY, value);
        break;

    case MENUITEM_MENU_DEBUG_RANGE_FORCED_ROUTE:
        RogueDebug_SetConfigRange(DEBUG_RANGE_FORCED_ROUTE, value);
        break;

    case MENUITEM_MENU_DEBUG_RANGE_FORCED_EVIL_TEAM:
        RogueDebug_SetConfigRange(DEBUG_RANGE_FORCED_EVIL_TEAM, value);
        break;
#endif
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
        bottomValid = bottom <= 19; // && bottom >= bottom;

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
