#include "global.h"
#include "battle_main.h"
#include "palette.h"
#include "main.h"
#include "data.h"
#include "daycare.h"
#include "event_data.h"
#include "international_string_util.h"
#include "gpu_regs.h"
#include "scanline_effect.h"
#include "task.h"
#include "malloc.h"
#include "decompress.h"
#include "bg.h"
#include "window.h"
#include "strings.h"
#include "string_util.h"
#include "text.h"
#include "item.h"
#include "overworld.h"
#include "menu.h"
#include "sound.h"
#include "trainer_pokemon_sprites.h"
#include "party_menu.h"
#include "pokedex.h"
#include "pokemon_icon.h"

#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/rgb.h"
#include "constants/songs.h"
#ifdef ROGUE_EXPANSION
#include "constants/form_change_types.h"
#endif

#include "rogue_baked.h"
#include "rogue_controller.h"
#include "rogue_gifts.h"
#include "rogue_pokedex.h"
#include "rogue_ridemon.h"
#include "rogue_settings.h"
#include "rogue_query.h"
#include "rogue_quest.h"
#include "rogue_safari.h"

#ifdef ROGUE_EXPANSION
#define DEX_GEN_LIMIT 9
#else
#define DEX_GEN_LIMIT 3
#endif

#define FLIP_VERTICAL (0x08 << 8)
#define FLIP_HORIZONTAL (0x04 << 8)

// Overview
#define COLUMN_ENTRY_COUNT 7
#define ROW_ENTRY_COUNT 4
#define OVERVIEW_ENTRY_COUNT (COLUMN_ENTRY_COUNT * ROW_ENTRY_COUNT)

enum
{
    PAGE_NONE,

    PAGE_TITLE_SCREEN,
    PAGE_SEARCH,
    PAGE_OVERVIEW,
    PAGE_MON_STATS,
    PAGE_MON_MOVES,
    PAGE_MON_EVOS,
    PAGE_MON_FORMS,

    PAGE_MON_RIDE_STATS,

    PAGE_MON_FIRST = PAGE_MON_STATS,
    PAGE_MON_LAST = PAGE_MON_RIDE_STATS,
};

enum
{
    MON_SPRITE_FRONT_PIC,
    MON_SPRITE_BACK_PIC,
    MON_SPRITE_ICON,
    MON_SPRITE_TYPE1,
    MON_SPRITE_TYPE2,
    MON_SPRITE_EVO_ICON1,
    MON_SPRITE_EVO_ICON2,
    MON_SPRITE_EVO_ICON3,
    MON_SPRITE_EVO_ICON4,
    MON_SPRITE_COUNT,
};

#define MAX_SPRITE_COUNT max(OVERVIEW_ENTRY_COUNT, MON_SPRITE_COUNT)


enum
{
    WIN_MON_SPECIES_NAME_NO,
    WIN_MON_PAGE_TITLE,
    WIN_MON_PAGE_CONTENT,
    WIN_TITLE_COUNTERS,
    WIN_TITLE_VARIANT_SELECT,
    WIN_COUNT,
};

static const struct WindowTemplate sMonEntryWinTemplates[WIN_COUNT + 1] =
{
    [WIN_MON_SPECIES_NAME_NO] = 
    {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 14,
        .width = 7,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 1,
    },
    [WIN_MON_PAGE_TITLE] = 
    {
        .bg = 0,
        .tilemapLeft = 15,
        .tilemapTop = 1,
        .width = 11,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 29,
    },
    [WIN_MON_PAGE_CONTENT] = 
    {
        .bg = 0,
        .tilemapLeft = 12,
        .tilemapTop = 3,
        .width = 17,
        .height = 16,
        .paletteNum = 15,
        .baseBlock = 51,
    },

    [WIN_TITLE_COUNTERS] = 
    {
        .bg = 0,
        .tilemapLeft = 3,
        .tilemapTop = 7,
        .width = 4,
        .height = 8,
        .paletteNum = 15,
        .baseBlock = 323,
    },
    [WIN_TITLE_VARIANT_SELECT] = 
    {
        .bg = 0,
        .tilemapLeft = 8,
        .tilemapTop = 7,
        .width = 15,
        .height = 5,
        .paletteNum = 15,
        .baseBlock = 355,
    },

    [WIN_COUNT] = DUMMY_WIN_TEMPLATE,
};

#ifdef ROGUE_EXPANSION
static const u8 sTitle_Stats[] = _("Stats");
static const u8 sTitle_Moves[] = _("Moves");
static const u8 sTitle_Evolutions[] = _("Evolutions");
static const u8 sTitle_Forms[] = _("Forms");
static const u8 sTitle_Riding[] = _("Poké Ride");

static const u8 sText_Types[] = _("Types");
static const u8 sText_Abilities[] = _("Abilities");

static const u8 sText_Total[] = _("Total");

static const u8 sText_HP[] = _("HP");
static const u8 sText_Attack[] = _("Atk");
static const u8 sText_Defence[] = _("Def");
static const u8 sText_SpAttack[] = _("Sp Atk");
static const u8 sText_SpDefence[] = _("Sp Def");
static const u8 sText_Speed[] = _("Speed");

static const u8 sText_Skills[] = _("Skills");
static const u8 sText_SkillClimbing[] = _("Climbing");
static const u8 sText_SkillSurf[] = _("Surfing");
static const u8 sText_SkillFlying[] = _("Flying");
static const u8 sText_SkillNone[] = _("None");

