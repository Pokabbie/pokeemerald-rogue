Rogue_Boss_4_MapScripts::
	.byte 0


Rogue_Battle_Boss4::
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToBattleFieldBoss4
	waitmovement 0
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, CameraToBattleFieldBoss4
	waitmovement 0
	msgbox Rogue_Battle_Boss4_Text_0
	applymovement 5, BossToFightBoss4
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToFightBoss4
	waitmovement 0
	playbgm MUS_ENCOUNTER_INTENSE, FALSE
	msgbox Rogue_Battle_Boss4_Text_1
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, TRAINER_NORMAN_1, 0, Rogue_Battle_Boss4_Text_2
	call Rogue_BattleDefeated_Boss4
	return


Rogue_BattleDefeated_Boss4::
	setweather WEATHER_NONE
	doweather
	message Rogue_BattleDefeated_Boss4_Text_0
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	setflag FLAG_BADGE05_GET
	setflag FLAG_ROGUE_DEFEATED_BOSS04
	msgbox Rogue_BattleDefeated_Boss4_Text_1
	waitmessage
	special HealPlayerParty
	special RemoveCameraObject
	warp MAP_ROGUE_HUB_TRANSITION, 0
	return


PlayerToBattleFieldBoss4:
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

CameraToBattleFieldBoss4:
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

PlayerToFightBoss4:
	walk_right
	face_right
	step_end

BossToFightBoss4:
	walk_left
	step_end

Rogue_Battle_Boss4_Text_0:
	.string "NORMAN: A new challenger!$"

Rogue_Battle_Boss4_Text_1:
	.string "Don't hold back now!$"

Rogue_Battle_Boss4_Text_2:
	.string "Well fought trainer!$"

Rogue_BattleDefeated_Boss4_Text_0:
	.string "{PLAYER} received a GYM BADGE!$"

Rogue_BattleDefeated_Boss4_Text_1:
	.string "Good luck in your future battles!$"
