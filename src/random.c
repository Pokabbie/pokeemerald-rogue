#include "global.h"
#include "random.h"

#ifdef ROGUE_FEATURE_HQ_RANDOM

struct PCG32 gRngValue;
struct PCG32 gRng2Value;
struct PCG32 gRngRogueValue;

u32 RandomPCG32(struct PCG32* rng);

u32 Random32(void)
{
    return RandomPCG32(&gRngValue);
}

u32 RandomAlt32(void)
{
    return RandomPCG32(&gRng2Value);
}

u32 RogueRandom32(void)
{
    return RandomPCG32(&gRngRogueValue);
}

u32 RandomCustom32(u32* seed)
{
    struct PCG32 rng;
    u32 result;
    rng.seed = *seed;
    rng.high = *seed;
    rng.low = *seed;

    result = RandomPCG32(&rng);

    *seed = rng.seed;
    return result;
}

void SeedRng(u32 seed)
{
    gRngValue.seed = seed;
    gRngValue.high = seed;
    gRngValue.low = seed;
}

void SeedRng2(u32 seed)
{
    gRng2Value.seed = seed;
    gRng2Value.high = seed;
    gRng2Value.low = seed;
}

void SeedRogueRng(u32 seed)
{
    gRngRogueValue.seed = seed;
    gRngRogueValue.high = seed;
    gRngRogueValue.low = seed;
}

#else
EWRAM_DATA static u8 sUnknown = 0;
EWRAM_DATA static u32 sRandCount = 0;

// IWRAM common
u32 gRngValue;
u32 gRng2Value;
u32 gRngRogueValue;

u16 Random(void)
{
    gRngValue = ISO_RANDOMIZE1(gRngValue);
    sRandCount++;
    return gRngValue >> 16;
}

void SeedRng(u16 seed)
{
    gRngValue = seed;
    sUnknown = 0;
}

void SeedRng2(u16 seed)
{
    gRng2Value = seed;
}

u16 Random2(void)
{
    gRng2Value = ISO_RANDOMIZE1(gRng2Value);
    return gRng2Value >> 16;
}

void SeedRogueRng(u16 seed)
{
    gRngRogueValue = seed;
}

u16 RogueRandom(void)
{
    gRngRogueValue = ISO_RANDOMIZE1(gRngRogueValue);
    return gRngRogueValue >> 16;
}

u16 RandomCustom(u32* seed)
{
    *seed = ISO_RANDOMIZE1(*seed);
    return *seed >> 16;
}
#endif