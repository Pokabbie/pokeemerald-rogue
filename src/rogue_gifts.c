#include "global.h"
#include "constants/abilities.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/rgb.h"
#include "constants/region_map_sections.h"
#include "event_data.h"
#include "random.h"

#include "rogue_baked.h"
#include "rogue_colour_utils.h"
#include "rogue_gifts.h"
#include "rogue_hub.h"
#include "rogue_pokedex.h"
#include "rogue_query.h"
#include "rogue_quest.h"
#include "rogue_save.h"

struct CustomTrainerData
{
    u8 const* name;
    u32 trainerId;
    u8 trainerColour;
};

struct CustomMonData
{
    u8 const* nickname;
    u16 const* moves;
    u16 const* abilities;
    u16 types[2];
    u32 otId;
    u16 movesCount;
    u16 customTrainerId;
    u16 species;
    u16 pokeball;
    u16 heldItem;
    u16 isShiny : 1;
    u16 isDefaultSpawn : 1;
};

static u8 const sRarityToCustomTrainerIndex[] = 
{
    [UNIQUE_RARITY_COMMON]      = CUSTOM_TRAINER_COMMON,
    [UNIQUE_RARITY_RARE]        = CUSTOM_TRAINER_RARE,
    [UNIQUE_RARITY_EPIC]        = CUSTOM_TRAINER_EPIC,
    [UNIQUE_RARITY_EXOTIC]      = CUSTOM_TRAINER_EXOTIC,
};

static u16 const sDynamicCustomMonAbilities[] = 
{
#ifdef ROGUE_EXPANSION
    ABILITY_GOOD_AS_GOLD,
    ABILITY_WATER_ABSORB,
    ABILITY_VOLT_ABSORB,
    ABILITY_FLASH_FIRE,
    ABILITY_SPEED_BOOST,
    ABILITY_ADAPTABILITY,
    ABILITY_INTIMIDATE,
    ABILITY_DRIZZLE,
    ABILITY_DROUGHT,
    ABILITY_SNOW_WARNING,
    ABILITY_SAND_STREAM,
    ABILITY_QUICK_DRAW,
    ABILITY_NEUTRALIZING_GAS,
    ABILITY_ELECTRIC_SURGE,
    ABILITY_PSYCHIC_SURGE,
    ABILITY_MISTY_SURGE,
    ABILITY_GRASSY_SURGE,
    ABILITY_PROTEAN,
    ABILITY_MAGIC_BOUNCE,
    ABILITY_MOXIE,
    ABILITY_ILLUSION,
    ABILITY_REGENERATOR,
    ABILITY_SUPER_LUCK,
    ABILITY_CONTRARY,
    ABILITY_NO_GUARD,
    ABILITY_SKILL_LINK,
    ABILITY_SHELL_ARMOR,
    ABILITY_GUTS,
    ABILITY_THICK_FAT,
    ABILITY_HUGE_POWER,
    ABILITY_SERENE_GRACE,
    ABILITY_BATTLE_ARMOR,
    ABILITY_WEAK_ARMOR,
    ABILITY_TOXIC_DEBRIS,
    ABILITY_UNSEEN_FIST,
    ABILITY_GORILLA_TACTICS,
    ABILITY_TOUGH_CLAWS,
    ABILITY_FULL_METAL_BODY,
    ABILITY_TECHNICIAN,
    ABILITY_EMERGENCY_EXIT,
    ABILITY_PRANKSTER,
    ABILITY_POWER_OF_ALCHEMY,
    ABILITY_OPPORTUNIST,
    ABILITY_LEVITATE,
    ABILITY_MULTISCALE,
    ABILITY_COSTAR,
    ABILITY_POISON_HEAL,
    ABILITY_NEUROFORCE,
    ABILITY_MOODY,
    ABILITY_HOSPITALITY,
    ABILITY_COMATOSE,
    ABILITY_BERSERK,
    ABILITY_SIMPLE,
    ABILITY_UNAWARE,
    ABILITY_HARVEST,
    ABILITY_MOLD_BREAKER,
    ABILITY_SHEER_FORCE,
    ABILITY_OVERCOAT,
    ABILITY_IRON_BARBS,
    ABILITY_DEFIANT,
    ABILITY_STALL,
    ABILITY_SNIPER,
    ABILITY_MULTITYPE
#else
    ABILITY_DRIZZLE,
    ABILITY_SPEED_BOOST,
    ABILITY_BATTLE_ARMOR,
    ABILITY_LIMBER,
    ABILITY_SAND_VEIL,
    ABILITY_STATIC,
    ABILITY_VOLT_ABSORB,
    ABILITY_WATER_ABSORB,
    ABILITY_CLOUD_NINE,
    ABILITY_COMPOUND_EYES,
    ABILITY_COLOR_CHANGE,
    ABILITY_FLASH_FIRE,
    ABILITY_SHIELD_DUST,
    ABILITY_SUCTION_CUPS,
    ABILITY_INTIMIDATE,
    ABILITY_SHADOW_TAG,
    ABILITY_ROUGH_SKIN,
    ABILITY_LEVITATE,
    ABILITY_EFFECT_SPORE,
    ABILITY_CLEAR_BODY,
    ABILITY_SERENE_GRACE,
    ABILITY_SWIFT_SWIM,
    ABILITY_CHLOROPHYLL,
    ABILITY_HUGE_POWER,
    ABILITY_INNER_FOCUS,
    ABILITY_SOUNDPROOF,
    ABILITY_RAIN_DISH,
    ABILITY_SAND_STREAM,
    ABILITY_PRESSURE,
    ABILITY_THICK_FAT,
    ABILITY_FLAME_BODY,
    ABILITY_RUN_AWAY,
    ABILITY_PICKUP,
    ABILITY_HUSTLE,
    ABILITY_CUTE_CHARM,
    ABILITY_SHED_SKIN,
    ABILITY_GUTS,
    ABILITY_MARVEL_SCALE,
    ABILITY_ROCK_HEAD,
    ABILITY_DROUGHT,
    ABILITY_WHITE_SMOKE,
    ABILITY_PURE_POWER,
    ABILITY_SHELL_ARMOR,
    ABILITY_AIR_LOCK,
#endif
};

