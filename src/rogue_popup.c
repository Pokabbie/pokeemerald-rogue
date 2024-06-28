#include "global.h"
#include "battle_pyramid.h"
#include "bg.h"
#include "event_data.h"
#include "field_weather.h"
#include "graphics.h"
#include "data.h"
#include "gpu_regs.h"
#include "international_string_util.h"
#include "item.h"
#include "item_icon.h"
#include "main.h" // temp
#include "menu.h"
#include "palette.h"
#include "party_menu.h"
#include "script.h"
#include "start_menu.h"
#include "string_util.h"
#include "sound.h"
#include "task.h"
#include "text.h"
#include "constants/battle_frontier.h"
#include "constants/items.h"
#include "constants/layouts.h"
#include "constants/region_map_sections.h"
#include "constants/weather.h"
#include "constants/songs.h"

#include "rogue.h"
#include "rogue_campaign.h"
#include "rogue_controller.h"
#include "rogue_debug.h"
#include "rogue_followmon.h"
#include "rogue_hub.h"
#include "rogue_pokedex.h"
#include "rogue_popup.h"
#include "rogue_quest.h"
#include "rogue_timeofday.h"

#define POPUP_QUEUE_CAPACITY 16

extern const u32 gItemIcon_RogueStatusMoney[];
extern const u32 gItemIconPalette_RogueStatusStarCustom[];

enum
{
    POPUP_ANIM_NONE,
    POPUP_ANIM_SLIDE_VERTICAL,
    POPUP_ANIM_SLIDE_HORIZONTAL,
};

enum
{
    POPUP_ICON_MODE_NONE,
    POPUP_ICON_MODE_ITEM,
    POPUP_ICON_MODE_POKEMON,
    POPUP_ICON_MODE_CUSTOM,
};

enum
{
    TEXT_EXPAND_NONE,
    TEXT_EXPAND_SPECIES_NAME,
    TEXT_EXPAND_PARTY_NICKNAME,
    TEXT_EXPAND_ITEM_NAME,
    TEXT_EXPAND_UNSIGNED_NUMBER,
};

enum
{
    POPUP_CUSTOM_ICON_POKEDEX,
    POPUP_CUSTOM_ICON_CLOUD,
    POPUP_CUSTOM_ICON_MONEY,
    POPUP_CUSTOM_ICON_TYPE_NORMAL,
    POPUP_CUSTOM_ICON_TYPE_FIGHTING,
    POPUP_CUSTOM_ICON_TYPE_FLYING,
    POPUP_CUSTOM_ICON_TYPE_POISON,
    POPUP_CUSTOM_ICON_TYPE_GROUND,
    POPUP_CUSTOM_ICON_TYPE_ROCK,
    POPUP_CUSTOM_ICON_TYPE_BUG,
    POPUP_CUSTOM_ICON_TYPE_GHOST,
    POPUP_CUSTOM_ICON_TYPE_STEEL,
    POPUP_CUSTOM_ICON_TYPE_MYSTERY,
    POPUP_CUSTOM_ICON_TYPE_FIRE,
    POPUP_CUSTOM_ICON_TYPE_WATER,
    POPUP_CUSTOM_ICON_TYPE_GRASS,
    POPUP_CUSTOM_ICON_TYPE_ELECTRIC,
    POPUP_CUSTOM_ICON_TYPE_PSYCHIC,
    POPUP_CUSTOM_ICON_TYPE_ICE,
    POPUP_CUSTOM_ICON_TYPE_DRAGON,
    POPUP_CUSTOM_ICON_TYPE_DARK,
    POPUP_CUSTOM_ICON_TYPE_FAIRY,
    POPUP_CUSTOM_ICON_COUNT,
};

struct PopupRequestTemplate
{
    u8 enterAnim;
    u8 exitAnim;
    u8 animDuration;
    u8 iconMode;
    u8 left;
    u8 down;
    u8 width;
    u8 height;
    u8 iconLeft;
    u8 iconDown;
    u8 iconWidth;
    u8 iconHeight;
    bool8 generateBorder;
    bool8 transparentText;
};

struct PopupRequest
{
    const u8* titleText;
    const u8* subtitleText;
    u32 expandTextData[3];
    u16 expandTextType[3];
    u8 titleTextCapacity;
    u8 templateId;
    u16 iconId;
    u16 displayDuration;
    u16 soundEffect;
    u16 fanfare;
    bool8 scriptAudioOnly : 1;
    bool8 cantBeSkipped : 1;
};

struct PopupManager
{
    struct PopupRequest requestQueue[POPUP_QUEUE_CAPACITY];
    u16 lastWeatherPopup;
    u8 windowId;
    u8 iconWindowId;
    u8 taskId;
    u8 lastShownId;
    u8 queuedId;
    u8 partyNotificationCounter;
    bool8 wasEnabled : 1;
    bool8 forceEnabled : 1;
    bool8 forceEnabledMuteAudio : 1;
    bool8 forceEnabledFromScript : 1;
    bool8 forceEnabledCanSkip : 1;
    bool8 hasPopupBeenSkipped : 1;
};

struct CustomIcon
{
    u32 const* icon;
    u32 const* palette;
};

static struct CustomIcon const sRoguePopupCustomIcons[POPUP_CUSTOM_ICON_COUNT] = 
{
    [POPUP_CUSTOM_ICON_POKEDEX] = 
    {
        .icon = gItemIcon_Pokedex,
        .palette = gItemIconPalette_Pokedex
    },
    [POPUP_CUSTOM_ICON_CLOUD] = 
    {
        .icon = gItemIcon_Cloud,
        .palette = gItemIconPalette_Cloud
    },
    [POPUP_CUSTOM_ICON_MONEY] = 
    {
        .icon = gItemIcon_RogueStatusMoney,
        .palette = gItemIconPalette_RogueStatusStarCustom
    },
    [POPUP_CUSTOM_ICON_TYPE_NORMAL] = 
    {
        .icon = gItemIcon_TypeNormal,
        .palette = gItemIconPalette_TypeNormal
    },
    [POPUP_CUSTOM_ICON_TYPE_FIGHTING] = 
    {
        .icon = gItemIcon_TypeFighting,
        .palette = gItemIconPalette_TypeFighting
    },
    [POPUP_CUSTOM_ICON_TYPE_FLYING] = 
    {
        .icon = gItemIcon_TypeFlying,
        .palette = gItemIconPalette_TypeFlying
    },
    [POPUP_CUSTOM_ICON_TYPE_POISON] = 
    {
        .icon = gItemIcon_TypePoison,
        .palette = gItemIconPalette_TypePoison
    },
    [POPUP_CUSTOM_ICON_TYPE_GROUND] = 
    {
        .icon = gItemIcon_TypeGround,
        .palette = gItemIconPalette_TypeGround
    },
    [POPUP_CUSTOM_ICON_TYPE_ROCK] = 
    {
        .icon = gItemIcon_TypeRock,
        .palette = gItemIconPalette_TypeRock
    },
    [POPUP_CUSTOM_ICON_TYPE_BUG] = 
    {
        .icon = gItemIcon_TypeBug,
        .palette = gItemIconPalette_TypeBug
    },
    [POPUP_CUSTOM_ICON_TYPE_GHOST] = 
    {
        .icon = gItemIcon_TypeGhost,
        .palette = gItemIconPalette_TypeGhost
    },
    [POPUP_CUSTOM_ICON_TYPE_STEEL] = 
    {
        .icon = gItemIcon_TypeSteel,
        .palette = gItemIconPalette_TypeSteel
    },
    [POPUP_CUSTOM_ICON_TYPE_MYSTERY] = 
    {
        .icon = gItemIcon_TypeMystery,
        .palette = gItemIconPalette_TypeMystery
    },
    [POPUP_CUSTOM_ICON_TYPE_FIRE] = 
    {
        .icon = gItemIcon_TypeFire,
        .palette = gItemIconPalette_TypeFire
    },
    [POPUP_CUSTOM_ICON_TYPE_WATER] = 
    {
        .icon = gItemIcon_TypeWater,
        .palette = gItemIconPalette_TypeWater
    },
    [POPUP_CUSTOM_ICON_TYPE_GRASS] = 
    {
        .icon = gItemIcon_TypeGrass,
        .palette = gItemIconPalette_TypeGrass
    },
    [POPUP_CUSTOM_ICON_TYPE_ELECTRIC] = 
    {
        .icon = gItemIcon_TypeElectric,
        .palette = gItemIconPalette_TypeElectric
    },
    [POPUP_CUSTOM_ICON_TYPE_PSYCHIC] = 
    {
        .icon = gItemIcon_TypePsychic,
        .palette = gItemIconPalette_TypePsychic
    },
    [POPUP_CUSTOM_ICON_TYPE_ICE] = 
    {
        .icon = gItemIcon_TypeIce,
        .palette = gItemIconPalette_TypeIce
    },
    [POPUP_CUSTOM_ICON_TYPE_DRAGON] = 
    {
        .icon = gItemIcon_TypeDragon,
        .palette = gItemIconPalette_TypeDragon
    },
    [POPUP_CUSTOM_ICON_TYPE_DARK] = 
    {
        .icon = gItemIcon_TypeDark,
        .palette = gItemIconPalette_TypeDark
    },
    [POPUP_CUSTOM_ICON_TYPE_FAIRY] = 
    {
        .icon = gItemIcon_TypeFairy,
        .palette = gItemIconPalette_TypeFairy
    }
};

