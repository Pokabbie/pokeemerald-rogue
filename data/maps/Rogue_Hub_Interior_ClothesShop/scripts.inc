Rogue_Hub_Interior_ClothesShop_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, Rogue_Hub_Interior_ClothesShop_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE
	.byte 0

Rogue_Hub_Interior_ClothesShop_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE:
	map_script_2 VAR_TEMP_1, 0, Rogue_Common_TurnPlayerNorth
	.2byte 0


Rogue_Hub_Clothes_TalkAssistant::
	lockall
	faceplayer
	msgbox Rogue_Hub_Clothes_TalkAssistant_Text_0, MSGBOX_DEFAULT
	releaseall
	return


Rogue_Hub_Clothes_ChangeStyle0::
	lock
	msgbox Rogue_Hub_Clothes_ChangeStyle0_Text_0
	message Rogue_Hub_Clothes_ChangeStyle0_Text_1
	waitmessage
	multichoice 20, 4, MULTI_STYLE_CHOICE, NO
	switch VAR_RESULT
	case MULTI_B_PRESSED, Rogue_Hub_Clothes_ChangeStyle0_3
	copyvar VAR_0x8006, VAR_RESULT
	setvar VAR_RESULT, YES
Rogue_Hub_Clothes_ChangeStyle0_1:
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_Clothes_ChangeStyle0_6
	msgbox Rogue_Hub_Clothes_ChangeStyle0_Text_2
Rogue_Hub_Clothes_ChangeStyle0_5:
	release
	return

Rogue_Hub_Clothes_ChangeStyle0_3:
	msgbox Rogue_Hub_Clothes_ChangeStyle0_Text_0
	setvar VAR_RESULT, NO
	goto Rogue_Hub_Clothes_ChangeStyle0_1

Rogue_Hub_Clothes_ChangeStyle0_6:
	special Script_ChangePlayerStyle0
	warp MAP_ROGUE_HUB_INTERIOR_CLOTHES_SHOP, 19, 9
	waitstate
	goto Rogue_Hub_Clothes_ChangeStyle0_5


Rogue_Hub_Clothes_ChangeStyle1::
	lock
	msgbox Rogue_Hub_Clothes_ChangeStyle1_Text_0
	message Rogue_Hub_Clothes_ChangeStyle1_Text_1
	waitmessage
	multichoice 20, 2, MULTI_STYLE2_CHOICE, NO
	switch VAR_RESULT
	case MULTI_B_PRESSED, Rogue_Hub_Clothes_ChangeStyle1_3
	copyvar VAR_0x8006, VAR_RESULT
	setvar VAR_RESULT, YES
Rogue_Hub_Clothes_ChangeStyle1_1:
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_Clothes_ChangeStyle1_6
	msgbox Rogue_Hub_Clothes_ChangeStyle0_Text_2
Rogue_Hub_Clothes_ChangeStyle1_5:
	release
	return

Rogue_Hub_Clothes_ChangeStyle1_3:
	msgbox Rogue_Hub_Clothes_ChangeStyle0_Text_0
	setvar VAR_RESULT, NO
	goto Rogue_Hub_Clothes_ChangeStyle1_1

Rogue_Hub_Clothes_ChangeStyle1_6:
	special Script_ChangePlayerStyle1
	warp MAP_ROGUE_HUB_INTERIOR_CLOTHES_SHOP, 21, 9
	waitstate
	goto Rogue_Hub_Clothes_ChangeStyle1_5


Rogue_Hub_Clothes_ChangeName::
	lock
	msgbox Rogue_Hub_Clothes_ChangeName_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_Clothes_ChangeName_2
	msgbox Rogue_Hub_Clothes_ChangeStyle0_Text_2
Rogue_Hub_Clothes_ChangeName_1:
	release
	return

Rogue_Hub_Clothes_ChangeName_2:
	fadescreen FADE_TO_BLACK
	special ChangePlayerNickname
	waitstate
	msgbox Rogue_Hub_Clothes_ChangeName_Text_1
	goto Rogue_Hub_Clothes_ChangeName_1


