#ifndef GUARD_ITEM_CONSTANTS_H
#define GUARD_ITEM_CONSTANTS_H

#ifdef ROGUE_EXPANSION
// These constants are used in gItems
#define POCKET_NONE        0
#define POCKET_ITEMS       1
#define POCKET_HELD_ITEMS  2
#define POCKET_MEDICINE    3
#define POCKET_STONES      4
#define POCKET_POKE_BALLS  5
#define POCKET_TM_HM       6
#define POCKET_BERRIES     7
#define POCKET_CHARMS      8
#define POCKET_KEY_ITEMS   9


#define ITEMS_POCKET       0
#define HELD_ITEMS_POCKET  1
#define MEDICINE_POCKET    2
#define STONES_POCKET      3
#define BALLS_POCKET       4
#define TMHM_POCKET        5
#define BERRIES_POCKET     6
#define CHARMS_POCKET      7
#define KEYITEMS_POCKET    8
#define POCKETS_COUNT      9

#else
// Vanilla

// These constants are used in gItems
#define POCKET_NONE        0
#define POCKET_ITEMS       1
#define POCKET_HELD_ITEMS  2
#define POCKET_MEDICINE    3
#define POCKET_POKE_BALLS  4
#define POCKET_TM_HM       5
#define POCKET_BERRIES     6
#define POCKET_CHARMS      7
#define POCKET_KEY_ITEMS   8

// These pockets aren't actually in used, but need to be defined, to avoid compilation error
#define POCKET_STONES      24


#define ITEMS_POCKET       0
#define HELD_ITEMS_POCKET  1
#define MEDICINE_POCKET    2
#define BALLS_POCKET       3
#define TMHM_POCKET        4
#define BERRIES_POCKET     5
#define CHARMS_POCKET      6
#define KEYITEMS_POCKET    7
#define POCKETS_COUNT      8
#endif

#endif // GUARD_ITEM_CONSTANTS_H
