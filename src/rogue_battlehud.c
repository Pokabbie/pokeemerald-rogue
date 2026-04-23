#include "global.h"
#include "constants/battle.h"
#include "constants/battle_anim.h"
#include "constants/songs.h"

#include "battle_anim.h"
#include "battle_main.h"
#include "battle_message.h"
#include "malloc.h"
#include "main.h"
#include "random.h"
#include "sound.h"
#include "string_util.h"
#include "sprite.h"
#include "trig.h"

#include "rogue_battlehud.h"
#include "rogue_settings.h"

#ifndef ROGUE_EXPANSION
#define B_WEATHER_SNOW                      B_WEATHER_HAIL
#endif

#define HUD_TAG_PALETTE_0                   0x1100
#define HUD_TAG_PALETTE_1                   0x1101

#define HUD_TAG_SPRITE_BLUE_LIGHT_WALL      0x1200
#define HUD_TAG_SPRITE_GREEN_LIGHT_WALL     0x1201
#define HUD_TAG_SPRITE_WHIRLWIND            0x1202
#define HUD_TAG_SPRITE_SPIDER_WEB           0x1203
#define HUD_TAG_SPRITE_SPIKES               0x1204
#define HUD_TAG_SPRITE_TOXIC_SPIKES         0x1205
#define HUD_TAG_SPRITE_STEALTH_ROCK         0x1206
#define HUD_TAG_SPRITE_RAIN                 0x1207
#define HUD_TAG_SPRITE_SUN                  0x1208
#define HUD_TAG_SPRITE_SNOW                 0x1209
#define HUD_TAG_SPRITE_SANDSTORM            0x120A
#define HUD_TAG_SPRITE_DEX_PROMPT           0x120B

#define MAX_OVERLAY_SPRITES 32

// Want to sit at default priority (2) so we sort  

#define SUBPRIORITY_PLAYER_ABOVE            0
#define SUBPRIORITY_PLAYER_BELOW            40

#define SUBPRIORITY_ENEMY_ABOVE             30
#define SUBPRIORITY_ENEMY_BELOW             200

// Sprites & Palettes
//

static const u8 sSpriteGfx_DexPrompt[] = INCBIN_U8("graphics/rogue_battlehud/sprites/dex_prompt.4bpp");
static const u8 sSpriteGfx_BlueLightWall[] = INCBIN_U8("graphics/rogue_battlehud/sprites/blue_light_wall.4bpp");
static const u8 sSpriteGfx_GreenLightWall[] = INCBIN_U8("graphics/rogue_battlehud/sprites/green_light_wall.4bpp");
static const u8 sSpriteGfx_Whirlwind[] = INCBIN_U8("graphics/rogue_battlehud/sprites/whirlwind.4bpp");
static const u8 sSpriteGfx_SpiderWeb[] = INCBIN_U8("graphics/rogue_battlehud/sprites/spider_web.4bpp");
static const u8 sSpriteGfx_Spikes[] = INCBIN_U8("graphics/rogue_battlehud/sprites/spikes.4bpp");
static const u8 sSpriteGfx_ToxicSpikes[] = INCBIN_U8("graphics/rogue_battlehud/sprites/toxic_spikes.4bpp");
static const u8 sSpriteGfx_StealthRock[] = INCBIN_U8("graphics/rogue_battlehud/sprites/stealth_rock.4bpp");

static const u8 sSpriteGfx_RainDrops[] = INCBIN_U8("graphics/rogue_battlehud/sprites/rain_drops.4bpp");
static const u8 sSpriteGfx_Sunlight[] = INCBIN_U8("graphics/rogue_battlehud/sprites/sunlight.4bpp");
static const u8 sSpriteGfx_Snowflakes[] = INCBIN_U8("graphics/rogue_battlehud/sprites/snowflakes.4bpp");
static const u8 sSpriteGfx_Sandstorm[] = INCBIN_U8("graphics/rogue_battlehud/sprites/sandstorm.4bpp");

