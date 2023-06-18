#include "global.h"
#include "battle_main.h"
#include "palette.h"
#include "main.h"
#include "data.h"
#include "event_data.h"
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
#include "overworld.h"
#include "menu.h"
#include "sound.h"
#include "trainer_pokemon_sprites.h"
#include "pokedex.h"
#include "pokemon_icon.h"

#include "constants/abilities.h"
#include "constants/rgb.h"
#include "constants/songs.h"

#include "rogue_pokedex.h"

#ifdef ROGUE_EXPANSION
#define DEX_GEN_LIMIT 8
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
    PAGE_MON_FORMS, // ? could just press A on stats screen to cycle forms
};

enum
{
    MON_SPRITE_FRONT_PIC,
    MON_SPRITE_BACK_PIC,
    MON_SPRITE_ICON,
    MON_SPRITE_TYPE1,
    MON_SPRITE_TYPE2,
    MON_SPRITE_COUNT,
};

#define MAX_SPRITE_COUNT max(OVERVIEW_ENTRY_COUNT, MON_SPRITE_COUNT)


enum
{
    WIN_MON_SPECIES,
    WIN_MON_LIST,
    WIN_TITLE_COUNTERS,
    WIN_TITLE_VARIANT_SELECT,
    WIN_COUNT,
};

static const struct WindowTemplate sMonEntryWinTemplates[WIN_COUNT + 1] =
{
    [WIN_MON_SPECIES] = 
    {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 14,
        .width = 7,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 1,
    },
    [WIN_MON_LIST] = 
    {
        .bg = 0,
        .tilemapLeft = 12,
        .tilemapTop = 3,
        .width = 17,
        .height = 16,
        .paletteNum = 15,
        .baseBlock = 29,
    },
    [WIN_TITLE_COUNTERS] = 
    {
        .bg = 0,
        .tilemapLeft = 3,
        .tilemapTop = 7,
        .width = 4,
        .height = 8,
        .paletteNum = 15,
        .baseBlock = 301,
    },
    [WIN_TITLE_VARIANT_SELECT] = 
    {
        .bg = 0,
        .tilemapLeft = 8,
        .tilemapTop = 7,
        .width = 15,
        .height = 5,
        .paletteNum = 15,
        .baseBlock = 333,
    },
    [WIN_COUNT] = DUMMY_WIN_TEMPLATE,
};

extern const u8 gText_DexNational[];
extern const u8 gText_DexHoenn[];
extern const u8 gText_PokedexDiploma[];

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
static void InitOverviewBg(void);
static void InitMonEntryWindows(void);
static void DestroyMonEntryWindows(void);
static void PrintDiplomaText(u8 *, u8, u8);
static void InitPageResources(u8 page);
static void DestroyPageResources(u8 page);

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

// Mon stats
static void MonStats_CreateSprites();
static void MonStats_DestroySprites();
static void MonStats_HandleInput(u8);

struct PokedexMenu
{
    u8 currentPage;
    u8 desiredPage;
    u8 pageSprites[MAX_SPRITE_COUNT];

    // Title screen
    bool8 titleScreenInEditMode;
    bool8 titleScreenCursorIdx;

    // Overview
    u16 selectedIdx;
    u16 scrollAmount;
    u16 overviewPageSpecies[OVERVIEW_ENTRY_COUNT];
    u16 overviewPageNumbers[OVERVIEW_ENTRY_COUNT];

    // Mon screen
    u16 lastCrySpecies;
    u16 viewBaseSpecies;
};

EWRAM_DATA static u8 *sTilemapBufferPtr = NULL;
EWRAM_DATA static struct PokedexMenu* sPokedexMenu = NULL;

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

static const u32 sMonStatsTilemap[] = INCBIN_U32("graphics/rogue_pokedex/mon_stats.bin.lz");
static const u32 sMonStatsTiles[] = INCBIN_U32("graphics/rogue_pokedex/mon_stats.4bpp.lz");

