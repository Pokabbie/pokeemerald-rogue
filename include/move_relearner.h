#ifndef GUARD_MOVE_RELEARNER_H
#define GUARD_MOVE_RELEARNER_H

void TeachMoveSetContextRelearnMove(void);
void TeachMoveSetContextEggMove(void);
void TeachMoveSetContextTutorMove(void);
void TeachMoveFromContext(void);
void TeachMoveRelearnerMove(void);
bool8 CanLearnMovesInCurrentContext(struct Pokemon* mon);
void MoveRelearnerShowHideHearts(s32);

#endif //GUARD_MOVE_RELEARNER_H
