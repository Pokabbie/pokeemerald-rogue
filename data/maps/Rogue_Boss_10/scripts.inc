Rogue_Boss_10_MapScripts::
	.byte 0


Rogue_Battle_Boss10::
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToBattleFieldBoss10
	waitmovement 0
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, CameraToBattleFieldBoss10
	waitmovement 0
	msgbox Rogue_Battle_Boss10_Text_0
	call Rogue_SetDesiredWeather
	applymovement 1, BossToFightBoss10
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToFightBoss10
	waitmovement 0
	playbgm MUS_ENCOUNTER_ELITE_FOUR, FALSE
	msgbox Rogue_Battle_Boss10_Text_1
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, TRAINER_ROGUE_BOSS_GLACIA, 0, Rogue_Battle_Boss10_Text_2
	call Rogue_BattleDefeated_Boss10
	return


Rogue_BattleDefeated_Boss10::
	call Rogue_ClearWeather
	message Rogue_BattleDefeated_Boss10_Text_0
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	setflag FLAG_ROGUE_DEFEATED_BOSS10
	msgbox Rogue_BattleDefeated_Boss10_Text_1
	waitmessage
	special HealPlayerParty
	special RemoveCameraObject
	warp MAP_ROGUE_HUB_TRANSITION, 0
	return


PlayerToBattleFieldBoss10:
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

CameraToBattleFieldBoss10:
	walk_up
	walk_up
	walk_up
	step_end

PlayerToFightBoss10:
	walk_up
	step_end

BossToFightBoss10:
	walk_down
	step_end

Rogue_Battle_Boss10_Text_0:
	.string "GLACIA: A new challenger!$"

Rogue_Battle_Boss10_Text_1:
	.string "Don't hold back now!$"

Rogue_Battle_Boss10_Text_2:
	.string "Well fought trainer!$"

Rogue_BattleDefeated_Boss10_Text_0:
	.string "{PLAYER} received a ELITE BADGE!$"

Rogue_BattleDefeated_Boss10_Text_1:
	.string "Good luck in your future battles!$"
