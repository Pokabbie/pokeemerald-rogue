#ifndef GUARD_CONSTANTS_VARS_H
#define GUARD_CONSTANTS_VARS_H

#define VARS_START 0x4000

// temporary vars
// The first 0x10 vars are are temporary--they are cleared every time a map is loaded.
#define TEMP_VARS_START            0x4000
#define VAR_TEMP_0                 (TEMP_VARS_START + 0x0)
#define VAR_TEMP_1                 (TEMP_VARS_START + 0x1)
#define VAR_TEMP_2                 (TEMP_VARS_START + 0x2)
#define VAR_TEMP_3                 (TEMP_VARS_START + 0x3)
#define VAR_TEMP_4                 (TEMP_VARS_START + 0x4)
#define VAR_TEMP_5                 (TEMP_VARS_START + 0x5)
#define VAR_TEMP_6                 (TEMP_VARS_START + 0x6)
#define VAR_TEMP_7                 (TEMP_VARS_START + 0x7)
#define VAR_TEMP_8                 (TEMP_VARS_START + 0x8)
#define VAR_TEMP_9                 (TEMP_VARS_START + 0x9)
#define VAR_TEMP_A                 (TEMP_VARS_START + 0xA)
#define VAR_TEMP_B                 (TEMP_VARS_START + 0xB)
#define VAR_TEMP_C                 (TEMP_VARS_START + 0xC)
#define VAR_TEMP_D                 (TEMP_VARS_START + 0xD)
#define VAR_TEMP_E                 (TEMP_VARS_START + 0xE)
#define VAR_TEMP_F                 (TEMP_VARS_START + 0xF)
#define TEMP_VARS_END              VAR_TEMP_F

// object gfx id vars
// These 0x10 vars are used to dynamically control a map object's sprite.
// For example, the rival's sprite id is dynamically set based on the player's gender.
// See VarGetObjectEventGraphicsId().
#define VAR_OBJ_GFX_ID_0           0x4010
#define VAR_OBJ_GFX_ID_1           0x4011
#define VAR_OBJ_GFX_ID_2           0x4012
#define VAR_OBJ_GFX_ID_3           0x4013
#define VAR_OBJ_GFX_ID_4           0x4014
#define VAR_OBJ_GFX_ID_5           0x4015
#define VAR_OBJ_GFX_ID_6           0x4016
#define VAR_OBJ_GFX_ID_7           0x4017
#define VAR_OBJ_GFX_ID_8           0x4018
#define VAR_OBJ_GFX_ID_9           0x4019
#define VAR_OBJ_GFX_ID_A           0x401A
#define VAR_OBJ_GFX_ID_B           0x401B
#define VAR_OBJ_GFX_ID_C           0x401C
#define VAR_OBJ_GFX_ID_D           0x401D
#define VAR_OBJ_GFX_ID_E           0x401E
#define VAR_OBJ_GFX_ID_F           0x401F

// general purpose vars
#define VAR_RECYCLE_GOODS                    0x4020
#define VAR_REPEL_STEP_COUNT                 0x4021
#define VAR_ICE_STEP_COUNT                   0x4022
#define VAR_STARTER_MON                      0x4023 // 0=Treecko, 1=Torchic, 2=Mudkip
#define VAR_MIRAGE_RND_H                     0x4024
#define VAR_MIRAGE_RND_L                     0x4025
#define VAR_SECRET_BASE_MAP                  0x4026
#define VAR_CYCLING_ROAD_RECORD_COLLISIONS   0x4027
#define VAR_CYCLING_ROAD_RECORD_TIME_L       0x4028
#define VAR_CYCLING_ROAD_RECORD_TIME_H       0x4029
#define VAR_FRIENDSHIP_STEP_COUNTER          0x402A
#define VAR_POISON_STEP_COUNTER              0x402B
#define VAR_RESET_RTC_ENABLE                 0x402C
#define VAR_ENIGMA_BERRY_AVAILABLE           0x402D
#define VAR_WONDER_NEWS_COUNTER              0x402E