static const u16 sSpritePal_0[] = INCBIN_U16("graphics/rogue_battlehud/palettes/pal0.gbapal");
static const u16 sSpritePal_1[] = INCBIN_U16("graphics/rogue_battlehud/palettes/pal1.gbapal");


static const struct SpriteSheet sSpriteSheet_Overlay[] =
{
    { sSpriteGfx_DexPrompt, sizeof(sSpriteGfx_DexPrompt), HUD_TAG_SPRITE_DEX_PROMPT },
    { sSpriteGfx_BlueLightWall, sizeof(sSpriteGfx_BlueLightWall), HUD_TAG_SPRITE_BLUE_LIGHT_WALL },
    { sSpriteGfx_GreenLightWall, sizeof(sSpriteGfx_GreenLightWall), HUD_TAG_SPRITE_GREEN_LIGHT_WALL },
    { sSpriteGfx_Whirlwind, sizeof(sSpriteGfx_Whirlwind), HUD_TAG_SPRITE_WHIRLWIND },
    { sSpriteGfx_SpiderWeb, sizeof(sSpriteGfx_Whirlwind), HUD_TAG_SPRITE_SPIDER_WEB },
    { sSpriteGfx_Spikes, sizeof(sSpriteGfx_Spikes), HUD_TAG_SPRITE_SPIKES },
    { sSpriteGfx_ToxicSpikes, sizeof(sSpriteGfx_ToxicSpikes), HUD_TAG_SPRITE_TOXIC_SPIKES },
    { sSpriteGfx_StealthRock, sizeof(sSpriteGfx_StealthRock), HUD_TAG_SPRITE_STEALTH_ROCK },
    {},
};

// Load as required, to avoid taking up too many tiles
static const struct SpriteSheet sSpriteSheet_Overlay_Rain = { sSpriteGfx_RainDrops, sizeof(sSpriteGfx_RainDrops), HUD_TAG_SPRITE_RAIN };
static const struct SpriteSheet sSpriteSheet_Overlay_Sun = { sSpriteGfx_Sunlight, sizeof(sSpriteGfx_Sunlight), HUD_TAG_SPRITE_SUN };
static const struct SpriteSheet sSpriteSheet_Overlay_Snow = { sSpriteGfx_Snowflakes, sizeof(sSpriteGfx_Snowflakes), HUD_TAG_SPRITE_SNOW };
static const struct SpriteSheet sSpriteSheet_Overlay_Sandstorm = { sSpriteGfx_Sandstorm, sizeof(sSpriteGfx_Sandstorm), HUD_TAG_SPRITE_SANDSTORM };

static const struct SpritePalette sSpritePalette_Overlay[] =
{
    { sSpritePal_0, HUD_TAG_PALETTE_0 },
    { sSpritePal_1, HUD_TAG_PALETTE_1 },
    {},
};

