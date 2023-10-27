
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
    void* compareValue;
    size_t watchSize;
};

static EWRAM_DATA struct MemoryStompTracker sMemoryStompTracker = {0};

void RogueMemStomp_SetTarget(void const* ptr, size_t size)
{
    if(sMemoryStompTracker.compareValue != NULL)
    {
        free(sMemoryStompTracker.compareValue);
        sMemoryStompTracker.compareValue = NULL;
    }

    if(ptr == NULL || size == 0)
    {
        DebugPrint("MemStomp: Tracking NULL");
        sMemoryStompTracker.watchPtr = NULL;
        sMemoryStompTracker.watchSize = 0;
    }
    else
    {
        DebugPrintf("MemStomp: Tracking %p (size: %d)", ptr, size);
        sMemoryStompTracker.watchPtr = ptr;
        sMemoryStompTracker.watchSize = size;

        sMemoryStompTracker.compareValue = malloc(size);

        memcpy(sMemoryStompTracker.compareValue, ptr, size);
    }

    // Poll on initial set
    MEMORY_STOMP_TRACKING_POLL();
}

void RogueMemStomp_Poll()
{
    if(sMemoryStompTracker.watchPtr != NULL)
    {
        AGB_ASSERT(memcmp(sMemoryStompTracker.compareValue, sMemoryStompTracker.watchPtr, sMemoryStompTracker.watchSize) == 0);
    }
}

#endif // DEBUG_FEATURE_MEMORY_STOMP_TRACKING