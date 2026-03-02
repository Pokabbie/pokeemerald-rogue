#include "global.h"
#include "constants/battle_anim.h"

#include "battle_anim.h"
#include "malloc.h"
#include "sprite.h"

#include "rogue_battlehud.h"
#include "rogue_settings.h"

#define HUD_TAG_PALETTE_0                   0x1100
#define HUD_TAG_PALETTE_1                   0x1101

#define HUD_TAG_SPRITE_BLUE_LIGHT_WALL      0x1200
#define HUD_TAG_SPRITE_GREEN_LIGHT_WALL     0x1201
#define HUD_TAG_SPRITE_WHIRLWIND            0x1202
#define HUD_TAG_SPRITE_SPIDER_WEB           0x1203
#define HUD_TAG_SPRITE_SPIKES               0x1204
#define HUD_TAG_SPRITE_TOXIC_SPIKES         0x1205
#define HUD_TAG_SPRITE_STEALTH_ROCK         0x1206

#define MAX_OVERLAY_SPRITES 32

// Want to sit at default priority (2) so we sort  

#define SUBPRIORITY_PLAYER_ABOVE            0
#define SUBPRIORITY_PLAYER_BELOW            40

#define SUBPRIORITY_ENEMY_ABOVE             30
#define SUBPRIORITY_ENEMY_BELOW             200


static const u8 sSpriteGfx_BlueLightWall[] = INCBIN_U8("graphics/rogue_battlehud/sprites/blue_light_wall.4bpp");
static const u8 sSpriteGfx_GreenLightWall[] = INCBIN_U8("graphics/rogue_battlehud/sprites/green_light_wall.4bpp");
static const u8 sSpriteGfx_Whirlwind[] = INCBIN_U8("graphics/rogue_battlehud/sprites/whirlwind.4bpp");
static const u8 sSpriteGfx_SpiderWeb[] = INCBIN_U8("graphics/rogue_battlehud/sprites/spider_web.4bpp");
static const u8 sSpriteGfx_Spikes[] = INCBIN_U8("graphics/rogue_battlehud/sprites/spikes.4bpp");
static const u8 sSpriteGfx_ToxicSpikes[] = INCBIN_U8("graphics/rogue_battlehud/sprites/toxic_spikes.4bpp");
static const u8 sSpriteGfx_StealthRock[] = INCBIN_U8("graphics/rogue_battlehud/sprites/stealth_rock.4bpp");

static const u16 sSpritePal_0[] = INCBIN_U16("graphics/rogue_battlehud/palettes/pal0.gbapal");
static const u16 sSpritePal_1[] = INCBIN_U16("graphics/rogue_battlehud/palettes/pal1.gbapal");


static const struct SpriteSheet sSpriteSheet_Overlay[] =
{
    { sSpriteGfx_BlueLightWall, sizeof(sSpriteGfx_BlueLightWall), HUD_TAG_SPRITE_BLUE_LIGHT_WALL },
    { sSpriteGfx_GreenLightWall, sizeof(sSpriteGfx_GreenLightWall), HUD_TAG_SPRITE_GREEN_LIGHT_WALL },
    { sSpriteGfx_Whirlwind, sizeof(sSpriteGfx_Whirlwind), HUD_TAG_SPRITE_WHIRLWIND },
    { sSpriteGfx_SpiderWeb, sizeof(sSpriteGfx_Whirlwind), HUD_TAG_SPRITE_SPIDER_WEB },
    { sSpriteGfx_Spikes, sizeof(sSpriteGfx_Spikes), HUD_TAG_SPRITE_SPIKES },
    { sSpriteGfx_ToxicSpikes, sizeof(sSpriteGfx_ToxicSpikes), HUD_TAG_SPRITE_TOXIC_SPIKES },
    { sSpriteGfx_StealthRock, sizeof(sSpriteGfx_StealthRock), HUD_TAG_SPRITE_STEALTH_ROCK },
    {},
};

static const struct SpritePalette sSpritePalette_Overlay[] =
{
    { sSpritePal_0, HUD_TAG_PALETTE_0 },
    { sSpritePal_1, HUD_TAG_PALETTE_1 },
    {},
};

