#ifndef ROGUE_FOLLOWMON_H
#define ROGUE_FOLLOWMON_H

const struct ObjectEventGraphicsInfo *GetFollowMonObjectEventInfo(u16 graphicsId);
void SetupFollowParterMonObjectEvent();
void ResetFollowParterMonObjectEvent();

bool8 FollowMon_IsCollisionExempt(struct ObjectEvent* obstacle, struct ObjectEvent* collider);
bool8 FollowMon_ProcessMonInteraction();

#endif