static const u8 sText_Base[] = _("{COLOR RED}{SHADOW LIGHT_RED}Base");
static const u8 sText_Alolan[] = _("{COLOR BLUE}{SHADOW LIGHT_BLUE}Alolan");
static const u8 sText_Galarian[] = _("{COLOR BLUE}{SHADOW LIGHT_BLUE}Galarian");
static const u8 sText_Paldean[] = _("{COLOR BLUE}{SHADOW LIGHT_BLUE}Paldean");
static const u8 sText_Hisuian[] = _("{COLOR BLUE}{SHADOW LIGHT_BLUE}Hisuian");
static const u8 sText_Mega[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Mega Evolution");
static const u8 sText_Primal[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Primal Reversion");
static const u8 sText_UltraBurst[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Ultra Burst");
static const u8 sText_Gigantamax[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Gigantamax");
static const u8 sText_TeraForm[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Tera Form");
static const u8 sText_Debug[] = _("{COLOR RED}{SHADOW LIGHT_RED}DEBUG VIEW ONLY");

static const u8 sText_NoFormData[] = _("{COLOR RED}{SHADOW LIGHT_RED}No Form data found");
#else
static const u8 sTitle_Stats[] = _("STATS");
static const u8 sTitle_Moves[] = _("MOVES");
static const u8 sTitle_Evolutions[] = _("EVOLUTIONS");
static const u8 sTitle_Forms[] = _("FORMS");
static const u8 sTitle_Riding[] = _("POKé RIDE");

static const u8 sText_Types[] = _("TYPES");
static const u8 sText_Abilities[] = _("ABILITIES");

static const u8 sText_Total[] = _("TOTAL");
static const u8 sText_HP[] = _("HP");
static const u8 sText_Attack[] = _("ATK");
static const u8 sText_Defence[] = _("DEF");
static const u8 sText_SpAttack[] = _("SP ATK");
static const u8 sText_SpDefence[] = _("SP DEF");
static const u8 sText_Speed[] = _("SPEED");

static const u8 sText_Skills[] = _("SKILLS");
static const u8 sText_SkillClimbing[] = _("CLIMBING");
static const u8 sText_SkillSurf[] = _("SURFING");
static const u8 sText_SkillFlying[] = _("FLYING");
static const u8 sText_SkillNone[] = _("NONE");
#endif

static const u8 sText_RideStar[] = _("{STAR_ICON}");
static const u8 sText_NoDataFound[] = _("{COLOR RED}{SHADOW LIGHT_RED}No data found");

extern const u8 gText_DexNational[];
extern const u8 gText_DexHoenn[];
extern const u8 gText_PokedexDiploma[];

static void CB2_Rogue_ShowPokedex(void);
static void MainCB2(void);
static void Task_SetupPage(u8);
static void Task_SwapToPage(u8);
static void Task_PageFadeIn(u8);
static void Task_PageWaitForKeyPress(u8);
static void Task_PageFadeOutAndExit(u8);
static void DisplayTitleScreenCountersText(void);
static void DisplayTitleDexVariantText(void);
static void DisplayMonEntryText(void);
static void DisplayMonStatsText(void);
static void DisplayMonMovesText(void);
static void DisplayMonEvosText(void);
static void DisplayMonFormsText(void);
static void DisplayMonRideStatsText(void);
static void InitOverviewBg(void);
static void InitMonEntryWindows(void);
static void DestroyMonEntryWindows(void);
static void InitPageResources(u8 fromPage, u8 toPage);
static void DestroyPageResources(u8 fromPage, u8 toPage);

static void GatherSpeciesStatsArray(u16 species, u8* stats);

static u16 GetVariantSpeciesAt(u8 variant, u16 index);
static u16 GetVariantSpeciesCount(u8 variant);
static u8 GetVariantGenLimit(u8 variant);
static bool8 CheckVariantContainsSpecies(u8 variant, u16 species);

// Title screen
static void TitleScreen_HandleInput(u8);
static void TitleScreen_RefillBg();
static void TitleScreen_RefillStarsBg();

// Overview
static void Overview_HandleInput(u8);
static void Overview_RefillBg();
static void Overview_CreateSprites();
static void Overview_DestroySprites();
static void Overview_SelectSpeciesToDiplay();
static void Overview_FillEntryBg(u8 entryX, u8 entryY, bool8 includeHeader);
static void Overview_FillEntryBg_Selected(u8 entryX, u8 entryY, bool8 includeHeader);
static u8 Overview_GetLastValidActiveIndex();
static u8 Overview_GetMaxScrollAmount();

// MonInfo
static void MonInfo_CreateSprites(bool8 includeType);
static void MonInfo_DestroySprites();

// Mon stats
static void MonStats_HandleInput(u8);

// Mon moves
static void MonMoves_HandleInput(u8);

// Mon evos
static void MonEvos_OpenMoveQuery();
static void MonEvos_CloseMoveQuery();
static bool8 MonEvos_IsTutorMoveTM(u16 moveIdx);
static bool8 MonEvos_IsTutorMoveTR(u16 moveIdx);
static bool8 MonEvos_IsTutorMove(u16 moveIdx);
static void MonEvos_HandleInput(u8);
static void MonEvos_CreateSprites();

// Mon forms
static void MonForms_HandleInput(u8);
static void MonForms_CreateSprites();

// Ride stats
static void MonRideStats_HandleInput(u8);

struct PokedexMenu
{
    u8 currentPage;
    u8 desiredPage;
    u8 pageSprites[MAX_SPRITE_COUNT];
    u8 displayArrowTask;
    u16 displayArrowOffset;

    // Title screen
    bool8 titleScreenInEditMode;
    bool8 titleScreenCursorIdx;

    // Overview
    u16 selectedIdx;
    u16 pageScrollAmount;
    u16 overviewPageSpecies[OVERVIEW_ENTRY_COUNT];
    u16 overviewPageNumbers[OVERVIEW_ENTRY_COUNT];

    // Mon screen
    u32 viewOtId;
    u16 lastCrySpecies;
    u16 viewBaseSpecies;
    u16 listScrollAmount;
    u8 partySlot;
};

enum
{
    DEX_VIEW_STANDARD,              // regular pokedex nothing speciton
    DEX_VIEW_SPECIFIC_MON,          // view entry for a specific party mon (support custom mons)
    DEX_VIEW_SELECT_MON,            // select a mon and return it in gSpecialVarResult
    DEX_VIEW_SELECT_SAFARI_MON,     // select a mon currently in the safari mon list
};

struct PokedexViewRequest
{
    u8 view;
    u16 dexVariantToRestore;
    union
    {
        struct
        {
            u32 OtId;
            u16 species;
            u8 partySlot;
        } specificMon;
        struct
        {
            bool8 requireSeen;
            bool8 requireCaught;
        } selectMon;
    } perView;
};

EWRAM_DATA static u8 *sTilemapBufferPtr = NULL;
EWRAM_DATA static struct PokedexMenu* sPokedexMenu = NULL;
EWRAM_DATA static struct PokedexViewRequest sPokedexViewReq = {0};

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static const u16 sDiplomaPalettes[][16] =
{
    INCBIN_U16("graphics/rogue_pokedex/info_screen.gbapal"),
    INCBIN_U16("graphics/rogue_pokedex/info_screen.gbapal"),
};

// HomePage
// Overview
// MonStats
// MonMoves
// MonEvos
// MonForms
//?

static const u32 sTitleScreenTilemap[] = INCBIN_U32("graphics/rogue_pokedex/front_page.bin.lz");
static const u32 sTitleScreenTiles[] = INCBIN_U32("graphics/rogue_pokedex/front_page.4bpp.lz");

static const u32 sOverviewTilemap[] = INCBIN_U32("graphics/rogue_pokedex/info_screen.bin.lz");
static const u32 sOverviewTiles[] = INCBIN_U32("graphics/rogue_pokedex/info_screen.4bpp.lz");

static const u32 sPageSplitTableTilemap[] = INCBIN_U32("graphics/rogue_pokedex/page_split_table.bin.lz");
static const u32 sPageListsTilemap[] = INCBIN_U32("graphics/rogue_pokedex/page_list.bin.lz");
static const u32 sPageFormsTilemap[] = INCBIN_U32("graphics/rogue_pokedex/page_forms.bin.lz");

// above share the same tilemap
static const u32 sPageTiles[] = INCBIN_U32("graphics/rogue_pokedex/page_tiles.4bpp.lz");

static void SetupPokedexViewDefault()
{
    gMain.savedCallback = CB2_ReturnToFieldContinueScript;
    sPokedexViewReq.view = DEX_VIEW_STANDARD;
    sPokedexViewReq.dexVariantToRestore = POKEDEX_INVALID_VARIANT;
    SetMainCallback2(CB2_Rogue_ShowPokedex);
}

void Rogue_ShowPokedexFromMenu(void)
{
    SetupPokedexViewDefault();
    gMain.savedCallback = CB2_ReturnToFieldWithOpenMenu;
}

void Rogue_ShowPokedexFromScript(void)
{
    SetupPokedexViewDefault();
}

void Rogue_ShowPokedexForPartySlot(u8 slot)
{
    SetupPokedexViewDefault();

    // ReturnToPartyMenuSubMenu called below
    sPokedexViewReq.view = DEX_VIEW_SPECIFIC_MON;
    sPokedexViewReq.perView.specificMon.species = GetMonData(&gPlayerParty[slot], MON_DATA_SPECIES);
    sPokedexViewReq.perView.specificMon.OtId = GetMonData(&gPlayerParty[slot], MON_DATA_OT_ID);
    sPokedexViewReq.perView.specificMon.partySlot = slot;
}

void Rogue_SelectPokemonInPokedexFromDex(bool8 requireSeen, bool8 requireCaught)
{
    Rogue_SelectPokemonInPokedexFromDexVariant(RoguePokedex_GetDexVariant(), requireSeen, requireCaught);
}

void Rogue_SelectPokemonInPokedexFromDexVariant(u8 variant, bool8 requireSeen, bool8 requireCaught)
{
    SetupPokedexViewDefault();

    sPokedexViewReq.view = DEX_VIEW_SELECT_MON;
    sPokedexViewReq.perView.selectMon.requireSeen = requireSeen;
    sPokedexViewReq.perView.selectMon.requireCaught = requireCaught;

    sPokedexViewReq.dexVariantToRestore = RoguePokedex_GetDexVariant();
    RoguePokedex_SetDexVariant(variant);
}

void Rogue_SelectPokemonInSafari()
{
    if(gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(ROGUE_INTERIOR_SAFARI_CAVE) && gSaveBlock1Ptr->location.mapNum == MAP_NUM(ROGUE_INTERIOR_SAFARI_CAVE))
        Rogue_SelectPokemonInPokedexFromDexVariant(POKEDEX_DYNAMIC_VARIANT_LEGEND_SAFARI, FALSE, FALSE);
    else
        Rogue_SelectPokemonInPokedexFromDexVariant(POKEDEX_DYNAMIC_VARIANT_NORMAL_SAFARI, FALSE, FALSE);

    sPokedexViewReq.view = DEX_VIEW_SELECT_SAFARI_MON;
}

static bool8 IsCurrentlySelectingMon()
{
    return sPokedexViewReq.view == DEX_VIEW_SELECT_MON || sPokedexViewReq.view == DEX_VIEW_SELECT_SAFARI_MON;
}

static void CB2_Rogue_ShowPokedex(void)
{
    u8 i;
    sPokedexMenu = AllocZeroed(sizeof(struct PokedexMenu));
    sPokedexMenu->currentPage = PAGE_NONE;
    sPokedexMenu->desiredPage = PAGE_TITLE_SCREEN;

    sPokedexMenu->lastCrySpecies = SPECIES_NONE;
    sPokedexMenu->viewBaseSpecies = SPECIES_NONE;
    sPokedexMenu->viewOtId = 0;
    sPokedexMenu->partySlot = PARTY_SIZE;

    sPokedexMenu->displayArrowTask = TASK_NONE;

    if(sPokedexViewReq.view == DEX_VIEW_SPECIFIC_MON)
    {
        sPokedexMenu->desiredPage = PAGE_MON_STATS;
        sPokedexMenu->viewBaseSpecies = sPokedexViewReq.perView.specificMon.species;
        sPokedexMenu->viewOtId = sPokedexViewReq.perView.specificMon.OtId;
        sPokedexMenu->partySlot = sPokedexViewReq.perView.specificMon.partySlot;
    }
    else if(IsCurrentlySelectingMon())
    {
        sPokedexMenu->desiredPage = PAGE_OVERVIEW;
    }

    for(i = 0; i < ARRAY_COUNT(sPokedexMenu->pageSprites); ++i)
        sPokedexMenu->pageSprites[i] = SPRITE_NONE;

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
    LoadPalette(sDiplomaPalettes, 0, 64);
    sTilemapBufferPtr = Alloc(BG_SCREEN_SIZE);

    InitOverviewBg();
    ResetTempTileDataBuffers();
    
    //DecompressAndCopyTileDataToVram(1, &sOverviewTiles, 0, 0, 0);
    //while (FreeTempTileDataBuffersIfPossible())
    //    ;
    //LZDecompressWram(sOverviewTilemap, sTilemapBufferPtr);
    //CopyBgTilemapBufferToVram(1);

    sPokedexMenu->currentPage = sPokedexMenu->desiredPage;
    InitPageResources(PAGE_NONE, sPokedexMenu->currentPage);

    // Fade into page
    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);

    EnableInterrupts(1);
    SetVBlankCallback(VBlankCB);
    SetMainCallback2(MainCB2);
    CreateTask(Task_PageFadeIn, 0);

    PlaySE(SE_PC_LOGIN);
}

static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void InitPageResources(u8 fromPage, u8 toPage)
{
    sPokedexMenu->listScrollAmount = 0;

    InitOverviewBg();
    ResetTempTileDataBuffers();

    // If we're swapping onto a mon page for the first tile load tiles
    if(toPage >= PAGE_MON_FIRST && toPage <= PAGE_MON_LAST)
    {
        if(fromPage >= PAGE_MON_FIRST && fromPage <= PAGE_MON_LAST)
        {
            // No need + causes VRAM issues
        }
        else
        {
            DecompressAndCopyTileDataToVram(1, &sPageTiles, 0, 0, 0);
            while (FreeTempTileDataBuffersIfPossible())
                ;
        }
    }

    switch (toPage)
    {
    case PAGE_TITLE_SCREEN:
        {
            DecompressAndCopyTileDataToVram(1, &sTitleScreenTiles, 0, 0, 0);
            while (FreeTempTileDataBuffersIfPossible())
                ;
            LZDecompressWram(sTitleScreenTilemap, sTilemapBufferPtr);
            CopyBgTilemapBufferToVram(1);

            InitMonEntryWindows();
            // Text printed below

            TitleScreen_RefillBg();
        }
        break;
        
    case PAGE_OVERVIEW:
        {
            u16 desiredIdx = 0;

            DecompressAndCopyTileDataToVram(1, &sOverviewTiles, 0, 0, 0);
            while (FreeTempTileDataBuffersIfPossible())
                ;
            LZDecompressWram(sOverviewTilemap, sTilemapBufferPtr);
            CopyBgTilemapBufferToVram(1);

            LoadMonIconPalettes();
            //BlendPalettes(PALETTES_ALL, 16, RGB_BLACK); // Ensure the mon icon palettes are faded

            desiredIdx = RoguePokedex_GetSpeciesCurrentNum(sPokedexMenu->viewBaseSpecies);

            // Try and put the location on the mon we were just viewing
            if(desiredIdx != 0) // invalid num
            {
                --desiredIdx;
                sPokedexMenu->pageScrollAmount = 0;

                if(desiredIdx > OVERVIEW_ENTRY_COUNT)
                {
                    sPokedexMenu->pageScrollAmount = min(1 + (desiredIdx - OVERVIEW_ENTRY_COUNT) / COLUMN_ENTRY_COUNT, Overview_GetMaxScrollAmount());
                }

                sPokedexMenu->selectedIdx = desiredIdx - (sPokedexMenu->pageScrollAmount * COLUMN_ENTRY_COUNT);
            }

            Overview_SelectSpeciesToDiplay();
            Overview_RefillBg();
            Overview_CreateSprites();
        }
        break;

    case PAGE_MON_STATS:
        {
            LZDecompressWram(sPageSplitTableTilemap, sTilemapBufferPtr);
            CopyBgTilemapBufferToVram(1);

            InitMonEntryWindows();
            // Text printed below

            LoadMonIconPalettes();

            MonInfo_CreateSprites(TRUE);
        }
        break;

    case PAGE_MON_MOVES:
        {
            LZDecompressWram(sPageListsTilemap, sTilemapBufferPtr);
            CopyBgTilemapBufferToVram(1);

            MonEvos_OpenMoveQuery();

            InitMonEntryWindows();
            // Text printed below

            LoadMonIconPalettes();

            MonInfo_CreateSprites(FALSE);
        }
        break;

    case PAGE_MON_EVOS:
        {
            LZDecompressWram(sPageFormsTilemap, sTilemapBufferPtr);
            CopyBgTilemapBufferToVram(1);

            InitMonEntryWindows();
            // Text printed below

            LoadMonIconPalettes();

            MonInfo_CreateSprites(FALSE);
            MonEvos_CreateSprites();
        }
        break;

    case PAGE_MON_FORMS:
        {
            LZDecompressWram(sPageFormsTilemap, sTilemapBufferPtr);
            CopyBgTilemapBufferToVram(1);

            InitMonEntryWindows();
            // Text printed below

            LoadMonIconPalettes();

            MonInfo_CreateSprites(FALSE);
            MonForms_CreateSprites();
        }
        break;

    case PAGE_MON_RIDE_STATS:
        {
            LZDecompressWram(sPageListsTilemap, sTilemapBufferPtr);
            CopyBgTilemapBufferToVram(1);

            InitMonEntryWindows();
            // Text printed below

            LoadMonIconPalettes();

            MonInfo_CreateSprites(FALSE);
        }
        break;

    default:
        break;
    }
}

static void DestroyPageResources(u8 fromPage, u8 toPage)
{    
    // TODO - Could stop sprites from flashing if we didn't destroy them here

    switch (fromPage)
    {
    case PAGE_TITLE_SCREEN:
        {
            DestroyMonEntryWindows();
        }
        break;

    case PAGE_OVERVIEW:
        {
            Overview_DestroySprites();
            FreeMonIconPalettes();
        }
        break;

    case PAGE_MON_STATS:
    case PAGE_MON_EVOS:
    case PAGE_MON_FORMS:
    case PAGE_MON_RIDE_STATS:
        {
            MonInfo_DestroySprites();
            FreeMonIconPalettes();

            DestroyMonEntryWindows();
        }
        break;

    case PAGE_MON_MOVES:
        {
            MonEvos_CloseMoveQuery();

            MonInfo_DestroySprites();
            FreeMonIconPalettes();

            DestroyMonEntryWindows();
        }
        break;
    
    default:
        break;
    }

    ResetSpriteData();
    FreeAllSpritePalettes();
    CopyBgTilemapBufferToVram(0);
}

#define tDoFade data[0]

static void Task_SetupPage(u8 taskId)
{
    DestroyPageResources(sPokedexMenu->currentPage, sPokedexMenu->desiredPage);
    InitPageResources(sPokedexMenu->currentPage, sPokedexMenu->desiredPage);
    
    sPokedexMenu->currentPage = sPokedexMenu->desiredPage;

    if(gTasks[taskId].tDoFade)
    {
        // Fade into page
        BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
    }

    gTasks[taskId].func = Task_PageFadeIn;
}

static void Task_SwapToPage2(u8);

static void Task_SwapToPage(u8 taskId)
{
    // If we're moving between stats page for the same mon, don't bother doing a fade
    if(sPokedexMenu->currentPage >= PAGE_MON_FIRST && sPokedexMenu->currentPage <= PAGE_MON_LAST && 
        sPokedexMenu->desiredPage >= PAGE_MON_FIRST && sPokedexMenu->desiredPage <= PAGE_MON_LAST)
    {
        gTasks[taskId].tDoFade = FALSE;//(sPokedexMenu->lastCrySpecies != sPokedexMenu->viewBaseSpecies);
    }
    else
    {
        gTasks[taskId].tDoFade = TRUE;
    }

    if(gTasks[taskId].tDoFade)
    {
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    }

    gTasks[taskId].func = Task_SwapToPage2;
}

static void Task_SwapToPage2(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_SetupPage;
}


static void Task_PageFadeIn2(u8 taskId);

static void Task_PageFadeIn(u8 taskId)
{
    // Print text now the fade has started
    switch (sPokedexMenu->currentPage)
    {
    case PAGE_TITLE_SCREEN:
        DisplayTitleScreenCountersText();
        DisplayTitleDexVariantText();
        break;

    case PAGE_MON_STATS:
        DisplayMonEntryText();
        DisplayMonStatsText();
        break;

    case PAGE_MON_MOVES:
        DisplayMonEntryText();
        DisplayMonMovesText();
        break;

    case PAGE_MON_EVOS:
        DisplayMonEntryText();
        DisplayMonEvosText();
        break;

    case PAGE_MON_FORMS:
        DisplayMonEntryText();
        DisplayMonFormsText();
        break;
    
    case PAGE_MON_RIDE_STATS:
        DisplayMonEntryText();
        DisplayMonRideStatsText();
        break;

    default:
        break;
    }

    gTasks[taskId].func = Task_PageFadeIn2;
}

static void Task_PageFadeIn2(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if(sPokedexMenu->currentPage >= PAGE_MON_FIRST && sPokedexMenu->currentPage <= PAGE_MON_LAST)
        {
            if(sPokedexMenu->lastCrySpecies != sPokedexMenu->viewBaseSpecies)
            {
                PlayCry_Normal(sPokedexMenu->viewBaseSpecies, 0);
                sPokedexMenu->lastCrySpecies = sPokedexMenu->viewBaseSpecies;
            }
        }

        gTasks[taskId].func = Task_PageWaitForKeyPress;
    }
}

#undef tDoFade

static void Task_PageWaitForKeyPress(u8 taskId)
{
    switch (sPokedexMenu->currentPage)
    {
    case PAGE_TITLE_SCREEN:
        TitleScreen_HandleInput(taskId);
        break;

    case PAGE_OVERVIEW:
        Overview_HandleInput(taskId);
        break;

    case PAGE_MON_STATS:
        MonStats_HandleInput(taskId);
        break;

    case PAGE_MON_MOVES:
        MonMoves_HandleInput(taskId);
        break;

    case PAGE_MON_EVOS:
        MonEvos_HandleInput(taskId);
        break;

    case PAGE_MON_FORMS:
        MonForms_HandleInput(taskId);
        break;

    case PAGE_MON_RIDE_STATS:
        MonRideStats_HandleInput(taskId);
        break;
    
    default:
        break;
    }
}

static void Task_PageFadeOutAndExit(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if(sPokedexViewReq.dexVariantToRestore != POKEDEX_INVALID_VARIANT)
            RoguePokedex_SetDexVariant(sPokedexViewReq.dexVariantToRestore);

        DestroyPageResources(sPokedexMenu->currentPage, PAGE_NONE);

        Free(sPokedexMenu);
        sPokedexMenu = NULL;

        Free(sTilemapBufferPtr);
        sTilemapBufferPtr = NULL;

        FreeAllWindowBuffers();
        DestroyTask(taskId);

        if(sPokedexViewReq.view == DEX_VIEW_SPECIFIC_MON)
        {
            ReturnToPartyMenuSubMenu();
        }
        else
        {
            SetMainCallback2(gMain.savedCallback);
        }
    }
}

static bool8 IsDebugAltForm(u16 species)
{
#ifdef ROGUE_EXPANSION
    // These forms will only be visible in the dex in debug builds
    //
    if(species >= SPECIES_UNOWN_B && species <= SPECIES_UNOWN_QMARK)
        return TRUE;

    if(species >= SPECIES_PIKACHU_COSPLAY && species <= SPECIES_PIKACHU_WORLD_CAP)
        return TRUE;

    if(species >= SPECIES_ARCEUS_FIGHTING && species <= SPECIES_ARCEUS_FAIRY)
        return TRUE;

    if(species >= SPECIES_GENESECT_DOUSE_DRIVE && species <= SPECIES_GENESECT_CHILL_DRIVE)
        return TRUE;

    if(species >= SPECIES_GRENINJA_BATTLE_BOND && species <= SPECIES_GRENINJA_ASH)
        return TRUE;

    if(species >= SPECIES_VIVILLON_POLAR && species <= SPECIES_VIVILLON_POKE_BALL)
        return TRUE;

    if(species >= SPECIES_PUMPKABOO_SMALL && species <= SPECIES_GOURGEIST_SUPER)
        return TRUE;

    if(species >= SPECIES_FURFROU_HEART_TRIM && species <= SPECIES_FURFROU_PHARAOH_TRIM)
        return TRUE;

    if(species >= SPECIES_SILVALLY_FIGHTING && species <= SPECIES_SILVALLY_FAIRY)
        return TRUE;

    if(species >= SPECIES_MINIOR_ORANGE && species <= SPECIES_MINIOR_CORE_VIOLET)
        return TRUE;

    if(species >= SPECIES_CRAMORANT_GULPING && species <= SPECIES_CRAMORANT_GORGING)
        return TRUE;

    if((species >= SPECIES_ALCREMIE_RUBY_CREAM && species <= SPECIES_ALCREMIE_STRAWBERRY_RAINBOW_SWIRL) || ((species >= SPECIES_ALCREMIE_BERRY && species <= SPECIES_ALCREMIE_RIBBON_RAINBOW_SWIRL)))
    {
        // These are the forms of alcremie which are shown
        if(species == SPECIES_ALCREMIE_STRAWBERRY ||
            species == SPECIES_ALCREMIE_BERRY ||
            species == SPECIES_ALCREMIE_LOVE ||
            species == SPECIES_ALCREMIE_STAR ||
            species == SPECIES_ALCREMIE_CLOVER ||
            species == SPECIES_ALCREMIE_FLOWER ||
            species == SPECIES_ALCREMIE_RIBBON
        )
            return FALSE;
        else
            return TRUE;
    }

    switch (species)
    {
    case SPECIES_PICHU_SPIKY_EARED:
    case SPECIES_FLOETTE_ETERNAL_FLOWER:
    case SPECIES_MIMIKYU_BUSTED:
    case SPECIES_EISCUE_NOICE_FACE:
    case SPECIES_MORPEKO_HANGRY:
    case SPECIES_ETERNATUS_ETERNAMAX:
    case SPECIES_ZARUDE_DADA:
    case SPECIES_GIMMIGHOUL_ROAMING:
        return TRUE;
    }
#endif

    return FALSE;
}

static bool8 IsAltFormVisible(u16 baseForm, u16 altForm)
{
    if(baseForm == altForm)
        return FALSE;

#ifdef ROGUE_EXPANSION
    // catch case like toxtricity where we have an alt dynamax form not in form change table
    if(gSpeciesInfo[altForm].isGigantamax && !IsDynamaxEnabled())
        return FALSE;

    if(gSpeciesInfo[altForm].isTeraForm && !IsTerastallizeEnabled())
        return FALSE;

    // Hide punching form until the reveal
    if(altForm == SPECIES_WOBBUFFET_PUNCHING)
    {
        if(Rogue_Use100PercEffects() || Rogue_Use200PercEffects())
            return TRUE;
        else
            return FALSE;
    }

    if(altForm == SPECIES_PIKIN_MEGA)
    {
        if(FlagGet(FLAG_ROGUE_UNLOCKED_PIKIN_EASTER_EGG))
            return TRUE;
        else
            return FALSE;
    }

    {
        u32 i;
        struct FormChange formChange;

        for (i = 0; TRUE; i++)
        {
            Rogue_ModifyFormChange(baseForm, i, &formChange);

            if(formChange.method == FORM_CHANGE_TERMINATOR)
                break;

            if(formChange.targetSpecies == altForm)
            {
                if(formChange.method == FORM_CHANGE_DISABLED_STUB)
                    return FALSE;
                else
                    break;
            }
        }
    }
#endif
    
    if(IsDebugAltForm(altForm))
    {
#ifdef ROGUE_DEBUG
        return TRUE;
#else
        return FALSE;
#endif
    }

    return TRUE;
}

static u16 GetDisplayedOverviewSpecies(u16 species)
{
    // Always display the base species for this slot
    if(IsCurrentlySelectingMon())
        return species;

#ifdef ROGUE_EXPANSION
    // If we haven't seen the base species check if we've seen any variants
    if(!GetSetPokedexSpeciesFlag(species, FLAG_GET_SEEN))
    {
        u8 i;
        u16 const* formTable = GetSpeciesFormTable(species);

        for(i = 0; formTable && formTable[i] != FORM_SPECIES_END; ++i)
        {
		    // Only consider regional variants
            if(gSpeciesInfo[formTable[i]].isAlolanForm || gSpeciesInfo[formTable[i]].isGalarianForm || gSpeciesInfo[formTable[i]].isHisuianForm || gSpeciesInfo[formTable[i]].isPaldeanForm)
            {
                if(GetSetPokedexSpeciesFlag(formTable[i], FLAG_GET_SEEN))
                {
                    return formTable[i];
                }
            }
        }
    }
#endif

    return species;
}

static bool8 CheckDexCompletion(u8 caseID)
{
    u16 i;
    u16 species;

    u8 dexVariant = RoguePokedex_GetDexVariant();
    u16 dexCount = GetVariantSpeciesCount(dexVariant);
    
    for (i = 0; i < dexCount; i++)
    {
        species = GetVariantSpeciesAt(dexVariant, i);
        species = GetDisplayedOverviewSpecies(species);

        if (!GetSetPokedexSpeciesFlag(species, caseID))
            return FALSE;
    }

    return TRUE;
}

u16 RoguePokedex_CountCaughtMonsForVariant(u16 dexVariant, u8 caseID)
{
    u16 i;
    u16 species;
    u16 count = 0;
    u16 dexCount = GetVariantSpeciesCount(dexVariant);
    
    for (i = 0; i < dexCount; i++)
    {
        species = GetVariantSpeciesAt(dexVariant, i);
        species = GetDisplayedOverviewSpecies(species);

        if (GetSetPokedexSpeciesFlag(species, caseID))
            count++;
    }

    return count;
}

u16 RoguePokedex_CountCurrentCaughtMons(u8 caseID)
{
    return RoguePokedex_CountCaughtMonsForVariant(RoguePokedex_GetDexVariant(), caseID);
}

u16 RoguePokedex_CountNationalCaughtMons(u8 caseID)
{
#ifdef ROGUE_EXPANSION
    return RoguePokedex_CountCaughtMonsForVariant(POKEDEX_VARIANT_NATIONAL_GEN9, caseID);
#else
    return RoguePokedex_CountCaughtMonsForVariant(POKEDEX_VARIANT_NATIONAL_GEN3, caseID);
#endif
}

static void DisplayTitleScreenCountersText(void)
{
    u8 color[3] = { TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY };
    u8 xCoords = RoguePokedex_GetCurrentDexLimit() > 999 ? 0 : 4;
    u8 digits = RoguePokedex_GetCurrentDexLimit() > 999 ? 4 : 3;

    FillWindowPixelBuffer(WIN_TITLE_COUNTERS, PIXEL_FILL(0));

    ConvertUIntToDecimalStringN(gStringVar4, RoguePokedex_CountCurrentCaughtMons(FLAG_GET_SEEN), STR_CONV_MODE_RIGHT_ALIGN, digits);
    AddTextPrinterParameterized4(WIN_TITLE_COUNTERS, FONT_NARROW, xCoords, 0, 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);

    ConvertUIntToDecimalStringN(gStringVar4, RoguePokedex_CountCurrentCaughtMons(FLAG_GET_CAUGHT), STR_CONV_MODE_RIGHT_ALIGN, digits);
    AddTextPrinterParameterized4(WIN_TITLE_COUNTERS, FONT_NARROW, xCoords, 24, 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);

    ConvertUIntToDecimalStringN(gStringVar4, RoguePokedex_CountCurrentCaughtMons(FLAG_GET_CAUGHT_SHINY), STR_CONV_MODE_RIGHT_ALIGN, digits);
    AddTextPrinterParameterized4(WIN_TITLE_COUNTERS, FONT_NARROW, xCoords, 48, 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);

    PutWindowTilemap(WIN_TITLE_COUNTERS);
    CopyWindowToVram(WIN_TITLE_COUNTERS, COPYWIN_FULL);
}

static const u8* GetDexRegionName()
{
    u8 region = RoguePokedex_GetDexRegion();
    return gPokedexRegions[region].displayName;
}

static const u8* GetDexVariantName()
{
    u8 variant = RoguePokedex_GetDexVariant();

    if(variant <= POKEDEX_VARIANT_END)
        return gPokedexVariants[variant].displayName;

    return gText_ThreeMarks;
}

static void AddTitleText(u8 const* title)
{
    u8 color[3] = { TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY };
    u16 offset;

    FillWindowPixelBuffer(WIN_MON_PAGE_TITLE, PIXEL_FILL(0));
    
    offset = GetStringCenterAlignXOffset(FONT_NORMAL, title, sMonEntryWinTemplates[WIN_MON_PAGE_TITLE].width * 8);
    AddTextPrinterParameterized4(WIN_MON_PAGE_TITLE, FONT_NORMAL, offset, 1, 0, 0, color, TEXT_SKIP_DRAW, title);

    PutWindowTilemap(WIN_MON_PAGE_TITLE);
    CopyWindowToVram(WIN_MON_PAGE_TITLE, COPYWIN_FULL);
}


static void DisplayTitleDexVariantText(void)
{
    if(RoguePokedex_IsVariantEditUnlocked())
    {
        u8 color[3] = { TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY };

        FillWindowPixelBuffer(WIN_TITLE_VARIANT_SELECT, PIXEL_FILL(0));

        AddTextPrinterParameterized4(WIN_TITLE_VARIANT_SELECT, FONT_NARROW, 4, 0, 0, 0, color, TEXT_SKIP_DRAW, GetDexRegionName());

        AddTextPrinterParameterized4(WIN_TITLE_VARIANT_SELECT, FONT_NARROW, 4, 24, 0, 0, color, TEXT_SKIP_DRAW, GetDexVariantName());

        PutWindowTilemap(WIN_TITLE_VARIANT_SELECT);
        CopyWindowToVram(WIN_TITLE_VARIANT_SELECT, COPYWIN_FULL);
    }
}

static void DisplayMonEntryText(void)
{
    u8 color[3] = { TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY };
    u16 speciesNum = RoguePokedex_GetSpeciesCurrentNum(sPokedexMenu->viewBaseSpecies);
    
    ConvertUIntToDecimalStringN(gStringVar1, speciesNum, STR_CONV_MODE_LEADING_ZEROS, speciesNum > 999 ? 4 : 3);
    StringExpandPlaceholders(gStringVar3, gText_NumberStr1);

    AddTextPrinterParameterized4(WIN_MON_SPECIES_NAME_NO, FONT_NARROW, 4, 1, 0, 0, color, TEXT_SKIP_DRAW, RoguePokedex_GetSpeciesName(sPokedexMenu->viewBaseSpecies));
    AddTextPrinterParameterized4(WIN_MON_SPECIES_NAME_NO, FONT_NARROW, 4, 17, 0, 0, color, TEXT_SKIP_DRAW, gStringVar3);

    PutWindowTilemap(WIN_MON_SPECIES_NAME_NO);
    CopyWindowToVram(WIN_MON_SPECIES_NAME_NO, COPYWIN_FULL);
}

extern const u8 gAbilityNames[][ABILITY_NAME_LENGTH + 1];

#define GET_STAT_COLOUR(stat) GET_STAT_COLOUR_RANGE(stats[stat], bestStatValue, worstStatValue)
#define GET_STAT_COLOUR_RANGE(value, bestValue, worstColor) (value >= bestValue ? bestStatColor : (value <= worstColor ? worstStatColor : statColor))

static void DisplayMonStatsText(void)
{
    u8 i;
    const u8 ySpacing = 16;
    u8 headerColor[3] = { TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY };
    u8 statColor[3] = { TEXT_COLOR_TRANSPARENT, TEXT_COLOR_BLUE, TEXT_COLOR_LIGHT_GRAY };

    AddTitleText(sTitle_Stats);

    FillWindowPixelBuffer(WIN_MON_PAGE_CONTENT, PIXEL_FILL(0));

    // Print types (Sprites display types setup later)
    AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NORMAL, 4, 1, 0, 0, headerColor, TEXT_SKIP_DRAW, sText_Types);

    // Print abilities
    {
        u16 prevAbility = ABILITY_NONE;
        u8 j = 1;

        AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NORMAL, 4, 1 + ySpacing * j, 0, 0, headerColor, TEXT_SKIP_DRAW, sText_Abilities);
        ++j;

        for(i = 0; i < NUM_ABILITY_SLOTS; ++i)
        {
            u16 ability = GetAbilityBySpecies(sPokedexMenu->viewBaseSpecies, i, sPokedexMenu->viewOtId);

            if(ability != ABILITY_NONE && ability != prevAbility)
            {
                AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 4, 1 + ySpacing * j, 0, 0, statColor, TEXT_SKIP_DRAW, gAbilityNames[ability]);
                prevAbility = ability;
                ++j;
            }
            else
            {
                AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 4, 1 + ySpacing * j, 0, 0, statColor, TEXT_SKIP_DRAW, gText_Dash);
                prevAbility = ability;
                ++j;
            }
        }
    }

    // Print stats
    {
        u16 bst;
        u8 stats[NUM_STATS];
        u8 bestStatValue;
        u8 worstStatValue;
        u8 bestStatColor[3] = { TEXT_COLOR_TRANSPARENT, TEXT_COLOR_GREEN, TEXT_COLOR_LIGHT_GRAY };
        u8 worstStatColor[3] = { TEXT_COLOR_TRANSPARENT, TEXT_COLOR_RED, TEXT_COLOR_LIGHT_GRAY };
        i = 0;

        GatherSpeciesStatsArray(sPokedexMenu->viewBaseSpecies, stats);
        bestStatValue = stats[RoguePokedex_GetSpeciesBestStat(sPokedexMenu->viewBaseSpecies)];
        worstStatValue = stats[RoguePokedex_GetSpeciesWorstStat(sPokedexMenu->viewBaseSpecies)];
        bst = RoguePokedex_GetSpeciesBST(sPokedexMenu->viewBaseSpecies);

        // Total
        ++i;
        AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NORMAL, 72, 1 + ySpacing * i, 0, 0, headerColor, TEXT_SKIP_DRAW, sText_Total);

        ConvertUIntToDecimalStringN(gStringVar4, bst, STR_CONV_MODE_RIGHT_ALIGN, 3);
        AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 115, 1 + ySpacing * i, 0, 0, GET_STAT_COLOUR_RANGE(bst, 600, 299), TEXT_SKIP_DRAW, gStringVar4);

        // HP
        ++i;
        AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NORMAL, 72, 1 + ySpacing * i, 0, 0, headerColor, TEXT_SKIP_DRAW, sText_HP);

        ConvertUIntToDecimalStringN(gStringVar4, stats[STAT_HP], STR_CONV_MODE_RIGHT_ALIGN, 3);
        AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 115, 1 + ySpacing * i, 0, 0, GET_STAT_COLOUR(STAT_HP), TEXT_SKIP_DRAW, gStringVar4);

        // Attack
        ++i;
        AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NORMAL, 72, 1 + ySpacing * i, 0, 0, headerColor, TEXT_SKIP_DRAW, sText_Attack);
        
        ConvertUIntToDecimalStringN(gStringVar4, stats[STAT_ATK], STR_CONV_MODE_RIGHT_ALIGN, 3);
        AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 115, 1 + ySpacing * i, 0, 0, GET_STAT_COLOUR(STAT_ATK), TEXT_SKIP_DRAW, gStringVar4);

        // Def
        ++i;
        AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NORMAL, 72, 1 + ySpacing * i, 0, 0, headerColor, TEXT_SKIP_DRAW, sText_Defence);
        
        ConvertUIntToDecimalStringN(gStringVar4, stats[STAT_DEF], STR_CONV_MODE_RIGHT_ALIGN, 3);
        AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 115, 1 + ySpacing * i, 0, 0, GET_STAT_COLOUR(STAT_DEF), TEXT_SKIP_DRAW, gStringVar4);

        // SpAttack
        ++i;
        AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NORMAL, 72, 1 + ySpacing * i, 0, 0, headerColor, TEXT_SKIP_DRAW, sText_SpAttack);
        
        ConvertUIntToDecimalStringN(gStringVar4, stats[STAT_SPATK], STR_CONV_MODE_RIGHT_ALIGN, 3);
        AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 115, 1 + ySpacing * i, 0, 0, GET_STAT_COLOUR(STAT_SPATK), TEXT_SKIP_DRAW, gStringVar4);

        // SpDef
        ++i;
        AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NORMAL, 72, 1 + ySpacing * i, 0, 0, headerColor, TEXT_SKIP_DRAW, sText_SpDefence);
        
        ConvertUIntToDecimalStringN(gStringVar4, stats[STAT_SPDEF], STR_CONV_MODE_RIGHT_ALIGN, 3);
        AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 115, 1 + ySpacing * i, 0, 0, GET_STAT_COLOUR(STAT_SPDEF), TEXT_SKIP_DRAW, gStringVar4);

        // Speed
        ++i;
        // Move 1 pixel higher
        AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NORMAL, 72, 0 + ySpacing * i, 0, 0, headerColor, TEXT_SKIP_DRAW, sText_Speed);

        ConvertUIntToDecimalStringN(gStringVar4, stats[STAT_SPEED], STR_CONV_MODE_RIGHT_ALIGN, 3);
        AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 115, 1 + ySpacing * i, 0, 0, GET_STAT_COLOUR(STAT_SPEED), TEXT_SKIP_DRAW, gStringVar4);
    }

    PutWindowTilemap(WIN_MON_PAGE_CONTENT);
    CopyWindowToVram(WIN_MON_PAGE_CONTENT, COPYWIN_FULL);
}