#define VAR_ROGUE_DEX_VARIANT                0x402F
#define VAR_ROGUE_DEX_GEN_LIMIT              0x4030
#define VAR_UNUSED_0x4031                    0x4031
#define VAR_UNUSED_0x4032                    0x4032
#define VAR_UNUSED_0x4033                    0x4033

#define VAR_DEOXYS_ROCK_STEP_COUNT           0x4034
#define VAR_DEOXYS_ROCK_LEVEL                0x4035
#define VAR_PC_BOX_TO_SEND_MON               0x4036
#define VAR_UNUSED_0x4037                    0x4037
#define VAR_UNUSED_0x4038                    0x4038
#define VAR_UNUSED_0x4039                    0x4039
#define VAR_FARAWAY_ISLAND_STEP_COUNTER      0x403A
#define VAR_REGICE_STEPS_1                   0x403B
#define VAR_REGICE_STEPS_2                   0x403C
#define VAR_REGICE_STEPS_3                   0x403D
#define VAR_UNUSED_0x403E                    0x403E
#define VAR_UNUSED_0x403F                    0x403F
#define VAR_DAYS                             0x4040
#define VAR_ROGUE_FLASK_HEALS_USED           0x4041
#define VAR_ROGUE_FLASK_HEALS_MAX            0x4042
#define VAR_UNUSED_0x4043                    0x4043
#define VAR_UNUSED_0x4044                    0x4044
#define VAR_UNUSED_0x4045                    0x4045
#define VAR_NATIONAL_DEX                     0x4046
#define VAR_UNUSED_0x4046                    0x4047
#define VAR_UNUSED_0x4048                    0x4048
#define VAR_UNUSED_0x4049                    0x4049
#define VAR_UNUSED_0x404A                    0x404A
#define VAR_UNUSED_0x404B                    0x404B
#define VAR_UNUSED_0x404C                    0x404C
#define VAR_UNUSED_0x404D                    0x404D
#define VAR_UNUSED_0x404E                    0x404E 
#define VAR_UNUSED_0x404F                    0x404F
#define VAR_UNUSED_0x4050                    0x4050
#define VAR_UNUSED_0x4051                    0x4051
#define VAR_UNUSED_0x4052                    0x4052 
#define VAR_UNUSED_0x4053                    0x4053
#define VAR_UNUSED_0x4054                    0x4054
#define VAR_UNUSED_0x4055                    0x4055 
#define VAR_UNUSED_0x4056                    0x4056 
#define VAR_UNUSED_0x4057                    0x4057
#define VAR_UNUSED_0x4058                    0x4058
#define VAR_UNUSED_0x4059                    0x4059 
#define VAR_UNUSED_0x405A                    0x405A
#define VAR_UNUSED_0x405B                    0x405B 
#define VAR_UNUSED_0x405C                    0x405C 
#define VAR_UNUSED_0x405D                    0x405D
#define VAR_UNUSED_0x405E                    0x405E
#define VAR_UNUSED_0x405F                    0x405F 
#define VAR_UNUSED_0x4060                    0x4060
#define VAR_ROGUE_ITEM0                      0x4061 
#define VAR_ROGUE_ITEM1                      0x4062 
#define VAR_UNUSED_0x4063                    0x4063
#define VAR_ROGUE_ITEM2                      0x4064 
#define VAR_ROGUE_ITEM3                      0x4065 
#define VAR_ROGUE_ITEM4                      0x4066 
#define VAR_UNUSED_0x4067                    0x4067 
#define VAR_ROGUE_ITEM5                      0x4068 
#define VAR_UNUSED_0x4069                    0x4069
#define VAR_ROGUE_ITEM6                      0x406A 
#define VAR_ROGUE_ITEM7                      0x406B 
#define VAR_ROGUE_ITEM8                      0x406C 
#define VAR_ROGUE_ITEM9                      0x406D 
#define VAR_ROGUE_ITEM10                     0x406E 
#define VAR_UNUSED_0x406F                    0x406F
#define VAR_ROGUE_ITEM11                     0x4070 
#define VAR_UNUSED_0x4071                    0x4071
#define VAR_UNUSED_0x4072                    0x4072
#define VAR_UNUSED_0x4073                    0x4073 
#define VAR_UNUSED_0x4074                    0x4074
#define VAR_ROGUE_ITEM12                     0x4075 
#define VAR_ROGUE_ITEM13                     0x4076 
#define VAR_ROGUE_ITEM14                     0x4077 
#define VAR_ROGUE_ITEM15                     0x4078 
#define VAR_ROGUE_ITEM16                     0x4079 
#define VAR_ROGUE_ITEM17                     0x407A 
#define VAR_UNUSED_0x407B                    0x407B
#define VAR_ROGUE_ITEM18                     0x407C 
#define VAR_ROGUE_DIFFICULTY                 0x407D
#define VAR_ROGUE_FURTHEST_DIFFICULTY        0x407E
#define VAR_ROGUE_STARTER0                   0x407F
#define VAR_ROGUE_STARTER1                   0x4080 
#define VAR_ROGUE_STARTER2                   0x4081
#define VAR_UNUSED_0x4082                    0x4082
#define VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2    0x4083
#define VAR_UNUSED_0x4084                    0x4084
#define VAR_UNUSED_0x4085                    0x4085 // 0-1: Wally tutorial, 2-6: 0-4 badges, 7: Defeated Norman, 8: Rematch Norman
#define VAR_UNUSED_0x4086                    0x4086
#define VAR_CABLE_CLUB_STATE                 0x4087
#define VAR_CONTEST_TYPE                     0x4088
#define VAR_SECRET_BASE_INITIALIZED          0x4089
#define VAR_CONTEST_PRIZE_PICKUP             0x408A
#define VAR_ROGUE_CURRENT_ROOM_IDX           0x408B
#define VAR_LITTLEROOT_HOUSES_STATE_BRENDAN  0x408C
#define VAR_LITTLEROOT_RIVAL_STATE           0x408D
#define VAR_BOARD_BRINEY_BOAT_STATE          0x408E
#define VAR_DEVON_CORP_3F_STATE              0x408F
#define VAR_BRINEY_HOUSE_STATE               0x4090
#define VAR_UNUSED_0x4091                    0x4091
#define VAR_LITTLEROOT_INTRO_STATE           0x4092
#define VAR_MAUVILLE_GYM_STATE               0x4093
#define VAR_LILYCOVE_MUSEUM_2F_STATE         0x4094
#define VAR_LILYCOVE_FAN_CLUB_STATE          0x4095
#define VAR_BRINEY_LOCATION                  0x4096
#define VAR_INIT_SECRET_BASE                 0x4097
#define VAR_PETALBURG_WOODS_STATE            0x4098
#define VAR_LILYCOVE_CONTEST_LOBBY_STATE     0x4099
#define VAR_RUSTURF_TUNNEL_STATE             0x409A
#define VAR_ROGUE_DESIRED_WEATHER            0x409B
#define VAR_ELITE_4_STATE                    0x409C
#define VAR_ROGUE_SKIP_TO_DIFFICULTY         0x409D
#define VAR_MOSSDEEP_SPACE_CENTER_STAIR_GUARD_STATE    0x409E
#define VAR_MOSSDEEP_SPACE_CENTER_STATE      0x409F
#define VAR_SLATEPORT_HARBOR_STATE           0x40A0
#define VAR_ROGUE_ADVENTURE_MONEY            0x40A1
#define VAR_SEAFLOOR_CAVERN_STATE            0x40A2
#define VAR_CABLE_CAR_STATION_STATE          0x40A3
#define VAR_SAFARI_ZONE_STATE                0x40A4  // 0: In or out of SZ, 1: Player exiting SZ, 2: Player entering SZ
#define VAR_TRICK_HOUSE_BEING_WATCHED_STATE  0x40A5
#define VAR_TRICK_HOUSE_FOUND_TRICK_MASTER   0x40A6
#define VAR_TRICK_HOUSE_ENTRANCE_STATE       0x40A7
#define VAR_ROGUE_ENABLED_GEN_LIMIT          0x40A8
#define VAR_CYCLING_CHALLENGE_STATE          0x40A9
#define VAR_SLATEPORT_MUSEUM_1F_STATE        0x40AA
#define VAR_TRICK_HOUSE_PUZZLE_1_STATE       0x40AB
#define VAR_TRICK_HOUSE_PUZZLE_2_STATE       0x40AC
#define VAR_TRICK_HOUSE_PUZZLE_3_STATE       0x40AD
#define VAR_TRICK_HOUSE_PUZZLE_4_STATE       0x40AE
#define VAR_TRICK_HOUSE_PUZZLE_5_STATE       0x40AF
#define VAR_TRICK_HOUSE_PUZZLE_6_STATE       0x40B0
#define VAR_TRICK_HOUSE_PUZZLE_7_STATE       0x40B1
#define VAR_TRICK_HOUSE_PUZZLE_8_STATE       0x40B2
#define VAR_WEATHER_INSTITUTE_STATE          0x40B3
#define VAR_SS_TIDAL_STATE                   0x40B4
#define VAR_TRICK_HOUSE_ENTER_FROM_CORRIDOR  0x40B5
#define VAR_TRICK_HOUSE_PUZZLE_7_STATE_2     0x40B6 // Leftover from RS, never set
#define VAR_SLATEPORT_FAN_CLUB_STATE         0x40B7
#define VAR_ROGUE_SPECIAL_ENCOUNTER_DATA     0x40B8
#define VAR_MT_PYRE_STATE                    0x40B9
#define VAR_NEW_MAUVILLE_STATE               0x40BA
#define VAR_ROGUE_CURRENT_LEVEL_CAP          0x40BB
#define VAR_BRAVO_TRAINER_BATTLE_TOWER_ON    0x40BC
#define VAR_JAGGED_PASS_ASH_WEATHER          0x40BD
#define VAR_GLASS_WORKSHOP_STATE             0x40BE
#define VAR_METEOR_FALLS_STATE               0x40BF
#define VAR_SOOTOPOLIS_MYSTERY_EVENTS_STATE  0x40C0
#define VAR_TRICK_HOUSE_PRIZE_PICKUP         0x40C1
#define VAR_PACIFIDLOG_TM_RECEIVED_DAY       0x40C2
#define VAR_VICTORY_ROAD_1F_STATE            0x40C3
#define VAR_FOSSIL_RESURRECTION_STATE        0x40C4
#define VAR_WHICH_FOSSIL_REVIVED             0x40C5
#define VAR_STEVENS_HOUSE_STATE              0x40C6
#define VAR_OLDALE_RIVAL_STATE               0x40C7
#define VAR_FOLLOW_MON_0                     0x40C8
#define VAR_FOLLOW_MON_1                     0x40C9
#define VAR_FOLLOW_MON_2                     0x40CA
#define VAR_FOLLOW_MON_3                     0x40CB
#define VAR_FOLLOW_MON_4                     0x40CC
#define VAR_FOLLOW_MON_5                     0x40CD
#define VAR_FOLLOW_MON_6                     0x40CE
#define VAR_FOLLOW_MON_7                     0x40CF
#define VAR_FOLLOW_MON_8                     0x40D0
#define VAR_FOLLOW_MON_9                     0x40D1
#define VAR_FOLLOW_MON_A                     0x40D2
#define VAR_FOLLOW_MON_B                     0x40D3
#define VAR_FOLLOW_MON_C                     0x40D4
#define VAR_FOLLOW_MON_D                     0x40D5
#define VAR_FOLLOW_MON_E                     0x40D6
#define VAR_FOLLOW_MON_F                     0x40D7
#define VAR_SKY_PILLAR_STATE                 0x40D8
#define VAR_FRONTIER_BATTLE_MODE             0x40D9
#define VAR_FRONTIER_FACILITY                0x40DA
#define VAR_ROAMER_POKEMON                   0x40DB
#define VAR_ROGUE_REGION_DEX_LIMIT           0x40DC
#define VAR_GIFT_PICHU_SLOT                  0x40DD
#define VAR_GIFT_UNUSED_1                    0x40DE // Var is written to, but never read
#define VAR_GIFT_UNUSED_2                    0x40DF // Var is written to, but never read
#define VAR_GIFT_UNUSED_3                    0x40E0 // Var is written to, but never read
#define VAR_GIFT_UNUSED_4                    0x40E1 // Var is written to, but never read
#define VAR_GIFT_UNUSED_5                    0x40E2 // Var is written to, but never read
#define VAR_GIFT_UNUSED_6                    0x40E3 // Var is written to, but never read
#define VAR_GIFT_UNUSED_7                    0x40E4 // var is written to, but never read
#define VAR_ROGUE_SAFARI_GENERATION          0x40E5
#define VAR_DAILY_SLOTS                      0x40E6
#define VAR_DAILY_WILDS                      0x40E7
#define VAR_DAILY_BLENDER                    0x40E8
#define VAR_DAILY_PLANTED_BERRIES            0x40E9
#define VAR_DAILY_PICKED_BERRIES             0x40EA
#define VAR_DAILY_ROULETTE                   0x40EB
#define VAR_SECRET_BASE_STEP_COUNTER         0x40EC // Used by Secret Base TV programs
#define VAR_SECRET_BASE_LAST_ITEM_USED       0x40ED // Used by Secret Base TV programs
#define VAR_SECRET_BASE_LOW_TV_FLAGS         0x40EE // Used by Secret Base TV programs
#define VAR_SECRET_BASE_HIGH_TV_FLAGS        0x40EF // Used by Secret Base TV programs
#define VAR_SECRET_BASE_IS_NOT_LOCAL         0x40F0 // Set to TRUE while in another player's secret base.
#define VAR_DAILY_BP                         0x40F1
#define VAR_WALLY_CALL_STEP_COUNTER          0x40F2
#define VAR_SCOTT_FORTREE_CALL_STEP_COUNTER  0x40F3
#define VAR_ROXANNE_CALL_STEP_COUNTER        0x40F4
#define VAR_SCOTT_BF_CALL_STEP_COUNTER       0x40F5
#define VAR_RIVAL_RAYQUAZA_CALL_STEP_COUNTER 0x40F6
#define VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1    0x40F7
#define VAR_ROGUE_DESIRED_CAMPAIGN           0x40F8
#define VAR_ROGUE_ACTIVE_CAMPAIGN            0x40F9
#define VAR_UNUSED_0x40FA                    0x40FA 
#define VAR_UNUSED_0x40FB                    0x40FB 
#define VAR_UNUSED_0x40FC                    0x40FC 
#define VAR_UNUSED_0x40FD                    0x40FD 
#define VAR_UNUSED_0x40FE                    0x40FE 
#define VAR_UNUSED_0x40FF                    0x40FF 

