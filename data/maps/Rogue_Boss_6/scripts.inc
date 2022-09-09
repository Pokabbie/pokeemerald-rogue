Rogue_Boss_6_MapScripts::
	.byte 0


Rogue_Battle_Boss6_Hint::
	lock
	call Rogue_GymIdentity_PreBoss
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Battle_Boss6_Hint_2
Rogue_Battle_Boss6_Hint_1:
	release
	return

Rogue_Battle_Boss6_Hint_2:
	msgbox Rogue_Battle_Boss6_Hint_Text_0, MSGBOX_DEFAULT
	goto Rogue_Battle_Boss6_Hint_1


Rogue_Battle_Boss6::
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToBattleFieldBoss6
	waitmovement 0
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, CameraToBattleFieldBoss6
	waitmovement 0
	goto_if_set FLAG_ROGUE_WEATHER_ACTIVE, Rogue_Battle_Boss6_2
Rogue_Battle_Boss6_1:
	msgbox Rogue_Battle_Boss6_Text_0
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToFightBoss6
	applymovement 6, BossLizaToFightBoss6
	applymovement 5, BossTateToFightBoss6
	waitmovement 0
	playbgm MUS_ENCOUNTER_INTENSE, FALSE
	msgbox Rogue_Battle_Boss6_Text_1
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, TRAINER_TATE_AND_LIZA_1, 0, Rogue_Battle_Boss6_Text_2
	call Rogue_BattleDefeated_Boss6
	return

Rogue_Battle_Boss6_2:
	setweather WEATHER_FOG_HORIZONTAL
	doweather
	goto Rogue_Battle_Boss6_1


Rogue_BattleDefeated_Boss6::
	setweather WEATHER_NONE
	doweather
	message Rogue_BattleDefeated_Boss6_Text_0
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	setflag FLAG_BADGE07_GET
	setflag FLAG_ROGUE_DEFEATED_BOSS06
	msgbox Rogue_BattleDefeated_Boss6_Text_1
	waitmessage
	special HealPlayerParty
	special RemoveCameraObject
	warp MAP_ROGUE_HUB_TRANSITION, 0
	return


PlayerToBattleFieldBoss6:
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

CameraToBattleFieldBoss6:
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

PlayerToFightBoss6:
	walk_right
	face_right
	step_end

BossTateToFightBoss6:
	walk_left
	walk_down
	face_left
	step_end

BossLizaToFightBoss6:
	walk_left
	step_end

Rogue_Battle_Boss6_Hint_Text_0:
	.string "A GYM LEADER of the type PSYCHIC\n"
	.string "awaits...$"

Rogue_Battle_Boss6_Text_0:
	.string "LIZA: Look TATE!\n"
	.string "A new challenger!$"

Rogue_Battle_Boss6_Text_1:
	.string "Don't hold back now!$"

Rogue_Battle_Boss6_Text_2:
	.string "Well fought trainer!$"

Rogue_BattleDefeated_Boss6_Text_0:
	.string "{PLAYER} received a GYM BADGE!$"

Rogue_BattleDefeated_Boss6_Text_1:
	.string "Good luck in your future battles!$"