static EWRAM_DATA struct PopupManager sRoguePopups = { 0 };

extern const u8 gText_Space[];

static const u8 sText_Popup_QuestComplete[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Quest Completed!");
static const u8 sText_Popup_QuestFail[] = _("{COLOR LIGHT_RED}{SHADOW RED}Quest Failed");
static const u8 sText_Popup_QuestUnlocked[] = _("{COLOR LIGHT_BLUE}{SHADOW BLUE}Quest Unlocked!");
static const u8 sText_Popup_LegendaryClause[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Clause Activated!");
static const u8 sText_Popup_RoamerLegendary[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Started Roaming!");
static const u8 sText_Popup_GiftPokemon[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Gift Pokémon!");
static const u8 sText_Popup_GiftShinyPokemon[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Gift Shiny {PKMN}!");
static const u8 sText_Popup_GiftCustomPokemon[] = _("{COLOR LIGHT_BLUE}{SHADOW BLUE}Gift Unique {PKMN}!");
static const u8 sText_Popup_DaycarePokemon[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Pokémon Egg");
static const u8 sText_Popup_UniquePokemon[] = _("Unique Pokémon");
static const u8 sText_Popup_UniquePokemonSubtitle[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Detected nearby!");
static const u8 sText_Popup_None[] = _("");

static const u8 sText_Popup_NewMoves[] = _("{COLOR LIGHT_BLUE}{SHADOW BLUE}New Moves!");
static const u8 sText_Popup_NewEvolution[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}New Evolution!");

static const u8 sText_Popup_PokemonChain[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}{STR_VAR_1} Chain");
static const u8 sText_Popup_PokemonChainBroke[] = _("{COLOR LIGHT_RED}{SHADOW RED}Broken Chain");

static const u8 sText_Popup_BagFull[] = _("{COLOR LIGHT_RED}{SHADOW RED}Bag too full.");
static const u8 sText_Popup_SingleItem[] = _("{STR_VAR_1}");
static const u8 sText_Popup_MultipleItem[] = _("{STR_VAR_1} {COLOR LIGHT_GREEN}{SHADOW GREEN}x{STR_VAR_2}");
static const u8 sText_Popup_Money[] = _("¥{STR_VAR_1}");
static const u8 sText_Popup_LostItem[] = _("{COLOR LIGHT_RED}{SHADOW RED}Lost Item.");
static const u8 sText_Popup_LostMoney[] = _("{COLOR LIGHT_RED}{SHADOW RED}Lost Money.");
static const u8 sText_Popup_UnlockedInShops[] = _("{COLOR LIGHT_BLUE}{SHADOW BLUE}Can now be bought!");
static const u8 sText_Popup_UnlockedDecor[] = _("{COLOR LIGHT_BLUE}{SHADOW BLUE}Decor Unlocked!");
static const u8 sText_Popup_TypePlateItem[] = _("Type Plates");
static const u8 sText_Popup_TypeMemoryItem[] = _("Type Memories");

static const u8 sText_Popup_BerriesRequipSuccess[] = _("{COLOR LIGHT_BLUE}{SHADOW BLUE}Re-equipped");
static const u8 sText_Popup_BerriesRequipSuccessSubtitle[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Taken from Bag");

static const u8 sText_Popup_BerriesRequipFail[] = _("Re-equip Fail");
static const u8 sText_Popup_BerriesRequipFailSubtitle[] = _("{COLOR LIGHT_RED}{SHADOW RED}None in Bag");

static const u8 sText_Popup_WeakLegendaryClause[] = _("{COLOR LIGHT_BLUE}{SHADOW BLUE}Basic Legend");
static const u8 sText_Popup_StrongLegendaryClause[] = _("{COLOR LIGHT_RED}{SHADOW RED}Strong Legend");

static const u8 sText_Popup_CampaignNoneScore[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Campaign Active!");
static const u8 sText_Popup_CampaignHighScore[] = _("{COLOR LIGHT_BLUE}{SHADOW BLUE}Aim for High Score!");
static const u8 sText_Popup_CampaignLowScore[] = _("{COLOR RED}{SHADOW LIGHT_RED}Aim for Low Score!");

static const u8 sText_Popup_SafariArea[] = _("{COLOR LIGHT_BLUE}{SHADOW BLUE}Safari Area");

static const u8 sText_Popup_StarterWarning[] = _("{COLOR LIGHT_RED}{SHADOW RED}Evos. Disabled");
static const u8 sText_Popup_GotWeaker[] = _("{COLOR LIGHT_RED}{SHADOW RED}Got Weaker.");
static const u8 sText_Popup_GotStronger[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Got Stronger!");
static const u8 sText_Popup_LostShiny[] = _("{COLOR LIGHT_RED}{SHADOW RED}Lost Shininess.");
static const u8 sText_Popup_GotShiny[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Became Shiny!");
static const u8 sText_Popup_BecameMale[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Became Male!");
static const u8 sText_Popup_BecameFemale[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Became Female!");

static const u8 sText_Popup_EncounterChain[] = _("{COLOR LIGHT_BLUE}{SHADOW BLUE}Encounter Chain");
static const u8 sText_Popup_EncounterChainEnd[] = _("{COLOR RED}{SHADOW LIGHT_RED}Chain Lost");

static const u8 sText_Popup_PokedexUnlock[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Received Pokedex!");
static const u8 sText_Popup_PokedexUpgrade[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Pokedex Upgraded!");

static const u8 sText_Popup_BagUpdate[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Bag Upgraded!");
static const u8 sText_Popup_UpgradeSlots[] = _("{COLOR LIGHT_BLUE}{SHADOW BLUE}+{STR_VAR_1} ({STR_VAR_2}) slots"); // assuming ITEM_BAG_SLOTS_PER_UPGRADE value


static const u8 sText_Popup_RogueAssistant[] = _("Rogue Assistant");
static const u8 sText_Popup_Connected[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Connected!");
static const u8 sText_Popup_Disconnected[] = _("{COLOR RED}{SHADOW LIGHT_RED}Disconnected.");

static const u8 sText_Popup_OutfitUnlocked[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Outfit Unlocked!");
static const u8 sText_Popup_ItsASecret[] = _("Shh… its a secret");

static const u8 sText_Popup_ExtraLifeTitle[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Extra Life!");
static const u8 sText_Popup_ExtraLifeSubtitle[] = _("{COLOR LIGHT_BLUE}{SHADOW BLUE}Sacred Ash used");

static const u8 sText_Popup_HealingFlaskRefilled[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Flask Refilled!");

static const u8 sText_Popup_FlightChargeRemaining[] = _("{STR_VAR_1} / {STR_VAR_2}");
static const u8 sText_Popup_FlightChargeSubtitle[] = _("{COLOR LIGHT_BLUE}{SHADOW BLUE}Flight Charges");

static const u8 sText_Popup_GymBadge[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Gym Badge {STR_VAR_1}");
static const u8 sText_Popup_EliteBadge[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Elite Badge {STR_VAR_1}");
static const u8 sText_Popup_ChampBadge[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Champion Badge");
static const u8 sText_Popup_VictoryLapGymBadge[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Victory Badge {STR_VAR_1}");
static const u8 sText_Popup_EarnBadge[] = _("Received badge!");

static const u8 sText_Popup_AdventureReplay[] = _("Adventure Replay");
static const u8 sText_Popup_AdventureReplaySubtitle[] = _("{COLOR LIGHT_GREEN}{SHADOW GREEN}Active");

static const u8 sText_Popup_QuestsDisabled[] = _("{COLOR LIGHT_RED}{SHADOW RED}Quests Inactive");
static const u8 sText_Popup_ChallengesDisabled[] = _("{COLOR LIGHT_RED}{SHADOW RED}Challenge Inactiv");
static const u8 sText_Popup_QuestsDisabledSubtitle[] = _("{COLOR LIGHT_BLUE}{SHADOW BLUE}Current Mode");

static const u8 sText_Popup_WeatherActive[] = _("{COLOR LIGHT_BLUE}{SHADOW BLUE}Weather Active");

static const u8 sWeatherNames[22][14] = {
    [WEATHER_NONE]               = _("None"),
    [WEATHER_SUNNY_CLOUDS]       = _("Sunny"),
    [WEATHER_SUNNY]              = _("Sunny"),
    [WEATHER_RAIN]               = _("Rain"),
    [WEATHER_SNOW]               = _("Snow"),
    [WEATHER_RAIN_THUNDERSTORM]  = _("Thunderstorm"),
#ifdef ROGUE_EXPANSION
    [WEATHER_PSYCHIC_FOG]        = _("Psychic Fog"),
    [WEATHER_MISTY_FOG]          = _("Misty Fog"),
#else
    [WEATHER_PSYCHIC_FOG]        = _("Fog"),
    [WEATHER_MISTY_FOG]          = _("Fog"),
#endif
    [WEATHER_VOLCANIC_ASH]       = _("Ash"),
    [WEATHER_SANDSTORM]          = _("Sandstorm"),
    [WEATHER_UNDERWATER]         = _("Underwater"),
    [WEATHER_SHADE]              = _("Shade"),
    [WEATHER_DROUGHT]            = _("Drought"),
    [WEATHER_DOWNPOUR]           = _("Downpour"),
    [WEATHER_UNDERWATER_BUBBLES] = _("Bubbles"),
    [WEATHER_ABNORMAL]           = _("Abnormal"),
    [WEATHER_ROUTE119_CYCLE]     = _("???"),
    [WEATHER_ROUTE123_CYCLE]     = _("???"),
};


#define DEFAULT_ANIM_DURATION 15
#define DEFAULT_DISPLAY_DURATION 90
#define SKIP_DISPLAY_DURATION 20
#define sStateNum           data[0]
#define tOnscreenTimer      data[1]
#define sDisplayTimer       data[2]
#define tIncomingPopUp      data[3]

enum
{
    POPUP_COMMON_CLASSIC,
    POPUP_COMMON_ITEM_TEXT,
    POPUP_COMMON_FIND_ITEM,
    POPUP_COMMON_POKEMON_TEXT,
    POPUP_COMMON_PARTY_INFO,
    POPUP_COMMON_INSTANT_POKEMON_TEXT,
    POPUP_COMMON_CUSTOM_ICON_TEXT,
    POPUP_COMMON_CUSTOM_ICON_SLIDE_TEXT,
};

static const struct PopupRequestTemplate sPopupRequestTemplates[] =
{
    [POPUP_COMMON_CLASSIC] = 
    {
        .enterAnim = POPUP_ANIM_SLIDE_VERTICAL,
        .exitAnim = POPUP_ANIM_SLIDE_VERTICAL,
        .animDuration = DEFAULT_ANIM_DURATION,
        .left = 1,
        .down = 1,
        .width = 10,
        .height = 4,
        .generateBorder = TRUE,
        .transparentText = FALSE,
    },
    [POPUP_COMMON_ITEM_TEXT] = 
    {
        .enterAnim = POPUP_ANIM_SLIDE_VERTICAL,
        .exitAnim = POPUP_ANIM_SLIDE_VERTICAL,
        .animDuration = DEFAULT_ANIM_DURATION,
        .generateBorder = FALSE,
        .transparentText = TRUE,
        .left = 10,
        .down = 0,
        .width = 10,
        .height = 4,
        
        .iconMode = POPUP_ICON_MODE_ITEM,
        .iconLeft = 7,
        .iconDown = 0,
        .iconWidth = 3,
        .iconHeight = 3,
    },
    [POPUP_COMMON_FIND_ITEM] = 
    {
        .enterAnim = POPUP_ANIM_NONE,
        .exitAnim = POPUP_ANIM_SLIDE_VERTICAL,
        .animDuration = DEFAULT_ANIM_DURATION,
        .generateBorder = FALSE,
        .transparentText = TRUE,
        .left = 10,
        .down = 0,
        .width = 10,
        .height = 4,
        
        .iconMode = POPUP_ICON_MODE_ITEM,
        .iconLeft = 7,
        .iconDown = 0,
        .iconWidth = 3,
        .iconHeight = 3,
    },
    [POPUP_COMMON_POKEMON_TEXT] = 
    {
        .enterAnim = POPUP_ANIM_SLIDE_VERTICAL,
        .exitAnim = POPUP_ANIM_SLIDE_VERTICAL,
        .animDuration = DEFAULT_ANIM_DURATION,
        .generateBorder = FALSE,
        .transparentText = TRUE,
        .left = 10,
        .down = 0,
        .width = 10,
        .height = 4,

        .iconMode = POPUP_ICON_MODE_POKEMON,
        .iconLeft = 6,
        .iconDown = 0,
        .iconWidth = 4,
        .iconHeight = 4,
    },
    [POPUP_COMMON_PARTY_INFO] = 
    {
        .enterAnim = POPUP_ANIM_SLIDE_VERTICAL,
        .exitAnim = POPUP_ANIM_SLIDE_VERTICAL,
        .animDuration = DEFAULT_ANIM_DURATION,
        .generateBorder = FALSE,
        .transparentText = TRUE,
        .left = 10,
        .down = 0,
        .width = 10,
        .height = 4,

        .iconMode = POPUP_ICON_MODE_POKEMON,
        .iconLeft = 6,
        .iconDown = 0,
        .iconWidth = 4,
        .iconHeight = 4,
    },
    [POPUP_COMMON_INSTANT_POKEMON_TEXT] = 
    {
        .enterAnim = POPUP_ANIM_NONE,
        .exitAnim = POPUP_ANIM_SLIDE_VERTICAL,
        .animDuration = DEFAULT_ANIM_DURATION,
        .generateBorder = FALSE,
        .transparentText = TRUE,
        .left = 10,
        .down = 0,
        .width = 10,
        .height = 4,

        .iconMode = POPUP_ICON_MODE_POKEMON,
        .iconLeft = 6,
        .iconDown = 0,
        .iconWidth = 4,
        .iconHeight = 4,
    },
    [POPUP_COMMON_CUSTOM_ICON_TEXT] = 
    {
        .enterAnim = POPUP_ANIM_NONE,
        .exitAnim = POPUP_ANIM_SLIDE_VERTICAL,
        .animDuration = DEFAULT_ANIM_DURATION,
        .generateBorder = FALSE,
        .transparentText = TRUE,
        .left = 10,
        .down = 0,
        .width = 10,
        .height = 4,
        
        .iconMode = POPUP_ICON_MODE_CUSTOM,
        .iconLeft = 7,
        .iconDown = 0,
        .iconWidth = 3,
        .iconHeight = 3,
    },
    [POPUP_COMMON_CUSTOM_ICON_SLIDE_TEXT] = 
    {
        .enterAnim = POPUP_ANIM_SLIDE_VERTICAL,
        .exitAnim = POPUP_ANIM_SLIDE_VERTICAL,
        .animDuration = DEFAULT_ANIM_DURATION,
        .generateBorder = FALSE,
        .transparentText = TRUE,
        .left = 10,
        .down = 0,
        .width = 10,
        .height = 4,
        
        .iconMode = POPUP_ICON_MODE_CUSTOM,
        .iconLeft = 7,
        .iconDown = 0,
        .iconWidth = 3,
        .iconHeight = 3,
    },
};

static void ShowQuestPopup(void);
static void HideQuestPopUpWindow(void);

static u8 GetActiveOnScreenDisplayTimer();
static void Task_QuestPopUpWindow(u8 taskId);
static void ShowQuestPopUpWindow(void);

static void ApplyPopupAnimation(struct PopupRequest* request, u16 timer, bool8);

void InitQuestWindow()
{
    sRoguePopups.windowId = WINDOW_NONE;
    sRoguePopups.iconWindowId = WINDOW_NONE;
}

static struct PopupRequest* GetCurrentPopup()
{
    return &sRoguePopups.requestQueue[sRoguePopups.lastShownId];
}

static u8 GetQuestPopUpWindowId(void)
{
    return sRoguePopups.windowId;
}

static u8 GetIconWindowId(void)
{
    return sRoguePopups.iconWindowId;
}

static void RemoveQuestPopUpWindow(void)
{
    if (sRoguePopups.windowId != WINDOW_NONE)
    {
        RemoveWindow(sRoguePopups.windowId);
        sRoguePopups.windowId = WINDOW_NONE;
    }

    if (sRoguePopups.iconWindowId != WINDOW_NONE)
    {
        RemoveWindow(sRoguePopups.iconWindowId);
        sRoguePopups.iconWindowId = WINDOW_NONE;
    }
}

static u8 AddQuestPopUpWindow(struct PopupRequest* request)
{
    struct PopupRequestTemplate const* template = &sPopupRequestTemplates[request->templateId];
    sRoguePopups.hasPopupBeenSkipped = FALSE;

    RemoveQuestPopUpWindow();

    sRoguePopups.windowId = AddWindowParameterized(
        0, 
        template->left,
        template->down,
        template->width,
        template->height, 
        15,
        0x107
    );

    // pal 14 is used the the borders

    if(template->iconMode != POPUP_ICON_MODE_NONE)
    {
        sRoguePopups.iconWindowId = AddWindowParameterized(
            0, 
            template->iconLeft,
            template->iconDown,
            template->iconWidth,
            template->iconHeight, 
            13,
            0x107 + (template->width * template->height)
        );
    }

    return sRoguePopups.windowId;
}

static void ShowQuestPopup(void)
{
    if (!FuncIsActiveTask(Task_QuestPopUpWindow))
    {
        struct PopupRequest* popupRequest = GetCurrentPopup();
        struct PopupRequestTemplate const* template = &sPopupRequestTemplates[popupRequest->templateId];
        
        sRoguePopups.taskId = CreateTask(Task_QuestPopUpWindow, 90);
        ApplyPopupAnimation(popupRequest, 0, FALSE);

        gTasks[sRoguePopups.taskId].sStateNum = 6;
        gTasks[sRoguePopups.taskId].sDisplayTimer = template->animDuration;
    }
    else
    {
        if (gTasks[sRoguePopups.taskId].sStateNum != 2)
            gTasks[sRoguePopups.taskId].sStateNum = 2;
        gTasks[sRoguePopups.taskId].tIncomingPopUp = 1;
    }
}

void Rogue_ClearPopupQueue(void)
{
    if (FuncIsActiveTask(Task_QuestPopUpWindow))
        HideQuestPopUpWindow();

    sRoguePopups.queuedId = 0;
    sRoguePopups.lastShownId = 0;
}


#define SKIP_POPUP_BUTTONS A_BUTTON | B_BUTTON | START_BUTTON

static bool8 ShouldSkipPopups()
{
    // only allow skipping in specifical from script scenarios
    if(sRoguePopups.forceEnabled && sRoguePopups.forceEnabledCanSkip)
    {
        // Always skip if pressing
        if(JOY_NEW(SKIP_POPUP_BUTTONS))
            return TRUE;

        // If holding skip after has been on screen for long enough
        if((JOY_HELD(SKIP_POPUP_BUTTONS) && GetActiveOnScreenDisplayTimer() >= 1))
            return TRUE;
    }

    return FALSE;
}

void Rogue_UpdatePopups(bool8 inOverworld, bool8 inputEnabled)
{
    bool8 enabled = inOverworld && inputEnabled; // May need to check this too? GetStartMenuWindowId

    START_TIMER(ROGUE_POPUPS);

    if(sRoguePopups.forceEnabled)
    {
        enabled = TRUE;
    }

    if(enabled)
    {
        // Just re-enabled so reset party notifications
        if(!sRoguePopups.wasEnabled)
            sRoguePopups.partyNotificationCounter = 0;

        if(sRoguePopups.queuedId != sRoguePopups.lastShownId)
        {
            if (!FuncIsActiveTask(Task_QuestPopUpWindow))
                ShowQuestPopup();
        }
        else if(sRoguePopups.forceEnabled)
        {
            // Disable script enabled mode now, as we've reached end of queue
            if(sRoguePopups.forceEnabledFromScript)
                ScriptContext_Enable();

            sRoguePopups.forceEnabled = FALSE;
            sRoguePopups.forceEnabledFromScript = FALSE;
        }
        else if(!RogueToD_ApplyWeatherVisuals() && sRoguePopups.lastWeatherPopup != GetSavedWeather())
        {
            // Apply weather popups if we have weather visuals disabled
            sRoguePopups.lastWeatherPopup = GetSavedWeather();

            if(sRoguePopups.lastWeatherPopup != WEATHER_NONE)
            {
                // We don't need to do this in the hub, as weather is purely asthetic in the hub
                if(Rogue_IsRunActive())
                    Rogue_PushPopup_WeatherActive(sRoguePopups.lastWeatherPopup);
            }
        }
        else
        {
            // Push next party notification, if
            Rogue_PushPopup_NextPartyNotification();
        }
        
        // If you press a button during a script, it will skip this notification
        if(sRoguePopups.forceEnabled && !GetCurrentPopup()->cantBeSkipped)
        {
            // If held wait a few frames before moving on
            if(ShouldSkipPopups())
            {
                sRoguePopups.hasPopupBeenSkipped = TRUE;

                //if (FuncIsActiveTask(Task_QuestPopUpWindow))
                //{
                //    sRoguePopups.hasPopupBeenSkipped = TRUE;
                //    //HideQuestPopUpWindow();
                //}
            }
        }
    }
    else
    {
        if (FuncIsActiveTask(Task_QuestPopUpWindow))
            HideQuestPopUpWindow();
    }

    sRoguePopups.wasEnabled = enabled;
    STOP_TIMER(ROGUE_POPUPS);
}

void Rogue_ForceEnablePopups(bool8 allowAudio, bool8 canSkip)
{
    sRoguePopups.forceEnabled = TRUE;
    sRoguePopups.forceEnabledMuteAudio = !allowAudio;
    sRoguePopups.forceEnabledFromScript = TRUE;
    sRoguePopups.forceEnabledCanSkip = canSkip;
}

bool8 Rogue_HasPendingPopups()
{
    return sRoguePopups.wasEnabled || sRoguePopups.forceEnabled;
}

void Rogue_DisplayPopupsFromScript()
{
    ScriptContext_Stop();
    sRoguePopups.forceEnabled = TRUE;
    sRoguePopups.forceEnabledMuteAudio = FALSE;
    sRoguePopups.forceEnabledFromScript = TRUE;
    sRoguePopups.forceEnabledCanSkip = FALSE;
}

void Rogue_DisplayPopupsFromScriptSkippable()
{
    ScriptContext_Stop();
    sRoguePopups.forceEnabled = TRUE;
    sRoguePopups.forceEnabledMuteAudio = FALSE;
    sRoguePopups.forceEnabledFromScript = TRUE;
    sRoguePopups.forceEnabledCanSkip = TRUE;
}

static void ApplyPopupAnimation(struct PopupRequest* request, u16 timer, bool8 useEnterAnim)
{
    struct PopupRequestTemplate const* template = &sPopupRequestTemplates[request->templateId];

    u16 value;
    u16 xStart, xEnd, yStart, yEnd;
    u16 invTimer;

    invTimer = template->animDuration - timer;
    xStart = 0;
    xEnd = 0;
    yStart = 0;
    yEnd = 0;

    switch (useEnterAnim ? template->enterAnim : template->exitAnim)
    {
    case POPUP_ANIM_SLIDE_VERTICAL:
        yStart = (template->height + 2) * 8;
        yEnd = 0;
        break;

    case POPUP_ANIM_SLIDE_HORIZONTAL:
        xStart = (template->width + 2) * 8;
        xEnd = 0;
        break;
    }

    if(xStart == xEnd)
        SetGpuReg(REG_OFFSET_BG0HOFS, xStart);
    else
    {
        value = (invTimer * xEnd) / template->animDuration + (timer * xStart) / template->animDuration;
        SetGpuReg(REG_OFFSET_BG0HOFS, value);
    }

    if(yStart == yEnd)
        SetGpuReg(REG_OFFSET_BG0VOFS, yStart);
    else
    {
        value = (invTimer * yEnd) / template->animDuration + (timer * yStart) / template->animDuration;
        SetGpuReg(REG_OFFSET_BG0VOFS, value);
    }
}

static u8 GetActiveOnScreenDisplayTimer()
{
    u8 taskId = FindTaskIdByFunc(Task_QuestPopUpWindow);
    if(taskId != TASK_NONE)
        return gTasks[taskId].tOnscreenTimer;

    return 0;
}

static void Task_QuestPopUpWindow(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    struct PopupRequest* popupRequest = GetCurrentPopup();
    struct PopupRequestTemplate const* template = &sPopupRequestTemplates[popupRequest->templateId];
    bool8 useEnterAnim = FALSE;
    u16 animDuration = sRoguePopups.hasPopupBeenSkipped ? 1 : template->animDuration;
    u16 displayDuration = sRoguePopups.hasPopupBeenSkipped ? SKIP_DISPLAY_DURATION : popupRequest->displayDuration;

    switch (task->sStateNum)
    {
    case 6:
        if (task->data[4] <= 5)
            task->data[4]++;
        else if (sRoguePopups.hasPopupBeenSkipped || (WaitFanfare(FALSE) && !IsSEPlaying()))
        {
            task->sStateNum = 0;
            task->data[4] = 0;
            ShowQuestPopUpWindow();
        }
        break;
    case 0:
        task->sDisplayTimer--;
        useEnterAnim = TRUE;
        if (task->sDisplayTimer <= 0 )
        {
            task->sDisplayTimer = 0;
            task->sStateNum = 1;
            gTasks[sRoguePopups.taskId].tOnscreenTimer = 0;
        }
        break;
    case 1:
        task->tOnscreenTimer++;
        if (task->tOnscreenTimer > displayDuration)
        {
            task->tOnscreenTimer = 0;
            task->sStateNum = 2;
        }
        break;
    case 2:
        task->sDisplayTimer++;
        if (task->sDisplayTimer >= animDuration)
        {
            task->sDisplayTimer = animDuration;
            if (task->tIncomingPopUp)
            {
                task->sStateNum = 6;
                task->data[4] = 0;
                task->tIncomingPopUp = 0;
            }
            else
            {
                task->sStateNum = 4;
                return;
            }
        }
        break;
    case 4:
        ClearStdWindowAndFrame(GetQuestPopUpWindowId(), TRUE);

        if(GetIconWindowId() != WINDOW_NONE)
        {
            FillWindowPixelBuffer(GetIconWindowId(), PIXEL_FILL(1));
            ClearWindowTilemap(GetIconWindowId());
            CopyWindowToVram(GetIconWindowId(), COPYWIN_FULL);
        }

        task->sStateNum = 5;
        break;
    case 5:
        HideQuestPopUpWindow();
        return;
    }

    ApplyPopupAnimation(popupRequest, task->sDisplayTimer, useEnterAnim);
}

#undef sStateNum

static void HideQuestPopUpWindow(void)
{
    if (FuncIsActiveTask(Task_QuestPopUpWindow))
    {
        ClearStdWindowAndFrame(GetQuestPopUpWindowId(), TRUE);

        if(GetIconWindowId() != WINDOW_NONE)
        {
            FillWindowPixelBuffer(GetIconWindowId(), PIXEL_FILL(1));
            ClearWindowTilemap(GetIconWindowId());
            CopyWindowToVram(GetIconWindowId(), COPYWIN_FULL);
        }

        RemoveQuestPopUpWindow();
        SetGpuReg_ForcedBlank(REG_OFFSET_BG0VOFS, 0);
        SetGpuReg_ForcedBlank(REG_OFFSET_BG0HOFS, 0);
        DestroyTask(sRoguePopups.taskId);
        
        sRoguePopups.lastShownId = (sRoguePopups.lastShownId + 1) % POPUP_QUEUE_CAPACITY;
    }
}

//static u8* AppendTypeName(u8* strPointer, u8 type)
//{
//    const u8 gText_AdjNormal[] = _("NORMAL");
//    const u8 gText_AdjFighting[] = _("FIGHT"); // Shortened for convienience
//    const u8 gText_AdjFlying[] = _("FLYING");
//    const u8 gText_AdjPoison[] = _("POISON");
//    const u8 gText_AdjGround[] = _("GROUND");
//    const u8 gText_AdjRock[] = _("ROCK");
//    const u8 gText_AdjBug[] = _("BUG");
//    const u8 gText_AdjGhost[] = _("GHOST");
//    const u8 gText_AdjSteel[] = _("STEEL");
//    const u8 gText_AdjFire[] = _("FIRE");
//    const u8 gText_AdjWater[] = _("WATER");
//    const u8 gText_AdjGrass[] = _("GRASS");
//    const u8 gText_AdjElectric[] = _("ELEC"); // Shortened for convienience
//    const u8 gText_AdjPsychic[] = _("PSYCHIC");
//    const u8 gText_AdjIce[] = _("ICE");
//    const u8 gText_AdjDragon[] = _("DRAGON");
//    const u8 gText_AdjDark[] = _("DARK");
//#ifdef ROGUE_EXPANSION
//    const u8 gText_AdjFairy[] = _("FAIRY");
//#endif
//    const u8 gText_AdjNone[] = _("???");
//
//    switch(type)
//    {
//        case TYPE_NORMAL:
//            return StringAppend(strPointer, gText_AdjNormal);
//
//        case TYPE_FIGHTING:
//            return StringAppend(strPointer, gText_AdjFighting);
//
//        case TYPE_FLYING:
//            return StringAppend(strPointer, gText_AdjFlying);
//
//        case TYPE_POISON:
//            return StringAppend(strPointer, gText_AdjPoison);
//
//        case TYPE_GROUND:
//            return StringAppend(strPointer, gText_AdjGround);
//
//        case TYPE_ROCK:
//            return StringAppend(strPointer, gText_AdjRock);
//
//        case TYPE_BUG:
//            return StringAppend(strPointer, gText_AdjBug);
//
//        case TYPE_GHOST:
//            return StringAppend(strPointer, gText_AdjGhost);
//
//        case TYPE_STEEL:
//            return StringAppend(strPointer, gText_AdjSteel);
//
//        case TYPE_FIRE:
//            return StringAppend(strPointer, gText_AdjFire);
//
//        case TYPE_WATER:
//            return StringAppend(strPointer, gText_AdjWater);
//
//        case TYPE_GRASS:
//            return StringAppend(strPointer, gText_AdjGrass);
//
//        case TYPE_ELECTRIC:
//            return StringAppend(strPointer, gText_AdjElectric);
//
//        case TYPE_PSYCHIC:
//            return StringAppend(strPointer, gText_AdjPsychic);
//
//        case TYPE_ICE:
//            return StringAppend(strPointer, gText_AdjIce);
//
//        case TYPE_DRAGON:
//            return StringAppend(strPointer, gText_AdjDragon);
//
//        case TYPE_DARK:
//            return StringAppend(strPointer, gText_AdjDark);
//
//#ifdef ROGUE_EXPANSION
//        case TYPE_FAIRY:
//            return StringAppend(strPointer, gText_AdjFairy);
//#endif
//
//        default:
//            return StringAppend(strPointer, gText_AdjNone);
//    }
//}

static void PrintPopupText( struct PopupRequest* popupRequest, u8 font, u8 const* text, u8 textCapacity, u8 x, u8 y)
{
    struct PopupRequestTemplate const* template = &sPopupRequestTemplates[popupRequest->templateId];

    u8 colours[] = 
    {
        gFonts[font].bgColor, 
        gFonts[font].fgColor, 
        gFonts[font].shadowColor, 
    };

    if(template->transparentText)
    {
        colours[0] = TEXT_COLOR_TRANSPARENT;
        colours[1] = TEXT_COLOR_WHITE;
        colours[2] = TEXT_COLOR_DARK_GRAY;
    }

    if(textCapacity == 0)
    {
        StringExpandPlaceholders(gStringVar4, text);
    }
    else
    {
        u8 buffer[32];
        u8* ptr;

        AGB_ASSERT(textCapacity < 32);
        ptr = StringCopyN(buffer, text, textCapacity);
        *ptr = EOS;
        StringExpandPlaceholders(gStringVar4, buffer);
    }

    x += GetStringCenterAlignXOffset(FONT_NARROW, gStringVar4, template->width * 8);
    AddTextPrinterParameterized3(GetQuestPopUpWindowId(), font, x, y, colours, TEXT_SKIP_DRAW, gStringVar4);
}

static void ExpandPopupText(struct PopupRequest* popup)
{
    u8* const textDest[] =
    {
        gStringVar1,
        gStringVar2,
        gStringVar3,
    };

    u8 i;
    u32 data;

    for(i = 0; i < ARRAY_COUNT(popup->expandTextType); ++i)
    {
        data = popup->expandTextData[i];

        if(popup->expandTextType[i] != TEXT_EXPAND_NONE)
        {
            switch(popup->expandTextType[i])
            {
                case TEXT_EXPAND_SPECIES_NAME:
                    StringCopy(textDest[i], RoguePokedex_GetSpeciesName(data));
                    break;

                case TEXT_EXPAND_PARTY_NICKNAME:
                    StringCopy_Nickname(textDest[i], gPlayerParty[data].box.nickname);
                    break;

                case TEXT_EXPAND_UNSIGNED_NUMBER:
                    ConvertIntToDecimalStringN(textDest[i], data, STR_CONV_MODE_LEFT_ALIGN, data > 99999 ? 9 : 5);
                    break;

                case TEXT_EXPAND_ITEM_NAME:
                    CopyItemName(data, textDest[i]);
                    break;
            }
        }
    }
}

static void ShowQuestPopUpWindow(void)
{
    struct PopupRequest* popupRequest = GetCurrentPopup();
    struct PopupRequestTemplate const* template = &sPopupRequestTemplates[popupRequest->templateId];

    AddQuestPopUpWindow(popupRequest);

    PutWindowTilemap(GetQuestPopUpWindowId());

    if(GetIconWindowId() != WINDOW_NONE)
        PutWindowTilemap(GetIconWindowId());

    if(template->generateBorder != FALSE)
        DrawStdWindowFrame(GetQuestPopUpWindowId(), FALSE);

    ExpandPopupText(popupRequest);

    if(popupRequest->titleText != NULL)
        PrintPopupText(popupRequest, FONT_NARROW, popupRequest->titleText, popupRequest->titleTextCapacity, 0, 1);

    if(popupRequest->subtitleText != NULL)
        PrintPopupText(popupRequest, FONT_SMALL, popupRequest->subtitleText, 0, 0, 14);


    CopyWindowToVram(GetQuestPopUpWindowId(), COPYWIN_FULL);

    if(GetIconWindowId() != WINDOW_NONE)
    {
        switch (template->iconMode)
        {
        case POPUP_ICON_MODE_ITEM:
            BlitItemIconToWindow(popupRequest->iconId, GetIconWindowId(), 0, 0, NULL);
            CopyWindowToVram(GetIconWindowId(), COPYWIN_FULL);
            break;

        case POPUP_ICON_MODE_POKEMON:
            BlitPokemonIconToWindow(popupRequest->iconId, GetIconWindowId(), 0, 0, NULL);
            CopyWindowToVram(GetIconWindowId(), COPYWIN_FULL);
            break;

        case POPUP_ICON_MODE_CUSTOM:
            BlitCustomItemIconToWindow(GetIconWindowId(), 0, 0, NULL, sRoguePopupCustomIcons[popupRequest->iconId].icon, sRoguePopupCustomIcons[popupRequest->iconId].palette);
            CopyWindowToVram(GetIconWindowId(), COPYWIN_FULL);
            break;

        default:
            AGB_ASSERT(FALSE);
            break;
        }
    }

    if(!gSaveBlock2Ptr->optionsPopupSoundOff)
    {
        bool8 playAudio = !popupRequest->scriptAudioOnly || sRoguePopups.forceEnabled;

        //if(JOY_HELD(SKIP_POPUP_BUTTONS))
        //    playAudio = FALSE;

        if(sRoguePopups.forceEnabled && sRoguePopups.forceEnabledMuteAudio)
            playAudio = FALSE;

        if(playAudio)
        {
            if(popupRequest->soundEffect)
                PlaySE(popupRequest->soundEffect);
            else if(popupRequest->fanfare)
                PlayFanfare(popupRequest->fanfare);
        }
    }
}

static struct PopupRequest* CreateNewPopup()
{
    u8 popupId = sRoguePopups.queuedId;
    sRoguePopups.queuedId = (sRoguePopups.queuedId + 1) % POPUP_QUEUE_CAPACITY;

    // We've queued sooo many popups that we need to push the lastShownId up by one so we see most of the popups
    if(sRoguePopups.lastShownId == sRoguePopups.queuedId)
        sRoguePopups.lastShownId = (sRoguePopups.lastShownId + 1) % POPUP_QUEUE_CAPACITY;

    memset(&sRoguePopups.requestQueue[popupId], 0, sizeof(sRoguePopups.requestQueue[popupId]));
    sRoguePopups.requestQueue[popupId].displayDuration = DEFAULT_DISPLAY_DURATION;
    return &sRoguePopups.requestQueue[popupId];
}

static bool8 HasTeachableMoves(struct Pokemon* mon, u8 fromLevel, u8 toLevel)
{
    u8 i;
    u16 species;

    if(fromLevel == toLevel)
        return FALSE;

    species = GetMonData(mon, MON_DATA_SPECIES);

    for (i = 0; gRoguePokemonProfiles[species].levelUpMoves[i].move != MOVE_NONE; i++)
    {
        if(gRoguePokemonProfiles[species].levelUpMoves[i].level > fromLevel && gRoguePokemonProfiles[species].levelUpMoves[i].level <= toLevel)
        {
            if(!MonKnowsMove(mon, gRoguePokemonProfiles[species].levelUpMoves[i].move))
                return TRUE;
        }
    }

    return FALSE;
}

void Rogue_PushPopup_NextPartyNotification()
{
    // Push one notification at a time
    // to avoid all the notifications clogging up the queue

    // Evo notifications
    if(sRoguePopups.partyNotificationCounter < PARTY_SIZE)
    {
        u8 i = sRoguePopups.partyNotificationCounter++;

        if(i < gPlayerPartyCount)
        {
            // Check for evolutions
            if(!gPlayerParty[i].rogueExtraData.hasPendingEvo)
            {
#ifdef ROGUE_EXPANSION
                u16 targetSpecies = GetEvolutionTargetSpecies(&gPlayerParty[i], EVO_MODE_NORMAL, ITEM_NONE, NULL);
#else
                u16 targetSpecies = GetEvolutionTargetSpecies(&gPlayerParty[i], EVO_MODE_NORMAL, ITEM_NONE);
#endif
                if(targetSpecies != SPECIES_NONE)
                {
                    Rogue_PushPopup_NewEvos(i);
                    gPlayerParty[i].rogueExtraData.hasPendingEvo = TRUE;
                }
            }
        }
    }
    // New move notifications
    else if(sRoguePopups.partyNotificationCounter < PARTY_SIZE * 2)
    {
        u8 i = sRoguePopups.partyNotificationCounter++ - PARTY_SIZE;

        if(i < gPlayerPartyCount)
        {
            u8 fromLvl, toLvl;

            fromLvl = gPlayerParty[i].rogueExtraData.lastPopupLevel;
            toLvl = GetMonData(&gPlayerParty[i], MON_DATA_LEVEL);

            // Check for new moves to learn
            if(HasTeachableMoves(&gPlayerParty[i], fromLvl, toLvl))
                Rogue_PushPopup_NewMoves(i);

            gPlayerParty[i].rogueExtraData.lastPopupLevel = toLvl;
        }
    }
}

void Rogue_PushPopup_NewMoves(u8 slotId)
{
    struct PopupRequest* popup = CreateNewPopup();
    u16 species = GetMonData(&gPlayerParty[slotId], MON_DATA_SPECIES);

    popup->templateId = POPUP_COMMON_PARTY_INFO;
    popup->iconId = species;
    popup->soundEffect = 0;
    popup->displayDuration = 60 - DEFAULT_ANIM_DURATION;
    
    popup->titleText = gPlayerParty[slotId].box.nickname;
    popup->subtitleText = sText_Popup_NewMoves;
    popup->titleTextCapacity = POKEMON_NAME_LENGTH;
}

void Rogue_PushPopup_NewEvos(u8 slotId)
{
    struct PopupRequest* popup = CreateNewPopup();
    u16 species = GetMonData(&gPlayerParty[slotId], MON_DATA_SPECIES);

    popup->templateId = POPUP_COMMON_PARTY_INFO;
    popup->iconId = species;
    popup->soundEffect = 0;
    popup->displayDuration = 60 - DEFAULT_ANIM_DURATION;
    
    popup->titleText = gPlayerParty[slotId].box.nickname;
    popup->subtitleText = sText_Popup_NewEvolution;
    popup->titleTextCapacity = POKEMON_NAME_LENGTH;
}

void Rogue_PushPopup_UnableToEvolve(u8 slotId)
{
    struct PopupRequest* popup = CreateNewPopup();
    u16 species = GetMonData(&gPlayerParty[slotId], MON_DATA_SPECIES);

    popup->templateId = POPUP_COMMON_POKEMON_TEXT;
    popup->iconId = species;
    popup->soundEffect = SE_NOT_EFFECTIVE;
    
    popup->titleText = gPlayerParty[slotId].box.nickname;
    popup->subtitleText = sText_Popup_StarterWarning;
    popup->titleTextCapacity = POKEMON_NAME_LENGTH;
}

void Rogue_PushPopup_MonStatChange(u8 slotId, bool8 improvement)
{
    struct PopupRequest* popup = CreateNewPopup();
    u16 species = GetMonData(&gPlayerParty[slotId], MON_DATA_SPECIES);

    popup->templateId = POPUP_COMMON_POKEMON_TEXT;
    popup->iconId = species;
    
    popup->titleText = gPlayerParty[slotId].box.nickname;
    popup->subtitleText = improvement ? sText_Popup_GotStronger : sText_Popup_GotWeaker;
    popup->titleTextCapacity = POKEMON_NAME_LENGTH;
}

void Rogue_PushPopup_MonShinyChange(u8 slotId, bool8 improvement)
{
    struct PopupRequest* popup = CreateNewPopup();
    u16 species = GetMonData(&gPlayerParty[slotId], MON_DATA_SPECIES);

    popup->templateId = POPUP_COMMON_POKEMON_TEXT;
    popup->iconId = species;
    
    popup->titleText = gPlayerParty[slotId].box.nickname;
    popup->subtitleText = improvement ? sText_Popup_GotShiny : sText_Popup_LostShiny;
    popup->titleTextCapacity = POKEMON_NAME_LENGTH;
}

void Rogue_PushPopup_MonGenderChange(u8 slotId, u8 gender)
{
    struct PopupRequest* popup = CreateNewPopup();
    u16 species = GetMonData(&gPlayerParty[slotId], MON_DATA_SPECIES);

    popup->templateId = POPUP_COMMON_POKEMON_TEXT;
    popup->iconId = species;
    
    popup->titleText = gPlayerParty[slotId].box.nickname;
    popup->subtitleText = gender == MON_MALE ? sText_Popup_BecameMale : sText_Popup_BecameFemale;
    popup->titleTextCapacity = POKEMON_NAME_LENGTH;
}

void Rogue_PushPopup_QuestComplete(u16 questId)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_ITEM_TEXT;
    popup->iconId = ITEM_QUEST_LOG;
    popup->soundEffect = SE_EXP_MAX;
    
    popup->titleText = RogueQuest_GetTitle(questId);
    popup->subtitleText = sText_Popup_QuestComplete;
}

void Rogue_PushPopup_QuestFail(u16 questId)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_ITEM_TEXT;
    popup->iconId = ITEM_QUEST_LOG;
    popup->soundEffect = SE_NOT_EFFECTIVE;
    
    popup->titleText = RogueQuest_GetTitle(questId);
    popup->subtitleText = sText_Popup_QuestFail;
}

void Rogue_PushPopup_QuestUnlocked(u16 questId)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_ITEM_TEXT;
    popup->iconId = ITEM_QUEST_LOG;
    popup->soundEffect = SE_EXP_MAX;
    
    popup->titleText = RogueQuest_GetTitle(questId);
    popup->subtitleText = sText_Popup_QuestComplete;
}

void Rogue_PushPopup_PokemonChain(u16 species, u16 chainSize)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_POKEMON_TEXT;
    popup->iconId = species;
    popup->soundEffect = 0;
    
    popup->titleText = RoguePokedex_GetSpeciesName(species);
    popup->subtitleText = sText_Popup_PokemonChain;

    popup->expandTextData[0] = chainSize;
    popup->expandTextType[0] = TEXT_EXPAND_UNSIGNED_NUMBER;
}

void Rogue_PushPopup_PokemonChainBroke(u16 species)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_POKEMON_TEXT;
    popup->iconId = species;
    popup->soundEffect = SE_NOT_EFFECTIVE;
    
    popup->titleText = RoguePokedex_GetSpeciesName(species);
    popup->subtitleText = sText_Popup_PokemonChainBroke;
}


void Rogue_PushPopup_WeakPokemonClause(u16 species, bool8 fromDaycare)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_POKEMON_TEXT;
    popup->iconId = species;
    popup->soundEffect = SE_BALL_OPEN;
    
    popup->titleText = sText_Popup_WeakLegendaryClause;
    popup->subtitleText = sText_Popup_LegendaryClause;
}

void Rogue_PushPopup_StrongPokemonClause(u16 species, bool8 fromDaycare)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_POKEMON_TEXT;
    popup->iconId = species;
    popup->soundEffect = SE_BALL_OPEN;
    
    popup->titleText = sText_Popup_StrongLegendaryClause;
    popup->subtitleText = sText_Popup_LegendaryClause;
}

void Rogue_PushPopup_RoamerPokemonActivated(u16 species)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_POKEMON_TEXT;
    popup->iconId = species;
    popup->soundEffect = SE_BALL_OPEN;
    
    popup->titleText = RoguePokedex_GetSpeciesName(species);
    popup->subtitleText = sText_Popup_RoamerLegendary;
}

void Rogue_PushPopup_AddItem(u16 itemId, u16 amount)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_FIND_ITEM;
    popup->iconId = itemId;

    popup->fanfare = MUS_OBTAIN_ITEM;
    popup->scriptAudioOnly = TRUE;

    if(amount == 1)
    {
        popup->titleText = sText_Popup_SingleItem;
        popup->subtitleText = NULL;
    }
    else
    {
        popup->titleText = sText_Popup_MultipleItem;
        popup->subtitleText = NULL;
    }

    popup->expandTextData[0] = itemId;
    popup->expandTextType[0] = TEXT_EXPAND_ITEM_NAME;

    popup->expandTextData[1] = amount;
    popup->expandTextType[1] = TEXT_EXPAND_UNSIGNED_NUMBER;
}


void Rogue_PushPopup_LostItem(u16 itemId, u16 amount)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_FIND_ITEM;
    popup->iconId = itemId;

    popup->soundEffect = SE_NOT_EFFECTIVE;
    popup->scriptAudioOnly = TRUE;

    if(amount == 1)
    {
        popup->titleText = sText_Popup_SingleItem;
        popup->subtitleText = sText_Popup_LostItem;
    }
    else
    {
        popup->titleText = sText_Popup_MultipleItem;
        popup->subtitleText = sText_Popup_LostItem;
    }

    popup->expandTextData[0] = itemId;
    popup->expandTextType[0] = TEXT_EXPAND_ITEM_NAME;

    popup->expandTextData[1] = amount;
    popup->expandTextType[1] = TEXT_EXPAND_UNSIGNED_NUMBER;
}

void Rogue_PushPopup_AddBerry(u16 itemId, u16 amount)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_FIND_ITEM;
    popup->iconId = itemId;

    popup->fanfare = MUS_OBTAIN_BERRY;
    popup->scriptAudioOnly = TRUE;

    if(amount == 1)
    {
        popup->titleText = sText_Popup_SingleItem;
        popup->subtitleText = NULL;
    }
    else
    {
        popup->titleText = sText_Popup_MultipleItem;
        popup->subtitleText = NULL;
    }

    popup->expandTextData[0] = itemId;
    popup->expandTextType[0] = TEXT_EXPAND_ITEM_NAME;

    popup->expandTextData[1] = amount;
    popup->expandTextType[1] = TEXT_EXPAND_UNSIGNED_NUMBER;
}

void Rogue_PushPopup_AddMoney(u32 amount)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_CUSTOM_ICON_SLIDE_TEXT;
    popup->iconId = POPUP_CUSTOM_ICON_MONEY;

    popup->fanfare = MUS_OBTAIN_ITEM;
    popup->scriptAudioOnly = TRUE;

    popup->titleText = sText_Popup_Money;
    popup->subtitleText = NULL;

    popup->expandTextData[0] = amount;
    popup->expandTextType[0] = TEXT_EXPAND_UNSIGNED_NUMBER;
}

void Rogue_PushPopup_LostMoney(u32 amount)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_CUSTOM_ICON_SLIDE_TEXT;
    popup->iconId = POPUP_CUSTOM_ICON_MONEY;

    popup->soundEffect = SE_NOT_EFFECTIVE;
    popup->scriptAudioOnly = TRUE;

    popup->titleText = sText_Popup_Money;
    popup->subtitleText = sText_Popup_LostMoney;

    popup->expandTextData[0] = amount;
    popup->expandTextType[0] = TEXT_EXPAND_UNSIGNED_NUMBER;
}

void Rogue_PushPopup_CannotTakeItem(u16 itemId, u16 amount)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_FIND_ITEM;
    popup->iconId = itemId;
    popup->soundEffect = SE_NOT_EFFECTIVE;

    if(amount == 1)
    {
        popup->titleText = sText_Popup_BagFull;
        popup->subtitleText = sText_Popup_SingleItem;
    }
    else
    {
        popup->titleText = sText_Popup_BagFull;
        popup->subtitleText = sText_Popup_MultipleItem;
    }

    popup->expandTextData[0] = itemId;
    popup->expandTextType[0] = TEXT_EXPAND_ITEM_NAME;

    popup->expandTextData[1] = amount;
    popup->expandTextType[1] = TEXT_EXPAND_UNSIGNED_NUMBER;
}

void Rogue_PushPopup_UnlockedShopItem(u16 itemId)
{
    // Hacked special case
    // We unlock multiple of these items at once, but we only want to display a single popup for "unlocked all of xyz"
#ifdef ROGUE_EXPANSION
    if(itemId >= ITEM_FLAME_PLATE && itemId <= ITEM_PIXIE_PLATE)
    {
        if(itemId == ITEM_IRON_PLATE)
        {
            struct PopupRequest* popup = CreateNewPopup();

            popup->templateId = POPUP_COMMON_FIND_ITEM;
            popup->iconId = itemId;
            popup->fanfare = MUS_OBTAIN_ITEM;

            popup->titleText = sText_Popup_TypePlateItem;
            popup->subtitleText = sText_Popup_UnlockedInShops;

            popup->expandTextData[0] = itemId;
            popup->expandTextType[0] = TEXT_EXPAND_ITEM_NAME;
        }
    }
    else if(itemId >= ITEM_FIRE_MEMORY && itemId <= ITEM_FAIRY_MEMORY)
    {
        if(itemId == ITEM_STEEL_MEMORY)
        {
            struct PopupRequest* popup = CreateNewPopup();

            popup->templateId = POPUP_COMMON_FIND_ITEM;
            popup->iconId = itemId;
            popup->fanfare = MUS_OBTAIN_ITEM;

            popup->titleText = sText_Popup_TypeMemoryItem;
            popup->subtitleText = sText_Popup_UnlockedInShops;

            popup->expandTextData[0] = itemId;
            popup->expandTextType[0] = TEXT_EXPAND_ITEM_NAME;
        }
    }
    else
#endif
    {
        struct PopupRequest* popup = CreateNewPopup();

        popup->templateId = POPUP_COMMON_FIND_ITEM;
        popup->iconId = itemId;
        popup->fanfare = MUS_OBTAIN_ITEM;

        popup->titleText = sText_Popup_SingleItem;
        popup->subtitleText = sText_Popup_UnlockedInShops;

        popup->expandTextData[0] = itemId;
        popup->expandTextType[0] = TEXT_EXPAND_ITEM_NAME;
    }
}

void Rogue_PushPopup_UnlockedDecor(u16 decorId)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_FIND_ITEM;
    popup->iconId = ITEM_BASEMENT_KEY;
    popup->fanfare = MUS_OBTAIN_ITEM;

    popup->titleText = RogueHub_GetDecorName(decorId);
    popup->subtitleText = sText_Popup_UnlockedDecor;
}

void Rogue_PushPopup_UnlockedDecorVariant(u16 decorVariantId)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_FIND_ITEM;
    popup->iconId = ITEM_BASEMENT_KEY;
    popup->fanfare = MUS_OBTAIN_ITEM;

    popup->titleText = RogueHub_GetDecorVariantName(decorVariantId);
    popup->subtitleText = sText_Popup_UnlockedDecor;
}

void Rogue_PushPopup_AddPokemon(u16 species, bool8 isCustom, bool8 isShiny)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_INSTANT_POKEMON_TEXT;
    popup->iconId = species;
    popup->fanfare = MUS_OBTAIN_TMHM;
    
    popup->titleText = RoguePokedex_GetSpeciesName(species);

    if(isCustom)
        popup->subtitleText = sText_Popup_GiftCustomPokemon;
    else if(isShiny)
        popup->subtitleText = sText_Popup_GiftShinyPokemon;
    else if(species == SPECIES_EGG)
    {
        popup->titleText = sText_Popup_DaycarePokemon;
        // no subtitle
    }
    else
        popup->subtitleText = sText_Popup_GiftPokemon;
}

void Rogue_PushPopup_UniquePokemonDetected(u16 species)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_POKEMON_TEXT;
    popup->iconId = species;
    
    popup->titleText = sText_Popup_UniquePokemon;
    popup->subtitleText = sText_Popup_UniquePokemonSubtitle;
}

void Rogue_PushPopup_RequipBerrySuccess(u16 itemId)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_FIND_ITEM;
    popup->iconId = itemId;

    popup->titleText = sText_Popup_BerriesRequipSuccess;
    popup->subtitleText = sText_Popup_BerriesRequipSuccessSubtitle;
}

void Rogue_PushPopup_RequipBerryFail(u16 itemId)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_FIND_ITEM;
    popup->iconId = itemId;
    popup->soundEffect = SE_NOT_EFFECTIVE;

    popup->titleText = sText_Popup_BerriesRequipFail;
    popup->subtitleText = sText_Popup_BerriesRequipFailSubtitle;
}

void Rogue_PushPopup_TriggerExtraLife(bool8 itemConsumed)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_ITEM_TEXT;
    popup->iconId = ITEM_SACRED_ASH;
    popup->fanfare = MUS_HEAL;
    
    popup->titleText = sText_Popup_ExtraLifeTitle;

    if(itemConsumed)
        popup->subtitleText = sText_Popup_ExtraLifeSubtitle;
}

void Rogue_PushPopup_FlaskRefilled()
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_ITEM_TEXT;
    popup->iconId = ITEM_HEALING_FLASK;
    
    popup->titleText = sText_Popup_HealingFlaskRefilled;
}

void Rogue_PushPopup_FlightChargeUsed(u32 remainingCharges, u32 totalCharges)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_FIND_ITEM;
    popup->iconId = ITEM_BASIC_RIDING_WHISTLE;
    popup->displayDuration = 30;
    
    popup->titleText = sText_Popup_FlightChargeRemaining;
    popup->subtitleText = sText_Popup_FlightChargeSubtitle;

    popup->expandTextData[0] = remainingCharges;
    popup->expandTextType[0] = TEXT_EXPAND_UNSIGNED_NUMBER;
    popup->expandTextData[1] = totalCharges;
    popup->expandTextType[1] = TEXT_EXPAND_UNSIGNED_NUMBER;
}

void Rogue_PushPopup_FlightChargeRefilled(u32 totalCharges)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_FIND_ITEM;
    popup->iconId = ITEM_BASIC_RIDING_WHISTLE;
    
    popup->titleText = sText_Popup_FlightChargeRemaining;
    popup->subtitleText = sText_Popup_FlightChargeSubtitle;

    popup->expandTextData[0] = totalCharges;
    popup->expandTextType[0] = TEXT_EXPAND_UNSIGNED_NUMBER;
    popup->expandTextData[1] = totalCharges;
    popup->expandTextType[1] = TEXT_EXPAND_UNSIGNED_NUMBER;
}

