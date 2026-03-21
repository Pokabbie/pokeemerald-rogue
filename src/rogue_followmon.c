#include "global.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/layouts.h"
#include "constants/map_types.h"
#include "constants/rogue.h"
#include "constants/songs.h"
#include "constants/weather.h"

#include "event_data.h"
#include "event_object_movement.h"
#include "fieldmap.h"
#include "field_weather.h"
#include "field_player_avatar.h"
#include "follow_me.h"
#include "metatile_behavior.h"
#include "pokemon.h"
#include "safari_zone.h"
#include "sound.h"
#include "random.h"
#include "script.h"

#include "rogue.h"
#include "rogue_adventurepaths.h"
#include "rogue_baked.h"
#include "rogue_controller.h"
#include "rogue_followmon.h"
#include "rogue_ridemon.h"
#include "rogue_multiplayer.h"
#include "rogue_popup.h"
#include "rogue_safari.h"
#include "rogue_settings.h"

// Care with increasing FOLLOWMON_MAX_SPAWN_SLOTS as it can cause lag

#define INVALID_SPAWN_SLOT 0xFF

struct FollowMonData
{
    bool8 pendingInterction;
    u8 activeCount;
    u8 encounterChainCount;
    u16 spawnCountdown;
    u16 spawnSlot;
    u16 pendingSpawnAnim;
    u16 encounterChainSpecies;
    u16 cachedPartnerMonGfx;
};

static EWRAM_DATA struct FollowMonData sFollowMonData = { 0 };

extern const struct RogueFollowMonGraphicsInfo gFollowMonGraphicsInfo[NUM_SPECIES];

static u16 MonSpeciesToFollowSpecies(u16 species, bool8 isShiny)
{
    if(species == SPECIES_NONE)
        return SPECIES_NONE;

#ifdef ROGUE_EXPANSION
    // Force special cases to reuse existing ones
    if(species >= SPECIES_PIKACHU_COSPLAY && species <= SPECIES_PIKACHU_LIBRE)
    {
        species = SPECIES_PIKACHU;
    }

    // We have partner and original cap overworld sprites
    if(species >= SPECIES_PIKACHU_HOENN_CAP && species <= SPECIES_PIKACHU_WORLD_CAP)
    {
        species = SPECIES_PIKACHU_PARTNER_CAP;
    }

    if(species >= SPECIES_ARCEUS_FIGHTING && species <= SPECIES_ARCEUS_FAIRY)
    {
        species = SPECIES_ARCEUS;
    }

    if(species >= SPECIES_GENESECT_DOUSE_DRIVE && species <= SPECIES_GENESECT_CHILL_DRIVE)
    {
        species = SPECIES_GENESECT;
    }

    if(species >= SPECIES_VIVILLON_POLAR && species <= SPECIES_VIVILLON_POKE_BALL)
    {
        species = SPECIES_VIVILLON;
    }

    if(species >= SPECIES_FLABEBE_YELLOW_FLOWER && species <= SPECIES_FLABEBE_WHITE_FLOWER)
    {
        species = SPECIES_FLABEBE;
    }

    if(species >= SPECIES_FLOETTE_YELLOW_FLOWER && species <= SPECIES_FLOETTE_ETERNAL_FLOWER)
    {
        species = SPECIES_FLOETTE;
    }

    if(species >= SPECIES_FLORGES_YELLOW_FLOWER && species <= SPECIES_FLORGES_WHITE_FLOWER)
    {
        species = SPECIES_FLORGES;
    }

    if(species >= SPECIES_FURFROU_HEART_TRIM && species <= SPECIES_FURFROU_PHARAOH_TRIM)
    {
        species = SPECIES_FURFROU;
    }

    if(species >= SPECIES_PUMPKABOO_SMALL && species <= SPECIES_PUMPKABOO_SUPER)
    {
        species = SPECIES_PUMPKABOO;
    }

    if(species >= SPECIES_GOURGEIST_SMALL && species <= SPECIES_GOURGEIST_SUPER)
    {
        species = SPECIES_GOURGEIST;
    }

    if(species >= SPECIES_SILVALLY_FIGHTING && species <= SPECIES_SILVALLY_FAIRY)
    {
        species = SPECIES_SILVALLY;
    }

    if(species >= SPECIES_MINIOR_METEOR_ORANGE && species <= SPECIES_MINIOR_CORE_VIOLET)
    {
        species = SPECIES_MINIOR;
    }

    if((species >= SPECIES_ALCREMIE_RUBY_CREAM && species <= SPECIES_ALCREMIE_STRAWBERRY_RAINBOW_SWIRL) || (species >= SPECIES_ALCREMIE_BERRY && species <= SPECIES_ALCREMIE_RIBBON_RAINBOW_SWIRL))
    {
        species = SPECIES_ALCREMIE;
    }

    switch (species)
    {
    case SPECIES_PICHU_SPIKY_EARED:
        species = SPECIES_PICHU;
        break;

    case SPECIES_DARMANITAN_ZEN_MODE: // TODO
        species = SPECIES_DARMANITAN;
        break;

    case SPECIES_TORNADUS_THERIAN:
        species = SPECIES_TORNADUS;
        break;

    case SPECIES_THUNDURUS_THERIAN: // TODO
        species = SPECIES_THUNDURUS;
        break;

    case SPECIES_LANDORUS_THERIAN: // TODO
        species = SPECIES_LANDORUS;
        break;

    case SPECIES_KYUREM_WHITE: // TODO
        species = SPECIES_KYUREM;
        break;

    case SPECIES_KYUREM_BLACK: // TODO
        species = SPECIES_KYUREM;
        break;

    case SPECIES_KELDEO_RESOLUTE:
        species = SPECIES_KELDEO;
        break;

    case SPECIES_GRENINJA_BATTLE_BOND:
    case SPECIES_GRENINJA_ASH:
        species = SPECIES_GRENINJA;
        break;

    case SPECIES_XERNEAS_ACTIVE:
        species = SPECIES_XERNEAS;
        break;

    case SPECIES_ZYGARDE_10:
        species = SPECIES_ZYGARDE_10_POWER_CONSTRUCT;
        break;

    case SPECIES_ZYGARDE_50_POWER_CONSTRUCT:
        species = SPECIES_ZYGARDE;
        break;

    case SPECIES_ROCKRUFF_OWN_TEMPO:
        species = SPECIES_ROCKRUFF;
        break;

    case SPECIES_WISHIWASHI_SCHOOL:
        species = SPECIES_WISHIWASHI;
        break;

    case SPECIES_MAGEARNA_ORIGINAL_COLOR:
        species = SPECIES_MAGEARNA;
        break;

    case SPECIES_SINISTEA_ANTIQUE:
        species = SPECIES_SINISTEA;
        break;

    case SPECIES_POLTEAGEIST_ANTIQUE:
        species = SPECIES_POLTEAGEIST;
        break;

    case SPECIES_ETERNATUS_ETERNAMAX:
        species = SPECIES_ETERNATUS;
        break;

    case SPECIES_ZARUDE_DADA:
        species = SPECIES_ZARUDE;
        break;

    case SPECIES_POLTCHAGEIST_ARTISAN:
        species = SPECIES_POLTCHAGEIST;
        break;

    case SPECIES_SINISTCHA_MASTERPIECE:
        species = SPECIES_SINISTCHA;
        break;
    }

#endif

    return species + (isShiny ? FOLLOWMON_SHINY_OFFSET : 0);
}

