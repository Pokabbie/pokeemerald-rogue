Rogue_Encounter_GameShow_MapScripts::
	.byte 0


Rogue_Encounter_GameShow_TriggerIntro::
	lockall
	msgbox Rogue_Encounter_GameShow_TriggerIntro_Text_0, MSGBOX_DEFAULT
	applymovement 2, GameShow_Host_Spin
	waitmovement 0
	msgbox Rogue_Encounter_GameShow_TriggerIntro_Text_1, MSGBOX_DEFAULT
	call Rogue_Encounter_GameShow_DropBoxes
	releaseall
	setvar VAR_TEMP_0, 1
	setvar VAR_TEMP_1, 1
	return


Rogue_Encounter_GameShow_RestoreBoxes::
	waitse
	playse SE_BALL_BOUNCE_1
	hideobjectat 3, MAP_ROGUE_ENCOUNTER_GAME_SHOW
	hideobjectat 1, MAP_ROGUE_ENCOUNTER_GAME_SHOW
	setobjectxy 3, 5, 2
	setobjectxy 1, 9, 2
	showobjectat 3, MAP_ROGUE_ENCOUNTER_GAME_SHOW
	showobjectat 1, MAP_ROGUE_ENCOUNTER_GAME_SHOW
	return


Rogue_Encounter_GameShow_DropBoxes::
	random 2
	copyvar VAR_TEMP_2, VAR_RESULT
	waitse
	playse SE_FALL
	waitse
	applymovement 3, GameShow_BoxFall
	applymovement 1, GameShow_BoxFall
	waitmovement 0
	waitse
	playse SE_BRIDGE_WALK
	return


Rogue_Encounter_GameShow_NextRound::
	compare VAR_TEMP_1, 3
	goto_if_ge Rogue_Encounter_GameShow_NextRound_1
	addvar VAR_TEMP_1, 1
	buffernumberstring STR_VAR_1, VAR_TEMP_1
	call Rogue_Encounter_GameShow_RestoreBoxes
	applymovement 2, GameShow_Host_Spin
	waitmovement 0
	msgbox Rogue_Encounter_GameShow_NextRound_Text_0, MSGBOX_DEFAULT
	call Rogue_Encounter_GameShow_DropBoxes
	return

Rogue_Encounter_GameShow_NextRound_1:
	call Rogue_Encounter_GameShow_Finish
	return


Rogue_Encounter_GameShow_Finish::
	call Rogue_Encounter_GameShow_RestoreBoxes
	applymovement 2, GameShow_Host_Spin
	waitmovement 0
	msgbox Rogue_Encounter_GameShow_Finish_Text_0, MSGBOX_DEFAULT
	applymovement 2, GameShow_Host_Exit
	waitmovement 0
	setobjectxyperm 2, 7, 0
	return


Rogue_Encounter_GameShow_DisplayGood::
	playse SE_DING_DONG
	waitse
	msgbox Rogue_Encounter_GameShow_DisplayGood_Text_0, MSGBOX_DEFAULT
	random 5
	switch VAR_RESULT
	case 0, Rogue_Encounter_GameShow_DisplayGood_3
	case 1, Rogue_Encounter_GameShow_DisplayGood_4
	case 2, Rogue_Encounter_GameShow_DisplayGood_5
	case 3, Rogue_Encounter_GameShow_DisplayGood_6
	case 4, Rogue_Encounter_GameShow_DisplayGood_7
Rogue_Encounter_GameShow_DisplayGood_1:
	call Rogue_Encounter_GameShow_NextRound
	return

Rogue_Encounter_GameShow_DisplayGood_3:
	addmoney 20000
	msgbox Rogue_Encounter_GameShow_DisplayGood_Text_1, MSGBOX_DEFAULT
	goto Rogue_Encounter_GameShow_DisplayGood_1

Rogue_Encounter_GameShow_DisplayGood_4:
	specialvar VAR_RESULT, CalculatePlayerPartyCount
	random VAR_RESULT
	copyvar VAR_0x8004, VAR_RESULT
	special BufferMonNickname
	setvar VAR_0x8005, 0
	special Rogue_AlterMonIVs
	msgbox Rogue_Encounter_GameShow_DisplayGood_Text_2, MSGBOX_DEFAULT
	goto Rogue_Encounter_GameShow_DisplayGood_1

