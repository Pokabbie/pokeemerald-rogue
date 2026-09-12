#include "global.h"
#include "pokemon.h"
#include "test/battle.h"
#include "rogue_team_rocket.h"

SINGLE_BATTLE_TEST("TR Wave A: TR Persian has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_PERSIAN_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_PERSIAN_ROCKET].baseHP, 65);
        EXPECT_EQ(gSpeciesInfo[SPECIES_PERSIAN_ROCKET].baseAttack, 110);
        EXPECT_EQ(gSpeciesInfo[SPECIES_PERSIAN_ROCKET].baseDefense, 70);
        EXPECT_EQ(gSpeciesInfo[SPECIES_PERSIAN_ROCKET].baseSpeed, 120);
        EXPECT_EQ(gSpeciesInfo[SPECIES_PERSIAN_ROCKET].baseSpAttack, 65);
        EXPECT_EQ(gSpeciesInfo[SPECIES_PERSIAN_ROCKET].baseSpDefense, 70);

        EXPECT_EQ(gSpeciesInfo[SPECIES_PERSIAN_ROCKET].types[0], TYPE_GHOST);
        EXPECT_EQ(gSpeciesInfo[SPECIES_PERSIAN_ROCKET].types[1], TYPE_DARK);

        EXPECT_EQ(gSpeciesInfo[SPECIES_PERSIAN_ROCKET].abilities[0], ABILITY_TOUGH_CLAWS);
        EXPECT_EQ(gSpeciesInfo[SPECIES_PERSIAN_ROCKET].abilities[1], ABILITY_NONE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_PERSIAN_ROCKET].abilities[2], ABILITY_TOUGH_CLAWS);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_PERSIAN_ROCKET].natDexNum,
            NATIONAL_DEX_PERSIAN
        );

        EXPECT_EQ(player->type1, TYPE_GHOST);
        EXPECT_EQ(player->type2, TYPE_DARK);
        EXPECT_EQ(player->ability, ABILITY_TOUGH_CLAWS);
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Persian maps to base species")
{
    GIVEN {
        PLAYER(SPECIES_PERSIAN_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_PERSIAN_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_PERSIAN_ROCKET),
            SPECIES_PERSIAN
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_PERSIAN));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_PERSIAN),
            SPECIES_PERSIAN
        );
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR NidKing has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_NIDOKING_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOKING_ROCKET].baseHP, 81);
        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOKING_ROCKET].baseAttack, 120);
        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOKING_ROCKET].baseDefense, 80);
        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOKING_ROCKET].baseSpeed, 100);
        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOKING_ROCKET].baseSpAttack, 85);
        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOKING_ROCKET].baseSpDefense, 74);

        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOKING_ROCKET].types[0], TYPE_POISON);
        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOKING_ROCKET].types[1], TYPE_DARK);

        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOKING_ROCKET].abilities[0], ABILITY_ADAPTABILITY);
        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOKING_ROCKET].abilities[1], ABILITY_NONE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOKING_ROCKET].abilities[2], ABILITY_ADAPTABILITY);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_NIDOKING_ROCKET].natDexNum,
            NATIONAL_DEX_NIDOKING
        );

        EXPECT_EQ(player->type1, TYPE_POISON);
        EXPECT_EQ(player->type2, TYPE_DARK);
        EXPECT_EQ(player->ability, ABILITY_ADAPTABILITY);
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR NidKing maps to base species")
{
    GIVEN {
        PLAYER(SPECIES_NIDOKING_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_NIDOKING_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_NIDOKING_ROCKET),
            SPECIES_NIDOKING
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_NIDOKING));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_NIDOKING),
            SPECIES_NIDOKING
        );
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR NidQuen has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_NIDOQUEEN_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOQUEEN_ROCKET].baseHP, 100);
        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOQUEEN_ROCKET].baseAttack, 100);
        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOQUEEN_ROCKET].baseDefense, 100);
        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOQUEEN_ROCKET].baseSpeed, 70);
        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOQUEEN_ROCKET].baseSpAttack, 85);
        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOQUEEN_ROCKET].baseSpDefense, 85);

        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOQUEEN_ROCKET].types[0], TYPE_POISON);
        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOQUEEN_ROCKET].types[1], TYPE_STEEL);

        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOQUEEN_ROCKET].abilities[0], ABILITY_EARTH_EATER);
        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOQUEEN_ROCKET].abilities[1], ABILITY_FUR_COAT);
        EXPECT_EQ(gSpeciesInfo[SPECIES_NIDOQUEEN_ROCKET].abilities[2], ABILITY_EARTH_EATER);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_NIDOQUEEN_ROCKET].natDexNum,
            NATIONAL_DEX_NIDOQUEEN
        );

        EXPECT_EQ(player->type1, TYPE_POISON);
        EXPECT_EQ(player->type2, TYPE_STEEL);
        EXPECT_EQ(player->ability, ABILITY_EARTH_EATER);
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR NidQuen maps to base species")
{
    GIVEN {
        PLAYER(SPECIES_NIDOQUEEN_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_NIDOQUEEN_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_NIDOQUEEN_ROCKET),
            SPECIES_NIDOQUEEN
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_NIDOQUEEN));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_NIDOQUEEN),
            SPECIES_NIDOQUEEN
        );
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Weezing has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_WEEZING_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_WEEZING_ROCKET].baseHP, 65);
        EXPECT_EQ(gSpeciesInfo[SPECIES_WEEZING_ROCKET].baseAttack, 95);
        EXPECT_EQ(gSpeciesInfo[SPECIES_WEEZING_ROCKET].baseDefense, 120);
        EXPECT_EQ(gSpeciesInfo[SPECIES_WEEZING_ROCKET].baseSpeed, 70);
        EXPECT_EQ(gSpeciesInfo[SPECIES_WEEZING_ROCKET].baseSpAttack, 90);
        EXPECT_EQ(gSpeciesInfo[SPECIES_WEEZING_ROCKET].baseSpDefense, 80);

        EXPECT_EQ(gSpeciesInfo[SPECIES_WEEZING_ROCKET].types[0], TYPE_POISON);
        EXPECT_EQ(gSpeciesInfo[SPECIES_WEEZING_ROCKET].types[1], TYPE_STEEL);

        EXPECT_EQ(gSpeciesInfo[SPECIES_WEEZING_ROCKET].abilities[0], ABILITY_LEVITATE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_WEEZING_ROCKET].abilities[1], ABILITY_NONE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_WEEZING_ROCKET].abilities[2], ABILITY_LEVITATE);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_WEEZING_ROCKET].natDexNum,
            NATIONAL_DEX_WEEZING
        );

        EXPECT_EQ(player->type1, TYPE_POISON);
        EXPECT_EQ(player->type2, TYPE_STEEL);
        EXPECT_EQ(player->ability, ABILITY_LEVITATE);
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Weezing maps to base species")
{
    GIVEN {
        PLAYER(SPECIES_WEEZING_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_WEEZING_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_WEEZING_ROCKET),
            SPECIES_WEEZING
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_WEEZING));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_WEEZING),
            SPECIES_WEEZING
        );
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Hypno has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_HYPNO_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_HYPNO_ROCKET].baseHP, 85);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HYPNO_ROCKET].baseAttack, 73);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HYPNO_ROCKET].baseDefense, 80);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HYPNO_ROCKET].baseSpeed, 90);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HYPNO_ROCKET].baseSpAttack, 100);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HYPNO_ROCKET].baseSpDefense, 92);

        EXPECT_EQ(gSpeciesInfo[SPECIES_HYPNO_ROCKET].types[0], TYPE_PSYCHIC);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HYPNO_ROCKET].types[1], TYPE_GHOST);

        EXPECT_EQ(gSpeciesInfo[SPECIES_HYPNO_ROCKET].abilities[0], ABILITY_MAGIC_BOUNCE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HYPNO_ROCKET].abilities[1], ABILITY_NONE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HYPNO_ROCKET].abilities[2], ABILITY_MAGIC_BOUNCE);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_HYPNO_ROCKET].natDexNum,
            NATIONAL_DEX_HYPNO
        );

        EXPECT_EQ(player->type1, TYPE_PSYCHIC);
        EXPECT_EQ(player->type2, TYPE_GHOST);
        EXPECT_EQ(player->ability, ABILITY_MAGIC_BOUNCE);
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Hypno maps to base species")
{
    GIVEN {
        PLAYER(SPECIES_HYPNO_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_HYPNO_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_HYPNO_ROCKET),
            SPECIES_HYPNO
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_HYPNO));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_HYPNO),
            SPECIES_HYPNO
        );
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Electiv has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_ELECTIVIRE_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_ELECTIVIRE_ROCKET].baseHP, 75);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ELECTIVIRE_ROCKET].baseAttack, 130);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ELECTIVIRE_ROCKET].baseDefense, 75);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ELECTIVIRE_ROCKET].baseSpeed, 105);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ELECTIVIRE_ROCKET].baseSpAttack, 95);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ELECTIVIRE_ROCKET].baseSpDefense, 85);

        EXPECT_EQ(gSpeciesInfo[SPECIES_ELECTIVIRE_ROCKET].types[0], TYPE_ELECTRIC);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ELECTIVIRE_ROCKET].types[1], TYPE_FIGHTING);

        EXPECT_EQ(gSpeciesInfo[SPECIES_ELECTIVIRE_ROCKET].abilities[0], ABILITY_TOUGH_CLAWS);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ELECTIVIRE_ROCKET].abilities[1], ABILITY_NONE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ELECTIVIRE_ROCKET].abilities[2], ABILITY_TOUGH_CLAWS);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_ELECTIVIRE_ROCKET].natDexNum,
            NATIONAL_DEX_ELECTIVIRE
        );

        EXPECT_EQ(player->type1, TYPE_ELECTRIC);
        EXPECT_EQ(player->type2, TYPE_FIGHTING);
        EXPECT_EQ(player->ability, ABILITY_TOUGH_CLAWS);
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Electiv maps to base species")
{
    GIVEN {
        PLAYER(SPECIES_ELECTIVIRE_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_ELECTIVIRE_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_ELECTIVIRE_ROCKET),
            SPECIES_ELECTIVIRE
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_ELECTIVIRE));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_ELECTIVIRE),
            SPECIES_ELECTIVIRE
        );
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Magmort has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_MAGMORTAR_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_MAGMORTAR_ROCKET].baseHP, 75);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MAGMORTAR_ROCKET].baseAttack, 95);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MAGMORTAR_ROCKET].baseDefense, 75);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MAGMORTAR_ROCKET].baseSpeed, 90);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MAGMORTAR_ROCKET].baseSpAttack, 135);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MAGMORTAR_ROCKET].baseSpDefense, 95);

        EXPECT_EQ(gSpeciesInfo[SPECIES_MAGMORTAR_ROCKET].types[0], TYPE_FIRE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MAGMORTAR_ROCKET].types[1], TYPE_POISON);

        EXPECT_EQ(gSpeciesInfo[SPECIES_MAGMORTAR_ROCKET].abilities[0], ABILITY_SHEER_FORCE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MAGMORTAR_ROCKET].abilities[1], ABILITY_NONE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MAGMORTAR_ROCKET].abilities[2], ABILITY_SHEER_FORCE);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_MAGMORTAR_ROCKET].natDexNum,
            NATIONAL_DEX_MAGMORTAR
        );

        EXPECT_EQ(player->type1, TYPE_FIRE);
        EXPECT_EQ(player->type2, TYPE_POISON);
        EXPECT_EQ(player->ability, ABILITY_SHEER_FORCE);
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Magmort maps to base species")
{
    GIVEN {
        PLAYER(SPECIES_MAGMORTAR_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_MAGMORTAR_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_MAGMORTAR_ROCKET),
            SPECIES_MAGMORTAR
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_MAGMORTAR));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_MAGMORTAR),
            SPECIES_MAGMORTAR
        );
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Scizor has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_SCIZOR_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_SCIZOR_ROCKET].baseHP, 70);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SCIZOR_ROCKET].baseAttack, 135);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SCIZOR_ROCKET].baseDefense, 105);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SCIZOR_ROCKET].baseSpeed, 80);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SCIZOR_ROCKET].baseSpAttack, 65);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SCIZOR_ROCKET].baseSpDefense, 80);

        EXPECT_EQ(gSpeciesInfo[SPECIES_SCIZOR_ROCKET].types[0], TYPE_BUG);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SCIZOR_ROCKET].types[1], TYPE_PSYCHIC);

        EXPECT_EQ(gSpeciesInfo[SPECIES_SCIZOR_ROCKET].abilities[0], ABILITY_SHARPNESS);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SCIZOR_ROCKET].abilities[1], ABILITY_TOUGH_CLAWS);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SCIZOR_ROCKET].abilities[2], ABILITY_SHARPNESS);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_SCIZOR_ROCKET].natDexNum,
            NATIONAL_DEX_SCIZOR
        );

        EXPECT_EQ(player->type1, TYPE_BUG);
        EXPECT_EQ(player->type2, TYPE_PSYCHIC);
        EXPECT_EQ(player->ability, ABILITY_SHARPNESS);
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Scizor maps to base species")
{
    GIVEN {
        PLAYER(SPECIES_SCIZOR_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_SCIZOR_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_SCIZOR_ROCKET),
            SPECIES_SCIZOR
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_SCIZOR));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_SCIZOR),
            SPECIES_SCIZOR
        );
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Gyarad has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_GYARADOS_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_GYARADOS_ROCKET].baseHP, 95);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GYARADOS_ROCKET].baseAttack, 125);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GYARADOS_ROCKET].baseDefense, 79);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GYARADOS_ROCKET].baseSpeed, 81);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GYARADOS_ROCKET].baseSpAttack, 60);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GYARADOS_ROCKET].baseSpDefense, 100);

        EXPECT_EQ(gSpeciesInfo[SPECIES_GYARADOS_ROCKET].types[0], TYPE_WATER);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GYARADOS_ROCKET].types[1], TYPE_DARK);

        EXPECT_EQ(gSpeciesInfo[SPECIES_GYARADOS_ROCKET].abilities[0], ABILITY_ADAPTABILITY);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GYARADOS_ROCKET].abilities[1], ABILITY_NONE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GYARADOS_ROCKET].abilities[2], ABILITY_ADAPTABILITY);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_GYARADOS_ROCKET].natDexNum,
            NATIONAL_DEX_GYARADOS
        );

        EXPECT_EQ(player->type1, TYPE_WATER);
        EXPECT_EQ(player->type2, TYPE_DARK);
        EXPECT_EQ(player->ability, ABILITY_ADAPTABILITY);
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Gyarad maps to base species")
{
    GIVEN {
        PLAYER(SPECIES_GYARADOS_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_GYARADOS_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_GYARADOS_ROCKET),
            SPECIES_GYARADOS
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_GYARADOS));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_GYARADOS),
            SPECIES_GYARADOS
        );
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Aerodac has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_AERODACTYL_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_AERODACTYL_ROCKET].baseHP, 80);
        EXPECT_EQ(gSpeciesInfo[SPECIES_AERODACTYL_ROCKET].baseAttack, 115);
        EXPECT_EQ(gSpeciesInfo[SPECIES_AERODACTYL_ROCKET].baseDefense, 75);
        EXPECT_EQ(gSpeciesInfo[SPECIES_AERODACTYL_ROCKET].baseSpeed, 130);
        EXPECT_EQ(gSpeciesInfo[SPECIES_AERODACTYL_ROCKET].baseSpAttack, 60);
        EXPECT_EQ(gSpeciesInfo[SPECIES_AERODACTYL_ROCKET].baseSpDefense, 75);

        EXPECT_EQ(gSpeciesInfo[SPECIES_AERODACTYL_ROCKET].types[0], TYPE_ROCK);
        EXPECT_EQ(gSpeciesInfo[SPECIES_AERODACTYL_ROCKET].types[1], TYPE_DRAGON);

        EXPECT_EQ(gSpeciesInfo[SPECIES_AERODACTYL_ROCKET].abilities[0], ABILITY_TOUGH_CLAWS);
        EXPECT_EQ(gSpeciesInfo[SPECIES_AERODACTYL_ROCKET].abilities[1], ABILITY_NONE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_AERODACTYL_ROCKET].abilities[2], ABILITY_TOUGH_CLAWS);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_AERODACTYL_ROCKET].natDexNum,
            NATIONAL_DEX_AERODACTYL
        );

        EXPECT_EQ(player->type1, TYPE_ROCK);
        EXPECT_EQ(player->type2, TYPE_DRAGON);
        EXPECT_EQ(player->ability, ABILITY_TOUGH_CLAWS);
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Aerodac maps to base species")
{
    GIVEN {
        PLAYER(SPECIES_AERODACTYL_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_AERODACTYL_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_AERODACTYL_ROCKET),
            SPECIES_AERODACTYL
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_AERODACTYL));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_AERODACTYL),
            SPECIES_AERODACTYL
        );
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Ariados has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_ARIADOS_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_ARIADOS_ROCKET].baseHP, 75);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ARIADOS_ROCKET].baseAttack, 110);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ARIADOS_ROCKET].baseDefense, 80);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ARIADOS_ROCKET].baseSpeed, 85);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ARIADOS_ROCKET].baseSpAttack, 70);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ARIADOS_ROCKET].baseSpDefense, 80);

        EXPECT_EQ(gSpeciesInfo[SPECIES_ARIADOS_ROCKET].types[0], TYPE_BUG);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ARIADOS_ROCKET].types[1], TYPE_DARK);

        EXPECT_EQ(gSpeciesInfo[SPECIES_ARIADOS_ROCKET].abilities[0], ABILITY_PRANKSTER);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ARIADOS_ROCKET].abilities[1], ABILITY_NONE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ARIADOS_ROCKET].abilities[2], ABILITY_PRANKSTER);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_ARIADOS_ROCKET].natDexNum,
            NATIONAL_DEX_ARIADOS
        );

        EXPECT_EQ(player->type1, TYPE_BUG);
        EXPECT_EQ(player->type2, TYPE_DARK);
        EXPECT_EQ(player->ability, ABILITY_PRANKSTER);
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Ariados maps to base species")
{
    GIVEN {
        PLAYER(SPECIES_ARIADOS_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_ARIADOS_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_ARIADOS_ROCKET),
            SPECIES_ARIADOS
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_ARIADOS));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_ARIADOS),
            SPECIES_ARIADOS
        );
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Xatu has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_XATU_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_XATU_ROCKET].baseHP, 75);
        EXPECT_EQ(gSpeciesInfo[SPECIES_XATU_ROCKET].baseAttack, 85);
        EXPECT_EQ(gSpeciesInfo[SPECIES_XATU_ROCKET].baseDefense, 75);
        EXPECT_EQ(gSpeciesInfo[SPECIES_XATU_ROCKET].baseSpeed, 110);
        EXPECT_EQ(gSpeciesInfo[SPECIES_XATU_ROCKET].baseSpAttack, 105);
        EXPECT_EQ(gSpeciesInfo[SPECIES_XATU_ROCKET].baseSpDefense, 80);

        EXPECT_EQ(gSpeciesInfo[SPECIES_XATU_ROCKET].types[0], TYPE_PSYCHIC);
        EXPECT_EQ(gSpeciesInfo[SPECIES_XATU_ROCKET].types[1], TYPE_GHOST);

        EXPECT_EQ(gSpeciesInfo[SPECIES_XATU_ROCKET].abilities[0], ABILITY_SPEED_BOOST);
        EXPECT_EQ(gSpeciesInfo[SPECIES_XATU_ROCKET].abilities[1], ABILITY_NONE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_XATU_ROCKET].abilities[2], ABILITY_SPEED_BOOST);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_XATU_ROCKET].natDexNum,
            NATIONAL_DEX_XATU
        );

        EXPECT_EQ(player->type1, TYPE_PSYCHIC);
        EXPECT_EQ(player->type2, TYPE_GHOST);
        EXPECT_EQ(player->ability, ABILITY_SPEED_BOOST);
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Xatu maps to base species")
{
    GIVEN {
        PLAYER(SPECIES_XATU_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_XATU_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_XATU_ROCKET),
            SPECIES_XATU
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_XATU));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_XATU),
            SPECIES_XATU
        );
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Ampharo has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_AMPHAROS_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_AMPHAROS_ROCKET].baseHP, 90);
        EXPECT_EQ(gSpeciesInfo[SPECIES_AMPHAROS_ROCKET].baseAttack, 75);
        EXPECT_EQ(gSpeciesInfo[SPECIES_AMPHAROS_ROCKET].baseDefense, 85);
        EXPECT_EQ(gSpeciesInfo[SPECIES_AMPHAROS_ROCKET].baseSpeed, 70);
        EXPECT_EQ(gSpeciesInfo[SPECIES_AMPHAROS_ROCKET].baseSpAttack, 125);
        EXPECT_EQ(gSpeciesInfo[SPECIES_AMPHAROS_ROCKET].baseSpDefense, 105);

        EXPECT_EQ(gSpeciesInfo[SPECIES_AMPHAROS_ROCKET].types[0], TYPE_ELECTRIC);
        EXPECT_EQ(gSpeciesInfo[SPECIES_AMPHAROS_ROCKET].types[1], TYPE_DRAGON);

        EXPECT_EQ(gSpeciesInfo[SPECIES_AMPHAROS_ROCKET].abilities[0], ABILITY_TRANSISTOR);
        EXPECT_EQ(gSpeciesInfo[SPECIES_AMPHAROS_ROCKET].abilities[1], ABILITY_DRAGONS_MAW);
        EXPECT_EQ(gSpeciesInfo[SPECIES_AMPHAROS_ROCKET].abilities[2], ABILITY_TRANSISTOR);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_AMPHAROS_ROCKET].natDexNum,
            NATIONAL_DEX_AMPHAROS
        );

        EXPECT_EQ(player->type1, TYPE_ELECTRIC);
        EXPECT_EQ(player->type2, TYPE_DRAGON);
        EXPECT_EQ(player->ability, ABILITY_TRANSISTOR);
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Ampharo maps to base species")
{
    GIVEN {
        PLAYER(SPECIES_AMPHAROS_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_AMPHAROS_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_AMPHAROS_ROCKET),
            SPECIES_AMPHAROS
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_AMPHAROS));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_AMPHAROS),
            SPECIES_AMPHAROS
        );
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Sudowoo has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_SUDOWOODO_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_SUDOWOODO_ROCKET].baseHP, 80);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SUDOWOODO_ROCKET].baseAttack, 120);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SUDOWOODO_ROCKET].baseDefense, 125);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SUDOWOODO_ROCKET].baseSpeed, 50);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SUDOWOODO_ROCKET].baseSpAttack, 45);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SUDOWOODO_ROCKET].baseSpDefense, 80);

        EXPECT_EQ(gSpeciesInfo[SPECIES_SUDOWOODO_ROCKET].types[0], TYPE_ROCK);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SUDOWOODO_ROCKET].types[1], TYPE_GRASS);

        EXPECT_EQ(gSpeciesInfo[SPECIES_SUDOWOODO_ROCKET].abilities[0], ABILITY_WATER_ABSORB);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SUDOWOODO_ROCKET].abilities[1], ABILITY_NONE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SUDOWOODO_ROCKET].abilities[2], ABILITY_WATER_ABSORB);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_SUDOWOODO_ROCKET].natDexNum,
            NATIONAL_DEX_SUDOWOODO
        );

        EXPECT_EQ(player->type1, TYPE_ROCK);
        EXPECT_EQ(player->type2, TYPE_GRASS);
        EXPECT_EQ(player->ability, ABILITY_WATER_ABSORB);
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Sudowoo maps to base species")
{
    GIVEN {
        PLAYER(SPECIES_SUDOWOODO_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_SUDOWOODO_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_SUDOWOODO_ROCKET),
            SPECIES_SUDOWOODO
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_SUDOWOODO));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_SUDOWOODO),
            SPECIES_SUDOWOODO
        );
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Honchkr has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_HONCHKROW_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_HONCHKROW_ROCKET].baseHP, 100);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HONCHKROW_ROCKET].baseAttack, 125);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HONCHKROW_ROCKET].baseDefense, 60);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HONCHKROW_ROCKET].baseSpeed, 85);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HONCHKROW_ROCKET].baseSpAttack, 105);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HONCHKROW_ROCKET].baseSpDefense, 60);

        EXPECT_EQ(gSpeciesInfo[SPECIES_HONCHKROW_ROCKET].types[0], TYPE_DARK);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HONCHKROW_ROCKET].types[1], TYPE_PSYCHIC);

        EXPECT_EQ(gSpeciesInfo[SPECIES_HONCHKROW_ROCKET].abilities[0], ABILITY_SUPREME_OVERLORD);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HONCHKROW_ROCKET].abilities[1], ABILITY_NONE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HONCHKROW_ROCKET].abilities[2], ABILITY_SUPREME_OVERLORD);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_HONCHKROW_ROCKET].natDexNum,
            NATIONAL_DEX_HONCHKROW
        );

        EXPECT_EQ(player->type1, TYPE_DARK);
        EXPECT_EQ(player->type2, TYPE_PSYCHIC);
        EXPECT_EQ(player->ability, ABILITY_SUPREME_OVERLORD);
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Honchkr maps to base species")
{
    GIVEN {
        PLAYER(SPECIES_HONCHKROW_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_HONCHKROW_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_HONCHKROW_ROCKET),
            SPECIES_HONCHKROW
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_HONCHKROW));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_HONCHKROW),
            SPECIES_HONCHKROW
        );
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Mismagi has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_MISMAGIUS_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_MISMAGIUS_ROCKET].baseHP, 70);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MISMAGIUS_ROCKET].baseAttack, 60);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MISMAGIUS_ROCKET].baseDefense, 70);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MISMAGIUS_ROCKET].baseSpeed, 110);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MISMAGIUS_ROCKET].baseSpAttack, 115);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MISMAGIUS_ROCKET].baseSpDefense, 105);

        EXPECT_EQ(gSpeciesInfo[SPECIES_MISMAGIUS_ROCKET].types[0], TYPE_GHOST);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MISMAGIUS_ROCKET].types[1], TYPE_FAIRY);

        EXPECT_EQ(gSpeciesInfo[SPECIES_MISMAGIUS_ROCKET].abilities[0], ABILITY_MAGIC_GUARD);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MISMAGIUS_ROCKET].abilities[1], ABILITY_NONE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MISMAGIUS_ROCKET].abilities[2], ABILITY_MAGIC_GUARD);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_MISMAGIUS_ROCKET].natDexNum,
            NATIONAL_DEX_MISMAGIUS
        );

        EXPECT_EQ(player->type1, TYPE_GHOST);
        EXPECT_EQ(player->type2, TYPE_FAIRY);
        EXPECT_EQ(player->ability, ABILITY_MAGIC_GUARD);
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Mismagi maps to base species")
{
    GIVEN {
        PLAYER(SPECIES_MISMAGIUS_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_MISMAGIUS_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_MISMAGIUS_ROCKET),
            SPECIES_MISMAGIUS
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_MISMAGIUS));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_MISMAGIUS),
            SPECIES_MISMAGIUS
        );
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Forretr has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_FORRETRESS_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_FORRETRESS_ROCKET].baseHP, 80);
        EXPECT_EQ(gSpeciesInfo[SPECIES_FORRETRESS_ROCKET].baseAttack, 100);
        EXPECT_EQ(gSpeciesInfo[SPECIES_FORRETRESS_ROCKET].baseDefense, 140);
        EXPECT_EQ(gSpeciesInfo[SPECIES_FORRETRESS_ROCKET].baseSpeed, 50);
        EXPECT_EQ(gSpeciesInfo[SPECIES_FORRETRESS_ROCKET].baseSpAttack, 60);
        EXPECT_EQ(gSpeciesInfo[SPECIES_FORRETRESS_ROCKET].baseSpDefense, 90);

        EXPECT_EQ(gSpeciesInfo[SPECIES_FORRETRESS_ROCKET].types[0], TYPE_BUG);
        EXPECT_EQ(gSpeciesInfo[SPECIES_FORRETRESS_ROCKET].types[1], TYPE_GHOST);

        EXPECT_EQ(gSpeciesInfo[SPECIES_FORRETRESS_ROCKET].abilities[0], ABILITY_REGENERATOR);
        EXPECT_EQ(gSpeciesInfo[SPECIES_FORRETRESS_ROCKET].abilities[1], ABILITY_NONE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_FORRETRESS_ROCKET].abilities[2], ABILITY_REGENERATOR);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_FORRETRESS_ROCKET].natDexNum,
            NATIONAL_DEX_FORRETRESS
        );

        EXPECT_EQ(player->type1, TYPE_BUG);
        EXPECT_EQ(player->type2, TYPE_GHOST);
        EXPECT_EQ(player->ability, ABILITY_REGENERATOR);
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Forretr maps to base species")
{
    GIVEN {
        PLAYER(SPECIES_FORRETRESS_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_FORRETRESS_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_FORRETRESS_ROCKET),
            SPECIES_FORRETRESS
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_FORRETRESS));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_FORRETRESS),
            SPECIES_FORRETRESS
        );
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Gliscor has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_GLISCOR_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_GLISCOR_ROCKET].baseHP, 80);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GLISCOR_ROCKET].baseAttack, 105);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GLISCOR_ROCKET].baseDefense, 125);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GLISCOR_ROCKET].baseSpeed, 95);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GLISCOR_ROCKET].baseSpAttack, 45);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GLISCOR_ROCKET].baseSpDefense, 90);

        EXPECT_EQ(gSpeciesInfo[SPECIES_GLISCOR_ROCKET].types[0], TYPE_GROUND);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GLISCOR_ROCKET].types[1], TYPE_DRAGON);

        EXPECT_EQ(gSpeciesInfo[SPECIES_GLISCOR_ROCKET].abilities[0], ABILITY_THICK_FAT);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GLISCOR_ROCKET].abilities[1], ABILITY_POISON_HEAL);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GLISCOR_ROCKET].abilities[2], ABILITY_THICK_FAT);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_GLISCOR_ROCKET].natDexNum,
            NATIONAL_DEX_GLISCOR
        );

        EXPECT_EQ(player->type1, TYPE_GROUND);
        EXPECT_EQ(player->type2, TYPE_DRAGON);
        EXPECT_EQ(player->ability, ABILITY_THICK_FAT);
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Gliscor maps to base species")
{
    GIVEN {
        PLAYER(SPECIES_GLISCOR_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_GLISCOR_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_GLISCOR_ROCKET),
            SPECIES_GLISCOR
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_GLISCOR));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_GLISCOR),
            SPECIES_GLISCOR
        );
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Granbul has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_GRANBULL_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_GRANBULL_ROCKET].baseHP, 90);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GRANBULL_ROCKET].baseAttack, 130);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GRANBULL_ROCKET].baseDefense, 80);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GRANBULL_ROCKET].baseSpeed, 60);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GRANBULL_ROCKET].baseSpAttack, 60);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GRANBULL_ROCKET].baseSpDefense, 80);

        EXPECT_EQ(gSpeciesInfo[SPECIES_GRANBULL_ROCKET].types[0], TYPE_FAIRY);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GRANBULL_ROCKET].types[1], TYPE_FIGHTING);

        EXPECT_EQ(gSpeciesInfo[SPECIES_GRANBULL_ROCKET].abilities[0], ABILITY_TOUGH_CLAWS);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GRANBULL_ROCKET].abilities[1], ABILITY_NONE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_GRANBULL_ROCKET].abilities[2], ABILITY_TOUGH_CLAWS);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_GRANBULL_ROCKET].natDexNum,
            NATIONAL_DEX_GRANBULL
        );

        EXPECT_EQ(player->type1, TYPE_FAIRY);
        EXPECT_EQ(player->type2, TYPE_FIGHTING);
        EXPECT_EQ(player->ability, ABILITY_TOUGH_CLAWS);
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Granbul maps to base species")
{
    GIVEN {
        PLAYER(SPECIES_GRANBULL_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_GRANBULL_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_GRANBULL_ROCKET),
            SPECIES_GRANBULL
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_GRANBULL));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_GRANBULL),
            SPECIES_GRANBULL
        );
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Houndoo has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_HOUNDOOM_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_HOUNDOOM_ROCKET].baseHP, 75);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HOUNDOOM_ROCKET].baseAttack, 95);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HOUNDOOM_ROCKET].baseDefense, 65);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HOUNDOOM_ROCKET].baseSpeed, 110);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HOUNDOOM_ROCKET].baseSpAttack, 125);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HOUNDOOM_ROCKET].baseSpDefense, 80);

        EXPECT_EQ(gSpeciesInfo[SPECIES_HOUNDOOM_ROCKET].types[0], TYPE_FIRE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HOUNDOOM_ROCKET].types[1], TYPE_POISON);

        EXPECT_EQ(gSpeciesInfo[SPECIES_HOUNDOOM_ROCKET].abilities[0], ABILITY_BEADS_OF_RUIN);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HOUNDOOM_ROCKET].abilities[1], ABILITY_NONE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HOUNDOOM_ROCKET].abilities[2], ABILITY_BEADS_OF_RUIN);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_HOUNDOOM_ROCKET].natDexNum,
            NATIONAL_DEX_HOUNDOOM
        );

        EXPECT_EQ(player->type1, TYPE_FIRE);
        EXPECT_EQ(player->type2, TYPE_POISON);
        EXPECT_EQ(player->ability, ABILITY_BEADS_OF_RUIN);
    }
}

SINGLE_BATTLE_TEST("TR Wave A: TR Houndoo maps to base species")
{
    GIVEN {
        PLAYER(SPECIES_HOUNDOOM_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_HOUNDOOM_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_HOUNDOOM_ROCKET),
            SPECIES_HOUNDOOM
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_HOUNDOOM));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_HOUNDOOM),
            SPECIES_HOUNDOOM
        );
    }
}

