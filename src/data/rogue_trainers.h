#include "constants/event_objects.h"
#include "constants/layouts.h"
#include "constants/maps.h"
#include "constants/pokemon.h"
#include "constants/rogue.h"
#include "constants/trainers.h"
#include "constants/weather.h"

#define ENCOUNTER_MAP(id, map) { .encounterId=id, .layout=LAYOUT_##map, .group=MAP_GROUP(map), .num=MAP_NUM(map) }

const u8 gRogueTypeWeatherTable[] = 
{
    [TYPE_NONE] = WEATHER_NONE,
    [TYPE_NORMAL] = WEATHER_NONE,
    [TYPE_FIGHTING] = WEATHER_SUNNY,
    [TYPE_FLYING] = WEATHER_NONE,
    [TYPE_POISON] = WEATHER_SHADE,
    [TYPE_GROUND] = WEATHER_SANDSTORM,
    [TYPE_ROCK] = WEATHER_SANDSTORM,
    [TYPE_BUG] = WEATHER_SHADE,
    [TYPE_GHOST] = WEATHER_VOLCANIC_ASH,
    [TYPE_STEEL] = WEATHER_SANDSTORM,
    [TYPE_MYSTERY] = WEATHER_UNDERWATER_BUBBLES,
    [TYPE_FIRE] = WEATHER_DROUGHT,
    [TYPE_WATER] = WEATHER_DOWNPOUR,
    [TYPE_GRASS] = WEATHER_LEAVES,
    [TYPE_ELECTRIC] = WEATHER_RAIN_THUNDERSTORM,
    [TYPE_PSYCHIC] = WEATHER_FOG_HORIZONTAL,
    [TYPE_ICE] = WEATHER_SNOW,
    [TYPE_DRAGON] = WEATHER_DROUGHT,
    [TYPE_DARK] = WEATHER_VOLCANIC_ASH,
#ifdef ROGUE_EXPANSION
    [TYPE_FAIRY] = WEATHER_FOG_DIAGONAL,
#endif
};

const struct RogueEncounterMap gRogueTypeToEliteRoom[] = 
{
    [TYPE_NONE] = ENCOUNTER_MAP(0, ROGUE_BOSS_11),
    [TYPE_NORMAL] = ENCOUNTER_MAP(0, ROGUE_BOSS_8),
    [TYPE_FIGHTING] = ENCOUNTER_MAP(0, ROGUE_BOSS_8),
    [TYPE_FLYING] = ENCOUNTER_MAP(0, ROGUE_BOSS_8),
    [TYPE_POISON] = ENCOUNTER_MAP(0, ROGUE_BOSS_9),
    [TYPE_GROUND] = ENCOUNTER_MAP(0, ROGUE_BOSS_11),
    [TYPE_ROCK] = ENCOUNTER_MAP(0, ROGUE_BOSS_11),
    [TYPE_BUG] = ENCOUNTER_MAP(0, ROGUE_BOSS_9),
    [TYPE_GHOST] = ENCOUNTER_MAP(0, ROGUE_BOSS_9),
    [TYPE_STEEL] = ENCOUNTER_MAP(0, ROGUE_BOSS_11),
    [TYPE_MYSTERY] = ENCOUNTER_MAP(0, ROGUE_BOSS_10),
    [TYPE_FIRE] = ENCOUNTER_MAP(0, ROGUE_BOSS_11),
    [TYPE_WATER] = ENCOUNTER_MAP(0, ROGUE_BOSS_10),
    [TYPE_GRASS] = ENCOUNTER_MAP(0, ROGUE_BOSS_8),
    [TYPE_ELECTRIC] = ENCOUNTER_MAP(0, ROGUE_BOSS_8),
    [TYPE_PSYCHIC] = ENCOUNTER_MAP(0, ROGUE_BOSS_9),
    [TYPE_ICE] = ENCOUNTER_MAP(0, ROGUE_BOSS_10),
    [TYPE_DRAGON] = ENCOUNTER_MAP(0, ROGUE_BOSS_11),
    [TYPE_DARK] = ENCOUNTER_MAP(0, ROGUE_BOSS_8),
#ifdef ROGUE_EXPANSION
    [TYPE_FAIRY] = ENCOUNTER_MAP(0, ROGUE_BOSS_10),
#endif
};

#undef ENCOUNTER_MAP

// Kinda a gross way of doing it, but was the simplest without hacking it in elsewhere
static const u16 sQuerySpecies_Kanto[] =
{
    SPECIES_BULBASAUR,
    SPECIES_CHARMANDER,
    SPECIES_SQUIRTLE,
    SPECIES_CATERPIE,
    SPECIES_WEEDLE,
    SPECIES_PIDGEY,
    SPECIES_RATTATA,
    SPECIES_SPEAROW,
    SPECIES_EKANS,
    SPECIES_PIKACHU,
    SPECIES_SANDSHREW,
    SPECIES_NIDORAN_F,
    SPECIES_NIDORAN_M,
    SPECIES_CLEFAIRY,
    SPECIES_VULPIX,
    SPECIES_JIGGLYPUFF,
    SPECIES_ZUBAT,
    SPECIES_ODDISH,
    SPECIES_PARAS,
    SPECIES_VENONAT,
    SPECIES_DIGLETT,
    SPECIES_MEOWTH,
    SPECIES_PSYDUCK,
    SPECIES_MANKEY,
    SPECIES_GROWLITHE,
    SPECIES_POLIWAG,
    SPECIES_ABRA,
    SPECIES_MACHOP,
    SPECIES_BELLSPROUT,
    SPECIES_TENTACOOL,
    SPECIES_GEODUDE,
    SPECIES_PONYTA,
    SPECIES_SLOWPOKE,
    SPECIES_MAGNEMITE,
    SPECIES_FARFETCHD,
    SPECIES_DODUO,
    SPECIES_SEEL,
    SPECIES_GRIMER,
    SPECIES_SHELLDER,
    SPECIES_GASTLY,
    SPECIES_ONIX,
    SPECIES_DROWZEE,
    SPECIES_KRABBY,
    SPECIES_VOLTORB,
    SPECIES_EXEGGCUTE,
    SPECIES_CUBONE,
    SPECIES_HITMONLEE,
    SPECIES_HITMONCHAN,
    SPECIES_LICKITUNG,
    SPECIES_KOFFING,
    SPECIES_RHYHORN,
    SPECIES_CHANSEY,
    SPECIES_TANGELA,
    SPECIES_KANGASKHAN,
    SPECIES_HORSEA,
    SPECIES_GOLDEEN,
    SPECIES_STARYU,
    SPECIES_MR_MIME,
    SPECIES_SCYTHER,
    SPECIES_JYNX,
    SPECIES_ELECTABUZZ,
    SPECIES_MAGMAR,
    SPECIES_PINSIR,
    SPECIES_TAUROS,
    SPECIES_MAGIKARP,
    SPECIES_LAPRAS,
    SPECIES_DITTO,
    SPECIES_EEVEE,
    SPECIES_PORYGON,
    SPECIES_OMANYTE,
    SPECIES_KABUTO,
    SPECIES_AERODACTYL,
    SPECIES_SNORLAX,
    SPECIES_ARTICUNO,
    SPECIES_ZAPDOS,
    SPECIES_MOLTRES,
    SPECIES_DRATINI,
    SPECIES_MEWTWO,
    SPECIES_MEW,
#ifdef ROGUE_EXPANSION
    SPECIES_RATTATA_ALOLAN,
    SPECIES_SANDSHREW_ALOLAN,
    SPECIES_VULPIX_ALOLAN,
    SPECIES_DIGLETT_ALOLAN,
    SPECIES_MEOWTH_ALOLAN,
    SPECIES_GEODUDE_ALOLAN,
    SPECIES_GRIMER_ALOLAN,

    SPECIES_MEOWTH_GALARIAN,
    SPECIES_PONYTA_GALARIAN,
    SPECIES_SLOWPOKE_GALARIAN,
    SPECIES_FARFETCHD_GALARIAN,
    SPECIES_MR_MIME_GALARIAN,
    SPECIES_ARTICUNO_GALARIAN,
    SPECIES_ZAPDOS_GALARIAN,
    SPECIES_MOLTRES_GALARIAN,
#endif
};


