Rogue_Hub_Interior_ClothesShop_MapScripts::
	.byte 0


Rogue_Hub_Clothes_TalkAssistant::
	lockall
	faceplayer
	msgbox Rogue_Hub_Clothes_TalkAssistant_Text_0, MSGBOX_DEFAULT
	releaseall
	return


Rogue_Hub_Clothes_SetStyle_EmrBrendan::
	lock
	msgbox Rogue_Hub_Clothes_SetStyle_EmrBrendan_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_Clothes_SetStyle_EmrBrendan_2
Rogue_Hub_Clothes_SetStyle_EmrBrendan_1:
	release
	return

Rogue_Hub_Clothes_SetStyle_EmrBrendan_2:
	setvar VAR_0x8006, 0
	special Script_ChangePlayerGender
	warp MAP_ROGUE_HUB, 5
	waitstate
	goto Rogue_Hub_Clothes_SetStyle_EmrBrendan_1


Rogue_Hub_Clothes_SetStyle_EmrMay::
	lock
	msgbox Rogue_Hub_Clothes_SetStyle_EmrBrendan_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_Clothes_SetStyle_EmrMay_2
Rogue_Hub_Clothes_SetStyle_EmrMay_1:
	release
	return

Rogue_Hub_Clothes_SetStyle_EmrMay_2:
	setvar VAR_0x8006, 1
	special Script_ChangePlayerGender
	warp MAP_ROGUE_HUB, 5
	waitstate
	goto Rogue_Hub_Clothes_SetStyle_EmrMay_1


Rogue_Hub_Clothes_SetStyle_Red::
	lock
	msgbox Rogue_Hub_Clothes_SetStyle_EmrBrendan_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_Clothes_SetStyle_Red_2
Rogue_Hub_Clothes_SetStyle_Red_1:
	release
	return

Rogue_Hub_Clothes_SetStyle_Red_2:
	setvar VAR_0x8006, 2
	special Script_ChangePlayerGender
	warp MAP_ROGUE_HUB, 5
	waitstate
	goto Rogue_Hub_Clothes_SetStyle_Red_1


Rogue_Hub_Clothes_SetStyle_Leaf::
	lock
	msgbox Rogue_Hub_Clothes_SetStyle_EmrBrendan_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_Clothes_SetStyle_Leaf_2
Rogue_Hub_Clothes_SetStyle_Leaf_1:
	release
	return

Rogue_Hub_Clothes_SetStyle_Leaf_2:
	setvar VAR_0x8006, 3
	special Script_ChangePlayerGender
	warp MAP_ROGUE_HUB, 5
	waitstate
	goto Rogue_Hub_Clothes_SetStyle_Leaf_1


Rogue_Hub_Clothes_SetStyle_RSBrendan::
	lock
	msgbox Rogue_Hub_Clothes_SetStyle_EmrBrendan_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_Clothes_SetStyle_RSBrendan_2
Rogue_Hub_Clothes_SetStyle_RSBrendan_1:
	release
	return

Rogue_Hub_Clothes_SetStyle_RSBrendan_2:
	setvar VAR_0x8006, 4
	special Script_ChangePlayerGender
	warp MAP_ROGUE_HUB, 5
	waitstate
	goto Rogue_Hub_Clothes_SetStyle_RSBrendan_1


Rogue_Hub_Clothes_SetStyle_RSMay::
	lock
	msgbox Rogue_Hub_Clothes_SetStyle_EmrBrendan_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_Clothes_SetStyle_RSMay_2
Rogue_Hub_Clothes_SetStyle_RSMay_1:
	release
	return

Rogue_Hub_Clothes_SetStyle_RSMay_2:
	setvar VAR_0x8006, 5
	special Script_ChangePlayerGender
	warp MAP_ROGUE_HUB, 5
	waitstate
	goto Rogue_Hub_Clothes_SetStyle_RSMay_1


Rogue_Hub_Clothes_TalkAssistant_Text_0:
	.string "Are you feeling bored of your look?\n"
	.string "If so you're in the right place!\p"
	.string "Just walk up to one of the statues to\n"
	.string "swap$"

Rogue_Hub_Clothes_SetStyle_EmrBrendan_Text_0:
	.string "Change to this style?$"
