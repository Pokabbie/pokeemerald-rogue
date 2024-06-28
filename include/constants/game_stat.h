#ifndef GUARD_CONSTANTS_GAME_STAT_H
#define GUARD_CONSTANTS_GAME_STAT_H

#define GAME_STAT_SAVED_GAME                 0
#define GAME_STAT_FIRST_HOF_PLAY_TIME        1
#define GAME_STAT_FASTEST_HOF_PLAY_TIME      2
#define GAME_STAT_SLOWEST_HOF_PLAY_TIME      3

#define GAME_STAT_TOTAL_RUNS                 4
#define GAME_STAT_RUN_WINS                   5
#define GAME_STAT_CURRENT_RUN_WIN_STREAK     6
#define GAME_STAT_LONGEST_RUN_WIN_STREAK     7
#define GAME_STAT_RUN_LOSSES                 8
#define GAME_STAT_CURRENT_RUN_LOSS_STREAK    9
#define GAME_STAT_LONGEST_RUN_LOSS_STREAK   10

#define GAME_STAT_TOTAL_BADGES              11
#define GAME_STAT_GYM_BADGES                12
#define GAME_STAT_ELITE_BADGES              13
#define GAME_STAT_CHAMPION_BADGES           14

#define GAME_STAT_TOTAL_BATTLES             15
#define GAME_STAT_WILD_BATTLES              16
#define GAME_STAT_TRAINER_BATTLES           17
#define GAME_STAT_RIVAL_BATTLES             18

#define GAME_STAT_POKEMON_CAUGHT            19
#define GAME_STAT_SHINIES_CAUGHT            20
#define GAME_STAT_LEGENDS_CAUGHT            21
#define GAME_STAT_ROAMERS_CAUGHT            22
#define GAME_STAT_POKEMON_RELEASED          23
#define GAME_STAT_POKEMON_FAINTED           24
#define GAME_STAT_EVOLVED_POKEMON           25

#define GAME_STAT_RANDO_TRADE_TOTAL_PKMN    26
#define GAME_STAT_RANDO_TRADE_SINGLE        27
#define GAME_STAT_RANDO_TRADE_PARTY         28

#define GAME_STAT_ITEMS_FOUND               29 // todo - hookup
#define GAME_STAT_ITEMS_BOUGHT              30 // todo - fixup
#define GAME_STAT_MOVES_BOUGHT              31 // todo - hookup
#define GAME_STAT_MONEY_SPENT               32 // todo - hookup

#define GAME_STAT_PLANTED_BERRIES           33
#define GAME_STAT_STEPS                     34
#define GAME_STAT_ENTERED_HOF               35 // (Isn't technically needed anymore as run wins tells this)
#define GAME_STAT_USED_POKECENTER           36 // Keep but hook up to all heals?
#define GAME_STAT_POKEMON_TRADES            37 // todo - rehook up
#define GAME_STAT_LINK_BATTLE_WINS          38 // todo - hookup when battle support is in
#define GAME_STAT_LINK_BATTLE_LOSSES        39 // todo - hookup when battle support is in
#define GAME_STAT_LINK_BATTLE_DRAWS         40 // todo - hookup when battle support is in (Prob need to split out into PvP and coop if do that?)
#define GAME_STAT_POKEBLOCKS                41 // (Isn't technically needed as we don't make any pokeblock in run)
#define GAME_STAT_CHECKED_POKEDEX           42
#define GAME_STAT_JUMPED_DOWN_LEDGES        43

#define NUM_USED_GAME_STATS                 44
#define NUM_GAME_STATS                      64 // This is the usable capacity

// Stat Ideas
//


// Total Items Picked up
// Total Money Spent (Shops & NPCs?)


// No pkmn fainte to Wobb

#endif // GUARD_CONSTANTS_GAME_STAT_H
