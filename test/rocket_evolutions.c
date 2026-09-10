#include "global.h"
#include "test/test.h"
#include "pokemon.h"
#include "item.h"
#include "item_use.h"
#include "rogue_team_rocket.h"
#include "constants/items.h"
#include "constants/pokemon.h"

struct RocketEvolutionRoute
{
    u16 sourceSpecies;
    u16 targetSpecies;
};

static const struct RocketEvolutionRoute sRocketEvolutionRoutes[] =
{
    { SPECIES_EKANS, SPECIES_ARBOK_ROCKET },
    { SPECIES_PIKACHU, SPECIES_RAICHU_ROCKET },
    { SPECIES_MACHOKE, SPECIES_MACHAMP_ROCKET },
    { SPECIES_TYROGUE, SPECIES_HITMONLEE_ROCKET },
    { SPECIES_VIBRAVA, SPECIES_FLYGON_ROCKET },
    { SPECIES_LUXIO, SPECIES_LUXRAY_ROCKET },
    { SPECIES_POOCHYENA, SPECIES_MIGHTYENA_ROCKET },
    { SPECIES_SEVIPER, SPECIES_SEVIPER_ROCKET },
    { SPECIES_SHUPPET, SPECIES_BANETTE_ROCKET },
    { SPECIES_ABSOL, SPECIES_ABSOL_ROCKET },
    { SPECIES_PHANPY, SPECIES_DONPHAN_ROCKET },
    { SPECIES_VAROOM, SPECIES_REVAVROOM_ROCKET },
    { SPECIES_CHARCADET, SPECIES_CERULEDGE_ROCKET },
    { SPECIES_PINSIR, SPECIES_PINSIR_ROCKET },
    { SPECIES_GOLBAT, SPECIES_CROBAT_ROCKET },
    { SPECIES_MEOWTH, SPECIES_PERSIAN_ROCKET },
    { SPECIES_NIDORINO, SPECIES_NIDOKING_ROCKET },
    { SPECIES_NIDORINA, SPECIES_NIDOQUEEN_ROCKET },
    { SPECIES_KOFFING, SPECIES_WEEZING_ROCKET },
    { SPECIES_DROWZEE, SPECIES_HYPNO_ROCKET },
    { SPECIES_ELECTABUZZ, SPECIES_ELECTIVIRE_ROCKET },
    { SPECIES_MAGMAR, SPECIES_MAGMORTAR_ROCKET },
    { SPECIES_SCYTHER, SPECIES_SCIZOR_ROCKET },
    { SPECIES_MAGIKARP, SPECIES_GYARADOS_ROCKET },
    { SPECIES_AERODACTYL, SPECIES_AERODACTYL_ROCKET },
    { SPECIES_SPINARAK, SPECIES_ARIADOS_ROCKET },
    { SPECIES_NATU, SPECIES_XATU_ROCKET },
    { SPECIES_FLAAFFY, SPECIES_AMPHAROS_ROCKET },
    { SPECIES_BONSLY, SPECIES_SUDOWOODO_ROCKET },
    { SPECIES_MURKROW, SPECIES_HONCHKROW_ROCKET },
    { SPECIES_MISDREAVUS, SPECIES_MISMAGIUS_ROCKET },
    { SPECIES_PINECO, SPECIES_FORRETRESS_ROCKET },
    { SPECIES_GLIGAR, SPECIES_GLISCOR_ROCKET },
    { SPECIES_SNUBBULL, SPECIES_GRANBULL_ROCKET },
    { SPECIES_HOUNDOUR, SPECIES_HOUNDOOM_ROCKET },
};

TEST("TR Rocket Evo: Dubious Disc is a directly usable evolution item")
{
    EXPECT_EQ(
        ItemId_GetType(ITEM_DUBIOUS_DISC),
        ITEM_USE_PARTY_MENU
    );

    EXPECT(
        ItemId_GetFieldFunc(ITEM_DUBIOUS_DISC)
        == ItemUseOutOfBattle_EvolutionStone
    );
}

TEST("TR Rocket Evo: all 35 conversion routes resolve correctly")
{
    u32 i;
    u32 routeIdx = 0;
    struct Pokemon mon;

    for (i = 0; i < ARRAY_COUNT(sRocketEvolutionRoutes); ++i)
        PARAMETRIZE { routeIdx = i; }

    ZeroMonData(&mon);

    CreateMon(
        &mon,
        sRocketEvolutionRoutes[routeIdx].sourceSpecies,
        50,
        31,
        TRUE,
        0x12345678,
        OT_ID_PRESET,
        0x87654321
    );

    EXPECT_EQ(
        RogueTeamRocket_GetEvolutionTarget(
            sRocketEvolutionRoutes[routeIdx].sourceSpecies
        ),
        sRocketEvolutionRoutes[routeIdx].targetSpecies
    );

    EXPECT_EQ(
        GetEvolutionTargetSpecies(
            &mon,
            EVO_MODE_ITEM_USE,
            ITEM_DUBIOUS_DISC,
            NULL
        ),
        sRocketEvolutionRoutes[routeIdx].targetSpecies
    );
}

TEST("TR Rocket Evo: original Porygon2 Dubious Disc evolution still works")
{
    struct Pokemon mon;

    ZeroMonData(&mon);

    CreateMon(
        &mon,
        SPECIES_PORYGON2,
        50,
        31,
        TRUE,
        0x12345678,
        OT_ID_PRESET,
        0x87654321
    );

    EXPECT_EQ(
        GetEvolutionTargetSpecies(
            &mon,
            EVO_MODE_ITEM_USE,
            ITEM_DUBIOUS_DISC,
            NULL
        ),
        SPECIES_PORYGON_Z
    );
}

TEST("TR Rocket Evo: Dubious Disc does not affect unrelated Pokemon")
{
    struct Pokemon mon;

    ZeroMonData(&mon);

    CreateMon(
        &mon,
        SPECIES_BULBASAUR,
        50,
        31,
        TRUE,
        0x12345678,
        OT_ID_PRESET,
        0x87654321
    );

    EXPECT_EQ(
        GetEvolutionTargetSpecies(
            &mon,
            EVO_MODE_ITEM_USE,
            ITEM_DUBIOUS_DISC,
            NULL
        ),
        SPECIES_NONE
    );
}