static u16 const sDynamicCustomMonMoves[] = 
{
#ifdef ROGUE_EXPANSION
    MOVE_HYDRO_STEAM,
    MOVE_CHILLING_WATER,
    MOVE_TORCH_SONG,
    MOVE_JET_PUNCH,
    MOVE_STONE_AXE,
    MOVE_THUNDEROUS_KICK,
    MOVE_SCORCHING_SANDS,
    MOVE_SURGING_STRIKES,
    MOVE_PARTING_SHOT,
    MOVE_GRASSY_GLIDE,
    MOVE_DYNAMAX_CANNON,
    MOVE_STOMPING_TANTRUM,
    MOVE_BANEFUL_BUNKER,
    MOVE_DIAMOND_STORM,
    MOVE_PLAY_ROUGH,
    MOVE_FREEZE_DRY,
    MOVE_PHANTOM_FORCE,
    MOVE_V_CREATE,
    MOVE_SECRET_SWORD,
    MOVE_TRICK_ROOM,
    MOVE_SPORE,
    MOVE_GUNK_SHOT,
    MOVE_NASTY_PLOT,
    MOVE_CLOSE_COMBAT,
    MOVE_U_TURN,
    MOVE_DRAGON_DANCE,
    MOVE_CALM_MIND,
    MOVE_LEAF_BLADE,
    MOVE_METEOR_MASH,
    MOVE_KNOCK_OFF,
    MOVE_SKILL_SWAP,
    MOVE_FAKE_OUT,
    MOVE_EXTREME_SPEED,
    MOVE_ANCIENT_POWER,
    MOVE_RAPID_SPIN,
    MOVE_MEGAHORN,
    MOVE_WISH,
    MOVE_THUNDER,
    MOVE_SWORDS_DANCE,
    MOVE_THUNDERCLAP,
    MOVE_GIGATON_HAMMER,
    MOVE_SALT_CURE,
    MOVE_WICKED_BLOW,
    MOVE_POLTERGEIST,
    MOVE_METEOR_BEAM,
    MOVE_SPIRIT_BREAK,
    MOVE_RECOVER,
    MOVE_NO_RETREAT,
    MOVE_SUBSTITUTE,
    MOVE_METRONOME,
    MOVE_TOPSY_TURVY,
    MOVE_QUIVER_DANCE,
    MOVE_BATON_PASS,
    MOVE_MOONGEIST_BEAM,
    MOVE_MAGMA_STORM,
    MOVE_RUINATION,
    MOVE_REVELATION_DANCE,
    MOVE_SUCKER_PUNCH,
    MOVE_COLLISION_COURSE,
    MOVE_ROLLOUT,
    MOVE_FURY_CUTTER,
    MOVE_LAST_RESPECTS,
    MOVE_ACROBATICS,
#else
    MOVE_PAY_DAY,
    MOVE_FIRE_PUNCH,
    MOVE_ICE_PUNCH,
    MOVE_THUNDER_PUNCH,
    MOVE_SWORDS_DANCE,
    MOVE_FLY,
    MOVE_BODY_SLAM,
    MOVE_SURF,
    MOVE_ICE_BEAM,
    MOVE_COUNTER,
    MOVE_DRAGON_RAGE,
    MOVE_EARTHQUAKE,
    MOVE_PSYCHIC,
    MOVE_METRONOME,
    MOVE_MIRROR_MOVE,
    MOVE_FIRE_BLAST,
    MOVE_SOFT_BOILED,
    MOVE_TRANSFORM,
    MOVE_SUBSTITUTE,
    MOVE_CURSE,
    MOVE_AEROBLAST,
    MOVE_MACH_PUNCH,
    MOVE_BELLY_DRUM,
    MOVE_ZAP_CANNON,
    MOVE_OUTRAGE,
    MOVE_GIGA_DRAIN,
    MOVE_ROLLOUT,
    MOVE_SACRED_FIRE,
    MOVE_MEGAHORN,
    MOVE_SYNTHESIS,
    MOVE_CRUNCH,
    MOVE_EXTREME_SPEED,
    MOVE_ANCIENT_POWER,
    MOVE_SHADOW_BALL,
    MOVE_FUTURE_SIGHT,
    MOVE_FAKE_OUT,
    MOVE_UPROAR,
    MOVE_WILL_O_WISP,
    MOVE_WISH,
    MOVE_ASSIST,
    MOVE_SUPERPOWER,
    MOVE_MAGIC_COAT,
    MOVE_BRICK_BREAK,
    MOVE_KNOCK_OFF,
    MOVE_SKILL_SWAP,
    MOVE_LUSTER_PURGE,
    MOVE_HYPER_VOICE,
    MOVE_BLAST_BURN,
    MOVE_HYDRO_CANNON,
    MOVE_OVERHEAT,
    MOVE_SIGNAL_BEAM,
    MOVE_AERIAL_ACE,
    MOVE_ICICLE_SPEAR,
    MOVE_FRENZY_PLANT,
    MOVE_BULK_UP,
    MOVE_VOLT_TACKLE,
    MOVE_CALM_MIND,
    MOVE_DRAGON_DANCE,
    MOVE_PSYCHO_BOOST,
    MOVE_ROCK_BLAST,
    MOVE_ERUPTION,
    MOVE_RECYCLE,
    MOVE_FACADE,
#endif
};

STATIC_ASSERT(ARRAY_COUNT(sDynamicCustomMonAbilities) <= 63, SizeOfDynamicCustomMonAbilities);
STATIC_ASSERT(ARRAY_COUNT(sDynamicCustomMonMoves) <= 63, SizeOfDynamicCustomMonMoves);

const u16 sTypeTintColors[NUMBER_OF_MON_TYPES] = 
{
//    [TYPE_NORMAL] = RGB_WHITE,
//    [TYPE_FIGHTING] = RGB(26, 8, 14),
//    [TYPE_FLYING] = RGB(31, 26, 7),
//    [TYPE_POISON] = RGB(26, 10, 25),
//    [TYPE_GROUND] = RGB(25, 23, 18),
//    [TYPE_ROCK] = RGB(18, 16, 8),
//    [TYPE_BUG] = RGB(18, 24, 6),
//    [TYPE_GHOST] = RGB(12, 10, 16),
//    [TYPE_STEEL] = RGB(19, 19, 20),
//    [TYPE_MYSTERY] = RGB_WHITE,
//    [TYPE_FIRE] = RGB(31, 20, 11),
//    [TYPE_WATER] = RGB(10, 18, 27),
//    [TYPE_GRASS] = RGB(12, 24, 11),
//    [TYPE_ELECTRIC] = RGB(30, 26, 7),
//    [TYPE_PSYCHIC] = RGB(31, 14, 15),
//    [TYPE_ICE] = RGB(14, 26, 25),
//    [TYPE_DRAGON] = RGB(10, 18, 27),
//    [TYPE_DARK] = RGB(6, 5, 8),
//#ifdef ROGUE_EXPANSION
//    [TYPE_FAIRY] = RGB(31, 15, 21),
//    [TYPE_STELLAR] = RGB(10, 18, 27),
//#endif


    [TYPE_NORMAL] = RGB_RANGE_255_TO_31(251, 193, 158),
    [TYPE_FIGHTING] = RGB_RANGE_255_TO_31(179, 39, 27),
    [TYPE_FLYING] = RGB_RANGE_255_TO_31(154, 217, 232),
    [TYPE_POISON] = RGB_RANGE_255_TO_31(113, 51, 232),
    [TYPE_GROUND] = RGB_RANGE_255_TO_31(142, 102, 59),
    [TYPE_ROCK] = RGB_RANGE_255_TO_31(142, 125, 60),
    [TYPE_BUG] = RGB_RANGE_255_TO_31(130, 142, 61),
    [TYPE_GHOST] = RGB_WHITE, //RGB_RANGE_255_TO_31(106, 46, 142),
    [TYPE_STEEL] = RGB_RANGE_255_TO_31(142, 142, 142),
    [TYPE_MYSTERY] = RGB_WHITE,
    [TYPE_FIRE] = RGB_RANGE_255_TO_31(255, 170, 71),
    [TYPE_WATER] = RGB_RANGE_255_TO_31(37, 178, 255),
    [TYPE_GRASS] = RGB_RANGE_255_TO_31(33, 255, 32),
    [TYPE_ELECTRIC] = RGB_RANGE_255_TO_31(255, 227, 20),
    [TYPE_PSYCHIC] = RGB_RANGE_255_TO_31(254, 20, 255),
    [TYPE_ICE] = RGB_RANGE_255_TO_31(163, 255, 253),
    [TYPE_DRAGON] = RGB_RANGE_255_TO_31(23, 20, 255),
    [TYPE_DARK] = RGB_RANGE_255_TO_31(61, 61, 61),
#ifdef ROGUE_EXPANSION
    [TYPE_FAIRY] = RGB_RANGE_255_TO_31(255, 192, 234),
    [TYPE_STELLAR] = RGB(10, 18, 27),
#endif
};

