#ifndef ROGUE_FOLLOWMON_H
#define ROGUE_FOLLOWMON_H

const struct ObjectEventGraphicsInfo *GetFollowMonObjectEventInfo(u16 graphicsId);
void SetupFollowParterMonObjectEvent();
void ResetFollowParterMonObjectEvent();

void FollowMon_SetGraphics(u16 id, u16 species, bool8 isShiny);

bool8 FollowMon_IsPartnerMonActive();
bool8 FollowMon_IsMonObject(struct ObjectEvent* object, bool8 ignorePartnerMon);

bool8 FollowMon_IsCollisionExempt(struct ObjectEvent* obstacle, struct ObjectEvent* collider);
bool8 FollowMon_ProcessMonInteraction();
void FollowMon_GetSpeciesFromLastInteracted(u16* species, bool8* isShiny);

#endif