#define VARS_END                             0x40FF
#define VARS_COUNT                           (VARS_END - VARS_START + 1)

#define SPECIAL_VARS_START            0x8000
// special vars
// They are commonly used as parameters to commands, or return values from commands.
#define VAR_0x8000                    0x8000
#define VAR_0x8001                    0x8001
#define VAR_0x8002                    0x8002
#define VAR_0x8003                    0x8003
#define VAR_0x8004                    0x8004
#define VAR_0x8005                    0x8005
#define VAR_0x8006                    0x8006
#define VAR_0x8007                    0x8007
#define VAR_0x8008                    0x8008
#define VAR_0x8009                    0x8009
#define VAR_0x800A                    0x800A
#define VAR_0x800B                    0x800B
#define VAR_FACING                    0x800C
#define VAR_RESULT                    0x800D
#define VAR_ITEM_ID                   0x800E
#define VAR_LAST_TALKED               0x800F
#define VAR_CONTEST_RANK              0x8010
#define VAR_CONTEST_CATEGORY          0x8011
#define VAR_MON_BOX_ID                0x8012
#define VAR_MON_BOX_POS               0x8013
#define VAR_UNUSED_0x8014             0x8014
#define VAR_TRAINER_BATTLE_OPPONENT_A 0x8015 // Alias of gTrainerBattleOpponent_A

#define SPECIAL_VARS_END              0x8015

#endif // GUARD_CONSTANTS_VARS_H