#include "data/rogue/custom_mons.h"

enum
{
    COMPRESSED_FORMAT_ORIGINAL = 0,
    COMPRESSED_FORMAT_MON_TYPE,
    COMPRESSED_FORMAT_UNUSED2,
    COMPRESSED_FORMAT_UNUSED3,
};

struct CompressedDynamicData
{
    u32 data1:21;
    u32 format:2; // COMPRESSED_FORMAT_[...]
    u32 data2:7;
    u32 reserved:2; // reserved for bitmask OTID_FLAG_CUSTOM_MON etc.
};
struct CompressedDynamicData_Original
{
    u32 move1:7; // 127 indices
    u32 move2:7; // 127 indices
    u32 move3:7; // 127 indices
    u32 format:2;
    u32 ability:7; // 127 indices
    u32 reserved:2;
};

struct CompressedDynamicData_MonType
{
    u32 type: 5;
    u32 typeSlot:1;
    u32 typeMoveFlip:1;
    u32 move1:7; // 127 indices
    u32 move2:7; // 127 indices
    u32 format:2;
    u32 ability:7; // 127 indices
    u32 reserved:2;
};


STATIC_ASSERT(sizeof(struct CompressedDynamicData) == sizeof(struct CompressedDynamicData_Original), SizeOfCompressedDynamicData_Original);
STATIC_ASSERT(sizeof(struct CompressedDynamicData) == sizeof(struct CompressedDynamicData_MonType), SizeOfCompressedDynamicData_MonType);

struct DynamicMonData
{
    u16 moves[MAX_MON_MOVES];
    u16 movesCount;
    u8 types[2];
    u16 ability;
};

STATIC_ASSERT(sizeof(struct CompressedDynamicData) == sizeof(u32), SizeOfDynamicCustomMonData);

static u16 SelectTypeBasedExtraMove(u8 type, u8 rng)
{
#ifdef ROGUE_EXPANSION
    switch (type)
    {        
    case TYPE_NORMAL:
        return rng ? MOVE_BOOMBURST : MOVE_EXTREME_SPEED;
    case TYPE_FIGHTING:
        return rng ? MOVE_FOCUS_BLAST : MOVE_CLOSE_COMBAT;
    case TYPE_FLYING:
        return rng ? MOVE_HURRICANE : MOVE_BRAVE_BIRD;
    case TYPE_POISON:
        return rng ? MOVE_SLUDGE_BOMB : MOVE_GUNK_SHOT;
    case TYPE_GROUND:
        return rng ? MOVE_EARTHQUAKE : MOVE_EARTH_POWER;
    case TYPE_ROCK:
        return rng ? MOVE_ROCK_SLIDE : MOVE_ANCIENT_POWER;
    case TYPE_BUG:
        return rng ? MOVE_LUNGE : MOVE_BUG_BUZZ;
    case TYPE_GHOST:
        return rng ? MOVE_SHADOW_BALL : MOVE_SHADOW_SNEAK;
    case TYPE_STEEL:
        return rng ? MOVE_IRON_HEAD : MOVE_MAGNET_BOMB;
    case TYPE_FIRE:
        return rng ? MOVE_OVERHEAT : MOVE_SACRED_FIRE;
    case TYPE_WATER:
        return rng ? MOVE_LIQUIDATION : MOVE_SURF;
    case TYPE_GRASS:
        return rng ? MOVE_GIGA_DRAIN : MOVE_LEAF_BLADE;
    case TYPE_ELECTRIC:
        return rng ? MOVE_THUNDERCLAP : MOVE_THUNDER;
    case TYPE_PSYCHIC:
        return rng ? MOVE_PSYCHIC : MOVE_PSYSHOCK;
    case TYPE_ICE:
        return rng ? MOVE_ICE_BEAM : MOVE_TRIPLE_AXEL;
    case TYPE_DRAGON:
        return rng ? MOVE_DRACO_METEOR : MOVE_DRAGON_RUSH;
    case TYPE_DARK:
        return rng ? MOVE_DARK_PULSE : MOVE_NIGHT_SLASH;
    case TYPE_FAIRY:
        return rng ? MOVE_MOONBLAST : MOVE_PLAY_ROUGH;
    }
#else

    switch (type)
    {        
    case TYPE_NORMAL:
        return rng ? MOVE_QUICK_ATTACK : MOVE_BODY_SLAM;
    case TYPE_FIGHTING:
        return rng ? MOVE_MACH_PUNCH : MOVE_FOCUS_PUNCH;
    case TYPE_FLYING:
        return rng ? MOVE_FLY : MOVE_AEROBLAST;
    case TYPE_POISON:
        return rng ? MOVE_TOXIC : MOVE_SLUDGE_BOMB;
    case TYPE_GROUND:
        return rng ? MOVE_EARTHQUAKE : MOVE_DIG;
    case TYPE_ROCK:
        return rng ? MOVE_ROCK_SLIDE : MOVE_SANDSTORM;
    case TYPE_BUG:
        return rng ? MOVE_BUG_BUZZ : MOVE_SIGNAL_BEAM;
    case TYPE_GHOST:
        return rng ? MOVE_SHADOW_BALL : MOVE_SHADOW_PUNCH;
    case TYPE_STEEL:
        return rng ? MOVE_METEOR_MASH : MOVE_IRON_DEFENSE;
    case TYPE_FIRE:
        return rng ? MOVE_FLAMETHROWER : MOVE_WILL_O_WISP;
    case TYPE_WATER:
        return rng ? MOVE_SURF : MOVE_WATER_PULSE;
    case TYPE_GRASS:
        return rng ? MOVE_TACKLE : MOVE_TACKLE;
    case TYPE_ELECTRIC:
        return rng ? MOVE_THUNDER : MOVE_VOLT_TACKLE;
    case TYPE_PSYCHIC:
        return rng ? MOVE_PSYCHIC : MOVE_CALM_MIND;
    case TYPE_ICE:
        return rng ? MOVE_ICE_BEAM : MOVE_BLIZZARD;
    case TYPE_DRAGON:
        return rng ? MOVE_DRAGON_BREATH : MOVE_DRAGON_CLAW;
    case TYPE_DARK:
        return rng ? MOVE_CRUNCH : MOVE_BITE;
    }
#endif

    return MOVE_RETURN;
}