#undef GET_STAT_COLOUR
#undef GET_STAT_COLOUR_RANGE

#define MAX_LIST_DISPLAY_COUNT 8

static u16 GetMaxMoveScrollOffset()
{
    u8 i;
    u16 count = 0;
    u16 species = sPokedexMenu->viewBaseSpecies;
    u32 customMonId = RogueGift_GetCustomMonIdBySpecies(species, sPokedexMenu->viewOtId);
    
    // Custom moves
    if(customMonId)
    {
        count += RogueGift_GetCustomMonMoveCount(customMonId);
    }

    // Level up
    for (i = 0; TRUE; i++)
    {
        if (gRoguePokemonProfiles[species].levelUpMoves[i].move == MOVE_NONE)
            break;
        ++count;
    }

    // Tutor/TM moves
    for (i = 0; TRUE; i++)
    {
        if (gRoguePokemonProfiles[species].tutorMoves[i] == MOVE_NONE)
            break;
        ++count;
    }

    return count - min(count, MAX_LIST_DISPLAY_COUNT);
}

static void DisplayMonMovesText()
{
    u8 i;
    u8 listIndex = 0;
    u8 displayCount = 0;
    const u8 ySpacing = 16;
    u8 color[3] = { TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY };
    u16 species = sPokedexMenu->viewBaseSpecies;

    AddTitleText(sTitle_Moves);

    FillWindowPixelBuffer(WIN_MON_PAGE_CONTENT, PIXEL_FILL(0));

    // Custom moves
    if(sPokedexMenu->viewOtId)
    {
        u32 customMonId = RogueGift_GetCustomMonIdBySpecies(species, sPokedexMenu->viewOtId);
        if(customMonId != 0)
        {
            u16 customMoveCount = RogueGift_GetCustomMonMoveCount(customMonId);
            
            for (i = 0; i < customMoveCount; i++)
            {
                // Is custom move
                StringCopy(gStringVar1, gMoveNames[RogueGift_GetCustomMonMove(customMonId, i)]);
                StringExpandPlaceholders(gStringVar3, gText_PokedexMovesCustom);
                
                if(listIndex >= sPokedexMenu->listScrollAmount)
                {
                    AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 4, ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, gStringVar3);
                    ++displayCount;
                }
                ++listIndex;
            }
        }
    }

    // Level moves
    {
        for (i = 0; displayCount < MAX_LIST_DISPLAY_COUNT; i++)
        {
            if (gRoguePokemonProfiles[species].levelUpMoves[i].move == MOVE_NONE)
                break;

            if(gRoguePokemonProfiles[species].levelUpMoves[i].level == 0)
            {
                // Is evo move
                StringCopy(gStringVar1, gMoveNames[gRoguePokemonProfiles[species].levelUpMoves[i].move]);
                StringExpandPlaceholders(gStringVar3, gText_PokedexMovesEvo);
            }
            else
            { 
                ConvertUIntToDecimalStringN(gStringVar1, gRoguePokemonProfiles[species].levelUpMoves[i].level, STR_CONV_MODE_RIGHT_ALIGN, 2);
                StringCopy(gStringVar2, gMoveNames[gRoguePokemonProfiles[species].levelUpMoves[i].move]);
                StringExpandPlaceholders(gStringVar3, gText_PokedexMovesLevel);
            }
            
            if(listIndex >= sPokedexMenu->listScrollAmount)
            {
                AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 4, ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, gStringVar3);
                ++displayCount;
            }
            ++listIndex;
        }
    }

    // TR moves
    if(displayCount < MAX_LIST_DISPLAY_COUNT)
    {
        u16 moveId;

        for(i = 0; displayCount < MAX_LIST_DISPLAY_COUNT; ++i)
        {
            moveId = gRoguePokemonProfiles[species].tutorMoves[i];

            if(moveId == MOVE_NONE)
                break;

            if(!MonEvos_IsTutorMoveTR(i))
                continue;

            if(listIndex >= sPokedexMenu->listScrollAmount)
            {
                StringCopy(gStringVar1, gMoveNames[moveId]);
                StringExpandPlaceholders(gStringVar2, gText_PokedexMovesTR);

                AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 4, ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, gStringVar2);
                ++displayCount;
            }
            ++listIndex;
        }
    }

    // TM moves
    if(displayCount < MAX_LIST_DISPLAY_COUNT)
    {
        u16 moveId;

        for(i = 0; displayCount < MAX_LIST_DISPLAY_COUNT; ++i)
        {
            moveId = gRoguePokemonProfiles[species].tutorMoves[i];

            if(moveId == MOVE_NONE)
                break;

            if(!MonEvos_IsTutorMoveTM(i))
                continue;
            
            if(listIndex >= sPokedexMenu->listScrollAmount)
            {
                StringCopy(gStringVar1, gMoveNames[moveId]);
                StringExpandPlaceholders(gStringVar2, gText_PokedexMovesTM);

                AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 4, ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, gStringVar2);
                ++displayCount;
            }
            ++listIndex;
        }
    }

    // Tutor moves
    if(displayCount < MAX_LIST_DISPLAY_COUNT)
    {
        u16 moveId;

        for(i = 0; displayCount < MAX_LIST_DISPLAY_COUNT; ++i)
        {
            moveId = gRoguePokemonProfiles[species].tutorMoves[i];

            if(moveId == MOVE_NONE)
                break;

            if(!MonEvos_IsTutorMove(i))
                continue;

            if(listIndex >= sPokedexMenu->listScrollAmount)
            {
                StringCopy(gStringVar1, gMoveNames[moveId]);
                StringExpandPlaceholders(gStringVar2, gText_PokedexMovesTutor);

                AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 4, ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, gStringVar2);
                ++displayCount;
            }
            ++listIndex;
        }
    }

    PutWindowTilemap(WIN_MON_PAGE_CONTENT);
    CopyWindowToVram(WIN_MON_PAGE_CONTENT, COPYWIN_FULL);
}

static u16 GetMaxEvoScrollOffset()
{
    u8 i;
    u8 count = 0;
    struct Evolution evo;
    u8 evoCount = Rogue_GetMaxEvolutionCount(sPokedexMenu->viewBaseSpecies);

    for(i = 0; i < evoCount; ++i)
    {
        Rogue_ModifyEvolution(sPokedexMenu->viewBaseSpecies, i, &evo);
        Rogue_ModifyEvolution_ApplyCurses(sPokedexMenu->viewBaseSpecies, i, &evo);

        if(evo.targetSpecies == SPECIES_NONE)
            continue;

        ++count;
    }

    return count != 0 ? count - 1 : 0;
}


static u16 GetActiveEvoSpecies()
{
    u8 i;
    u8 listIndex = 0;
    struct Evolution evo;
    u8 evoCount = Rogue_GetMaxEvolutionCount(sPokedexMenu->viewBaseSpecies);

    for(i = 0; i < evoCount; ++i)
    {
        Rogue_ModifyEvolution(sPokedexMenu->viewBaseSpecies, i, &evo);
        Rogue_ModifyEvolution_ApplyCurses(sPokedexMenu->viewBaseSpecies, i, &evo);

        if(evo.targetSpecies == SPECIES_NONE)
            continue;

        if(listIndex >= sPokedexMenu->listScrollAmount)
            return evo.targetSpecies;

        ++listIndex;
    }

    return SPECIES_NONE;
}

#ifdef ROGUE_EXPANSION
static u16 GetMaxFormScrollOffset()
{
    u8 i;
    u8 count = 0;
    u16 const* formTable = GetSpeciesFormTable(sPokedexMenu->viewBaseSpecies);

    for(i = 0; formTable && formTable[i] != FORM_SPECIES_END; ++i)
    {
        if(IsAltFormVisible(sPokedexMenu->viewBaseSpecies, formTable[i]))
            ++count;
    }

    return count != 0 ? count - 1 : 0;
}

static u16 GetActiveFormSpecies()
{
    u8 i;
    u8 listIndex = 0;
    u16 const* formTable = GetSpeciesFormTable(sPokedexMenu->viewBaseSpecies);

    for(i = 0; formTable && formTable[i] != FORM_SPECIES_END; ++i)
    {
        if(IsAltFormVisible(sPokedexMenu->viewBaseSpecies, formTable[i]))
        {
            if(listIndex >= sPokedexMenu->listScrollAmount)
                return formTable[i];

            ++listIndex;
        }
    }

    return SPECIES_NONE;
}
#endif

extern const u8 gTypeNames[NUMBER_OF_MON_TYPES][TYPE_NAME_LENGTH + 1];
extern const u8 *const gNatureNamePointers[];

static void DisplayMonEvosText()
{
    u8 i;
    const u8 ySpacing = 16;
    u8 color[3] = { TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY };
    u8 listIndex = 0;
    u8 displayCount = 0;
    struct Evolution evo;
    u8 evoCount = Rogue_GetMaxEvolutionCount(sPokedexMenu->viewBaseSpecies);

    AddTitleText(sTitle_Evolutions);

    FillWindowPixelBuffer(WIN_MON_PAGE_CONTENT, PIXEL_FILL(0));

    for(i = 0; i < evoCount && displayCount < 8; ++i)
    {
        Rogue_ModifyEvolution(sPokedexMenu->viewBaseSpecies, i, &evo);
        Rogue_ModifyEvolution_ApplyCurses(sPokedexMenu->viewBaseSpecies, i, &evo);

        if(evo.targetSpecies == SPECIES_NONE)
            continue;

        if(listIndex >= sPokedexMenu->listScrollAmount)
        {
            switch(evo.method)
            {
                case EVO_FRIENDSHIP:
                    StringCopy(gStringVar4, gText_PokedexEvoFriendship);
                    break;
                case EVO_FRIENDSHIP_DAY:
                    StringCopy(gStringVar4, gText_PokedexEvoFriendshipDay);
                    break;
                case EVO_FRIENDSHIP_NIGHT:
                    StringCopy(gStringVar4, gText_PokedexEvoFriendshipNight);
                    break;

                case EVO_LEVEL:
                case EVO_LEVEL_SILCOON:
                case EVO_LEVEL_CASCOON:
                case EVO_LEVEL_NINJASK:
                case EVO_LEVEL_SHEDINJA:
                    ConvertUIntToDecimalStringN(gStringVar1, evo.param, STR_CONV_MODE_LEFT_ALIGN, 2);
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoLevel);
                    break;
                case EVO_TRADE:
                    StringCopy(gStringVar4, gText_PokedexEvoTrade);
                    break;
                case EVO_TRADE_ITEM:
                    StringCopy(gStringVar1, Rogue_GetItemName(evo.param));
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoTradeItem);
                    break;
                case EVO_ITEM:
                    StringCopy(gStringVar1, Rogue_GetItemName(evo.param));
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoItem);
                    break;
        
                case EVO_LEVEL_ATK_GT_DEF:
                    ConvertUIntToDecimalStringN(gStringVar1, evo.param, STR_CONV_MODE_LEFT_ALIGN, 2);
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoLevelAtkGtDef);
                    break;
                case EVO_LEVEL_ATK_EQ_DEF:
                    ConvertUIntToDecimalStringN(gStringVar1, evo.param, STR_CONV_MODE_LEFT_ALIGN, 2);
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoLevelAtkEqDef);
                    break;
                case EVO_LEVEL_ATK_LT_DEF:
                    ConvertUIntToDecimalStringN(gStringVar1, evo.param, STR_CONV_MODE_LEFT_ALIGN, 2);
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoLevelAtkLtDef);
                    break;
                
                case EVO_BEAUTY:
                    StringCopy(gStringVar4, gText_PokedexEvoBeauty);
                    break;
                case EVO_LEVEL_ITEM:
                    StringCopy(gStringVar1, Rogue_GetItemName(evo.param));
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoLevelItem);
                    break;

                case EVO_LEVEL_DAY:
                    ConvertUIntToDecimalStringN(gStringVar1, evo.param, STR_CONV_MODE_LEFT_ALIGN, 2);
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoLevelDay);
                    break;
                case EVO_LEVEL_NIGHT:
                    ConvertUIntToDecimalStringN(gStringVar1, evo.param, STR_CONV_MODE_LEFT_ALIGN, 2);
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoLevelNight);
                    break;

#ifdef ROGUE_EXPANSION
                case EVO_LEVEL_MALE:
                    ConvertUIntToDecimalStringN(gStringVar1, evo.param, STR_CONV_MODE_LEFT_ALIGN, 2);
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoLevelMale);
                    break;
                case EVO_LEVEL_FEMALE:
                    ConvertUIntToDecimalStringN(gStringVar1, evo.param, STR_CONV_MODE_LEFT_ALIGN, 2);
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoLevelFemale);
                    break;

                case EVO_LEVEL_DUSK:
                    ConvertUIntToDecimalStringN(gStringVar1, evo.param, STR_CONV_MODE_LEFT_ALIGN, 2);
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoLevelDusk);
                    break;

                case EVO_ITEM_HOLD_DAY:
                    StringCopy(gStringVar1, Rogue_GetItemName(evo.param));
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoLevelItemDay);
                    break;
                case EVO_ITEM_HOLD_NIGHT:
                    StringCopy(gStringVar1, Rogue_GetItemName(evo.param));
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoLevelItemNight);
                    break;

                case EVO_MOVE:
                    StringCopy(gStringVar1, gMoveNames[evo.param]);
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoMove);
                    break;
                case EVO_MOVE_TYPE:
                    StringCopy(gStringVar1, gTypeNames[evo.param]);
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoMoveType);
                    break;

                case EVO_ITEM_MALE:
                    StringCopy(gStringVar1, Rogue_GetItemName(evo.param));
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoItemMale);
                    break;
                case EVO_ITEM_FEMALE:
                    StringCopy(gStringVar1, Rogue_GetItemName(evo.param));
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoItemFemale);
                    break;

                case EVO_LEVEL_RAIN:
                    ConvertUIntToDecimalStringN(gStringVar1, evo.param, STR_CONV_MODE_LEFT_ALIGN, 2);
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoLevelRain);
                    break;

                case EVO_LEVEL_NATURE_AMPED:
                case EVO_LEVEL_NATURE_LOW_KEY:
                    ConvertUIntToDecimalStringN(gStringVar1, evo.param, STR_CONV_MODE_LEFT_ALIGN, 2);
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoLevel);
                    break;

                case EVO_LEVEL_TWO_SEGMENT:
                case EVO_LEVEL_THREE_SEGMENT:
                case EVO_LEVEL_FAMILY_OF_THREE:
                case EVO_LEVEL_FAMILY_OF_FOUR:
                    ConvertUIntToDecimalStringN(gStringVar1, evo.param, STR_CONV_MODE_LEFT_ALIGN, 2);
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoLevel);
                    break;

                case EVO_LEVEL_30_NATURE:
                    StringCopy(gStringVar1, gNatureNamePointers[evo.param]);
                    StringExpandPlaceholders(gStringVar4, gText_PokedexEvoLevel30Nature);
                    break;