void Rogue_PushPopup_UnlockPokedex()
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_CUSTOM_ICON_TEXT;
    popup->iconId = POPUP_CUSTOM_ICON_POKEDEX;
    popup->fanfare = MUS_RG_OBTAIN_KEY_ITEM;

    popup->titleText = sText_Popup_PokedexUnlock;
}

void Rogue_PushPopup_UpgradePokedex()
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_CUSTOM_ICON_TEXT;
    popup->iconId = POPUP_CUSTOM_ICON_POKEDEX;
    popup->fanfare = MUS_RG_OBTAIN_KEY_ITEM;

    popup->titleText = sText_Popup_PokedexUpgrade;
}

void Rogue_PushPopup_UpgradeBagCapacity()
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_ITEM_TEXT;
    popup->iconId = ITEM_BERRY_POUCH;
    popup->fanfare = MUS_LEVEL_UP;

    popup->titleText = sText_Popup_BagUpdate;
    popup->subtitleText = sText_Popup_UpgradeSlots;

    popup->expandTextData[0] = ITEM_BAG_SLOTS_PER_UPGRADE;
    popup->expandTextType[0] = TEXT_EXPAND_UNSIGNED_NUMBER;

    popup->expandTextData[1] = GetBagUnreservedTotalSlots();
    popup->expandTextType[1] = TEXT_EXPAND_UNSIGNED_NUMBER;
}

