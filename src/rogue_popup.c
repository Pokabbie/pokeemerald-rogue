#include "global.h"
#include "battle_pyramid.h"
#include "bg.h"
#include "event_data.h"
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
#include "rogue_followmon.h"
#include "rogue_popup.h"

#define POPUP_QUEUE_CAPACITY 8

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
    POPUP_CUSTOM_ICON_COUNT,
};

struct PopupRequestTemplate
{
    u8 enterAnim;
    u8 exitAnim;
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
    u16 expandTextData[3];
    u16 expandTextType[3];
    u8 titleTextCapacity;
    u8 templateId;
    u16 iconId;
    u16 soundEffect;
    u16 fanfare;
    bool8 scriptAudioOnly : 1;
};

struct PopupManager
{
    struct PopupRequest requestQueue[POPUP_QUEUE_CAPACITY];
    u8 windowId;
    u8 iconWindowId;
    u8 taskId;
    u8 lastShownId;
    u8 queuedId;
    u8 partyNotificationCounter;
    bool8 wasEnabled : 1;
    bool8 scriptEnabled : 1;
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
    }
};

static EWRAM_DATA struct PopupManager sRoguePopups = { 0 };

extern const u8 gText_Space[];

extern const u8 gText_Popup_QuestComplete[];
extern const u8 gText_Popup_QuestFail[];
extern const u8 gText_Popup_LegendaryClause[];
extern const u8 gText_Popup_None[];

extern const u8 gText_Popup_NewMoves[];
extern const u8 gText_Popup_NewEvolution[];

extern const u8 gText_Popup_PokemonChain[];
extern const u8 gText_Popup_PokemonChainBroke[];

extern const u8 gPopupText_WeakLegendaryClause[];
extern const u8 gPopupText_StrongLegendaryClause[];

extern const u8 gText_Popup_BagFull[];
extern const u8 gText_Popup_SingleItem[];
extern const u8 gText_Popup_MultipleItem[];

extern const u8 gPopupText_CampaignNoneScore[];
extern const u8 gPopupText_CampaignHighScore[];
extern const u8 gPopupText_CampaignLowScore[];

extern const u8 gPopupText_SafariArea[];
extern const u8 gPopupText_StarterWarning[];

extern const u8 gPopupText_EncounterChain[];
extern const u8 gPopupText_EncounterChainEnd[];

extern const u8 gText_Popup_PokedexUnlock[];
extern const u8 gText_Popup_PokedexUpgrade[];

extern const u8 gText_Popup_RogueAssistant[];
extern const u8 gText_Popup_Connected[];
extern const u8 gText_Popup_Disconnected[];

enum
{
    POPUP_COMMON_CLASSIC,
    POPUP_COMMON_ITEM_TEXT,
    POPUP_COMMON_FIND_ITEM,
    POPUP_COMMON_POKEMON_TEXT,
    POPUP_COMMON_CUSTOM_ICON_TEXT,
};

static const struct PopupRequestTemplate sPopupRequestTemplates[] =
{
    [POPUP_COMMON_CLASSIC] = 
    {
        .enterAnim = POPUP_ANIM_SLIDE_VERTICAL,
        .exitAnim = POPUP_ANIM_SLIDE_VERTICAL,
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

#define SLIDE_ANIM_DURATION 20
#define sStateNum data[0]
#define sDisplayTimer data[2]

static void ShowQuestPopup(void);
static void HideQuestPopUpWindow(void);

static void Task_QuestPopUpWindow(u8 taskId);
static void ShowQuestPopUpWindow(void);
static void LoadQuestPopUpWindowBg(void);

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
        sRoguePopups.taskId = CreateTask(Task_QuestPopUpWindow, 90);
        ApplyPopupAnimation(GetCurrentPopup(), 0, FALSE);

        gTasks[sRoguePopups.taskId].sStateNum = 6;
        gTasks[sRoguePopups.taskId].sDisplayTimer = SLIDE_ANIM_DURATION;
    }
    else
    {
        if (gTasks[sRoguePopups.taskId].sStateNum != 2)
            gTasks[sRoguePopups.taskId].sStateNum = 2;
        gTasks[sRoguePopups.taskId].data[3] = 1;
    }
}

void Rogue_ClearPopupQueue(void)
{
    if (FuncIsActiveTask(Task_QuestPopUpWindow))
        HideQuestPopUpWindow();

    sRoguePopups.queuedId = 0;
    sRoguePopups.lastShownId = 0;
}

