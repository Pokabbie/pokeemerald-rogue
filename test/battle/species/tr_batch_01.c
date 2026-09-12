#include "global.h"
#include "pokemon.h"
#include "test/battle.h"
#include "rogue_team_rocket.h"

SINGLE_BATTLE_TEST("TR Batch 01: TR Luxray has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_LUXRAY_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_LUXRAY_ROCKET].baseHP, 80);
        EXPECT_EQ(gSpeciesInfo[SPECIES_LUXRAY_ROCKET].baseAttack, 125);
        EXPECT_EQ(gSpeciesInfo[SPECIES_LUXRAY_ROCKET].baseDefense, 85);
        EXPECT_EQ(gSpeciesInfo[SPECIES_LUXRAY_ROCKET].baseSpeed, 90);
        EXPECT_EQ(gSpeciesInfo[SPECIES_LUXRAY_ROCKET].baseSpAttack, 95);
        EXPECT_EQ(gSpeciesInfo[SPECIES_LUXRAY_ROCKET].baseSpDefense, 85);

        EXPECT_EQ(gSpeciesInfo[SPECIES_LUXRAY_ROCKET].types[0], TYPE_ELECTRIC);
        EXPECT_EQ(gSpeciesInfo[SPECIES_LUXRAY_ROCKET].types[1], TYPE_DARK);

        EXPECT_EQ(gSpeciesInfo[SPECIES_LUXRAY_ROCKET].abilities[0], ABILITY_STRONG_JAW);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_LUXRAY_ROCKET].natDexNum,
            NATIONAL_DEX_LUXRAY
        );

        EXPECT_EQ(player->type1, TYPE_ELECTRIC);
        EXPECT_EQ(player->type2, TYPE_DARK);
        EXPECT_EQ(player->ability, ABILITY_STRONG_JAW);
    }
}

SINGLE_BATTLE_TEST("TR Batch 01: TR Luxray maps back to its base species")
{
    GIVEN {
        PLAYER(SPECIES_LUXRAY_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_LUXRAY_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_LUXRAY_ROCKET),
            SPECIES_LUXRAY
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_LUXRAY));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_LUXRAY),
            SPECIES_LUXRAY
        );
    }
}

SINGLE_BATTLE_TEST("TR Batch 01: TR Mtyena has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_MIGHTYENA_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_MIGHTYENA_ROCKET].baseHP, 75);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MIGHTYENA_ROCKET].baseAttack, 110);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MIGHTYENA_ROCKET].baseDefense, 75);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MIGHTYENA_ROCKET].baseSpeed, 95);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MIGHTYENA_ROCKET].baseSpAttack, 60);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MIGHTYENA_ROCKET].baseSpDefense, 75);

        EXPECT_EQ(gSpeciesInfo[SPECIES_MIGHTYENA_ROCKET].types[0], TYPE_DARK);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MIGHTYENA_ROCKET].types[1], TYPE_FIRE);

        EXPECT_EQ(gSpeciesInfo[SPECIES_MIGHTYENA_ROCKET].abilities[0], ABILITY_STRONG_JAW);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_MIGHTYENA_ROCKET].natDexNum,
            NATIONAL_DEX_MIGHTYENA
        );

        EXPECT_EQ(player->type1, TYPE_DARK);
        EXPECT_EQ(player->type2, TYPE_FIRE);
        EXPECT_EQ(player->ability, ABILITY_STRONG_JAW);
    }
}

SINGLE_BATTLE_TEST("TR Batch 01: TR Mtyena maps back to its base species")
{
    GIVEN {
        PLAYER(SPECIES_MIGHTYENA_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_MIGHTYENA_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_MIGHTYENA_ROCKET),
            SPECIES_MIGHTYENA
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_MIGHTYENA));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_MIGHTYENA),
            SPECIES_MIGHTYENA
        );
    }
}

SINGLE_BATTLE_TEST("TR Batch 01: TR Seviper has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_SEVIPER_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_SEVIPER_ROCKET].baseHP, 73);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SEVIPER_ROCKET].baseAttack, 115);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SEVIPER_ROCKET].baseDefense, 70);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SEVIPER_ROCKET].baseSpeed, 85);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SEVIPER_ROCKET].baseSpAttack, 105);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SEVIPER_ROCKET].baseSpDefense, 70);

        EXPECT_EQ(gSpeciesInfo[SPECIES_SEVIPER_ROCKET].types[0], TYPE_POISON);
        EXPECT_EQ(gSpeciesInfo[SPECIES_SEVIPER_ROCKET].types[1], TYPE_DRAGON);

        EXPECT_EQ(gSpeciesInfo[SPECIES_SEVIPER_ROCKET].abilities[0], ABILITY_MERCILESS);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_SEVIPER_ROCKET].natDexNum,
            NATIONAL_DEX_SEVIPER
        );

        EXPECT_EQ(player->type1, TYPE_POISON);
        EXPECT_EQ(player->type2, TYPE_DRAGON);
        EXPECT_EQ(player->ability, ABILITY_MERCILESS);
    }
}

