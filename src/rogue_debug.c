
#include "global.h"
#include "constants/songs.h"
#include "main.h"
#include "malloc.h"
#include "sound.h"
#include "string.h"
#include "string_util.h"

#include "rogue_baked.h"
#include "rogue_debug.h"
#include "rogue_quest.h"

#ifdef DEBUG_FEATURE_FRAME_TIMERS
static void ResetClockCounters();
static u32 SampleClock();
static void ResetTimerData();
static void EnqueuePrintTimers();
#endif

#ifdef ROGUE_DEBUG

static u32 CountMoneyQuestRewards(u32 questFlag)
{
    u16 i, j;
    u32 total = 0;

    for(i = 0; i < QUEST_ID_COUNT; ++i)
    {
        if(RogueQuest_GetConstFlag(i, questFlag))
        {
            for(j = 0; j < RogueQuest_GetRewardCount(i); ++j)
            {
                struct RogueQuestReward const* reward = RogueQuest_GetReward(i, j);

                if(reward->type == QUEST_REWARD_MONEY)
                {
                    total += reward->perType.money.amount;
                }
            }
        }
    }

    return total;
}

static u32 CountItemQuestRewards(u16 itemId, u32 questFlag)
{
    u16 i, j;
    u32 total = 0;

    for(i = 0; i < QUEST_ID_COUNT; ++i)
    {
        if(RogueQuest_GetConstFlag(i, questFlag))
        {
            for(j = 0; j < RogueQuest_GetRewardCount(i); ++j)
            {
                struct RogueQuestReward const* reward = RogueQuest_GetReward(i, j);

                if(reward->type == QUEST_REWARD_ITEM && reward->perType.item.item == itemId)
                {
                    AGB_ASSERT(reward->perType.item.count < 32); // sanity check to make sure we're not giving too much out at once 
                    total += reward->perType.item.count;
                }
            }
        }
    }

    return total;
}

static u32 CountBuildingSuppliesNeededForUpgrades()
{
    u16 i;
    u32 total = 0;

    for(i = 0; i < ARRAY_COUNT(gRogueHubAreas); ++i)
    {
        total += gRogueHubAreas[i].buildCost;
    }

    for(i = 0; i < ARRAY_COUNT(gRogueHubUpgrades); ++i)
    {
        total += gRogueHubUpgrades[i].buildCost;
    }

    return total;
}

extern const u16 gRogueBake_FinalEvoSpecies_Count;
extern const u16 gRogueBake_EggSpecies_Count;

void RogueDebug_MainInit(void)
{
    MEMORY_STOMP_TRACKING_SET_TARGET(NULL, 0);

    // Data checks
    {
        DebugPrint("[Quest Verify]");
        DebugPrint("    [Counts]");
        DebugPrintf("        [Main] %d", RogueQuest_GetQuestTotalCountFor(QUEST_CONST_IS_MAIN_QUEST, TRUE));
        DebugPrintf("        [Challenge] %d", RogueQuest_GetQuestTotalCountFor(QUEST_CONST_IS_CHALLENGE, TRUE));
        DebugPrintf("        [Mastery] %d", RogueQuest_GetQuestTotalCountFor(QUEST_CONST_IS_MON_MASTERY, TRUE));

        DebugPrint("    [Rewards]");
        DebugPrintf("        [Main] Money:%d", CountMoneyQuestRewards(QUEST_CONST_IS_MAIN_QUEST));
        DebugPrintf("        [Main] BuildingSupplies:%d", CountItemQuestRewards(ITEM_BUILDING_SUPPLIES, QUEST_CONST_IS_MAIN_QUEST));

        DebugPrintf("        [Challenge] Money:%d", CountMoneyQuestRewards(QUEST_CONST_IS_CHALLENGE));
        DebugPrintf("        [Challenge] BuildingSupplies:%d", CountItemQuestRewards(ITEM_BUILDING_SUPPLIES, QUEST_CONST_IS_CHALLENGE));

        DebugPrintf("        [Mastery] Money:%d", CountMoneyQuestRewards(QUEST_CONST_IS_MON_MASTERY));
        DebugPrintf("        [Mastery] BuildingSupplies:%d", CountItemQuestRewards(ITEM_BUILDING_SUPPLIES, QUEST_CONST_IS_MON_MASTERY));


        // None of these should give building supplies
        AGB_ASSERT(CountItemQuestRewards(ITEM_BUILDING_SUPPLIES, QUEST_CONST_IS_CHALLENGE) == 0);
        AGB_ASSERT(CountItemQuestRewards(ITEM_BUILDING_SUPPLIES, QUEST_CONST_IS_MON_MASTERY) == 0);

        DebugPrint("[Hub Verify]");
        DebugPrintf("    TotalCost:%d", CountBuildingSuppliesNeededForUpgrades());
        DebugPrintf("    Remainder:%d", CountItemQuestRewards(ITEM_BUILDING_SUPPLIES, QUEST_CONST_IS_MAIN_QUEST) - CountBuildingSuppliesNeededForUpgrades());

        AGB_ASSERT(CountBuildingSuppliesNeededForUpgrades() <= CountItemQuestRewards(ITEM_BUILDING_SUPPLIES, QUEST_CONST_IS_MAIN_QUEST));


        DebugPrint("[Bake Verify]");
        DebugPrintf("    FinalEvoSpecies_Count:%d", gRogueBake_FinalEvoSpecies_Count);
        DebugPrintf("    EggSpecies_Count:%d", gRogueBake_EggSpecies_Count);

        AGB_ASSERT(gRogueBake_FinalEvoSpecies_Count == SPECIES_FINAL_EVO_STAGE_COUNT);
        AGB_ASSERT(gRogueBake_EggSpecies_Count == SPECIES_EGG_EVO_STAGE_COUNT);

        {
            u32 i;

            // Comment in to debug
            //DebugPrint("[Mastery Flag]");

            for(i = SPECIES_NONE + 1; i < NUM_SPECIES; ++i)
            {
                //DebugPrintf("    check %d (egg:%d)", i, Rogue_GetEggSpecies(i));
                RogueQuest_GetMonMasteryFlag(i);
            }
        }
    }
}