void CB2_Rogue_ShowPokedex(void)
{
    u8 i;
    sPokedexMenu = AllocZeroed(sizeof(struct PokedexMenu));
    sPokedexMenu->currentPage = PAGE_NONE;
    sPokedexMenu->desiredPage = PAGE_TITLE_SCREEN;

    sPokedexMenu->lastCrySpecies = SPECIES_NONE;

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
    sTilemapBufferPtr = malloc(BG_SCREEN_SIZE);

    InitOverviewBg();
    ResetTempTileDataBuffers();
    
    //DecompressAndCopyTileDataToVram(1, &sOverviewTiles, 0, 0, 0);
    //while (FreeTempTileDataBuffersIfPossible())
    //    ;
    //LZDecompressWram(sOverviewTilemap, sTilemapBufferPtr);
    //CopyBgTilemapBufferToVram(1);

    sPokedexMenu->currentPage = sPokedexMenu->desiredPage;
    InitPageResources(sPokedexMenu->currentPage);

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

static void InitPageResources(u8 page)
{
    InitOverviewBg();
    ResetTempTileDataBuffers();

    switch (page)
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
            DecompressAndCopyTileDataToVram(1, &sOverviewTiles, 0, 0, 0);
            while (FreeTempTileDataBuffersIfPossible())
                ;
            LZDecompressWram(sOverviewTilemap, sTilemapBufferPtr);
            CopyBgTilemapBufferToVram(1);

            LoadMonIconPalettes();
            //BlendPalettes(PALETTES_ALL, 16, RGB_BLACK); // Ensure the mon icon palettes are faded

            Overview_SelectSpeciesToDiplay();
            Overview_RefillBg();
            Overview_CreateSprites();
        }
        break;

    case PAGE_MON_STATS:
        {
            DecompressAndCopyTileDataToVram(1, &sMonStatsTiles, 0, 0, 0);
            while (FreeTempTileDataBuffersIfPossible())
                ;
            LZDecompressWram(sMonStatsTilemap, sTilemapBufferPtr);
            CopyBgTilemapBufferToVram(1);

            InitMonEntryWindows();
            // Text printed below

            LoadMonIconPalettes();

            MonStats_CreateSprites();
        }
        break;
    
    default:
        break;
    }
}

static void DestroyPageResources(u8 page)
{
    u8 i;
    
    switch (page)
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
        {
            MonStats_DestroySprites();
            FreeMonIconPalettes();

            DestroyMonEntryWindows();
        }
        break;
    
    default:
        break;
    }

    ResetSpriteData();
    FreeAllSpritePalettes();
}

#define tDoFade data[0]

static void Task_SetupPage(u8 taskId)
{
    DestroyPageResources(sPokedexMenu->currentPage);

    sPokedexMenu->currentPage = sPokedexMenu->desiredPage;

    InitPageResources(sPokedexMenu->currentPage);

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
    if(sPokedexMenu->currentPage >= PAGE_MON_STATS && sPokedexMenu->currentPage <= PAGE_MON_FORMS && 
        sPokedexMenu->desiredPage >= PAGE_MON_STATS && sPokedexMenu->desiredPage <= PAGE_MON_FORMS)
    {
        gTasks[taskId].tDoFade = (sPokedexMenu->lastCrySpecies != sPokedexMenu->viewBaseSpecies);
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
    
    default:
        break;
    }

    gTasks[taskId].func = Task_PageFadeIn2;
}

static void Task_PageFadeIn2(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if(sPokedexMenu->currentPage >= PAGE_MON_STATS && sPokedexMenu->currentPage <= PAGE_MON_FORMS)
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
    
    default:
        break;
    }
}

static void Task_PageFadeOutAndExit(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DestroyPageResources(sPokedexMenu->currentPage);

        Free(sPokedexMenu);
        sPokedexMenu = NULL;

        Free(sTilemapBufferPtr);
        sTilemapBufferPtr = NULL;

        FreeAllWindowBuffers();
        DestroyTask(taskId);
        SetMainCallback2(CB2_ReturnToFieldFadeFromBlack);
    }
}

static u16 GetDexCount(u8 caseID)
{
    u16 i = 0;
    u16 count = 0;

    if(RoguePokedex_IsNationalDexActive())
    {
        for (i = 0; i < RoguePokedex_GetNationalDexLimit(); i++)
        {
            if (GetSetPokedexFlag(i + 1, caseID))
                count++;
        }
    }
    else
    {
        u8 dexVariant = RoguePokedex_GetDexVariant();
        
        for (i = 0; i < gPokedexVariants[dexVariant].speciesCount; i++)
        {
            if (GetSetPokedexFlag(SpeciesToNationalPokedexNum(gPokedexVariants[dexVariant].speciesList[i]), caseID))
                count++;
        }
    }

    return count;
}

