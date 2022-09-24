Rogue_Boss_0_MapScripts::
	.byte 0


Rogue_Battle_Boss0_Hint::
	lock
	call Rogue_GymIdentity_PreBoss
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Battle_Boss0_Hint_2
Rogue_Battle_Boss0_Hint_1:
	release
	return

Rogue_Battle_Boss0_Hint_2:
	switch VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1
	case TYPE_NONE, Rogue_Battle_Boss0_Hint_6
	case TYPE_NORMAL, Rogue_Battle_Boss0_Hint_7
	case TYPE_FIGHTING, Rogue_Battle_Boss0_Hint_8
	case TYPE_FLYING, Rogue_Battle_Boss0_Hint_9
	case TYPE_POISON, Rogue_Battle_Boss0_Hint_10
	case TYPE_GROUND, Rogue_Battle_Boss0_Hint_11
	case TYPE_ROCK, Rogue_Battle_Boss0_Hint_12
	case TYPE_BUG, Rogue_Battle_Boss0_Hint_13
	case TYPE_GHOST, Rogue_Battle_Boss0_Hint_14
	case TYPE_STEEL, Rogue_Battle_Boss0_Hint_15
	case TYPE_MYSTERY, Rogue_Battle_Boss0_Hint_16
	case TYPE_FIRE, Rogue_Battle_Boss0_Hint_17
	case TYPE_WATER, Rogue_Battle_Boss0_Hint_18
	case TYPE_GRASS, Rogue_Battle_Boss0_Hint_19
	case TYPE_ELECTRIC, Rogue_Battle_Boss0_Hint_20
	case TYPE_PSYCHIC, Rogue_Battle_Boss0_Hint_21
	case TYPE_ICE, Rogue_Battle_Boss0_Hint_22
	case TYPE_DRAGON, Rogue_Battle_Boss0_Hint_23
	case TYPE_DARK, Rogue_Battle_Boss0_Hint_24
	case 18, Rogue_Battle_Boss0_Hint_25
	bufferstring STR_VAR_1, Rogue_Battle_Boss0_Hint_Text_20
Rogue_Battle_Boss0_Hint_4:
	msgbox Rogue_Battle_Boss0_Hint_Text_21, MSGBOX_DEFAULT
	goto Rogue_Battle_Boss0_Hint_1

Rogue_Battle_Boss0_Hint_6:
	bufferstring STR_VAR_1, Rogue_Battle_Boss0_Hint_Text_0
	goto Rogue_Battle_Boss0_Hint_4

Rogue_Battle_Boss0_Hint_7:
	bufferstring STR_VAR_1, Rogue_Battle_Boss0_Hint_Text_1
	goto Rogue_Battle_Boss0_Hint_4

Rogue_Battle_Boss0_Hint_8:
	bufferstring STR_VAR_1, Rogue_Battle_Boss0_Hint_Text_2
	goto Rogue_Battle_Boss0_Hint_4

Rogue_Battle_Boss0_Hint_9:
	bufferstring STR_VAR_1, Rogue_Battle_Boss0_Hint_Text_3
	goto Rogue_Battle_Boss0_Hint_4

Rogue_Battle_Boss0_Hint_10:
	bufferstring STR_VAR_1, Rogue_Battle_Boss0_Hint_Text_4
	goto Rogue_Battle_Boss0_Hint_4

Rogue_Battle_Boss0_Hint_11:
	bufferstring STR_VAR_1, Rogue_Battle_Boss0_Hint_Text_5
	goto Rogue_Battle_Boss0_Hint_4

Rogue_Battle_Boss0_Hint_12:
	bufferstring STR_VAR_1, Rogue_Battle_Boss0_Hint_Text_6
	goto Rogue_Battle_Boss0_Hint_4

Rogue_Battle_Boss0_Hint_13:
	bufferstring STR_VAR_1, Rogue_Battle_Boss0_Hint_Text_7
	goto Rogue_Battle_Boss0_Hint_4

Rogue_Battle_Boss0_Hint_14:
	bufferstring STR_VAR_1, Rogue_Battle_Boss0_Hint_Text_8
	goto Rogue_Battle_Boss0_Hint_4

Rogue_Battle_Boss0_Hint_15:
	bufferstring STR_VAR_1, Rogue_Battle_Boss0_Hint_Text_9
	goto Rogue_Battle_Boss0_Hint_4

Rogue_Battle_Boss0_Hint_16:
	bufferstring STR_VAR_1, Rogue_Battle_Boss0_Hint_Text_10
	goto Rogue_Battle_Boss0_Hint_4

Rogue_Battle_Boss0_Hint_17:
	bufferstring STR_VAR_1, Rogue_Battle_Boss0_Hint_Text_11
	goto Rogue_Battle_Boss0_Hint_4

Rogue_Battle_Boss0_Hint_18:
	bufferstring STR_VAR_1, Rogue_Battle_Boss0_Hint_Text_12
	goto Rogue_Battle_Boss0_Hint_4