static void SpriteCallbackFlicker(struct Sprite *sprite);
static void SpriteCallbackTailwind(struct Sprite *sprite);

static const struct SpriteTemplate sReflectWallSpriteTemplate =
{
    .tileTag = HUD_TAG_SPRITE_BLUE_LIGHT_WALL,
    .paletteTag = HUD_TAG_PALETTE_1,
    .oam = &gOamData_AffineOff_ObjNormal_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static const struct SpriteTemplate sLightScreenWallSpriteTemplate =
{
    .tileTag = HUD_TAG_SPRITE_GREEN_LIGHT_WALL,
    .paletteTag = HUD_TAG_PALETTE_1,
    .oam = &gOamData_AffineOff_ObjNormal_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static const struct SpriteTemplate sWhirlwindSpriteTemplate =
{
    .tileTag = HUD_TAG_SPRITE_WHIRLWIND,
    .paletteTag = HUD_TAG_PALETTE_1,
    .oam = &gOamData_AffineOff_ObjNormal_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackTailwind,
};

static const struct SpriteTemplate sSpiderWebSpriteTemplate =
{
    .tileTag = HUD_TAG_SPRITE_SPIDER_WEB,
    .paletteTag = HUD_TAG_PALETTE_0,
    .oam = &gOamData_AffineOff_ObjNormal_64x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static const struct SpriteTemplate sSpikesSpriteTemplate =
{
    .tileTag = HUD_TAG_SPRITE_SPIKES,
    .paletteTag = HUD_TAG_PALETTE_0,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static const struct SpriteTemplate sToxicSpikesSpriteTemplate =
{
    .tileTag = HUD_TAG_SPRITE_TOXIC_SPIKES,
    .paletteTag = HUD_TAG_PALETTE_0,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static const struct SpriteTemplate sStealthRockSpriteTemplate =
{
    .tileTag = HUD_TAG_SPRITE_STEALTH_ROCK,
    .paletteTag = HUD_TAG_PALETTE_0,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};


struct RogueBattleOverlay
{
    u8 sprites[MAX_OVERLAY_SPRITES];
    u8 spriteCount;
};

EWRAM_DATA struct RogueBattleOverlay* gRogueBattleOverlay = NULL;


void RogueBH_CreateBattleOverlay()
{
    if(gRogueBattleOverlay == NULL)
    {
        u8 spikeCount, toxicSpikeCount;
        bool8 hasReflect, hasLightscreen, hasStealthRock, hasStickyWeb, hasTailwind;
        u8 spriteCount = 0;

        LoadSpriteSheets(sSpriteSheet_Overlay);
        LoadSpritePalettes(sSpritePalette_Overlay);

        gRogueBattleOverlay = Alloc(sizeof(struct RogueBattleOverlay));

        // Player
        {
            spikeCount = (gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SPIKES) ? gSideTimers[B_SIDE_PLAYER].spikesAmount : 0;
            hasReflect = (gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_REFLECT);
            hasLightscreen = (gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_LIGHTSCREEN);
#ifdef ROGUE_EXPANSION
            toxicSpikeCount = (gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TOXIC_SPIKES) ? gSideTimers[B_SIDE_PLAYER].toxicSpikesAmount : 0; 
            hasStealthRock = (gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_STEALTH_ROCK);
            hasStickyWeb = (gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_STICKY_WEB);
            hasTailwind = (gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
#else
            toxicSpikeCount = 0; 
            hasStealthRock = FALSE;
            hasStickyWeb = FALSE;
            hasTailwind = FALSE;
#endif

#ifdef ROGUE_DEBUG
            if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_FULL_BATTLE_HUD))
            {
                spikeCount = 3;
                toxicSpikeCount = 3; 
                hasReflect = TRUE;
                hasLightscreen = TRUE;
                hasStealthRock = TRUE;
                hasStickyWeb = TRUE;
                hasTailwind = TRUE;
            }
#endif

            // Sticky web
            if(hasStickyWeb)
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sSpiderWebSpriteTemplate, 66, 107, SUBPRIORITY_PLAYER_BELOW);


            // Reflect
            if(hasReflect)
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sReflectWallSpriteTemplate, 62, 72, SUBPRIORITY_PLAYER_BELOW);


            // Light Screen
            if(hasLightscreen)
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sLightScreenWallSpriteTemplate, 70, 78, SUBPRIORITY_PLAYER_BELOW);


            // Stealth Rock
            if(hasStealthRock)
            {
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sStealthRockSpriteTemplate, 16, 83, SUBPRIORITY_PLAYER_ABOVE);
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sStealthRockSpriteTemplate, 106, 93, SUBPRIORITY_PLAYER_ABOVE);
            }


            // Spikes
            if(spikeCount >= 1)
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sSpikesSpriteTemplate, 24, 98, (gBattleTypeFlags & BATTLE_TYPE_DOUBLE) ? SUBPRIORITY_PLAYER_ABOVE : SUBPRIORITY_PLAYER_BELOW);
            
            if(spikeCount >= 2)
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sSpikesSpriteTemplate, 20, 102, (gBattleTypeFlags & BATTLE_TYPE_DOUBLE) ? SUBPRIORITY_PLAYER_ABOVE : SUBPRIORITY_PLAYER_BELOW);
            
            if(spikeCount >= 3)
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sSpikesSpriteTemplate, 16, 106, (gBattleTypeFlags & BATTLE_TYPE_DOUBLE) ? SUBPRIORITY_PLAYER_ABOVE : SUBPRIORITY_PLAYER_BELOW);


            // Toxic Spikes
            if(toxicSpikeCount >= 1)
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sToxicSpikesSpriteTemplate, 106, 98, (gBattleTypeFlags & BATTLE_TYPE_DOUBLE) ? SUBPRIORITY_PLAYER_ABOVE : SUBPRIORITY_PLAYER_BELOW);
            
            if(toxicSpikeCount >= 2)
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sToxicSpikesSpriteTemplate, 110, 102, (gBattleTypeFlags & BATTLE_TYPE_DOUBLE) ? SUBPRIORITY_PLAYER_ABOVE : SUBPRIORITY_PLAYER_BELOW);
            
            if(toxicSpikeCount >= 2)
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sToxicSpikesSpriteTemplate, 114, 106, (gBattleTypeFlags & BATTLE_TYPE_DOUBLE) ? SUBPRIORITY_PLAYER_ABOVE : SUBPRIORITY_PLAYER_BELOW);


            // Tailwind
            if(hasTailwind)
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sWhirlwindSpriteTemplate, -64, 82, SUBPRIORITY_PLAYER_ABOVE);
        }

        // Opponent
        {
            spikeCount = (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_SPIKES) ? gSideTimers[B_SIDE_OPPONENT].spikesAmount : 0;
            hasReflect = (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_REFLECT);
            hasLightscreen = (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_LIGHTSCREEN);
#ifdef ROGUE_EXPANSION
            toxicSpikeCount = (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TOXIC_SPIKES) ? gSideTimers[B_SIDE_OPPONENT].toxicSpikesAmount : 0; 
            hasStealthRock = (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK);
            hasStickyWeb = (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STICKY_WEB);
            hasTailwind = (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TAILWIND);
#else
            toxicSpikeCount = 0; 
            hasStealthRock = FALSE;
            hasStickyWeb = FALSE;
            hasTailwind = FALSE;
#endif

#ifdef ROGUE_DEBUG
            if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_FULL_BATTLE_HUD))
            {
                spikeCount = 3;
                toxicSpikeCount = 3; 
                hasReflect = TRUE;
                hasLightscreen = TRUE;
                hasStealthRock = TRUE;
                hasStickyWeb = TRUE;
                hasTailwind = TRUE;
            }