static const struct SpriteTemplate sDexPromptSpriteTemplate =
{
    .tileTag = HUD_TAG_SPRITE_DEX_PROMPT,
    .paletteTag = HUD_TAG_PALETTE_0,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

// Hazards
//


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

// Weather
//

static void AnimRainDrop(struct Sprite *sprite);
static void AnimRainDrop_Step(struct Sprite *sprite);
static void AnimSandstorm(struct Sprite *sprite);
static void AnimSandstorm_Step(struct Sprite *sprite);
static void AnimSunlight(struct Sprite *sprite);
static void AnimSunlight_Step(struct Sprite *sprite);
static void AnimSnowflake(struct Sprite *sprite);
static void AnimSnowflake_Step(struct Sprite *sprite);

static const union AnimCmd sAnim_RainDrop[] =
{
    ANIMCMD_FRAME(0, 2),
    ANIMCMD_FRAME(8, 2),
    ANIMCMD_FRAME(16, 2),
    ANIMCMD_FRAME(24, 6),
    ANIMCMD_FRAME(32, 2),
    ANIMCMD_FRAME(40, 2),
    ANIMCMD_FRAME(48, 2),
    //ANIMCMD_LOOP(0),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_RainDrop[] =
{
    sAnim_RainDrop,
};

static const struct SpriteTemplate sRainDropSpriteTemplate =
{
    .tileTag = HUD_TAG_SPRITE_RAIN,
    .paletteTag = HUD_TAG_PALETTE_1,
    .oam = &gOamData_AffineOff_ObjNormal_16x32,
    .anims = sAnims_RainDrop,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimRainDrop,
};

static const union AnimCmd sAnim_Sandstorm[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(8, 4),
    ANIMCMD_FRAME(16, 4),
    ANIMCMD_FRAME(24, 4),
    ANIMCMD_FRAME(32, 4),
    ANIMCMD_FRAME(40, 4),
    ANIMCMD_FRAME(48, 4),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_Sandstorm[] =
{
    sAnim_Sandstorm,
};

static const struct SpriteTemplate sSandstormSpriteTemplate =
{
    .tileTag = HUD_TAG_SPRITE_SANDSTORM,
    .paletteTag = HUD_TAG_PALETTE_1,
    .oam = &gOamData_AffineOff_ObjNormal_16x32,
    .anims = sAnims_Sandstorm,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSandstorm,
};

static const union AnimCmd sAnim_Snowflake[] =
{
    ANIMCMD_FRAME(0, 2),
    ANIMCMD_FRAME(8, 2),
    ANIMCMD_FRAME(16, 2),
    ANIMCMD_FRAME(24, 6),
    ANIMCMD_FRAME(32, 2),
    ANIMCMD_FRAME(40, 2),
    ANIMCMD_FRAME(48, 2),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_Snowflake[] =
{
    sAnim_Snowflake,
};

static const struct SpriteTemplate sSnowSpriteTemplate =
{
    .tileTag = HUD_TAG_SPRITE_SNOW,
    .paletteTag = HUD_TAG_PALETTE_1,
    .oam = &gOamData_AffineOff_ObjNormal_16x32,
    .anims = sAnims_Snowflake,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSnowflake,
};

static const union AffineAnimCmd sAffineAnim_SunlightRay[] =
{
    AFFINEANIMCMD_FRAME(0x50, 0x50, 0, 0),
    AFFINEANIMCMD_FRAME(0x2, 0x2, 10, 1),
    AFFINEANIMCMD_JUMP(1),
};

static const union AffineAnimCmd *const sAffineAnims_SunlightRay[] =
{
    sAffineAnim_SunlightRay,
};

static const struct SpriteTemplate sSunlightRaySpriteTemplate =
{
    .tileTag = HUD_TAG_SPRITE_SUN,
    .paletteTag = HUD_TAG_PALETTE_1,
    .oam = &gOamData_AffineNormal_ObjBlend_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_SunlightRay,
    .callback = AnimSunlight,
};

struct RogueBattleOverlay
{
    u8 sprites[MAX_OVERLAY_SPRITES];
    u8 spriteCount;
    u8 dexPromptSprite;
    bool8 statViewActive : 1;
};

EWRAM_DATA struct RogueBattleOverlay* gRogueBattleOverlay = NULL;


void RogueBH_CreateBattleOverlay()
{
    if(gRogueBattleOverlay == NULL)
    {
        u8 spikeCount, toxicSpikeCount;
        bool8 hasReflect, hasLightscreen, hasStealthRock, hasStickyWeb, hasTailwind;
        u8 spriteCount = 0;
        u16 weather = gBattleWeather;

        LoadSpriteSheets(sSpriteSheet_Overlay);
        LoadSpritePalettes(sSpritePalette_Overlay);

        gRogueBattleOverlay = Alloc(sizeof(struct RogueBattleOverlay));
        gRogueBattleOverlay->statViewActive = FALSE;
        gRogueBattleOverlay->dexPromptSprite = SPRITE_NONE;

#ifdef ROGUE_DEBUG
        if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_FULL_BATTLE_HUD))
        {
            switch (Random2() % 4)
            {
            case 0:
                weather = B_WEATHER_RAIN_PERMANENT;
                break;
            case 1:
                weather = B_WEATHER_SANDSTORM_PERMANENT;
                break;
            case 2:
                weather = B_WEATHER_SUN_PERMANENT;
                break;
            case 3:
                weather = B_WEATHER_SNOW;
                break;

            }
        }
#endif

        // Field
        {
            u8 i;

            if(weather & B_WEATHER_RAIN)
            {
                LoadSpriteSheet(&sSpriteSheet_Overlay_Rain);

                for(i = 0; i < 4; ++i)
                    gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sRainDropSpriteTemplate, 0, 0, 4);
            }
            else if(weather & B_WEATHER_SANDSTORM)
            {
                LoadSpriteSheet(&sSpriteSheet_Overlay_Sandstorm);

                for(i = 0; i < 8; ++i)
                    gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sSandstormSpriteTemplate, 0, 0, 4);
            }
            else if(weather & B_WEATHER_SUN)
            {
                LoadSpriteSheet(&sSpriteSheet_Overlay_Sun);

                for(i = 0; i < 4; ++i)
                {
                    u8 sprite = CreateSprite(&sSunlightRaySpriteTemplate, 0, 0, 4);
                    gSprites[sprite].data[0] = i * 10;
                    
                    gRogueBattleOverlay->sprites[spriteCount++] = sprite;
                }
            }
            else if(weather & B_WEATHER_SNOW)
            {
                LoadSpriteSheet(&sSpriteSheet_Overlay_Snow);

                for(i = 0; i < 8; ++i)
                {
                    u8 sprite = CreateSprite(&sSnowSpriteTemplate, 0, 0, 4);
                    gSprites[sprite].data[0] = i * 40;
                    gSprites[sprite].data[1] = 10 + (Random2() % 40);
                    
                    gRogueBattleOverlay->sprites[spriteCount++] = sprite;
                }
            }
        }

        // Side

        // Player
        {
            spikeCount = (gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SPIKES) ? gSideTimers[B_SIDE_PLAYER].spikesAmount : 0;
            hasReflect = !!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_REFLECT);
            hasLightscreen = !!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_LIGHTSCREEN);
#ifdef ROGUE_EXPANSION
            toxicSpikeCount = (gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TOXIC_SPIKES) ? gSideTimers[B_SIDE_PLAYER].toxicSpikesAmount : 0; 
            hasStealthRock = !!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_STEALTH_ROCK);
            hasStickyWeb = !!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_STICKY_WEB);
            hasTailwind = !!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
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
                gRogueBattleOverlay->sprites[spriteCount++] = CreateSprite(&sSpiderWebSpriteTemplate, 66, 107, SUBPRIORITY_PLAYER_BELOW + 1);


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
            hasReflect = !!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_REFLECT);
            hasLightscreen = !!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_LIGHTSCREEN);