static bool8 CheckDexCompletion(u8 caseID)
{
    u16 i = 0;

    if(RoguePokedex_IsNationalDexActive())
    {
        for (i = 0; i < RoguePokedex_GetNationalDexLimit(); i++)
        {
            if (!GetSetPokedexFlag(i + 1, caseID))
                return FALSE;
        }
    }
    else
    {
        u8 dexVariant = RoguePokedex_GetDexVariant();
        
        for (i = 0; i < gPokedexVariants[dexVariant].speciesCount; i++)
        {
            if (!GetSetPokedexFlag(SpeciesToNationalPokedexNum(gPokedexVariants[dexVariant].speciesList[i]), caseID))
                return FALSE;
        }
    }

    return TRUE;
}

static void DisplayTitleScreenCountersText(void)
{
    u8 color[3] = {0, 2, 3};

    FillWindowPixelBuffer(WIN_TITLE_COUNTERS, PIXEL_FILL(0));

    ConvertUIntToDecimalStringN(gStringVar4, GetDexCount(FLAG_GET_SEEN), STR_CONV_MODE_RIGHT_ALIGN, 3);
    AddTextPrinterParameterized4(WIN_TITLE_COUNTERS, FONT_NARROW, 4, 0, 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);

    ConvertUIntToDecimalStringN(gStringVar4, GetDexCount(FLAG_GET_CAUGHT), STR_CONV_MODE_RIGHT_ALIGN, 3);
    AddTextPrinterParameterized4(WIN_TITLE_COUNTERS, FONT_NARROW, 4, 24, 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);

    ConvertUIntToDecimalStringN(gStringVar4, GetDexCount(FLAG_GET_CAUGHT), STR_CONV_MODE_RIGHT_ALIGN, 3); // TODO - shiny catches
    AddTextPrinterParameterized4(WIN_TITLE_COUNTERS, FONT_NARROW, 4, 48, 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);

    PutWindowTilemap(WIN_TITLE_COUNTERS);
    CopyWindowToVram(WIN_TITLE_COUNTERS, COPYWIN_FULL);
}

static const u8* GetDexRegionName()
{
    if(RoguePokedex_IsNationalDexActive())
    {
        return gText_National;
    }
    else
    {
        u8 region = RoguePokedex_GetDexRegion();
        return gPokedexRegions[region].displayName;
    }
}

static const u8* GetDexVariantName()
{
    if(RoguePokedex_IsNationalDexActive())
    {
        ConvertUIntToDecimalStringN(gStringVar1, RoguePokedex_GetDexGenLimit(), STR_CONV_MODE_LEFT_ALIGN, 2);
        StringExpandPlaceholders(gStringVar2, gText_GenStr1);
        return gStringVar2;
    }
    else
    {
        u8 variant = RoguePokedex_GetDexVariant();
        return gPokedexVariants[variant].displayName;
    }
}

static void DisplayTitleDexVariantText(void)
{
    if(RoguePokedex_IsVariantEditUnlocked())
    {
        u8 color[3] = {0, 2, 3};
        bool8 arrowActive = sPokedexMenu->titleScreenInEditMode;

        FillWindowPixelBuffer(WIN_TITLE_VARIANT_SELECT, PIXEL_FILL(0));

        AddTextPrinterParameterized4(WIN_TITLE_VARIANT_SELECT, FONT_NARROW, 4 + (arrowActive ? 8 : 0), 0, 0, 0, color, TEXT_SKIP_DRAW, GetDexRegionName());

        AddTextPrinterParameterized4(WIN_TITLE_VARIANT_SELECT, FONT_NARROW, 4 + (arrowActive ? 8 : 0), 24, 0, 0, color, TEXT_SKIP_DRAW, GetDexVariantName());

        if(arrowActive)
        {
            AddTextPrinterParameterized4(WIN_TITLE_VARIANT_SELECT, FONT_NARROW, 4, 24 * sPokedexMenu->titleScreenCursorIdx, 0, 0, color, TEXT_SKIP_DRAW, gText_SelectorArrow);
        }

        PutWindowTilemap(WIN_TITLE_VARIANT_SELECT);
        CopyWindowToVram(WIN_TITLE_VARIANT_SELECT, COPYWIN_FULL);
    }
}

static void DisplayMonEntryText(void)
{
    u8 color[3] = {0, 2, 3};

    AddTextPrinterParameterized4(WIN_MON_SPECIES, FONT_NARROW, 4, 1, 0, 0, color, TEXT_SKIP_DRAW, gSpeciesNames[sPokedexMenu->viewBaseSpecies]);

    PutWindowTilemap(WIN_MON_SPECIES);
    CopyWindowToVram(WIN_MON_SPECIES, COPYWIN_FULL);
}

extern const u8 gAbilityNames[][ABILITY_NAME_LENGTH + 1];

