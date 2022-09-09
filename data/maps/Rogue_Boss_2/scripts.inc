Rogue_Boss_2_MapScripts::
	.byte 0


Rogue_Battle_Boss2_Hint::
	lock
	call Rogue_GymIdentity_PreBoss
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Battle_Boss2_Hint_2
Rogue_Battle_Boss2_Hint_1:
	release
	return

Rogue_Battle_Boss2_Hint_2:
	msgbox Rogue_Battle_Boss2_Hint_Text_0, MSGBOX_DEFAULT
	goto Rogue_Battle_Boss2_Hint_1


Rogue_Battle_Boss2::
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToBattleFieldBoss2
	waitmovement 0
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, CameraToBattleFieldBoss2
	waitmovement 0
	msgbox Rogue_Battle_Boss2_Text_0
	goto_if_set FLAG_ROGUE_WEATHER_ACTIVE, Rogue_Battle_Boss2_2
Rogue_Battle_Boss2_1:
	applymovement 5, BossToFightBoss2
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToFightBoss2
	waitmovement 0
	playbgm MUS_ENCOUNTER_INTENSE, FALSE
	msgbox Rogue_Battle_Boss2_Text_1
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, TRAINER_WATTSON_1, 0, Rogue_Battle_Boss2_Text_2
	call Rogue_BattleDefeated_Boss2
	return

Rogue_Battle_Boss2_2:
	setweather WEATHER_RAIN_THUNDERSTORM
	doweather
	goto Rogue_Battle_Boss2_1


Rogue_BattleDefeated_Boss2::
	setweather WEATHER_NONE
	doweather
	message Rogue_BattleDefeated_Boss2_Text_0
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	setflag FLAG_BADGE03_GET
	setflag FLAG_ROGUE_DEFEATED_BOSS02
	msgbox Rogue_BattleDefeated_Boss2_Text_1
	waitmessage
	special HealPlayerParty
	special RemoveCameraObject
	warp MAP_ROGUE_HUB_TRANSITION, 0
	return


PlayerToBattleFieldBoss2:
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

CameraToBattleFieldBoss2:
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

PlayerToFightBoss2:
	walk_right
	face_right
	step_end

BossToFightBoss2:
	walk_left
	step_end

Rogue_Battle_Boss2_Hint_Text_0:
	.string "A GYM LEADER of the type ELECTRIC\n"
	.string "awaits...$"

Rogue_Battle_Boss2_Text_0:
	.string "WATTSON: A new challenger!$"

Rogue_Battle_Boss2_Text_1:
	.string "Don't hold back now!$"

Rogue_Battle_Boss2_Text_2:
	.string "Well fought trainer!$"

Rogue_BattleDefeated_Boss2_Text_0:
	.string "{PLAYER} received a GYM BADGE!$"

Rogue_BattleDefeated_Boss2_Text_1:
	.string "Good luck in your future battles!$"