#ifdef ROGUE_EXPANSION
            toxicSpikeCount = (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TOXIC_SPIKES) ? gSideTimers[B_SIDE_OPPONENT].toxicSpikesAmount : 0; 
            hasStealthRock = !!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK);
            hasStickyWeb = !!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STICKY_WEB);
            hasTailwind = !!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TAILWIND);
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

            FreeSpriteTilesByTag(sSpriteSheet_Overlay_Rain.tag);
            FreeSpriteTilesByTag(sSpriteSheet_Overlay_Sandstorm.tag);
            FreeSpriteTilesByTag(sSpriteSheet_Overlay_Sun.tag);
            FreeSpriteTilesByTag(sSpriteSheet_Overlay_Snow.tag);

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

            // This should be safe to do? (I hope)
            ResetAffineAnimData();
        }

        FREE_AND_SET_NULL(gRogueBattleOverlay);
    }
}

void RogueBH_ToggleStatView()
{
    if(gRogueBattleOverlay != NULL)
    {
        gRogueBattleOverlay->statViewActive = !gRogueBattleOverlay->statViewActive;

        if(gRogueBattleOverlay->statViewActive)
        {
            gMultiUsePlayerCursor = MAX_BATTLERS_COUNT;
        }
    }
}

bool8 RogueBH_IsStatViewActive()
{
    return (gRogueBattleOverlay != NULL && gRogueBattleOverlay->statViewActive);
}