void Rogue_UpdatePopups(bool8 inOverworld, bool8 inputEnabled)
{
    bool8 enabled = inOverworld && inputEnabled; // May need to check this too? GetStartMenuWindowId

    if(sRoguePopups.scriptEnabled)
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
        else if(sRoguePopups.scriptEnabled)
        {
            // Disable script enabled mode now, as we've reached end of queue
            sRoguePopups.scriptEnabled = FALSE;
            EnableBothScriptContexts();
        }
        else
        {
            // Push next party notification, if
            Rogue_PushPopup_NextPartyNotification();
        }
        
        // If you press a button during a script, it will skip this notification
        if(sRoguePopups.scriptEnabled)
        {
            if(JOY_NEW(A_BUTTON | B_BUTTON | START_BUTTON))
            {
                if (FuncIsActiveTask(Task_QuestPopUpWindow))
                    HideQuestPopUpWindow();
            }
        }
    }
    else
    {
        if (FuncIsActiveTask(Task_QuestPopUpWindow))
            HideQuestPopUpWindow();
    }

    sRoguePopups.wasEnabled = enabled;
}

void Rogue_DisplayPopupsFromScript()
{
    ScriptContext1_Stop();
    sRoguePopups.scriptEnabled = TRUE;
}

static void ApplyPopupAnimation(struct PopupRequest* request, u16 timer, bool8 useEnterAnim)
{
    struct PopupRequestTemplate const* template = &sPopupRequestTemplates[request->templateId];

    u16 value;
    u16 xStart, xEnd, yStart, yEnd;
    u16 invTimer;

    invTimer = SLIDE_ANIM_DURATION - timer;
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
        value = (invTimer * xEnd) / SLIDE_ANIM_DURATION + (timer * xStart) / SLIDE_ANIM_DURATION;
        SetGpuReg(REG_OFFSET_BG0HOFS, value);
    }

    if(yStart == yEnd)
        SetGpuReg(REG_OFFSET_BG0VOFS, yStart);
    else
    {
        value = (invTimer * yEnd) / SLIDE_ANIM_DURATION + (timer * yStart) / SLIDE_ANIM_DURATION;
        SetGpuReg(REG_OFFSET_BG0VOFS, value);
    }
}

