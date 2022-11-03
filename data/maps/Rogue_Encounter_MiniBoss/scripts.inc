Rogue_Encounter_MiniBoss_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Rogue_MiniBoss_OnLoad
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, Rogue_Encounter_MiniBoss_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE
	.byte 0

Rogue_Encounter_MiniBoss_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE:
	map_script_2 VAR_TEMP_1, 0, Rogue_Common_TurnPlayerEast
	.2byte 0


Rogue_MiniBoss_OnLoad::
	call Rogue_SetDesiredWeather
	return


Rogue_Battle_MiniBoss::
	lock
	applymovement OBJ_EVENT_ID_PLAYER, MiniBoss_PlayerToBattleField
	waitmovement 0
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, MiniBoss_CameraToBattleField
	waitmovement 0
	call Rogue_MiniBoss_IntroSpeech
	applymovement 1, MiniBoss_BossToFight
	applymovement OBJ_EVENT_ID_PLAYER, MiniBoss_PlayerToFight
	waitmovement 0
	playbgm MUS_ENCOUNTER_INTENSE, FALSE
	call Rogue_MiniBoss_KickOffBattle
	call Rogue_BattleDefeated_MiniBoss
	return


Rogue_BattleDefeated_MiniBoss::
	call Rogue_ClearWeather
	bufferitemname STR_VAR_1, VAR_ROGUE_ITEM0
	bufferitemname STR_VAR_2, VAR_ROGUE_ITEM1
	msgbox Rogue_BattleDefeated_MiniBoss_Text_0
	message Rogue_BattleDefeated_MiniBoss_Text_1
	waitmessage
	multichoice 17, 6, MULTI_RIGHTLEFT, TRUE
	switch VAR_RESULT
	case 0, Rogue_BattleDefeated_MiniBoss_3
	case 1, Rogue_BattleDefeated_MiniBoss_4
Rogue_BattleDefeated_MiniBoss_1:
	special HealPlayerParty
	special RemoveCameraObject
	warp MAP_ROGUE_HUB_TRANSITION, 0
	waitstate
	release
	return

Rogue_BattleDefeated_MiniBoss_3:
	setvar VAR_0x8000, 0
	copyvar VAR_0x8001, VAR_ROGUE_ITEM0
	call Rogue_GiveItem_Internal
	goto Rogue_BattleDefeated_MiniBoss_1

Rogue_BattleDefeated_MiniBoss_4:
	setvar VAR_0x8000, 1
	copyvar VAR_0x8001, VAR_ROGUE_ITEM1
	call Rogue_GiveItem_Internal
	goto Rogue_BattleDefeated_MiniBoss_1


MiniBoss_PlayerToBattleField:
	walk_right
	step_end

MiniBoss_CameraToBattleField:
	walk_right
	walk_right
	walk_right
	step_end

MiniBoss_PlayerToFight:
	walk_right
	face_right
	step_end

MiniBoss_BossToFight:
	walk_left
	step_end

Rogue_MiniBoss_IntroSpeech::
	return


Rogue_MiniBoss_KickOffBattle::
	buffertrainername STR_VAR_1, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA
	switch VAR_ROGUE_SPECIAL_ENCOUNTER_DATA
	case TRAINER_ROGUE_MINI_BOSS_MAXIE, Rogue_MiniBoss_KickOffBattle_2
	case TRAINER_ROGUE_MINI_BOSS_ARCHIE, Rogue_MiniBoss_KickOffBattle_2
	case TRAINER_ROGUE_MINI_BOSS_WALLY, Rogue_MiniBoss_KickOffBattle_3
	case TRAINER_ROGUE_MINI_BOSS_MIRROR, Rogue_MiniBoss_KickOffBattle_4
	case TRAINER_ROGUE_MINI_BOSS_RIVAL, Rogue_MiniBoss_KickOffBattle_5
	msgbox Rogue_MiniBoss_KickOffBattle_Text_8
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, 0, Rogue_MiniBoss_KickOffBattle_Text_9
	return

Rogue_MiniBoss_KickOffBattle_2:
	msgbox Rogue_MiniBoss_KickOffBattle_Text_0
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, 0, Rogue_MiniBoss_KickOffBattle_Text_1
	return

Rogue_MiniBoss_KickOffBattle_3:
	msgbox Rogue_MiniBoss_KickOffBattle_Text_2
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, 0, Rogue_MiniBoss_KickOffBattle_Text_3
	return

Rogue_MiniBoss_KickOffBattle_4:
	msgbox Rogue_MiniBoss_KickOffBattle_Text_4
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, 0, Rogue_MiniBoss_KickOffBattle_Text_5
	return

Rogue_MiniBoss_KickOffBattle_5:
	msgbox Rogue_MiniBoss_KickOffBattle_Text_6
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, 0, Rogue_MiniBoss_KickOffBattle_Text_7
	return


Rogue_BattleDefeated_MiniBoss_Text_0:
	.string "I've got {STR_VAR_1} in my LEFT hand\n"
	.string "and {STR_VAR_2} in my right.$"

Rogue_BattleDefeated_MiniBoss_Text_1:
	.string "Which do you want to take?\n"
	.string "L: {STR_VAR_1}, R: {STR_VAR_2}$"

Rogue_MiniBoss_KickOffBattle_Text_0:
	.string "{STR_VAR_1}: Don't think I'll let you\n"
	.string "through lightly!$"

Rogue_MiniBoss_KickOffBattle_Text_1:
	.string "Well that was unexpected...$"

Rogue_MiniBoss_KickOffBattle_Text_2:
	.string "{STR_VAR_1}: Oh hey!\n"
	.string "Will you battle with me!$"

Rogue_MiniBoss_KickOffBattle_Text_3:
	.string "What a fun battle!$"

Rogue_MiniBoss_KickOffBattle_Text_4:
	.string "{PLAYER}: ... ... ...$"

Rogue_MiniBoss_KickOffBattle_Text_5:
	.string "...!$"

Rogue_MiniBoss_KickOffBattle_Text_6:
	.string "{STR_VAR_1}: I've think I've crafted\n"
	.string "the best team! Let me show you!$"

Rogue_MiniBoss_KickOffBattle_Text_7:
	.string "Wow! I didn't expect that!$"

Rogue_MiniBoss_KickOffBattle_Text_8:
	.string "{STR_VAR_1}: Show me everything you\n"
	.string "got!$"

Rogue_MiniBoss_KickOffBattle_Text_9:
	.string "Well colour me impressed!$"
