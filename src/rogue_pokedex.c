#include "global.h"
#include "palette.h"
#include "main.h"
#include "gpu_regs.h"
#include "scanline_effect.h"
#include "task.h"
#include "malloc.h"
#include "decompress.h"
#include "bg.h"
#include "window.h"
#include "string_util.h"
#include "text.h"
#include "overworld.h"
#include "menu.h"
#include "sound.h"
#include "pokedex.h"
#include "pokemon_icon.h"

#include "constants/rgb.h"
#include "constants/songs.h"

#include "rogue_pokedex.h"

// Overview
#define COLUMN_ENTRY_COUNT 7
#define ROW_ENTRY_COUNT 4
#define OVERVIEW_ENTRY_COUNT (COLUMN_ENTRY_COUNT * ROW_ENTRY_COUNT)

extern const u8 gText_DexNational[];
extern const u8 gText_DexHoenn[];
extern const u8 gText_PokedexDiploma[];

static void MainCB2(void);
static void Task_DiplomaFadeIn(u8);
static void Task_DiplomaWaitForKeyPress(u8);
static void Task_DiplomaFadeOut(u8);
static void DisplayDiplomaText(void);
static void InitDiplomaBg(void);
static void InitDiplomaWindow(void);
static void PrintDiplomaText(u8 *, u8, u8);

// Overview
static void Overview_RefillBg();
static void Overview_RecreateSprites();
static void Overview_SelectSpeciesToDiplay();
static void Overview_FillEntryBg(u8 entryX, u8 entryY, bool8 includeHeader);
static void Overview_FillEntryBg_Selected(u8 entryX, u8 entryY, bool8 includeHeader);
static u8 Overview_GetLastValidActiveIndex();
static u8 Overview_GetMaxScrollAmount();

struct PokedexMenu
{
    u16 selectedIdx;
    u16 scrollAmount;
    u8 overviewSprites[OVERVIEW_ENTRY_COUNT];
    u16 overviewSpecies[OVERVIEW_ENTRY_COUNT];
    u16 overviewNumbers[OVERVIEW_ENTRY_COUNT];
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

static const u32 sOverviewTilemap[] = INCBIN_U32("graphics/rogue_pokedex/info_screen.bin.lz");
static const u32 sDiplomaTiles[] = INCBIN_U32("graphics/rogue_pokedex/info_screen.4bpp.lz");

void CB2_Rogue_ShowPokedex(void)
{
    u8 i;
    sPokedexMenu = AllocZeroed(sizeof(struct PokedexMenu));

    for(i = 0; i < OVERVIEW_ENTRY_COUNT; ++i)
        sPokedexMenu->overviewSprites[i] = SPRITE_NONE;

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
    InitDiplomaBg();
    InitDiplomaWindow();
    ResetTempTileDataBuffers();
    DecompressAndCopyTileDataToVram(1, &sDiplomaTiles, 0, 0, 0);
    while (FreeTempTileDataBuffersIfPossible())
        ;
    LZDecompressWram(sOverviewTilemap, sTilemapBufferPtr);
    CopyBgTilemapBufferToVram(1);
    DisplayDiplomaText();

    // Init overview - TODO move
    LoadMonIconPalettes();

    Overview_SelectSpeciesToDiplay();
    Overview_RefillBg();
    Overview_RecreateSprites();

    BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
    EnableInterrupts(1);
    SetVBlankCallback(VBlankCB);
    SetMainCallback2(MainCB2);
    CreateTask(Task_DiplomaFadeIn, 0);
}

static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void Task_DiplomaFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_DiplomaWaitForKeyPress;
}

static void Task_DiplomaWaitForKeyPress(u8 taskId)
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


    // Clamp scroll amount
    if(prevSelectedIdx  != sPokedexMenu->selectedIdx)
    {
        sPokedexMenu->selectedIdx = min(sPokedexMenu->selectedIdx, Overview_GetLastValidActiveIndex());
    }

    if(prevScrollAmount != sPokedexMenu->scrollAmount)
    {
        sPokedexMenu->scrollAmount = min(sPokedexMenu->scrollAmount, Overview_GetMaxScrollAmount());
    }

    if (JOY_NEW(B_BUTTON))
    {
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_DiplomaFadeOut;
    }
    else
    {
        if(prevScrollAmount != sPokedexMenu->scrollAmount)
        {
            // Scroll up/down
            Overview_SelectSpeciesToDiplay();
            
            // Clamp it here incase we go out of bounds when jumping from front to back
            sPokedexMenu->selectedIdx = min(sPokedexMenu->selectedIdx, Overview_GetLastValidActiveIndex());

            Overview_RefillBg();
            Overview_RecreateSprites();

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
}

static void Task_DiplomaFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        u8 i;

        FreeMonIconPalettes();

        for(i = 0; i < OVERVIEW_ENTRY_COUNT; ++i)
        {
            if(sPokedexMenu->overviewSprites[i] != SPRITE_NONE)
                FreeAndDestroyMonIconSprite(&gSprites[sPokedexMenu->overviewSprites[i]]);
        }

        Free(sPokedexMenu);
        sPokedexMenu = NULL;

        Free(sTilemapBufferPtr);
        sTilemapBufferPtr = NULL;

        FreeAllWindowBuffers();
        DestroyTask(taskId);
        SetMainCallback2(CB2_ReturnToFieldFadeFromBlack);
    }
}

