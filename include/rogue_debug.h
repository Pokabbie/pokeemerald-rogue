#ifndef ROGUE_DEBUG_H
#define ROGUE_DEBUG_H

// Comment features in if want to use them

// Memory Stomp Tracking:
// - Helper to narrow down where a memory stomp is happening
// Usage:
// MEMORY_STOMP_TRACKING_SET_TARGET - Pass in the memory address of the target (Be aware of deallocs)
// MEMORY_STOMP_TRACKING_POLL - Place around the code to narrow down where the stomp is happening
//

// (uncomment to enable)
#ifdef ROGUE_DEBUG
#define DEBUG_FEATURE_MEMORY_STOMP_TRACKING
#endif


void RogueDebug_MainInit(void);
void RogueDebug_MainCB(void);


#ifdef DEBUG_FEATURE_MEMORY_STOMP_TRACKING

#define MEMORY_STOMP_TRACKING_SET_TARGET(ptr, size) RogueMemStomp_SetTarget(ptr, size)
#define MEMORY_STOMP_TRACKING_POLL() RogueMemStomp_Poll()

void RogueMemStomp_SetTarget(void const* ptr, size_t size);
void RogueMemStomp_Poll();
#else
#define MEMORY_STOMP_TRACKING_SET_TARGET(ptr, size)
#define MEMORY_STOMP_TRACKING_POLL()

#endif

#endif //ROGUE_DEBUG_H