static void Task_QuestPopUpWindow(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    struct PopupRequest* popupRequest = GetCurrentPopup();
    bool8 useEnterAnim = FALSE;

    switch (task->sStateNum)
    {
    case 6:
        task->data[4]++;
        if (task->data[4] > 5)
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
            gTasks[sRoguePopups.taskId].data[1] = 0;
        }
        break;
    case 1:
        task->data[1]++;
        if (task->data[1] > 120 )
        {
            task->data[1] = 0;
            task->sStateNum = 2;
        }
        break;
    case 2:
        task->sDisplayTimer++;
        if (task->sDisplayTimer >= SLIDE_ANIM_DURATION)
        {
            task->sDisplayTimer = SLIDE_ANIM_DURATION;
            if (task->data[3])
            {
                task->sStateNum = 6;
                task->data[4] = 0;
                task->data[3] = 0;
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

static u8* AppendTypeName(u8* strPointer, u8 type)
{
    const u8 gText_AdjNormal[] = _("NORMAL");
    const u8 gText_AdjFighting[] = _("FIGHT"); // Shortened for convienience
    const u8 gText_AdjFlying[] = _("FLYING");
    const u8 gText_AdjPoison[] = _("POISON");
    const u8 gText_AdjGround[] = _("GROUND");
    const u8 gText_AdjRock[] = _("ROCK");
    const u8 gText_AdjBug[] = _("BUG");
    const u8 gText_AdjGhost[] = _("GHOST");
    const u8 gText_AdjSteel[] = _("STEEL");
    const u8 gText_AdjFire[] = _("FIRE");
    const u8 gText_AdjWater[] = _("WATER");
    const u8 gText_AdjGrass[] = _("GRASS");
    const u8 gText_AdjElectric[] = _("ELEC"); // Shortened for convienience
    const u8 gText_AdjPsychic[] = _("PSYCHIC");
    const u8 gText_AdjIce[] = _("ICE");
    const u8 gText_AdjDragon[] = _("DRAGON");
    const u8 gText_AdjDark[] = _("DARK");
#ifdef ROGUE_EXPANSION
    const u8 gText_AdjFairy[] = _("FAIRY");
#endif
    const u8 gText_AdjNone[] = _("???");

    switch(type)
    {
        case TYPE_NORMAL:
            return StringAppend(strPointer, gText_AdjNormal);

        case TYPE_FIGHTING:
            return StringAppend(strPointer, gText_AdjFighting);

        case TYPE_FLYING:
            return StringAppend(strPointer, gText_AdjFlying);

        case TYPE_POISON:
            return StringAppend(strPointer, gText_AdjPoison);

        case TYPE_GROUND:
            return StringAppend(strPointer, gText_AdjGround);

        case TYPE_ROCK:
            return StringAppend(strPointer, gText_AdjRock);

        case TYPE_BUG:
            return StringAppend(strPointer, gText_AdjBug);

        case TYPE_GHOST:
            return StringAppend(strPointer, gText_AdjGhost);

        case TYPE_STEEL:
            return StringAppend(strPointer, gText_AdjSteel);

        case TYPE_FIRE:
            return StringAppend(strPointer, gText_AdjFire);

        case TYPE_WATER:
            return StringAppend(strPointer, gText_AdjWater);

        case TYPE_GRASS:
            return StringAppend(strPointer, gText_AdjGrass);

        case TYPE_ELECTRIC:
            return StringAppend(strPointer, gText_AdjElectric);

        case TYPE_PSYCHIC:
            return StringAppend(strPointer, gText_AdjPsychic);

        case TYPE_ICE:
            return StringAppend(strPointer, gText_AdjIce);

        case TYPE_DRAGON:
            return StringAppend(strPointer, gText_AdjDragon);

        case TYPE_DARK:
            return StringAppend(strPointer, gText_AdjDark);

#ifdef ROGUE_EXPANSION
        case TYPE_FAIRY:
            return StringAppend(strPointer, gText_AdjFairy);
#endif

        default:
            return StringAppend(strPointer, gText_AdjNone);
    }
}

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
    u16 data;

    for(i = 0; i < ARRAY_COUNT(popup->expandTextType); ++i)
    {
        data = popup->expandTextData[i];

        if(popup->expandTextType[i] != TEXT_EXPAND_NONE)
        {
            switch(popup->expandTextType[i])
            {
                case TEXT_EXPAND_SPECIES_NAME:
                    StringCopy(textDest[i], gSpeciesNames[data]);
                    break;

                case TEXT_EXPAND_PARTY_NICKNAME:
                    StringCopy_Nickname(textDest[i], gPlayerParty[data].box.nickname);
                    break;

                case TEXT_EXPAND_UNSIGNED_NUMBER:
                    ConvertIntToDecimalStringN(textDest[i], data, STR_CONV_MODE_LEFT_ALIGN, 3);
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
        if(!popupRequest->scriptAudioOnly || sRoguePopups.scriptEnabled)
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

    memset(&sRoguePopups.requestQueue[popupId], 0, sizeof(sRoguePopups.requestQueue[popupId]));
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
                u16 targetSpecies = GetEvolutionTargetSpecies(&gPlayerParty[i], EVO_MODE_NORMAL, ITEM_NONE, NULL);
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

    popup->templateId = POPUP_COMMON_POKEMON_TEXT;
    popup->iconId = species;
    popup->soundEffect = 0;
    
    popup->titleText = gPlayerParty[slotId].box.nickname;
    popup->subtitleText = gText_Popup_NewMoves;
    popup->titleTextCapacity = POKEMON_NAME_LENGTH;
}

void Rogue_PushPopup_NewEvos(u8 slotId)
{
    struct PopupRequest* popup = CreateNewPopup();
    u16 species = GetMonData(&gPlayerParty[slotId], MON_DATA_SPECIES);

    popup->templateId = POPUP_COMMON_POKEMON_TEXT;
    popup->iconId = species;
    popup->soundEffect = 0;
    
    popup->titleText = gPlayerParty[slotId].box.nickname;
    popup->subtitleText = gText_Popup_NewEvolution;
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
    popup->subtitleText = gPopupText_StarterWarning;
    popup->titleTextCapacity = POKEMON_NAME_LENGTH;
}


void Rogue_PushPopup_QuestComplete(u16 questId)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_ITEM_TEXT;
    popup->iconId = ITEM_QUEST_LOG;
    popup->soundEffect = SE_EXP_MAX;
    
    popup->titleText = gRogueQuests[questId].title;
    popup->subtitleText = gText_Popup_QuestComplete;
}

void Rogue_PushPopup_QuestFail(u16 questId)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_ITEM_TEXT;
    popup->iconId = ITEM_QUEST_LOG;
    popup->soundEffect = SE_NOT_EFFECTIVE;
    
    popup->titleText = gRogueQuests[questId].title;
    popup->subtitleText = gText_Popup_QuestFail;
}

void Rogue_PushPopup_PokemonChain(u16 species, u16 chainSize)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_POKEMON_TEXT;
    popup->iconId = species;
    popup->soundEffect = 0;
    
    popup->titleText = gSpeciesNames[species];
    popup->subtitleText = gText_Popup_PokemonChain;

    popup->expandTextData[0] = chainSize;
    popup->expandTextType[0] = TEXT_EXPAND_UNSIGNED_NUMBER;
}

void Rogue_PushPopup_PokemonChainBroke(u16 species)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_POKEMON_TEXT;
    popup->iconId = species;
    popup->soundEffect = SE_NOT_EFFECTIVE;
    
    popup->titleText = gSpeciesNames[species];
    popup->subtitleText = gText_Popup_PokemonChainBroke;
}


void Rogue_PushPopup_WeakPokemonClause(u16 species)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_POKEMON_TEXT;
    popup->iconId = species;
    popup->soundEffect = SE_BALL_OPEN;
    
    popup->titleText = gPopupText_WeakLegendaryClause;
    popup->subtitleText = gText_Popup_LegendaryClause;
}