#endif
                default:
                    StringCopy(gStringVar4, gText_PokedexEvoTODO);
                    break;
            };

            // Add arrow
            if(displayCount == 0)
                AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 35, ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, gText_SelectorArrow);

            if(GetSetPokedexSpeciesFlag(evo.targetSpecies, FLAG_GET_SEEN))
            {
                AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 35 + (displayCount == 0 ? 8 : 0), ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, RoguePokedex_GetSpeciesName(evo.targetSpecies));
                ++displayCount;
            }
            else
            {
                AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 35 + (displayCount == 0 ? 8 : 0), ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, gText_FiveMarks);
                ++displayCount;
            }

            AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 35, ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);
            ++displayCount;
        }
        ++listIndex;
    }

    if(displayCount == 0)
    {
        AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 15, 0, 0, 0, color, TEXT_SKIP_DRAW, gText_PokedexEvoNoData);
    }

    PutWindowTilemap(WIN_MON_PAGE_CONTENT);
    CopyWindowToVram(WIN_MON_PAGE_CONTENT, COPYWIN_FULL);
}

static void DisplayMonFormsText()
{
#ifdef ROGUE_EXPANSION
    u8 i;
    const u8 ySpacing = 16;
    u8 color[3] = { TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY };
    u8 listIndex = 0;
    u8 displayCount = 0;
    u16 const* formTable = GetSpeciesFormTable(sPokedexMenu->viewBaseSpecies);

    AddTitleText(sTitle_Forms);

    FillWindowPixelBuffer(WIN_MON_PAGE_CONTENT, PIXEL_FILL(0));

    for(i = 0; formTable && formTable[i] != FORM_SPECIES_END && displayCount < 8; ++i)
    {
        if(IsAltFormVisible(sPokedexMenu->viewBaseSpecies, formTable[i]))
        {
            if(listIndex >= sPokedexMenu->listScrollAmount)
            {
                // Add arrow
                if(displayCount == 0)
                    AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 35, ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, gText_SelectorArrow);

                if(GetSetPokedexSpeciesFlag(formTable[i], FLAG_GET_SEEN))
                {
                    AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 35 + (displayCount == 0 ? 8 : 0), ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, RoguePokedex_GetSpeciesName(formTable[i]));
                    ++displayCount;
                }
                else
                {
                    AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 35 + (displayCount == 0 ? 8 : 0), ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, gText_FiveMarks);
                    ++displayCount;
                }

                if(gSpeciesInfo[formTable[i]].isAlolanForm)
                {
                    AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 35, ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, sText_Alolan);
                }
                else if(gSpeciesInfo[formTable[i]].isGalarianForm)
                {
                    AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 35, ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, sText_Galarian);
                }
                else if(gSpeciesInfo[formTable[i]].isPaldeanForm)
                {
                    AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 35, ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, sText_Paldean);
                }
                else if(gSpeciesInfo[formTable[i]].isHisuianForm)
                {
                    AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 35, ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, sText_Hisuian);
                }
                else if(gSpeciesInfo[formTable[i]].isMegaEvolution)
                {
                    AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 35, ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, sText_Mega);
                }
                else if(gSpeciesInfo[formTable[i]].isPrimalReversion)
                {
                    AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 35, ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, sText_Primal);
                }
                else if(gSpeciesInfo[formTable[i]].isUltraBurst)
                {
                    AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 35, ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, sText_UltraBurst);
                }
                else if(gSpeciesInfo[formTable[i]].isGigantamax)
                {
                    AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 35, ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, sText_Gigantamax);
                }
                else if(gSpeciesInfo[formTable[i]].isTeraForm)
                {
                    AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 35, ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, sText_TeraForm);
                }
                else if(GET_BASE_SPECIES_ID(sPokedexMenu->viewBaseSpecies) == formTable[i])
                {
                    AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 35, ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, sText_Base);
                }
                else if(IsDebugAltForm(formTable[i]))
                {
                    AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 35, ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, sText_Debug);
                }

                //AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 35, ySpacing * displayCount, 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);
                ++displayCount;
            }
            ++listIndex;
        }
    }

    if(displayCount == 0)
    {
        u16 offset = GetStringCenterAlignXOffset(FONT_NARROW, sText_NoFormData, sMonEntryWinTemplates[WIN_MON_PAGE_CONTENT].width * 8);
        AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, offset, 0, 0, 0, color, TEXT_SKIP_DRAW, sText_NoFormData);
    }

    PutWindowTilemap(WIN_MON_PAGE_CONTENT);
    CopyWindowToVram(WIN_MON_PAGE_CONTENT, COPYWIN_FULL);
#endif
}

static void DisplayMonRideStatsText()
{
    const u8 ySpacing = 16;
    u8 headerColor[3] = { TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY };
    u8 statColor[3] = { TEXT_COLOR_TRANSPARENT, TEXT_COLOR_BLUE, TEXT_COLOR_LIGHT_GRAY };

    AddTitleText(sTitle_Riding);

    FillWindowPixelBuffer(WIN_MON_PAGE_CONTENT, PIXEL_FILL(0));

    if(Rogue_IsValidRideSpecies(sPokedexMenu->viewBaseSpecies))
    {
        u16 i;
        u16 y = 0;
        u8 skillCount = 0;

        // Speed
        AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NORMAL, 4, 1 + ySpacing * y, 0, 0, headerColor, TEXT_SKIP_DRAW, sText_Speed);

        StringCopy(gStringVar4, gText_EmptyString2);

        for(i = 0; i < Rogue_GetRideSpeciesSpeedStars(sPokedexMenu->viewBaseSpecies); ++i)
            StringAppend(gStringVar4, sText_RideStar);

        AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NORMAL, 40, 1 + ySpacing * y, 0, 0, statColor, TEXT_SKIP_DRAW, gStringVar4);
        ++y;

        // Skills
        AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NORMAL, 4, 1 + ySpacing * y, 0, 0, headerColor, TEXT_SKIP_DRAW, sText_Skills);
        ++y;

        if(FlagGet(FLAG_SYS_RIDING_LEDGE_JUMP) && Rogue_IsValidRideClimbSpecies(sPokedexMenu->viewBaseSpecies))
        {
            AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 4, 1 + ySpacing * y, 0, 0, statColor, TEXT_SKIP_DRAW, sText_SkillClimbing);
            ++y;
            ++skillCount;
        }

        if(FlagGet(FLAG_SYS_RIDING_SURF) && Rogue_IsValidRideSwimSpecies(sPokedexMenu->viewBaseSpecies))
        {
            AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 4, 1 + ySpacing * y, 0, 0, statColor, TEXT_SKIP_DRAW, sText_SkillSurf);
            ++y;
            ++skillCount;
        }

        if(FlagGet(FLAG_SYS_RIDING_FLY) && Rogue_IsValidRideFlySpecies(sPokedexMenu->viewBaseSpecies))
        {
            AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 4, 1 + ySpacing * y, 0, 0, statColor, TEXT_SKIP_DRAW, sText_SkillFlying);
            ++y;
            ++skillCount;
        }

        if(skillCount == 0)
        {
            AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, 4, 1 + ySpacing * y, 0, 0, statColor, TEXT_SKIP_DRAW, sText_SkillNone);
            ++y;
        }
    }
    else
    {
        u16 offset = GetStringCenterAlignXOffset(FONT_NARROW, sText_NoDataFound, sMonEntryWinTemplates[WIN_MON_PAGE_CONTENT].width * 8);
        AddTextPrinterParameterized4(WIN_MON_PAGE_CONTENT, FONT_NARROW, offset, 0, 0, 0, statColor, TEXT_SKIP_DRAW, sText_NoDataFound);
    }

    PutWindowTilemap(WIN_MON_PAGE_CONTENT);
    CopyWindowToVram(WIN_MON_PAGE_CONTENT, COPYWIN_FULL);
}

static const struct BgTemplate sDiplomaBgTemplates[2] =
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

static void InitOverviewBg(void)
{
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sDiplomaBgTemplates, ARRAY_COUNT(sDiplomaBgTemplates));
    SetBgTilemapBuffer(1, sTilemapBufferPtr);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
    ShowBg(0);
    ShowBg(1);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_BLDALPHA, 0);
    SetGpuReg(REG_OFFSET_BLDY, 0);
}

static void InitMonEntryWindows(void)
{
    u8 i;

    InitWindows(sMonEntryWinTemplates);
    DeactivateAllTextPrinters();
    LoadPalette(gStandardMenuPalette, 0xF0, 0x20);
    
    for(i = 0; i < WIN_COUNT; ++i)
    {
        FillWindowPixelBuffer(i, PIXEL_FILL(0));
        PutWindowTilemap(i);
    }
}

static void DestroyMonEntryWindows(void)
{
    u8 i;

    for(i = 0; i < WIN_COUNT; ++i)
    {
        ClearStdWindowAndFrameToTransparent(i, TRUE);
        RemoveWindow(i);
    }

    FreeAllWindowBuffers();
}

// Title screen
//
static const struct ScrollArrowsTemplate sTitleScreen_ModeArrowsTemplate_Region =
{
    .firstArrowType = SCROLL_ARROW_LEFT,
    .firstX = 63,
    .firstY = 64,
    .secondArrowType = SCROLL_ARROW_RIGHT,
    .secondX = 184,
    .secondY = 64,
    .fullyUpThreshold = -1,
    .fullyDownThreshold = -1,
    .tileTag = 5325,
    .palTag = 5325,
    .palNum = 0,
};

static const struct ScrollArrowsTemplate sTitleScreen_ModeArrowsTemplate_Game =
{
    .firstArrowType = SCROLL_ARROW_LEFT,
    .firstX = 63,
    .firstY = 88,
    .secondArrowType = SCROLL_ARROW_RIGHT,
    .secondX = 184,
    .secondY = 88,
    .fullyUpThreshold = -1,
    .fullyDownThreshold = -1,
    .tileTag = 5325,
    .palTag = 5325,
    .palNum = 0,
};

static void TitleScreen_AddScrollArrows(void)
{
    AGB_ASSERT(sPokedexMenu != NULL);
    if (sPokedexMenu->displayArrowTask == TASK_NONE)
    {
        if(sPokedexMenu->titleScreenCursorIdx == 0)
            sPokedexMenu->displayArrowTask = AddScrollIndicatorArrowPair(&sTitleScreen_ModeArrowsTemplate_Region, &sPokedexMenu->displayArrowOffset);
        else
            sPokedexMenu->displayArrowTask = AddScrollIndicatorArrowPair(&sTitleScreen_ModeArrowsTemplate_Game, &sPokedexMenu->displayArrowOffset);
    }
}

static void TitleScreen_RemoveScrollArrows(void)
{
    AGB_ASSERT(sPokedexMenu != NULL);
    if (sPokedexMenu->displayArrowTask != TASK_NONE)
    {
        RemoveScrollIndicatorArrowPair(sPokedexMenu->displayArrowTask);
        sPokedexMenu->displayArrowTask = TASK_NONE;
    }
}

static void TitleScreen_HandleInput(u8 taskId)
{
    if(sPokedexMenu->titleScreenInEditMode)
    {
        if (JOY_NEW(A_BUTTON | B_BUTTON | SELECT_BUTTON))
        {
            sPokedexMenu->titleScreenInEditMode = FALSE;
            PlaySE(SE_PIN);

            //DisplayTitleScreenCountersText();
            DisplayTitleDexVariantText();
            TitleScreen_RemoveScrollArrows();
        }
        else if(JOY_NEW(DPAD_UP | DPAD_DOWN))
        {
            PlaySE(SE_SELECT);
            sPokedexMenu->titleScreenCursorIdx = (sPokedexMenu->titleScreenCursorIdx + 1) % 2;
            DisplayTitleDexVariantText();
            TitleScreen_RemoveScrollArrows();
            TitleScreen_AddScrollArrows();
        }
        else if(JOY_REPEAT(DPAD_LEFT))
        {
            // Edit region
            if(sPokedexMenu->titleScreenCursorIdx == 0)
            {
                u8 region = RoguePokedex_GetDexRegion();

                PlaySE(SE_SELECT);

                if(region == POKEDEX_REGION_START)
                    region = POKEDEX_REGION_END;
                else
                    --region;

                RoguePokedex_SetDexRegion(region);
            }
            // Edit variant
            else
            {
                u8 region = RoguePokedex_GetDexRegion();

                if(region == POKEDEX_REGION_NONE)
                {
                    PlaySE(SE_SELECT);
                    AGB_ASSERT(FALSE); // old code path
                }
                else if(gPokedexRegions[region].variantCount <= 1)
                {
                    // Cannot change variant as we don't have one to swap to
                    PlaySE(SE_FAILURE);
                }
                else
                {
                    u8 idx;
                    u8 variant = RoguePokedex_GetDexVariant();

                    for(idx = 0; idx < gPokedexRegions[region].variantCount; ++idx)
                    {
                        if(gPokedexRegions[region].variantList[idx] == variant)
                            break;
                    }

                    if(idx < gPokedexRegions[region].variantCount)
                    {
                        PlaySE(SE_SELECT);

                        if(idx == 0)
                            idx = gPokedexRegions[region].variantCount - 1;
                        else
                            --idx;

                        RoguePokedex_SetDexVariant(gPokedexRegions[region].variantList[idx]);
                    }
                    else
                    {
                        // ???
                        PlaySE(SE_FAILURE);
                    }
                }
            }

            DisplayTitleDexVariantText();
            DisplayTitleScreenCountersText();
            TitleScreen_RefillStarsBg();
        }
        else if(JOY_REPEAT(DPAD_RIGHT))
        {
            // Edit region
            if(sPokedexMenu->titleScreenCursorIdx == 0)
            {
                u8 region = RoguePokedex_GetDexRegion();

                PlaySE(SE_SELECT);

                if(region == POKEDEX_REGION_END)
                    region = POKEDEX_REGION_START;
                else if(region == POKEDEX_REGION_END)
                    region = POKEDEX_REGION_START;
                else
                    ++region;

                RoguePokedex_SetDexRegion(region);
            }
            // Edit variant
            else
            {
                u8 region = RoguePokedex_GetDexRegion();

                if(region == POKEDEX_REGION_NONE)
                {
                    PlaySE(SE_SELECT);
                    AGB_ASSERT(FALSE); // old code path
                }
                else if(gPokedexRegions[region].variantCount <= 1)
                {
                    // Cannot change variant as we don't have one to swap to
                    PlaySE(SE_FAILURE);
                }
                else
                {
                    u8 idx;
                    u8 variant = RoguePokedex_GetDexVariant();

                    for(idx = 0; idx < gPokedexRegions[region].variantCount; ++idx)
                    {
                        if(gPokedexRegions[region].variantList[idx] == variant)
                            break;
                    }

                    if(idx < gPokedexRegions[region].variantCount)
                    {
                        PlaySE(SE_SELECT);
                        idx = (idx + 1) % gPokedexRegions[region].variantCount;

                        RoguePokedex_SetDexVariant(gPokedexRegions[region].variantList[idx]);
                    }
                    else
                    {
                        // ???
                        PlaySE(SE_FAILURE);
                    }
                }
            }

            DisplayTitleDexVariantText();
            DisplayTitleScreenCountersText();
            TitleScreen_RefillStarsBg();
        }
    }
    else
    {
        if (JOY_NEW(A_BUTTON))
        {
            sPokedexMenu->pageScrollAmount = 0;
            sPokedexMenu->selectedIdx = 0;

            sPokedexMenu->desiredPage = PAGE_OVERVIEW;
            gTasks[taskId].func = Task_SwapToPage;

            PlaySE(SE_WIN_OPEN);
        }
        else if (JOY_NEW(B_BUTTON))
        {
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            gTasks[taskId].func = Task_PageFadeOutAndExit;

            PlaySE(SE_PC_OFF);
        }
        else if (JOY_NEW(SELECT_BUTTON) && RoguePokedex_IsVariantEditEnabled())
        {
            sPokedexMenu->titleScreenInEditMode = TRUE;
            sPokedexMenu->titleScreenCursorIdx = 0;
            PlaySE(SE_PIN);

            //DisplayTitleScreenCountersText();
            DisplayTitleDexVariantText();
            TitleScreen_AddScrollArrows();
        }
    }
}

static void TitleScreen_RefillBg()
{
    if(!RoguePokedex_IsVariantEditEnabled())
    {
        // Remove prompt hint
        FillBgTilemapBufferRect_Palette0(1, 0x04, 19, 18, 8, 1);
    }

    if(!RoguePokedex_IsVariantEditUnlocked())
    {
        // Remove text boxes for dex variant select
        FillBgTilemapBufferRect_Palette0(1, 0x04, 8, 7, 15, 2);
        FillBgTilemapBufferRect_Palette0(1, 0x04, 8, 10, 15, 2);

    }

    // Add stars for the full completion
    if(CheckDexCompletion(FLAG_GET_SEEN))
    {
        FillBgTilemapBufferRect_Palette0(1, 0x5C, 5, 7, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x5D, 6, 7, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x66, 5, 8, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x67, 6, 8, 1, 1);
    }

    if(CheckDexCompletion(FLAG_GET_CAUGHT))
    {
        FillBgTilemapBufferRect_Palette0(1, 0x5C, 5, 10, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x5D, 6, 10, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x66, 5, 11, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x67, 6, 11, 1, 1);
    }

    if(CheckDexCompletion(FLAG_GET_CAUGHT_SHINY))
    {
        FillBgTilemapBufferRect_Palette0(1, 0x5C, 5, 13, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x5D, 6, 13, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x66, 5, 14, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x67, 6, 14, 1, 1);
    }

    TitleScreen_RefillStarsBg();
}

static void TitleScreen_RefillStarsBg()
{
    if(CheckDexCompletion(FLAG_GET_SEEN))
    {
        FillBgTilemapBufferRect_Palette0(1, 0x5C, 5, 7, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x5D, 6, 7, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x66, 5, 8, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x67, 6, 8, 1, 1);
    }
    else
    {
        FillBgTilemapBufferRect_Palette0(1, 0x48, 5, 7, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x49, 6, 7, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x48 | FLIP_VERTICAL, 5, 8, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x49 | FLIP_VERTICAL, 6, 8, 1, 1);
    }

    if(CheckDexCompletion(FLAG_GET_CAUGHT))
    {
        FillBgTilemapBufferRect_Palette0(1, 0x5C, 5, 10, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x5D, 6, 10, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x66, 5, 11, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x67, 6, 11, 1, 1);
    }
    else
    {
        FillBgTilemapBufferRect_Palette0(1, 0x48, 5, 10, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x49, 6, 10, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x48 | FLIP_VERTICAL, 5, 11, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x49 | FLIP_VERTICAL, 6, 11, 1, 1);
    }

    if(CheckDexCompletion(FLAG_GET_CAUGHT_SHINY))
    {
        FillBgTilemapBufferRect_Palette0(1, 0x5C, 5, 13, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x5D, 6, 13, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x66, 5, 14, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x67, 6, 14, 1, 1);
    }
    else
    {
        FillBgTilemapBufferRect_Palette0(1, 0x48, 5, 13, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x49, 6, 13, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x48 | FLIP_VERTICAL, 5, 14, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x49 | FLIP_VERTICAL, 6, 14, 1, 1);
    }

    ScheduleBgCopyTilemapToVram(1);
}

// Overview
//
enum
{
    ENTRY_TYPE_NONE,        // background
    ENTRY_TYPE_DISABLED,
    ENTRY_TYPE_EMPTY, 
    ENTRY_TYPE_QUESTION_MARK, // ?
    ENTRY_TYPE_SEEN,
    ENTRY_TYPE_CAUGHT,
    ENTRY_TYPE_CAUGHT_SHINY,
    ENTRY_TYPE_GREEN_CIRCLE,
    ENTRY_TYPE_RED_CROSS,
};

enum
{
    ENTRY_DIR_LEFT,
    ENTRY_DIR_CENTRE,
    ENTRY_DIR_RIGHT,
    ENTRY_DIR_COUNT
};

#define HEADER_EMPTY 0x0E
#define HEADER_BLACK 0x43
#define HEADER_NUM0  0x0D
#define HEADER_NUM1  0x03
#define HEADER_NUM2  0x04
#define HEADER_NUM3  0x05
#define HEADER_NUM4  0x07
#define HEADER_NUM5  0x08
#define HEADER_NUM6  0x09
#define HEADER_NUM7  0x0A
#define HEADER_NUM8  0x0B
#define HEADER_NUM9  0x0C