Rogue_Encounter_GameShow_DisplayGood_5:
	setvar VAR_0x8004, 255
	setvar VAR_0x8005, 0
	special Rogue_AlterMonIVs
	msgbox Rogue_Encounter_GameShow_DisplayGood_Text_3, MSGBOX_DEFAULT
	goto Rogue_Encounter_GameShow_DisplayGood_1

Rogue_Encounter_GameShow_DisplayGood_6:
	random 10
	msgbox Rogue_Encounter_GameShow_DisplayGood_Text_4, MSGBOX_DEFAULT
	switch VAR_RESULT
	case 0, Rogue_Encounter_GameShow_DisplayGood_9
	case 1, Rogue_Encounter_GameShow_DisplayGood_10
	case 2, Rogue_Encounter_GameShow_DisplayGood_11
	case 3, Rogue_Encounter_GameShow_DisplayGood_12
	case 4, Rogue_Encounter_GameShow_DisplayGood_13
	case 5, Rogue_Encounter_GameShow_DisplayGood_14
	case 6, Rogue_Encounter_GameShow_DisplayGood_15
	case 7, Rogue_Encounter_GameShow_DisplayGood_16
	case 8, Rogue_Encounter_GameShow_DisplayGood_17
	case 9, Rogue_Encounter_GameShow_DisplayGood_18
	goto Rogue_Encounter_GameShow_DisplayGood_1

Rogue_Encounter_GameShow_DisplayGood_7:
	special HealPlayerParty
	msgbox Rogue_Encounter_GameShow_DisplayGood_Text_5, MSGBOX_DEFAULT
	goto Rogue_Encounter_GameShow_DisplayGood_1

Rogue_Encounter_GameShow_DisplayGood_9:
	giveitem ITEM_MAX_POTION, 3
	goto Rogue_Encounter_GameShow_DisplayGood_1

Rogue_Encounter_GameShow_DisplayGood_10:
	giveitem ITEM_FULL_RESTORE, 3
	goto Rogue_Encounter_GameShow_DisplayGood_1

Rogue_Encounter_GameShow_DisplayGood_11:
	giveitem ITEM_MASTER_BALL, 1
	goto Rogue_Encounter_GameShow_DisplayGood_1

Rogue_Encounter_GameShow_DisplayGood_12:
	giveitem ITEM_TM26_EARTHQUAKE, 1
	goto Rogue_Encounter_GameShow_DisplayGood_1

Rogue_Encounter_GameShow_DisplayGood_13:
	giveitem ITEM_TM15_HYPER_BEAM, 1
	goto Rogue_Encounter_GameShow_DisplayGood_1

Rogue_Encounter_GameShow_DisplayGood_14:
	giveitem ITEM_TM14_BLIZZARD, 1
	goto Rogue_Encounter_GameShow_DisplayGood_1

Rogue_Encounter_GameShow_DisplayGood_15:
	giveitem ITEM_TM38_FIRE_BLAST, 1
	goto Rogue_Encounter_GameShow_DisplayGood_1

Rogue_Encounter_GameShow_DisplayGood_16:
	giveitem ITEM_TM25_THUNDER, 1
	goto Rogue_Encounter_GameShow_DisplayGood_1

Rogue_Encounter_GameShow_DisplayGood_17:
	giveitem ITEM_POKE_BALL, 50
	goto Rogue_Encounter_GameShow_DisplayGood_1

Rogue_Encounter_GameShow_DisplayGood_18:
	giveitem ITEM_ULTRA_BALL, 20
	goto Rogue_Encounter_GameShow_DisplayGood_1


Rogue_Encounter_GameShow_DisplayBad::
	playse SE_BOO
	waitse
	msgbox Rogue_Encounter_GameShow_DisplayBad_Text_0, MSGBOX_DEFAULT
	random 9
	switch VAR_RESULT
	case 0, Rogue_Encounter_GameShow_DisplayBad_3
	case 1, Rogue_Encounter_GameShow_DisplayBad_4
	case 2, Rogue_Encounter_GameShow_DisplayBad_5
	case 3, Rogue_Encounter_GameShow_DisplayBad_6
	case 4, Rogue_Encounter_GameShow_DisplayBad_7
	case 5, Rogue_Encounter_GameShow_DisplayBad_8
	case 6, Rogue_Encounter_GameShow_DisplayBad_9
	case 7, Rogue_Encounter_GameShow_DisplayBad_10
	case 8, Rogue_Encounter_GameShow_DisplayBad_11