static void UncompressDynamicMonData(u32 customMonId, struct DynamicMonData* outData)
{
    struct CompressedDynamicData* compressedUntyped = (struct CompressedDynamicData*)&customMonId;

    outData->ability = ABILITY_NONE;
    outData->movesCount = 0;
    outData->types[0] = TYPE_NONE;
    outData->types[1] = TYPE_NONE;

    if(compressedUntyped->format == COMPRESSED_FORMAT_ORIGINAL)
    {
        struct CompressedDynamicData_Original* compressedData = (struct CompressedDynamicData_Original*)compressedUntyped;

        outData->ability = ((compressedData->ability - 1) < ARRAY_COUNT(sDynamicCustomMonAbilities)) ? sDynamicCustomMonAbilities[compressedData->ability - 1] : ABILITY_NONE;

        if(compressedData->move1 != 0 && (compressedData->move1 - 1) < ARRAY_COUNT(sDynamicCustomMonMoves))
            outData->moves[outData->movesCount++] = sDynamicCustomMonMoves[compressedData->move1 - 1];

        if(compressedData->move2 != 0 && (compressedData->move2 - 1) < ARRAY_COUNT(sDynamicCustomMonMoves))
            outData->moves[outData->movesCount++] = sDynamicCustomMonMoves[compressedData->move2 - 1];

        if(compressedData->move3 != 0 && (compressedData->move3 - 1) < ARRAY_COUNT(sDynamicCustomMonMoves))
            outData->moves[outData->movesCount++] = sDynamicCustomMonMoves[compressedData->move3 - 1];

        //if(compressedData->move4 != 0 && (compressedData->move4 - 1) < ARRAY_COUNT(sDynamicCustomMonMoves))
        //    outData->moves[outData->movesCount++] = sDynamicCustomMonMoves[compressedData->move4 - 1];
    }
    else if(compressedUntyped->format == COMPRESSED_FORMAT_MON_TYPE)
    {
        struct CompressedDynamicData_MonType* compressedData = (struct CompressedDynamicData_MonType*)compressedUntyped;

        outData->ability = ((compressedData->ability - 1) < ARRAY_COUNT(sDynamicCustomMonAbilities)) ? sDynamicCustomMonAbilities[compressedData->ability - 1] : ABILITY_NONE;

        outData->types[compressedData->typeSlot] = compressedData->type;
        
        outData->moves[outData->movesCount++] = SelectTypeBasedExtraMove(compressedData->type, compressedData->typeMoveFlip);

        if(compressedData->move1 != 0 && (compressedData->move1 - 1) < ARRAY_COUNT(sDynamicCustomMonMoves))
            outData->moves[outData->movesCount++] = sDynamicCustomMonMoves[compressedData->move1 - 1];

        if(compressedData->move2 != 0 && (compressedData->move2 - 1) < ARRAY_COUNT(sDynamicCustomMonMoves))
            outData->moves[outData->movesCount++] = sDynamicCustomMonMoves[compressedData->move2 - 1];
    }
    else
    {
        AGB_ASSERT(FALSE);
    }
};

static u32 CompressedDynamicDataToCustomMonId(struct CompressedDynamicData* inData)
{
    u32 id;
    id = *((u32*)inData);
    return id;
}

u32 RogueGift_GetCustomMonId(struct Pokemon* mon)
{
    return RogueGift_GetCustomMonIdBySpecies(GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_OT_ID));
}

u32 RogueGift_GetCustomBoxMonId(struct BoxPokemon* mon)
{
    return RogueGift_GetCustomMonIdBySpecies(GetBoxMonData(mon, MON_DATA_SPECIES), GetBoxMonData(mon, MON_DATA_OT_ID));
}

static u16 TransformCheckSpecies(u16 species)
{
#ifdef ROGUE_EXPANSION
    // Special case (probably should get a proper fix for this)
    if(species >= SPECIES_PIKACHU_COSPLAY && species <= SPECIES_PICHU_SPIKY_EARED)
        return species;

    if(!gSpeciesInfo[species].isAlolanForm && !gSpeciesInfo[species].isGalarianForm && !gSpeciesInfo[species].isHisuianForm && !gSpeciesInfo[species].isPaldeanForm)
    {
        // Fix for castform/other form changes
        return GET_BASE_SPECIES_ID(species);
    }
#endif

    return species;
}

u32 RogueGift_GetCustomMonIdBySpecies(u16 species, u32 otId)
{
    u32 i;

    species = TransformCheckSpecies(species);

    if((otId & OTID_FLAG_CUSTOM_MON) && (otId & OTID_FLAG_DYNAMIC_CUSTOM_MON))
    {
        // otId is the custom mon id, as it encodes the data into it
        return otId;
    }
    if(otId & OTID_FLAG_CUSTOM_MON)
    {
        species = Rogue_GetEggSpecies(species);

        for(i = CUSTOM_MON_NONE + 1; i < CUSTOM_MON_COUNT; ++i)
        {
            struct CustomMonData const* monData = &sCustomPokemon[i];
            
            if(monData->otId == otId && Rogue_GetEggSpecies(monData->species) == species)
                return i;
        }
    }

    return CUSTOM_MON_NONE;
}

u16 RogueGift_GetCustomMonMove(u32 id, u8 i)
{
    if(id & OTID_FLAG_DYNAMIC_CUSTOM_MON)
    {
        struct DynamicMonData dynamicData;
        UncompressDynamicMonData(id, &dynamicData);
        return i < dynamicData.movesCount ? dynamicData.moves[i] : MOVE_NONE;
    }
    else
    {
        AGB_ASSERT(id < CUSTOM_MON_COUNT);

        if(id != CUSTOM_MON_NONE)
        {
            struct CustomMonData const* monData = &sCustomPokemon[id];
            return i < monData->movesCount ? monData->moves[i] : MOVE_NONE;
        }
    }

    return MOVE_NONE;
}

u16 RogueGift_GetCustomMonMoveCount(u32 id)
{
    if(id & OTID_FLAG_DYNAMIC_CUSTOM_MON)
    {
        struct DynamicMonData dynamicData;
        UncompressDynamicMonData(id, &dynamicData);
        return dynamicData.movesCount;
    }
    else
    {
        AGB_ASSERT(id < CUSTOM_MON_COUNT);

        if(id != CUSTOM_MON_NONE)
        {
            struct CustomMonData const* monData = &sCustomPokemon[id];

            if(id == CUSTOM_MON_ABBIE_MAREEP && !FlagGet(FLAG_ROGUE_UNLOCKED_PIKIN_EASTER_EGG))
            {
                // Don't show last move until activated easter egg
                return monData->movesCount - 1;
            }

            return monData->movesCount;
        }
    }

    return 0;
}