static u8 Overview_SelectDigitTile(u8 digit)
{
    switch (digit)
    {
    case 0:
        return HEADER_NUM0;
    case 1:
        return HEADER_NUM1;
    case 2:
        return HEADER_NUM2;
    case 3:
        return HEADER_NUM3;
    case 4:
        return HEADER_NUM4;
    case 5:
        return HEADER_NUM5;
    case 6:
        return HEADER_NUM6;
    case 7:
        return HEADER_NUM7;
    case 8:
        return HEADER_NUM8;
    case 9:
        return HEADER_NUM9;
    }

    return HEADER_EMPTY;
}

static bool32 CheckIfAnyEvosMatch(u16 species, u8 dexFlag)
{
    if(GetSetPokedexSpeciesFlag(species, dexFlag))
        return TRUE;

    {
        struct Evolution evo;
        u32 i;
        u32 evoCount = Rogue_GetMaxEvolutionCount(species);

        for(i = 0; i < evoCount; ++i)
        {
            Rogue_ModifyEvolution(species, i, &evo);

            if(evo.targetSpecies != SPECIES_NONE)
            {
                if(CheckIfAnyEvosMatch(evo.targetSpecies, dexFlag))
                    return TRUE;
            }
        }
    }

    return FALSE;
}

static bool32 GetSpeciesDisplayDexFlag(u16 species, u8 dexFlag)
{
    u8 dexVariant = RoguePokedex_GetDexVariant();
    
    // Daycare variant, we want to display based on if we have any data in the evo chain
    if(dexVariant == POKEDEX_DYNAMIC_VARIANT_EGG_SPECIES)
        return CheckIfAnyEvosMatch(species, dexFlag);
    else
        return GetSetPokedexSpeciesFlag(species, dexFlag);
}

static u8 Overview_GetEntryType(s8 entryX, s8 entryY, s8 deltaX, s8 deltaY)
{
    u8 idx;
    u16 species;

    entryX += deltaX;
    entryY += deltaY;

    if(entryX < 0 || entryX >= COLUMN_ENTRY_COUNT)
        return ENTRY_TYPE_NONE;

    if(entryY < 0 || entryY >= ROW_ENTRY_COUNT)
        return ENTRY_TYPE_NONE;

    idx = entryX + entryY * COLUMN_ENTRY_COUNT;
    species = sPokedexMenu->overviewPageSpecies[idx];

    if(species == SPECIES_NONE)
        return ENTRY_TYPE_DISABLED;

        
    // We don't care if we've seen this mon or not
    if(IsCurrentlySelectingMon())
    {
        //u8 dexVariant = RoguePokedex_GetDexVariant();
//
        //if(dexVariant == POKEDEX_DYNAMIC_VARIANT_EGG_SPECIES)
        //{
        //    // This view is used for daycare egg selection, so display mons we have seen/caught to hatch regardless of where in the evo chain we have dex data
        //    if(CheckIfAnyEvosMatch(species, FLAG_GET_CAUGHT))
        //        return ENTRY_TYPE_CAUGHT;
        //    else if(CheckIfAnyEvosMatch(species, FLAG_GET_SEEN))
        //        return ENTRY_TYPE_SEEN;
        //}
        //else 
        
        if(
            (!sPokedexViewReq.perView.selectMon.requireSeen || GetSpeciesDisplayDexFlag(species, FLAG_GET_SEEN)) &&
            (!sPokedexViewReq.perView.selectMon.requireCaught || GetSpeciesDisplayDexFlag(species, FLAG_GET_CAUGHT))
        )
        {
            if(sPokedexViewReq.view == DEX_VIEW_SELECT_SAFARI_MON)
            {
                return ENTRY_TYPE_EMPTY;
            }
            else
            {
                // Display icons based on dex state
                if(GetSpeciesDisplayDexFlag(species, FLAG_GET_CAUGHT_SHINY))
                    return ENTRY_TYPE_CAUGHT_SHINY;
                else if(GetSpeciesDisplayDexFlag(species, FLAG_GET_CAUGHT))
                    return ENTRY_TYPE_CAUGHT;
                else if(GetSpeciesDisplayDexFlag(species, FLAG_GET_SEEN))
                    return ENTRY_TYPE_SEEN;
            }
        }
    }
    else
    {
        // Display icons based on dex state
        if(GetSpeciesDisplayDexFlag(species, FLAG_GET_CAUGHT_SHINY))
            return ENTRY_TYPE_CAUGHT_SHINY;
        else if(GetSpeciesDisplayDexFlag(species, FLAG_GET_CAUGHT))
            return ENTRY_TYPE_CAUGHT;
        else if(GetSpeciesDisplayDexFlag(species, FLAG_GET_SEEN))
            return ENTRY_TYPE_SEEN;
    }

    return ENTRY_TYPE_QUESTION_MARK;
}

static bool8 Overview_IsEntrySelected(s8 entryX, s8 entryY, s8 deltaX, s8 deltaY)
{
    entryX += deltaX;
    entryY += deltaY;

    if(entryX < 0 || entryX >= COLUMN_ENTRY_COUNT)
        return FALSE;

    if(entryY < 0 || entryY >= ROW_ENTRY_COUNT)
        return FALSE;

    return entryX + entryY * COLUMN_ENTRY_COUNT == sPokedexMenu->selectedIdx;
}


static void Overview_FillEntryTileBoundary_Header(u8 tileX, u8 tileY, u8 leftType, u8 rightType, bool8 leftSelected, bool8 rightSelected)
{
    switch (leftType)
    {
    case ENTRY_TYPE_NONE:
        if(rightType == ENTRY_TYPE_DISABLED)
            FillBgTilemapBufferRect_Palette0(1, 0x42, tileX + 0, tileY + 0, 1, 1);
        else
            FillBgTilemapBufferRect_Palette0(1, 0x02, tileX + 0, tileY + 0, 1, 1);
        break;

    case ENTRY_TYPE_DISABLED:
        if(rightType == ENTRY_TYPE_NONE)
            FillBgTilemapBufferRect_Palette0(1, 0x42 | FLIP_HORIZONTAL, tileX + 0, tileY + 0, 1, 1);
        else if(rightType == ENTRY_TYPE_DISABLED)
            FillBgTilemapBufferRect_Palette0(1, 0x45, tileX + 0, tileY + 0, 1, 1);
        else
            FillBgTilemapBufferRect_Palette0(1, 0x44, tileX + 0, tileY + 0, 1, 1);
        break;

    default:
        if(rightType == ENTRY_TYPE_NONE)
            FillBgTilemapBufferRect_Palette0(1, 0x02 | FLIP_HORIZONTAL, tileX + 0, tileY + 0, 1, 1);
        else if(rightType == ENTRY_TYPE_DISABLED)
            FillBgTilemapBufferRect_Palette0(1, 0x44 | FLIP_HORIZONTAL, tileX + 0, tileY + 0, 1, 1);
        else
            FillBgTilemapBufferRect_Palette0(1, 0x06, tileX + 0, tileY + 0, 1, 1);
        break;
    }
}

static void Overview_FillEntryTileBoundary_Body(u8 tileX, u8 tileY, u8 leftType, u8 rightType, bool8 leftSelected, bool8 rightSelected)
{
    switch (leftType)
    {
    case ENTRY_TYPE_NONE:
        if(rightType == ENTRY_TYPE_DISABLED)
        {
            FillBgTilemapBufferRect_Palette0(1, 0x42, tileX + 0, tileY + 1, 1, 3);
            FillBgTilemapBufferRect_Palette0(1, 0x4B, tileX + 0, tileY + 4, 1, 1);
        }
        else
        {
            if(rightSelected)
            {
                FillBgTilemapBufferRect_Palette0(1, 0x29, tileX + 0, tileY + 1, 1, 1);
                FillBgTilemapBufferRect_Palette0(1, 0x2F, tileX + 0, tileY + 2, 1, 2);
                FillBgTilemapBufferRect_Palette0(1, 0x33, tileX + 0, tileY + 4, 1, 1);
            }
            else
            {
                FillBgTilemapBufferRect_Palette0(1, 0x11, tileX + 0, tileY + 1, 1, 1);
                FillBgTilemapBufferRect_Palette0(1, 0x17, tileX + 0, tileY + 2, 1, 2);
                FillBgTilemapBufferRect_Palette0(1, 0x20, tileX + 0, tileY + 4, 1, 1);
            }
        }
        break;

    case ENTRY_TYPE_DISABLED:
        if(rightType == ENTRY_TYPE_NONE)
        {
            FillBgTilemapBufferRect_Palette0(1, 0x42 | FLIP_HORIZONTAL, tileX + 0, tileY + 1, 1, 3);
            FillBgTilemapBufferRect_Palette0(1, 0x4B | FLIP_HORIZONTAL, tileX + 0, tileY + 4, 1, 1);
        }
        else if(rightType == ENTRY_TYPE_DISABLED)
        {
            FillBgTilemapBufferRect_Palette0(1, 0x45, tileX + 0, tileY + 1, 1, 3);
            FillBgTilemapBufferRect_Palette0(1, 0x4E, tileX + 0, tileY + 4, 1, 1);
        }
        else
        {
            if(rightSelected)
            {
                FillBgTilemapBufferRect_Palette0(1, 0x47, tileX + 0, tileY + 1, 1, 1);
                FillBgTilemapBufferRect_Palette0(1, 0x49, tileX + 0, tileY + 2, 1, 2);
                FillBgTilemapBufferRect_Palette0(1, 0x4F, tileX + 0, tileY + 4, 1, 1);
            }
            else
            {
                FillBgTilemapBufferRect_Palette0(1, 0x46, tileX + 0, tileY + 1, 1, 1);
                FillBgTilemapBufferRect_Palette0(1, 0x48, tileX + 0, tileY + 2, 1, 2);
                FillBgTilemapBufferRect_Palette0(1, 0x4D, tileX + 0, tileY + 4, 1, 1);
            }
        }
        break;

    default:
        if(leftSelected)
        {
            if(rightType == ENTRY_TYPE_NONE)
            {
                FillBgTilemapBufferRect_Palette0(1, 0x29 | FLIP_HORIZONTAL, tileX + 0, tileY + 1, 1, 1);
                FillBgTilemapBufferRect_Palette0(1, 0x2F | FLIP_HORIZONTAL, tileX + 0, tileY + 2, 1, 2);
                FillBgTilemapBufferRect_Palette0(1, 0x33 | FLIP_HORIZONTAL, tileX + 0, tileY + 4, 1, 1);
            }
            else if(rightType == ENTRY_TYPE_DISABLED)
            {
                FillBgTilemapBufferRect_Palette0(1, 0x47 | FLIP_HORIZONTAL, tileX + 0, tileY + 1, 1, 1);
                FillBgTilemapBufferRect_Palette0(1, 0x49 | FLIP_HORIZONTAL, tileX + 0, tileY + 2, 1, 2);
                FillBgTilemapBufferRect_Palette0(1, 0x4F | FLIP_HORIZONTAL, tileX + 0, tileY + 4, 1, 1);
            }
            else
            {
                if(rightSelected)
                {
                    FillBgTilemapBufferRect_Palette0(1, 0x2B, tileX + 0, tileY + 1, 1, 1);
                    FillBgTilemapBufferRect_Palette0(1, 0x30, tileX + 0, tileY + 2, 1, 2);
                    FillBgTilemapBufferRect_Palette0(1, 0x36, tileX + 0, tileY + 4, 1, 1);
                }
                else
                {
                    FillBgTilemapBufferRect_Palette0(1, 0x3E | FLIP_HORIZONTAL, tileX + 0, tileY + 1, 1, 1);
                    FillBgTilemapBufferRect_Palette0(1, 0x40 | FLIP_HORIZONTAL, tileX + 0, tileY + 2, 1, 2);
                    FillBgTilemapBufferRect_Palette0(1, 0x41 | FLIP_HORIZONTAL, tileX + 0, tileY + 4, 1, 1);
                }
            }
        }
        else
        {
            if(rightType == ENTRY_TYPE_NONE)
            {
                FillBgTilemapBufferRect_Palette0(1, 0x11 | FLIP_HORIZONTAL, tileX + 0, tileY + 1, 1, 1);
                FillBgTilemapBufferRect_Palette0(1, 0x17 | FLIP_HORIZONTAL, tileX + 0, tileY + 2, 1, 2);
                FillBgTilemapBufferRect_Palette0(1, 0x20 | FLIP_HORIZONTAL, tileX + 0, tileY + 4, 1, 1);
            }
            else if(rightType == ENTRY_TYPE_DISABLED)
            {
                FillBgTilemapBufferRect_Palette0(1, 0x46 | FLIP_HORIZONTAL, tileX + 0, tileY + 1, 1, 1);
                FillBgTilemapBufferRect_Palette0(1, 0x48 | FLIP_HORIZONTAL, tileX + 0, tileY + 2, 1, 2);
                FillBgTilemapBufferRect_Palette0(1, 0x4D | FLIP_HORIZONTAL, tileX + 0, tileY + 4, 1, 1);
            }
            else
            {
                if(rightSelected)
                {
                    FillBgTilemapBufferRect_Palette0(1, 0x3E, tileX + 0, tileY + 1, 1, 1);
                    FillBgTilemapBufferRect_Palette0(1, 0x40, tileX + 0, tileY + 2, 1, 2);
                    FillBgTilemapBufferRect_Palette0(1, 0x41, tileX + 0, tileY + 4, 1, 1);
                }
                else
                {
                    FillBgTilemapBufferRect_Palette0(1, 0x13, tileX + 0, tileY + 1, 1, 1);
                    FillBgTilemapBufferRect_Palette0(1, 0x18, tileX + 0, tileY + 2, 1, 2);
                    FillBgTilemapBufferRect_Palette0(1, 0x23, tileX + 0, tileY + 4, 1, 1);
                }
            }
        }
        break;
    }
}

static void Overview_FillEntryTileCentre_Header(u8 tileX, u8 tileY, u8 entryType, u16 numToDisplay)
{
    if(entryType == ENTRY_TYPE_DISABLED)
        FillBgTilemapBufferRect_Palette0(1, HEADER_BLACK, tileX + 1, tileY + 0, 3, 1);
    else if(numToDisplay == (u16)-1)
        FillBgTilemapBufferRect_Palette0(1, HEADER_EMPTY, tileX + 1, tileY + 0, 3, 1);
    else
    {
        u8 digits[3];
        digits[0] = numToDisplay % 10;
        digits[1] = (numToDisplay / 10) % 10;
        digits[2] = (numToDisplay / 100) % 10;

        FillBgTilemapBufferRect_Palette0(1, Overview_SelectDigitTile(digits[2]), tileX + 1, tileY + 0, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, Overview_SelectDigitTile(digits[1]), tileX + 2, tileY + 0, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, Overview_SelectDigitTile(digits[0]), tileX + 3, tileY + 0, 1, 1);
    }
}

static void Overview_FillEntryTileCentre_Body(u8 tileX, u8 tileY, u8 entryType, bool8 entrySelected)
{
    if(entryType == ENTRY_TYPE_NONE)
    {
        // Should never reach here so make it look obv broken
        FillBgTilemapBufferRect_Palette0(1, 0x28, tileX + 1, tileY + 1, 3, 4);
    }
    else if(entryType == ENTRY_TYPE_DISABLED)
    {
        FillBgTilemapBufferRect_Palette0(1, 0x43, tileX + 1, tileY + 1, 3, 3);
        FillBgTilemapBufferRect_Palette0(1, 0x4C, tileX + 1, tileY + 4, 3, 1);
    }
    else if(entryType == ENTRY_TYPE_QUESTION_MARK)
    {
        FillBgTilemapBufferRect_Palette0(1, 0x19, tileX + 1, tileY + 2, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x1A, tileX + 2, tileY + 2, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x1B, tileX + 3, tileY + 2, 1, 1);

        FillBgTilemapBufferRect_Palette0(1, 0x0F, tileX + 1, tileY + 3, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x1D, tileX + 2, tileY + 3, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x1E, tileX + 3, tileY + 3, 1, 1);

        if(entrySelected)
        {
            FillBgTilemapBufferRect_Palette0(1, 0x2A, tileX + 1, tileY + 1, 1, 1);
            FillBgTilemapBufferRect_Palette0(1, 0x2C, tileX + 2, tileY + 1, 1, 1);
            FillBgTilemapBufferRect_Palette0(1, 0x2D, tileX + 3, tileY + 1, 1, 1);

            FillBgTilemapBufferRect_Palette0(1, 0x34, tileX + 1, tileY + 4, 1, 1);
            FillBgTilemapBufferRect_Palette0(1, 0x39, tileX + 2, tileY + 4, 1, 1);
            FillBgTilemapBufferRect_Palette0(1, 0x34 | FLIP_HORIZONTAL, tileX + 3, tileY + 4, 1, 1);
        }
        else
        {
            FillBgTilemapBufferRect_Palette0(1, 0x12, tileX + 1, tileY + 1, 1, 1);
            FillBgTilemapBufferRect_Palette0(1, 0x14, tileX + 2, tileY + 1, 1, 1);
            FillBgTilemapBufferRect_Palette0(1, 0x15, tileX + 3, tileY + 1, 1, 1);

            FillBgTilemapBufferRect_Palette0(1, 0x21, tileX + 1, tileY + 4, 1, 1);
            FillBgTilemapBufferRect_Palette0(1, 0x26, tileX + 2, tileY + 4, 1, 1);
            FillBgTilemapBufferRect_Palette0(1, 0x21, tileX + 3, tileY + 4, 1, 1);
        }
    }
    else
    {
        // All of these have the same background and swap out the bottom most tile
        FillBgTilemapBufferRect_Palette0(1, entrySelected ? 0x2A : 0x12, tileX + 1, tileY + 1, 3, 1);
        FillBgTilemapBufferRect_Palette0(1, 0x0F, tileX + 1, tileY + 2, 3, 2);
        
        FillBgTilemapBufferRect_Palette0(1, entrySelected ? 0x34 : 0x21, tileX + 1, tileY + 4, 1, 1);
        FillBgTilemapBufferRect_Palette0(1, entrySelected ? 0x34 | FLIP_HORIZONTAL : 0x21, tileX + 3, tileY + 4, 1, 1);

        switch (entryType)
        {
        case ENTRY_TYPE_SEEN:
            FillBgTilemapBufferRect_Palette0(1, entrySelected ? 0x38 : 0x25, tileX + 2, tileY + 4, 1, 1);
            break;

        case ENTRY_TYPE_CAUGHT:
            FillBgTilemapBufferRect_Palette0(1, entrySelected ? 0x37 : 0x24, tileX + 2, tileY + 4, 1, 1);
            break;

        case ENTRY_TYPE_CAUGHT_SHINY:
            FillBgTilemapBufferRect_Palette0(1, entrySelected ? 0x35 : 0x22, tileX + 2, tileY + 4, 1, 1);
            break;

        case ENTRY_TYPE_GREEN_CIRCLE:
            FillBgTilemapBufferRect_Palette0(1, entrySelected ? 0x3B : 0x27, tileX + 2, tileY + 4, 1, 1);
            break;

        case ENTRY_TYPE_RED_CROSS:
            FillBgTilemapBufferRect_Palette0(1, entrySelected ? 0x3C : 0x28, tileX + 2, tileY + 4, 1, 1);
            break;

        default: // ENTRY_TYPE_EMPTY
            FillBgTilemapBufferRect_Palette0(1, entrySelected ? 0x3A : 0x21, tileX + 2, tileY + 4, 1, 1);
            break;
        }
    }
}

