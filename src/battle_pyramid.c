#include "global.h"
#include "battle_pyramid.h"
#include "battle_pyramid_bag.h"
#include "event_data.h"
#include "battle.h"
#include "battle_setup.h"
#include "battle_tower.h"
#include "save.h"
#include "strings.h"
#include "fieldmap.h"
#include "party_menu.h"
#include "palette.h"
#include "field_screen_effect.h"
#include "field_message_box.h"
#include "random.h"
#include "item.h"
#include "util.h"
#include "sound.h"
#include "task.h"
#include "start_menu.h"
#include "string_util.h"
#include "trainer_see.h"
#include "main.h"
#include "load_save.h"
#include "script.h"
#include "malloc.h"
#include "overworld.h"
#include "event_scripts.h"
#include "graphics.h"
#include "constants/battle_frontier.h"
#include "constants/battle_pyramid.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/frontier_util.h"
#include "constants/items.h"
#include "constants/layouts.h"
#include "constants/metatile_labels.h"
#include "constants/moves.h"
#include "constants/trainers.h"

#include "rogue_baked.h"

// code
void CallBattlePyramidFunction(void)
{
}

u16 LocalIdToPyramidTrainerId(u8 localId)
{
    return 0;
}

bool8 GetBattlePyramidTrainerFlag(u8 eventId)
{
    return FALSE;
}

void MarkApproachingPyramidTrainersAsBattled(void)
{
}

void GenerateBattlePyramidWildMon(void)
{
}

u8 GetPyramidRunMultiplier(void)
{
    return 0;
}

u8 InBattlePyramid(void)
{
    return FALSE;
}

bool8 InBattlePyramid_(void)
{
    return FALSE;
}

void PausePyramidChallenge(void)
{
}

void SoftResetInBattlePyramid(void)
{
}

void CopyPyramidTrainerSpeechBefore(u16 trainerId)
{
}

void CopyPyramidTrainerWinSpeech(u16 trainerId)
{
}

void CopyPyramidTrainerLoseSpeech(u16 trainerId)
{
}

u8 GetTrainerEncounterMusicIdInBattlePyramid(u16 trainerId)
{
    return 0;
}

void GenerateBattlePyramidFloorLayout(u16 *backupMapData, bool8 setPlayerPosition)
{
    
}

void LoadBattlePyramidObjectEventTemplates(void)
{
}

void LoadBattlePyramidFloorObjectEventScripts(void)
{
}

u8 GetNumBattlePyramidObjectEvents(void)
{
    return 0;
}