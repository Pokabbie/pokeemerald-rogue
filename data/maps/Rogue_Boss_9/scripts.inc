Rogue_Boss_9_MapScripts::
	.byte 0


Rogue_Battle_Boss9::
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToBattleFieldBoss9
	waitmovement 0
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, CameraToBattleFieldBoss9
	waitmovement 0
	msgbox Rogue_Battle_Boss9_Text_0
	setweather WEATHER_SHADE
	doweather
	applymovement 1, BossToFightBoss9
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToFightBoss9
	waitmovement 0
	trainerbattle_single TRAINER_PHOEBE, Rogue_Battle_Boss9_Text_1, Rogue_Battle_Boss9_Text_2, Rogue_BattleDefeated_Boss9
	return


Rogue_BattleDefeated_Boss9::
	message Rogue_BattleDefeated_Boss9_Text_0
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	setflag FLAG_BADGE08_GET
	setflag FLAG_ROGUE_DEFEATED_BOSS07
	msgbox Rogue_BattleDefeated_Boss9_Text_1
	waitmessage
	special RemoveCameraObject
	warp MAP_ROGUE_HUB_TRANSITION, 0
	return


PlayerToBattleFieldBoss9:
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

CameraToBattleFieldBoss9:
	walk_up
	walk_up
	walk_up
	step_end

PlayerToFightBoss9:
	walk_up
	step_end

BossToFightBoss9:
	walk_down
	step_end

Rogue_Battle_Boss9_Text_0:
	.string "PHOEBE: A new challenger!$"

Rogue_Battle_Boss9_Text_1:
	.string "Don't hold back now!$"

Rogue_Battle_Boss9_Text_2:
	.string "Well fought trainer!$"

Rogue_BattleDefeated_Boss9_Text_0:
	.string "{PLAYER} received the ?? BADGE!$"

Rogue_BattleDefeated_Boss9_Text_1:
	.string "Good luck in your future battles!$"