u16 FollowMon_GetMonGraphics(struct Pokemon* mon)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES);
    return MonSpeciesToFollowSpecies(species, IsMonShiny(mon));
}

u16 FollowMon_GetBoxMonGraphics(struct BoxPokemon* mon)
{
    u16 species = GetBoxMonData(mon, MON_DATA_SPECIES);
    bool8 shiny = GetBoxMonData(mon, MON_DATA_IS_SHINY);
    return MonSpeciesToFollowSpecies(species, shiny);
}

static const struct ObjectEventGraphicsInfo *GetFollowMonObjectEventInfoFromSpecies(u16 species)
{
    if(species >= FOLLOWMON_SHINY_OFFSET)
    {
        species -= FOLLOWMON_SHINY_OFFSET;

        // Return the shiny gfx if we have one
        //if(species < NUM_SPECIES && gObjectEventShinyMonGraphicsInfoPointers[species])
        //    return gObjectEventShinyMonGraphicsInfoPointers[species];
    }

    // Return the normal gfx if we have one
    if(species < NUM_SPECIES && gFollowMonGraphicsInfo[species].objectEventGfxInfo)
        return gFollowMonGraphicsInfo[species].objectEventGfxInfo;

    // Return a fallback sprite
    return gFollowMonGraphicsInfo[SPECIES_NONE].objectEventGfxInfo;
}

const struct ObjectEventGraphicsInfo *GetFollowMonObjectEventInfo(u16 graphicsId)
{
    u16 species;

    if(graphicsId >= OBJ_EVENT_GFX_FOLLOW_MON_0 && graphicsId <= OBJ_EVENT_GFX_FOLLOW_MON_LAST)
    {
        u16 varNo = graphicsId - OBJ_EVENT_GFX_FOLLOW_MON_0;
        species = VarGet(VAR_FOLLOW_MON_0 + varNo);
    }
    else // OBJ_EVENT_GFX_FOLLOW_MON_PARTNER
    {
        species = FollowMon_GetPartnerFollowSpecies(TRUE);
    }

    return GetFollowMonObjectEventInfoFromSpecies(species);
}