Rogue_Encounter_GameShow_DisplayBad_1:
	call Rogue_Encounter_GameShow_NextRound
	return

Rogue_Encounter_GameShow_DisplayBad_3:
	removemoney 20000
	msgbox Rogue_Encounter_GameShow_DisplayBad_Text_1, MSGBOX_DEFAULT
	goto Rogue_Encounter_GameShow_DisplayBad_1

Rogue_Encounter_GameShow_DisplayBad_4:
	specialvar VAR_RESULT, CalculatePlayerPartyCount
	random VAR_RESULT
	copyvar VAR_0x8004, VAR_RESULT
	special BufferMonNickname
	setvar VAR_0x8005, 1
	special Rogue_AlterMonIVs
	msgbox Rogue_Encounter_GameShow_DisplayBad_Text_2, MSGBOX_DEFAULT
	goto Rogue_Encounter_GameShow_DisplayBad_1

Rogue_Encounter_GameShow_DisplayBad_5:
	setvar VAR_0x8004, 255
	setvar VAR_0x8005, 1
	special Rogue_AlterMonIVs
	msgbox Rogue_Encounter_GameShow_DisplayBad_Text_3, MSGBOX_DEFAULT
	goto Rogue_Encounter_GameShow_DisplayBad_1

Rogue_Encounter_GameShow_DisplayBad_6:
	specialvar VAR_RESULT, CalculatePlayerPartyCount
	random VAR_RESULT
	copyvar VAR_0x8004, VAR_RESULT
	special BufferMonNickname
	special Rogue_RandomisePartyMon
	msgbox Rogue_Encounter_GameShow_DisplayBad_Text_4, MSGBOX_DEFAULT
	goto Rogue_Encounter_GameShow_DisplayBad_1

Rogue_Encounter_GameShow_DisplayBad_7:
	specialvar VAR_RESULT, CalculatePlayerPartyCount
	random VAR_RESULT
	copyvar VAR_0x8004, VAR_RESULT
	special BufferMonNickname
	random 5
	copyvar VAR_0x8005, VAR_RESULT
	special Rogue_ApplyStatusToMon
	switch VAR_0x8005
	case 0, Rogue_Encounter_GameShow_DisplayBad_13
	case 1, Rogue_Encounter_GameShow_DisplayBad_14
	case 2, Rogue_Encounter_GameShow_DisplayBad_15
	case 3, Rogue_Encounter_GameShow_DisplayBad_16
	case 4, Rogue_Encounter_GameShow_DisplayBad_17
	goto Rogue_Encounter_GameShow_DisplayBad_1

Rogue_Encounter_GameShow_DisplayBad_8:
	setvar VAR_0x8004, 255
	random 5
	copyvar VAR_0x8005, VAR_RESULT
	special Rogue_ApplyStatusToMon
	switch VAR_0x8005
	case 0, Rogue_Encounter_GameShow_DisplayBad_19
	case 1, Rogue_Encounter_GameShow_DisplayBad_20
	case 2, Rogue_Encounter_GameShow_DisplayBad_21
	case 3, Rogue_Encounter_GameShow_DisplayBad_22
	case 4, Rogue_Encounter_GameShow_DisplayBad_23
	goto Rogue_Encounter_GameShow_DisplayBad_1

Rogue_Encounter_GameShow_DisplayBad_9:
	removemoney 100
	msgbox Rogue_Encounter_GameShow_DisplayBad_Text_15, MSGBOX_DEFAULT
	goto Rogue_Encounter_GameShow_DisplayBad_1

Rogue_Encounter_GameShow_DisplayBad_10:
	removemoney 1000
	msgbox Rogue_Encounter_GameShow_DisplayBad_Text_16, MSGBOX_DEFAULT
	goto Rogue_Encounter_GameShow_DisplayBad_1

Rogue_Encounter_GameShow_DisplayBad_11:
	removemoney 10000
	msgbox Rogue_Encounter_GameShow_DisplayBad_Text_17, MSGBOX_DEFAULT
	goto Rogue_Encounter_GameShow_DisplayBad_1

Rogue_Encounter_GameShow_DisplayBad_13:
	msgbox Rogue_Encounter_GameShow_DisplayBad_Text_5, MSGBOX_DEFAULT
	goto Rogue_Encounter_GameShow_DisplayBad_1