void RogueDebug_MainCB(void)
{
    MEMORY_STOMP_TRACKING_POLL();

#ifdef DEBUG_FEATURE_FRAME_TIMERS
    if(JOY_HELD(R_BUTTON) && JOY_NEW(L_BUTTON))
    {
        EnqueuePrintTimers();
    }
#endif
}

void RogueDebug_ResetFrameTimers()
{
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    ResetClockCounters();
    ResetTimerData();
#endif
}

void RogueDebug_PrintTimers()
{
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    EnqueuePrintTimers();
#endif
}

#else // !DEBUG - stubs

void RogueDebug_MainInit(void)
{

}

void RogueDebug_MainCB(void)
{
}

void RogueDebug_ResetFrameTimers()
{
}

#endif // DEBUG


#ifdef DEBUG_FEATURE_MEMORY_STOMP_TRACKING
struct MemoryStompTracker
{
    void const* watchPtr;
    u8 compareValue[16];
    size_t watchSize;
};

static EWRAM_DATA struct MemoryStompTracker sMemoryStompTracker = {0};

// memcpy was acting weirdly, so use this as an alternative

static void ManualCopy(void* dst, void const* src, size_t size)
{
    size_t i;
    u8* dst8 = (u8*)dst;
    u8 const* src8 = (u8 const*)src;

    for(i = 0; i < size; ++i)
        dst8[i] = src8[i];
}

static bool8 ManualCompare(void const* dst, void const* src, size_t size)
{
    size_t i;
    u8 const* dst8 = (u8*)dst;
    u8 const* src8 = (u8 const*)src;

    for(i = 0; i < size; ++i)
    {
        if(dst8[i] != src8[i])
            return FALSE;
    }

    return TRUE;
}

void RogueMemStomp_SetTarget(void const* ptr, size_t size)
{
    AGB_ASSERT(size < ARRAY_COUNT(sMemoryStompTracker.compareValue));

    if(ptr == NULL || size == 0)
    {
        DebugPrint("MemStomp: Tracking NULL");
        sMemoryStompTracker.watchPtr = NULL;
        sMemoryStompTracker.watchSize = 0;
    }
    else
    {
        DebugPrintf("MemStomp: Tracking %x (size: %d)", ptr, size);
        sMemoryStompTracker.watchPtr = ptr;
        sMemoryStompTracker.watchSize = size;

        ManualCopy(sMemoryStompTracker.compareValue, ptr, size);
    }

    // Poll on initial set
    MEMORY_STOMP_TRACKING_POLL();
}

void RogueMemStomp_Poll()
{
    if(sMemoryStompTracker.watchPtr != NULL)
    {
        AGB_ASSERT(ManualCompare(sMemoryStompTracker.compareValue, sMemoryStompTracker.watchPtr, sMemoryStompTracker.watchSize) == TRUE);
    }
}

#endif // DEBUG_FEATURE_MEMORY_STOMP_TRACKING


#ifdef DEBUG_FEATURE_FRAME_TIMERS

struct FrameTimerData
{
    u32 duration;
    u32 marker : 16;
    u32 depth : 7;
    u32 hasStarted : 1;
    u32 unused : 8;
};

