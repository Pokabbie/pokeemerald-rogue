static const u8 sRouteEncontersField[] = {
    TYPE_GRASS, TYPE_NORMAL
};

static const u8 sRouteEncontersForest[] = {
    TYPE_BUG, TYPE_GHOST, TYPE_POISON
};

static const u8 sRouteEncontersCave[] = {
    TYPE_ROCK, TYPE_ICE, TYPE_DRAGON
};

static const u8 sRouteEncontersMountain[] = {
    TYPE_GROUND, TYPE_FIRE, TYPE_FIGHTING
};

static const u8 sRouteEncontersWaterfront[] = {
    TYPE_WATER, TYPE_FLYING
};

static const u8 sRouteEncontersUrban[] = {
    TYPE_STEEL, TYPE_ELECTRIC, TYPE_PSYCHIC
};

const struct RogueRouteData gRogueRouteTable[ROGUE_ROUTE_COUNT] = {
    [ROGUE_ROUTE_FIELD] = 
    {
        .wildTypeTableCount = ARRAY_COUNT(sRouteEncontersField),
        .wildTypeTable = sRouteEncontersField
    },
    [ROGUE_ROUTE_FOREST] = 
    {
        .wildTypeTableCount = ARRAY_COUNT(sRouteEncontersForest),
        .wildTypeTable = sRouteEncontersForest
    },
    [ROGUE_ROUTE_CAVE] = 
    {
        .wildTypeTableCount = ARRAY_COUNT(sRouteEncontersCave),
        .wildTypeTable = sRouteEncontersCave
    },
    [ROGUE_ROUTE_MOUNTAIN] = 
    {
        .wildTypeTableCount = ARRAY_COUNT(sRouteEncontersMountain),
        .wildTypeTable = sRouteEncontersMountain
    },
    [ROGUE_ROUTE_WATERFRONT] = 
    {
        .wildTypeTableCount = ARRAY_COUNT(sRouteEncontersWaterfront),
        .wildTypeTable = sRouteEncontersWaterfront
    },
    [ROGUE_ROUTE_URBAN] = 
    {
        .wildTypeTableCount = ARRAY_COUNT(sRouteEncontersUrban),
        .wildTypeTable = sRouteEncontersUrban
    },
};