Rogue_Encounter_GameShow_DisplayBad_14:
	msgbox Rogue_Encounter_GameShow_DisplayBad_Text_6, MSGBOX_DEFAULT
	goto Rogue_Encounter_GameShow_DisplayBad_1

Rogue_Encounter_GameShow_DisplayBad_15:
	msgbox Rogue_Encounter_GameShow_DisplayBad_Text_7, MSGBOX_DEFAULT
	goto Rogue_Encounter_GameShow_DisplayBad_1

Rogue_Encounter_GameShow_DisplayBad_16:
	msgbox Rogue_Encounter_GameShow_DisplayBad_Text_8, MSGBOX_DEFAULT
	goto Rogue_Encounter_GameShow_DisplayBad_1

Rogue_Encounter_GameShow_DisplayBad_17:
	msgbox Rogue_Encounter_GameShow_DisplayBad_Text_9, MSGBOX_DEFAULT
	goto Rogue_Encounter_GameShow_DisplayBad_1

Rogue_Encounter_GameShow_DisplayBad_19:
	msgbox Rogue_Encounter_GameShow_DisplayBad_Text_10, MSGBOX_DEFAULT
	goto Rogue_Encounter_GameShow_DisplayBad_1

Rogue_Encounter_GameShow_DisplayBad_20:
	msgbox Rogue_Encounter_GameShow_DisplayBad_Text_11, MSGBOX_DEFAULT
	goto Rogue_Encounter_GameShow_DisplayBad_1

Rogue_Encounter_GameShow_DisplayBad_21:
	msgbox Rogue_Encounter_GameShow_DisplayBad_Text_12, MSGBOX_DEFAULT
	goto Rogue_Encounter_GameShow_DisplayBad_1

Rogue_Encounter_GameShow_DisplayBad_22:
	msgbox Rogue_Encounter_GameShow_DisplayBad_Text_13, MSGBOX_DEFAULT
	goto Rogue_Encounter_GameShow_DisplayBad_1

Rogue_Encounter_GameShow_DisplayBad_23:
	msgbox Rogue_Encounter_GameShow_DisplayBad_Text_14, MSGBOX_DEFAULT
	goto Rogue_Encounter_GameShow_DisplayBad_1


Rogue_Encounter_GameShow_ChooseLeft::
	lock
	turnobject 2, DIR_WEST
	msgbox Rogue_Encounter_GameShow_ChooseLeft_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Encounter_GameShow_ChooseLeft_2
Rogue_Encounter_GameShow_ChooseLeft_1:
	release
	return

Rogue_Encounter_GameShow_ChooseLeft_2:
	msgbox Rogue_Encounter_GameShow_ChooseLeft_Text_1, MSGBOX_DEFAULT
	compare VAR_TEMP_2, 0
	goto_if_eq Rogue_Encounter_GameShow_ChooseLeft_4
	call Rogue_Encounter_GameShow_DisplayBad
	goto Rogue_Encounter_GameShow_ChooseLeft_1

Rogue_Encounter_GameShow_ChooseLeft_4:
	call Rogue_Encounter_GameShow_DisplayGood
	goto Rogue_Encounter_GameShow_ChooseLeft_1


Rogue_Encounter_GameShow_ChooseRight::
	lock
	turnobject 2, DIR_EAST
	msgbox Rogue_Encounter_GameShow_ChooseRight_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Encounter_GameShow_ChooseRight_2
Rogue_Encounter_GameShow_ChooseRight_1:
	release
	return

Rogue_Encounter_GameShow_ChooseRight_2:
	msgbox Rogue_Encounter_GameShow_ChooseRight_Text_1, MSGBOX_DEFAULT
	compare VAR_TEMP_2, 0
	goto_if_eq Rogue_Encounter_GameShow_ChooseRight_4
	call Rogue_Encounter_GameShow_DisplayGood
	goto Rogue_Encounter_GameShow_ChooseRight_1

Rogue_Encounter_GameShow_ChooseRight_4:
	call Rogue_Encounter_GameShow_DisplayBad
	goto Rogue_Encounter_GameShow_ChooseRight_1


GameShow_BoxFall:
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	step_end

GameShow_Host_Spin:
	face_right
	delay_1
	face_up
	delay_1
	face_left
	delay_1
	face_down
	delay_1
	face_right
	delay_1
	face_up
	delay_1
	face_left
	delay_1
	face_down
	delay_1
	jump_in_place_down
	step_end