static const u16 sQuerySpecies_Blue[] =
{
    SPECIES_PIKACHU,
    SPECIES_NIDORAN_M,
    SPECIES_CLEFAIRY,
    SPECIES_VULPIX,
    SPECIES_ODDISH,
    SPECIES_GROWLITHE,
    SPECIES_POLIWAG,
    SPECIES_ABRA,
    SPECIES_MACHOP,
    SPECIES_GEODUDE,
    SPECIES_GASTLY,
    SPECIES_EXEGGCUTE,
    SPECIES_RHYHORN,
    SPECIES_HORSEA,
    SPECIES_ARTICUNO,
    SPECIES_ZAPDOS,
    SPECIES_MOLTRES,
    SPECIES_MEWTWO,
    SPECIES_STARYU,
    SPECIES_SCYTHER,
    SPECIES_JYNX,
    SPECIES_ELECTABUZZ,
    SPECIES_MAGMAR,
    SPECIES_EEVEE,
    SPECIES_ONIX,
    SPECIES_MAGNEMITE,
    SPECIES_SLOWPOKE,
};

static const u16 sQuerySpecies_ProfOak[] =
{
    SPECIES_BULBASAUR,
    SPECIES_CHARMANDER,
    SPECIES_SQUIRTLE,
    SPECIES_PIDGEY,
    SPECIES_PIKACHU,
    SPECIES_NIDORAN_F,
    SPECIES_ZUBAT,
    SPECIES_DIGLETT,
    SPECIES_PSYDUCK,
    SPECIES_MANKEY,
    SPECIES_TENTACOOL,
    SPECIES_PONYTA,
    SPECIES_FARFETCHD,
    SPECIES_DODUO,
    SPECIES_SEEL,
    SPECIES_GRIMER,
    SPECIES_SHELLDER,
    SPECIES_DROWZEE,
    SPECIES_KRABBY,
    SPECIES_VOLTORB,
    SPECIES_CUBONE,
    SPECIES_HITMONLEE,
    SPECIES_HITMONCHAN,
    SPECIES_LICKITUNG,
    SPECIES_KOFFING,
    SPECIES_CHANSEY,
    SPECIES_TANGELA,
    SPECIES_KANGASKHAN,
    SPECIES_MR_MIME,
    SPECIES_PINSIR,
    SPECIES_TAUROS,
    SPECIES_MAGIKARP,
    SPECIES_LAPRAS,
    SPECIES_DITTO,
    SPECIES_PORYGON,
    SPECIES_OMANYTE,
    SPECIES_KABUTO,
    SPECIES_AERODACTYL,
    SPECIES_SNORLAX,
    SPECIES_ARTICUNO,
    SPECIES_ZAPDOS,
    SPECIES_MOLTRES,
    SPECIES_DRATINI,
    SPECIES_MEWTWO,
    SPECIES_MEW,
#ifdef ROGUE_EXPANSION
    SPECIES_SANDSHREW_ALOLAN,
    SPECIES_VULPIX_ALOLAN,
    SPECIES_DIGLETT_ALOLAN,
    SPECIES_MEOWTH_ALOLAN,
    SPECIES_GEODUDE_ALOLAN,
    SPECIES_GRIMER_ALOLAN,

    SPECIES_MEOWTH_GALARIAN,
    SPECIES_PONYTA_GALARIAN,
    SPECIES_SLOWPOKE_GALARIAN,
    SPECIES_FARFETCHD_GALARIAN,
    SPECIES_MR_MIME_GALARIAN,
    SPECIES_ARTICUNO_GALARIAN,
    SPECIES_ZAPDOS_GALARIAN,
    SPECIES_MOLTRES_GALARIAN,
#endif
};

static const u16 sQuerySpecies_Kate[] =
{
    SPECIES_VENUSAUR,
    SPECIES_ARBOK,
    SPECIES_NIDOKING,
    SPECIES_GENGAR,
    SPECIES_TENTACRUEL,
    SPECIES_MUK,
    SPECIES_VILEPLUME,
    SPECIES_NIDOQUEEN,
    SPECIES_GOLBAT,
    SPECIES_VICTREEBEL,
    SPECIES_WEEZING,
    SPECIES_CLEFABLE,
    SPECIES_BEEDRILL,

    SPECIES_CROBAT,
    SPECIES_HOUNDOOM,
    SPECIES_ARIADOS,
    SPECIES_HERACROSS,

    SPECIES_MIGHTYENA,
    SPECIES_SABLEYE,
    SPECIES_SWALOT,
    SPECIES_SEVIPER,
    SPECIES_BANETTE,
    SPECIES_ABSOL,
    SPECIES_SHARPEDO,
#ifdef ROGUE_EXPANSION
    SPECIES_FROSLASS,
    SPECIES_DUSKNOIR,
    SPECIES_DRIFBLIM,
    SPECIES_MISMAGIUS,
    SPECIES_HONCHKROW,
    SPECIES_SKUNTANK,
    SPECIES_TOXICROAK,
    SPECIES_WEAVILE,
    SPECIES_ROTOM,
    SPECIES_ROTOM_HEAT,
    SPECIES_ROTOM_WASH,
    SPECIES_ROTOM_FROST,
    SPECIES_ROTOM_FAN,
    SPECIES_ROTOM_MOW,
    SPECIES_GARCHOMP,

    SPECIES_SCOLIPEDE,
    SPECIES_KROOKODILE,
    SPECIES_SCRAFTY,
    SPECIES_COFAGRIGUS,
    SPECIES_GARBODOR,
    SPECIES_AMOONGUSS,
    SPECIES_JELLICENT,
    SPECIES_CHANDELURE,
    SPECIES_HAXORUS,
    SPECIES_BISHARP,
    SPECIES_GOLURK,
    SPECIES_HYDREIGON,

    SPECIES_PANGORO,
    SPECIES_MEOWSTIC,
    SPECIES_AEGISLASH,
    SPECIES_DRAGALGE,
    SPECIES_TYRANTRUM,
    SPECIES_GOURGEIST,
    SPECIES_NOIVERN,

    SPECIES_DECIDUEYE,
    SPECIES_RATICATE_ALOLAN,
    SPECIES_PERSIAN_ALOLAN,
    SPECIES_MUK_ALOLAN,
    SPECIES_SALAZZLE,
    SPECIES_MAROWAK_ALOLAN,
    SPECIES_TREVENANT,
    SPECIES_NINETALES_ALOLAN,
    SPECIES_DHELMISE,

    SPECIES_THIEVUL,
    SPECIES_OBSTAGOON,
    SPECIES_WEEZING_GALARIAN,
    SPECIES_MIMIKYU,
    SPECIES_TOXTRICITY,
    SPECIES_DRAGAPULT,
#endif
};

