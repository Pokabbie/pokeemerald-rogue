#ifndef ROGUE_RIDEMON_H
#define ROGUE_RIDEMON_H

void Rogue_RideMonInit();
struct RogueRideMonState* Rogue_GetPlayerRideMonStatePtr();

void Rogue_GetOnOffRideMon(u8 whistleType, bool8 forWarp);
bool8 Rogue_HandleRideMonInput();
void Rogue_UpdateRideMons();
void Rogue_HandleRideMonMovementIfNeeded(u8 objectEventId);
bool8 Rogue_IsActiveRideMonObject(u8 objectEventId);
void Rogue_OnResetRideMonSprites();

void Rogue_SetupRideObject(u8 rideObjectId, u8 objectEventId, u16 rideSpecies, bool8 isFlying);
void Rogue_ClearRideObject(u8 rideObjectId);

void Rogue_OnRideMonWarp();

u8 Rogue_GetRideMonSprite(struct ObjectEvent* objectEvent);
u16 Rogue_GetRideMonSpeciesGfx(u8 rideObject);

bool8 Rogue_IsValidRideSpecies(u16 species);
bool8 Rogue_IsValidRideClimbSpecies(u16 species);
bool8 Rogue_IsValidRideSwimSpecies(u16 species);
bool8 Rogue_IsValidRideFlySpecies(u16 species);

u8 Rogue_GetRideSpeciesSpeedStars(u16 species);

bool8 Rogue_CanRideMonInvJumpLedge();
bool8 Rogue_CanRideMonSwim();
bool8 Rogue_CanRideMonFly();

bool8 Rogue_HasUsableSwimOrFlyCharge();
bool8 Rogue_ShouldRunRidemonTrappedScript();

u32 Rogue_GetRemainingFlightCharges();
u32 Rogue_GetMaxFlightCharges();
void Rogue_DecreaseFlightCharges();
void Rogue_RefillFlightCharges(bool8 createPopup);

bool8 Rogue_IsRideActive();
bool8 Rogue_IsRideMonSwimming();
bool8 Rogue_IsRideMonFlying();
bool8 Rogue_RideMonIsCollisionExempt(struct ObjectEvent* obstacle, struct ObjectEvent* collider);

void MovePlayerOnRideMon(u8 direction, u16 newKeys, u16 heldKeys);
s16 RideMonGetPlayerSpeed();

#endif