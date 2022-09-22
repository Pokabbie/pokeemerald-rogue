Rogue_Boss_8_MapScripts::
	.byte 0


Rogue_Battle_Boss8::
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToBattleFieldBoss8
	waitmovement 0
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, CameraToBattleFieldBoss8
	waitmovement 0
	buffertrainername STR_VAR_1, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA
	msgbox Rogue_Battle_Boss8_Text_0
	call Rogue_SetDesiredWeather
	applymovement 1, BossToFightBoss8
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToFightBoss8
	waitmovement 0
	playbgm MUS_ENCOUNTER_ELITE_FOUR, FALSE
	msgbox Rogue_Battle_Boss8_Text_1
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, 0, Rogue_Battle_Boss8_Text_2
	call Rogue_BattleDefeated_Boss8
	return


Rogue_BattleDefeated_Boss8::
	call Rogue_ClearWeather
	message Rogue_BattleDefeated_Boss8_Text_0
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	msgbox Rogue_BattleDefeated_Boss8_Text_1
	waitmessage
	special HealPlayerParty
	special RemoveCameraObject
	warp MAP_ROGUE_HUB_TRANSITION, 0
	return


PlayerToBattleFieldBoss8:
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

CameraToBattleFieldBoss8:
	walk_up
	walk_up
	walk_up
	step_end

PlayerToFightBoss8:
	walk_up
	step_end

BossToFightBoss8:
	walk_down
	step_end

Rogue_Battle_Boss8_Text_0:
	.string "{STR_VAR_1}: A new challenger!$"

Rogue_Battle_Boss8_Text_1:
	.string "Don't hold back now!$"

Rogue_Battle_Boss8_Text_2:
	.string "Well fought trainer!$"

Rogue_BattleDefeated_Boss8_Text_0:
	.string "{PLAYER} received a ELITE BADGE!$"

Rogue_BattleDefeated_Boss8_Text_1:
	.string "Good luck in your future battles!$"
