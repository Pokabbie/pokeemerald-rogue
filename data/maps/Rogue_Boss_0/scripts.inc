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
	applymovement 3, BossToFight
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
	goto_if_unset FLAG_ROGUE_QUESTS_NEVER_ASK_FOR_RETIRE, Rogue_BattleDefeated_Boss0_3
Rogue_BattleDefeated_Boss0_1:
	switch VAR_ROGUE_DIFFICULTY
	case 1, Rogue_BattleDefeated_Boss0_8
	case 2, Rogue_BattleDefeated_Boss0_9
	case 3, Rogue_BattleDefeated_Boss0_10
	case 4, Rogue_BattleDefeated_Boss0_11
	case 5, Rogue_BattleDefeated_Boss0_12
	case 6, Rogue_BattleDefeated_Boss0_13
	case 7, Rogue_BattleDefeated_Boss0_14
	case 8, Rogue_BattleDefeated_Boss0_15
Rogue_BattleDefeated_Boss0_6:
	msgbox Rogue_BattleDefeated_Boss0_Text_3
	waitmessage
	special HealPlayerParty
	special RemoveCameraObject
	warp MAP_ROGUE_HUB_TRANSITION, 0
	waitstate
	return

Rogue_BattleDefeated_Boss0_2:
	msgbox Rogue_BattleDefeated_Boss0_Text_1, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Rogue_BattleDefeated_Boss0_16
	setflag FLAG_ROGUE_QUESTS_NEVER_ASK_FOR_RETIRE
	goto Rogue_BattleDefeated_Boss0_1

Rogue_BattleDefeated_Boss0_3:
	goto_if_set FLAG_ROGUE_QUESTS_ASK_FOR_RETIRE, Rogue_BattleDefeated_Boss0_2
	goto Rogue_BattleDefeated_Boss0_1

Rogue_BattleDefeated_Boss0_8:
	checkitem ITEM_HM01
	compare VAR_RESULT, NO
	goto_if_eq Rogue_BattleDefeated_Boss0_19
	goto Rogue_BattleDefeated_Boss0_6

Rogue_BattleDefeated_Boss0_9:
	checkitem ITEM_HM05_FLASH
	compare VAR_RESULT, NO
	goto_if_eq Rogue_BattleDefeated_Boss0_21
	goto Rogue_BattleDefeated_Boss0_6

Rogue_BattleDefeated_Boss0_10:
	checkitem ITEM_HM06_ROCK_SMASH
	compare VAR_RESULT, NO
	goto_if_eq Rogue_BattleDefeated_Boss0_23
	goto Rogue_BattleDefeated_Boss0_6

Rogue_BattleDefeated_Boss0_11:
	checkitem ITEM_HM04_STRENGTH
	compare VAR_RESULT, NO
	goto_if_eq Rogue_BattleDefeated_Boss0_25
	goto Rogue_BattleDefeated_Boss0_6

Rogue_BattleDefeated_Boss0_12:
	checkitem ITEM_HM08_DIVE
	compare VAR_RESULT, NO
	goto_if_eq Rogue_BattleDefeated_Boss0_27
	goto Rogue_BattleDefeated_Boss0_6

Rogue_BattleDefeated_Boss0_13:
	checkitem ITEM_HM02_FLY
	compare VAR_RESULT, NO
	goto_if_eq Rogue_BattleDefeated_Boss0_29
	goto Rogue_BattleDefeated_Boss0_6

Rogue_BattleDefeated_Boss0_14:
	checkitem ITEM_HM07_WATERFALL
	compare VAR_RESULT, NO
	goto_if_eq Rogue_BattleDefeated_Boss0_31
	goto Rogue_BattleDefeated_Boss0_6

Rogue_BattleDefeated_Boss0_15:
	checkitem ITEM_HM03_SURF
	compare VAR_RESULT, NO
	goto_if_eq Rogue_BattleDefeated_Boss0_33
	goto Rogue_BattleDefeated_Boss0_6

Rogue_BattleDefeated_Boss0_16:
	msgbox Rogue_BattleDefeated_Boss0_Text_2, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_BattleDefeated_Boss0_35
	setflag FLAG_ROGUE_QUESTS_NEVER_ASK_FOR_RETIRE
	goto Rogue_BattleDefeated_Boss0_1

Rogue_BattleDefeated_Boss0_19:
	giveitem ITEM_HM01
	goto Rogue_BattleDefeated_Boss0_6

Rogue_BattleDefeated_Boss0_21:
	giveitem ITEM_HM05_FLASH
	goto Rogue_BattleDefeated_Boss0_6

Rogue_BattleDefeated_Boss0_23:
	giveitem ITEM_HM06_ROCK_SMASH
	goto Rogue_BattleDefeated_Boss0_6

Rogue_BattleDefeated_Boss0_25:
	giveitem ITEM_HM04_STRENGTH
	goto Rogue_BattleDefeated_Boss0_6

Rogue_BattleDefeated_Boss0_27:
	giveitem ITEM_HM08_DIVE
	goto Rogue_BattleDefeated_Boss0_6

Rogue_BattleDefeated_Boss0_29:
	giveitem ITEM_HM02_FLY
	goto Rogue_BattleDefeated_Boss0_6

Rogue_BattleDefeated_Boss0_31:
	giveitem ITEM_HM07_WATERFALL
	goto Rogue_BattleDefeated_Boss0_6

Rogue_BattleDefeated_Boss0_33:
	giveitem ITEM_HM03_SURF
	goto Rogue_BattleDefeated_Boss0_6

Rogue_BattleDefeated_Boss0_35:
	call Rogue_ReturnToHub
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
	.string "Looks like you have some Quests to\n"
	.string "collect!\p"
	.string "You can either continue this Adventure\n"
	.string "or Retire and collect them now.\p"
	.string "Would you like to continue this\n"
	.string "Adventure?$"

Rogue_BattleDefeated_Boss0_Text_2:
	.string "So you want to Retire and return to the\n"
	.string "HUB?$"

Rogue_BattleDefeated_Boss0_Text_3:
	.string "Good luck in your future battles!$"