static const u16 sQuerySpecies_Erma[] =
{
    SPECIES_ARBOK,
    SPECIES_SANDSLASH,
    SPECIES_NINETALES,
    SPECIES_VENOMOTH,
    SPECIES_ARCANINE,
    SPECIES_VICTREEBEL,
    SPECIES_SLOWBRO,
    SPECIES_CLOYSTER,
    SPECIES_GENGAR,
    SPECIES_HYPNO,
    SPECIES_LAPRAS,
    SPECIES_FLAREON,
    SPECIES_KABUTOPS,
    SPECIES_ARTICUNO,
    SPECIES_DRAGONITE,

    SPECIES_MEGANIUM,
    SPECIES_ARIADOS,
    SPECIES_LANTURN,
    SPECIES_AMPHAROS,
    SPECIES_JUMPLUFF,
    SPECIES_QUAGSIRE,
    SPECIES_SCIZOR,
    SPECIES_TYRANITAR,
    SPECIES_LUGIA,

    SPECIES_DUSTOX,
    SPECIES_SHIFTRY,
    SPECIES_GARDEVOIR,
    SPECIES_MASQUERAIN,
    SPECIES_DELCATTY,
    SPECIES_AGGRON,
    SPECIES_FLYGON,
    SPECIES_LUNATONE,
    SPECIES_BANETTE,
    SPECIES_DUSCLOPS,
    SPECIES_SALAMENCE,
    SPECIES_METAGROSS,
    SPECIES_LATIAS,
    SPECIES_GROUDON,
    SPECIES_JIRACHI,

#ifdef ROGUE_EXPANSION
    SPECIES_SANDSLASH_ALOLAN,
    SPECIES_NINETALES_ALOLAN,
    SPECIES_RAPIDASH_GALARIAN,

    SPECIES_EMPOLEON,
    SPECIES_LUXRAY,
    SPECIES_ROSERADE,
    SPECIES_RAMPARDOS,
    SPECIES_VESPIQUEN,
    SPECIES_PACHIRISU,
    SPECIES_LOPUNNY,
    SPECIES_MISMAGIUS,
    SPECIES_SPIRITOMB,
    SPECIES_GARCHOMP,
    SPECIES_DRAPION,
    SPECIES_MAGNEZONE,
    SPECIES_TOGEKISS,
    SPECIES_GLACEON,
    SPECIES_GLISCOR,
    SPECIES_FROSLASS,
    SPECIES_DIALGA,
    SPECIES_HEATRAN,
    SPECIES_DARKRAI,

    SPECIES_ZEBSTRIKA,
    SPECIES_GIGALITH,
    SPECIES_EXCADRILL,
    SPECIES_LEAVANNY,
    SPECIES_SCOLIPEDE,
    SPECIES_WHIMSICOTT,
    SPECIES_LILLIGANT,
    SPECIES_CRUSTLE,
    SPECIES_ARCHEOPS,
    SPECIES_ZOROARK,
    SPECIES_EMOLGA,
    SPECIES_FERROTHORN,
    SPECIES_EELEKTROSS,
    SPECIES_CHANDELURE,
    SPECIES_MIENSHAO,
    SPECIES_BISHARP,
    SPECIES_HYDREIGON,
    SPECIES_VOLCARONA,
    SPECIES_RESHIRAM,
    SPECIES_MELOETTA,

    SPECIES_GOGOAT,
    SPECIES_MALAMAR,
    SPECIES_DRAGALGE,
    SPECIES_TYRANTRUM,
    SPECIES_AURORUS,
    SPECIES_CARBINK,
    SPECIES_TREVENANT,
    SPECIES_GOURGEIST,
    SPECIES_XERNEAS,
    SPECIES_DIANCIE,
    SPECIES_TOXAPEX,
    SPECIES_LURANTIS,
    SPECIES_TSAREENA,
    SPECIES_MIMIKYU,
    SPECIES_TAPU_LELE,
    SPECIES_TAPU_FINI,
    SPECIES_LUNALA,
    SPECIES_NIHILEGO,
    SPECIES_MAGEARNA,

    SPECIES_HATTERENE,
    SPECIES_ALCREMIE,
    SPECIES_FROSMOTH,
    SPECIES_DRACOVISH,
    SPECIES_DURALUDON,
    SPECIES_DRAGAPULT,
    SPECIES_REGIELEKI,
#endif
};

static const u16 sQuerySpecies_Tails[] =
{
    SPECIES_NINETALES,
    SPECIES_CLEFABLE,
    SPECIES_GYARADOS,
    SPECIES_STARMIE,
    SPECIES_GENGAR,
    SPECIES_MAGNETON,
    SPECIES_MACHAMP,
    SPECIES_VAPOREON,
    SPECIES_JOLTEON,
    SPECIES_FLAREON,
    SPECIES_LAPRAS,
    SPECIES_DRAGONITE,
    SPECIES_MEWTWO,
    SPECIES_MEW,

    SPECIES_CROBAT,
    SPECIES_LANTURN,
    SPECIES_AMPHAROS,
    SPECIES_AZUMARILL,
    SPECIES_SCIZOR,
    SPECIES_HERACROSS,
    SPECIES_ESPEON,
    SPECIES_UMBREON,
    SPECIES_SKARMORY,
    SPECIES_HOUNDOOM,
    SPECIES_KINGDRA,
    SPECIES_PORYGON2,
    SPECIES_TYRANITAR,
    SPECIES_LUGIA,
    SPECIES_HO_OH,

    SPECIES_GARDEVOIR,
    SPECIES_BRELOOM,
    SPECIES_SHEDINJA,
    SPECIES_AGGRON,
    SPECIES_SHARPEDO,
    SPECIES_FLYGON,
    SPECIES_MILOTIC,
    SPECIES_WALREIN,
    SPECIES_SALAMENCE,
    SPECIES_METAGROSS,
    SPECIES_KYOGRE,
    SPECIES_GROUDON,
    SPECIES_RAYQUAZA,

#ifdef ROGUE_EXPANSION
    SPECIES_RAICHU_ALOLAN,
    SPECIES_NINETALES_ALOLAN,
    SPECIES_MAGNEZONE,
    SPECIES_RAPIDASH_GALARIAN,

    SPECIES_PORYGON_Z,

    SPECIES_STARAPTOR,
    SPECIES_ROSERADE,
    SPECIES_SPIRITOMB,
    SPECIES_GARCHOMP,
    SPECIES_LUCARIO,
    SPECIES_DRAPION,
    SPECIES_WEAVILE,
    SPECIES_ELECTIVIRE,
    SPECIES_MAGMORTAR,
    SPECIES_TOGEKISS,
    SPECIES_YANMEGA,
    SPECIES_LEAFEON,
    SPECIES_GLACEON,
    SPECIES_GALLADE,
    SPECIES_FROSLASS,
    SPECIES_DUSKNOIR,
    SPECIES_ROTOM_WASH,
    SPECIES_ROTOM_FAN,
    SPECIES_DIALGA,
    SPECIES_PALKIA,
    SPECIES_GIRATINA,

    SPECIES_EXCADRILL,
    SPECIES_SCOLIPEDE,
    SPECIES_WHIMSICOTT,
    SPECIES_KROOKODILE,
    SPECIES_SCRAFTY,
    SPECIES_COFAGRIGUS,
    SPECIES_ZOROARK,
    SPECIES_EMOLGA,
    SPECIES_ESCAVALIER,
    SPECIES_CHANDELURE,
    SPECIES_EELEKTROSS,
    SPECIES_VOLCARONA,

    SPECIES_PANGORO,
    SPECIES_AEGISLASH,
    SPECIES_AROMATISSE,
    SPECIES_KLEFKI,
    SPECIES_TREVENANT,
    SPECIES_SYLVEON,
    SPECIES_XERNEAS,
    SPECIES_YVELTAL,
    
    SPECIES_TOXAPEX,
    SPECIES_BEWEAR,
    SPECIES_TOGEDEMARU,
    SPECIES_MIMIKYU,
    SPECIES_TAPU_KOKO,
    SPECIES_TAPU_LELE,
    SPECIES_TAPU_BULU,
    SPECIES_TAPU_FINI,

    SPECIES_CORVIKNIGHT,
    SPECIES_CENTISKORCH,
    SPECIES_GRIMMSNARL,
    SPECIES_DURALUDON,
#endif
};