static void DisplayDiplomaText(void)
{
    //if (HasAllMons())
    //{
    //    SetGpuReg(REG_OFFSET_BG1HOFS, DISPLAY_WIDTH + 16);
    //    StringCopy(gStringVar1, gText_DexNational);
    //}
    //else
    //{
    //    SetGpuReg(REG_OFFSET_BG1HOFS, 0);
    //    StringCopy(gStringVar1, gText_DexHoenn);
    //}
    //StringExpandPlaceholders(gStringVar4, gText_PokedexDiploma);
    //PrintDiplomaText(gStringVar4, 0, 1);
    //PutWindowTilemap(0);
    //CopyWindowToVram(0, COPYWIN_FULL);
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

static void InitDiplomaBg(void)
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

static const struct WindowTemplate sDiplomaWinTemplates[2] =
{
    {
        .bg = 0,
        .tilemapLeft = 5,
        .tilemapTop = 2,
        .width = 20,
        .height = 16,
        .paletteNum = 15,
        .baseBlock = 1,
    },
    DUMMY_WIN_TEMPLATE,
};

static void InitDiplomaWindow(void)
{
    InitWindows(sDiplomaWinTemplates);
    DeactivateAllTextPrinters();
    LoadPalette(gStandardMenuPalette, 0xF0, 0x20);
    FillWindowPixelBuffer(0, PIXEL_FILL(0));
    PutWindowTilemap(0);
}

static void PrintDiplomaText(u8 *text, u8 var1, u8 var2)
{
    u8 color[3] = {0, 2, 3};

    AddTextPrinterParameterized4(0, FONT_NORMAL, var1, var2, 0, 0, color, TEXT_SKIP_DRAW, text);
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

#define FLIP_HORIZONTAL (0x04 << 8)

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
    species = sPokedexMenu->overviewSpecies[idx];

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
        u16 num = sPokedexMenu->overviewNumbers[entryX + entryY * COLUMN_ENTRY_COUNT];

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

static void Overview_RecreateSprites()
{
    u8 i, x , y;
    u16 species;

    for(i = 0; i < OVERVIEW_ENTRY_COUNT; ++i)
    {
        if(sPokedexMenu->overviewSprites[i] != SPRITE_NONE)
            FreeAndDestroyMonIconSprite(&gSprites[sPokedexMenu->overviewSprites[i]]);
    }
    
    for(x = 0; x < 7; ++x)
    {
        for(y = 0; y < 4; ++y)
        {
            i = x + y * COLUMN_ENTRY_COUNT;
            species = sPokedexMenu->overviewSpecies[i];

            if(species != SPECIES_NONE)
            {
                if(GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_GET_CAUGHT))
                {
                    // Animated
                    sPokedexMenu->overviewSprites[i] = CreateMonIcon(sPokedexMenu->overviewSpecies[i], SpriteCB_MonIcon, 28 + 32 * x, 18 + 40 * y, 0, 0, TRUE);
                }
                else if(GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_GET_SEEN))
                {
                    // Non animated
                    sPokedexMenu->overviewSprites[i] = CreateMonIcon(sPokedexMenu->overviewSpecies[i], SpriteCallbackDummy, 28 + 32 * x, 18 + 40 * y, 0, 0, TRUE);
                }
                else
                {
                    // Place ? icon
                    //sPokedexMenu->overviewSprites[i] = CreateMissingMonIcon(SpriteCallbackDummy, 28 + 32 * x, 18 + 40 * y, 0, 0);
                }
            }
        }
    }
}

static u8 RoguePokedex_GetDexVariant()
{
    // Variant none is basically national dex mode
    return POKEDEX_VARIANT_NONE;
    //return POKEDEX_VARIANT_HOENN_RSE;
}

static bool8 RoguePokdex_IsNationalDexActive()
{
    return RoguePokedex_GetDexVariant() == POKEDEX_VARIANT_NONE;
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

        if(RoguePokdex_IsNationalDexActive())
        {
            if(1 + num < NATIONAL_DEX_COUNT)
                species = NationalPokedexNumToSpecies(1 + num);
        }
        else
        {
            u8 dexVariant = RoguePokedex_GetDexVariant();
            if(num < gPokedexVariants[dexVariant].speciesCount)
                species = gPokedexVariants[dexVariant].speciesList[num];
        }

        sPokedexMenu->overviewSpecies[i] = species;
        sPokedexMenu->overviewNumbers[i] = num + 1;
    }
}

static u8 Overview_GetLastValidActiveIndex()
{
    u8 i, j;

    for(i = 0; i < OVERVIEW_ENTRY_COUNT; ++i)
    {
        j = OVERVIEW_ENTRY_COUNT - i - 1;
        if(sPokedexMenu->overviewSpecies[j] != SPECIES_NONE)
            return j;
    }

    return 0;
}

static u8 Overview_GetMaxScrollAmount()
{
    if(RoguePokdex_IsNationalDexActive())
        return (NATIONAL_DEX_COUNT / COLUMN_ENTRY_COUNT) - ROW_ENTRY_COUNT + 1;
    else
    {
        u8 dexVariant = RoguePokedex_GetDexVariant();
        return (gPokedexVariants[dexVariant].speciesCount / COLUMN_ENTRY_COUNT) - ROW_ENTRY_COUNT + 1;
    }
}