void SetupFollowParterMonObjectEvent()
{
    bool8 shouldFollowMonBeVisible = FlagGet(FLAG_SYS_SHOW_POKE_FOLLOWER) && !FlagGet(FLAG_SCRIPT_HIDE_FOLLOWER);

    if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_HIDE_FOLLOWER))
        shouldFollowMonBeVisible = FALSE;

    if(shouldFollowMonBeVisible && FollowMon_GetPartnerFollowSpecies(TRUE) == SPECIES_NONE)
        shouldFollowMonBeVisible = FALSE;

    if(shouldFollowMonBeVisible && RogueAdv_IsViewingPath())
        shouldFollowMonBeVisible = FALSE;

    if(shouldFollowMonBeVisible && Rogue_IsCatchingContestActive())
        shouldFollowMonBeVisible = FALSE;

    // Don't show if on bike, surfing or riding mon
    if(shouldFollowMonBeVisible && (gPlayerAvatar.flags & (PLAYER_AVATAR_FLAG_MACH_BIKE | PLAYER_AVATAR_FLAG_ACRO_BIKE | PLAYER_AVATAR_FLAG_SURFING | PLAYER_AVATAR_FLAG_UNDERWATER | PLAYER_AVATAR_FLAG_RIDING)))
        shouldFollowMonBeVisible = FALSE;

    // Hide whilst decorating
    if(shouldFollowMonBeVisible && VarGet(VAR_ROGUE_SPECIAL_MODE) == ROGUE_SPECIAL_MODE_DECORATING)
        shouldFollowMonBeVisible = FALSE;

    if(shouldFollowMonBeVisible)
    {
        if(!FollowMon_IsPartnerMonActive())
        {
            u16 localId = OBJ_EVENT_ID_FOLLOWER;
            //u8 objectEventId = 
            SpawnSpecialObjectEventParameterized(
                OBJ_EVENT_GFX_FOLLOW_MON_PARTNER,
                MOVEMENT_TYPE_FACE_DOWN,
                localId,
                gSaveBlock1Ptr->pos.x + MAP_OFFSET,
                gSaveBlock1Ptr->pos.y + MAP_OFFSET,
                3
            );

            SetUpFollowerSprite(localId, FOLLOWER_FLAG_CAN_LEAVE_ROUTE | FOLLOWER_FLAG_FOLLOW_DURING_SCRIPT); // Check FOLLOWER_FLAG_ALL
            gSaveBlock2Ptr->follower.scriptId = FOLLOW_SCRIPT_ID_FOLLOW_MON;
        }
    }
    else
    {
        ResetFollowParterMonObjectEvent();
    }
}

void ResetFollowParterMonObjectEvent()
{
    if(FollowMon_IsPartnerMonActive())
        DestroyFollower();
}

void UpdateWildEncounterChain(u16 species)
{
    if(sFollowMonData.encounterChainSpecies != species)
    {
        if(sFollowMonData.encounterChainCount > 3)
            Rogue_PushPopup_PokemonChainBroke(sFollowMonData.encounterChainSpecies);

        sFollowMonData.encounterChainSpecies = species;
        sFollowMonData.encounterChainCount = 0;
    }

    if(species != SPECIES_NONE)
    {
        if(sFollowMonData.encounterChainCount < 255)
            ++sFollowMonData.encounterChainCount;

        if(sFollowMonData.encounterChainCount >= 3)
            Rogue_PushPopup_PokemonChain(species, sFollowMonData.encounterChainCount);
    }
}

u16 GetWildChainSpecies()
{
    return sFollowMonData.encounterChainSpecies;
}

u8 GetWildChainCount()
{
    return sFollowMonData.encounterChainCount;
}

void FollowMon_SetGraphics(u16 id, u16 species, bool8 isShiny)
{
    u16 gfxSpecies = MonSpeciesToFollowSpecies(species, isShiny);
    VarSet(VAR_FOLLOW_MON_0 + id, gfxSpecies);
}

void FollowMon_SetGraphicsRaw(u16 id, u16 gfxSpecies)
{
    VarSet(VAR_FOLLOW_MON_0 + id, gfxSpecies);
}

void FollowMon_SetGraphicsFromMon(u16 id, struct Pokemon* mon)
{
    u16 gfxSpecies = FollowMon_GetMonGraphics(mon);
    VarSet(VAR_FOLLOW_MON_0 + id, gfxSpecies);
}

void FollowMon_SetGraphicsFromParty()
{
    u8 i;

    for(i = 0; i < PARTY_SIZE; ++i)
    {
        if(i < gPlayerPartyCount)
            FollowMon_SetGraphicsFromMon(i, &gPlayerParty[i]);
        else
            FollowMon_SetGraphics(i, SPECIES_NONE, FALSE);
    }
}

u16 FollowMon_GetGraphics(u16 id)
{
    return VarGet(VAR_FOLLOW_MON_0 + id);
}

u16 const* FollowMon_GetGraphicsForPalSlot(u16 palSlot)
{
    u16 species = SPECIES_NONE;

    if(palSlot == 0)
    {
        // Partner
        species = FollowMon_GetPartnerFollowSpecies(TRUE);
    }
    else
    {
        // Base of graphics slot
        species = FollowMon_GetGraphics(palSlot - 1);
    }

    if(species >= FOLLOWMON_SHINY_OFFSET)
    {
        species -= FOLLOWMON_SHINY_OFFSET;

        if(gFollowMonGraphicsInfo[species].shinyPal != NULL)
            return gFollowMonGraphicsInfo[species].shinyPal;
    }

    return gFollowMonGraphicsInfo[species].normalPal;
}