static const u16 sQuerySpecies_Raven[] =
{
    SPECIES_NIDOKING,
    SPECIES_VILEPLUME,
    SPECIES_RAICHU,
    SPECIES_DRAGONITE,
    SPECIES_FLAREON,
    SPECIES_VAPOREON,
    SPECIES_ARTICUNO,
    SPECIES_RAPIDASH,

    SPECIES_FERALIGATR,
    SPECIES_AMPHAROS,
    SPECIES_MISDREAVUS,
    SPECIES_BELLOSSOM,
    SPECIES_HO_OH,

    SPECIES_BLAZIKEN,
    SPECIES_MAWILE,
    SPECIES_ALTARIA,
    SPECIES_BANETTE,
    SPECIES_JIRACHI,

#ifdef ROGUE_EXPANSION
    SPECIES_PACHIRISU,
    SPECIES_MISMAGIUS,
    SPECIES_HONCHKROW,
    SPECIES_VESPIQUEN,
    SPECIES_EMPOLEON,
    SPECIES_MANAPHY,

    SPECIES_WHIMSICOTT,
    SPECIES_SCOLIPEDE,
    SPECIES_KROOKODILE,
    SPECIES_VOLCARONA,
    SPECIES_BRAVIARY,
    SPECIES_CINCCINO,

    SPECIES_GOODRA,
    SPECIES_PANGORO,
    SPECIES_MEOWSTIC,
    SPECIES_CLAWITZER,
    SPECIES_GOURGEIST,
    SPECIES_DIANCIE,

    SPECIES_PRIMARINA,
    SPECIES_RAICHU_ALOLAN,
    SPECIES_BEWEAR,
    SPECIES_TAPU_LELE,

    SPECIES_DRACOZOLT,
    SPECIES_RAPIDASH_GALARIAN,
    SPECIES_ALCREMIE
#endif
};