static void DisplayMonStatsText(void)
{
    u8 i;
    const u8 ySpacing = 16;
    u8 color[3] = {0, 2, 3};

    // Print abilities
    {
        u16 prevAbility = ABILITY_NONE;
        u8 j = 0;

        for(i = 0; i < ARRAY_COUNT(gBaseStats[sPokedexMenu->viewBaseSpecies].abilities); ++i)
        {
            u16 ability = gBaseStats[sPokedexMenu->viewBaseSpecies].abilities[i];

            if(ability != ABILITY_NONE && ability != prevAbility)
            {
                AddTextPrinterParameterized4(WIN_MON_LIST, FONT_NARROW, 4, 1 + ySpacing * (2 + j), 0, 0, color, TEXT_SKIP_DRAW, gAbilityNames[ability]);
                prevAbility = ability;
                ++j;
            }
        }
    }

    // Print stats
    {
        u16 statTotal = 0;
        i = 0;

        // HP
        statTotal += gBaseStats[sPokedexMenu->viewBaseSpecies].baseHP;
        ConvertUIntToDecimalStringN(gStringVar4, gBaseStats[sPokedexMenu->viewBaseSpecies].baseHP, STR_CONV_MODE_RIGHT_ALIGN, 3);
        AddTextPrinterParameterized4(WIN_MON_LIST, FONT_NARROW, 115, 1 + ySpacing * (++i), 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);

        // Attack
        statTotal += gBaseStats[sPokedexMenu->viewBaseSpecies].baseAttack;
        ConvertUIntToDecimalStringN(gStringVar4, gBaseStats[sPokedexMenu->viewBaseSpecies].baseAttack, STR_CONV_MODE_RIGHT_ALIGN, 3);
        AddTextPrinterParameterized4(WIN_MON_LIST, FONT_NARROW, 115, 1 + ySpacing * (++i), 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);

        // Def
        statTotal += gBaseStats[sPokedexMenu->viewBaseSpecies].baseDefense;
        ConvertUIntToDecimalStringN(gStringVar4, gBaseStats[sPokedexMenu->viewBaseSpecies].baseDefense, STR_CONV_MODE_RIGHT_ALIGN, 3);
        AddTextPrinterParameterized4(WIN_MON_LIST, FONT_NARROW, 115, 1 + ySpacing * (++i), 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);

        // SpAttack
        statTotal += gBaseStats[sPokedexMenu->viewBaseSpecies].baseSpAttack;
        ConvertUIntToDecimalStringN(gStringVar4, gBaseStats[sPokedexMenu->viewBaseSpecies].baseSpAttack, STR_CONV_MODE_RIGHT_ALIGN, 3);
        AddTextPrinterParameterized4(WIN_MON_LIST, FONT_NARROW, 115, 1 + ySpacing * (++i), 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);

        // SpDef
        statTotal += gBaseStats[sPokedexMenu->viewBaseSpecies].baseSpDefense;
        ConvertUIntToDecimalStringN(gStringVar4, gBaseStats[sPokedexMenu->viewBaseSpecies].baseSpDefense, STR_CONV_MODE_RIGHT_ALIGN, 3);
        AddTextPrinterParameterized4(WIN_MON_LIST, FONT_NARROW, 115, 1 + ySpacing * (++i), 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);

        // Speed
        statTotal += gBaseStats[sPokedexMenu->viewBaseSpecies].baseSpeed;
        ConvertUIntToDecimalStringN(gStringVar4, gBaseStats[sPokedexMenu->viewBaseSpecies].baseSpeed, STR_CONV_MODE_RIGHT_ALIGN, 3);
        AddTextPrinterParameterized4(WIN_MON_LIST, FONT_NARROW, 115, 1 + ySpacing * (++i), 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);

        // Total
        ConvertUIntToDecimalStringN(gStringVar4, statTotal, STR_CONV_MODE_RIGHT_ALIGN, 3);
        AddTextPrinterParameterized4(WIN_MON_LIST, FONT_NARROW, 115, 1 + ySpacing * (++i), 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);
    }

    PutWindowTilemap(WIN_MON_LIST);
    CopyWindowToVram(WIN_MON_LIST, COPYWIN_FULL);
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

static void PrintDiplomaText(u8 *text, u8 var1, u8 var2)
{
    u8 color[3] = {0, 2, 3};

    AddTextPrinterParameterized4(0, FONT_NORMAL, var1, var2, 0, 0, color, TEXT_SKIP_DRAW, text);
}

// Title screen
//

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
        }
        else if(JOY_NEW(DPAD_UP | DPAD_DOWN))
        {
            PlaySE(SE_SELECT);
            sPokedexMenu->titleScreenCursorIdx = (sPokedexMenu->titleScreenCursorIdx + 1) % 2;
            DisplayTitleDexVariantText();
        }
        else if(JOY_NEW(DPAD_LEFT))
        {
            // Edit region
            if(sPokedexMenu->titleScreenCursorIdx == 0)
            {
                u8 region = RoguePokedex_GetDexRegion();

                PlaySE(SE_SELECT);

                if(region == POKEDEX_REGION_START)
                    region = POKEDEX_REGION_NONE;
                else if(region == POKEDEX_REGION_NONE)
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
                    u8 genLimit = RoguePokedex_GetDexGenLimit();
                    PlaySE(SE_SELECT);

                    if(genLimit == 1)
                        RoguePokedex_SetDexGenLimit(DEX_GEN_LIMIT);
                    else
                        RoguePokedex_SetDexGenLimit(genLimit - 1);
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
        else if(JOY_NEW(DPAD_RIGHT))
        {
            // Edit region
            if(sPokedexMenu->titleScreenCursorIdx == 0)
            {
                u8 region = RoguePokedex_GetDexRegion();

                PlaySE(SE_SELECT);

                if(region == POKEDEX_REGION_END)
                    region = POKEDEX_REGION_NONE;
                else if(region == POKEDEX_REGION_NONE)
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
                    u8 genLimit = RoguePokedex_GetDexGenLimit();
                    PlaySE(SE_SELECT);

                    if(genLimit == DEX_GEN_LIMIT)
                        RoguePokedex_SetDexGenLimit(1);
                    else
                        RoguePokedex_SetDexGenLimit(genLimit + 1);
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
            sPokedexMenu->scrollAmount = 0;
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

    // TODO - shiny
    if(CheckDexCompletion(FLAG_GET_CAUGHT))
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

    // TODO - update shiny counter
    if(CheckDexCompletion(FLAG_GET_CAUGHT))
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

    if(GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_GET_CAUGHT))
        return ENTRY_TYPE_CAUGHT;
    else if(GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_GET_SEEN))
        return ENTRY_TYPE_SEEN;

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
    u16 prevScrollAmount = sPokedexMenu->scrollAmount;

    if(JOY_NEW(DPAD_LEFT))
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
    else if(JOY_NEW(DPAD_RIGHT))
    {
        u8 x, y;
        x = sPokedexMenu->selectedIdx % COLUMN_ENTRY_COUNT;
        y = sPokedexMenu->selectedIdx / COLUMN_ENTRY_COUNT;

        x = (x + 1) % COLUMN_ENTRY_COUNT;
        sPokedexMenu->selectedIdx = x + y * COLUMN_ENTRY_COUNT;
    }
    else if(JOY_NEW(DPAD_UP))
    {
        if(sPokedexMenu->selectedIdx >= COLUMN_ENTRY_COUNT)
            sPokedexMenu->selectedIdx -= COLUMN_ENTRY_COUNT; // jump back a row
        else
        {
            if(sPokedexMenu->scrollAmount != 0)
            --sPokedexMenu->scrollAmount;
        }
    }
    else if(JOY_NEW(DPAD_DOWN))
    {
        if(sPokedexMenu->selectedIdx < OVERVIEW_ENTRY_COUNT - COLUMN_ENTRY_COUNT)
            sPokedexMenu->selectedIdx += COLUMN_ENTRY_COUNT; // jump down a row
        else
            ++sPokedexMenu->scrollAmount;
    }
    else if(JOY_NEW(L_BUTTON))
    {
        if(sPokedexMenu->scrollAmount != 0)
            sPokedexMenu->scrollAmount -= min(sPokedexMenu->scrollAmount, ROW_ENTRY_COUNT);
        else if(sPokedexMenu->selectedIdx != 0)
            sPokedexMenu->selectedIdx = 0; // Put back to first slot before looping
        else
            sPokedexMenu->scrollAmount = Overview_GetMaxScrollAmount();

        justJumpedPage = TRUE;
    }
    else if(JOY_NEW(R_BUTTON))
    {
        u8 maxScrollAmount = Overview_GetMaxScrollAmount();
        sPokedexMenu->scrollAmount += ROW_ENTRY_COUNT;
        
        if(sPokedexMenu->scrollAmount > maxScrollAmount)
        {
            u8 maxIdx = Overview_GetLastValidActiveIndex();
            if(sPokedexMenu->selectedIdx != maxIdx)
            {
                sPokedexMenu->scrollAmount = maxScrollAmount;
                sPokedexMenu->selectedIdx = maxIdx;
            }
            else
            {
                sPokedexMenu->scrollAmount = 0;
            }
        }

        justJumpedPage = TRUE;
    }
    else if (JOY_NEW(A_BUTTON))
    {
        u16 species = sPokedexMenu->overviewPageSpecies[sPokedexMenu->selectedIdx];

        if(GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_GET_SEEN))
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
    else if (JOY_NEW(B_BUTTON))
    {
        sPokedexMenu->desiredPage = PAGE_TITLE_SCREEN;
        gTasks[taskId].func = Task_SwapToPage;

        PlaySE(SE_SELECT);
    }


    // Clamp scroll amount
    if(prevSelectedIdx  != sPokedexMenu->selectedIdx)
    {
        sPokedexMenu->selectedIdx = min(sPokedexMenu->selectedIdx, Overview_GetLastValidActiveIndex());
    }

    if(prevScrollAmount != sPokedexMenu->scrollAmount)
    {
        sPokedexMenu->scrollAmount = min(sPokedexMenu->scrollAmount, Overview_GetMaxScrollAmount());
    }

    if(prevScrollAmount != sPokedexMenu->scrollAmount)
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
                if(GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_GET_CAUGHT))
                {
                    // Animated
                    sPokedexMenu->pageSprites[i] = CreateMonIcon(sPokedexMenu->overviewPageSpecies[i], SpriteCB_MonIcon, 28 + 32 * x, 18 + 40 * y, 0, 0, TRUE);
                }
                else if(GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_GET_SEEN))
                {
                    // Non animated
                    sPokedexMenu->pageSprites[i] = CreateMonIcon(sPokedexMenu->overviewPageSpecies[i], SpriteCallbackDummy, 28 + 32 * x, 18 + 40 * y, 0, 0, TRUE);
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

    for(i = 0; i < OVERVIEW_ENTRY_COUNT; ++i)
    {
        num = i + sPokedexMenu->scrollAmount * COLUMN_ENTRY_COUNT;

        species = SPECIES_NONE;

        if(RoguePokedex_IsNationalDexActive())
        {
            if(1 + num <= RoguePokedex_GetNationalDexLimit())
                species = NationalPokedexNumToSpecies(1 + num);
        }
        else
        {
            u8 dexVariant = RoguePokedex_GetDexVariant();
            if(num < gPokedexVariants[dexVariant].speciesCount)
                species = gPokedexVariants[dexVariant].speciesList[num];
        }

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
    if(RoguePokedex_IsNationalDexActive())
        return (RoguePokedex_GetNationalDexLimit() / COLUMN_ENTRY_COUNT) - ROW_ENTRY_COUNT + 1;
    else
    {
        u8 dexVariant = RoguePokedex_GetDexVariant();
        return (gPokedexVariants[dexVariant].speciesCount / COLUMN_ENTRY_COUNT) - ROW_ENTRY_COUNT + 1;
    }
}

// Mon Stats
//
u32 GetPokedexMonPersonality(u16 species);

void LoadMoveTypesSpritesheetAndPalette();
u8 CreateMonTypeIcon(u16 typeId, u8 x, u8 y);
void DestroyMonTypIcon(u8 spriteId);

static void MonStats_CreateSprites()
{
    LoadMoveTypesSpritesheetAndPalette(); // TODO - move

    sPokedexMenu->pageSprites[MON_SPRITE_FRONT_PIC] = CreateMonPicSprite_Affine(
        sPokedexMenu->viewBaseSpecies,
        NON_SHINY_PLACEHOLDER,
        GetPokedexMonPersonality(sPokedexMenu->viewBaseSpecies),
#ifdef ROGUE_EXPANSION
        GetGenderForSpecies(sPokedexMenu->viewBaseSpecies, 0),
#endif
        FALSE, // display as shiny if we have seen it?? 
        MON_PIC_AFFINE_FRONT,
        48, 66, 
        0, 
        gMonPaletteTable[sPokedexMenu->viewBaseSpecies].tag // gMonShinyPaletteTable
    );

    //sPokedexMenu->pageSprites[MON_SPRITE_BACK_PIC = CreateMonPicSprite_Affine(
    //    sPokedexMenu->viewBaseSpecies,
    //    NON_SHINY_PLACEHOLDER,
    //    GetPokedexMonPersonality(sPokedexMenu->viewBaseSpecies),
    //    FALSE, // display as shiny if we have seen it?? 
    //    MON_PIC_AFFINE_BACK,
    //    192, 70, 
    //    0, 
    //    gMonPaletteTable[sPokedexMenu->viewBaseSpecies].tag
    //);

    sPokedexMenu->pageSprites[MON_SPRITE_ICON] = CreateMonIcon(sPokedexMenu->viewBaseSpecies, SpriteCallbackDummy, 48, 8, 0, 0, TRUE);

    sPokedexMenu->pageSprites[MON_SPRITE_TYPE1] = CreateMonTypeIcon(gBaseStats[sPokedexMenu->viewBaseSpecies].type1, 138, 24);

    if(gBaseStats[sPokedexMenu->viewBaseSpecies].type2 != gBaseStats[sPokedexMenu->viewBaseSpecies].type1)
        sPokedexMenu->pageSprites[MON_SPRITE_TYPE2] = CreateMonTypeIcon(gBaseStats[sPokedexMenu->viewBaseSpecies].type2, 138 + 33, 24);
}

static void MonStats_DestroySprites()
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

// Can lag out if too many
#define MAX_NEIGHBOUR_CHECKS 100

static u16 MonStats_GetMonNeighbour(u16 fromSpecies, s8 offset)
{
    u16 i;

    if(RoguePokedex_IsNationalDexActive())
    {
        // GetSetPokedexFlag is slow so call it as little as possible
        u16 checkNum;
        u16 baseNum = SpeciesToNationalPokedexNum(fromSpecies);

        for(i = 1; i < MAX_NEIGHBOUR_CHECKS; ++i)
        {
            if(offset == 1)
            {
                checkNum = (baseNum + i - 1) % RoguePokedex_GetNationalDexLimit() + 1;
            }
            else // offset == 1
            {
                if(i >= baseNum)
                    checkNum = RoguePokedex_GetNationalDexLimit() - (i - baseNum); // loop back round
                else
                    checkNum = baseNum - i;
            }

            if(checkNum == fromSpecies)
                continue;

            // Only allowed to jump to seen mons
            if(!GetSetPokedexFlag(checkNum, FLAG_GET_SEEN))
                continue;

            return NationalPokedexNumToSpecies(checkNum);
        }
    }
    else
    {
        u16 fromIdx = (u16)-1;
        u8 dexVariant = RoguePokedex_GetDexVariant();

        for(i = 0; i < gPokedexVariants[dexVariant].speciesCount; ++i)
        {
            if(gPokedexVariants[dexVariant].speciesList[i] == fromSpecies)
            {
                fromIdx = i;
                break;
            }
        }

        if(fromIdx != (u16)-1)
        {
            u16 checkIdx;
            u16 checkSpecies;

            for(i = 0; i < gPokedexVariants[dexVariant].speciesCount; ++i)
            {
                if(offset == 1)
                {
                    checkIdx = (fromIdx + i) % gPokedexVariants[dexVariant].speciesCount;
                }
                else // offset == -1
                {
                    if(i >= fromIdx)
                        checkIdx = gPokedexVariants[dexVariant].speciesCount - (i - fromIdx); // loop back round
                    else
                        checkIdx = fromIdx - i;
                }

                if(checkIdx == fromIdx)
                    continue;

                checkSpecies = gPokedexVariants[dexVariant].speciesList[checkIdx];

                // Only allowed to jump to seen mons
                if(!GetSetPokedexFlag(SpeciesToNationalPokedexNum(checkSpecies), FLAG_GET_SEEN))
                    continue;

                return checkSpecies;
            }
        }

    }

    // Failed fallback
    return fromSpecies;
}

static void MonStats_HandleInput(u8 taskId)
{
    u16 viewSpecies = sPokedexMenu->viewBaseSpecies;

    // TODO A_BUTTON cycle forms (if any)
    if(JOY_NEW(L_BUTTON))
    {
        viewSpecies = MonStats_GetMonNeighbour(sPokedexMenu->viewBaseSpecies, -1);

        if(viewSpecies == sPokedexMenu->viewBaseSpecies)
            PlaySE(SE_FAILURE);
    }
    else if(JOY_NEW(R_BUTTON))
    {
        viewSpecies = MonStats_GetMonNeighbour(sPokedexMenu->viewBaseSpecies, 1);

        if(viewSpecies == sPokedexMenu->viewBaseSpecies)
            PlaySE(SE_FAILURE);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        // Go back up to overview
        sPokedexMenu->desiredPage = PAGE_OVERVIEW;
        gTasks[taskId].func = Task_SwapToPage;
        PlaySE(SE_PIN);
    }

    if(viewSpecies != sPokedexMenu->viewBaseSpecies)
    {
        sPokedexMenu->viewBaseSpecies = viewSpecies;
        gTasks[taskId].func = Task_SwapToPage;
        PlaySE(SE_DEX_PAGE);
    }
}

u8 RoguePokedex_GetDexRegion()
{
    if(!RoguePokedex_IsNationalDexActive())
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
    }

    return POKEDEX_REGION_NONE;
}

