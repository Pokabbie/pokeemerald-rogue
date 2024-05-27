#ifndef ROGUE_FOLLOWMON_H
#define ROGUE_FOLLOWMON_H

const struct ObjectEventGraphicsInfo *GetFollowMonObjectEventInfo(u16 graphicsId);
void SetupFollowParterMonObjectEvent();
void ResetFollowParterMonObjectEvent();

void UpdateWildEncounterChain(u16 species);
u16 GetWildChainSpecies();
u8 GetWildChainCount();

void FollowMon_SetGraphics(u16 id, u16 species, bool8 isShiny);
void FollowMon_SetGraphicsRaw(u16 id, u16 gfxSpecies);
void FollowMon_SetGraphicsFromMon(u16 id, struct Pokemon* mon);
void FollowMon_SetGraphicsFromParty();
u16 FollowMon_GetGraphics(u16 id);

u16 const* FollowMon_GetGraphicsForPalSlot(u16 palSlot);
bool8 FollowMon_IsLargeGfx(u16 gfxSpecies);

bool8 FollowMon_IsPartnerMonActive();
u16 FollowMon_GetPartnerFollowSpecies(bool8 includeShinyOffset);
void FollowMon_ClearCachedPartnerSpecies();
u16 FollowMon_GetMonGraphics(struct Pokemon* mon);
u16 FollowMon_GetBoxMonGraphics(struct BoxPokemon* mon);

bool8 FollowMon_IsMonObject(struct ObjectEvent* object, bool8 ignorePartnerMon);
bool8 FollowMon_ShouldAlwaysAnimation(struct ObjectEvent *objectEvent);
bool8 FollowMon_ShouldAnimationGrass(struct ObjectEvent *objectEvent);

bool8 FollowMon_IsCollisionExempt(struct ObjectEvent* obstacle, struct ObjectEvent* collider);
bool8 FollowMon_ProcessMonInteraction();
void FollowMon_GetSpeciesFromLastInteracted(u16* species, bool8* isShiny, u8* spawnSlot);
bool8 FollowMon_IsSlotEnabled(u8 slot);

void FollowMon_OverworldCB();
void FollowMon_OnWarp();
void FollowMon_RecountActiveObjects();
void FollowMon_OnObjectEventSpawned(struct ObjectEvent *objectEvent);
void FollowMon_OnObjectEventRemoved(struct ObjectEvent *objectEvent);

bool8 IsSafeToSpawnObjectEvents();

#endif