bool8 FollowMon_IsLargeGfx(u16 gfxSpecies)
{
    return GetFollowMonObjectEventInfoFromSpecies(gfxSpecies)->size > 512;
}

bool8 FollowMon_IsPartnerMonActive()
{
    // TODO - If we use other partners, gonna have to check object too
    return PlayerHasFollower();
}

u16 GetPartnerFollowSpeciesInternal()
{
    u16 species;

    if(Rogue_IsRideActive())
        species = Rogue_GetRideMonSpeciesGfx(0);
    else
    {
        u8 i;
        species = SPECIES_NONE;

        for(i = 0; i < gPlayerPartyCount; ++i)
        {
            if(!GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG) && GetMonData(&gPlayerParty[i], MON_DATA_HP))
            {
                species = FollowMon_GetMonGraphics(&gPlayerParty[i]);
                break;
            }
        }
    }

    return species;
}

u16 FollowMon_GetPartnerFollowSpecies(bool8 includeShinyOffset)
{
    u16 species;

    if(sFollowMonData.cachedPartnerMonGfx == NUM_SPECIES)
        sFollowMonData.cachedPartnerMonGfx = GetPartnerFollowSpeciesInternal();

    species = sFollowMonData.cachedPartnerMonGfx;

    if(!includeShinyOffset && species >= FOLLOWMON_SHINY_OFFSET)
        species -= FOLLOWMON_SHINY_OFFSET;

    return species;
}

void FollowMon_ClearCachedPartnerSpecies()
{
    sFollowMonData.cachedPartnerMonGfx = NUM_SPECIES;
}

bool8 FollowMon_IsMonObject(struct ObjectEvent* object, bool8 ignorePartnerMon)
{
    u16 localId = object->localId;
    u16 graphicsId = object->graphicsId;

    if(localId >= OBJ_EVENT_ID_FOLLOW_MON_FIRST && localId <= OBJ_EVENT_ID_FOLLOW_MON_LAST)
    {
        // Fast check
        return TRUE;
    }

    // Check gfx id
    if(graphicsId >= OBJ_EVENT_GFX_VAR_FIRST && graphicsId <= OBJ_EVENT_GFX_VAR_LAST)
    {
        graphicsId = VarGet(VAR_OBJ_GFX_ID_0 + (object->graphicsId - OBJ_EVENT_GFX_VAR_FIRST));
    }

    if(object->graphicsId >= OBJ_EVENT_GFX_FOLLOW_MON_FIRST && object->graphicsId <= OBJ_EVENT_GFX_FOLLOW_MON_LAST)
    {
        if(!ignorePartnerMon || graphicsId != OBJ_EVENT_GFX_FOLLOW_MON_PARTNER )
            return TRUE;
    }

    return FALSE;
}

static bool8 IsExpensiveWeatherActive()
{
    switch (GetSavedWeather())
    {
    case WEATHER_RAIN:
    //case WEATHER_SNOW:
    case WEATHER_RAIN_THUNDERSTORM:
    case WEATHER_FOG_HORIZONTAL:
    case WEATHER_VOLCANIC_ASH:
    case WEATHER_SANDSTORM:
    case WEATHER_FOG_DIAGONAL:
    case WEATHER_DOWNPOUR:
    case WEATHER_LEAVES:
        return TRUE;
    }

    return FALSE;
}

bool8 FollowMon_ShouldAlwaysAnimation(struct ObjectEvent *objectEvent)
{
    return TRUE;

    //if(Rogue_InWildSafari())
    //{
    //    // Don't animate in safari
    //    return FALSE;
    //}
//
    //if(objectEvent->graphicsId == OBJ_EVENT_GFX_FOLLOW_MON_PARTNER && !IsExpensiveWeatherActive())
    //{
    //    return TRUE;
    //}
//
    //if(Rogue_IsRunActive())
    //{
    //    // TODO - Enabled for legendaries
    //}
//
    //return FALSE;
}

bool8 FollowMon_ShouldAnimationGrass(struct ObjectEvent *objectEvent)
{
    if(Rogue_AreWildMonEnabled())
    {
        // Weather is laggy, so turn off grass entirely for these
        if(IsExpensiveWeatherActive())
            return FALSE;

        // Turn off animated grass when wild mons are active because it's pretty laggy and causes palette issues
        if(objectEvent == &gObjectEvents[gPlayerAvatar.objectEventId])
            return TRUE; // We will only keep on grass animation for the player

        return FALSE;
    }
    else
    {
        // Follow mons never animate grass
        return !FollowMon_IsMonObject(objectEvent, FALSE);
    }
}

static bool8 AreElevationsCompatible(u8 a, u8 b)
{
    if (a == 0 || b == 0)
        return TRUE;

    if (a != b)
        return FALSE;

    return TRUE;
}