static const struct RogueTrainer sRouteTrainers_Bosses[] = 
{
    // Kanto Gyms
    //
    {
        .trainerName = _("BROCK"),
        .objectEventGfx = OBJ_EVENT_GFX_KANTO_BROCK,
        .trainerPic = TRAINER_PIC_KANTO_BROCK,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_KANTO | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_ROCK, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_GROUND, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },

    {
        .trainerName = _("MISTY"),
        .objectEventGfx = OBJ_EVENT_GFX_KANTO_MISTY,
        .trainerPic = TRAINER_PIC_KANTO_MISTY,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_KANTO | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_WATER, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_FLYING, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },

    {
        .trainerName = _("LT. SURGE"),
        .objectEventGfx = OBJ_EVENT_GFX_KANTO_LTSURGE,
        .trainerPic = TRAINER_PIC_KANTO_LTSURGE,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_KANTO | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_ELECTRIC, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_FIGHTING, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },

    {
        .trainerName = _("ERIKA"),
        .objectEventGfx = OBJ_EVENT_GFX_KANTO_ERIKA,
        .trainerPic = TRAINER_PIC_KANTO_ERIKA,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_KANTO | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_GRASS, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
#ifdef ROGUE_EXPANSION
            {
                .monCount = 6,
                .incTypes = { TYPE_FAIRY, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
#endif
            {
                .monCount = 6,
                .incTypes = { TYPE_POISON, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },

    {
        .trainerName = _("KOGA"),
        .objectEventGfx = OBJ_EVENT_GFX_KANTO_KOGA,
        .trainerPic = TRAINER_PIC_KANTO_KOGA,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_KANTO | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_POISON, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_GHOST, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },

    {
        .trainerName = _("SABRINA"),
        .objectEventGfx = OBJ_EVENT_GFX_KANTO_SABRINA,
        .trainerPic = TRAINER_PIC_KANTO_SABRINA,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_KANTO | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_PSYCHIC, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_DARK, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_GHOST, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },

    {
        .trainerName = _("BLAINE"),
        .objectEventGfx = OBJ_EVENT_GFX_KANTO_BLAINE,
        .trainerPic = TRAINER_PIC_KANTO_BLAINE,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_KANTO | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_FIRE, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_GROUND, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },

    {
        .trainerName = _("GIOVANNI"),
        .objectEventGfx = OBJ_EVENT_GFX_KANTO_GIOVANNI,
        .trainerPic = TRAINER_PIC_KANTO_GIOVANNI,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_KANTO | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_GROUND, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_POISON, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },

    // Kanto Elite
    //

    {
        .trainerName = _("LORELEI"),
        .objectEventGfx = OBJ_EVENT_GFX_KANTO_LORELEI,
        .trainerPic = TRAINER_PIC_KANTO_LORELEI,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_KANTO | TRAINER_FLAG_ELITE,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_ICE, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_WATER, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },

    {
        .trainerName = _("BRUNO"),
        .objectEventGfx = OBJ_EVENT_GFX_KANTO_BRUNO,
        .trainerPic = TRAINER_PIC_KANTO_BRUNO,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_KANTO | TRAINER_FLAG_ELITE,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_FIGHTING, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_GROUND, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_STEEL, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },

    {
        .trainerName = _("AGATHA"),
        .objectEventGfx = OBJ_EVENT_GFX_KANTO_AGATHA,
        .trainerPic = TRAINER_PIC_KANTO_AGATHA,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_KANTO | TRAINER_FLAG_ELITE,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_GHOST, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_POISON, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_BUG, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },

    {
        .trainerName = _("LANCE"),
        .objectEventGfx = OBJ_EVENT_GFX_KANTO_LANCE,
        .trainerPic = TRAINER_PIC_KANTO_LANCE,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_NONE,
        .trainerFlags = TRAINER_FLAG_KANTO | TRAINER_FLAG_ELITE,
        .monGenerators = 
        {
            {
                .monCount = 3,
                .incTypes = { TYPE_DRAGON, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_FLYING, TYPE_NONE },
                .excTypes = { TYPE_NORMAL, TYPE_NONE },
            },
        }
    },

    // Kanto Champions
    //

    {
        .trainerName = _("BLUE"),
        .objectEventGfx = OBJ_EVENT_GFX_KANTO_BLUE,
        .trainerPic = TRAINER_PIC_KANTO_BLUE,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_NONE,
        .trainerFlags = TRAINER_FLAG_KANTO | TRAINER_FLAG_PRE_CHAMP,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .generatorFlags = TRAINER_GENERATOR_FLAG_UNIQUE_COVERAGE,
                .incTypes = { TYPE_NONE },
                .excTypes = { TYPE_NONE },
            }
        },
        .aceMonGenerators = 
        {
            {
                .monCount = 6,
                .generatorFlags = TRAINER_GENERATOR_FLAG_UNIQUE_COVERAGE | TRAINER_GENERATOR_FLAG_LEGENDARY_ONLY,
                .incTypes = { TYPE_NONE },
                .excTypes = { TYPE_NONE },
            }
        }
    },

    {
        .trainerName = _("PROF. OAK"),
        .objectEventGfx = OBJ_EVENT_GFX_KANTO_PROFOAK,
        .trainerPic = TRAINER_PIC_KANTO_PROFOAK,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_NONE,
        .trainerFlags = TRAINER_FLAG_KANTO | TRAINER_FLAG_FINAL_CHAMP,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .generatorFlags = TRAINER_GENERATOR_FLAG_UNIQUE_COVERAGE,
                .incTypes = { TYPE_NONE },
                .excTypes = { TYPE_NONE },
            }
        },
        .aceMonGenerators = 
        {
            {
                .monCount = 6,
                .generatorFlags = TRAINER_GENERATOR_FLAG_UNIQUE_COVERAGE | TRAINER_GENERATOR_FLAG_LEGENDARY_ONLY,
                .incTypes = { TYPE_NONE },
                .excTypes = { TYPE_NONE },
            }
        }
    },

    // Johto Gyms
    //
    {
        .trainerName = _("FALKNER"),
        .objectEventGfx = OBJ_EVENT_GFX_JOHTO_FALKNER,
        .trainerPic = TRAINER_PIC_JOHTO_FALKNER,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_JOHTO | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_FLYING, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            }
        }
    },

    {
        .trainerName = _("BUGSY"),
        .objectEventGfx = OBJ_EVENT_GFX_JOHTO_BUGSY,
        .trainerPic = TRAINER_PIC_JOHTO_BUGSY,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_JOHTO | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_BUG, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            }
        }
    },

    {
        .trainerName = _("WHITNEY"),
        .objectEventGfx = OBJ_EVENT_GFX_JOHTO_WHITNEY,
        .trainerPic = TRAINER_PIC_JOHTO_WHITNEY,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_JOHTO | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_NORMAL, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            }
        }
    },

    {
        .trainerName = _("MORTY"),
        .objectEventGfx = OBJ_EVENT_GFX_JOHTO_MORTY,
        .trainerPic = TRAINER_PIC_JOHTO_MORTY,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DROUGHT,
        .trainerFlags = TRAINER_FLAG_JOHTO | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_GHOST, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_FIRE, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            }
        }
    },

    {
        .trainerName = _("CHUCK"),
        .objectEventGfx = OBJ_EVENT_GFX_JOHTO_CHUCK,
        .trainerPic = TRAINER_PIC_JOHTO_CHUCK,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_JOHTO | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_FIGHTING, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            }
        }
    },

    {
        .trainerName = _("JASMINE"),
        .objectEventGfx = OBJ_EVENT_GFX_JOHTO_JASMINE,
        .trainerPic = TRAINER_PIC_JOHTO_JASMINE,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_JOHTO | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_STEEL, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            }
        }
    },

    {
        .trainerName = _("PRYCE"),
        .objectEventGfx = OBJ_EVENT_GFX_JOHTO_PRYCE,
        .trainerPic = TRAINER_PIC_JOHTO_PRYCE,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_JOHTO | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_ICE, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_ROCK, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            }
        }
    },

    {
        .trainerName = _("CLAIR"),
        .objectEventGfx = OBJ_EVENT_GFX_JOHTO_CLAIR,
        .trainerPic = TRAINER_PIC_JOHTO_CLAIR,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE,
        .preferredWeather = WEATHER_RAIN,
        .trainerFlags = TRAINER_FLAG_JOHTO | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 3,
                .incTypes = { TYPE_DRAGON, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_WATER, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            }
        }
    },

    // Johto Elite
    //

    {
        .trainerName = _("WILL"),
        .objectEventGfx = OBJ_EVENT_GFX_JOHTO_WILL,
        .trainerPic = TRAINER_PIC_JOHTO_WILL,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_JOHTO | TRAINER_FLAG_ELITE,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_PSYCHIC, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
#ifdef ROGUE_EXPANSION
            {
                .monCount = 6,
                .incTypes = { TYPE_FAIRY, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
#endif
            {
                .monCount = 6,
                .incTypes = { TYPE_DARK, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            }
        }
    },

    {
        .trainerName = _("KOGA"),
        .objectEventGfx = OBJ_EVENT_GFX_KANTO_KOGA,
        .trainerPic = TRAINER_PIC_KANTO_KOGA,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_JOHTO | TRAINER_FLAG_ELITE,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_POISON, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_DARK, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_FIGHTING, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            }
        }
    },

    {
        .trainerName = _("BRUNO"),
        .objectEventGfx = OBJ_EVENT_GFX_KANTO_BRUNO,
        .trainerPic = TRAINER_PIC_KANTO_BRUNO,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_JOHTO | TRAINER_FLAG_ELITE,
        .monGenerators = 
        {
            {
                .monCount = 3,
                .incTypes = { TYPE_FIGHTING, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_STEEL, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_GROUND, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            }
        }
    },

    {
        .trainerName = _("KAREN"),
        .objectEventGfx = OBJ_EVENT_GFX_JOHTO_KAREN,
        .trainerPic = TRAINER_PIC_JOHTO_KAREN,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_JOHTO | TRAINER_FLAG_ELITE,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_DARK, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_NORMAL, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            }
        }
    },

    // Johto Champions
    //
    {
        .trainerName = _("LANCE"),
        .objectEventGfx = OBJ_EVENT_GFX_KANTO_LANCE,
        .trainerPic = TRAINER_PIC_KANTO_LANCE,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_NONE,
        .trainerFlags = TRAINER_FLAG_JOHTO | TRAINER_FLAG_PRE_CHAMP,
        .monGenerators = 
        {
            {
                .monCount = 3,
                .incTypes = { TYPE_DRAGON, TYPE_FLYING },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_DRAGON, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            }
        }
    },

    {
        .trainerName = _("RED"),
        .objectEventGfx = OBJ_EVENT_GFX_JOHTO_RED,
        .trainerPic = TRAINER_PIC_JOHTO_RED,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_JOHTO | TRAINER_FLAG_FINAL_CHAMP,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .generatorFlags = TRAINER_GENERATOR_FLAG_UNIQUE_COVERAGE,
                .incTypes = { TYPE_NONE },
                .excTypes = { TYPE_NONE },
            }
        },
        .aceMonGenerators = 
        {
            {
                .monCount = 6,
                .generatorFlags = TRAINER_GENERATOR_FLAG_UNIQUE_COVERAGE | TRAINER_GENERATOR_FLAG_LEGENDARY_ONLY,
                .incTypes = { TYPE_NONE },
                .excTypes = { TYPE_NONE },
            }
        }
    },
    
    // Hoenn Gyms
    //
    {
        .trainerName = _("ROXANNE"),
        .objectEventGfx = OBJ_EVENT_GFX_ROXANNE,
        .trainerPic = TRAINER_PIC_LEADER_ROXANNE,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_ROCK, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_STEEL, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
    
    {
        .trainerName = _("BRAWLY"),
        .objectEventGfx = OBJ_EVENT_GFX_BRAWLY,
        .trainerPic = TRAINER_PIC_LEADER_BRAWLY,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_FIGHTING, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_DARK, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },

    {
        .trainerName = _("WATTSON"),
        .objectEventGfx = OBJ_EVENT_GFX_WATTSON,
        .trainerPic = TRAINER_PIC_LEADER_WATTSON,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_ELECTRIC, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_STEEL, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
    
    {
        .trainerName = _("FLANNERY"),
        .objectEventGfx = OBJ_EVENT_GFX_FLANNERY,
        .trainerPic = TRAINER_PIC_LEADER_FLANNERY,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_FIRE, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_GROUND, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
    
    {
        .trainerName = _("NORMAN"),
        .objectEventGfx = OBJ_EVENT_GFX_NORMAN,
        .trainerPic = TRAINER_PIC_LEADER_NORMAN,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_NORMAL, TYPE_NONE },
                .excTypes = { TYPE_FLYING, TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_GHOST, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        },
        .aceMonGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_DRAGON, TYPE_NONE },
                .excTypes = { TYPE_NONE },
                .generatorFlags = TRAINER_GENERATOR_FLAG_LEGENDARY_ONLY,
            },
        }
    },
    
    {
        .trainerName = _("WINONA"),
        .objectEventGfx = OBJ_EVENT_GFX_WINONA,
        .trainerPic = TRAINER_PIC_LEADER_WINONA,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_FLYING, TYPE_NONE },
                .excTypes = { TYPE_NORMAL, TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_NORMAL, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
    
    {
        .trainerName = _("LIZA"),
        .objectEventGfx = OBJ_EVENT_GFX_LIZA,
        .trainerPic = TRAINER_PIC_LEADER_TATE_AND_LIZA,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_PSYCHIC, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_GHOST, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
    
    {
        .trainerName = _("JUAN"),
        .objectEventGfx = OBJ_EVENT_GFX_JUAN,
        .trainerPic = TRAINER_PIC_LEADER_JUAN,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_HOENN | TRAINER_FLAG_GYM,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_WATER, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_ICE, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
    
    // Hoenn Elite
    //
    {
        .trainerName = _("SIDNEY"),
        .objectEventGfx = OBJ_EVENT_GFX_SIDNEY,
        .trainerPic = TRAINER_PIC_ELITE_FOUR_SIDNEY,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_HOENN | TRAINER_FLAG_ELITE,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_DARK, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_FIGHTING, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },

    {
        .trainerName = _("PHOEBE"),
        .objectEventGfx = OBJ_EVENT_GFX_PHOEBE,
        .trainerPic = TRAINER_PIC_ELITE_FOUR_PHOEBE,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_HOENN | TRAINER_FLAG_ELITE,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_GHOST, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_POISON, TYPE_BUG },
                .excTypes = { TYPE_NONE },
            },
        },
        .aceMonGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_PSYCHIC, TYPE_NONE },
                .excTypes = { TYPE_NONE },
                .generatorFlags = TRAINER_GENERATOR_FLAG_LEGENDARY_ONLY,
            },
        }
    },

    {
        .trainerName = _("GLACIA"),
        .objectEventGfx = OBJ_EVENT_GFX_GLACIA,
        .trainerPic = TRAINER_PIC_ELITE_FOUR_GLACIA,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_HOENN | TRAINER_FLAG_ELITE,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_ICE, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_WATER, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },

    {
        .trainerName = _("DRAKE"),
        .objectEventGfx = OBJ_EVENT_GFX_DRAKE,
        .trainerPic = TRAINER_PIC_ELITE_FOUR_DRAKE,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_HOENN | TRAINER_FLAG_ELITE,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_DRAGON, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_FIRE, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
    
    // Hoenn Champs
    //
    {
        .trainerName = _("WALLACE"),
        .objectEventGfx = OBJ_EVENT_GFX_WALLACE,
        .trainerPic = TRAINER_PIC_CHAMPION_WALLACE,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_HOENN | TRAINER_FLAG_PRE_CHAMP,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_WATER, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_DRAGON, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },

    {
        .trainerName = _("STEVEN"),
        .objectEventGfx = OBJ_EVENT_GFX_STEVEN,
        .trainerPic = TRAINER_PIC_STEVEN,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_HOENN | TRAINER_FLAG_FINAL_CHAMP,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_STEEL, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_GROUND, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        },
        .aceMonGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_PSYCHIC, TYPE_NONE },
                .excTypes = { TYPE_NONE },
                .generatorFlags = TRAINER_GENERATOR_FLAG_LEGENDARY_ONLY,
            },
        }
    },

    // Placeholder Rainbow mode bosses for missing types
