Rogue_Boss_12_MapScripts::
	.byte 0


Rogue_Battle_Boss12::
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToBattleFieldBoss12
	waitmovement 0
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, CameraToBattleFieldBoss12
	waitmovement 0
	msgbox Rogue_Battle_Boss12_Text_0
	setweather WEATHER_DOWNPOUR
	doweather
	applymovement 1, BossToFightBoss12
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToFightBoss12
	waitmovement 0
	trainerbattle_single TRAINER_WALLACE, Rogue_Battle_Boss12_Text_1, Rogue_Battle_Boss12_Text_2, Rogue_BattleDefeated_Boss12
	return


Rogue_BattleDefeated_Boss12::
	message Rogue_BattleDefeated_Boss12_Text_0
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	setflag FLAG_ROGUE_DEFEATED_BOSS12
	msgbox Rogue_BattleDefeated_Boss12_Text_1
	waitmessage
	special RemoveCameraObject
	warp MAP_ROGUE_HUB_TRANSITION, 0
	return


PlayerToBattleFieldBoss12:
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

CameraToBattleFieldBoss12:
	walk_up
	walk_up
	walk_up
	step_end

PlayerToFightBoss12:
	walk_up
	step_end

BossToFightBoss12:
	walk_down
	step_end

Rogue_Battle_Boss12_Text_0:
	.string "WALLACE: A new challenger!$"

Rogue_Battle_Boss12_Text_1:
	.string "Don't hold back now!$"

Rogue_Battle_Boss12_Text_2:
	.string "Well fought trainer!$"

Rogue_BattleDefeated_Boss12_Text_0:
	.string "{PLAYER} received the ?? BADGE!$"

Rogue_BattleDefeated_Boss12_Text_1:
	.string "Good luck in your future battles!$"