void Rogue_PushPopup_AssistantConnected()
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_CUSTOM_ICON_TEXT;
    popup->iconId = POPUP_CUSTOM_ICON_POKEDEX;

    popup->titleText = sText_Popup_RogueAssistant;
    popup->subtitleText = sText_Popup_Connected;
}

void Rogue_PushPopup_AssistantDisconnected()
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_CUSTOM_ICON_TEXT;
    popup->iconId = POPUP_CUSTOM_ICON_POKEDEX;


    popup->titleText = sText_Popup_RogueAssistant;
    popup->subtitleText = sText_Popup_Disconnected;
}

void Rogue_PushPopup_EasterEggOutfitUnlocked()
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_ITEM_TEXT;
    popup->iconId = ITEM_GREEN_SCARF;
    popup->fanfare = MUS_RG_OBTAIN_KEY_ITEM;

    popup->titleText = sText_Popup_OutfitUnlocked;
    popup->subtitleText = sText_Popup_ItsASecret;
}

void Rogue_PushPopup_NewBadgeGet(u8 difficulty)
{
    struct PopupRequest* popup = CreateNewPopup();
    u8 type = gRogueRun.completedBadges[difficulty];

    AGB_ASSERT(type != TYPE_NONE);
    if(type == TYPE_NONE)
        type = TYPE_MYSTERY;

    popup->templateId = POPUP_COMMON_CUSTOM_ICON_TEXT;
    popup->iconId = POPUP_CUSTOM_ICON_TYPE_NORMAL + type;
    popup->displayDuration = 330;
    popup->cantBeSkipped = TRUE;

    if(difficulty >= ROGUE_CHAMP_START_DIFFICULTY)
    {
        popup->fanfare = MUS_OBTAIN_SYMBOL;
        popup->titleText = sText_Popup_ChampBadge;
        popup->subtitleText = sText_Popup_EarnBadge;
    }
    else if(difficulty >= ROGUE_ELITE_START_DIFFICULTY)
    {
        popup->fanfare = MUS_OBTAIN_BADGE;
        popup->titleText = sText_Popup_EliteBadge;
        popup->subtitleText = sText_Popup_EarnBadge;

        popup->expandTextData[0] = difficulty + 1 - ROGUE_ELITE_START_DIFFICULTY;
        popup->expandTextType[0] = TEXT_EXPAND_UNSIGNED_NUMBER;
    }
    else
    {
        popup->fanfare = MUS_OBTAIN_BADGE;
        popup->titleText = sText_Popup_GymBadge;
        popup->subtitleText = sText_Popup_EarnBadge;

        popup->expandTextData[0] = difficulty + 1;
        popup->expandTextType[0] = TEXT_EXPAND_UNSIGNED_NUMBER;
    }
}

