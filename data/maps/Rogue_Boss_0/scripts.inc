Rogue_Boss_0_MapScripts::
	.byte 0


Rogue_Battle_Boss0::
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToBattleField
	waitmovement 0
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, CameraToBattleField
	waitmovement 0
	msgbox Rogue_Battle_Boss0_Text_0
	setweather WEATHER_SANDSTORM
	doweather
	applymovement 5, BossToFight
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToFight
	waitmovement 0
	trainerbattle_single TRAINER_ROXANNE_1, Rogue_Battle_Boss0_Text_1, Rogue_Battle_Boss0_Text_2, Rogue_BattleDefeated_Boss0
	return


Rogue_BattleDefeated_Boss0::
	message Rogue_BattleDefeated_Boss0_Text_0
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	setflag FLAG_BADGE01_GET
	setflag FLAG_ROGUE_DEFEATED_ROXANNE
	msgbox Rogue_BattleDefeated_Boss0_Text_1
	waitmessage
	special RemoveCameraObject
	warp MAP_ROGUE_HUB_TRANSITION, 0
	return


PlayerToBattleField:
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

CameraToBattleField:
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

PlayerToFight:
	walk_right
	face_right
	step_end

BossToFight:
	walk_left
	step_end

Rogue_Battle_Boss0_Text_0:
	.string "ROXANNE: A new challenger!$"

Rogue_Battle_Boss0_Text_1:
	.string "Don't hold back now!$"

Rogue_Battle_Boss0_Text_2:
	.string "Well fought trainer!$"

Rogue_BattleDefeated_Boss0_Text_0:
	.string "{PLAYER} received the STONE BADGE!$"

Rogue_BattleDefeated_Boss0_Text_1:
	.string "Good luck in your future battles!$"
