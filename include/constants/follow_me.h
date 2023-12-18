#ifndef GUARD_CONSTANTS_FOLLOW_ME_H
#define GUARD_CONSTANTS_FOLLOW_ME_H

#define FOLLOWER_FLAG_HAS_RUNNING_FRAMES    (1 << 0)
#define FOLLOWER_FLAG_CAN_BIKE              (1 << 1)
#define FOLLOWER_FLAG_CAN_LEAVE_ROUTE       (1 << 2)     // teleport, dig, fly, etc
#define FOLLOWER_FLAG_CAN_SURF              (1 << 3)
#define FOLLOWER_FLAG_CAN_WATERFALL         (1 << 4)
#define FOLLOWER_FLAG_CAN_DIVE              (1 << 5)
#define FOLLOWER_FLAG_CAN_ROCK_CLIMB        (1 << 6)    // need rock climb implemented
#define FOLLOWER_FLAG_CLEAR_ON_WHITE_OUT    (1 << 7)
#define FOLLOWER_FLAG_CUSTOM_FOLLOW_SCRIPT  (1 << 8)   // follower has custom script while following (use loadword prior to setfollower)
#define FOLLOWER_FLAG_FOLLOW_DURING_SCRIPT  (1 << 9)

#define FOLLOW_SCRIPT_ID_NONE               0
#define FOLLOW_SCRIPT_ID_FOLLOW_MON         1

#define FOLLOWER_FLAG_ALL_WATER             FOLLOWER_FLAG_CAN_SURF | FOLLOWER_FLAG_CAN_WATERFALL | FOLLOWER_FLAG_CAN_DIVE
#define FOLLOWER_FLAG_ALL_LAND              FOLLOWER_FLAG_HAS_RUNNING_FRAMES | FOLLOWER_FLAG_CAN_BIKE | FOLLOWER_FLAG_CAN_LEAVE_ROUTE
#define FOLLOWER_FLAG_ALL                   FOLLOWER_FLAG_ALL_WATER | FOLLOWER_FLAG_ALL_LAND | FOLLOWER_FLAG_CLEAR_ON_WHITE_OUT


#endif // GUARD_CONSTANTS_FOLLOW_ME_H