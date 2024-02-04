#ifndef GUARD_RANDOM_H
#define GUARD_RANDOM_H

// The number 1103515245 comes from the example implementation of rand and srand
// in the ISO C standard.
#define ISO_RANDOMIZE1(val)(1103515245 * (val) + 24691)
#define ISO_RANDOMIZE2(val)(1103515245 * (val) + 12345)

#ifdef ROGUE_FEATURE_HQ_RANDOM

extern struct PCG32 gRngValue;
extern struct PCG32 gRng2Value;
extern struct PCG32 gRngRogueValue;

#define RAND_TYPE struct PCG32

u32 Random32(void);
u32 RandomAlt32(void);
u32 RogueRandom32(void);
u32 RandomCustom32(u32* seed);


//Returns a 16-bit pseudorandom number
#define Random_16(value) ((u16)(value ^ (value >> 16)))

#define Random()            Random_16(Random32())
#define Random2()           Random_16(RandomAlt32())
#define RogueRandom()       Random_16(RogueRandom32())
#define RandomCustom(seed)  Random_16(RandomCustom32(seed))

//Sets the initial seed value of the pseudorandom number generator
void SeedRng(u32 seed);
void SeedRng2(u32 seed);
void SeedRogueRng(u32 seed);

#else

extern u32 gRngValue;
extern u32 gRng2Value;
extern u32 gRngRogueValue;

#define RAND_TYPE u32

//Returns a 16-bit pseudorandom number
u16 Random(void);
u16 Random2(void);
u16 RogueRandom(void);
u16 RandomCustom(u32* seed);

//Returns a 32-bit pseudorandom number
#define Random32() (Random() | (Random() << 16))

//Sets the initial seed value of the pseudorandom number generator
void SeedRng(u16 seed);
void SeedRng2(u16 seed);
void SeedRogueRng(u16 seed);

#endif

#endif // GUARD_RANDOM_H