u16 RogueGift_GetCustomMonAbility(u32 id, u8 i)
{
    if(id & OTID_FLAG_DYNAMIC_CUSTOM_MON)
    {
        struct DynamicMonData dynamicData;
        UncompressDynamicMonData(id, &dynamicData);
        return i == 0 ? dynamicData.ability : ABILITY_NONE;
    }
    else
    {
        AGB_ASSERT(id < CUSTOM_MON_COUNT);

        if(id != CUSTOM_MON_NONE)
        {
            struct CustomMonData const* monData = &sCustomPokemon[id];
            return (monData->abilities != NULL && i < NUM_ABILITY_SLOTS) ? monData->abilities[i] : ABILITY_NONE;
        }
    }

    return ABILITY_NONE;
}

u16 RogueGift_GetCustomMonAbilityCount(u32 id)
{
    if(id & OTID_FLAG_DYNAMIC_CUSTOM_MON)
    {
        struct DynamicMonData dynamicData;
        UncompressDynamicMonData(id, &dynamicData);
        return dynamicData.ability != ABILITY_NONE ? NUM_ABILITY_SLOTS : 0;
    }
    else
    {
        struct CustomMonData const* monData = &sCustomPokemon[id];
        AGB_ASSERT(id < CUSTOM_MON_COUNT);

        return (monData->abilities != NULL) ? NUM_ABILITY_SLOTS : 0;
    }
}

u8 RogueGift_GetCustomMonType(u32 id, u8 i)
{
    if(id & OTID_FLAG_DYNAMIC_CUSTOM_MON)
    {
        struct DynamicMonData dynamicData;
        UncompressDynamicMonData(id, &dynamicData);
        return dynamicData.types[i];
    }
    else
    {
        AGB_ASSERT(id < CUSTOM_MON_COUNT);

        if(id != CUSTOM_MON_NONE)
        {
            struct CustomMonData const* monData = &sCustomPokemon[id];
            return monData->types[i];
        }
    }

    return TYPE_NONE;
}

bool8 RogueGift_CanRenameCustomMon(u32 id)
{
    if(id & OTID_FLAG_DYNAMIC_CUSTOM_MON)
    {
        return TRUE;
    }
    else
    {
        // We're allowed to rename exotics, as they're technically dynamic custom mons too
        struct CustomMonData const* monData = &sCustomPokemon[id];
        AGB_ASSERT(id < CUSTOM_MON_COUNT);

        return monData->customTrainerId == CUSTOM_TRAINER_EXOTIC;
    }
}

bool8 RogueGift_DisplayCustomMonRarity(u32 id)
{
    if(id & OTID_FLAG_DYNAMIC_CUSTOM_MON)
        return TRUE;
    else
    {
        struct CustomMonData const* monData = &sCustomPokemon[id]; 
        if(monData->customTrainerId == CUSTOM_TRAINER_EXOTIC)
            return TRUE;
    }

    return FALSE;
}

u8 RogueGift_GetCustomMonRarity(u32 id)
{
    if(id & OTID_FLAG_DYNAMIC_CUSTOM_MON)
    {
        u16 moveCount = RogueGift_GetCustomMonMoveCount(id);
        u16 abilityCount = RogueGift_GetCustomMonAbilityCount(id);

        if(abilityCount != 0)
        {
            if(moveCount <= 1)
                return UNIQUE_RARITY_RARE;
            else
                return UNIQUE_RARITY_EPIC;
        }

        return UNIQUE_RARITY_COMMON;
    }
    else
    {
        struct CustomMonData const* monData = &sCustomPokemon[id]; 
        AGB_ASSERT(id < CUSTOM_MON_COUNT);

        switch (monData->customTrainerId)
        {
        case CUSTOM_TRAINER_COMMON:
            return UNIQUE_RARITY_COMMON;
        
        case CUSTOM_TRAINER_RARE:
            return UNIQUE_RARITY_RARE;
        
        case CUSTOM_TRAINER_EPIC:
            return UNIQUE_RARITY_EPIC;
        
        case CUSTOM_TRAINER_EXOTIC:
            return UNIQUE_RARITY_EXOTIC;
        }

        // We expect most/all of these to be classed as exotic
        return UNIQUE_RARITY_EXOTIC;
    }
}

u8 const* RogueGift_GetRarityName(u8 rarity)
{
    switch (rarity)
    {
    case UNIQUE_RARITY_COMMON:
        return sCustomTrainers[CUSTOM_TRAINER_COMMON].name;
    
    case UNIQUE_RARITY_RARE:
        return sCustomTrainers[CUSTOM_TRAINER_RARE].name;
    
    case UNIQUE_RARITY_EPIC:
        return sCustomTrainers[CUSTOM_TRAINER_EPIC].name;
    
    case UNIQUE_RARITY_EXOTIC:
        return sCustomTrainers[CUSTOM_TRAINER_EXOTIC].name;
    }

    AGB_ASSERT(FALSE);
    return sCustomTrainers[CUSTOM_TRAINER_QUESTS].name;
}

void RogueGift_CreateMon(u32 customMonId, struct Pokemon* mon, u16 species, u8 level, u8 fixedIV)
{
    struct CustomTrainerData const* trainerData = NULL;
    u32 temp = 0;
    u16 customMoveCount = RogueGift_GetCustomMonMoveCount(customMonId);
    u16 customAbilityCount = RogueGift_GetCustomMonAbilityCount(customMonId);
    
    if(customMonId & OTID_FLAG_DYNAMIC_CUSTOM_MON)
    {
        u8 customTrainerId = sRarityToCustomTrainerIndex[RogueGift_GetCustomMonRarity(customMonId)];
        trainerData = &sCustomTrainers[customTrainerId];

        ZeroMonData(mon);
        CreateMon(mon, species, level, fixedIV, 0, 0, OT_ID_CUSTOM_MON, customMonId);

        // Assign pokeball
        temp = ITEM_ROGUE_BALL;
        SetMonData(mon, MON_DATA_POKEBALL, &temp);
    }
    else
    {
        struct CustomMonData const* monData = &sCustomPokemon[customMonId]; 
        trainerData = &sCustomTrainers[monData->customTrainerId];

        AGB_ASSERT(customMonId < CUSTOM_MON_COUNT);
        AGB_ASSERT(Rogue_GetEggSpecies(species) == Rogue_GetEggSpecies(monData->species));
        AGB_ASSERT(trainerData->trainerId & OTID_FLAG_CUSTOM_MON);
        AGB_ASSERT(!(trainerData->trainerId & OTID_FLAG_DYNAMIC_CUSTOM_MON));

        ZeroMonData(mon);
        CreateMon(mon, species, level, fixedIV, 0, 0, OT_ID_CUSTOM_MON, trainerData->trainerId);

        // Update nickname
        if(monData->nickname != NULL)
        {
            SetMonData(mon, MON_DATA_NICKNAME, monData->nickname);
        }

        // Assign pokeball
        temp = monData->pokeball;
        SetMonData(mon, MON_DATA_POKEBALL, &temp);

        // Set shiny state
        temp = monData->isShiny ? 1 : 0;
        SetMonData(mon, MON_DATA_IS_SHINY, &temp);
    }

    // Met location
    temp = METLOC_FATEFUL_ENCOUNTER;
    SetMonData(mon, MON_DATA_MET_LOCATION, &temp);

    // Update OT name
    SetMonData(mon, MON_DATA_OT_NAME, trainerData->name);

    // Update OT colour
    temp = trainerData->trainerColour;
    SetMonData(mon, MON_DATA_OT_GENDER, &temp);

    // Default moveset is first 4 moves from custom moveset
    if(customMoveCount)
    {
        u8 m, j;
        u16 moves[MAX_MON_MOVES];

        for(m = 0; m < MAX_MON_MOVES && m < customMoveCount; ++m)
        {
            moves[m] = RogueGift_GetCustomMonMove(customMonId, m);
        }

        // Fill the rest of the moves with default moves
        for(j = 0; m < MAX_MON_MOVES; ++m, ++j)
        {
            moves[m] = GetMonData(mon, MON_DATA_MOVE1 + j);
        }

        // Give back moves
        for(m = 0; m < MAX_MON_MOVES; ++m)
        {
            temp = moves[m];
            SetMonData(mon, MON_DATA_MOVE1 + m, &temp);
            SetMonData(mon, MON_DATA_PP1 + m, &gBattleMoves[temp].pp);
        }
    }

    // Default custom ability
    if(customAbilityCount)
    {
        temp = (RogueGift_GetCustomMonAbility(customMonId, 1) != ABILITY_NONE) ? (Random() % 2) : 0;
        SetMonData(mon, MON_DATA_ABILITY_NUM, &temp);
    }
}