static void Overview_HandleInput(u8 taskId)
{
    bool8 justJumpedPage = FALSE;
    u16 prevSelectedIdx = sPokedexMenu->selectedIdx;
    u16 prevScrollAmount = sPokedexMenu->pageScrollAmount;

    if(JOY_REPEAT(DPAD_LEFT))
    {
        u8 x, y;
        x = sPokedexMenu->selectedIdx % COLUMN_ENTRY_COUNT;
        y = sPokedexMenu->selectedIdx / COLUMN_ENTRY_COUNT;

        if(x == 0)
            x = COLUMN_ENTRY_COUNT - 1;
        else
            --x;

        sPokedexMenu->selectedIdx = x + y * COLUMN_ENTRY_COUNT;
    }
    else if(JOY_REPEAT(DPAD_RIGHT))
    {
        u8 x, y;
        x = sPokedexMenu->selectedIdx % COLUMN_ENTRY_COUNT;
        y = sPokedexMenu->selectedIdx / COLUMN_ENTRY_COUNT;

        x = (x + 1) % COLUMN_ENTRY_COUNT;
        sPokedexMenu->selectedIdx = x + y * COLUMN_ENTRY_COUNT;
    }
    else if(JOY_REPEAT(DPAD_UP))
    {
        if(sPokedexMenu->selectedIdx >= COLUMN_ENTRY_COUNT)
            sPokedexMenu->selectedIdx -= COLUMN_ENTRY_COUNT; // jump back a row
        else
        {
            if(sPokedexMenu->pageScrollAmount != 0)
            --sPokedexMenu->pageScrollAmount;
        }
    }
    else if(JOY_REPEAT(DPAD_DOWN))
    {
        if(sPokedexMenu->selectedIdx < OVERVIEW_ENTRY_COUNT - COLUMN_ENTRY_COUNT)
            sPokedexMenu->selectedIdx += COLUMN_ENTRY_COUNT; // jump down a row
        else
            ++sPokedexMenu->pageScrollAmount;
    }
    else if(JOY_REPEAT(L_BUTTON))
    {
        if(sPokedexMenu->pageScrollAmount != 0)
            sPokedexMenu->pageScrollAmount -= min(sPokedexMenu->pageScrollAmount, ROW_ENTRY_COUNT);
        else if(sPokedexMenu->selectedIdx != 0)
            sPokedexMenu->selectedIdx = 0; // Put back to first slot before looping
        else
            sPokedexMenu->pageScrollAmount = Overview_GetMaxScrollAmount();

        justJumpedPage = TRUE;
    }
    else if(JOY_REPEAT(R_BUTTON))
    {
        u8 maxScrollAmount = Overview_GetMaxScrollAmount();
        sPokedexMenu->pageScrollAmount += ROW_ENTRY_COUNT;
        
        if(sPokedexMenu->pageScrollAmount > maxScrollAmount)
        {
            u8 maxIdx = Overview_GetLastValidActiveIndex();
            if(sPokedexMenu->selectedIdx != maxIdx)
            {
                sPokedexMenu->pageScrollAmount = maxScrollAmount;
                sPokedexMenu->selectedIdx = maxIdx;
            }
            else
            {
                sPokedexMenu->pageScrollAmount = 0;
            }
        }

        justJumpedPage = TRUE;
    }
    else if (JOY_NEW(A_BUTTON))
    {
        u16 species = sPokedexMenu->overviewPageSpecies[sPokedexMenu->selectedIdx];

        if(IsCurrentlySelectingMon())
        {
            if(
                (!sPokedexViewReq.perView.selectMon.requireSeen || GetSpeciesDisplayDexFlag(species, FLAG_GET_SEEN)) &&
                (!sPokedexViewReq.perView.selectMon.requireCaught || GetSpeciesDisplayDexFlag(species, FLAG_GET_CAUGHT))
            )
            {
                if(sPokedexViewReq.view == DEX_VIEW_SELECT_SAFARI_MON)
                {
                    // Return index for safari mon
                    u16 dexVariant = RoguePokedex_GetDexVariant();
                    u16 dexIndex = sPokedexMenu->pageScrollAmount * COLUMN_ENTRY_COUNT + sPokedexMenu->selectedIdx;
                    
                    switch (dexVariant)
                    {
                        case POKEDEX_DYNAMIC_VARIANT_NORMAL_SAFARI:
                        case POKEDEX_DYNAMIC_VARIANT_LEGEND_SAFARI:
                        {
                            u16 i = (dexVariant == POKEDEX_DYNAMIC_VARIANT_LEGEND_SAFARI) ? ROGUE_SAFARI_LEGENDS_START_INDEX : 0;
                            u16 total = (dexVariant == POKEDEX_DYNAMIC_VARIANT_LEGEND_SAFARI) ? ROGUE_SAFARI_TOTAL_MONS : ROGUE_SAFARI_LEGENDS_START_INDEX;
                            u16 count = 0;

                            for(; i < total; ++i)
                            {
                                if(gRogueSaveBlock->safariMons[i].species != SPECIES_NONE)
                                {
                                    if(dexIndex == count++)
                                    {
                                        gSpecialVar_Result = i;
                                        break;
                                    }
                                }
                            }

                            if(i >= ROGUE_SAFARI_TOTAL_MONS)
                            {
                                AGB_ASSERT(FALSE);
                                gSpecialVar_Result = ROGUE_SAFARI_TOTAL_MONS;
                            }
                        }

                        default:
                            AGB_ASSERT(FALSE);
                            break;
                    }
                }
                else
                    gSpecialVar_Result = species;

                // Immediately exit if viewing view party summary
                BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
                gTasks[taskId].func = Task_PageFadeOutAndExit;

                PlaySE(SE_PIN);
            }
            else
            {
                // Can't select if we haven't seen this mon
                PlaySE(SE_FAILURE);
            }
        }
        else
        {
            if(GetSpeciesDisplayDexFlag(species, FLAG_GET_SEEN))
            {
                // Swap to the stats page
                sPokedexMenu->desiredPage = PAGE_MON_STATS;
                sPokedexMenu->viewBaseSpecies = species;
                gTasks[taskId].func = Task_SwapToPage;

                PlaySE(SE_PIN);
            }
            else
            {
                // Can't open if we haven't seen this mon
                PlaySE(SE_FAILURE);
            }
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        if(IsCurrentlySelectingMon())
        {
            gSpecialVar_Result = (sPokedexViewReq.view == DEX_VIEW_SELECT_SAFARI_MON) ? ROGUE_SAFARI_TOTAL_MONS : SPECIES_NONE;

            // Immediately exit if viewing view party summary
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            gTasks[taskId].func = Task_PageFadeOutAndExit;

            PlaySE(SE_PC_OFF);
        }
        else
        {
            sPokedexMenu->desiredPage = PAGE_TITLE_SCREEN;
            gTasks[taskId].func = Task_SwapToPage;

            PlaySE(SE_SELECT);
        }
    }


    // Clamp scroll amount
    if(prevSelectedIdx  != sPokedexMenu->selectedIdx)
    {
        sPokedexMenu->selectedIdx = min(sPokedexMenu->selectedIdx, Overview_GetLastValidActiveIndex());
    }

    if(prevScrollAmount != sPokedexMenu->pageScrollAmount)
    {
        sPokedexMenu->pageScrollAmount = min(sPokedexMenu->pageScrollAmount, Overview_GetMaxScrollAmount());
    }

    if(prevScrollAmount != sPokedexMenu->pageScrollAmount)
    {
        // Scroll up/down
        Overview_SelectSpeciesToDiplay();
        
        // Clamp it here incase we go out of bounds when jumping from front to back
        sPokedexMenu->selectedIdx = min(sPokedexMenu->selectedIdx, Overview_GetLastValidActiveIndex());

        Overview_RefillBg();
        Overview_DestroySprites();
        Overview_CreateSprites();

        PlaySE(justJumpedPage ? SE_DEX_PAGE : SE_DEX_SCROLL);
    }
    else if(prevSelectedIdx != sPokedexMenu->selectedIdx)
    {
        // Highlight new selected entry
        u8 prevEntryX = prevSelectedIdx % COLUMN_ENTRY_COUNT;
        u8 prevEntryY = prevSelectedIdx / COLUMN_ENTRY_COUNT;
        u8 newEntryX = sPokedexMenu->selectedIdx % COLUMN_ENTRY_COUNT;
        u8 newEntryY = sPokedexMenu->selectedIdx / COLUMN_ENTRY_COUNT;

        Overview_FillEntryBg_Selected(prevEntryX, prevEntryY, FALSE);
        Overview_FillEntryBg_Selected(newEntryX, newEntryY, FALSE);

        PlaySE(SE_DEX_SCROLL);
    }
}

static void Overview_RefillBg()
{
    u8 x = 0;
    u8 y = 0;

    for(x = 0; x < COLUMN_ENTRY_COUNT; ++x)
        for(y = 0; y < ROW_ENTRY_COUNT; ++y)
            Overview_FillEntryBg(x, y, TRUE);
}

static void Overview_FillEntryBgInternal(u8 entryX, u8 entryY, bool8 includeHeader, bool8 includeRightColumn)
{
    u8 tileX = 1 + entryX * 4;
    u8 tileY = 0 + entryY * 5;
    u8 entryType[ENTRY_DIR_COUNT];
    bool8 entrySelected[ENTRY_DIR_COUNT];

    AGB_ASSERT(entryX + entryY * COLUMN_ENTRY_COUNT < COLUMN_ENTRY_COUNT * ROW_ENTRY_COUNT);

    entryType[ENTRY_DIR_LEFT] = Overview_GetEntryType(entryX, entryY, -1, 0);
    entryType[ENTRY_DIR_CENTRE] = Overview_GetEntryType(entryX, entryY, 0, 0);
    entryType[ENTRY_DIR_RIGHT] = Overview_GetEntryType(entryX, entryY, 1, 0);

    entrySelected[ENTRY_DIR_LEFT] = Overview_IsEntrySelected(entryX, entryY, -1, 0);
    entrySelected[ENTRY_DIR_CENTRE] = Overview_IsEntrySelected(entryX, entryY, 0, 0);
    entrySelected[ENTRY_DIR_RIGHT] = Overview_IsEntrySelected(entryX, entryY, 1, 0);

    if(includeHeader)
    {
        u16 num = sPokedexMenu->overviewPageNumbers[entryX + entryY * COLUMN_ENTRY_COUNT];

        Overview_FillEntryTileBoundary_Header(tileX, tileY, entryType[ENTRY_DIR_LEFT], entryType[ENTRY_DIR_CENTRE], entrySelected[ENTRY_DIR_LEFT], entrySelected[ENTRY_DIR_CENTRE]);
        Overview_FillEntryTileCentre_Header(tileX, tileY, entryType[ENTRY_DIR_CENTRE], num);

        // If we're in the last column (Or we are JUST refreshing this tile) handle right hand side
        if(entryX + 1 == COLUMN_ENTRY_COUNT || includeRightColumn)
            Overview_FillEntryTileBoundary_Header(tileX + 4, tileY, entryType[ENTRY_DIR_CENTRE], entryType[ENTRY_DIR_RIGHT], entrySelected[ENTRY_DIR_CENTRE], entrySelected[ENTRY_DIR_RIGHT]);
    }

    {
        Overview_FillEntryTileBoundary_Body(tileX, tileY, entryType[ENTRY_DIR_LEFT], entryType[ENTRY_DIR_CENTRE], entrySelected[ENTRY_DIR_LEFT], entrySelected[ENTRY_DIR_CENTRE]);
        Overview_FillEntryTileCentre_Body(tileX, tileY, entryType[ENTRY_DIR_CENTRE], entrySelected[ENTRY_DIR_CENTRE]);

        // If we're in the last column (Or we are JUST refreshing this tile) handle right hand side
        if(entryX + 1 == COLUMN_ENTRY_COUNT || includeRightColumn)
            Overview_FillEntryTileBoundary_Body(tileX + 4, tileY, entryType[ENTRY_DIR_CENTRE], entryType[ENTRY_DIR_RIGHT], entrySelected[ENTRY_DIR_CENTRE], entrySelected[ENTRY_DIR_RIGHT]);
    }

    ScheduleBgCopyTilemapToVram(1);
}

static void Overview_FillEntryBg(u8 entryX, u8 entryY, bool8 includeHeader)
{
    Overview_FillEntryBgInternal(entryX, entryY, TRUE, FALSE);
}

static void Overview_FillEntryBg_Selected(u8 entryX, u8 entryY, bool8 includeHeader)
{
    Overview_FillEntryBgInternal(entryX, entryY, TRUE, TRUE);
}

static void Overview_CreateSprites()
{
    u8 i, x , y;
    u16 species;
    
    for(x = 0; x < 7; ++x)
    {
        for(y = 0; y < 4; ++y)
        {
            i = x + y * COLUMN_ENTRY_COUNT;
            species = sPokedexMenu->overviewPageSpecies[i];

            if(species != SPECIES_NONE)
            {
                if(IsCurrentlySelectingMon())
                {
                    if(sPokedexViewReq.perView.selectMon.requireSeen && !GetSpeciesDisplayDexFlag(species, FLAG_GET_SEEN))
                        continue;

                    if(sPokedexViewReq.perView.selectMon.requireCaught && !GetSpeciesDisplayDexFlag(species, FLAG_GET_CAUGHT))
                        continue;

                    // Always display in select mon view
                    // Non animated
                    sPokedexMenu->pageSprites[i] = CreateMonIcon(sPokedexMenu->overviewPageSpecies[i], SpriteCallbackDummy, 28 + 32 * x, 18 + 40 * y, 0, 0, MON_MALE);
                }
                else
                {
                    if(GetSpeciesDisplayDexFlag(species, FLAG_GET_CAUGHT))
                    {
                        // Animated
                        sPokedexMenu->pageSprites[i] = CreateMonIcon(sPokedexMenu->overviewPageSpecies[i], SpriteCB_MonIcon, 28 + 32 * x, 18 + 40 * y, 0, 0, MON_MALE);
                    }
                    else if(GetSpeciesDisplayDexFlag(species, FLAG_GET_SEEN))
                    {
                        // Non animated
                        sPokedexMenu->pageSprites[i] = CreateMonIcon(sPokedexMenu->overviewPageSpecies[i], SpriteCallbackDummy, 28 + 32 * x, 18 + 40 * y, 0, 0, MON_MALE);
                    }
                    else
                    {
                        // Place ? icon
                        //sPokedexMenu->pageSprites[i] = CreateMissingMonIcon(SpriteCallbackDummy, 28 + 32 * x, 18 + 40 * y, 0, 0);
                    }
                }
            }
        }
    }
}

static void Overview_DestroySprites()
{
    u8 i;

    for(i = 0; i < ARRAY_COUNT(sPokedexMenu->pageSprites); ++i)
    {
        if(sPokedexMenu->pageSprites[i] != SPRITE_NONE)
            FreeAndDestroyMonIconSprite(&gSprites[sPokedexMenu->pageSprites[i]]);

        sPokedexMenu->pageSprites[i] = SPRITE_NONE;
    }
}

static void Overview_SelectSpeciesToDiplay()
{
    u8 i;
    u16 num;
    u16 species;
    u8 dexVariant = RoguePokedex_GetDexVariant();
    u16 dexCount = GetVariantSpeciesCount(dexVariant);

    for(i = 0; i < OVERVIEW_ENTRY_COUNT; ++i)
    {
        num = i + sPokedexMenu->pageScrollAmount * COLUMN_ENTRY_COUNT;

        species = SPECIES_NONE;

        if(num < dexCount)
            species = GetDisplayedOverviewSpecies(GetVariantSpeciesAt(dexVariant, num));

        sPokedexMenu->overviewPageSpecies[i] = species;
        sPokedexMenu->overviewPageNumbers[i] = num + 1;
    }
}

static u8 Overview_GetLastValidActiveIndex()
{
    u8 i, j;

    for(i = 0; i < OVERVIEW_ENTRY_COUNT; ++i)
    {
        j = OVERVIEW_ENTRY_COUNT - i - 1;
        if(sPokedexMenu->overviewPageSpecies[j] != SPECIES_NONE)
            return j;
    }

    return 0;
}

static u8 Overview_GetMaxScrollAmount()
{
    u8 dexVariant = RoguePokedex_GetDexVariant();
    u16 dexCount = GetVariantSpeciesCount(dexVariant);

    return (dexCount / COLUMN_ENTRY_COUNT) - ROW_ENTRY_COUNT + 1;
}

// Mon info
//
u32 GetPokedexMonPersonality(u16 species);

void LoadMoveTypesSpritesheetAndPalette();
u8 CreateMonTypeIcon(u16 typeId, u8 x, u8 y);
void DestroyMonTypIcon(u8 spriteId);

static void MonInfo_CreateSprites(bool8 includeType)
{
    // display as shiny if we have seen it
    bool8 isShiny = GetSpeciesDisplayDexFlag(sPokedexMenu->viewBaseSpecies, FLAG_GET_CAUGHT_SHINY);

    LoadMoveTypesSpritesheetAndPalette(); // TODO - move

    sPokedexMenu->pageSprites[MON_SPRITE_FRONT_PIC] = CreateMonPicSprite_Affine(
        sPokedexMenu->viewBaseSpecies,
        NON_SHINY_PLACEHOLDER,
        GetPokedexMonPersonality(sPokedexMenu->viewBaseSpecies),
#ifdef ROGUE_EXPANSION
        GetGenderForSpecies(sPokedexMenu->viewBaseSpecies, 0),
#endif
        isShiny,
        MON_PIC_AFFINE_FRONT,
        48, 66, 
        0,
#ifdef ROGUE_EXPANSION
        sPokedexMenu->viewBaseSpecies
#else
        isShiny ? gMonShinyPaletteTable[sPokedexMenu->viewBaseSpecies].tag : gMonPaletteTable[sPokedexMenu->viewBaseSpecies].tag
#endif
    );

    sPokedexMenu->pageSprites[MON_SPRITE_ICON] = CreateMonIcon(sPokedexMenu->viewBaseSpecies, SpriteCallbackDummy, 48, 8, 0, 0, MON_MALE);

    if(includeType)
    {
        sPokedexMenu->pageSprites[MON_SPRITE_TYPE1] = CreateMonTypeIcon(RoguePokedex_GetSpeciesType(sPokedexMenu->viewBaseSpecies, 0), 138, 24);

        if(RoguePokedex_GetSpeciesType(sPokedexMenu->viewBaseSpecies, 0) != RoguePokedex_GetSpeciesType(sPokedexMenu->viewBaseSpecies, 1))
            sPokedexMenu->pageSprites[MON_SPRITE_TYPE2] = CreateMonTypeIcon(RoguePokedex_GetSpeciesType(sPokedexMenu->viewBaseSpecies, 1), 138 + 33, 24);
    }
}

static void MonInfo_DestroySprites()
{
    u8 i;
    u8 spriteId;

    for(i = 0; i < ARRAY_COUNT(sPokedexMenu->pageSprites); ++i)
    {
        spriteId = sPokedexMenu->pageSprites[i];

        if(spriteId != SPRITE_NONE)
        {
            switch (i)
            {
            case MON_SPRITE_FRONT_PIC:
            case MON_SPRITE_BACK_PIC:
                FreeAndDestroyMonPicSprite(spriteId);
                break;
            
            case MON_SPRITE_ICON:
            case MON_SPRITE_EVO_ICON1:
            case MON_SPRITE_EVO_ICON2:
            case MON_SPRITE_EVO_ICON3:
            case MON_SPRITE_EVO_ICON4:
                FreeAndDestroyMonIconSprite(&gSprites[spriteId]);
                break;

            case MON_SPRITE_TYPE1:
            case MON_SPRITE_TYPE2:
                DestroyMonTypIcon(spriteId);
                break;
                
            default:
                break;
            }
        }
    }
}

// Mon Stats
//

// Can lag out if too many
#define MAX_NEIGHBOUR_CHECKS 100

static u16 MonStats_GetMonNeighbour(u16 currViewSpecies, s8 offset)
{
    u16 i;
    u16 currViewIdx = (u16)-1;
    u8 dexVariant = RoguePokedex_GetDexVariant();
    u16 dexCount = GetVariantSpeciesCount(dexVariant);

    // Loop through party when using L/R from that menu
    if(sPokedexViewReq.view == DEX_VIEW_SPECIFIC_MON)
    {
        do
        {
            if(offset == 1)
                sPokedexMenu->partySlot = (sPokedexMenu->partySlot + 1) % gPlayerPartyCount;
            else // offset == -1
            {
                if(sPokedexMenu->partySlot == 0)
                    sPokedexMenu->partySlot = gPlayerPartyCount - 1;
                else
                    --sPokedexMenu->partySlot;
            }
        }
        while(GetMonData(&gPlayerParty[sPokedexMenu->partySlot], MON_DATA_SPECIES) == SPECIES_NONE);

        sPokedexMenu->viewBaseSpecies = SPECIES_NONE; // force it here so it always suceeds
        sPokedexMenu->viewOtId = GetMonData(&gPlayerParty[sPokedexMenu->partySlot], MON_DATA_OT_ID);

        return GetMonData(&gPlayerParty[sPokedexMenu->partySlot], MON_DATA_SPECIES);
    }
    else
    {

#ifdef ROGUE_EXPANSION
        currViewSpecies = GET_BASE_SPECIES_ID(currViewSpecies);
#endif

        for(i = 0; i < dexCount; ++i)
        {
            if(GetVariantSpeciesAt(dexVariant, i) == currViewSpecies)
            {
                currViewIdx = i;
                break;
            }
        }

        for(i = 0; i < MAX_NEIGHBOUR_CHECKS; ++i)
        {
            u16 checkIdx;
            u16 checkSpecies;

            checkIdx = currViewIdx;

            while(TRUE)
            {
                if(offset == 1)
                    checkIdx = (checkIdx + 1) % dexCount;
                else // offset == -1
                {
                    if(checkIdx == 0)
                        checkIdx = dexCount - 1;
                    else
                        --checkIdx;
                }

                // If we've done a full loop, we've failed to find next species we can view
                if(checkIdx == currViewIdx)
                    break;

                checkSpecies = GetVariantSpeciesAt(dexVariant, checkIdx);

                // Only allowed to jump to seen mons
                if(!GetSpeciesDisplayDexFlag(checkSpecies, FLAG_GET_SEEN))
                    continue;

#ifdef ROGUE_DEBUG
                // Debug behaviour for evo and forms page to make it easier to cycle through and debug
                //
                if(JOY_HELD(SELECT_BUTTON))
                {
                    if(sPokedexMenu->currentPage == PAGE_MON_EVOS)
                    {
                        if(Rogue_GetMaxEvolutionCount(checkSpecies) == 0)
                            continue;
                    }
#ifdef ROGUE_EXPANSION
                    else if(sPokedexMenu->currentPage == PAGE_MON_FORMS)
                    {
                        if(Rogue_GetActiveFormChangeCount(checkSpecies) == 0)
                            continue;
                    }
#endif
                }
#endif

                return checkSpecies;
            }
        }
    }

    // Failed fallback
    return currViewSpecies;
}

static bool8 IsMonPageUnlocked(u8 page)
{
    switch (page)
    {
    case PAGE_MON_RIDE_STATS:
        return CheckBagHasItem(ITEM_BASIC_RIDING_WHISTLE, 1);

    case PAGE_MON_FORMS:
#ifdef ROGUE_EXPANSION
        return TRUE;
#else
        return FALSE;
#endif
    }

    return TRUE;
}

static u8 NavigateNextMonPage(u8 startPage, u8 dir)
{
    u8 currentPage = startPage;
    while(TRUE)
    {
        if(dir == 1)
        {
            if(currentPage == PAGE_MON_LAST)
                currentPage = PAGE_MON_FIRST;
            else
                ++currentPage;
        }
        else // if(dir == -1)
        {
            if(currentPage == PAGE_MON_FIRST)
                currentPage = PAGE_MON_LAST;
            else
                --currentPage;
        }

        if(IsMonPageUnlocked(currentPage))
            return currentPage;

        if(currentPage == startPage)
            return currentPage;
    }

    return startPage;
}

static bool8 MonInfo_HandleInput(u8 taskId)
{
    u16 viewSpecies = sPokedexMenu->viewBaseSpecies;
    bool8 useInput = FALSE;

    if(JOY_REPEAT(L_BUTTON))
    {
        useInput = TRUE;
        viewSpecies = MonStats_GetMonNeighbour(sPokedexMenu->viewBaseSpecies, -1);

        if(viewSpecies == sPokedexMenu->viewBaseSpecies)
            PlaySE(SE_FAILURE);
    }
    else if(JOY_REPEAT(R_BUTTON))
    {
        useInput = TRUE;
        viewSpecies = MonStats_GetMonNeighbour(sPokedexMenu->viewBaseSpecies, 1);

        if(viewSpecies == sPokedexMenu->viewBaseSpecies)
            PlaySE(SE_FAILURE);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        useInput = TRUE;

        if(sPokedexViewReq.view == DEX_VIEW_SPECIFIC_MON)
        {
            // Immediately exit if viewing view party summary
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            gTasks[taskId].func = Task_PageFadeOutAndExit;

            PlaySE(SE_PC_OFF);
        }
        else
        {
            // Go back up to overview
            sPokedexMenu->desiredPage = PAGE_OVERVIEW;
            gTasks[taskId].func = Task_SwapToPage;
            PlaySE(SE_PIN);
        }
    }
    else if(JOY_REPEAT(DPAD_LEFT))
    {
        useInput = TRUE;

        sPokedexMenu->desiredPage = NavigateNextMonPage(sPokedexMenu->currentPage, -1);
        gTasks[taskId].func = Task_SwapToPage;
        PlaySE(SE_PIN);
    }
    else if(JOY_REPEAT(DPAD_RIGHT))
    {
        useInput = TRUE;

        sPokedexMenu->desiredPage = NavigateNextMonPage(sPokedexMenu->currentPage, 1);
        gTasks[taskId].func = Task_SwapToPage;
        PlaySE(SE_PIN);
    }

    if(viewSpecies != sPokedexMenu->viewBaseSpecies)
    {
        sPokedexMenu->viewBaseSpecies = viewSpecies;
        gTasks[taskId].func = Task_SwapToPage;
        PlaySE(SE_DEX_PAGE);
    }

    return useInput;
}

static void MonStats_HandleInput(u8 taskId)
{
    MonInfo_HandleInput(taskId);
}


static void MonMoves_HandleInput(u8 taskId)
{
    u16 maxScrollOffset;

    if(MonInfo_HandleInput(taskId))
        return;

    maxScrollOffset = GetMaxMoveScrollOffset();

    if((sPokedexMenu->listScrollAmount != 0 && JOY_HELD(DPAD_UP)) || JOY_NEW(DPAD_UP))
    {
        if(sPokedexMenu->listScrollAmount == 0)
            sPokedexMenu->listScrollAmount = GetMaxMoveScrollOffset();
        else
            sPokedexMenu->listScrollAmount -= 1;

        PlaySE(SE_DEX_SCROLL);
        DisplayMonMovesText();
    }
    else if((sPokedexMenu->listScrollAmount != maxScrollOffset && JOY_HELD(DPAD_DOWN)) || JOY_NEW(DPAD_DOWN))
    {
        if(sPokedexMenu->listScrollAmount == maxScrollOffset)
            sPokedexMenu->listScrollAmount = 0;
        else
            sPokedexMenu->listScrollAmount = min(maxScrollOffset, sPokedexMenu->listScrollAmount + 1);

        PlaySE(SE_DEX_SCROLL);
        DisplayMonMovesText();
    }
}

// Just needs to be large enough to cover all tutor move indices
#define MOVE_QUERY_OFFSET 300

static void MonEvos_OpenMoveQuery()
{
    u8 i;
    u16 moveId, itemId;
    u16 species = sPokedexMenu->viewBaseSpecies;

    // To help speed up viewing we're going to precalculate whether a special move is TM, TR or Tutor
    // (This isn't really a proper query, we're just reusing the bit field checking mostly)
    RogueCustomQuery_Begin();


    // Tutor/TM moves
    for (i = 0; TRUE; i++)
    {
        u16 tmIndex = i;
        u16 trIndex = i + MOVE_QUERY_OFFSET;

        moveId = gRoguePokemonProfiles[species].tutorMoves[i];

        if(moveId == MOVE_NONE)
            break;

        itemId = BattleMoveIdToItemId(moveId);

        if(itemId == ITEM_NONE)
        {
            RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, tmIndex);
            RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, trIndex);
        }
        else if(itemId >= ITEM_TM01 && itemId <= ITEM_HM08)
        {
            // Is TM move
            RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, tmIndex);
            RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, trIndex);
        }
        else
        {
            // Is TR move
            RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, tmIndex);
            RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, trIndex);
        }
    }

}