void Rogue_PushPopup_StrongPokemonClause(u16 species)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_POKEMON_TEXT;
    popup->iconId = species;
    popup->soundEffect = SE_BALL_OPEN;
    
    popup->titleText = gPopupText_StrongLegendaryClause;
    popup->subtitleText = gText_Popup_LegendaryClause;
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
        popup->titleText = gText_Popup_SingleItem;
        popup->subtitleText = NULL;
    }
    else
    {
        popup->titleText = gText_Popup_MultipleItem;
        popup->subtitleText = NULL;
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
        popup->titleText = gText_Popup_SingleItem;
        popup->subtitleText = NULL;
    }
    else
    {
        popup->titleText = gText_Popup_MultipleItem;
        popup->subtitleText = NULL;
    }

    popup->expandTextData[0] = itemId;
    popup->expandTextType[0] = TEXT_EXPAND_ITEM_NAME;

    popup->expandTextData[1] = amount;
    popup->expandTextType[1] = TEXT_EXPAND_UNSIGNED_NUMBER;
}

void Rogue_PushPopup_CannotTakeItem(u16 itemId, u16 amount)
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_FIND_ITEM;
    popup->iconId = itemId;
    popup->soundEffect = SE_NOT_EFFECTIVE;

    if(amount == 1)
    {
        popup->titleText = gText_Popup_BagFull;
        popup->subtitleText = gText_Popup_SingleItem;
    }
    else
    {
        popup->titleText = gText_Popup_BagFull;
        popup->subtitleText = gText_Popup_MultipleItem;
    }

    popup->expandTextData[0] = itemId;
    popup->expandTextType[0] = TEXT_EXPAND_ITEM_NAME;

    popup->expandTextData[1] = amount;
    popup->expandTextType[1] = TEXT_EXPAND_UNSIGNED_NUMBER;
}

void Rogue_PushPopup_UnlockPokedex()
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_CUSTOM_ICON_TEXT;
    popup->iconId = POPUP_CUSTOM_ICON_POKEDEX;
    popup->fanfare = FANFARE_RG_OBTAIN_KEY_ITEM;

    popup->titleText = gText_Popup_PokedexUnlock;
}

void Rogue_PushPopup_UpgradePokedex()
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_CUSTOM_ICON_TEXT;
    popup->iconId = POPUP_CUSTOM_ICON_POKEDEX;
    popup->fanfare = FANFARE_RG_OBTAIN_KEY_ITEM;

    popup->titleText = gText_Popup_PokedexUpgrade;
}

void Rogue_PushPopup_AssistantConnected()
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_CUSTOM_ICON_TEXT;
    popup->iconId = POPUP_CUSTOM_ICON_POKEDEX;

    popup->titleText = gText_Popup_RogueAssistant;
    popup->subtitleText = gText_Popup_Connected;
}

void Rogue_PushPopup_AssistantDisconnected()
{
    struct PopupRequest* popup = CreateNewPopup();

    popup->templateId = POPUP_COMMON_CUSTOM_ICON_TEXT;
    popup->iconId = POPUP_CUSTOM_ICON_POKEDEX;


    popup->titleText = gText_Popup_RogueAssistant;
    popup->subtitleText = gText_Popup_Disconnected;
}