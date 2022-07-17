Rogue_Encounter_RestStop_MapScripts::
	.byte 0


Rogue_Encounter_RestStop_Heal::
	lock
	faceplayer
	msgbox Rogue_Encounter_RestStop_Heal_Text_0, MSGBOX_DEFAULT
	call Common_EventScript_OutOfCenterPartyHeal
	msgbox Rogue_Encounter_RestStop_Heal_Text_1, MSGBOX_DEFAULT
	release
	return


Rogue_Encounter_RestStop_MoveRelearn::
	lock
	faceplayer
	msgbox Rogue_Encounter_RestStop_MoveRelearn_Text_0, MSGBOX_DEFAULT
	showmoneybox 0, 0
	msgbox Rogue_Encounter_RestStop_MoveRelearn_Text_1, MSGBOX_YESNO
	hidemoneybox
	compare VAR_RESULT, NO
	goto_if_eq Rogue_Encounter_RestStop_MoveRelearn_2
Rogue_Encounter_RestStop_MoveRelearn_1:
	checkmoney 1000
	compare VAR_RESULT, NO
	goto_if_eq Rogue_Encounter_RestStop_MoveRelearn_5
Rogue_Encounter_RestStop_MoveRelearn_4:
	special ChooseMonForMoveRelearner
	waitstate
	compare VAR_0x8004, PARTY_NOTHING_CHOSEN
	goto_if_eq Rogue_Encounter_RestStop_MoveRelearn_8
Rogue_Encounter_RestStop_MoveRelearn_7:
	special IsSelectedMonEgg
	compare VAR_RESULT, TRUE
	goto_if_eq Rogue_Encounter_RestStop_MoveRelearn_11
Rogue_Encounter_RestStop_MoveRelearn_10:
	special TeachMoveRelearnerMove
	waitstate
	compare VAR_0x8004, 0
	goto_if_eq Rogue_Encounter_RestStop_MoveRelearn_14
Rogue_Encounter_RestStop_MoveRelearn_13:
	removemoney 1000
	release
	return

Rogue_Encounter_RestStop_MoveRelearn_2:
	goto Rogue_Encounter_RestStop_MoveRelearnCancel
	goto Rogue_Encounter_RestStop_MoveRelearn_1

Rogue_Encounter_RestStop_MoveRelearn_5:
	goto Rogue_Encounter_RestStop_NotEnoughMoney
	goto Rogue_Encounter_RestStop_MoveRelearn_4

Rogue_Encounter_RestStop_MoveRelearn_8:
	goto Rogue_Encounter_RestStop_MoveRelearnCancel
	goto Rogue_Encounter_RestStop_MoveRelearn_7

Rogue_Encounter_RestStop_MoveRelearn_11:
	goto Rogue_Encounter_RestStop_MoveRelearnCancel
	goto Rogue_Encounter_RestStop_MoveRelearn_10

Rogue_Encounter_RestStop_MoveRelearn_14:
	goto Rogue_Encounter_RestStop_MoveRelearnCancel
	goto Rogue_Encounter_RestStop_MoveRelearn_13


Rogue_Encounter_RestStop_EggMoveLearn::
	lock
	faceplayer
	msgbox Rogue_Encounter_RestStop_EggMoveLearn_Text_0, MSGBOX_DEFAULT
	showmoneybox 0, 0
	msgbox Rogue_Encounter_RestStop_EggMoveLearn_Text_1, MSGBOX_YESNO
	hidemoneybox
	compare VAR_RESULT, NO
	goto_if_eq Rogue_Encounter_RestStop_EggMoveLearn_2
Rogue_Encounter_RestStop_EggMoveLearn_1:
	checkmoney 2000
	compare VAR_RESULT, NO
	goto_if_eq Rogue_Encounter_RestStop_EggMoveLearn_5
Rogue_Encounter_RestStop_EggMoveLearn_4:
	special ChooseMonForMoveRelearner
	waitstate
	compare VAR_0x8004, PARTY_NOTHING_CHOSEN
	goto_if_eq Rogue_Encounter_RestStop_EggMoveLearn_8
Rogue_Encounter_RestStop_EggMoveLearn_7:
	special IsSelectedMonEgg
	compare VAR_RESULT, TRUE
	goto_if_eq Rogue_Encounter_RestStop_EggMoveLearn_11
Rogue_Encounter_RestStop_EggMoveLearn_10:
	special TeachMoveEggMove
	waitstate
	compare VAR_0x8004, 0
	goto_if_eq Rogue_Encounter_RestStop_EggMoveLearn_14
Rogue_Encounter_RestStop_EggMoveLearn_13:
	removemoney 2000
	release
	return

Rogue_Encounter_RestStop_EggMoveLearn_2:
	goto Rogue_Encounter_RestStop_MoveRelearnCancel
	goto Rogue_Encounter_RestStop_EggMoveLearn_1

Rogue_Encounter_RestStop_EggMoveLearn_5:
	goto Rogue_Encounter_RestStop_NotEnoughMoney
	goto Rogue_Encounter_RestStop_EggMoveLearn_4

Rogue_Encounter_RestStop_EggMoveLearn_8:
	goto Rogue_Encounter_RestStop_MoveRelearnCancel
	goto Rogue_Encounter_RestStop_EggMoveLearn_7

Rogue_Encounter_RestStop_EggMoveLearn_11:
	goto Rogue_Encounter_RestStop_MoveRelearnCancel
	goto Rogue_Encounter_RestStop_EggMoveLearn_10

Rogue_Encounter_RestStop_EggMoveLearn_14:
	goto Rogue_Encounter_RestStop_MoveRelearnCancel
	goto Rogue_Encounter_RestStop_EggMoveLearn_13