static void MonEvos_CloseMoveQuery()
{
    RogueCustomQuery_End();
}

static bool8 MonEvos_IsTutorMoveTM(u16 moveIdx)
{
    return RogueMiscQuery_CheckState(moveIdx);
}

static bool8 MonEvos_IsTutorMoveTR(u16 moveIdx)
{
    return RogueMiscQuery_CheckState(moveIdx + MOVE_QUERY_OFFSET);
}

static bool8 MonEvos_IsTutorMove(u16 moveIdx)
{
    return !(MonEvos_IsTutorMoveTM(moveIdx) || MonEvos_IsTutorMoveTR(moveIdx));
}

static void MonEvos_HandleInput(u8 taskId)
{
    if(MonInfo_HandleInput(taskId))
        return;

    if(JOY_REPEAT(DPAD_UP))
    {
        u16 maxScrollOffset = GetMaxEvoScrollOffset();

        if(maxScrollOffset == 0)
        {
            PlaySE(SE_FAILURE);
        }
        else
        {
            if(sPokedexMenu->listScrollAmount == 0)
                sPokedexMenu->listScrollAmount = maxScrollOffset;
            else
                --sPokedexMenu->listScrollAmount;

            PlaySE(SE_DEX_SCROLL);
            DisplayMonEvosText();
            MonEvos_CreateSprites();
        }
    }
    else if(JOY_REPEAT(DPAD_DOWN))
    {
        u16 maxScrollOffset = GetMaxEvoScrollOffset();

        if(maxScrollOffset == 0)
        {
            PlaySE(SE_FAILURE);
        }
        else
        {
            if(sPokedexMenu->listScrollAmount == maxScrollOffset)
                sPokedexMenu->listScrollAmount = 0;
            else
                ++sPokedexMenu->listScrollAmount;

            PlaySE(SE_DEX_SCROLL);
            DisplayMonEvosText();
            MonEvos_CreateSprites();
        }
    }
    else if(JOY_NEW(A_BUTTON))
    {
        u16 species = GetActiveEvoSpecies();

        if(species == SPECIES_NONE || !GetSpeciesDisplayDexFlag(species, FLAG_GET_SEEN))
        {
            PlaySE(SE_FAILURE);
        }
        else
        {
            sPokedexMenu->desiredPage = PAGE_MON_EVOS;
            sPokedexMenu->viewBaseSpecies = species;
            gTasks[taskId].func = Task_SwapToPage;

            PlaySE(SE_PIN);
        }
    }
}

static void MonEvos_CreateSprites()
{
    u8 i;
    u8 listIndex = 0;
    u8 displayCount = 0;
    struct Evolution evo;
    u8 evoCount = Rogue_GetMaxEvolutionCount(sPokedexMenu->viewBaseSpecies);

    // Destroy any previous sprites
    for(i = 0; i < 4; ++i)
    {
        if(sPokedexMenu->pageSprites[MON_SPRITE_EVO_ICON1 + i] != SPRITE_NONE)
        {
            FreeAndDestroyMonIconSprite(&gSprites[sPokedexMenu->pageSprites[MON_SPRITE_EVO_ICON1 + i]]);
            sPokedexMenu->pageSprites[MON_SPRITE_EVO_ICON1 + i] = SPRITE_NONE;
        }
    }

    for(i = 0; i < evoCount && displayCount < 4; ++i)
    {
        Rogue_ModifyEvolution(sPokedexMenu->viewBaseSpecies, i, &evo);
        Rogue_ModifyEvolution_ApplyCurses(sPokedexMenu->viewBaseSpecies, i, &evo);

        if(evo.targetSpecies == SPECIES_NONE)
            continue;

        if(listIndex >= sPokedexMenu->listScrollAmount)
        {
            if(GetSpeciesDisplayDexFlag(evo.targetSpecies, FLAG_GET_SEEN))
                sPokedexMenu->pageSprites[MON_SPRITE_EVO_ICON1 + displayCount] = CreateMonIcon(evo.targetSpecies, SpriteCallbackDummy, 98 + 16, 24 + 16 + 32 * displayCount, 0, 0, MON_MALE);
            else
                sPokedexMenu->pageSprites[MON_SPRITE_EVO_ICON1 + displayCount] = CreateMissingMonIcon(SpriteCallbackDummy, 98 + 16, 24 + 16 + 32 * displayCount, 0, 0);
            ++displayCount;
        }
        ++listIndex;
    }
}

static void MonForms_HandleInput(u8 taskId)
{
    if(MonInfo_HandleInput(taskId))
        return;

#ifdef ROGUE_EXPANSION
    if(JOY_REPEAT(DPAD_UP))
    {
        u16 maxScrollOffset = GetMaxFormScrollOffset();

        if(maxScrollOffset == 0)
        {
            PlaySE(SE_FAILURE);
        }
        else
        {
            if(sPokedexMenu->listScrollAmount == 0)
                sPokedexMenu->listScrollAmount = maxScrollOffset;
            else
                --sPokedexMenu->listScrollAmount;

            PlaySE(SE_DEX_SCROLL);
            DisplayMonFormsText();
            MonForms_CreateSprites();
        }
    }
    else if(JOY_REPEAT(DPAD_DOWN))
    {
        u16 maxScrollOffset = GetMaxFormScrollOffset();

        if(maxScrollOffset == 0)
        {
            PlaySE(SE_FAILURE);
        }
        else
        {
            if(sPokedexMenu->listScrollAmount == maxScrollOffset)
                sPokedexMenu->listScrollAmount = 0;
            else
                ++sPokedexMenu->listScrollAmount;

            PlaySE(SE_DEX_SCROLL);
            DisplayMonFormsText();
            MonForms_CreateSprites();
        }
    }
    else if(JOY_NEW(A_BUTTON))
    {
        u16 species = GetActiveFormSpecies();

        if(species == SPECIES_NONE || !GetSpeciesDisplayDexFlag(species, FLAG_GET_SEEN))
        {
            PlaySE(SE_FAILURE);
        }
        else
        {
            sPokedexMenu->desiredPage = PAGE_MON_FORMS;
            sPokedexMenu->viewBaseSpecies = species;
            gTasks[taskId].func = Task_SwapToPage;

            PlaySE(SE_PIN);
        }
    }
#endif
}

static void MonForms_CreateSprites()
{
#ifdef ROGUE_EXPANSION
    u8 i;
    u8 listIndex = 0;
    u8 displayCount = 0;
    u16 const* formTable = GetSpeciesFormTable(sPokedexMenu->viewBaseSpecies);

    // Destroy any previous sprites
    for(i = 0; i < 4; ++i)
    {
        if(sPokedexMenu->pageSprites[MON_SPRITE_EVO_ICON1 + i] != SPRITE_NONE)
        {
            FreeAndDestroyMonIconSprite(&gSprites[sPokedexMenu->pageSprites[MON_SPRITE_EVO_ICON1 + i]]);
            sPokedexMenu->pageSprites[MON_SPRITE_EVO_ICON1 + i] = SPRITE_NONE;
        }
    }

    for(i = 0; formTable && formTable[i] != FORM_SPECIES_END && displayCount < 4; ++i)
    {
        if(IsAltFormVisible(sPokedexMenu->viewBaseSpecies, formTable[i]))
        {
            if(listIndex >= sPokedexMenu->listScrollAmount)
            {
                if(GetSpeciesDisplayDexFlag(formTable[i], FLAG_GET_SEEN))
                    sPokedexMenu->pageSprites[MON_SPRITE_EVO_ICON1 + displayCount] = CreateMonIcon(formTable[i], SpriteCallbackDummy, 98 + 16, 24 + 16 + 32 * displayCount, 0, 0, MON_MALE);
                else
                    sPokedexMenu->pageSprites[MON_SPRITE_EVO_ICON1 + displayCount] = CreateMissingMonIcon(SpriteCallbackDummy, 98 + 16, 24 + 16 + 32 * displayCount, 0, 0);

                ++displayCount;
            }
            ++listIndex;
        }
    }
#endif
}

static void MonRideStats_HandleInput(u8 taskId)
{
    MonInfo_HandleInput(taskId);
}

u8 RoguePokedex_GetDexRegion()
{
    u8 i, j;
    u8 dexVariant = RoguePokedex_GetDexVariant();

    for(i = POKEDEX_REGION_START; i <= POKEDEX_REGION_END; ++i)
    {
        for(j = 0; j < gPokedexRegions[i].variantCount; ++j)
        {
            if(gPokedexRegions[i].variantList[j] == dexVariant)
                return i;
        }
    }

    return 0;
}

void RoguePokedex_SetDexRegion(u8 region)
{
    if(region < POKEDEX_REGION_COUNT)
        RoguePokedex_SetDexVariant(gPokedexRegions[region].variantList[gPokedexRegions[region].variantCount - 1]);
    else
        RoguePokedex_SetDexVariant(POKEDEX_REGION_START);
}

u8 RoguePokedex_GetDexVariant()
{
    u8 dexVariant = Rogue_GetConfigRange(CONFIG_RANGE_POKEDEX_VARIANT);

    if(dexVariant <= POKEDEX_DYNAMIC_VARIANT_END)
        return dexVariant;

    return POKEDEX_VARIANT_DEFAULT;
}

void RoguePokedex_SetDexVariant(u8 variant)
{
    if(variant <= POKEDEX_DYNAMIC_VARIANT_END)
    {
        Rogue_SetConfigRange(CONFIG_RANGE_POKEDEX_VARIANT, variant);
    }
    else
    {
        // Likely wanting to enter national dex mode
        Rogue_SetConfigRange(CONFIG_RANGE_POKEDEX_VARIANT, POKEDEX_VARIANT_DEFAULT);
    }
}

u8 RoguePokedex_GetDexGenLimit()
{
    u8 variant = RoguePokedex_GetDexVariant();
    u8 genLimit = GetVariantGenLimit(variant);

    if(genLimit != 0 && genLimit <= DEX_GEN_LIMIT)
        return genLimit;

    return DEX_GEN_LIMIT;
}

u16 RoguePokedex_GetCurrentDexLimit()
{
    u8 dexVariant = RoguePokedex_GetDexVariant();
    u16 dexCount = GetVariantSpeciesCount(dexVariant);

    return dexCount;
}

bool8 RoguePokedex_IsVariantEditUnlocked()
{
    return FlagGet(FLAG_ROGUE_MET_POKABBIE);
}

bool8 RoguePokedex_IsVariantEditEnabled()
{
    return RoguePokedex_IsVariantEditUnlocked() && Rogue_CanEditConfig();
}

u8 SpeciesToGen(u16 species);

bool8 RoguePokedex_IsSpeciesEnabled(u16 species)
{
    u8 genLimit = RoguePokedex_GetDexGenLimit();
    u8 speciesGen = SpeciesToGen(species);

    if(species == SPECIES_NONE || speciesGen > genLimit)
        return FALSE;
    
#ifdef ROGUE_EXPANSION
    species = GET_BASE_SPECIES_ID(species);
#endif

    {
        u8 variant = RoguePokedex_GetDexVariant();

        // The species or the base species is allowed to use this
        return CheckVariantContainsSpecies(variant, species) || CheckVariantContainsSpecies(variant, Rogue_GetEggSpecies(species));
    }

    return TRUE;
}

bool8 RoguePokedex_IsBaseSpeciesEnabled(u16 species)
{
    if(species == SPECIES_NONE)
        return FALSE;

    {
        u8 variant = RoguePokedex_GetDexVariant();

        // The species or the base species is allowed to use this
        return CheckVariantContainsSpecies(variant, species);
    }

    return TRUE;
}

u16 RoguePokedex_GetSpeciesCurrentNum(u16 species)
{
    if(!RoguePokedex_IsSpeciesEnabled(species))
        return 0;

#ifdef ROGUE_EXPANSION
    species = GET_BASE_SPECIES_ID(species);
#endif

    {
        u16 i;
        u8 variant = RoguePokedex_GetDexVariant();
        u16 dexCount = GetVariantSpeciesCount(variant);

        for(i = 0; i < dexCount; ++i)
        {
            if(GetVariantSpeciesAt(variant, i) == species)
                return i + 1;
        }
    }

    return 0;
}

u16 RoguePokedex_RedirectSpeciesGetSetFlag(u16 species)
{
#ifdef ROGUE_EXPANSION
    //if(species >= SPECIES_VENUSAUR_MEGA && species <= SPECIES_GROUDON_PRIMAL)
    //    return GET_BASE_SPECIES_ID(species);

    if(species >= SPECIES_PIKACHU_COSPLAY && species <= SPECIES_PIKACHU_WORLD_CAP)
        return SPECIES_PIKACHU;

    if(species >= SPECIES_UNOWN_B && species <= SPECIES_UNOWN_QMARK)
        return SPECIES_UNOWN;

    if(species >= SPECIES_ARCEUS_FIGHTING && species <= SPECIES_ARCEUS_FAIRY)
        return SPECIES_ARCEUS;

    if(species >= SPECIES_VIVILLON_POLAR && species <= SPECIES_VIVILLON_POKE_BALL)
        return SPECIES_ARCEUS;

    if(species >= SPECIES_FLABEBE_YELLOW_FLOWER && species <= SPECIES_FLORGES_WHITE_FLOWER)
        return GET_BASE_SPECIES_ID(species);

    if(species >= SPECIES_FURFROU_HEART_TRIM && species <= SPECIES_FURFROU_PHARAOH_TRIM)
        return GET_BASE_SPECIES_ID(species);

    if(species >= SPECIES_PUMPKABOO_SMALL && species <= SPECIES_XERNEAS_ACTIVE)
        return GET_BASE_SPECIES_ID(species);

    if(species >= SPECIES_SILVALLY_FIGHTING && species <= SPECIES_SILVALLY_FAIRY)
        return SPECIES_SILVALLY;

    if(species >= SPECIES_MINIOR_ORANGE && species <= SPECIES_MINIOR_CORE_VIOLET)
        return SPECIES_MINIOR;

    if(species >= SPECIES_CRAMORANT_GULPING && species <= SPECIES_MORPEKO_HANGRY)
        return GET_BASE_SPECIES_ID(species);

    if(species >= SPECIES_ALCREMIE_BERRY && species <= SPECIES_ALCREMIE_RIBBON_RAINBOW_SWIRL)
        return GET_BASE_SPECIES_ID(species);

    if(species >= SPECIES_SQUAWKABILLY_GREEN_PLUMAGE && species <= SPECIES_SQUAWKABILLY_WHITE_PLUMAGE)
        return SPECIES_SQUAWKABILLY;

    if(species >= SPECIES_TATSUGIRI_CURLY && species <= SPECIES_TATSUGIRI_STRETCHY)
        return SPECIES_TATSUGIRI;

    if(species >= SPECIES_OGERPON_TEAL_MASK && species <= SPECIES_OGERPON_CORNERSTONE_MASK_TERA)
        return SPECIES_OGERPON;

    switch (species)
    {
    case SPECIES_PICHU_SPIKY_EARED:
    case SPECIES_GRENINJA_BATTLE_BOND:
    case SPECIES_MEOWSTIC_FEMALE:
    case SPECIES_AEGISLASH_BLADE:
    case SPECIES_MIMIKYU_BUSTED:
    case SPECIES_BASCULEGION_FEMALE:
    case SPECIES_OINKOLOGNE_FEMALE:
    case SPECIES_MAUSHOLD_FAMILY_OF_FOUR:
    case SPECIES_PALAFIN_HERO:
    case SPECIES_DUDUNSPARCE_THREE_SEGMENT:
    case SPECIES_POLTCHAGEIST_ARTISAN:
    case SPECIES_SINISTCHA_MASTERPIECE:
    case SPECIES_TERAPAGOS_STELLAR:
    case SPECIES_TERAPAGOS_TERASTAL:
    case SPECIES_WOBBUFFET_PUNCHING:
    case SPECIES_PIKIN_MEGA:
        return GET_BASE_SPECIES_ID(species);
    }
#endif

    return species;
}

