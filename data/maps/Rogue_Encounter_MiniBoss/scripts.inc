Rogue_Encounter_MiniBoss_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Rogue_MiniBoss_OnLoad
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, Rogue_Encounter_MiniBoss_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE
	.byte 0

Rogue_Encounter_MiniBoss_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE:
	map_script_2 VAR_TEMP_1, 0, Rogue_Common_TurnPlayerEast
	.2byte 0


Rogue_MiniBoss_OnLoad::
	goto_if_set FLAG_ROGUE_WEATHER_ACTIVE, Rogue_MiniBoss_OnLoad_1
	return

Rogue_MiniBoss_OnLoad_1:
	call Rogue_MiniBoss_SetWeather
	return


Rogue_Battle_MiniBoss::
	lockall
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
	setweather WEATHER_NONE
	doweather
	msgbox Rogue_BattleDefeated_MiniBoss_Text_0
	special HealPlayerParty
	special RemoveCameraObject
	warp MAP_ROGUE_HUB_TRANSITION, 0
	return


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
	case 0, Rogue_MiniBoss_SetWeather_2
	case 1, Rogue_MiniBoss_SetWeather_3
	return

Rogue_MiniBoss_SetWeather_2:
	setweather WEATHER_DROUGHT
	doweather
	return

Rogue_MiniBoss_SetWeather_3:
	setweather WEATHER_RAIN
	doweather
	return


Rogue_MiniBoss_IntroSpeech::
	return


Rogue_MiniBoss_KickOffBattle::
	switch VAR_ROGUE_SPECIAL_ENCOUNTER_DATA
	case 0, Rogue_MiniBoss_KickOffBattle_2
	case 1, Rogue_MiniBoss_KickOffBattle_3
	msgbox Rogue_MiniBoss_KickOffBattle_Text_3
	return

Rogue_MiniBoss_KickOffBattle_2:
	msgbox Rogue_MiniBoss_KickOffBattle_Text_0
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, TRAINER_MAXIE_MAGMA_HIDEOUT, 0, Rogue_MiniBoss_KickOffBattle_Text_1
	return

Rogue_MiniBoss_KickOffBattle_3:
	msgbox Rogue_MiniBoss_KickOffBattle_Text_2
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, TRAINER_ARCHIE, 0, Rogue_MiniBoss_KickOffBattle_Text_1
	return


Rogue_BattleDefeated_MiniBoss_Text_0:
	.string "TODO item choice!$"

Rogue_MiniBoss_KickOffBattle_Text_0:
	.string "MAXIE: Don't think I'll let you through\n"
	.string "lightly!$"

Rogue_MiniBoss_KickOffBattle_Text_1:
	.string "Well that was unexpected...$"

Rogue_MiniBoss_KickOffBattle_Text_2:
	.string "ARCHIE: Don't think I'll let you through\n"
	.string "lightly!$"

Rogue_MiniBoss_KickOffBattle_Text_3:
	.string "UNEXPECTED TRAINER INDEX$"