void RoguePokedex_SetDexRegion(u8 region)
{
    if(region < POKEDEX_REGION_COUNT)
        RoguePokedex_SetDexVariant(gPokedexRegions[region].variantList[0]);
    else
        RoguePokedex_SetDexVariant(POKEDEX_REGION_NONE);
}

u8 RoguePokedex_GetDexVariant()
{
    u8 dexVariant = VarGet(VAR_ROGUE_DEX_VARIANT);

    if(dexVariant < POKEDEX_VARIANT_COUNT)
        return dexVariant;

    // Variant none is basically national dex mode
    return POKEDEX_VARIANT_NONE;
}

void RoguePokedex_SetDexVariant(u8 variant)
{
    if(variant < POKEDEX_VARIANT_COUNT)
    {
        VarSet(VAR_ROGUE_DEX_VARIANT, variant);
        RoguePokedex_SetDexGenLimit(gPokedexVariants[variant].genLimit);
    }
    else
    {
        // Likely wanting to enter national dex mode
        VarSet(VAR_ROGUE_DEX_VARIANT, POKEDEX_VARIANT_NONE);
        RoguePokedex_SetDexGenLimit(DEX_GEN_LIMIT);
    }
}

u8 RoguePokedex_GetDexGenLimit()
{
    u8 genLimit = VarGet(VAR_ROGUE_DEX_GEN_LIMIT);

    if(genLimit != 0 && genLimit <= DEX_GEN_LIMIT)
        return genLimit;

    return DEX_GEN_LIMIT;
}