GameShow_Host_Exit:
	jump_2_up
	jump_2_up
	step_end

Rogue_Encounter_GameShow_TriggerIntro_Text_0:
	.string "WAHEY!$"

Rogue_Encounter_GameShow_TriggerIntro_Text_1:
	.string "Welcome to my ever so lovingly setup\n"
	.string "GAME SHOW!\p"
	.string "This game is simple!\n"
	.string "You'll get 2 choices...\p"
	.string "One might be good...\n"
	.string "One might be bad...!\p"
	.string "Now!\n"
	.string "On to Round 1!$"

Rogue_Encounter_GameShow_NextRound_Text_0:
	.string "On to Round {STR_VAR_1}!$"

Rogue_Encounter_GameShow_Finish_Text_0:
	.string "That's all for now folks!\n"
	.string "Ke ke ke$"

Rogue_Encounter_GameShow_DisplayGood_Text_0:
	.string "Well chosen!$"

Rogue_Encounter_GameShow_DisplayGood_Text_1:
	.string "You just got an extra ¥20000!$"

Rogue_Encounter_GameShow_DisplayGood_Text_2:
	.string "{STR_VAR_1} just got a bit stronger!$"

Rogue_Encounter_GameShow_DisplayGood_Text_3:
	.string "All of your POKéMON all just got a bit\n"
	.string "stronger!$"

Rogue_Encounter_GameShow_DisplayGood_Text_4:
	.string "You will recieve a special item!$"

Rogue_Encounter_GameShow_DisplayGood_Text_5:
	.string "All of your POKéMON all just got all\n"
	.string "healed up!$"

Rogue_Encounter_GameShow_DisplayBad_Text_0:
	.string "Oh no...$"

Rogue_Encounter_GameShow_DisplayBad_Text_1:
	.string "You just lost ¥20000.$"

Rogue_Encounter_GameShow_DisplayBad_Text_2:
	.string "{STR_VAR_1} just got a bit weaker.$"

Rogue_Encounter_GameShow_DisplayBad_Text_3:
	.string "All of your POKéMON all just got a bit\n"
	.string "weaker.$"

Rogue_Encounter_GameShow_DisplayBad_Text_4:
	.string "Oh no!\n"
	.string "{STR_VAR_1} was traded away.$"

Rogue_Encounter_GameShow_DisplayBad_Text_5:
	.string "{STR_VAR_1} is now poisoned.$"

Rogue_Encounter_GameShow_DisplayBad_Text_6:
	.string "{STR_VAR_1} is now paralyzed.$"

Rogue_Encounter_GameShow_DisplayBad_Text_7:
	.string "{STR_VAR_1} is now sleeping.$"

Rogue_Encounter_GameShow_DisplayBad_Text_8:
	.string "{STR_VAR_1} is now frozen.$"

Rogue_Encounter_GameShow_DisplayBad_Text_9:
	.string "{STR_VAR_1} is now burnt.$"

Rogue_Encounter_GameShow_DisplayBad_Text_10:
	.string "All of your POKéMON are now poisoned.$"

Rogue_Encounter_GameShow_DisplayBad_Text_11:
	.string "All of your POKéMON are now paralyzed.$"

Rogue_Encounter_GameShow_DisplayBad_Text_12:
	.string "All of your POKéMON are now sleeping.$"

Rogue_Encounter_GameShow_DisplayBad_Text_13:
	.string "All of your POKéMON are now frozen.$"

Rogue_Encounter_GameShow_DisplayBad_Text_14:
	.string "All of your POKéMON are now burnt.$"

Rogue_Encounter_GameShow_DisplayBad_Text_15:
	.string "You just lost ¥100.$"

Rogue_Encounter_GameShow_DisplayBad_Text_16:
	.string "You just lost ¥1000.$"

Rogue_Encounter_GameShow_DisplayBad_Text_17:
	.string "You just lost ¥10000.$"

Rogue_Encounter_GameShow_ChooseLeft_Text_0:
	.string "So you pick the LEFT?$"

Rogue_Encounter_GameShow_ChooseLeft_Text_1:
	.string "The LEFT it is!$"

Rogue_Encounter_GameShow_ChooseRight_Text_0:
	.string "So you pick the RIGHT?$"

Rogue_Encounter_GameShow_ChooseRight_Text_1:
	.string "The RIGHT it is!$"