#ifdef ROGUE_EXPANSION
    {
        .trainerName = _("ANABEL"),
        .objectEventGfx = OBJ_EVENT_GFX_ANABEL,
        .trainerPic = TRAINER_PIC_SALON_MAIDEN_ANABEL,
        .trainerClass = TRAINER_CLASS_LEADER,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_NONE,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_FAIRY, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_PSYCHIC, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        },
    },
#endif

	// TODO - Glitch set
};


static const struct RogueTrainer sRouteTrainers_MiniBosses[] = 
{
    {
        .trainerName = _("MAXIE"),
        .objectEventGfx = OBJ_EVENT_GFX_MAXIE,
        .trainerPic = TRAINER_PIC_MAGMA_LEADER_MAXIE,
        .trainerClass = TRAINER_CLASS_MAGMA_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MAGMA,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_FIRE, TYPE_DARK },
                .excTypes = { TYPE_WATER, TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_STEEL, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
    {
        .trainerName = _("ARCHIE"),
        .objectEventGfx = OBJ_EVENT_GFX_ARCHIE,
        .trainerPic = TRAINER_PIC_AQUA_LEADER_ARCHIE,
        .trainerClass = TRAINER_CLASS_AQUA_LEADER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_AQUA,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_WATER, TYPE_DARK },
                .excTypes = { TYPE_FIRE, TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_ICE, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },

    {
        .trainerName = _("WALLY"),
        .objectEventGfx = OBJ_EVENT_GFX_WALLY,
        .trainerPic = TRAINER_PIC_WALLY,
        .trainerClass = TRAINER_CLASS_RIVAL,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_PSYCHIC, TYPE_NORMAL },
                .excTypes = { TYPE_FLYING, TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_DARK, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },

    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_PLAYER_RIVAL,
        .trainerPic = TRAINER_PIC_PLAYER_OPPOSITE_AVATAR,
        .trainerClass = TRAINER_CLASS_RIVAL,
        .encounterMusic_gender = 0,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION | TRAINER_FLAG_NAME_IS_OPPOSITE_AVATAR,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .generatorFlags = TRAINER_GENERATOR_FLAG_UNIQUE_COVERAGE,
                .incTypes = { TYPE_NONE },
                .excTypes = { TYPE_NONE },
            }
        }
    },
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_PLAYER_AVATAR,
        .trainerPic = TRAINER_PIC_PLAYER_AVATAR,
        .trainerClass = TRAINER_CLASS_RIVAL,
        .encounterMusic_gender = 0,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION | TRAINER_FLAG_NAME_IS_PLAYER,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .generatorFlags = TRAINER_GENERATOR_FLAG_MIRROR_EXACT,
                .incTypes = { TYPE_NONE },
                .excTypes = { TYPE_NONE },
            }
        }
    },

    {
        .trainerName = _("LUCY"),
        .objectEventGfx = OBJ_EVENT_GFX_LUCY,
        .trainerPic = TRAINER_PIC_PIKE_QUEEN_LUCY,
        .trainerClass = TRAINER_CLASS_PIKE_QUEEN,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_POISON, TYPE_NONE },
                .excTypes = { TYPE_GRASS, TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_GHOST, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
    {
        .trainerName = _("BRANDON"),
        .objectEventGfx = OBJ_EVENT_GFX_BRANDON,
        .trainerPic = TRAINER_PIC_PYRAMID_KING_BRANDON,
        .trainerClass = TRAINER_CLASS_PYRAMID_KING,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_GROUND, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_STEEL, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
    {
        .trainerName = _("TUCKER"),
        .objectEventGfx = OBJ_EVENT_GFX_TUCKER,
        .trainerPic = TRAINER_PIC_DOME_ACE_TUCKER,
        .trainerClass = TRAINER_CLASS_DOME_ACE,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_BUG, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_FLYING, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
    {
        .trainerName = _("SPENSER"),
        .objectEventGfx = OBJ_EVENT_GFX_SPENSER,
        .trainerPic = TRAINER_PIC_PALACE_MAVEN_SPENSER,
        .trainerClass = TRAINER_CLASS_PALACE_MAVEN,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_GRASS, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_NORMAL, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
    {
        .trainerName = _("GRETA"),
        .objectEventGfx = OBJ_EVENT_GFX_GRETA,
        .trainerPic = TRAINER_PIC_ARENA_TYCOON_GRETA,
        .trainerClass = TRAINER_CLASS_ARENA_TYCOON,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_FIGHTING, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_DARK, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
    {
        .trainerName = _("NOLAND"),
        .objectEventGfx = OBJ_EVENT_GFX_NOLAND,
        .trainerPic = TRAINER_PIC_FACTORY_HEAD_NOLAND,
        .trainerClass = TRAINER_CLASS_FACTORY_HEAD,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_ICE, TYPE_DRAGON },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_NORMAL, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },

#ifdef ROGUE_EXPANSION
    {
        .trainerName = _("ANABEL"),
        .objectEventGfx = OBJ_EVENT_GFX_ANABEL,
        .trainerPic = TRAINER_PIC_SALON_MAIDEN_ANABEL,
        .trainerClass = TRAINER_CLASS_SALON_MAIDEN,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_FAIRY, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
            {
                .monCount = 6,
                .incTypes = { TYPE_NORMAL, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
#endif
};


static const struct RogueTrainer sRouteTrainers_RouteTrainers[] = 
{
    // Aqua grunts
    {
        .trainerName = _("GRUNT"),
        .objectEventGfx = OBJ_EVENT_GFX_AQUA_MEMBER_M,
        .trainerPic = TRAINER_PIC_AQUA_GRUNT_M,
        .trainerClass = TRAINER_CLASS_TEAM_AQUA,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_AQUA,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_WATER, TYPE_DARK },
                .excTypes = { TYPE_FIRE, TYPE_NONE },
            },
        }
    },
    {
        .trainerName = _("GRUNT"),
        .objectEventGfx = OBJ_EVENT_GFX_AQUA_MEMBER_F,
        .trainerPic = TRAINER_PIC_AQUA_GRUNT_F,
        .trainerClass = TRAINER_CLASS_TEAM_AQUA,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_AQUA,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_WATER, TYPE_DARK },
                .excTypes = { TYPE_FIRE, TYPE_NONE },
            },
        }
    },

    // Magma grunts
    {
        .trainerName = _("GRUNT"),
        .objectEventGfx = OBJ_EVENT_GFX_MAGMA_MEMBER_M,
        .trainerPic = TRAINER_PIC_MAGMA_GRUNT_M,
        .trainerClass = TRAINER_CLASS_TEAM_MAGMA,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MAGMA,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_FIRE, TYPE_DARK },
                .excTypes = { TYPE_WATER, TYPE_NONE },
            },
        }
    },
    {
        .trainerName = _("GRUNT"),
        .objectEventGfx = OBJ_EVENT_GFX_MAGMA_MEMBER_F,
        .trainerPic = TRAINER_PIC_MAGMA_GRUNT_F,
        .trainerClass = TRAINER_CLASS_TEAM_MAGMA,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_MAGMA,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_FIRE, TYPE_DARK },
                .excTypes = { TYPE_WATER, TYPE_NONE },
            },
        }
    },

    // Misc trainers

    // Breeders
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_WOMAN_2,
        .trainerPic = TRAINER_PIC_POKEMON_BREEDER_F,
        .trainerClass = TRAINER_CLASS_PKMN_BREEDER,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = {}
    },
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_MAN_4,
        .trainerPic = TRAINER_PIC_POKEMON_BREEDER_M,
        .trainerClass = TRAINER_CLASS_PKMN_BREEDER,
        .encounterMusic_gender =  TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = {}
    },

    // Cool trainers
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_MAN_3,
        .trainerPic = TRAINER_PIC_COOLTRAINER_M,
        .trainerClass = TRAINER_CLASS_COOLTRAINER,
        .encounterMusic_gender =  TRAINER_ENCOUNTER_MUSIC_COOL,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = {}
    },
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_WOMAN_5,
        .trainerPic = TRAINER_PIC_COOLTRAINER_F,
        .trainerClass = TRAINER_CLASS_COOLTRAINER,
        .encounterMusic_gender =  F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_COOL,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = {}
    },

    // Psychic
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_PSYCHIC_M,
        .trainerPic = TRAINER_PIC_PSYCHIC_M,
        .trainerClass = TRAINER_CLASS_PSYCHIC,
        .encounterMusic_gender =  TRAINER_ENCOUNTER_MUSIC_INTENSE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_PSYCHIC, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_LASS,
        .trainerPic = TRAINER_PIC_PSYCHIC_F,
        .trainerClass = TRAINER_CLASS_PSYCHIC,
        .encounterMusic_gender =  F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_INTENSE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_PSYCHIC, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },

    // Tubers
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_TUBER_M,
        .trainerPic = TRAINER_PIC_TUBER_M,
        .trainerClass = TRAINER_CLASS_TUBER_M,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_GIRL,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = {}
    },
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_TUBER_F,
        .trainerPic = TRAINER_PIC_TUBER_F,
        .trainerClass = TRAINER_CLASS_TUBER_F,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_GIRL,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = {}
    },

    // Swimmer
    //{
    //    .trainerName = _("???"),
    //    .objectEventGfx = OBJ_EVENT_GFX_SWIMMER_M,
    //    .trainerPic = TRAINER_PIC_SWIMMER_M,
    //    .trainerClass = TRAINER_CLASS_SWIMMER_M,
    //    .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_SWIMMER,
    //    .preferredWeather = WEATHER_DEFAULT,
    //    .trainerFlags = TRAINER_FLAG_ANY_REGION,
    //    .monGenerators = {}
    //},
    //{
    //    .trainerName = _("???"),
    //    .objectEventGfx = OBJ_EVENT_GFX_SWIMMER_F,
    //    .trainerPic = TRAINER_PIC_SWIMMER_F,
    //    .trainerClass = TRAINER_CLASS_SWIMMER_F,
    //    .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_SWIMMER,
    //    .preferredWeather = WEATHER_DEFAULT,
    //    .trainerFlags = TRAINER_FLAG_ANY_REGION,
    //    .monGenerators = {}
    //},

    // Expert
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_EXPERT_M,
        .trainerPic = TRAINER_PIC_EXPERT_M,
        .trainerClass = TRAINER_CLASS_EXPERT,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_INTENSE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = {}
    },
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_EXPERT_F,
        .trainerPic = TRAINER_PIC_EXPERT_F,
        .trainerClass = TRAINER_CLASS_EXPERT,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_INTENSE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = {}
    },

    // Triathlete
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_RUNNING_TRIATHLETE_M,
        .trainerPic = TRAINER_PIC_RUNNING_TRIATHLETE_M,
        .trainerClass = TRAINER_CLASS_TRIATHLETE,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = {}
    },
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_RUNNING_TRIATHLETE_F,
        .trainerPic = TRAINER_PIC_RUNNING_TRIATHLETE_F,
        .trainerClass = TRAINER_CLASS_TRIATHLETE,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = {}
    },

    // School kid
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_SCHOOL_KID_M,
        .trainerPic = TRAINER_PIC_SCHOOL_KID_M,
        .trainerClass = TRAINER_CLASS_SCHOOL_KID,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_GIRL,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = {}
    },
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_GIRL_3,
        .trainerPic = TRAINER_PIC_SCHOOL_KID_F,
        .trainerClass = TRAINER_CLASS_SCHOOL_KID,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_GIRL,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = {}
    },

    // One offs

    // Hiker
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_HIKER,
        .trainerPic = TRAINER_PIC_HIKER,
        .trainerClass = TRAINER_CLASS_HIKER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_HIKER,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_GROUND, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
    // Bird Keeper
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_MAN_5,
        .trainerPic = TRAINER_PIC_BIRD_KEEPER,
        .trainerClass = TRAINER_CLASS_BIRD_KEEPER,
        .encounterMusic_gender =  TRAINER_ENCOUNTER_MUSIC_COOL,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_FLYING, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
    // Collector
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_MANIAC,
        .trainerPic = TRAINER_PIC_COLLECTOR,
        .trainerClass = TRAINER_CLASS_COLLECTOR,
        .encounterMusic_gender =  TRAINER_ENCOUNTER_MUSIC_SUSPICIOUS,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = {}
    },
    // Black belt
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_BLACK_BELT,
        .trainerPic = TRAINER_PIC_BLACK_BELT,
        .trainerClass = TRAINER_CLASS_BLACK_BELT,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_INTENSE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_FIGHTING, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
    // Hex maniac
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_HEX_MANIAC,
        .trainerPic = TRAINER_PIC_HEX_MANIAC,
        .trainerClass = TRAINER_CLASS_HEX_MANIAC,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_SUSPICIOUS,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_GHOST, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
    // Ruin maniac
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_BACKPACKER_M,
        .trainerPic = TRAINER_PIC_RUIN_MANIAC,
        .trainerClass = TRAINER_CLASS_RUIN_MANIAC,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_HIKER,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_ROCK, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
    // Beauty
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_BEAUTY,
        .trainerPic = TRAINER_PIC_BEAUTY,
        .trainerClass = TRAINER_CLASS_BEAUTY,
        .encounterMusic_gender = F_TRAINER_FEMALE | TRAINER_ENCOUNTER_MUSIC_FEMALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
