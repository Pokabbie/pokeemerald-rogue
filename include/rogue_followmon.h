#ifndef ROGUE_FOLLOWMON_H
#define ROGUE_FOLLOWMON_H

const struct ObjectEventGraphicsInfo *GetFollowMonObjectEventInfo(u16 graphicsId);
void SetupFollowParterMonObjectEvent();
void ResetFollowParterMonObjectEvent();

void UpdateWildEncounterChain(u16 species);
u16 GetWildChainSpecies();
u8 GetWildChainCount();

void FollowMon_SetGraphics(u16 id, u16 species, bool8 isShiny);
u16 FollowMon_GetGraphics(u16 id);

bool8 FollowMon_IsPartnerMonActive();
u16 FollowMon_GetPartnerFollowSpecies(bool8 includeShinyOffset);
u16 FollowMon_GetMonGraphics(struct Pokemon* mon);

bool8 FollowMon_IsMonObject(struct ObjectEvent* object, bool8 ignorePartnerMon);
bool8 FollowMon_ShouldAlwaysAnimation(struct ObjectEvent *objectEvent);
bool8 FollowMon_ShouldAnimationGrass(struct ObjectEvent *objectEvent);

bool8 FollowMon_IsCollisionExempt(struct ObjectEvent* obstacle, struct ObjectEvent* collider);
bool8 FollowMon_ProcessMonInteraction();
void FollowMon_GetSpeciesFromLastInteracted(u16* species, bool8* isShiny, u8* spawnSlot);

void FollowMon_OverworldCB();
void FollowMon_OnWarp();
void FollowMon_OnObjectEventSpawned(struct ObjectEvent *objectEvent);
void FollowMon_OnObjectEventRemoved(struct ObjectEvent *objectEvent);

bool8 IsSafeToSpawnObjectEvents();

#endif