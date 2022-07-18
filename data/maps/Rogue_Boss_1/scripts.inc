Rogue_Boss_1_MapScripts::
	.byte 0


Rogue_Battle_Boss1::
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToBattleFieldBoss1
	waitmovement 0
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, CameraToBattleFieldBoss1
	waitmovement 0
	msgbox Rogue_Battle_Boss1_Text_0
	applymovement 5, BossToFightBoss1
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToFightBoss1
	waitmovement 0
	trainerbattle_single TRAINER_BRAWLY_1, Rogue_Battle_Boss1_Text_1, Rogue_Battle_Boss1_Text_2, Rogue_BattleDefeated_Boss1
	return


Rogue_BattleDefeated_Boss1::
	message Rogue_BattleDefeated_Boss1_Text_0
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	setflag FLAG_BADGE02_GET
	setflag FLAG_ROGUE_DEFEATED_BOSS01
	msgbox Rogue_BattleDefeated_Boss1_Text_1
	waitmessage
	special RemoveCameraObject
	warp MAP_ROGUE_HUB_TRANSITION, 0
	return


PlayerToBattleFieldBoss1:
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

CameraToBattleFieldBoss1:
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

PlayerToFightBoss1:
	walk_right
	face_right
	step_end

BossToFightBoss1:
	walk_left
	step_end

Rogue_Battle_Boss1_Text_0:
	.string "BRAWLY: A new challenger!$"

Rogue_Battle_Boss1_Text_1:
	.string "Don't hold back now!$"

Rogue_Battle_Boss1_Text_2:
	.string "Well fought trainer!$"

Rogue_BattleDefeated_Boss1_Text_0:
	.string "{PLAYER} received the ?? BADGE!$"

Rogue_BattleDefeated_Boss1_Text_1:
	.string "Good luck in your future battles!$"
