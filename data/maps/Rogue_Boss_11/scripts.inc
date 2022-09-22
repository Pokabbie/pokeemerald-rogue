Rogue_Boss_11_MapScripts::
	.byte 0


Rogue_Battle_Boss11::
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToBattleFieldBoss11
	waitmovement 0
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, CameraToBattleFieldBoss11
	waitmovement 0
	msgbox Rogue_Battle_Boss11_Text_0
	call Rogue_SetDesiredWeather
	applymovement 1, BossToFightBoss11
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToFightBoss11
	waitmovement 0
	playbgm MUS_ENCOUNTER_ELITE_FOUR, FALSE
	msgbox Rogue_Battle_Boss11_Text_1
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, TRAINER_DRAKE, 0, Rogue_Battle_Boss11_Text_2
	call Rogue_BattleDefeated_Boss11
	return


Rogue_BattleDefeated_Boss11::
	call Rogue_ClearWeather
	message Rogue_BattleDefeated_Boss11_Text_0
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	setflag FLAG_ROGUE_DEFEATED_BOSS11
	msgbox Rogue_BattleDefeated_Boss11_Text_1
	waitmessage
	special HealPlayerParty
	special RemoveCameraObject
	warp MAP_ROGUE_HUB_TRANSITION, 0
	return


PlayerToBattleFieldBoss11:
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

CameraToBattleFieldBoss11:
	walk_up
	walk_up
	walk_up
	step_end

PlayerToFightBoss11:
	walk_up
	step_end

BossToFightBoss11:
	walk_down
	step_end

Rogue_Battle_Boss11_Text_0:
	.string "DRAKE: A new challenger!$"

Rogue_Battle_Boss11_Text_1:
	.string "Don't hold back now!$"

Rogue_Battle_Boss11_Text_2:
	.string "Well fought trainer!$"

Rogue_BattleDefeated_Boss11_Text_0:
	.string "{PLAYER} received a ELITE BADGE!$"

Rogue_BattleDefeated_Boss11_Text_1:
	.string "Good luck in your future battles!$"