bool8 FollowMon_IsCollisionExempt(struct ObjectEvent* obstacle, struct ObjectEvent* collider)
{
    struct ObjectEvent* player = &gObjectEvents[gPlayerAvatar.objectEventId];
    
    // Disable collision exemption for tutorial
    //if(Rogue_InWildSafari() && VarGet(VAR_ROGUE_INTRO_STATE) == ROGUE_INTRO_STATE_CATCH_MON)
    //    return FALSE;

    if(Rogue_IsRunActive() || Rogue_InWildSafari() || GetSafariZoneFlag())
    {
        if (collider == player)
        {
            // Player can walk on top of follow mon
            if(FollowMon_IsMonObject(obstacle, TRUE))
            {
                sFollowMonData.pendingInterction = TRUE;
                return TRUE;
            }
        }
        else if(obstacle == player)
        {
            // Follow mon can walk onto player
            if(FollowMon_IsMonObject(collider, TRUE))
            {
                sFollowMonData.pendingInterction = TRUE;
                return TRUE;
            }
        }
        else if(Rogue_AreWildMonEnabled())
        {
            if(!FollowMon_IsMonObject(collider, TRUE) && FollowMon_IsMonObject(obstacle, FALSE))
            {
                // Other objects can walk through follow mons, whilst wandering mons is active
                return TRUE;
            }
        }
    }

    return FALSE;
}

bool8 FollowMon_ProcessMonInteraction()
{
    if(Rogue_AreWildMonEnabled() && VarGet(VAR_REPEL_STEP_COUNT) != 0)
    {
        // Never auto trigger battle whilst repel is active
        sFollowMonData.pendingInterction = FALSE;
        return FALSE;
    }

    if(sFollowMonData.pendingInterction)
    {
        u8 i;
        struct ObjectEvent *curObject;
        struct ObjectEvent* player = &gObjectEvents[gPlayerAvatar.objectEventId];
    
        sFollowMonData.pendingInterction = FALSE;
        
        for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
        {
            curObject = &gObjectEvents[i];
            if (curObject->active && curObject != player && FollowMon_IsMonObject(curObject, TRUE))
            {
                if ((curObject->currentCoords.x == player->currentCoords.x && curObject->currentCoords.y == player->currentCoords.y) || (curObject->previousCoords.x == player->currentCoords.x && curObject->previousCoords.y == player->currentCoords.y))
                {
                    if (AreElevationsCompatible(curObject->currentElevation, player->currentElevation))
                    {
                        // There is a valid collision so exectute the attached script
                        const u8* script = GetObjectEventScriptPointerByObjectEventId(i);

                        if(script != NULL)
                        {
                            VarSet(VAR_LAST_TALKED, curObject->localId);
                            ScriptContext_SetupScript(script);
                            return TRUE;
                        }
                    }
                }
            }
        }
    }

    return FALSE;
}

void FollowMon_GetSpeciesFromLastInteracted(u16* species, bool8* isShiny, u8* spawnSlot)
{
    struct ObjectEvent *curObject;
    u8 lastTalkedId = VarGet(VAR_LAST_TALKED);

    if(lastTalkedId == OBJ_EVENT_ID_FOLLOWER)
    {
        u16 gfxSpecies = FollowMon_GetPartnerFollowSpecies(TRUE);

        if(gfxSpecies >= FOLLOWMON_SHINY_OFFSET)
        {
            *spawnSlot = INVALID_SPAWN_SLOT;
            *species = gfxSpecies - FOLLOWMON_SHINY_OFFSET;
            *isShiny = TRUE;
        }
        else
        {
            *spawnSlot = INVALID_SPAWN_SLOT;
            *species = gfxSpecies;
            *isShiny = FALSE;
        }
    }
    else
    {
        u8 objEventId = GetObjectEventIdByLocalIdAndMap(lastTalkedId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);

        *species = FALSE;
        *isShiny = FALSE;

        if(objEventId < OBJECT_EVENTS_COUNT)
        {
            curObject = &gObjectEvents[objEventId];
            if(FollowMon_IsMonObject(curObject, TRUE))
            {
                u16 varNo = curObject->graphicsId - OBJ_EVENT_GFX_FOLLOW_MON_0;
                u16 gfxSpecies = VarGet(VAR_FOLLOW_MON_0 + varNo);

                if(gfxSpecies >= FOLLOWMON_SHINY_OFFSET)
                {
                    *spawnSlot = varNo;
                    *species = gfxSpecies - FOLLOWMON_SHINY_OFFSET;
                    *isShiny = TRUE;
                }
                else
                {
                    *spawnSlot = varNo;
                    *species = gfxSpecies;
                    *isShiny = FALSE;
                }
            }
        }
    }
}

static u8 FindObjectEventForGfx(u16 gfxId);

static bool8 IsSpawningWaterMons()
{
    struct ObjectEvent* player = &gObjectEvents[gPlayerAvatar.objectEventId];
    u16 tileBehavior = MapGridGetMetatileBehaviorAt(player->currentCoords.x, player->currentCoords.y);
    return ((Rogue_IsRideMonSwimming() || (gPlayerAvatar.flags & (PLAYER_AVATAR_FLAG_SURFING | PLAYER_AVATAR_FLAG_UNDERWATER))) && !MetatileBehavior_IsBridgeOverWater(tileBehavior));
}

