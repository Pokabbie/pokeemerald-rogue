#include "global.h"
#include "constants/songs.h"

#include "palette.h"
#include "pokemon_icon.h"
#include "main.h"
#include "field_screen_effect.h"
#include "gpu_regs.h"
#include "scanline_effect.h"
#include "task.h"
#include "trainer_pokemon_sprites.h"
#include "malloc.h"
#include "decompress.h"
#include "event_data.h"
#include "bg.h"
#include "window.h"
#include "script.h"
#include "sound.h"
#include "strings.h"
#include "string_util.h"
#include "text.h"
#include "overworld.h"
#include "menu.h"
#include "pokedex.h"
#include "constants/rgb.h"
#include "random.h"

#include "rogue_battlechecker.h"
#include "rogue_controller.h"
#include "rogue_player_customisation.h"
#include "rogue_trainers.h"

#define FLIP_VERTICAL (0x08 << 8)
#define FLIP_HORIZONTAL (0x04 << 8)

#define INVALID_TRAINDER_ID ((u16)-1)

#define GFX_ICON_SPECIES(species) (species >= FOLLOWMON_SHINY_OFFSET ? (species - FOLLOWMON_SHINY_OFFSET) : species)

enum
{
    WIN_HEADER,

    WIN_COUNT,
};

struct BattleCheckerState
{
    u8 currIndex;
    u8 playerTrainerSprite;
    u8 enemyTrainerSprite;
};

static void MainCB2(void);
static void Task_BattleCheckerFadeIn(u8);
static void Task_BattleCheckerWaitForKeyPress(u8);
static void Task_BattleCheckerFadeOut(u8);
static void InitBattleCheckerBg(void);
static void InitBattleCheckerSprites(void);
static void InitBattleCheckerHeader(void);
static void InitBattleCheckerWindow(void);

EWRAM_DATA static u8* sBattleCheckerTilemapPtr = NULL;
EWRAM_DATA static struct BattleCheckerState* sBattleCheckerState = NULL;

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static const u16 sBattleCheckerBgPalettes[][16] =
{
    INCBIN_U16("graphics/rogue_battlechecker/checker.gbapal"),
};

static const u32 sBattleCheckerTilemap[] = INCBIN_U32("graphics/rogue_battlechecker/checker.bin.lz");
static const u32 sBattleCheckerTiles[] = INCBIN_U32("graphics/rogue_battlechecker/checker.4bpp.lz");


static const struct BgTemplate sBattleCheckerBgTemplates[2] =
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

static const struct WindowTemplate sBattleCheckerWinTemplates[WIN_COUNT + 1] =
{
    [WIN_HEADER] =
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 0,
        .width = 29,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 1,
    },
    [WIN_COUNT] = DUMMY_WIN_TEMPLATE,
};

#define SPRITE_SHEET_TILE_TAG 5525
#define SPRITE_SHEET_PAL_TAG 5526

static const struct OamData sPointerSpriteOamData =
{
    .x = 0,
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = 0,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x32),
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x32),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 1,
    .affineParam = 0,
};