static u8 const sPositionCycleOrder[] =
{
    B_POSITION_PLAYER_LEFT,
    B_POSITION_PLAYER_RIGHT,
    B_POSITION_OPPONENT_RIGHT,
    B_POSITION_OPPONENT_LEFT,
};

static s32 CyclePosition(s32 pos, s32 inc)
{
    s32 i;

    for(i = 0; i < ARRAY_COUNT(sPositionCycleOrder); ++i)
    {
        if(sPositionCycleOrder[i] == pos)
        {
            break;
        }
    }

    i = (i + inc) % ARRAY_COUNT(sPositionCycleOrder);
    if(i < 0)
        i += ARRAY_COUNT(sPositionCycleOrder);

    return sPositionCycleOrder[i];
}

#ifndef ROGUE_EXPANSION
#define SpriteCB_HideAsMoveTarget SpriteCb_HideAsMoveTarget
#define SpriteCB_ShowAsMoveTarget SpriteCb_ShowAsMoveTarget
#endif

void RogueBH_HandleStatViewUpdate(u32 battler)
{
    u8 prevCursorPos = gMultiUsePlayerCursor;

    if(gRogueBattleOverlay->dexPromptSprite == SPRITE_NONE)
    {
        gRogueBattleOverlay->dexPromptSprite = CreateSprite(&sDexPromptSpriteTemplate, 84, 108, SUBPRIORITY_PLAYER_ABOVE);
    }


    if(gMultiUsePlayerCursor == MAX_BATTLERS_COUNT)
    {
        gMultiUsePlayerCursor = battler;
    }

    if (JOY_NEW(SELECT_BUTTON | B_BUTTON))
    {
        PlaySE(SE_WIN_OPEN);
        RogueBH_ToggleStatView();

        if(gRogueBattleOverlay->dexPromptSprite != SPRITE_NONE)
        {
            DestroySprite(&gSprites[gRogueBattleOverlay->dexPromptSprite]);
            gRogueBattleOverlay->dexPromptSprite = SPRITE_NONE;
        }

        gSprites[gBattlerSpriteIds[gMultiUsePlayerCursor]].callback = SpriteCB_HideAsMoveTarget;
        return;
    }
    else if (JOY_NEW(DPAD_ANY))
    {
        PlaySE(SE_SELECT);

        s32 pos = GetBattlerPosition(gMultiUsePlayerCursor);

        do
        {
            pos = CyclePosition(pos, JOY_NEW(DPAD_RIGHT | DPAD_UP) ? 1 : -1);

            gMultiUsePlayerCursor = GetBattlerAtPosition(pos);
        } 
        while (gBattleMons[gMultiUsePlayerCursor].hp == 0 || gBattleMons[gMultiUsePlayerCursor].species == SPECIES_NONE);
    }

    if(prevCursorPos != gMultiUsePlayerCursor)
    {
        gSprites[gBattlerSpriteIds[gMultiUsePlayerCursor]].callback = SpriteCB_ShowAsMoveTarget;

        if(prevCursorPos < MAX_BATTLERS_COUNT)
            gSprites[gBattlerSpriteIds[prevCursorPos]].callback = SpriteCB_HideAsMoveTarget;

        RogueBH_PrintStatView();
    }
}