Rogue_Battle_Boss0_Hint_19:
	bufferstring STR_VAR_1, Rogue_Battle_Boss0_Hint_Text_13
	goto Rogue_Battle_Boss0_Hint_4

Rogue_Battle_Boss0_Hint_20:
	bufferstring STR_VAR_1, Rogue_Battle_Boss0_Hint_Text_14
	goto Rogue_Battle_Boss0_Hint_4

Rogue_Battle_Boss0_Hint_21:
	bufferstring STR_VAR_1, Rogue_Battle_Boss0_Hint_Text_15
	goto Rogue_Battle_Boss0_Hint_4

Rogue_Battle_Boss0_Hint_22:
	bufferstring STR_VAR_1, Rogue_Battle_Boss0_Hint_Text_16
	goto Rogue_Battle_Boss0_Hint_4

Rogue_Battle_Boss0_Hint_23:
	bufferstring STR_VAR_1, Rogue_Battle_Boss0_Hint_Text_17
	goto Rogue_Battle_Boss0_Hint_4

Rogue_Battle_Boss0_Hint_24:
	bufferstring STR_VAR_1, Rogue_Battle_Boss0_Hint_Text_18
	goto Rogue_Battle_Boss0_Hint_4

Rogue_Battle_Boss0_Hint_25:
	bufferstring STR_VAR_1, Rogue_Battle_Boss0_Hint_Text_19
	goto Rogue_Battle_Boss0_Hint_4


Rogue_Battle_Boss0::
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToBattleField
	waitmovement 0
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, CameraToBattleField
	waitmovement 0
	buffertrainername STR_VAR_1, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA
	msgbox Rogue_Battle_Boss0_Text_0
	call Rogue_SetDesiredWeather
	applymovement 5, BossToFight
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToFight
	waitmovement 0
	playbgm MUS_ENCOUNTER_INTENSE, FALSE
	msgbox Rogue_Battle_Boss0_Text_1
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, 0, Rogue_Battle_Boss0_Text_2
	call Rogue_BattleDefeated_Boss0
	return


Rogue_BattleDefeated_Boss0::
	call Rogue_ClearWeather
	message Rogue_BattleDefeated_Boss0_Text_0
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	msgbox Rogue_BattleDefeated_Boss0_Text_1
	waitmessage
	special HealPlayerParty
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

Rogue_Battle_Boss0_Hint_Text_0:
	.string "NONE$"

Rogue_Battle_Boss0_Hint_Text_1:
	.string "NORMAL$"

Rogue_Battle_Boss0_Hint_Text_2:
	.string "FIGHTING$"

Rogue_Battle_Boss0_Hint_Text_3:
	.string "FLYING$"

Rogue_Battle_Boss0_Hint_Text_4:
	.string "POISON$"

Rogue_Battle_Boss0_Hint_Text_5:
	.string "GROUND$"

Rogue_Battle_Boss0_Hint_Text_6:
	.string "ROCK$"

Rogue_Battle_Boss0_Hint_Text_7:
	.string "BUG$"

Rogue_Battle_Boss0_Hint_Text_8:
	.string "GHOST$"

Rogue_Battle_Boss0_Hint_Text_9:
	.string "STEEL$"

Rogue_Battle_Boss0_Hint_Text_10:
	.string "STRANGE$"

Rogue_Battle_Boss0_Hint_Text_11:
	.string "FIRE$"

Rogue_Battle_Boss0_Hint_Text_12:
	.string "WATER$"

Rogue_Battle_Boss0_Hint_Text_13:
	.string "GRASS$"

Rogue_Battle_Boss0_Hint_Text_14:
	.string "ELECTRIC$"

Rogue_Battle_Boss0_Hint_Text_15:
	.string "PSYCHIC$"

Rogue_Battle_Boss0_Hint_Text_16:
	.string "ICE$"

Rogue_Battle_Boss0_Hint_Text_17:
	.string "DRAGON$"

Rogue_Battle_Boss0_Hint_Text_18:
	.string "DARK$"

Rogue_Battle_Boss0_Hint_Text_19:
	.string "FAIRY$"

Rogue_Battle_Boss0_Hint_Text_20:
	.string "???$"

Rogue_Battle_Boss0_Hint_Text_21:
	.string "A GYM LEADER of the type\n"
	.string "{STR_VAR_1} awaits...$"

Rogue_Battle_Boss0_Text_0:
	.string "{STR_VAR_1}: A new challenger!$"

Rogue_Battle_Boss0_Text_1:
	.string "Don't hold back now!$"

Rogue_Battle_Boss0_Text_2:
	.string "Well fought trainer!$"

Rogue_BattleDefeated_Boss0_Text_0:
	.string "{PLAYER} received a GYM BADGE!$"

Rogue_BattleDefeated_Boss0_Text_1:
	.string "Good luck in your future battles!$"
