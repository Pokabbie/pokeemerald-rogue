Rogue_Boss_8_MapScripts::
	.byte 0


Rogue_Battle_Boss8::
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToBattleFieldBoss8
	waitmovement 0
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, CameraToBattleFieldBoss8
	waitmovement 0
	msgbox Rogue_Battle_Boss8_Text_0
	setweather WEATHER_VOLCANIC_ASH
	doweather
	applymovement 1, BossToFightBoss8
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToFightBoss8
	waitmovement 0
	trainerbattle_single TRAINER_SIDNEY, Rogue_Battle_Boss8_Text_1, Rogue_Battle_Boss8_Text_2, Rogue_BattleDefeated_Boss8
	return


Rogue_BattleDefeated_Boss8::
	message Rogue_BattleDefeated_Boss8_Text_0
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	setflag FLAG_BADGE08_GET
	setflag FLAG_ROGUE_DEFEATED_BOSS07
	msgbox Rogue_BattleDefeated_Boss8_Text_1
	waitmessage
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
	.string "SIDNEY: A new challenger!$"

Rogue_Battle_Boss8_Text_1:
	.string "Don't hold back now!$"

Rogue_Battle_Boss8_Text_2:
	.string "Well fought trainer!$"

Rogue_BattleDefeated_Boss8_Text_0:
	.string "{PLAYER} received the ?? BADGE!$"

Rogue_BattleDefeated_Boss8_Text_1:
	.string "Good luck in your future battles!$"