bool8 RoguePokedex_IsSpeciesLegendary(u16 species)
{
#ifdef ROGUE_EXPANSION
    species = GET_BASE_SPECIES_ID(species);
#endif

    switch(species)
    {
        case SPECIES_ARTICUNO:
        case SPECIES_ZAPDOS:
        case SPECIES_MOLTRES:
        case SPECIES_MEWTWO:
        case SPECIES_MEW:

        case SPECIES_RAIKOU:
        case SPECIES_ENTEI:
        case SPECIES_SUICUNE:
        case SPECIES_LUGIA:
        case SPECIES_HO_OH:
        case SPECIES_CELEBI:

        case SPECIES_REGIROCK:
        case SPECIES_REGICE:
        case SPECIES_REGISTEEL:
        case SPECIES_KYOGRE:
        case SPECIES_GROUDON:
        case SPECIES_RAYQUAZA:
        case SPECIES_LATIAS:
        case SPECIES_LATIOS:
        case SPECIES_JIRACHI:
        case SPECIES_DEOXYS:
#ifdef ROGUE_EXPANSION

        case SPECIES_UXIE:
        case SPECIES_MESPRIT:
        case SPECIES_AZELF:
        case SPECIES_DIALGA:
        case SPECIES_PALKIA:
        case SPECIES_HEATRAN:
        case SPECIES_REGIGIGAS:
        case SPECIES_GIRATINA:
        case SPECIES_CRESSELIA:
        case SPECIES_PHIONE:
        case SPECIES_MANAPHY:
        case SPECIES_DARKRAI:
        case SPECIES_SHAYMIN:
        case SPECIES_ARCEUS:

        case SPECIES_VICTINI:
        case SPECIES_COBALION:
        case SPECIES_TERRAKION:
        case SPECIES_VIRIZION:
        case SPECIES_TORNADUS:
        case SPECIES_THUNDURUS:
        case SPECIES_RESHIRAM:
        case SPECIES_ZEKROM:
        case SPECIES_LANDORUS:
        case SPECIES_KYUREM:
        case SPECIES_KELDEO:
        case SPECIES_MELOETTA:
        case SPECIES_GENESECT:

        case SPECIES_XERNEAS:
        case SPECIES_YVELTAL:
        case SPECIES_ZYGARDE:
        case SPECIES_DIANCIE:
        case SPECIES_HOOPA:
        case SPECIES_VOLCANION:
        
        case SPECIES_TYPE_NULL:
        case SPECIES_SILVALLY:
        case SPECIES_TAPU_KOKO:
        case SPECIES_TAPU_LELE:
        case SPECIES_TAPU_BULU:
        case SPECIES_TAPU_FINI:
        case SPECIES_COSMOG:
        case SPECIES_COSMOEM:
        case SPECIES_SOLGALEO:
        case SPECIES_LUNALA:
        case SPECIES_NIHILEGO:
        case SPECIES_BUZZWOLE:
        case SPECIES_PHEROMOSA:
        case SPECIES_XURKITREE:
        case SPECIES_CELESTEELA:
        case SPECIES_KARTANA:
        case SPECIES_GUZZLORD:
        case SPECIES_NECROZMA:
        case SPECIES_MAGEARNA:
        case SPECIES_MARSHADOW:
        case SPECIES_POIPOLE:
        case SPECIES_NAGANADEL:
        case SPECIES_STAKATAKA:
        case SPECIES_BLACEPHALON:
        case SPECIES_ZERAORA:
        case SPECIES_MELTAN:
        case SPECIES_MELMETAL:

        case SPECIES_ZACIAN:
        case SPECIES_ZAMAZENTA:
        case SPECIES_ETERNATUS:
        case SPECIES_KUBFU:
        case SPECIES_URSHIFU:
        case SPECIES_ZARUDE:
        case SPECIES_REGIELEKI:
        case SPECIES_REGIDRAGO:
        case SPECIES_GLASTRIER:
        case SPECIES_SPECTRIER:
        case SPECIES_CALYREX:

        case SPECIES_ENAMORUS:

        case SPECIES_WO_CHIEN:
        case SPECIES_CHIEN_PAO:
        case SPECIES_TING_LU:
        case SPECIES_CHI_YU:
        case SPECIES_KORAIDON:
        case SPECIES_MIRAIDON:
        case SPECIES_WALKING_WAKE:
        case SPECIES_IRON_LEAVES:
        case SPECIES_OKIDOGI:
        case SPECIES_MUNKIDORI:
        case SPECIES_FEZANDIPITI:
        case SPECIES_OGERPON:

        case SPECIES_GOUGING_FIRE:
        case SPECIES_RAGING_BOLT:
        case SPECIES_IRON_BOULDER:
        case SPECIES_IRON_CROWN:
        case SPECIES_TERAPAGOS:
        case SPECIES_PECHARUNT:

        // Forms
        case SPECIES_KYUREM_WHITE:
        case SPECIES_KYUREM_BLACK:
        
        case SPECIES_NECROZMA_DUSK_MANE:
        case SPECIES_NECROZMA_DAWN_WINGS:
        case SPECIES_NECROZMA_ULTRA:

        case SPECIES_ZACIAN_CROWNED_SWORD:
        case SPECIES_ZAMAZENTA_CROWNED_SHIELD:
        case SPECIES_ETERNATUS_ETERNAMAX:
        case SPECIES_URSHIFU_RAPID_STRIKE_STYLE:
        case SPECIES_ZARUDE_DADA:
        case SPECIES_CALYREX_ICE_RIDER:
        case SPECIES_CALYREX_SHADOW_RIDER:

        case SPECIES_ARTICUNO_GALARIAN:
        case SPECIES_ZAPDOS_GALARIAN:
        case SPECIES_MOLTRES_GALARIAN:

        case SPECIES_TERAPAGOS_TERASTAL:
        case SPECIES_TERAPAGOS_STELLAR:
#endif
            return TRUE;
    };

    return FALSE;
}

bool8 RoguePokedex_IsSpeciesValidBoxLegendary(u16 species)
{
#ifdef ROGUE_EXPANSION
    species = GET_BASE_SPECIES_ID(species);
#endif

    switch(species)
    {
        case SPECIES_MEWTWO:
        case SPECIES_MEW:

        case SPECIES_LUGIA:
        case SPECIES_HO_OH:
        case SPECIES_CELEBI:

        case SPECIES_KYOGRE:
        case SPECIES_GROUDON:
        case SPECIES_RAYQUAZA:
#ifdef ROGUE_EXPANSION

        case SPECIES_DIALGA:
        case SPECIES_PALKIA:
        case SPECIES_REGIGIGAS:
        case SPECIES_GIRATINA:
        case SPECIES_ARCEUS:

        case SPECIES_RESHIRAM:
        case SPECIES_ZEKROM:
        case SPECIES_KYUREM:

        case SPECIES_XERNEAS:
        case SPECIES_YVELTAL:
        case SPECIES_ZYGARDE:

        case SPECIES_COSMOEM:
        case SPECIES_SOLGALEO:
        case SPECIES_LUNALA:
        case SPECIES_NECROZMA:

        case SPECIES_ZACIAN:
        case SPECIES_ZAMAZENTA:
        case SPECIES_ETERNATUS:
        case SPECIES_CALYREX:
        
        case SPECIES_KORAIDON:
        case SPECIES_MIRAIDON:
        case SPECIES_OGERPON:

        case SPECIES_TERAPAGOS:
        case SPECIES_PECHARUNT:

        // Forms
        case SPECIES_KYUREM_WHITE:
        case SPECIES_KYUREM_BLACK:
        
        case SPECIES_NECROZMA_DUSK_MANE:
        case SPECIES_NECROZMA_DAWN_WINGS:
        case SPECIES_NECROZMA_ULTRA:

        case SPECIES_ZACIAN_CROWNED_SWORD:
        case SPECIES_ZAMAZENTA_CROWNED_SHIELD:
        case SPECIES_CALYREX_ICE_RIDER:
        case SPECIES_CALYREX_SHADOW_RIDER:
#endif
            return TRUE;
    };

    return FALSE;
}

bool8 RoguePokedex_IsSpeciesValidRoamerLegendary(u16 species)
{
#ifdef ROGUE_EXPANSION
    species = GET_BASE_SPECIES_ID(species);
#endif

    switch(species)
    {
        case SPECIES_ARTICUNO:
        case SPECIES_ZAPDOS:
        case SPECIES_MOLTRES:
        case SPECIES_MEW:

        case SPECIES_RAIKOU:
        case SPECIES_ENTEI:
        case SPECIES_SUICUNE:
        case SPECIES_CELEBI:

        case SPECIES_LATIAS:
        case SPECIES_LATIOS:
        case SPECIES_JIRACHI:
#ifdef ROGUE_EXPANSION

        case SPECIES_UXIE:
        case SPECIES_MESPRIT:
        case SPECIES_AZELF:
        case SPECIES_CRESSELIA:
        case SPECIES_PHIONE:
        case SPECIES_MANAPHY:
        case SPECIES_DARKRAI:
        case SPECIES_SHAYMIN:

        case SPECIES_VICTINI:
        case SPECIES_COBALION:
        case SPECIES_TERRAKION:
        case SPECIES_VIRIZION:
        case SPECIES_TORNADUS:
        case SPECIES_THUNDURUS:
        case SPECIES_LANDORUS:

        case SPECIES_DIANCIE:
        case SPECIES_HOOPA:
        
        case SPECIES_COSMOG:
        case SPECIES_NIHILEGO:
        case SPECIES_BUZZWOLE:
        case SPECIES_PHEROMOSA:
        case SPECIES_XURKITREE:
        case SPECIES_CELESTEELA:
        case SPECIES_KARTANA:
        case SPECIES_GUZZLORD:
        case SPECIES_NECROZMA:
        case SPECIES_MAGEARNA:
        case SPECIES_MARSHADOW:
        case SPECIES_POIPOLE:
        case SPECIES_NAGANADEL:
        case SPECIES_STAKATAKA:
        case SPECIES_BLACEPHALON:
        case SPECIES_ZERAORA:

        case SPECIES_KUBFU:
        case SPECIES_URSHIFU:
        case SPECIES_ZARUDE:

        case SPECIES_ENAMORUS:

        case SPECIES_WO_CHIEN:
        case SPECIES_CHIEN_PAO:
        case SPECIES_TING_LU:
        case SPECIES_CHI_YU:

        case SPECIES_WALKING_WAKE:
        case SPECIES_IRON_LEAVES:
        case SPECIES_GOUGING_FIRE:
        case SPECIES_RAGING_BOLT:
        case SPECIES_IRON_BOULDER:
        case SPECIES_IRON_CROWN:
        
        case SPECIES_OKIDOGI:
        case SPECIES_MUNKIDORI:
        case SPECIES_FEZANDIPITI:

        // Forms
        case SPECIES_URSHIFU_RAPID_STRIKE_STYLE:
        case SPECIES_ZARUDE_DADA:

        case SPECIES_ARTICUNO_GALARIAN:
        case SPECIES_ZAPDOS_GALARIAN:
        case SPECIES_MOLTRES_GALARIAN:
#endif
            return TRUE;
    };

    return FALSE;
}

bool8 RoguePokedex_IsSpeciesParadox(u16 species)
{
#ifdef ROGUE_EXPANSION
    if(species >= SPECIES_GREAT_TUSK && species <= SPECIES_IRON_THORNS)
        return TRUE;

    switch (species)
    {
    case SPECIES_ROARING_MOON:
    case SPECIES_IRON_VALIANT:
    case SPECIES_WALKING_WAKE:
    case SPECIES_IRON_LEAVES:
    case SPECIES_GOUGING_FIRE:
    case SPECIES_RAGING_BOLT:
    case SPECIES_IRON_BOULDER:
    case SPECIES_IRON_CROWN:
        return TRUE;
    }
#endif

    return FALSE;
}

u8 const* RoguePokedex_GetSpeciesName(u16 species)
{
#ifdef ROGUE_EXPANSION
    return gSpeciesInfo[species].speciesName;
#else
    return gSpeciesNames[species];
#endif
}

u8 RoguePokedex_GetSpeciesType(u16 species, u8 typeIndex)
{
#ifdef ROGUE_EXPANSION
    AGB_ASSERT(typeIndex < ARRAY_COUNT(gSpeciesInfo[species].types));
    return gSpeciesInfo[species].types[typeIndex];
#define gRogueSpeciesInfo  gSpeciesInfo
#else
    AGB_ASSERT(typeIndex < 2);

    if(typeIndex == 0)
        return gBaseStats[species].type1;
    else
        return gBaseStats[species].type2;
#endif
}

u16 RoguePokedex_GetSpeciesBST(u16 species)
{
    u16 statTotal =
        gRogueSpeciesInfo[species].baseHP +
        gRogueSpeciesInfo[species].baseAttack +
        gRogueSpeciesInfo[species].baseDefense +
        gRogueSpeciesInfo[species].baseSpAttack +
        gRogueSpeciesInfo[species].baseSpDefense +
        gRogueSpeciesInfo[species].baseSpeed;
    return statTotal;
}

static void GatherSpeciesStatsArray(u16 species, u8* stats)
{
    stats[STAT_HP] = gRogueSpeciesInfo[species].baseHP;
    stats[STAT_ATK] = gRogueSpeciesInfo[species].baseAttack;
    stats[STAT_DEF] = gRogueSpeciesInfo[species].baseDefense;
    stats[STAT_SPATK] = gRogueSpeciesInfo[species].baseSpAttack;
    stats[STAT_SPDEF] = gRogueSpeciesInfo[species].baseSpDefense;
    stats[STAT_SPEED] = gRogueSpeciesInfo[species].baseSpeed;
}

static u8 SelectBestWorstStat(u16 species, bool8 selectLargest)
{
    u8 i;
    u8 stats[NUM_STATS];
    u8 statId = 0;
    u8 statScore = 0;

    GatherSpeciesStatsArray(species, stats);

    for(i = 0; i < NUM_STATS; ++i)
    {
        if(selectLargest)
        {
            if(i == 0 || stats[i] > statScore)
            {
                statId = i;
                statScore = stats[i];
            }
        }
        else
        {
            if(i == 0 || stats[i] < statScore)
            {
                statId = i;
                statScore = stats[i];
            }
        }
    }

    return statId;
}

u8 RoguePokedex_GetSpeciesBestStat(u16 species)
{
    return SelectBestWorstStat(species, TRUE);
}

u8 RoguePokedex_GetSpeciesWorstStat(u16 species)
{
    return SelectBestWorstStat(species, FALSE);
}

void RoguePokedex_GetSpeciesStatArray(u16 species, u8* stats, u8 bufferSize)
{
    AGB_ASSERT(bufferSize >= NUM_STATS);
    GatherSpeciesStatsArray(species, stats);
}

extern const u16 gRogueBake_EggSpecies[];
extern const u16 gRogueBake_FinalEvoSpecies[];

static u16 GetVariantSpeciesAt(u8 variant, u16 index)
{
    if(variant <= POKEDEX_VARIANT_END)
        return gPokedexVariants[variant].speciesList[index];
    else
    {
        AGB_ASSERT(variant <= POKEDEX_DYNAMIC_VARIANT_END);

        switch (variant)
        {
            case POKEDEX_DYNAMIC_VARIANT_NORMAL_SAFARI:
            case POKEDEX_DYNAMIC_VARIANT_LEGEND_SAFARI:
            {
                u16 i = (variant == POKEDEX_DYNAMIC_VARIANT_LEGEND_SAFARI) ? ROGUE_SAFARI_LEGENDS_START_INDEX : 0;
                u16 total = (variant == POKEDEX_DYNAMIC_VARIANT_LEGEND_SAFARI) ? ROGUE_SAFARI_TOTAL_MONS : ROGUE_SAFARI_LEGENDS_START_INDEX;
                u16 count = 0;

                for(; i < total; ++i)
                {
                    if(gRogueSaveBlock->safariMons[i].species != SPECIES_NONE)
                    {
                        if(index == count++)
                            return Rogue_GetEggSpecies(gRogueSaveBlock->safariMons[i].species);
                    }
                }

                return SPECIES_NONE;
            }

            case POKEDEX_DYNAMIC_VARIANT_EGG_SPECIES:
                return gRogueBake_EggSpecies[index];
                break;

            case POKEDEX_DYNAMIC_VARIANT_FINAL_SPECIES:
                return gRogueBake_FinalEvoSpecies[index];
                break;

            default:
                AGB_ASSERT(FALSE);
                break;
        }

        return SPECIES_BULBASAUR;
    }
}

static u16 GetVariantSpeciesCount(u8 variant)
{
    if(variant <= POKEDEX_VARIANT_END)
        return gPokedexVariants[variant].speciesCount;
    else
    {
        AGB_ASSERT(variant <= POKEDEX_DYNAMIC_VARIANT_END);

        switch (variant)
        {
            case POKEDEX_DYNAMIC_VARIANT_NORMAL_SAFARI:
            case POKEDEX_DYNAMIC_VARIANT_LEGEND_SAFARI:
            {
                u16 i = (variant == POKEDEX_DYNAMIC_VARIANT_LEGEND_SAFARI) ? ROGUE_SAFARI_LEGENDS_START_INDEX : 0;
                u16 total = (variant == POKEDEX_DYNAMIC_VARIANT_LEGEND_SAFARI) ? ROGUE_SAFARI_TOTAL_MONS : ROGUE_SAFARI_LEGENDS_START_INDEX;
                u16 count = 0;

                for(; i < total; ++i)
                {
                    if(gRogueSaveBlock->safariMons[i].species != SPECIES_NONE)
                        ++count;
                }

                return count;
            }

            case POKEDEX_DYNAMIC_VARIANT_EGG_SPECIES:
                return SPECIES_EGG_EVO_STAGE_COUNT;
                break;

            case POKEDEX_DYNAMIC_VARIANT_FINAL_SPECIES:
                return SPECIES_FINAL_EVO_STAGE_COUNT;
                break;

            default:
                AGB_ASSERT(FALSE);
                break;
        }

        return 0;
    }
}

static u8 GetVariantGenLimit(u8 variant)
{
    if(variant <= POKEDEX_VARIANT_END)
        return gPokedexVariants[variant].genLimit;
    else
    {
        AGB_ASSERT(variant <= POKEDEX_DYNAMIC_VARIANT_END);
        return POKEDEX_MAX_GEN;
    }
}

static bool8 CheckVariantContainsSpecies(u8 variant, u16 species)
{
    bool8 result;

    if(variant <= POKEDEX_VARIANT_END && Rogue_CheckPokedexVariantFlag(variant, species, &result))
    {
        return result;
    }
    else
    {
        u16 i;
        u16 dexCount = GetVariantSpeciesCount(variant);

        for(i = 0; i < dexCount; ++i)
        {
            if(GetVariantSpeciesAt(variant, i) == species)
                return TRUE;
        }
    }

    return FALSE;
}