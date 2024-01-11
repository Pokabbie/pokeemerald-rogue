#ifndef GUARD_CONSTANTS_BERRY_H
#define GUARD_CONSTANTS_BERRY_H

#define BERRY_NONE 0

#define BERRY_FIRMNESS_UNKNOWN     0
#define BERRY_FIRMNESS_VERY_SOFT   1
#define BERRY_FIRMNESS_SOFT        2
#define BERRY_FIRMNESS_HARD        3
#define BERRY_FIRMNESS_VERY_HARD   4
#define BERRY_FIRMNESS_SUPER_HARD  5

#define FLAVOR_SPICY   0
#define FLAVOR_DRY     1
#define FLAVOR_SWEET   2
#define FLAVOR_BITTER  3
#define FLAVOR_SOUR    4
#define FLAVOR_COUNT   5

#define BERRY_STAGE_NO_BERRY    0  // there is no tree planted and the soil is completely flat.
#define BERRY_STAGE_PLANTED     1
#define BERRY_STAGE_SPROUTED    2
#define BERRY_STAGE_TALLER      3
#define BERRY_STAGE_FLOWERING   4
#define BERRY_STAGE_BERRIES     5
#define BERRY_STAGE_SPARKLING   255

// Berries can be watered in the following stages:
// - BERRY_STAGE_PLANTED
// - BERRY_STAGE_SPROUTED
// - BERRY_STAGE_TALLER
// - BERRY_STAGE_FLOWERING
#define NUM_WATER_STAGES 4

// IDs for berry tree objects, indexes into berryTrees in SaveBlock1
// Named for whatever berry is initially planted there on a new game
// Those with no initial berry are named "soil"
#define BERRY_TREE_HUB_1                1
#define BERRY_TREE_HUB_2                2
#define BERRY_TREE_HUB_3                3
#define BERRY_TREE_HUB_4                4
#define BERRY_TREE_HUB_5                5
#define BERRY_TREE_HUB_6                6
#define BERRY_TREE_HUB_7                7
#define BERRY_TREE_HUB_8                8
#define BERRY_TREE_HUB_9                9
#define BERRY_TREE_HUB_10               10
#define BERRY_TREE_HUB_11               11
#define BERRY_TREE_HUB_12               12
#define BERRY_TREE_HUB_13               13
#define BERRY_TREE_HUB_14               14
#define BERRY_TREE_HUB_15               15
#define BERRY_TREE_HUB_16               16
#define BERRY_TREE_HUB_17               17
#define BERRY_TREE_HUB_18               18
#define BERRY_TREE_HUB_19               19
#define BERRY_TREE_HUB_20               20

#define BERRY_TREE_HUB_FIRST            BERRY_TREE_HUB_1
#define BERRY_TREE_HUB_LAST             BERRY_TREE_HUB_20
#define BERRY_TREE_HUB_COUNT            (BERRY_TREE_HUB_LAST - BERRY_TREE_HUB_FIRST + 1)


#define BERRY_TREE_ROUTE_1              21
#define BERRY_TREE_ROUTE_2              22
#define BERRY_TREE_ROUTE_3              23
#define BERRY_TREE_ROUTE_4              24
#define BERRY_TREE_ROUTE_5              25
#define BERRY_TREE_ROUTE_6              26
#define BERRY_TREE_ROUTE_7              27
#define BERRY_TREE_ROUTE_8              28
#define BERRY_TREE_ROUTE_9              29
#define BERRY_TREE_ROUTE_10             30
#define BERRY_TREE_ROUTE_11             31
#define BERRY_TREE_ROUTE_12             32
#define BERRY_TREE_ROUTE_13             33
#define BERRY_TREE_ROUTE_14             34
#define BERRY_TREE_ROUTE_15             35
#define BERRY_TREE_ROUTE_16             36
#define BERRY_TREE_ROUTE_17             37
#define BERRY_TREE_ROUTE_18             38
#define BERRY_TREE_ROUTE_19             39
#define BERRY_TREE_ROUTE_20             40

#define BERRY_TREE_ROUTE_FIRST          BERRY_TREE_ROUTE_1
#define BERRY_TREE_ROUTE_LAST           BERRY_TREE_ROUTE_20
#define BERRY_TREE_ROUTE_COUNT          (BERRY_TREE_ROUTE_LAST - BERRY_TREE_ROUTE_FIRST + 1)


#define BERRY_TREE_DAYCARE_1            41
#define BERRY_TREE_DAYCARE_2            42
#define BERRY_TREE_DAYCARE_3            43
#define BERRY_TREE_DAYCARE_4            44
#define BERRY_TREE_DAYCARE_5            45
#define BERRY_TREE_DAYCARE_6            46
#define BERRY_TREE_DAYCARE_7            47
#define BERRY_TREE_DAYCARE_8            48
#define BERRY_TREE_DAYCARE_9            49
#define BERRY_TREE_DAYCARE_10           50
#define BERRY_TREE_DAYCARE_11           51
#define BERRY_TREE_DAYCARE_12           52
#define BERRY_TREE_DAYCARE_13           53
#define BERRY_TREE_DAYCARE_14           54
#define BERRY_TREE_DAYCARE_15           55
#define BERRY_TREE_DAYCARE_16           56
#define BERRY_TREE_DAYCARE_17           57
#define BERRY_TREE_DAYCARE_18           58
#define BERRY_TREE_DAYCARE_19           59
#define BERRY_TREE_DAYCARE_20           60

#define BERRY_TREE_DAYCARE_FIRST        BERRY_TREE_DAYCARE_1
#define BERRY_TREE_DAYCARE_LAST         BERRY_TREE_DAYCARE_20
#define BERRY_TREE_DAYCARE_COUNT        (BERRY_TREE_DAYCARE_LAST - BERRY_TREE_DAYCARE_FIRST + 1)

// Remainder are unused

#define BERRY_TREES_COUNT 128

#endif // GUARD_CONSTANTS_BERRY_H