SINGLE_BATTLE_TEST("TR Batch 01: TR Seviper maps back to its base species")
{
    GIVEN {
        PLAYER(SPECIES_SEVIPER_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_SEVIPER_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_SEVIPER_ROCKET),
            SPECIES_SEVIPER
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_SEVIPER));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_SEVIPER),
            SPECIES_SEVIPER
        );
    }
}

SINGLE_BATTLE_TEST("TR Batch 01: TR Banette has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_BANETTE_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_BANETTE_ROCKET].baseHP, 64);
        EXPECT_EQ(gSpeciesInfo[SPECIES_BANETTE_ROCKET].baseAttack, 130);
        EXPECT_EQ(gSpeciesInfo[SPECIES_BANETTE_ROCKET].baseDefense, 70);
        EXPECT_EQ(gSpeciesInfo[SPECIES_BANETTE_ROCKET].baseSpeed, 85);
        EXPECT_EQ(gSpeciesInfo[SPECIES_BANETTE_ROCKET].baseSpAttack, 83);
        EXPECT_EQ(gSpeciesInfo[SPECIES_BANETTE_ROCKET].baseSpDefense, 68);

        EXPECT_EQ(gSpeciesInfo[SPECIES_BANETTE_ROCKET].types[0], TYPE_GHOST);
        EXPECT_EQ(gSpeciesInfo[SPECIES_BANETTE_ROCKET].types[1], TYPE_NORMAL);

        EXPECT_EQ(gSpeciesInfo[SPECIES_BANETTE_ROCKET].abilities[0], ABILITY_TOUGH_CLAWS);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_BANETTE_ROCKET].natDexNum,
            NATIONAL_DEX_BANETTE
        );

        EXPECT_EQ(player->type1, TYPE_GHOST);
        EXPECT_EQ(player->type2, TYPE_NORMAL);
        EXPECT_EQ(player->ability, ABILITY_TOUGH_CLAWS);
    }
}

SINGLE_BATTLE_TEST("TR Batch 01: TR Banette maps back to its base species")
{
    GIVEN {
        PLAYER(SPECIES_BANETTE_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_BANETTE_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_BANETTE_ROCKET),
            SPECIES_BANETTE
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_BANETTE));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_BANETTE),
            SPECIES_BANETTE
        );
    }
}

SINGLE_BATTLE_TEST("TR Batch 01: TR Absol has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_ABSOL_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_ABSOL_ROCKET].baseHP, 65);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ABSOL_ROCKET].baseAttack, 140);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ABSOL_ROCKET].baseDefense, 70);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ABSOL_ROCKET].baseSpeed, 95);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ABSOL_ROCKET].baseSpAttack, 75);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ABSOL_ROCKET].baseSpDefense, 65);

        EXPECT_EQ(gSpeciesInfo[SPECIES_ABSOL_ROCKET].types[0], TYPE_DARK);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ABSOL_ROCKET].types[1], TYPE_FAIRY);

        EXPECT_EQ(gSpeciesInfo[SPECIES_ABSOL_ROCKET].abilities[0], ABILITY_TOUGH_CLAWS);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_ABSOL_ROCKET].natDexNum,
            NATIONAL_DEX_ABSOL
        );

        EXPECT_EQ(player->type1, TYPE_DARK);
        EXPECT_EQ(player->type2, TYPE_FAIRY);
        EXPECT_EQ(player->ability, ABILITY_TOUGH_CLAWS);
    }
}

SINGLE_BATTLE_TEST("TR Batch 01: TR Absol maps back to its base species")
{
    GIVEN {
        PLAYER(SPECIES_ABSOL_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_ABSOL_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_ABSOL_ROCKET),
            SPECIES_ABSOL
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_ABSOL));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_ABSOL),
            SPECIES_ABSOL
        );
    }
}