static u32 SelectNextMoveIndex(u16 species)
{
    if(RogueMiscQuery_AnyActiveElements())
    {
        u32 i;
        u16 moveId = RogueMiscQuery_SelectRandomElement(Random());
        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, moveId);

        for (i = 0; i < ARRAY_COUNT(sDynamicCustomMonMoves); i++)
        {
            if(sDynamicCustomMonMoves[i] == moveId)
                return 1 + i;
        }

        // Should never get here
        AGB_ASSERT(FALSE);
        return 1;
    }

    // Can get here if we've ran out of move options, as everything else is already known
    return 0;
}

static u32 SelectNextAbilityIndex(u16 species)
{
    u8 i;

    // Give the mon a new ability for it
    while(TRUE)
    {
        u32 idx = (Random() % ARRAY_COUNT(sDynamicCustomMonAbilities));

        for(i = 0; i < NUM_ABILITY_SLOTS; ++i)
        {
            if(GetAbilityBySpecies(species, i, 0) == sDynamicCustomMonAbilities[idx])
            {
                idx = 10000;
                break;
            }
        }

        if(idx != 10000)
            return 1 + idx;
    }

    return 0;
}

static u32 SelectRandomType(u16 species, u8 index)
{
    u8 type;
    do
    {
        type = Random() % NUMBER_OF_MON_TYPES;
    } 
    while (!IS_STANDARD_TYPE(type) || GetTypeBySpecies(species, 0, 0) == type || GetTypeBySpecies(species, 1, 0) == type);

    return type;
}

u32 RogueGift_CreateDynamicMonId(u8 rarity, u16 species)
{
    u16 i;
    u32 temp;
    struct CompressedDynamicData compressedDataUntyped = {0};
    struct CustomTrainerData const* trainerData = &sCustomTrainers[sRarityToCustomTrainerIndex[rarity]];
    struct RoguePokemonProfile const* pokemonProfile = Rogue_GetPokemonProfile(species);

    switch (RogueRandom() % 2)
    {
    case 0:
        compressedDataUntyped.format = COMPRESSED_FORMAT_ORIGINAL;
        break;

    case 1:
        compressedDataUntyped.format = COMPRESSED_FORMAT_MON_TYPE;
        break;
    }

    // Lock this behind upgrade
    if(compressedDataUntyped.format == COMPRESSED_FORMAT_MON_TYPE && !RogueHub_HasUpgrade(HUB_UPGRADE_LAB_UNIQUE_TYPINGS))
    {
        compressedDataUntyped.format = COMPRESSED_FORMAT_ORIGINAL;
    }

    // Start query with moves which are valid
    RogueCustomQuery_Begin();

    for (i = 0; i < ARRAY_COUNT(sDynamicCustomMonMoves); i++)
    {
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, sDynamicCustomMonMoves[i]);
    }

    for (i = 0; pokemonProfile->levelUpMoves[i].move != MOVE_NONE; i++)
    {
        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, pokemonProfile->levelUpMoves[i].move);
    }

    for (i = 0; pokemonProfile->tutorMoves[i] != MOVE_NONE; i++)
    {
        RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, pokemonProfile->tutorMoves[i]);
    }

    if(compressedDataUntyped.format == COMPRESSED_FORMAT_ORIGINAL)
    {
        struct CompressedDynamicData_Original* compressedData = (struct CompressedDynamicData_Original*)&compressedDataUntyped;

        switch (rarity)
        {
        case UNIQUE_RARITY_COMMON:
            compressedData->move1 = SelectNextMoveIndex(species);
            compressedData->move2 = SelectNextMoveIndex(species);
            break;

        case UNIQUE_RARITY_RARE:
            compressedData->move1 = SelectNextMoveIndex(species);
            compressedData->ability = SelectNextAbilityIndex(species);
            break;

        case UNIQUE_RARITY_EPIC:
            compressedData->move1 = SelectNextMoveIndex(species);
            compressedData->move2 = SelectNextMoveIndex(species);
            compressedData->move3 = SelectNextMoveIndex(species);
            //compressedData.move4 = SelectNextMoveIndex(species);
            compressedData->ability = SelectNextAbilityIndex(species);
            break;

        default:
            AGB_ASSERT(FALSE);
            break;
        }
    }
    else if(compressedDataUntyped.format == COMPRESSED_FORMAT_MON_TYPE)
    {
        struct CompressedDynamicData_MonType* compressedData = (struct CompressedDynamicData_MonType*)&compressedDataUntyped;
        compressedData->typeSlot = Random() % 1;
        compressedData->typeMoveFlip = Random() % 1;

        switch (rarity)
        {
        case UNIQUE_RARITY_COMMON:
            compressedData->type = SelectRandomType(species, compressedData->typeSlot);
            break;

        case UNIQUE_RARITY_RARE:
            compressedData->type = SelectRandomType(species, compressedData->typeSlot);
            compressedData->move1 = SelectNextMoveIndex(species);
            compressedData->move2 = SelectNextMoveIndex(species);
            break;

        case UNIQUE_RARITY_EPIC:
            compressedData->type = SelectRandomType(species, compressedData->typeSlot);
            compressedData->move1 = SelectNextMoveIndex(species);
            compressedData->move2 = SelectNextMoveIndex(species);
            compressedData->ability = SelectNextAbilityIndex(species);
            break;

        default:
            AGB_ASSERT(FALSE);
            break;
        }
    }
    else
    {
        AGB_ASSERT(FALSE);
    }

    RogueCustomQuery_End();

    temp = CompressedDynamicDataToCustomMonId(&compressedDataUntyped);
    temp |= (OTID_FLAG_CUSTOM_MON | OTID_FLAG_DYNAMIC_CUSTOM_MON);