#endif

            // Sticky web
            if(hasStickyWeb)
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sSpiderWebSpriteTemplate, 176, 66, SUBPRIORITY_ENEMY_BELOW);


            // Reflect
            if(hasReflect)
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sReflectWallSpriteTemplate, 168, 35, SUBPRIORITY_ENEMY_ABOVE);


            // Light Screen
            if(hasLightscreen)
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sLightScreenWallSpriteTemplate, 176, 41, SUBPRIORITY_ENEMY_ABOVE);


            // Stealth Rock
            if(hasStealthRock)
            {
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sStealthRockSpriteTemplate, 126, 18, SUBPRIORITY_ENEMY_ABOVE);
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sStealthRockSpriteTemplate, 216, 38, SUBPRIORITY_ENEMY_ABOVE);
            }


            // Spikes
            if(spikeCount >= 1)
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sSpikesSpriteTemplate, 134, 50, SUBPRIORITY_ENEMY_BELOW);
            
            if(spikeCount >= 2)
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sSpikesSpriteTemplate, 130, 54, SUBPRIORITY_ENEMY_BELOW);
                
            if(spikeCount >= 3)
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sSpikesSpriteTemplate, 126, 58, SUBPRIORITY_ENEMY_BELOW);


            // Toxic Spikes
            if(toxicSpikeCount >= 1)
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sToxicSpikesSpriteTemplate, 216, 50, SUBPRIORITY_ENEMY_BELOW);

            if(toxicSpikeCount >= 2)
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sToxicSpikesSpriteTemplate, 220, 54, SUBPRIORITY_ENEMY_BELOW);

            if(toxicSpikeCount >= 3)
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sToxicSpikesSpriteTemplate, 224, 58, SUBPRIORITY_ENEMY_BELOW);


            // Tailwind
            if(hasTailwind)
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sWhirlwindSpriteTemplate, 264, 32, SUBPRIORITY_ENEMY_ABOVE);
        }

        AGB_ASSERT(spriteCount <= MAX_OVERLAY_SPRITES);
        gRogueBattleOverlay->spriteCount = spriteCount;
    }
}

