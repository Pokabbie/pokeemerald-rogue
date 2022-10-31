Rogue_Encounter_Graveyard_MapScripts::
	.byte 0


Rogue_Graveyard_ResurectionOption_UNUSED::
	lock
	goto_if_set FLAG_TEMP_1, Rogue_Graveyard_ResurectionOption_UNUSED_2
	msgbox Rogue_Graveyard_ResurectionOption_UNUSED_Text_1
	compare VAR_ROGUE_MAX_PARTY_SIZE, 2
	goto_if_gt Rogue_Graveyard_ResurectionOption_UNUSED_5
	msgbox Rogue_Graveyard_ResurectionOption_UNUSED_Text_9
	setflag FLAG_TEMP_1
Rogue_Graveyard_ResurectionOption_UNUSED_1:
	release
	return

Rogue_Graveyard_ResurectionOption_UNUSED_2:
	msgbox Rogue_Graveyard_ResurectionOption_UNUSED_Text_0
	goto Rogue_Graveyard_ResurectionOption_UNUSED_1

Rogue_Graveyard_ResurectionOption_UNUSED_5:
	specialvar VAR_0x8000, CalculatePlayerPartyCount
	bufferitemname STR_VAR_1, ITEM_REVIVAL_HERB
	msgbox Rogue_Graveyard_ResurectionOption_UNUSED_Text_2
	checkitemspace ITEM_REVIVAL_HERB
	compare VAR_RESULT, NO
	goto_if_eq Rogue_Graveyard_ResurectionOption_UNUSED_8
	msgbox Rogue_Graveyard_ResurectionOption_UNUSED_Text_4, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Graveyard_ResurectionOption_UNUSED_11
	msgbox Rogue_Graveyard_ResurectionOption_UNUSED_Text_6
	goto Rogue_Graveyard_ResurectionOption_UNUSED_1

Rogue_Graveyard_ResurectionOption_UNUSED_8:
	msgbox Rogue_Graveyard_ResurectionOption_UNUSED_Text_3
	goto Rogue_Graveyard_ResurectionOption_UNUSED_1

Rogue_Graveyard_ResurectionOption_UNUSED_11:
	compare VAR_0x8000, VAR_ROGUE_MAX_PARTY_SIZE
	goto_if_ge Rogue_Graveyard_ResurectionOption_UNUSED_15
	setvar VAR_0x8004, 6
Rogue_Graveyard_ResurectionOption_UNUSED_14:
	compare VAR_0x8004, PARTY_NOTHING_CHOSEN
	goto_if_eq Rogue_Graveyard_ResurectionOption_UNUSED_18
	special Rogue_ReducePartySize
	msgbox Rogue_Graveyard_ResurectionOption_UNUSED_Text_7
	giveitem ITEM_REVIVAL_HERB
	setflag FLAG_TEMP_1
	msgbox Rogue_Graveyard_ResurectionOption_UNUSED_Text_8
	goto Rogue_Graveyard_ResurectionOption_UNUSED_1

Rogue_Graveyard_ResurectionOption_UNUSED_15:
	msgbox Rogue_Graveyard_ResurectionOption_UNUSED_Text_5
	fadescreen FADE_TO_BLACK
	special ChoosePartyMon
	waitstate
	goto Rogue_Graveyard_ResurectionOption_UNUSED_14

Rogue_Graveyard_ResurectionOption_UNUSED_18:
	msgbox Rogue_Graveyard_ResurectionOption_UNUSED_Text_6
	goto Rogue_Graveyard_ResurectionOption_UNUSED_1


Rogue_Graveyard_ResurectionOption_UNUSED_Text_0:
	.string "Good luck child...$"

Rogue_Graveyard_ResurectionOption_UNUSED_Text_1:
	.string "Hello child...\p"
	.string "You seem to be struggling...$"

Rogue_Graveyard_ResurectionOption_UNUSED_Text_2:
	.string "I may be able to offer some help.\p"
	.string "I can give you a {STR_VAR_1} but it\n"
	.string "comes at a great cost...\p"
	.string "It will reduce the number of POKéMON\n"
	.string "you may carry in you PARTY for the\l"
	.string "remainder of this adventure...$"

Rogue_Graveyard_ResurectionOption_UNUSED_Text_3:
	.string "Oh...\n"
	.string "It looks like you're bag is too full...\p"
	.string "Free up some space if you are in need of\n"
	.string "my help...$"

Rogue_Graveyard_ResurectionOption_UNUSED_Text_4:
	.string "Would you like to accept my offer?$"

Rogue_Graveyard_ResurectionOption_UNUSED_Text_5:
	.string "You will have to release a POKéMON in\n"
	.string "order to continue...$"

Rogue_Graveyard_ResurectionOption_UNUSED_Text_6:
	.string "A courageous decision my child...$"

Rogue_Graveyard_ResurectionOption_UNUSED_Text_7:
	.string "And so it is done...$"

Rogue_Graveyard_ResurectionOption_UNUSED_Text_8:
	.string "Remember child...\p"
	.string "That item may only be used during\n"
	.string "battle...\p"
	.string "Do not wait until it is too late to use\n"
	.string "it!$"

Rogue_Graveyard_ResurectionOption_UNUSED_Text_9:
	.string "However I'm afraid I can't help you any\n"
	.string "further...$"
