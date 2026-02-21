#ifndef GUARD_ITEM_USE_H
#define GUARD_ITEM_USE_H

void ItemUseOutOfBattle_Mail(u8);
void ItemUseOutOfBattle_Bike(u8);
void ItemUseOutOfBattle_RideMon(u8);
void ItemUseOutOfBattle_Rod(u8);
void ItemUseOutOfBattle_HealingFlask(u8);
void ItemUseOutOfBattle_Itemfinder(u8);
void ItemUseOutOfBattle_PokeblockCase(u8);
void ItemUseOutOfBattle_QuestLog(u8);
void ItemUseOutOfBattle_CGear(u8);
void ItemUseOutOfBattle_WorldMap(u8);
void ItemUseOutOfBattle_DayCarePhone(u8);
void ItemUseOutOfBattle_GoldenSeed(u8);
void ItemUseOutOfBattle_CoinCase(u8);
void ItemUseOutOfBattle_PowderJar(u8);
void ItemUseOutOfBattle_SSTicket(u8);
void ItemUseOutOfBattle_WailmerPail(u8);
void ItemUseOutOfBattle_Medicine(u8);
void ItemUseOutOfBattle_AbilityCapsule(u8);
void ItemUseOutOfBattle_AbilityPatch(u8);
void ItemUseOutOfBattle_NatureMint(u8);
void ItemUseOutOfBattle_TeraShard(u8);
void ItemUseOutOfBattle_MaxMushroom(u8);
void ItemUseOutOfBattle_ResetEVs(u8);
void ItemUseOutOfBattle_ReduceEV(u8);
void ItemUseOutOfBattle_SacredAsh(u8);
void ItemUseOutOfBattle_PPRecovery(u8);
void ItemUseOutOfBattle_PPUp(u8);
void ItemUseOutOfBattle_RareCandy(u8);
void ItemUseOutOfBattle_TMHM(u8);
void ItemUseOutOfBattle_Repel(u8);
void ItemUseOutOfBattle_Lure(u8);
void ItemUseOutOfBattle_Pokeblock(u8);
void ItemUseOutOfBattle_EscapeRope(u8);
void ItemUseOutOfBattle_BlackWhiteFlute(u8);
void ItemUseOutOfBattle_EvolutionStone(u8);
void ItemUseOutOfBattle_Berry(u8);
void ItemUseOutOfBattle_EnigmaBerry(u8);
void ItemUseOutOfBattle_FormChange(u8);
void ItemUseOutOfBattle_FormChange_ConsumedOnUse(u8);
void ItemUseOutOfBattle_RotomCatalog(u8);
void ItemUseOutOfBattle_ZygardeCube(u8);
void ItemUseOutOfBattle_Meteorite(u8);
void ItemUseOutOfBattle_Fusion(u8);
void ItemUseOutOfBattle_Honey(u8);
void ItemUseOutOfBattle_CannotUse(u8);
void ItemUseOutOfBattle_ExpShare(u8);
void ItemUseInBattle_BagMenu(u8 taskId);
void ItemUseInBattle_PartyMenu(u8 taskId);
void ItemUseInBattle_PartyMenuChooseMove(u8 taskId);
void Task_UseDigEscapeRopeOnField(u8 taskId);
u8 CanUseDigOrEscapeRopeOnCurMap(void);
u8 CheckIfItemIsTMHMOrEvolutionStone(u16 itemId);
void FieldUseFunc_VsSeeker(u8 taskId);
void Task_ItemUse_CloseMessageBoxAndReturnToField_VsSeeker(u8 taskId);

enum {
    BALL_THROW_UNABLE_TWO_MONS,
    BALL_THROW_UNABLE_NO_ROOM,
    BALL_THROW_UNABLE_SEMI_INVULNERABLE,
    BALL_THROW_ABLE,
    BALL_THROW_UNABLE_DISABLED_FLAG,
};

bool32 CanThrowBall(void);

#endif // GUARD_ITEM_USE_H