static u8 CountActiveObjectEvents()
{
    u8 i;
    u8 count = 0;

    for(i = 0; i < OBJECT_EVENTS_COUNT; ++i)
    {
        if(gObjectEvents[i].active)
            ++count;
    }

    return count;
}

static bool8 IsSpawnSlotValid(u16 slot)
{
    // 0 : normal pal index 1
    if(slot == 0)
    {
        // Slot 0 is used for static encounters
        if(Rogue_IsRunActive())
        {
            if(
                gRogueAdvPath.currentRoomType != ADVPATH_ROOM_LEGENDARY &&
                gRogueAdvPath.currentRoomType != ADVPATH_ROOM_WILD_DEN &&
                gRogueAdvPath.currentRoomType != ADVPATH_ROOM_HONEY_TREE
            )
                return TRUE;
        }
        else
            return TRUE;
    }

    // 1 : normal pal index 2
    if(slot == 1)
    {
        return TRUE;
    }
    
    // 2 : normal pal index 3 (Shared for the multiplayer player palette)
    // 3 : normal pal index 4 (Shared for the multiplayer follower palette) <- OBJ_EVENT_GFX_MP_FOLLOW_MON
    if(slot == 2 || slot == 3)
    {
        if(!RogueMP_IsActive())
            return TRUE;
    }

    // 4 : normal pal index 10
    if(slot == 4)
    {
        if(Rogue_IsRunActive())
        {
            // Only enable if we aren't using the pal10 slot for rival
            return FlagGet(FLAG_ROGUE_RIVAL_DISABLED);
        }
        else
        {
            return TRUE;
        }
    }

    // 5 : normal pal index 1 (partner slot)
    if(slot == 5)
    {
        return !FollowMon_IsPartnerMonActive() && !Rogue_IsRideActive();
    }

    return FALSE;
}

bool8 FollowMon_IsSlotEnabled(u8 slot)
{
    return IsSpawnSlotValid(slot);
}

static u16 ActiveSpawnSlotCount()
{
    u16 slot;
    u16 count = 0;

    for(slot = 0; slot < FOLLOWMON_MAX_SPAWN_SLOTS; ++slot)
    {
        if(IsSpawnSlotValid(slot))
            ++count;
    }

    return count;
}