void RoguePokedex_SetDexGenLimit(u8 genLimit)
{
    if(genLimit != 0 && genLimit <= DEX_GEN_LIMIT)
        VarSet(VAR_ROGUE_DEX_GEN_LIMIT, genLimit);
    else
        VarSet(VAR_ROGUE_DEX_GEN_LIMIT, DEX_GEN_LIMIT);

}

bool8 RoguePokedex_IsNationalDexActive()
{
    // Variant none is treated national dex mode
    return RoguePokedex_GetDexVariant() == POKEDEX_VARIANT_NONE;
}

u16 RoguePokedex_GetNationalDexLimit()
{
    switch (RoguePokedex_GetDexGenLimit())
    {
    case 1:
        return NATIONAL_DEX_MEW;

    case 2:
        return NATIONAL_DEX_CELEBI;

#ifdef ROGUE_EXPANSION
    case 3:
        return NATIONAL_DEX_DEOXYS;

    case 4:
        return NATIONAL_DEX_ARCEUS;

    case 5:
        return NATIONAL_DEX_GENESECT;

    case 6:
        return NATIONAL_DEX_VOLCANION;

    case 7:
        return NATIONAL_DEX_MELMETAL;
#endif
    
    default:
        return NATIONAL_DEX_COUNT;
    }
}

bool8 RoguePokedex_IsVariantEditUnlocked()
{
    // TODO - link to post game unlock?
    return TRUE;
}

bool8 RoguePokedex_IsVariantEditEnabled()
{
    return RoguePokedex_IsVariantEditUnlocked();
}

u8 SpeciesToGen(u16 species);

bool8 RoguePokedex_IsSpeciesEnabled(u16 species)
{
    u8 genLimit = RoguePokedex_GetDexGenLimit();
    u8 speciesGen = SpeciesToGen(species);;

    if(speciesGen > genLimit)
        return FALSE;
    
#ifdef ROGUE_EXPANSION
    species = GET_BASE_SPECIES_ID(species);
#endif

    if(!RoguePokedex_IsNationalDexActive())
    {
        // TODO - Bake down bitset to quick check
        u8 variant = RoguePokedex_GetDexVariant();
        u8 i;

        for(i = 0; i < gPokedexVariants[variant].speciesCount; ++i)
        {
            if(gPokedexVariants[variant].speciesList[i] == species)
                return TRUE;
        }

        return FALSE;
    }

    return TRUE;
}