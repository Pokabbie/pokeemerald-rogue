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

Rogue_MiniBoss_SetWeather::
	switch VAR_ROGUE_SPECIAL_ENCOUNTER_DATA
	case OBJ_EVENT_GFX_MAXIE, Rogue_MiniBoss_SetWeather_2
	case OBJ_EVENT_GFX_ARCHIE, Rogue_MiniBoss_SetWeather_3
	case OBJ_EVENT_GFX_WALLY, Rogue_MiniBoss_SetWeather_4
	case OBJ_EVENT_GFX_BRENDAN_NORMAL, Rogue_MiniBoss_SetWeather_5
	return

Rogue_MiniBoss_SetWeather_2:
	setweather WEATHER_DROUGHT
	doweather
	return

Rogue_MiniBoss_SetWeather_3:
	setweather WEATHER_RAIN
	doweather
	return

Rogue_MiniBoss_SetWeather_4:
	setweather WEATHER_FOG_HORIZONTAL
	doweather
	return

Rogue_MiniBoss_SetWeather_5:
	setweather WEATHER_UNDERWATER_BUBBLES
	doweather
	return


Rogue_MiniBoss_IntroSpeech::
	return


Rogue_MiniBoss_KickOffBattle::
	switch VAR_ROGUE_SPECIAL_ENCOUNTER_DATA
	case OBJ_EVENT_GFX_MAXIE, Rogue_MiniBoss_KickOffBattle_2
	case OBJ_EVENT_GFX_ARCHIE, Rogue_MiniBoss_KickOffBattle_3
	case OBJ_EVENT_GFX_WALLY, Rogue_MiniBoss_KickOffBattle_4
	case OBJ_EVENT_GFX_BRENDAN_NORMAL, Rogue_MiniBoss_KickOffBattle_5
	msgbox Rogue_MiniBoss_KickOffBattle_Text_7
	return

Rogue_MiniBoss_KickOffBattle_2:
	msgbox Rogue_MiniBoss_KickOffBattle_Text_0
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, TRAINER_MAXIE_MAGMA_HIDEOUT, 0, Rogue_MiniBoss_KickOffBattle_Text_1
	return

Rogue_MiniBoss_KickOffBattle_3:
	msgbox Rogue_MiniBoss_KickOffBattle_Text_2
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, TRAINER_ARCHIE, 0, Rogue_MiniBoss_KickOffBattle_Text_1
	return

Rogue_MiniBoss_KickOffBattle_4:
	msgbox Rogue_MiniBoss_KickOffBattle_Text_3
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, TRAINER_WALLY_VR_1, 0, Rogue_MiniBoss_KickOffBattle_Text_4
	return

Rogue_MiniBoss_KickOffBattle_5:
	msgbox Rogue_MiniBoss_KickOffBattle_Text_5
	checkplayergender
	switch VAR_RESULT
	case 0, Rogue_MiniBoss_KickOffBattle_8
	case 1, Rogue_MiniBoss_KickOffBattle_9
	case 2, Rogue_MiniBoss_KickOffBattle_10
	case 3, Rogue_MiniBoss_KickOffBattle_11
	return

Rogue_MiniBoss_KickOffBattle_8:
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, TRAINER_BRENDAN_PLACEHOLDER, 0, Rogue_MiniBoss_KickOffBattle_Text_6
	return

Rogue_MiniBoss_KickOffBattle_9:
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, TRAINER_MAY_PLACEHOLDER, 0, Rogue_MiniBoss_KickOffBattle_Text_6
	return

Rogue_MiniBoss_KickOffBattle_10:
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, TRAINER_RED, 0, Rogue_MiniBoss_KickOffBattle_Text_6
	return

Rogue_MiniBoss_KickOffBattle_11:
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, TRAINER_LEAF, 0, Rogue_MiniBoss_KickOffBattle_Text_6
	return


Rogue_BattleDefeated_MiniBoss_Text_0:
	.string "I've got {STR_VAR_1} in my LEFT hand\n"
	.string "and {STR_VAR_2} in my right.$"

Rogue_BattleDefeated_MiniBoss_Text_1:
	.string "Which do you want to take?\n"
	.string "L: {STR_VAR_1}, R: {STR_VAR_2}$"

Rogue_MiniBoss_KickOffBattle_Text_0:
	.string "MAXIE: Don't think I'll let you through\n"
	.string "lightly!$"

Rogue_MiniBoss_KickOffBattle_Text_1:
	.string "Well that was unexpected...$"

Rogue_MiniBoss_KickOffBattle_Text_2:
	.string "ARCHIE: Don't think I'll let you through\n"
	.string "lightly!$"

Rogue_MiniBoss_KickOffBattle_Text_3:
	.string "WALLY: Oh hey!\n"
	.string "Will you battle with me!$"

Rogue_MiniBoss_KickOffBattle_Text_4:
	.string "What a fun fight!$"

Rogue_MiniBoss_KickOffBattle_Text_5:
	.string "{PLAYER}: ... ... ...$"

Rogue_MiniBoss_KickOffBattle_Text_6:
	.string "...!$"

Rogue_MiniBoss_KickOffBattle_Text_7:
	.string "UNEXPECTED TRAINER INDEX$"