SINGLE_BATTLE_TEST("TR Batch 01: TR Donphan has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_DONPHAN_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_DONPHAN_ROCKET].baseHP, 90);
        EXPECT_EQ(gSpeciesInfo[SPECIES_DONPHAN_ROCKET].baseAttack, 125);
        EXPECT_EQ(gSpeciesInfo[SPECIES_DONPHAN_ROCKET].baseDefense, 130);
        EXPECT_EQ(gSpeciesInfo[SPECIES_DONPHAN_ROCKET].baseSpeed, 60);
        EXPECT_EQ(gSpeciesInfo[SPECIES_DONPHAN_ROCKET].baseSpAttack, 60);
        EXPECT_EQ(gSpeciesInfo[SPECIES_DONPHAN_ROCKET].baseSpDefense, 70);

        EXPECT_EQ(gSpeciesInfo[SPECIES_DONPHAN_ROCKET].types[0], TYPE_GROUND);
        EXPECT_EQ(gSpeciesInfo[SPECIES_DONPHAN_ROCKET].types[1], TYPE_STEEL);

        EXPECT_EQ(gSpeciesInfo[SPECIES_DONPHAN_ROCKET].abilities[0], ABILITY_STAMINA);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_DONPHAN_ROCKET].natDexNum,
            NATIONAL_DEX_DONPHAN
        );

        EXPECT_EQ(player->type1, TYPE_GROUND);
        EXPECT_EQ(player->type2, TYPE_STEEL);
        EXPECT_EQ(player->ability, ABILITY_STAMINA);
    }
}

SINGLE_BATTLE_TEST("TR Batch 01: TR Donphan maps back to its base species")
{
    GIVEN {
        PLAYER(SPECIES_DONPHAN_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_DONPHAN_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_DONPHAN_ROCKET),
            SPECIES_DONPHAN
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_DONPHAN));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_DONPHAN),
            SPECIES_DONPHAN
        );
    }
}

SINGLE_BATTLE_TEST("TR Batch 01: TR Revavrm has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_REVAVROOM_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_REVAVROOM_ROCKET].baseHP, 80);
        EXPECT_EQ(gSpeciesInfo[SPECIES_REVAVROOM_ROCKET].baseAttack, 125);
        EXPECT_EQ(gSpeciesInfo[SPECIES_REVAVROOM_ROCKET].baseDefense, 95);
        EXPECT_EQ(gSpeciesInfo[SPECIES_REVAVROOM_ROCKET].baseSpeed, 100);
        EXPECT_EQ(gSpeciesInfo[SPECIES_REVAVROOM_ROCKET].baseSpAttack, 54);
        EXPECT_EQ(gSpeciesInfo[SPECIES_REVAVROOM_ROCKET].baseSpDefense, 76);

        EXPECT_EQ(gSpeciesInfo[SPECIES_REVAVROOM_ROCKET].types[0], TYPE_STEEL);
        EXPECT_EQ(gSpeciesInfo[SPECIES_REVAVROOM_ROCKET].types[1], TYPE_POISON);

        EXPECT_EQ(gSpeciesInfo[SPECIES_REVAVROOM_ROCKET].abilities[0], ABILITY_EARTH_EATER);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_REVAVROOM_ROCKET].natDexNum,
            NATIONAL_DEX_REVAVROOM
        );

        EXPECT_EQ(player->type1, TYPE_STEEL);
        EXPECT_EQ(player->type2, TYPE_POISON);
        EXPECT_EQ(player->ability, ABILITY_EARTH_EATER);
    }
}

SINGLE_BATTLE_TEST("TR Batch 01: TR Revavrm maps back to its base species")
{
    GIVEN {
        PLAYER(SPECIES_REVAVROOM_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_REVAVROOM_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_REVAVROOM_ROCKET),
            SPECIES_REVAVROOM
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_REVAVROOM));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_REVAVROOM),
            SPECIES_REVAVROOM
        );
    }
}

SINGLE_BATTLE_TEST("TR Batch 01: TR Ceruldg has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_CERULEDGE_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_CERULEDGE_ROCKET].baseHP, 75);
        EXPECT_EQ(gSpeciesInfo[SPECIES_CERULEDGE_ROCKET].baseAttack, 135);
        EXPECT_EQ(gSpeciesInfo[SPECIES_CERULEDGE_ROCKET].baseDefense, 85);
        EXPECT_EQ(gSpeciesInfo[SPECIES_CERULEDGE_ROCKET].baseSpeed, 95);
        EXPECT_EQ(gSpeciesInfo[SPECIES_CERULEDGE_ROCKET].baseSpAttack, 60);
        EXPECT_EQ(gSpeciesInfo[SPECIES_CERULEDGE_ROCKET].baseSpDefense, 105);

        EXPECT_EQ(gSpeciesInfo[SPECIES_CERULEDGE_ROCKET].types[0], TYPE_FIRE);
        EXPECT_EQ(gSpeciesInfo[SPECIES_CERULEDGE_ROCKET].types[1], TYPE_DARK);

        EXPECT_EQ(gSpeciesInfo[SPECIES_CERULEDGE_ROCKET].abilities[0], ABILITY_SHARPNESS);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_CERULEDGE_ROCKET].natDexNum,
            NATIONAL_DEX_CERULEDGE
        );

        EXPECT_EQ(player->type1, TYPE_FIRE);
        EXPECT_EQ(player->type2, TYPE_DARK);
        EXPECT_EQ(player->ability, ABILITY_SHARPNESS);
    }
}

