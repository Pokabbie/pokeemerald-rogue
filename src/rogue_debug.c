
#include "global.h"
#include "malloc.h"

#include "rogue_debug.h"

#ifdef ROGUE_DEBUG
void RogueDebug_MainInit(void)
{
    MEMORY_STOMP_TRACKING_SET_TARGET(NULL, 0);
}

void RogueDebug_MainCB(void)
{
    MEMORY_STOMP_TRACKING_POLL();
}

#else // !DEBUG - stubs

void RogueDebug_MainInit(void)
{

}

void RogueDebug_MainCB(void)
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