void RogueBH_RemoveBattleOverlay(bool32 fromResetSprites)
{
    if(gRogueBattleOverlay != NULL)
    {
        // Remove sprites manually, as the scene is not being fully reset
        if(!fromResetSprites)
        {
            u8 i;

            for(i = 0; sSpriteSheet_Overlay[i].data != NULL; ++i)
            {
                FreeSpriteTilesByTag(sSpriteSheet_Overlay[i].tag);
            }

            for(i = 0; sSpritePalette_Overlay[i].data != NULL; ++i)
            {
                FreeSpritePaletteByTag(sSpritePalette_Overlay[i].tag);
            }

            for(i = 0; i < gRogueBattleOverlay->spriteCount; ++i)
            {
                DestroySprite(&gSprites[gRogueBattleOverlay->sprites[i]]);
            }
        }

        FREE_AND_SET_NULL(gRogueBattleOverlay);
    }
}

static void SpriteCallbackFlicker(struct Sprite *sprite)
{
    if(sprite->data[0] > 16)
    {
        sprite->invisible = !sprite->invisible;
        sprite->data[0] = 0;
    }
    else
    {
        ++sprite->data[0];
    }
}

#define PADDING 128

// Stolen from sprite.c (Not sure why it's not working tbh)
static void SetSpriteOamFlipBits(struct Sprite *sprite)
{
    sprite->oam.matrixNum &= 0x7;
    sprite->oam.matrixNum |= (((sprite->hFlip) & 1) << 3);
    sprite->oam.matrixNum |= (((sprite->vFlip) & 1) << 4);
}

static void SpriteCallbackTailwind(struct Sprite *sprite)
{
    if(sprite->subpriority == SUBPRIORITY_ENEMY_ABOVE || sprite->subpriority == SUBPRIORITY_ENEMY_BELOW)
    {
        if(sprite->hFlip != TRUE)
        {
            sprite->hFlip = TRUE;
            SetSpriteOamFlipBits(sprite);
        }

        sprite->x -= 8;

        if(sprite->x <= -64 - PADDING)
        {
            sprite->x = 264 + PADDING;
        }
    }
    else
    {
        sprite->x += 8;

        if(sprite->x >= 264 + PADDING)
        {
            sprite->x = -64 - PADDING;
        }
    }
}