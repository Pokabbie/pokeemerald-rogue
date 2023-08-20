#ifndef ROGUE_SAVE_H
#define ROGUE_SAVE_H

extern struct RogueSaveBlock *gRogueSaveBlock;

enum
{
    SAVE_VER_ID_1_X,
    SAVE_VER_ID_2_0
};

void RogueSave_UpdatePointers();

void RogueSave_ClearData();

void RogueSave_FormatForWriting();
void RogueSave_FormatForReading();

u16 RogueSave_GetVersionIdFor(u16 saveVersion);
u16 RogueSave_GetVersionId();

void RogueSave_OnSaveLoaded();

void RogueSave_SaveHubStates();
void RogueSave_LoadHubStates();

#endif //ROGUE_SAVE_H
