Rogue_Boss_5_MapScripts::
	.byte 0


Rogue_Battle_Boss5::
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToBattleFieldBoss5
	waitmovement 0
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, CameraToBattleFieldBoss5
	waitmovement 0
	msgbox Rogue_Battle_Boss5_Text_0
	applymovement 5, BossToFightBoss5
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToFightBoss5
	waitmovement 0
	playbgm MUS_ENCOUNTER_INTENSE, FALSE
	msgbox Rogue_Battle_Boss5_Text_1
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, TRAINER_WINONA_1, 0, Rogue_Battle_Boss5_Text_2
	call Rogue_BattleDefeated_Boss5
	return


Rogue_BattleDefeated_Boss5::
	message Rogue_BattleDefeated_Boss5_Text_0
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	setflag FLAG_BADGE06_GET
	setflag FLAG_ROGUE_DEFEATED_BOSS05
	msgbox Rogue_BattleDefeated_Boss5_Text_1
	waitmessage
	special RemoveCameraObject
	warp MAP_ROGUE_HUB_TRANSITION, 0
	return


PlayerToBattleFieldBoss5:
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
	walk_right
	step_end

CameraToBattleFieldBoss5:
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

PlayerToFightBoss5:
	walk_right
	face_right
	step_end

BossToFightBoss5:
	walk_left
	step_end

Rogue_Battle_Boss5_Text_0:
	.string "WINONA: A new challenger!$"

Rogue_Battle_Boss5_Text_1:
	.string "Don't hold back now!$"

Rogue_Battle_Boss5_Text_2:
	.string "Well fought trainer!$"

Rogue_BattleDefeated_Boss5_Text_0:
	.string "{PLAYER} received a GYM BADGE!$"

Rogue_BattleDefeated_Boss5_Text_1:
	.string "Good luck in your future battles!$"