static const u8 sText_ClearColumn1[] = _("{CLEAR_TO 48}"); // 54 - 6
static const u8 sText_ClearColumn2[] = _("{CLEAR_TO 108}");
static const u8 sText_ClearColumn3[] = _("{CLEAR_TO 162}");

static const u8 sText_StatPrefix[] = _("{FONT_NORMAL}");
static const u8 sText_StatUp[] = _("{STAT_UP}");
static const u8 sText_StatDown[] = _("{STAT_DOWN}");
static const u8 sText_StatNone[] = _("{STAT_NONE}");

static const u8 sText_Atk[] = _("{FONT_NARROW}Atk");
static const u8 sText_SpAtk[] = _("{FONT_NARROW}SpAtk");
static const u8 sText_Def[] = _("{FONT_NARROW}Def");
static const u8 sText_SpDef[] = _("{FONT_NARROW}SpDef");
static const u8 sText_Speed[] = _("{FONT_NARROW}Spe");
static const u8 sText_Accuracy[] = _("{FONT_NARROW}Acc");
static const u8 sText_Evasion[] = _("{FONT_NARROW}Eva");

static u8* AppendBoostString(u8* ptr, s32 stages)
{
    s32 i;
    ptr = StringAppend(ptr, sText_StatPrefix);

    stages = DEFAULT_STAT_STAGE - stages;

    for(i = 0; i < 6; ++i)
    {
        if(stages < 0)
        {
            if(i < -stages)
            {
                ptr = StringAppend(ptr, sText_StatDown);
            }
            else
            {
                ptr = StringAppend(ptr, sText_StatNone);
            }
        }
        else
        {
            if(i < stages)
            {
                ptr = StringAppend(ptr, sText_StatUp);
            }
            else
            {
                ptr = StringAppend(ptr, sText_StatNone);
            }
        }
    }

    return ptr;
}