#ifdef ROGUE_EXPANSION
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_FAIRY, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
#else
        .monGenerators = {}
#endif
    },
    // Rich boy
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_RICH_BOY,
        .trainerPic = TRAINER_PIC_RICH_BOY,
        .trainerClass = TRAINER_CLASS_RICH_BOY,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_RICH,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = {}
    },
    // Bug catcher
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_BUG_CATCHER,
        .trainerPic = TRAINER_PIC_BUG_CATCHER,
        .trainerClass = TRAINER_CLASS_BUG_CATCHER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_BUG, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
    // Bug catcher
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_GENTLEMAN,
        .trainerPic = TRAINER_PIC_GENTLEMAN,
        .trainerClass = TRAINER_CLASS_GENTLEMAN,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_RICH,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = {}
    },
    // Fisherman
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_FISHERMAN,
        .trainerPic = TRAINER_PIC_FISHERMAN,
        .trainerClass = TRAINER_CLASS_FISHERMAN,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_MALE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_WATER, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
    // Dragon tamer
    {
        .trainerName = _("???"),
        .objectEventGfx = OBJ_EVENT_GFX_BOY_3,
        .trainerPic = TRAINER_PIC_DRAGON_TAMER,
        .trainerClass = TRAINER_CLASS_DRAGON_TAMER,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_INTENSE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = 
        {
            {
                .monCount = 6,
                .incTypes = { TYPE_DRAGON, TYPE_NONE },
                .excTypes = { TYPE_NONE },
            },
        }
    },
};

