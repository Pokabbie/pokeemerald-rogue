#ifndef ROGUE_QUERY_H
#define ROGUE_QUERY_H

typedef bool8 (*QueryCallback)(u16 elem, u16 usrData);
typedef bool8 (*QueryFilterCallback)(u16 elem, void* usrData);
typedef u8 (*WeightCallback)(u16 index, u16 elem, void* usrData);

enum
{
    QUERY_FUNC_INCLUDE,
    QUERY_FUNC_EXCLUDE,
};

struct RogueQueryDebug
{
    u16 uncollapsedBufferCapacity;
    u8* uncollapsedBufferPtr;
    u16* collapseBufferPtr;
    u16* collapseSizePtr;
};

// RogueQuery 2.0 API
// Misc./Global
void RogueQuery_Init();
void RogueMiscQuery_EditElement(u8 func, u16 elem);
void RogueMiscQuery_EditRange(u8 func, u16 fromId, u16 toId);
bool8 RogueMiscQuery_CheckState(u16 elem);
bool8 RogueMiscQuery_AnyActiveStates(u16 fromId, u16 toId);
void RogueMiscQuery_FilterByChance(u16 rngSeed, u8 func, u8 chance, u8 minCount);
bool8 RogueMiscQuery_AnyActiveElements();
u16 RogueMiscQuery_SelectRandomElement(u16 rngValue);

void RogueCustomQuery_Begin();
void RogueCustomQuery_End();

// Mon Query
void RogueMonQuery_Begin();
void RogueMonQuery_End();
void RogueMonQuery_Reset(u8 func);
void RogueMonQuery_IsSpeciesActive();
void RogueMonQuery_IsBaseSpeciesInCurrentDex(u8 func);
void RogueMonQuery_IsSeenInPokedex(u8 func);
void RogueMonQuery_TransformIntoEggSpecies();
void RogueMonQuery_TransformIntoEvos(u8 levelLimit, bool8 includeItemEvos, bool8 keepSourceSpecies);
void RogueMonQuery_IsOfType(u8 func, u32 typeFlags);
void RogueMonQuery_IsOfGeneration(u8 func, u32 generationFlags);
void RogueMonQuery_EvosContainType(u8 func, u32 typeFlags);
void RogueMonQuery_ContainsPresetFlags(u8 func, u32 flags);
void RogueMonQuery_IsLegendary(u8 func);
void RogueMonQuery_IsLegendaryWithPresetFlags(u8 func, u32 flags);
void RogueMonQuery_IsBoxLegendary(u8 func);
void RogueMonQuery_IsRoamerLegendary(u8 func);
void RogueMonQuery_AnyActiveEvos(u8 func);
void RogueMonQuery_CustomFilter(QueryFilterCallback filterFunc, void* usrData);

bool8 Query_IsSpeciesEnabled(u16 species);

// Item Query
void RogueItemQuery_Begin();
void RogueItemQuery_End();
void RogueItemQuery_Reset(u8 func);
void RogueItemQuery_IsItemActive();
void RogueItemQuery_IsStoredInPocket(u8 func, u8 pocket);
void RogueItemQuery_IsEvolutionItem(u8 func);
void RogueItemQuery_IsGeneralShopItem(u8 func);
void RogueItemQuery_IsHeldItem(u8 func);
void RogueItemQuery_InPriceRange(u8 func, u16 minPrice, u16 maxPrice);

// Traine Query
void RogueTrainerQuery_Begin();
void RogueTrainerQuery_End();
void RogueTrainerQuery_Reset(u8 func);
void RogueTrainerQuery_ContainsClassFlag(u8 func, u32 trainerFlags);
void RogueTrainerQuery_ContainsTrainerFlag(u8 func, u32 trainerFlags);
void RogueTrainerQuery_IsOfTypeGroup(u8 func, u16 typeGroup);

// Adventure Path Query
void RoguePathsQuery_Begin();
void RoguePathsQuery_Reset(u8 func);
void RoguePathsQuery_End();
void RoguePathsQuery_IsOfType(u8 func, u8 roomType);

// Move Query
void RogueMoveQuery_Begin();
void RogueMoveQuery_Reset(u8 func);
void RogueMoveQuery_IsTM(u8 func);
void RogueMoveQuery_IsHM(u8 func);
void RogueMoveQuery_End();

// Weight selection
bool8 RogueWeightQuery_IsOverSafeCapacity();
void RogueWeightQuery_Begin();
void RogueWeightQuery_End();
bool8 RogueWeightQuery_HasAnyWeights();
bool8 RogueWeightQuery_HasMultipleWeights();
void RogueWeightQuery_CalculateWeights(WeightCallback callback, void* data);
void RogueWeightQuery_FillWeights(u8 weight);
u16 RogueWeightQuery_SelectRandomFromWeights(u16 randValue);
u16 RogueWeightQuery_SelectRandomFromWeightsWithUpdate(u16 randValue, u8 updatedWeight);
//u16 RogueWeightQuery_SelectFromWeights();

// List selection
void RogueListQuery_Begin();
void RogueListQuery_End();
u16 const* RogueListQuery_CollapseItems(u8 sortMode, bool8 flipSort);

// Debug
void RogueDebugQuery_FillPC(bool8 append);
void RogueDebugQuery_FillBag();

#endif