static u16 NextSpawnMonSlot()
{
    u16 slot;
    u16 species;
    u8 level; // ignore
    bool8 isShiny;

    species = SPECIES_NONE;
    slot = FOLLOWMON_MAX_SPAWN_SLOTS;

    // Attempt to find a free slot first
    // (Don't do this in catching contest as we want mons to disappear fairly fast)
    if(!Rogue_IsCatchingContestActive())
    {
        for(slot = 0; slot < FOLLOWMON_MAX_SPAWN_SLOTS; ++slot)
        {
            if(IsSpawnSlotValid(slot) && FindObjectEventForGfx(OBJ_EVENT_GFX_FOLLOW_MON_0 + slot) == OBJECT_EVENTS_COUNT)
                break;
        }
    }

    // All mon slots are in use
    if(slot == FOLLOWMON_MAX_SPAWN_SLOTS)
    {
        // Cycle through so we remove the oldest mon first
        sFollowMonData.spawnSlot = (sFollowMonData.spawnSlot + 1) % FOLLOWMON_MAX_SPAWN_SLOTS;
        slot = sFollowMonData.spawnSlot;
    }

    if(!IsSpawnSlotValid(slot))
        return INVALID_SPAWN_SLOT;

    // Remove any existing id by this slot
    RemoveObjectEventByLocalIdAndMap(OBJ_EVENT_ID_FOLLOW_MON_FIRST + slot, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
    
    // Check that we don't have too many sprites on screen before spawning
    // (lag reduction)
    if(sFollowMonData.activeCount != 0 && CountActiveObjectEvents() >= FOLLOWMON_IDEAL_OBJECT_EVENT_COUNT)
    {
        return INVALID_SPAWN_SLOT;
    }

    if(Rogue_InWildSafari())
    {
        struct RogueSafariMon* mon = RogueSafari_ChooseSafariMonForSlot(slot);

        if(mon != NULL)
        {
            species = Rogue_GetEggSpecies(mon->species);
            isShiny = mon->shinyFlag != 0;
        }
    }
    else
    {
        if(IsSpawningWaterMons())
            Rogue_CreateWildMon(1, &species, &level, &isShiny); // WILD_AREA_WATER
        else
            Rogue_CreateWildMon(0, &species, &level, &isShiny);

        // Player roamer cry to indicate that it has spawned
        if(species != SPECIES_NONE && gRogueRun.wildEncounters.roamer.species == species)
            PlayCry_Normal(species, 25);
    }

    if(species == SPECIES_NONE)
    {
        return INVALID_SPAWN_SLOT;
    }

    FollowMon_SetGraphics(slot, species, isShiny);
    return slot;
}

static bool8 CheckForObjectEventAtLocation(s16 x, s16 y)
{
    u8 i;
    for(i = 0; i < OBJECT_EVENTS_COUNT; ++i)
    {
        if(gObjectEvents[i].active && gObjectEvents[i].currentCoords.x == x && gObjectEvents[i].currentCoords.y == y)
        {
            return TRUE;
        }
    }

    return FALSE;
}

static bool8 TrySelectTile(s16* outX, s16* outY)
{
    u8 tryCount;
    u8 elevation;
    u16 tileBehavior;
    s16 playerX, playerY;
    s16 x, y;
    u8 closeDistance;

    for(tryCount = 0; tryCount < 3; ++tryCount)
    {
        // Spawn further away when surfing
        if(IsSpawningWaterMons())
            closeDistance = 3;
        else
            closeDistance = 1;

        // Select a random tile in [-7, -4] [7, 4] range
        // Make sure is not directly next to player
        do
        {
            x = (s16)(Random() % 15) - 7;
            y = (s16)(Random() % 9) - 4;
        }
        while (abs(x) <= closeDistance && abs(y) <= closeDistance);

        // We won't spawn mons in in the immediate facing direction
        // (stops mons spawning in as I'm running in a straight line)
        switch (GetPlayerFacingDirection())
        {
        case DIR_NORTH:
            if(x == 0 && y < 0)
                x = -1;
            break;
        case DIR_SOUTH:
            if(x == 0  && y > 0)
                x = 1;
            break;

        case DIR_EAST:
            if(y == 0 && x > 0)
                y = -1;
            break;
        case DIR_WEST:
            if(y == 0 && x < 0)
                y = 1;
            break;
        }

        
        PlayerGetDestCoords(&playerX, &playerY);
        x += playerX;
        y += playerY;

        elevation = MapGridGetElevationAt(x, y);

        // 0 is change of elevation, 15 is multiple elevation e.g. bridges
        // Causes weird interaction issues so just don't let mons spawn here
        if (elevation == 0 || elevation == 15)
            return FALSE;

        tileBehavior = MapGridGetMetatileBehaviorAt(x, y);
        if(IsSpawningWaterMons())
        {
            if(MetatileBehavior_IsWaterWildEncounter(tileBehavior) && !MapGridIsImpassableAt(x, y))
            {
                *outX = x;
                *outY = y;

                if(!CheckForObjectEventAtLocation(x, y))
                    return TRUE;
            }
        }
        else
        {
            if(MetatileBehavior_IsLandWildEncounter(tileBehavior) && !MapGridIsImpassableAt(x, y))
            {
                *outX = x;
                *outY = y;

                if(!CheckForObjectEventAtLocation(x, y))
                    return TRUE;
            }
        }
    }

    return FALSE;
}

// MovementAction_EmoteFollowMonSpawn
bool8 MovementAction_FollowMonGrassSpawn(struct ObjectEvent *objEvent);
bool8 MovementAction_FollowMonWaterSpawn(struct ObjectEvent *objEvent);
bool8 MovementAction_FollowMonCaveSpawn(struct ObjectEvent *objEvent);
bool8 MovementAction_FollowMonShinySpawn(struct ObjectEvent *objEvent);

static u8 FindObjectEventForGfx(u16 gfxId)
{
    u8 i;
    for(i = 0; i < OBJECT_EVENTS_COUNT; ++i)
    {
        if(gObjectEvents[i].active && gObjectEvents[i].graphicsId == gfxId)
        {
            return i;
        }
    }

    return OBJECT_EVENTS_COUNT;
}

static bool8 InShinyAnimationRange(u8 objectEventId)
{
    // Only play anims if within range
    if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_AREA_SAFARI_ZONE_TUTORIAL || !Rogue_AreWildMonEnabled())
    {
        // Static spawns only play anim when you're close enough to see it
        s16 x, y;
        PlayerGetDestCoords(&x, &y);

        x = gObjectEvents[objectEventId].currentCoords.x - x;
        y = gObjectEvents[objectEventId].currentCoords.y - y;

        return abs(x) <= 4 && abs(y) <= 4;
    }

    // Wild spawns instantly play animation
    return TRUE;
}