struct FrameTimerManager
{
    struct FrameTimerData timers[FRAME_TIMER_COUNT];
    u16 currentMarker;
    u16 currentDepth;
    u16 printTimerQueued : 1;
    u16 printTimerActive : 1;
};

static EWRAM_DATA struct FrameTimerManager sFrameTimerManager = {0};

static void ResetClockCounters()
{
    // reset counters
    REG_TM2CNT_L = 0;
    REG_TM3CNT_L = 0;

    // make sure timer 2 is disabled
    REG_TM2CNT_H = 0;
    REG_TM3CNT_H = 0;

    // init timers (tim3 count up mode, tim2 every clock cycle)
    REG_TM3CNT_H = TIMER_ENABLE | TIMER_COUNTUP;
    REG_TM2CNT_H = TIMER_1CLK | TIMER_ENABLE;
}

static u32 SampleClock()
{
    // stop timers
    //REG_TM2CNT_H = 0;
    //REG_TM3CNT_H = 0;

    // return result
    return REG_TM2CNT_L | (REG_TM3CNT_L << 16u);
}

u32 RogueDebug_SampleClock()
{
    return SampleClock();
}

void RogueDebug_StartTimer(u16 timer)
{
    AGB_ASSERT(!sFrameTimerManager.timers[timer].hasStarted);
    sFrameTimerManager.timers[timer].duration = SampleClock();
    sFrameTimerManager.timers[timer].hasStarted = TRUE;
    sFrameTimerManager.timers[timer].depth = sFrameTimerManager.currentDepth;
    sFrameTimerManager.timers[timer].marker = sFrameTimerManager.currentMarker++;
    ++sFrameTimerManager.currentDepth;
}

void RogueDebug_StopTimer(u16 timer)
{
    AGB_ASSERT(sFrameTimerManager.timers[timer].hasStarted);
    sFrameTimerManager.timers[timer].duration = (SampleClock() - sFrameTimerManager.timers[timer].duration);
    sFrameTimerManager.timers[timer].hasStarted = FALSE;
    --sFrameTimerManager.currentDepth;
}

#define PRINT_TIMER(timer) \
    if(sFrameTimerManager.timers[FRAME_TIMER_ ## timer].marker == i) \
    { \
        DebugPrintf("%s" #timer " = %d us (%d %%)", BufferIndents(sFrameTimerManager.timers[FRAME_TIMER_ ## timer].depth), GetFrameTimeInDisplayUnits(sFrameTimerManager.timers[FRAME_TIMER_ ## timer].duration), GetFrameTimeAsPercentage(sFrameTimerManager.timers[FRAME_TIMER_ ## timer].duration)); \
        continue; \
    }

static u8 const* BufferIndents(u32 depth)
{
    u32 i;
    for(i = 0; i < depth * 3; ++i)
    {
        gStringVar4[i] = 32; // space
    }

    gStringVar4[i] = 0;
    return gStringVar4;
}

static u32 GetFrameTimeInDisplayUnits(u32 time)
{
    // GBA clock takes 2^-24s per cycle
    // convert to microseconds
    u64 value = ((u64)time * (u64)1000000) / (u64)16777216;
    return (u32)value;
}

u32 RogueDebug_ClockToDisplayUnits(u32 time)
{
    return GetFrameTimeInDisplayUnits(time);
}

static u32 GetFrameTimeAsPercentage(u32 time)
{
    return (time * 100) / sFrameTimerManager.timers[FRAME_TIMER_FRAME_TOTAL].duration;
}

static void ResetTimerData()
{
    sFrameTimerManager.currentDepth = 0;

    if(sFrameTimerManager.printTimerActive)
    {
        u16 i;
        sFrameTimerManager.printTimerActive = FALSE;

        DebugPrint("======= TIMES START =======");

        for(i = 0; i < sFrameTimerManager.currentMarker; ++i)
        {
            FOR_EACH_TIMER(PRINT_TIMER)
        }

        DebugPrint("======== TIMES END ========");
        PlaySE(SE_EXP_MAX);
    }

    if(sFrameTimerManager.printTimerQueued)
    {
        u16 i;

        sFrameTimerManager.printTimerQueued = FALSE;
        sFrameTimerManager.printTimerActive = TRUE;

        // Clear all stale values
        for(i = 0; i < FRAME_TIMER_COUNT; ++i)
            sFrameTimerManager.timers[i].duration = 0;
    }
}

#undef PRINT_TIMER

static void EnqueuePrintTimers()
{
    sFrameTimerManager.printTimerQueued = TRUE;
}

#endif // DEBUG_FEATURE_FRAME_TIMERS