static const union AnimCmd sPointerSprite_PointerFrame[] =
{
    ANIMCMD_FRAME(0, 5, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sPointerSprite_PencilFrame[] =
{
    ANIMCMD_FRAME(16, 5, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sPointerSprite_PointerOutlineFrame[] =
{
    ANIMCMD_FRAME(32, 5, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sPointerSprite_PencilOutlineFrame[] =
{
    ANIMCMD_FRAME(48, 5, FALSE, FALSE),
    ANIMCMD_END
};

enum
{
    POINTER_ANIM_POINTER,
    POINTER_ANIM_PENCIL,
    POINTER_ANIM_POINTER_OUTLINE,
    POINTER_ANIM_PENCIL_OUTLINE,
    POINTER_ANIM_COUNT,
};

static const union AnimCmd *const sPencilAnimationCommands[POINTER_ANIM_COUNT] =
{
    [POINTER_ANIM_POINTER] = sPointerSprite_PointerFrame,
    [POINTER_ANIM_PENCIL] = sPointerSprite_PencilFrame,
    [POINTER_ANIM_POINTER_OUTLINE] = sPointerSprite_PointerOutlineFrame,
    [POINTER_ANIM_PENCIL_OUTLINE] = sPointerSprite_PencilOutlineFrame,
};

static const struct SpriteTemplate sPointerSprite =
{
    .tileTag = SPRITE_SHEET_TILE_TAG,
    .paletteTag = SPRITE_SHEET_PAL_TAG,
    .oam = &sPointerSpriteOamData,
    .anims = sPencilAnimationCommands,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

//static const struct OamData sPointerSpriteOamData =
//{
//    .x = 0,
//    .y = 0,
//    .affineMode = ST_OAM_AFFINE_OFF,
//    .objMode = ST_OAM_OBJ_NORMAL,
//    .mosaic = 0,
//    .bpp = ST_OAM_4BPP,
//    .shape = SPRITE_SHAPE(32x32),
//    .matrixNum = 0,
//    .size = SPRITE_SIZE(32x32),
//    .tileNum = 0,
//    .priority = 0,
//    .paletteNum = 1,
//    .affineParam = 0,
//};

void Rogue_OpenBattleChecker(MainCallback callback)
{
    gMain.savedCallback = callback;
    LockPlayerFieldControls();
    gFieldCallback = FieldCB_ContinueScriptHandleMusic;
    
    AGB_ASSERT(sBattleCheckerTilemapPtr == NULL);
    AGB_ASSERT(sBattleCheckerState == NULL);

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
    LoadPalette(sBattleCheckerBgPalettes, 0, ARRAY_COUNT(sBattleCheckerBgPalettes) * 32);
    sBattleCheckerTilemapPtr = Alloc(0x1000);
    sBattleCheckerState = AllocZeroed(sizeof(struct BattleCheckerState));
    
    sBattleCheckerState->currIndex = gRogueRun.partySnapshotCount - 1;
    sBattleCheckerState->playerTrainerSprite = SPRITE_NONE;
    sBattleCheckerState->enemyTrainerSprite = SPRITE_NONE;

    InitBattleCheckerBg();
    InitBattleCheckerSprites();
    InitBattleCheckerWindow();
    InitBattleCheckerHeader();
    ResetTempTileDataBuffers();
    DecompressAndCopyTileDataToVram(1, &sBattleCheckerTiles, 0, 0, 0);
    while (FreeTempTileDataBuffersIfPossible())
        ;
    LZDecompressWram(sBattleCheckerTilemap, sBattleCheckerTilemapPtr);
    CopyBgTilemapBufferToVram(1);
    BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
    EnableInterrupts(1);
    SetVBlankCallback(VBlankCB);
    SetMainCallback2(MainCB2);

    CreateTask(Task_BattleCheckerFadeIn, 0);
}


static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void Task_BattleCheckerFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_BattleCheckerWaitForKeyPress;
}

static void Task_BattleCheckerWaitForKeyPress(u8 taskId)
{
    //u8 gameState = CalculateBoardState();
//
    if (JOY_NEW(B_BUTTON))
    {
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_BattleCheckerFadeOut;
        return;
    }

    if(sBattleCheckerState->currIndex < gRogueRun.partySnapshotCount)
    {
        if(JOY_NEW(DPAD_LEFT | DPAD_RIGHT))
        {
            PlaySE(SE_DEX_PAGE);

            if(JOY_NEW(DPAD_RIGHT))
            {
                sBattleCheckerState->currIndex++;
                if(sBattleCheckerState->currIndex == gRogueRun.partySnapshotCount)
                {
                    sBattleCheckerState->currIndex = 0;
                }
            }
            else //if(JOY_NEW(DPAD_LEFT))
            {
                if(sBattleCheckerState->currIndex == 0)
                    sBattleCheckerState->currIndex = gRogueRun.partySnapshotCount - 1;
                else
                    sBattleCheckerState->currIndex--;
            }

            InitBattleCheckerHeader();
            InitBattleCheckerSprites();
        }
    }
}

static void FreeTrainerSprites()
{
    if(sBattleCheckerState->playerTrainerSprite != SPRITE_NONE)
    {
        FreeAndDestroyTrainerPicSprite(sBattleCheckerState->playerTrainerSprite);
        sBattleCheckerState->playerTrainerSprite = SPRITE_NONE;
    }
    
    if(sBattleCheckerState->enemyTrainerSprite != SPRITE_NONE)
    {
        FreeAndDestroyTrainerPicSprite(sBattleCheckerState->enemyTrainerSprite);
        sBattleCheckerState->enemyTrainerSprite = SPRITE_NONE;
    }
    ResetAllPicSprites();
}

static void Task_BattleCheckerFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        FreeTrainerSprites();

        Free(sBattleCheckerTilemapPtr);
        Free(sBattleCheckerState);
        sBattleCheckerTilemapPtr = NULL;
        sBattleCheckerState = NULL;
        FreeAllWindowBuffers();
        DestroyTask(taskId);
        SetMainCallback2(gMain.savedCallback);
    }
}

static void InitBattleCheckerBg(void)
{
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sBattleCheckerBgTemplates, ARRAY_COUNT(sBattleCheckerBgTemplates));
    SetBgTilemapBuffer(1, sBattleCheckerTilemapPtr);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
    ShowBg(0);
    ShowBg(1);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_BLDALPHA, 0);
    SetGpuReg(REG_OFFSET_BLDY, 0);
}

static u8 const sText_PickExit[] = _("{DPAD_LEFTRIGHT}Pick {B_BUTTON}Exit");
static u8 const sText_VS[] = _("    VS ");
static u8 const sText_Slash[] = _(" / ");

static void InitBattleCheckerHeader(void)
{
    u8 const color[3] = {0, 1, 2};
    u8* str = gStringVar4;

    AGB_ASSERT(sBattleCheckerState != NULL);

    FillWindowPixelBuffer(WIN_HEADER, PIXEL_FILL(0));
    
    str = ConvertUIntToDecimalStringN(str, sBattleCheckerState->currIndex + 1, STR_CONV_MODE_RIGHT_ALIGN, 2);
    str = StringAppend(str, sText_Slash);
    str = ConvertUIntToDecimalStringN(str, gRogueRun.partySnapshotCount, STR_CONV_MODE_RIGHT_ALIGN, 2);

    if(sBattleCheckerState->currIndex < gRogueRun.partySnapshotCount)
    {
        struct RoguePartySnapshot* snapshot = &gRogueRun.partySnapshots[sBattleCheckerState->currIndex];

        if(snapshot->trainerId != INVALID_TRAINDER_ID)
        {
            str = StringAppend(gStringVar4, sText_VS);
            str = StringAppend(gStringVar4, Rogue_GetTrainerName(snapshot->trainerId));
        }
    }

    AddTextPrinterParameterized4(WIN_HEADER, FONT_NORMAL, 0, 0, 0, 0, color, TEXT_SKIP_DRAW, gStringVar4);
    AddTextPrinterParameterized4(WIN_HEADER, FONT_NORMAL, 156, 0, 0, 0, color, TEXT_SKIP_DRAW, sText_PickExit);

    PutWindowTilemap(WIN_HEADER);
    CopyWindowToVram(WIN_HEADER, COPYWIN_FULL);
}

static void InitBattleCheckerSprites(void)
{
    AGB_ASSERT(sBattleCheckerState != NULL);

    FreeTrainerSprites();
    ResetSpriteData();
    FreeAllSpritePalettes();
    LoadMonIconPalettes();

    if(sBattleCheckerState->currIndex < gRogueRun.partySnapshotCount)
    {
        u32 i;
        struct RoguePartySnapshot* snapshot = &gRogueRun.partySnapshots[sBattleCheckerState->currIndex];

        for(i = 0; i < PARTY_SIZE; ++i)
        {
            u16 iconSpecies = GFX_ICON_SPECIES(snapshot->partySpeciesGfx[i]);

            if(iconSpecies != SPECIES_NONE)
            {
                CreateMonIcon(
                    iconSpecies, 
                    SpriteCB_MonIcon, // todo - if fainted SpriteCallbackDummy
                    52 + 32 * (i % 3),
                    104 + 32 * (i / 3),
                    0, 
                    snapshot->partyPersonalities[i],
                    MON_MALE
                );
            }
            
            if(snapshot->trainerId != INVALID_TRAINDER_ID)
            {
                iconSpecies = GFX_ICON_SPECIES(snapshot->enemySpeciesGfx[i]);
                if(iconSpecies != SPECIES_NONE)
                {
                    CreateMonIcon(
                        iconSpecies, 
                        SpriteCB_MonIcon, // todo - if fainted SpriteCallbackDummy
                        124 + 32 * (i % 3),
                        32 + 32 * (i /3),
                        0, 
                        0,
                        MON_MALE
                    );
                }
            }
        }

        // Player trainer
        sBattleCheckerState->playerTrainerSprite = CreateTrainerPicSprite(
            RoguePlayer_GetTrainerFrontPic(), 
            TRUE,
            172, 32 + 92, 
            9, 
            TAG_NONE);

        // Enemy trainer
        if(snapshot->trainerId != INVALID_TRAINDER_ID)
        {
            sBattleCheckerState->enemyTrainerSprite = CreateTrainerPicSprite(
                Rogue_GetTrainer(snapshot->trainerId)->trainerPic, 
                TRUE,
                68, 40 + 12, 
                8, 
                TAG_NONE);
        }
    }
}

static void InitBattleCheckerWindow(void)
{
    InitWindows(sBattleCheckerWinTemplates);
    DeactivateAllTextPrinters();
    LoadPalette(gStandardMenuPalette, 0xF0, 0x20);

    //FillWindowPixelBuffer(WIN_HEADER, PIXEL_FILL(0));
    //PutWindowTilemap(WIN_HEADER);
}