#ifdef ROGUE_DEBUG
    // Ensure data (un)compresses correctly
    if(compressedDataUntyped.format == COMPRESSED_FORMAT_ORIGINAL)
    {
        struct CompressedDynamicData_Original* compressedData = (struct CompressedDynamicData_Original*)&compressedDataUntyped;

        struct DynamicMonData dynamicData = {0};
        

        u16 ability = (compressedData->ability == 0) ? ABILITY_NONE : sDynamicCustomMonAbilities[compressedData->ability - 1];
        u16 moves[MAX_MON_MOVES] = 
        {
            (compressedData->move1 == 0) ? MOVE_NONE : sDynamicCustomMonMoves[compressedData->move1 - 1],
            (compressedData->move2 == 0) ? MOVE_NONE : sDynamicCustomMonMoves[compressedData->move2 - 1],
            (compressedData->move3 == 0) ? MOVE_NONE : sDynamicCustomMonMoves[compressedData->move3 - 1],
            MOVE_NONE
            //(compressedData.move4 == 0) ? MOVE_NONE : sDynamicCustomMonMoves[compressedData.move4 - 1],
        };

        UncompressDynamicMonData(temp, &dynamicData);

        AGB_ASSERT(moves[0] == dynamicData.moves[0]);
        AGB_ASSERT(moves[1] == dynamicData.moves[1]);
        AGB_ASSERT(moves[2] == dynamicData.moves[2]);
        AGB_ASSERT(moves[3] == dynamicData.moves[3]);

        AGB_ASSERT(ability == dynamicData.ability);
    }

#endif
    return temp;
}

static bool8 IsDynamicUniqueMonValid(struct UniqueMon* mon)
{
    return !(mon->species == SPECIES_NONE || mon->customMonId == 0 || mon->countDown == 0);
}

static u8 RandomRarity()
{
    u8 rarity;

    switch (Random() % 7)
    {
    case 0:
        rarity = UNIQUE_RARITY_EXOTIC;
        break;

    case 1:
    case 2:
        rarity = UNIQUE_RARITY_EPIC;
        break;

    case 3:
    case 4:
        rarity = UNIQUE_RARITY_RARE;
        break;
    
    case 5:
    case 6:
        rarity = UNIQUE_RARITY_COMMON;
        break;

    default:
        AGB_ASSERT(FALSE);
        rarity = UNIQUE_RARITY_COMMON;
        break;
    }

    if(rarity == UNIQUE_RARITY_EXOTIC && !RogueHub_HasUpgrade(HUB_UPGRADE_LAB_UNIQUE_MON_RARITY_EXOTIC))
        rarity = UNIQUE_RARITY_EPIC;

    if(rarity == UNIQUE_RARITY_EPIC && !RogueHub_HasUpgrade(HUB_UPGRADE_LAB_UNIQUE_MON_RARITY_EPIC))
        rarity = UNIQUE_RARITY_RARE;

    if(rarity == UNIQUE_RARITY_RARE && !RogueHub_HasUpgrade(HUB_UPGRADE_LAB_UNIQUE_MON_RARITY_RARE))
        rarity = UNIQUE_RARITY_COMMON;

    return rarity;
}

static bool8 IsSlotUnlocked(u8 slot)
{
    if(RogueHub_HasUpgrade(HUB_UPGRADE_LAB_UNIQUE_MON_LAB))
    {
        // 3 slots unlocked by default
        if(slot <= 2)
            return TRUE;

        if(slot == 3)
            return RogueHub_HasUpgrade(HUB_UPGRADE_LAB_UNIQUE_MON_SLOTS0);

        if(slot == 4)
            return RogueHub_HasUpgrade(HUB_UPGRADE_LAB_UNIQUE_MON_SLOTS1);
    }

    return FALSE;
}

static bool8 IsCustomMonInUse(u32 customMonId)
{
    u32 i;

    for(i = 0; i < DYNAMIC_UNIQUE_MON_COUNT; ++i)
    {
        if(IsDynamicUniqueMonValid(&gRogueSaveBlock->dynamicUniquePokemon[i]) && gRogueSaveBlock->dynamicUniquePokemon[i].customMonId == customMonId)
            return TRUE;
    }

    return FALSE;
}

static u32 SelectUnusedUnlockedExoticMon()
{
    struct RogueQuestReward const* reward;
    u32 currentList[CUSTOM_MON_COUNT];
    u32 listSize = 0;
    u32 questId, j;
    u32 questCount;

    for(j = 0; j < CUSTOM_MON_COUNT; ++j)
    {
        if(sCustomPokemon[j].isDefaultSpawn)
            currentList[listSize++] = j;
    }

    // Populate with exotic mons we have already unlocked
    for(questId = 0; questId < QUEST_ID_COUNT; ++questId)
    {
        if(RogueQuest_HasCollectedRewards(questId))
        {
            questCount = RogueQuest_GetRewardCount(questId);

            for(j = 0; j < questCount; ++j)
            {
                reward = RogueQuest_GetReward(questId, j);

                if(reward->type == QUEST_REWARD_POKEMON && reward->perType.pokemon.customMonId != 0)
                {
                    if(!IsCustomMonInUse(reward->perType.pokemon.customMonId))
                        currentList[listSize++] = reward->perType.pokemon.customMonId;
                }
            }
        }
    }

    // Pick from random options
    if(listSize != 0)
        return currentList[Random() % listSize];

    return 0;
}

