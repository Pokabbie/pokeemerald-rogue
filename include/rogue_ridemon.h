#ifndef ROGUE_RIDEMON_H
#define ROGUE_RIDEMON_H

void Rogue_RideMonInit();

void Rogue_GetOnOffRideMon(bool8 forWarp);
void Rogue_CreateDestroyRideMonSprites();
void Rogue_UpdateRideMonSprites();

u8 Rogue_GetRideMonSprite(struct ObjectEvent* objectEvent);

bool8 Rogue_CanRideMonInvJumpLedge();
bool8 Rogue_CanRideMonSwim();
bool8 Rogue_CanRideMonFly();

bool8 Rogue_IsRideMonSwimming();
bool8 Rogue_IsRideMonFlying();

void MovePlayerOnRideMon(u8 direction, u16 newKeys, u16 heldKeys);
s16 RideMonGetPlayerSpeed();

#endif