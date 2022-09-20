#define ROGUE_DEBUG
#define ROGUE_DEBUG_PAUSE_PANEL
//#define ROGUE_DEBUG_STEAL_TEAM

//#define ROGUE_EXPANSION

#define ROGUE_FEATURE_ENCOUNTER_PREVIEW

// It looks like file.c:line: size of array `id' is negative
#define ROGUE_STATIC_ASSERT(expr, id) typedef char id[(expr) ? 1 : -1];

#define ROGUE_ROUTE_COUNT 12
#define ROGUE_HUB_BERRY_TREE_COUNT 16

#define ROGUE_MAX_ADVPATH_ROWS 7
#define ROGUE_MAX_ADVPATH_COLUMNS 7


#define ROGUE_SHOP_NONE                 0
#define ROGUE_SHOP_MEDICINE             1
#define ROGUE_SHOP_BALLS                2
#define ROGUE_SHOP_TMS                  3
#define ROGUE_SHOP_BATTLE_ENHANCERS     4
#define ROGUE_SHOP_HELD_ITEMS           5
#define ROGUE_SHOP_RARE_HELD_ITEMS      6
#define ROGUE_SHOP_BERRIES              7

#include "rogue_quests.h"