SINGLE_BATTLE_TEST("TR Batch 01: TR Ceruldg maps back to its base species")
{
    GIVEN {
        PLAYER(SPECIES_CERULEDGE_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_CERULEDGE_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_CERULEDGE_ROCKET),
            SPECIES_CERULEDGE
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_CERULEDGE));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_CERULEDGE),
            SPECIES_CERULEDGE
        );
    }
}

SINGLE_BATTLE_TEST("TR Batch 01: TR Pinsir has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_PINSIR_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_PINSIR_ROCKET].baseHP, 65);
        EXPECT_EQ(gSpeciesInfo[SPECIES_PINSIR_ROCKET].baseAttack, 135);
        EXPECT_EQ(gSpeciesInfo[SPECIES_PINSIR_ROCKET].baseDefense, 105);
        EXPECT_EQ(gSpeciesInfo[SPECIES_PINSIR_ROCKET].baseSpeed, 95);
        EXPECT_EQ(gSpeciesInfo[SPECIES_PINSIR_ROCKET].baseSpAttack, 55);
        EXPECT_EQ(gSpeciesInfo[SPECIES_PINSIR_ROCKET].baseSpDefense, 75);

        EXPECT_EQ(gSpeciesInfo[SPECIES_PINSIR_ROCKET].types[0], TYPE_BUG);
        EXPECT_EQ(gSpeciesInfo[SPECIES_PINSIR_ROCKET].types[1], TYPE_FIGHTING);

        EXPECT_EQ(gSpeciesInfo[SPECIES_PINSIR_ROCKET].abilities[0], ABILITY_MOXIE);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_PINSIR_ROCKET].natDexNum,
            NATIONAL_DEX_PINSIR
        );

        EXPECT_EQ(player->type1, TYPE_BUG);
        EXPECT_EQ(player->type2, TYPE_FIGHTING);
        EXPECT_EQ(player->ability, ABILITY_MOXIE);
    }
}

SINGLE_BATTLE_TEST("TR Batch 01: TR Pinsir maps back to its base species")
{
    GIVEN {
        PLAYER(SPECIES_PINSIR_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_PINSIR_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_PINSIR_ROCKET),
            SPECIES_PINSIR
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_PINSIR));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_PINSIR),
            SPECIES_PINSIR
        );
    }
}

SINGLE_BATTLE_TEST("TR Batch 01: TR Crobat has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_CROBAT_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_CROBAT_ROCKET].baseHP, 85);
        EXPECT_EQ(gSpeciesInfo[SPECIES_CROBAT_ROCKET].baseAttack, 105);
        EXPECT_EQ(gSpeciesInfo[SPECIES_CROBAT_ROCKET].baseDefense, 85);
        EXPECT_EQ(gSpeciesInfo[SPECIES_CROBAT_ROCKET].baseSpeed, 130);
        EXPECT_EQ(gSpeciesInfo[SPECIES_CROBAT_ROCKET].baseSpAttack, 70);
        EXPECT_EQ(gSpeciesInfo[SPECIES_CROBAT_ROCKET].baseSpDefense, 85);

        EXPECT_EQ(gSpeciesInfo[SPECIES_CROBAT_ROCKET].types[0], TYPE_POISON);
        EXPECT_EQ(gSpeciesInfo[SPECIES_CROBAT_ROCKET].types[1], TYPE_DARK);

        EXPECT_EQ(gSpeciesInfo[SPECIES_CROBAT_ROCKET].abilities[0], ABILITY_SPEED_BOOST);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_CROBAT_ROCKET].natDexNum,
            NATIONAL_DEX_CROBAT
        );

        EXPECT_EQ(player->type1, TYPE_POISON);
        EXPECT_EQ(player->type2, TYPE_DARK);
        EXPECT_EQ(player->ability, ABILITY_SPEED_BOOST);
    }
}

SINGLE_BATTLE_TEST("TR Batch 01: TR Crobat maps back to its base species")
{
    GIVEN {
        PLAYER(SPECIES_CROBAT_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_CROBAT_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_CROBAT_ROCKET),
            SPECIES_CROBAT
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_CROBAT));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_CROBAT),
            SPECIES_CROBAT
        );
    }
}