Rogue_Hub_Clothes_SetStyle_Brendan::
	lock
	msgbox Rogue_Hub_Clothes_SetStyle_Brendan_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_Clothes_SetStyle_Brendan_2
Rogue_Hub_Clothes_SetStyle_Brendan_1:
	release
	return

Rogue_Hub_Clothes_SetStyle_Brendan_2:
	setvar VAR_0x8006, 0
	special Script_ChangePlayerGender
	warp MAP_ROGUE_HUB_INTERIOR_CLOTHES_SHOP, 4, 9
	waitstate
	goto Rogue_Hub_Clothes_SetStyle_Brendan_1


Rogue_Hub_Clothes_SetStyle_May::
	lock
	msgbox Rogue_Hub_Clothes_SetStyle_Brendan_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_Clothes_SetStyle_May_2
Rogue_Hub_Clothes_SetStyle_May_1:
	release
	return

Rogue_Hub_Clothes_SetStyle_May_2:
	setvar VAR_0x8006, 1
	special Script_ChangePlayerGender
	warp MAP_ROGUE_HUB_INTERIOR_CLOTHES_SHOP, 8, 9
	waitstate
	goto Rogue_Hub_Clothes_SetStyle_May_1


Rogue_Hub_Clothes_SetStyle_Red::
	lock
	msgbox Rogue_Hub_Clothes_SetStyle_Brendan_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_Clothes_SetStyle_Red_2
Rogue_Hub_Clothes_SetStyle_Red_1:
	release
	return

Rogue_Hub_Clothes_SetStyle_Red_2:
	setvar VAR_0x8006, 2
	special Script_ChangePlayerGender
	warp MAP_ROGUE_HUB_INTERIOR_CLOTHES_SHOP, 29, 9
	waitstate
	goto Rogue_Hub_Clothes_SetStyle_Red_1


Rogue_Hub_Clothes_SetStyle_Leaf::
	lock
	msgbox Rogue_Hub_Clothes_SetStyle_Brendan_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_Clothes_SetStyle_Leaf_2
Rogue_Hub_Clothes_SetStyle_Leaf_1:
	release
	return

Rogue_Hub_Clothes_SetStyle_Leaf_2:
	setvar VAR_0x8006, 3
	special Script_ChangePlayerGender
	warp MAP_ROGUE_HUB_INTERIOR_CLOTHES_SHOP, 33, 9
	waitstate
	goto Rogue_Hub_Clothes_SetStyle_Leaf_1


Rogue_Hub_Clothes_TalkAssistant_Text_0:
	.string "Are you feeling bored of your look?\n"
	.string "If so you're in the right place!\p"
	.string "Just walk up to one of the statues to\n"
	.string "swap your avatar!$"

Rogue_Hub_Clothes_ChangeStyle0_Text_0:
	.string "Hello!\n"
	.string "I can help with adjusting your\l"
	.string "appearance$"

Rogue_Hub_Clothes_ChangeStyle0_Text_1:
	.string "Which appearance option would you like\n"
	.string "to swap to?$"

Rogue_Hub_Clothes_ChangeStyle0_Text_2:
	.string "Come back any time!$"

Rogue_Hub_Clothes_ChangeStyle1_Text_0:
	.string "Hello!\n"
	.string "I can help with adjusting your clothing\l"
	.string "style$"

Rogue_Hub_Clothes_ChangeStyle1_Text_1:
	.string "Which clothing style option would you\n"
	.string "like to swap to?$"

Rogue_Hub_Clothes_ChangeName_Text_0:
	.string "Would you like to change your name?$"

Rogue_Hub_Clothes_ChangeName_Text_1:
	.string "{PLAYER} it is!$"

Rogue_Hub_Clothes_SetStyle_Brendan_Text_0:
	.string "Change to this style?$"