void FollowMon_OverworldCB()
{
    if(Rogue_AreWildMonEnabled())
    {
        if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_STOP_WILD_SPAWNING))
            return;


        // Speed up spawning
        if(!Rogue_IsCatchingContestActive())
        {
            if(sFollowMonData.activeCount <= 1)
            {
                // Super fast spawn for new things on screen
                sFollowMonData.spawnCountdown = min(sFollowMonData.spawnCountdown, 15);
            }
            else if(sFollowMonData.activeCount <= (ActiveSpawnSlotCount() - 1))
            {
                // Fast spawn to reach capacity
                sFollowMonData.spawnCountdown = min(sFollowMonData.spawnCountdown, 60);
            }
        }

        if(sFollowMonData.spawnCountdown == 0)
        {
            s16 x, y;

            if(IsSafeToSpawnObjectEvents() && TrySelectTile(&x, &y))
            {
                u16 spawnSlot = NextSpawnMonSlot();

                if(spawnSlot != INVALID_SPAWN_SLOT)
                {
                    u8 localId = OBJ_EVENT_ID_FOLLOW_MON_FIRST + spawnSlot;
                    u8 objectEventId = SpawnSpecialObjectEventParameterized(
                        OBJ_EVENT_GFX_FOLLOW_MON_0 + spawnSlot,
                        MOVEMENT_TYPE_WANDER_AROUND,
                        localId,
                        x,
                        y,
                        MapGridGetElevationAt(x, y)
                    );

                    gObjectEvents[objectEventId].rangeX = 8;
                    gObjectEvents[objectEventId].rangeY = 8;

                    // Hide reflections for spawns in water
                    // (It just looks weird)
                    if(IsSpawningWaterMons())
                    {
                        gObjectEvents[objectEventId].hideReflection = TRUE; 
                    }

                    // Slower replacement spawning
                    sFollowMonData.spawnCountdown = 60 * (3 + Random() % 2);
                }

                if(Rogue_IsCatchingContestActive())
                {
                    // Always use set spawn/despawn interval for catchign contest
                    sFollowMonData.spawnCountdown = 120;
                }
            }
        }
        else
        {
            --sFollowMonData.spawnCountdown;
        }
    }

    // Play spawn animation when player is close enough
    if(sFollowMonData.pendingSpawnAnim != 0)
    {
        u16 spawnSlot;
        u16 gfxId;
        u16 bitFlag;
        bool8 isShiny;
        u8 objectEventId;

        for(gfxId = OBJ_EVENT_GFX_FOLLOW_MON_0; gfxId < OBJ_EVENT_GFX_FOLLOW_MON_LAST; ++gfxId)
        {
            spawnSlot = gfxId - OBJ_EVENT_GFX_FOLLOW_MON_0;
            bitFlag = (1 << spawnSlot);

            if((sFollowMonData.pendingSpawnAnim & bitFlag) != 0)
            {
                objectEventId = FindObjectEventForGfx(gfxId);

                if(objectEventId != OBJECT_EVENTS_COUNT)
                {
                    isShiny = (VarGet(VAR_FOLLOW_MON_0 + spawnSlot) >= FOLLOWMON_SHINY_OFFSET);

                    if(isShiny)
                    {
                        // Play special shiny animation only once in range
                        if(InShinyAnimationRange(objectEventId))
                        {
                            MovementAction_FollowMonShinySpawn(&gObjectEvents[objectEventId]);
                            PlaySE(SE_SHINY);

                            sFollowMonData.pendingSpawnAnim &= ~bitFlag;
                        }
                    }
                    else 
                    {
                        // Only play anim if they are wild spawning mons
                        if(Rogue_AreWildMonEnabled())
                        {
                            // Instantly play a small animation to ground the spawning a bit
                            if(IsSpawningWaterMons())
                            {
                                MovementAction_FollowMonWaterSpawn(&gObjectEvents[objectEventId]);
                            }
                            else if(gMapHeader.cave || gMapHeader.mapType == MAP_TYPE_UNDERGROUND)
                            {
                                MovementAction_FollowMonCaveSpawn(&gObjectEvents[objectEventId]);
                            }
                            else
                            {
                                MovementAction_FollowMonGrassSpawn(&gObjectEvents[objectEventId]);
                            }
                        }

                        sFollowMonData.pendingSpawnAnim &= ~bitFlag;
                    }
                }
            }
        }
    }
}

bool8 IsSafeToSpawnObjectEvents()
{
    struct ObjectEvent* player = &gObjectEvents[gPlayerAvatar.objectEventId];

    // Only spawn when player is at a valid tile position
    return (player->currentCoords.x == player->previousCoords.x && player->currentCoords.y == player->previousCoords.y);
}

void FollowMon_OnWarp()
{
    sFollowMonData.spawnCountdown = 0;
    sFollowMonData.activeCount = 0;
}

static bool8 PlaySpawnAnims()
{
    return Rogue_IsRunActive() || Rogue_InWildSafari();
}

void FollowMon_RecountActiveObjects()
{
    u8 i;

    sFollowMonData.activeCount = 0;

    FollowMon_ClearCachedPartnerSpecies();

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        if(FollowMon_IsMonObject(&gObjectEvents[i], TRUE))
        {
            ++sFollowMonData.activeCount;
        }
    }
}

void FollowMon_OnObjectEventSpawned(struct ObjectEvent *objectEvent)
{
    if(PlaySpawnAnims())
    {
        u16 spawnSlot = objectEvent->graphicsId - OBJ_EVENT_GFX_FOLLOW_MON_0;

        if(sFollowMonData.activeCount != 255)
            ++sFollowMonData.activeCount;

        sFollowMonData.pendingSpawnAnim |= (1 << spawnSlot);
    }
}

void FollowMon_OnObjectEventRemoved(struct ObjectEvent *objectEvent)
{
    if(PlaySpawnAnims())
    {
        u16 spawnSlot = objectEvent->graphicsId - OBJ_EVENT_GFX_FOLLOW_MON_0;

        if(Rogue_InWildSafari())
            RogueSafari_RemoveMonFromSlot(spawnSlot);

        if(sFollowMonData.activeCount != 0)
            --sFollowMonData.activeCount;
    }
}