void RogueGift_EnsureDynamicCustomMonsAreValid()
{
    u8 i, write;
    u16 newSpecies[DYNAMIC_UNIQUE_MON_COUNT];
    u8 dexVariant = RoguePokedex_GetDexVariant();

    // We use query below, so grab some new unique species now
    {
        // Override dex variant to our max nat dex
        u8 dexVariantToRestore = RoguePokedex_GetDexVariant();
        RoguePokedex_SetDexVariant(POKEDEX_VARIANT_DEFAULT);

        RogueMonQuery_Begin();
        RogueMonQuery_IsSpeciesActive();
        RogueMonQuery_TransformIntoEggSpecies();
        RogueMonQuery_IsLegendary(QUERY_FUNC_EXCLUDE);
        
        for(i = 0; i < DYNAMIC_UNIQUE_MON_COUNT; ++i)
        {
            if(IsDynamicUniqueMonValid(&gRogueSaveBlock->dynamicUniquePokemon[i]))
            {
                RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, gRogueSaveBlock->dynamicUniquePokemon[i].species);
            }
        }

        // Remove random entries until we can safely calcualte weights without going over
        while(RogueWeightQuery_IsOverSafeCapacity())
        {
            RogueMiscQuery_FilterByChance(Random(), QUERY_FUNC_INCLUDE, 50, DYNAMIC_UNIQUE_MON_COUNT);
        }

        RogueWeightQuery_Begin();
        RogueWeightQuery_FillWeights(1);

        for(i = 0; i < DYNAMIC_UNIQUE_MON_COUNT; ++i)
        {
            newSpecies[i] = RogueWeightQuery_SelectRandomFromWeightsWithUpdate(Random(), 0);
        }

        RogueWeightQuery_End();

        RogueMonQuery_End();

        RoguePokedex_SetDexVariant(dexVariantToRestore);
    }

    // Shift all the empty slots up 
    write = 0;
    for(i = 0; i < DYNAMIC_UNIQUE_MON_COUNT; ++i)
    {
        if(IsDynamicUniqueMonValid(&gRogueSaveBlock->dynamicUniquePokemon[i]))
        {
            if(write != i)
            {
                memcpy(&gRogueSaveBlock->dynamicUniquePokemon[write], &gRogueSaveBlock->dynamicUniquePokemon[i], sizeof(struct UniqueMon));
                memset(&gRogueSaveBlock->dynamicUniquePokemon[i], 0, sizeof(struct UniqueMon));
            }

            write++;
        }
        else
        {
            memset(&gRogueSaveBlock->dynamicUniquePokemon[i], 0, sizeof(struct UniqueMon));
        }
    }

    // Replace invalid mons
    for(i = 0; i < DYNAMIC_UNIQUE_MON_COUNT; ++i)
    {
        // We don't want to populate slots that aren't active
        if(!IsSlotUnlocked(i))
            continue;

        if(!IsDynamicUniqueMonValid(&gRogueSaveBlock->dynamicUniquePokemon[i]))
        {
            u8 rarity = RandomRarity();

            gRogueSaveBlock->dynamicUniquePokemon[i].countDown = 60 + 30 * i; // Time remaining is based on the slot

            if(rarity == UNIQUE_RARITY_EXOTIC)
            {
                u32 customMonId = SelectUnusedUnlockedExoticMon();

                if(customMonId != 0)
                {
                    struct CustomMonData const* monData = &sCustomPokemon[customMonId];
                    AGB_ASSERT(customMonId < CUSTOM_MON_COUNT);

                    gRogueSaveBlock->dynamicUniquePokemon[i].species = Rogue_GetEggSpecies(monData->species);
                    gRogueSaveBlock->dynamicUniquePokemon[i].customMonId = customMonId;
                    continue;
                }

                // Fallback to just have an epic mon in this slot
                rarity = UNIQUE_RARITY_EPIC;
            }

            gRogueSaveBlock->dynamicUniquePokemon[i].species = newSpecies[i];
            gRogueSaveBlock->dynamicUniquePokemon[i].customMonId = RogueGift_CreateDynamicMonId(rarity, newSpecies[i]);
        }
    }

}

void RogueGift_ClearDynamicCustomMons()
{
    memset(&gRogueSaveBlock->dynamicUniquePokemon[0], 0, sizeof(struct UniqueMon) * DYNAMIC_UNIQUE_MON_COUNT);
}

struct UniqueMon* RogueGift_GetDynamicUniqueMon(u8 slot)
{
    AGB_ASSERT(slot < DYNAMIC_UNIQUE_MON_COUNT);
    return &gRogueSaveBlock->dynamicUniquePokemon[slot];
}

void RogueGift_CountDownDynamicCustomMons()
{
    u8 i;

    for(i = 0; i < DYNAMIC_UNIQUE_MON_COUNT; ++i)
    {
        if(gRogueSaveBlock->dynamicUniquePokemon[i].countDown != 0)
            --gRogueSaveBlock->dynamicUniquePokemon[i].countDown;
    }
}

void RogueGift_RemoveDynamicCustomMon(u32 customMonId)
{
    u8 i;

    for(i = 0; i < DYNAMIC_UNIQUE_MON_COUNT; ++i)
    {
        if(gRogueSaveBlock->dynamicUniquePokemon[i].customMonId == customMonId)
        {
            memset(&gRogueSaveBlock->dynamicUniquePokemon[i], 0, sizeof(struct UniqueMon));
        }
    }
}

bool8 RogueGift_IsDynamicMonSlotEnabled(u8 slot)
{
    AGB_ASSERT(slot < DYNAMIC_UNIQUE_MON_COUNT);

    if(!IsSlotUnlocked(slot))
        return FALSE;

    return IsDynamicUniqueMonValid(&gRogueSaveBlock->dynamicUniquePokemon[slot]);
}

u32 RogueGift_TryFindEnabledDynamicCustomMonForSpecies(u16 species)
{
    u8 i;

    species = Rogue_GetEggSpecies(species);

    for(i = 0; i < DYNAMIC_UNIQUE_MON_COUNT; ++i)
    {
        if(RogueGift_IsDynamicMonSlotEnabled(i))
        {
            if(gRogueSaveBlock->dynamicUniquePokemon[i].species == species)
                return gRogueSaveBlock->dynamicUniquePokemon[i].customMonId;
        }
    }

    return 0;
}

static u8 ModifyShinyType(u8 type)
{
    if(type == TYPE_NONE)
        return type;

    do
    {
        type = (type + 5) % NUMBER_OF_MON_TYPES;
    } 
    while (!IS_STANDARD_TYPE(type));

    return type;
    
}

bool8 RogueGift_TryApplyPaletteModify(u32 id, bool8 isShiny, u16 const* inputPal, u16 const* layerRefPal, u16* outputPal)
{
    u8 type1 = RogueGift_GetCustomMonType(id, 0);
    u8 type2 = RogueGift_GetCustomMonType(id, 1);

    if(type1 != TYPE_NONE || type2 != TYPE_NONE)
    {
        u16 layerPal[16];
        u16 layerWhitePoints[PALETTE_MODIFY_LAYER_COUNT];
        u16 colorsToApply[PALETTE_MODIFY_LAYER_COUNT];

        if(isShiny)
        {
            // Give a psuedo random tint for shiny pal version
            type1 = ModifyShinyType(type1);
            type2 = ModifyShinyType(type2);
        }

        colorsToApply[0] = RGB_ALPHA;
        colorsToApply[1] = RGB_ALPHA;
        colorsToApply[2] = RGB_ALPHA;

        if(type1 != TYPE_NONE)
            colorsToApply[0] = sTypeTintColors[type1];

        if(type2 != TYPE_NONE)
            colorsToApply[1] = sTypeTintColors[type2];

        if(layerRefPal == NULL)
        {
            // Ref and Input are the same so generate layers and whitepoints together
            Rogue_GenerateLayerPaletteByHue(inputPal, inputPal, layerPal);
            Rogue_GenerateWhitePointsPerLayers(inputPal, layerPal, layerWhitePoints, gDefaultPaletteLayerMasks);
        }
        else
        {
            // Generate temp layers for ref so we can generate white points
            Rogue_GenerateLayerPaletteByHue(layerRefPal, layerRefPal, layerPal);
            Rogue_GenerateWhitePointsPerLayers(layerRefPal, layerPal, layerWhitePoints, gDefaultPaletteLayerMasks);

            // Now generate actual palette for input
            Rogue_GenerateLayerPaletteByHue(inputPal, layerRefPal, layerPal);
        }

        Rogue_ModifyPaletteByLayersHueShift(inputPal, layerPal, layerWhitePoints, outputPal, gDefaultPaletteLayerMasks, colorsToApply);

        return TRUE;
    }

    return FALSE;
}