Rogue_Encounter_RestStop_NotEnoughMoney::
	msgbox Rogue_Encounter_RestStop_NotEnoughMoney_Text_0, MSGBOX_DEFAULT
	release
	return


Rogue_Encounter_RestStop_MoveRelearnCancel::
	msgbox Rogue_Encounter_RestStop_MoveRelearnCancel_Text_0, MSGBOX_DEFAULT
	release
	return


Rogue_Encounter_RestStop_MoveDelete::
	lock
	faceplayer
	msgbox Rogue_Encounter_RestStop_MoveDelete_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Rogue_Encounter_RestStop_MoveDelete_2
Rogue_Encounter_RestStop_MoveDelete_1:
	special ChoosePartyMon
	waitstate
	compare VAR_0x8004, PARTY_NOTHING_CHOSEN
	goto_if_eq Rogue_Encounter_RestStop_MoveDelete_5
Rogue_Encounter_RestStop_MoveDelete_4:
	special IsSelectedMonEgg
	compare VAR_RESULT, TRUE
	goto_if_eq Rogue_Encounter_RestStop_MoveDelete_8
Rogue_Encounter_RestStop_MoveDelete_7:
	special GetNumMovesSelectedMonHas
	compare VAR_RESULT, 1
	goto_if_le Rogue_Encounter_RestStop_MoveDelete_11
Rogue_Encounter_RestStop_MoveDelete_10:
	fadescreen FADE_TO_BLACK
	special MoveDeleterChooseMoveToForget
	fadescreen FADE_FROM_BLACK
	compare VAR_0x8005, MAX_MON_MOVES
	goto_if_eq Rogue_Encounter_RestStop_MoveDelete_14
Rogue_Encounter_RestStop_MoveDelete_13:
	special MoveDeleterForgetMove
	playfanfare MUS_MOVE_DELETED
	waitfanfare
	release
	return

Rogue_Encounter_RestStop_MoveDelete_2:
	goto Rogue_Encounter_RestStop_MoveDeleteCancel
	goto Rogue_Encounter_RestStop_MoveDelete_1

Rogue_Encounter_RestStop_MoveDelete_5:
	goto Rogue_Encounter_RestStop_MoveDeleteCancel
	goto Rogue_Encounter_RestStop_MoveDelete_4

Rogue_Encounter_RestStop_MoveDelete_8:
	goto Rogue_Encounter_RestStop_MoveDeleteEgg
	goto Rogue_Encounter_RestStop_MoveDelete_7

Rogue_Encounter_RestStop_MoveDelete_11:
	goto Rogue_Encounter_RestStop_MoveDeleteNoMoves
	goto Rogue_Encounter_RestStop_MoveDelete_10

Rogue_Encounter_RestStop_MoveDelete_14:
	goto Rogue_Encounter_RestStop_MoveDeleteCancel
	goto Rogue_Encounter_RestStop_MoveDelete_13


Rogue_Encounter_RestStop_MoveDeleteNoMoves::
	msgbox Rogue_Encounter_RestStop_MoveDeleteNoMoves_Text_0, MSGBOX_DEFAULT
	release
	return


Rogue_Encounter_RestStop_MoveDeleteEgg::
	msgbox Rogue_Encounter_RestStop_MoveDeleteEgg_Text_0, MSGBOX_DEFAULT
	release
	return


Rogue_Encounter_RestStop_MoveDeleteCancel::
	msgbox Rogue_Encounter_RestStop_MoveDeleteCancel_Text_0, MSGBOX_DEFAULT
	release
	return


Rogue_Encounter_RestStop_Heal_Text_0:
	.string "You look tired.\n"
	.string "Let me patch you up!$"

Rogue_Encounter_RestStop_Heal_Text_1:
	.string "Take care!$"

Rogue_Encounter_RestStop_MoveRelearn_Text_0:
	.string "I'm the LVL MOVE TUTOR.\p"
	.string "I can teach POKéMON moves if you'd like.\p"
	.string "For a cost of course.$"

Rogue_Encounter_RestStop_MoveRelearn_Text_1:
	.string "Would you like me to teach your POKéMON\n"
	.string "a move for ¥1000?$"

Rogue_Encounter_RestStop_EggMoveLearn_Text_0:
	.string "I'm the EGG MOVE TUTOR.\p"
	.string "I can teach POKéMON moves if you'd like.\p"
	.string "For a cost of course.$"

Rogue_Encounter_RestStop_EggMoveLearn_Text_1:
	.string "Would you like me to teach your POKéMON\n"
	.string "a move for ¥2000?$"

Rogue_Encounter_RestStop_NotEnoughMoney_Text_0:
	.string "You don't have enough money!\p"
	.string "Come back once you've got enough.$"

Rogue_Encounter_RestStop_MoveRelearnCancel_Text_0:
	.string "You know where to find me, if you change\n"
	.string "your mind.$"

Rogue_Encounter_RestStop_MoveDelete_Text_0:
	.string "Uh..\n"
	.string "Oh, yes, I'm the MOVE DELETER.\p"
	.string "I can make POKéMON forget their moves,\n"
	.string "if you'd like?$"

Rogue_Encounter_RestStop_MoveDeleteNoMoves_Text_0:
	.string "This POKéMON needs move moves before I\n"
	.string "can make it forget any.$"

Rogue_Encounter_RestStop_MoveDeleteEgg_Text_0:
	.string "As much as wish I could..\n"
	.string "I can't unteach an EGG.$"

Rogue_Encounter_RestStop_MoveDeleteCancel_Text_0:
	.string "Oh.. OK then.$"