void Rogue_PushPopup_VictoryLapProgress(u8 type, u16 victories)
{
    struct PopupRequest* popup = CreateNewPopup();

    if(type == TYPE_NONE)
        type = TYPE_MYSTERY;

    popup->templateId = POPUP_COMMON_CUSTOM_ICON_TEXT;
    popup->iconId = POPUP_CUSTOM_ICON_TYPE_NORMAL + type;
    popup->fanfare = MUS_OBTAIN_ITEM;
    popup->titleText = sText_Popup_VictoryLapGymBadge;
    popup->subtitleText = sText_Popup_EarnBadge;

    popup->expandTextData[0] = victories;
    popup->expandTextType[0] = TEXT_EXPAND_UNSIGNED_NUMBER;
}

void Rogue_PushPopup_WeatherActive(u16 weather)
{
    if(
        weather != WEATHER_SHADE && 
        weather != WEATHER_VOLCANIC_ASH
    )
    {
        struct PopupRequest* popup = CreateNewPopup();

        popup->templateId = POPUP_COMMON_CUSTOM_ICON_SLIDE_TEXT;
        popup->iconId = POPUP_CUSTOM_ICON_CLOUD;

        popup->titleText = sWeatherNames[weather];
        popup->subtitleText = sText_Popup_WeatherActive;
    }
}

