#define ROGUE_DEBUG
//#define ROGUE_DEBUG_PAUSE_PANEL
//#define ROGUE_DEBUG_STEAL_TEAM

//#define ROGUE_EXPANSION

#define ROGUE_FEATURE_ENCOUNTER_PREVIEW

// It looks like file.c:line: size of array `id' is negative
#define ROGUE_STATIC_ASSERT(expr, id) typedef char id[(expr) ? 1 : -1];

#define ROGUE_ROUTE_COUNT 12

#define ROGUE_MAX_ADVPATH_ROWS 7
#define ROGUE_MAX_ADVPATH_COLUMNS 7