#ifdef ROGUE_DEBUG

static const struct RogueTrainer sRouteTrainers_DebugTrainers[] = 
{
    // Automation/Debug trainer battle
    {
        .trainerName = _("DEBUG"),
        .objectEventGfx = OBJ_EVENT_GFX_NURSE,
        .trainerPic = TRAINER_PIC_POKABBIE_DITTO,
        .trainerClass = TRAINER_CLASS_RIVAL,
        .encounterMusic_gender = TRAINER_ENCOUNTER_MUSIC_INTENSE,
        .preferredWeather = WEATHER_DEFAULT,
        .trainerFlags = TRAINER_FLAG_ANY_REGION,
        .monGenerators = {}
    },
};
#endif

const struct RogueTrainerCollection gRogueTrainers = 
{
    .bossCount = ARRAY_COUNT(sRouteTrainers_Bosses),
    .boss = sRouteTrainers_Bosses,
    .minibossCount = ARRAY_COUNT(sRouteTrainers_MiniBosses),
    .miniboss = sRouteTrainers_MiniBosses,
    .routeTrainersCount = ARRAY_COUNT(sRouteTrainers_RouteTrainers),
    .routeTrainers = sRouteTrainers_RouteTrainers,
#ifdef ROGUE_DEBUG
    .debugTrainersCount = ARRAY_COUNT(sRouteTrainers_DebugTrainers),
    .debugTrainers = sRouteTrainers_DebugTrainers,
#endif
};