void Rogue_PushPopup_AdventureReplay()
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_ITEM_TEXT;
    popup->iconId = ITEM_OLD_SEA_MAP;
    //popup->soundEffect = SE_NOT_EFFECTIVE;
    
    popup->titleText = sText_Popup_AdventureReplay;
    popup->subtitleText = sText_Popup_AdventureReplaySubtitle;
}

void Rogue_PushPopup_MainQuestsDisabled()
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_ITEM_TEXT;
    popup->iconId = ITEM_C_GEAR;
    popup->soundEffect = SE_UNLOCK;
    
    popup->titleText = sText_Popup_QuestsDisabled;
    popup->subtitleText = sText_Popup_QuestsDisabledSubtitle;
}

void Rogue_PushPopup_ChallengeQuestsDisabled()
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_ITEM_TEXT;
    popup->iconId = ITEM_C_GEAR;
    popup->soundEffect = SE_UNLOCK;
    
    popup->titleText = sText_Popup_ChallengesDisabled;
    popup->subtitleText = sText_Popup_QuestsDisabledSubtitle;
}

void Rogue_PushPopup_CustomPopup(struct CustomPopup const* template)
{
    struct PopupRequest* popup = CreateNewPopup();

    if(template->itemIcon != ITEM_NONE)
    {
        popup->templateId = POPUP_COMMON_ITEM_TEXT;
        popup->iconId = template->itemIcon;
    }
    else if(template->speciesIcon != ITEM_NONE)
    {
        popup->templateId = POPUP_COMMON_POKEMON_TEXT;
        popup->iconId = template->speciesIcon;
    }

    if(template->soundEffect != MUS_DUMMY)
        popup->soundEffect = template->soundEffect;
    else if(template->fanfare != MUS_DUMMY)
        popup->fanfare = template->fanfare;
    
    popup->titleText = template->titleStr;
    popup->subtitleText = template->subtitleStr;
}