void RogueBH_PrintStatView()
{
    if(gRogueBattleOverlay != NULL && gMultiUsePlayerCursor < MAX_BATTLERS_COUNT)
    {
        u8* ptr = gStringVar4;
        struct BattlePokemon* mon = &gBattleMons[gMultiUsePlayerCursor];
        ptr = StringCopy(ptr, gText_EmptyString3);

        ptr = StringAppend(ptr, sText_Atk);
        ptr = AppendBoostString(ptr, mon->statStages[STAT_ATK]);

        ptr = StringAppend(ptr, sText_ClearColumn1);
        ptr = StringAppend(ptr, sText_SpAtk);
        ptr = AppendBoostString(ptr, mon->statStages[STAT_SPATK]);

        //

        ptr = StringAppend(ptr, sText_ClearColumn3);
        ptr = StringAppend(ptr, sText_Accuracy);
        ptr = AppendBoostString(ptr, mon->statStages[STAT_ACC]);

        // new line

        ptr = StringAppend(ptr, gText_NewLine);
        ptr = StringAppend(ptr, sText_Def);
        ptr = AppendBoostString(ptr, mon->statStages[STAT_DEF]);

        ptr = StringAppend(ptr, sText_ClearColumn1);
        ptr = StringAppend(ptr, sText_SpDef);
        ptr = AppendBoostString(ptr, mon->statStages[STAT_SPDEF]);

        ptr = StringAppend(ptr, sText_ClearColumn2);
        ptr = StringAppend(ptr, sText_Speed);
        ptr = AppendBoostString(ptr, mon->statStages[STAT_SPEED]);

        ptr = StringAppend(ptr, sText_ClearColumn3);
        ptr = StringAppend(ptr, sText_Evasion);
        ptr = AppendBoostString(ptr, mon->statStages[STAT_EVASION]);

        BattlePutTextOnWindow(gStringVar4, B_WIN_MSG);
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

static void AnimRainDrop(struct Sprite *sprite)
{
    SeekSpriteAnim(sprite, 0);

    sprite->invisible = FALSE;
    sprite->data[0] = 0;
    sprite->data[1] = (Random2() % 10);
    sprite->x = Random2() % DISPLAY_WIDTH;
    sprite->y = Random2() % (DISPLAY_HEIGHT / 2);
    sprite->x2 = 0;
    sprite->y2 = 0;

    sprite->callback = AnimRainDrop_Step;
}

static void AnimRainDrop_Step(struct Sprite *sprite)
{
    if (++sprite->data[0] <= 13)
    {
        sprite->x2++;
        sprite->y2 += 4;
    }
    
    if(sprite->animEnded)
    {
        sprite->invisible = TRUE;
        --sprite->data[1];

        if(sprite->data[1] <= 0)
        {
            // Reset
            sprite->callback = AnimRainDrop;
        }
    }
}
static void AnimSandstorm(struct Sprite *sprite)
{
    SeekSpriteAnim(sprite, 0);

    sprite->invisible = FALSE;
    sprite->data[0] = 0;
    sprite->data[1] = (Random2() % 10);
    if(sprite->data[3] != 1)
    {
        // Random initial position
        sprite->data[3] = 1;
        sprite->x = Random2() % DISPLAY_WIDTH;
    }
    else
    {
        // Wrap around the edge
        sprite->x = 0;
    }
    sprite->y = Random2() % ((DISPLAY_HEIGHT * 3) / 4);
    sprite->x2 = 0;
    sprite->y2 = 0;

    sprite->callback = AnimSandstorm_Step;
}


static void AnimSandstorm_Step(struct Sprite *sprite)
{
    sprite->x2 += 10;
    sprite->data[2] = !sprite->data[2];

    if(sprite->data[2])
        sprite->y2 += 1;
    
    if(sprite->animEnded)
    {
        sprite->invisible = TRUE;
        --sprite->data[1];

        if(sprite->data[1] <= 0)
        {
            // Reset
            sprite->callback = AnimSandstorm;
        }
    }
}

static void AnimSunlight_Reset(struct Sprite *sprite)
{
    // Move offscreen
    sprite->x = DISPLAY_WIDTH + 64;

    sprite->data[0] = 180;
    sprite->invisible = TRUE;
    sprite->callback = AnimSunlight;
}

static void AnimSunlight(struct Sprite *sprite)
{
    sprite->invisible = TRUE;

    if(sprite->data[0]-- < 0)
    {
        sprite->invisible = FALSE;
        sprite->callback = AnimSunlight_Step;
    }
}

static void AnimSunlight_Step(struct Sprite *sprite)
{
    StartSpriteAffineAnim(sprite, 0);

    sprite->x = 0;
    sprite->y = 0;
    sprite->x2 = 0;
    sprite->y2 = 0;
    sprite->data[0] = 60;
    sprite->data[2] = 140;
    sprite->data[4] = 80;
    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, AnimSunlight_Reset);
}

static void AnimSnowflake(struct Sprite *sprite)
{
    sprite->invisible = TRUE;
    --sprite->data[1];

    if(sprite->data[1] <= 0)
    {
        SeekSpriteAnim(sprite, 0);
        StartSpriteAnim(sprite, 0);

        sprite->invisible = FALSE;
        sprite->data[0] = 0;
        sprite->data[1] = 10 + (Random2() % 40);
        sprite->x = Random2() % DISPLAY_WIDTH;
        sprite->y = 0;//Random2() % (DISPLAY_HEIGHT / 2);
        sprite->x2 = 0;
        sprite->y2 = 0;

        sprite->callback = AnimSnowflake_Step;
    }
}

static void AnimSnowflake_Step(struct Sprite *sprite)
{    
    if (sprite->y + sprite->y2 >= DISPLAY_HEIGHT)
    {
        sprite->invisible = TRUE;

        // Reset
        sprite->callback = AnimSnowflake;
    }
    else
    {
        sprite->x2++;
        sprite->y2 += 3;
    }
}