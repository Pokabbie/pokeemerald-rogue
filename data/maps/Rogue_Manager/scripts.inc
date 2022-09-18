Rogue_Manager_MapScripts::
	.byte 0


Rogue_PostBattle_Internal::
	msgbox Rogue_PostBattle_Internal_Text_0, MSGBOX_AUTOCLOSE
	return


Rogue_GiveItem_Internal::
	compare VAR_0x8001, ITEM_MASTER_BALL
	goto_if_eq Rogue_GiveItem_Internal_2
	goto_if_set FLAG_ROGUE_EXPANSION_ACTIVE, Rogue_GiveItem_Internal_4
	finditem VAR_0x8001, 3
	return

Rogue_GiveItem_Internal_2:
	finditem VAR_0x8001, 1
	return

Rogue_GiveItem_Internal_4:
	compare VAR_0x8001, 250
	goto_if_ge Rogue_GiveItem_Internal_17
Rogue_GiveItem_Internal_15:
	compare VAR_0x8001, 357
	goto_if_ge Rogue_GiveItem_Internal_14
Rogue_GiveItem_Internal_12:
	compare VAR_0x8001, 79
	goto_if_eq Rogue_GiveItem_Internal_9
	compare VAR_0x8001, 80
	goto_if_eq Rogue_GiveItem_Internal_9
	finditem VAR_0x8001, 3
	return

Rogue_GiveItem_Internal_7:
	finditem VAR_0x8001, 1
	return

Rogue_GiveItem_Internal_8:
	finditem VAR_0x8001, 1
	return

Rogue_GiveItem_Internal_9:
	finditem VAR_0x8001, 1
	return

Rogue_GiveItem_Internal_14:
	compare VAR_0x8001, 391
	goto_if_le Rogue_GiveItem_Internal_8
	goto Rogue_GiveItem_Internal_12

Rogue_GiveItem_Internal_17:
	compare VAR_0x8001, 338
	goto_if_le Rogue_GiveItem_Internal_7
	goto Rogue_GiveItem_Internal_15


Rogue_QuickSaveRun::
	lockall
	setflag FLAG_ROGUE_VALID_QUICK_SAVE
	call Common_EventScript_SaveGame
	compare VAR_RESULT, YES
	goto_if_eq Rogue_QuickSaveRun_2
	clearflag FLAG_ROGUE_VALID_QUICK_SAVE
Rogue_QuickSaveRun_1:
	releaseall
	return

Rogue_QuickSaveRun_2:
	fadescreen FADE_TO_BLACK
	delay 60
	special DoSoftReset
	goto Rogue_QuickSaveRun_1


Rogue_QuickSaveLoad::
	lockall
	goto_if_set FLAG_ROGUE_VALID_QUICK_SAVE, Rogue_QuickSaveLoad_2
	call Rogue_ReturnToHub
Rogue_QuickSaveLoad_1:
	releaseall
	return

Rogue_QuickSaveLoad_2:
	clearflag FLAG_ROGUE_VALID_QUICK_SAVE
	msgbox Rogue_QuickSaveLoad_Text_0
	waitmessage
	call Common_EventScript_ForceSaveGame
	goto Rogue_QuickSaveLoad_1


Rogue_RetireFromRun::
	lockall
	msgbox Rogue_RetireFromRun_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_RetireFromRun_2
Rogue_RetireFromRun_1:
	releaseall
	return

Rogue_RetireFromRun_2:
	call Rogue_ReturnToHub
	goto Rogue_RetireFromRun_1


Rogue_ReturnToHub::
	warprespawn
	return


Rogue_Common_TurnPlayerNorth::
	turnobject OBJ_EVENT_ID_PLAYER, DIR_NORTH
	end


Rogue_Common_TurnPlayerSouth::
	turnobject OBJ_EVENT_ID_PLAYER, DIR_SOUTH
	end


Rogue_Common_TurnPlayerEast::
	turnobject OBJ_EVENT_ID_PLAYER, DIR_EAST
	end


Rogue_Common_TurnPlayerWest::
	turnobject OBJ_EVENT_ID_PLAYER, DIR_WEST
	end


Rogue_Shop_GetRandomIndex::
	compare VAR_TEMP_F, 0
	goto_if_eq Rogue_Shop_GetRandomIndex_1
	copyvar VAR_RESULT, VAR_TEMP_F
	return

Rogue_Shop_GetRandomIndex_1:
	random 5
	addvar VAR_RESULT, 1
	copyvar VAR_TEMP_F, VAR_RESULT
	return


Rogue_Shop_Balls::
	lock
	faceplayer
	message Rogue_Shop_Balls_Text_0
	waitmessage
	rogue_dynamicpokemart ROGUE_SHOP_BALLS
	msgbox Rogue_Shop_Balls_Text_1
	release
	return


Rogue_Shop_Medicine::
	lock
	faceplayer
	message Rogue_Shop_Medicine_Text_0
	waitmessage
	rogue_dynamicpokemart ROGUE_SHOP_MEDICINE
	msgbox Rogue_Shop_Balls_Text_1
	release
	return


Rogue_Shop_TMs::
	lock
	faceplayer
	message Rogue_Shop_TMs_Text_0
	waitmessage
	rogue_dynamicpokemart ROGUE_SHOP_TMS
	msgbox Rogue_Shop_Balls_Text_1
	release
	return


Rogue_Shop_BattleEnhancers::
	lock
	faceplayer
	message Rogue_Shop_BattleEnhancers_Text_0
	waitmessage
	rogue_dynamicpokemart ROGUE_SHOP_BATTLE_ENHANCERS
	msgbox Rogue_Shop_Balls_Text_1
	release
	return


Rogue_Shop_HeldItems::
	lock
	faceplayer
	message Rogue_Shop_HeldItems_Text_0
	waitmessage
	rogue_dynamicpokemart ROGUE_SHOP_HELD_ITEMS
	msgbox Rogue_Shop_Balls_Text_1
	release
	return


Rogue_Shop_RareHeldItems::
	lock
	faceplayer
	message Rogue_Shop_RareHeldItems_Text_0
	waitmessage
	rogue_dynamicpokemart ROGUE_SHOP_RARE_HELD_ITEMS
	msgbox Rogue_Shop_Balls_Text_1
	release
	return


RogueSafari_Sign_South::
	goto_if_set FLAG_ROGUE_EXPANSION_ACTIVE, RogueSafari_Sign_South_1
	msgbox RogueSafari_Sign_South_Text_1, MSGBOX_SIGN
	return

RogueSafari_Sign_South_1:
	msgbox RogueSafari_Sign_South_Text_0, MSGBOX_SIGN
	return


RogueSafari_Sign_SouthWest::
	msgbox RogueSafari_Sign_SouthWest_Text_0, MSGBOX_SIGN
	return


RogueSafari_Sign_NorthWest::
	msgbox RogueSafari_Sign_NorthWest_Text_0, MSGBOX_SIGN
	return


RogueSafari_Sign_North::
	msgbox RogueSafari_Sign_North_Text_0, MSGBOX_SIGN
	return


RogueSafari_Sign_NorthEast::
	msgbox RogueSafari_Sign_NorthEast_Text_0, MSGBOX_SIGN
	return


RogueSafari_Sign_SouthEast::
	msgbox RogueSafari_Sign_SouthEast_Text_0, MSGBOX_SIGN
	return


Rogue_Tutor_MoveRelearn::
	special TeachMoveSetContextRelearnMove
	lock
	faceplayer
	msgbox Rogue_Tutor_MoveRelearn_Text_0, MSGBOX_DEFAULT
	showmoneybox 0, 0
	msgbox Rogue_Tutor_MoveRelearn_Text_1, MSGBOX_YESNO
	hidemoneybox
	compare VAR_RESULT, NO
	goto_if_eq Rogue_Tutor_MoveRelearn_2
Rogue_Tutor_MoveRelearn_1:
	checkmoney 1000
	compare VAR_RESULT, NO
	goto_if_eq Rogue_Tutor_MoveRelearn_5
Rogue_Tutor_MoveRelearn_4:
	special ChooseMonForMoveRelearner
	waitstate
	compare VAR_0x8004, PARTY_NOTHING_CHOSEN
	goto_if_eq Rogue_Tutor_MoveRelearn_8
Rogue_Tutor_MoveRelearn_7:
	special IsSelectedMonEgg
	compare VAR_RESULT, TRUE
	goto_if_eq Rogue_Tutor_MoveRelearn_11
Rogue_Tutor_MoveRelearn_10:
	special TeachMoveFromContext
	waitstate
	compare VAR_0x8004, 0
	goto_if_eq Rogue_Tutor_MoveRelearn_14
Rogue_Tutor_MoveRelearn_13:
	removemoney 1000
	release
	return

Rogue_Tutor_MoveRelearn_2:
	goto Rogue_Tutor_MoveRelearnCancel
	goto Rogue_Tutor_MoveRelearn_1

Rogue_Tutor_MoveRelearn_5:
	goto Rogue_Tutor_NotEnoughMoney
	goto Rogue_Tutor_MoveRelearn_4

Rogue_Tutor_MoveRelearn_8:
	goto Rogue_Tutor_MoveRelearnCancel
	goto Rogue_Tutor_MoveRelearn_7

Rogue_Tutor_MoveRelearn_11:
	goto Rogue_Tutor_MoveRelearnCancel
	goto Rogue_Tutor_MoveRelearn_10

Rogue_Tutor_MoveRelearn_14:
	goto Rogue_Tutor_MoveRelearnCancel
	goto Rogue_Tutor_MoveRelearn_13


Rogue_Tutor_EggMoveLearn::
	special TeachMoveSetContextEggMove
	lock
	faceplayer
	msgbox Rogue_Tutor_EggMoveLearn_Text_0, MSGBOX_DEFAULT
	showmoneybox 0, 0
	msgbox Rogue_Tutor_EggMoveLearn_Text_1, MSGBOX_YESNO
	hidemoneybox
	compare VAR_RESULT, NO
	goto_if_eq Rogue_Tutor_EggMoveLearn_2
Rogue_Tutor_EggMoveLearn_1:
	checkmoney 2000
	compare VAR_RESULT, NO
	goto_if_eq Rogue_Tutor_EggMoveLearn_5
Rogue_Tutor_EggMoveLearn_4:
	special ChooseMonForMoveRelearner
	waitstate
	compare VAR_0x8004, PARTY_NOTHING_CHOSEN
	goto_if_eq Rogue_Tutor_EggMoveLearn_8
Rogue_Tutor_EggMoveLearn_7:
	special IsSelectedMonEgg
	compare VAR_RESULT, TRUE
	goto_if_eq Rogue_Tutor_EggMoveLearn_11
Rogue_Tutor_EggMoveLearn_10:
	special TeachMoveFromContext
	waitstate
	compare VAR_0x8004, 0
	goto_if_eq Rogue_Tutor_EggMoveLearn_14
Rogue_Tutor_EggMoveLearn_13:
	removemoney 2000
	release
	return

Rogue_Tutor_EggMoveLearn_2:
	goto Rogue_Tutor_MoveRelearnCancel
	goto Rogue_Tutor_EggMoveLearn_1

Rogue_Tutor_EggMoveLearn_5:
	goto Rogue_Tutor_NotEnoughMoneyKind
	goto Rogue_Tutor_EggMoveLearn_4

Rogue_Tutor_EggMoveLearn_8:
	goto Rogue_Tutor_MoveRelearnCancel
	goto Rogue_Tutor_EggMoveLearn_7

Rogue_Tutor_EggMoveLearn_11:
	goto Rogue_Tutor_MoveRelearnCancel
	goto Rogue_Tutor_EggMoveLearn_10

Rogue_Tutor_EggMoveLearn_14:
	goto Rogue_Tutor_MoveRelearnCancel
	goto Rogue_Tutor_EggMoveLearn_13


Rogue_Tutor_TutorMoveLearn::
	special TeachMoveSetContextTutorMove
	lock
	faceplayer
	msgbox Rogue_Tutor_TutorMoveLearn_Text_0, MSGBOX_DEFAULT
	showmoneybox 0, 0
	msgbox Rogue_Tutor_TutorMoveLearn_Text_1, MSGBOX_YESNO
	hidemoneybox
	compare VAR_RESULT, NO
	goto_if_eq Rogue_Tutor_TutorMoveLearn_2
Rogue_Tutor_TutorMoveLearn_1:
	checkmoney 3000
	compare VAR_RESULT, NO
	goto_if_eq Rogue_Tutor_TutorMoveLearn_5
Rogue_Tutor_TutorMoveLearn_4:
	special ChooseMonForMoveRelearner
	waitstate
	compare VAR_0x8004, PARTY_NOTHING_CHOSEN
	goto_if_eq Rogue_Tutor_TutorMoveLearn_8
Rogue_Tutor_TutorMoveLearn_7:
	special IsSelectedMonEgg
	compare VAR_RESULT, TRUE
	goto_if_eq Rogue_Tutor_TutorMoveLearn_11
Rogue_Tutor_TutorMoveLearn_10:
	special TeachMoveFromContext
	waitstate
	compare VAR_0x8004, 0
	goto_if_eq Rogue_Tutor_TutorMoveLearn_14
Rogue_Tutor_TutorMoveLearn_13:
	removemoney 3000
	release
	return

Rogue_Tutor_TutorMoveLearn_2:
	goto Rogue_Tutor_MoveRelearnCancel
	goto Rogue_Tutor_TutorMoveLearn_1

Rogue_Tutor_TutorMoveLearn_5:
	goto Rogue_Tutor_NotEnoughMoney
	goto Rogue_Tutor_TutorMoveLearn_4

Rogue_Tutor_TutorMoveLearn_8:
	goto Rogue_Tutor_MoveRelearnCancel
	goto Rogue_Tutor_TutorMoveLearn_7

Rogue_Tutor_TutorMoveLearn_11:
	goto Rogue_Tutor_MoveRelearnCancel
	goto Rogue_Tutor_TutorMoveLearn_10

Rogue_Tutor_TutorMoveLearn_14:
	goto Rogue_Tutor_MoveRelearnCancel
	goto Rogue_Tutor_TutorMoveLearn_13


Rogue_Tutor_NotEnoughMoneyKind::
	msgbox Rogue_Tutor_NotEnoughMoneyKind_Text_0, MSGBOX_DEFAULT
	release
	return


Rogue_Tutor_NotEnoughMoney::
	msgbox Rogue_Tutor_NotEnoughMoney_Text_0, MSGBOX_DEFAULT
	release
	return


Rogue_Tutor_MoveRelearnCancel::
	msgbox Rogue_Tutor_MoveRelearnCancel_Text_0, MSGBOX_DEFAULT
	release
	return


Rogue_Tutor_MoveDelete::
	lock
	faceplayer
	msgbox Rogue_Tutor_MoveDelete_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Rogue_Tutor_MoveDelete_2
Rogue_Tutor_MoveDelete_1:
	special ChoosePartyMon
	waitstate
	compare VAR_0x8004, PARTY_NOTHING_CHOSEN
	goto_if_eq Rogue_Tutor_MoveDelete_5
Rogue_Tutor_MoveDelete_4:
	special IsSelectedMonEgg
	compare VAR_RESULT, TRUE
	goto_if_eq Rogue_Tutor_MoveDelete_8
Rogue_Tutor_MoveDelete_7:
	special GetNumMovesSelectedMonHas
	compare VAR_RESULT, 1
	goto_if_le Rogue_Tutor_MoveDelete_11
Rogue_Tutor_MoveDelete_10:
	fadescreen FADE_TO_BLACK
	special MoveDeleterChooseMoveToForget
	fadescreen FADE_FROM_BLACK
	compare VAR_0x8005, MAX_MON_MOVES
	goto_if_eq Rogue_Tutor_MoveDelete_14
Rogue_Tutor_MoveDelete_13:
	special MoveDeleterForgetMove
	playfanfare MUS_MOVE_DELETED
	waitfanfare
	release
	return

Rogue_Tutor_MoveDelete_2:
	goto Rogue_Tutor_MoveDeleteCancel
	goto Rogue_Tutor_MoveDelete_1

Rogue_Tutor_MoveDelete_5:
	goto Rogue_Tutor_MoveDeleteCancel
	goto Rogue_Tutor_MoveDelete_4

Rogue_Tutor_MoveDelete_8:
	goto Rogue_Tutor_MoveDeleteEgg
	goto Rogue_Tutor_MoveDelete_7

Rogue_Tutor_MoveDelete_11:
	goto Rogue_Tutor_MoveDeleteNoMoves
	goto Rogue_Tutor_MoveDelete_10

Rogue_Tutor_MoveDelete_14:
	goto Rogue_Tutor_MoveDeleteCancel
	goto Rogue_Tutor_MoveDelete_13


Rogue_Tutor_MoveDeleteNoMoves::
	msgbox Rogue_Tutor_MoveDeleteNoMoves_Text_0, MSGBOX_DEFAULT
	release
	return


Rogue_Tutor_MoveDeleteEgg::
	msgbox Rogue_Tutor_MoveDeleteEgg_Text_0, MSGBOX_DEFAULT
	release
	return


Rogue_Tutor_MoveDeleteCancel::
	msgbox Rogue_Tutor_MoveDeleteCancel_Text_0, MSGBOX_DEFAULT
	release
	return


Rogue_Battle_PreBoss::
	lock
	faceplayer
	buffernumberstring STR_VAR_1, VAR_ROGUE_CURRENT_LEVEL_CAP
	msgbox Rogue_Battle_PreBoss_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Battle_PreBoss_2
	msgbox Rogue_Battle_PreBoss_Text_2, MSGBOX_DEFAULT
Rogue_Battle_PreBoss_1:
	release
	return

Rogue_Battle_PreBoss_2:
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, TRAINER_ROGUE_POKEFAN_F, 0, Rogue_Battle_PreBoss_Text_1
	goto Rogue_Battle_PreBoss_1


Rogue_Heal_PreBoss::
	lock
	faceplayer
	msgbox Rogue_Heal_PreBoss_Text_0, MSGBOX_DEFAULT
	goto_if_set FLAG_ROGUE_FREE_HEAL_USED, Rogue_Heal_PreBoss_2
	msgbox Rogue_Heal_PreBoss_Text_3, MSGBOX_DEFAULT
	msgbox Rogue_Heal_PreBoss_Text_4, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Heal_PreBoss_19
Rogue_Heal_PreBoss_1:
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Heal_PreBoss_6
	msgbox Rogue_Heal_PreBoss_Text_6, MSGBOX_DEFAULT
Rogue_Heal_PreBoss_5:
	release
	return

Rogue_Heal_PreBoss_2:
	showmoneybox 0, 0
	switch VAR_ROGUE_DIFFICULTY
	case 0, Rogue_Heal_PreBoss_11
	case 1, Rogue_Heal_PreBoss_12
	case 2, Rogue_Heal_PreBoss_13
	case 3, Rogue_Heal_PreBoss_14
	case 4, Rogue_Heal_PreBoss_15
	case 5, Rogue_Heal_PreBoss_16
	case 6, Rogue_Heal_PreBoss_17
	checkconstantmoney 24000
	bufferconstantnumberstring STR_VAR_1, 24000
Rogue_Heal_PreBoss_9:
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Heal_PreBoss_22
	msgbox Rogue_Heal_PreBoss_Text_2, MSGBOX_DEFAULT
Rogue_Heal_PreBoss_21:
	hidemoneybox
	goto Rogue_Heal_PreBoss_1

Rogue_Heal_PreBoss_6:
	msgbox Rogue_Heal_PreBoss_Text_5, MSGBOX_DEFAULT
	goto Rogue_Heal_PreBoss_5

Rogue_Heal_PreBoss_11:
	checkconstantmoney 1000
	bufferconstantnumberstring STR_VAR_1, 1000
	goto Rogue_Heal_PreBoss_9

Rogue_Heal_PreBoss_12:
	checkconstantmoney 2000
	bufferconstantnumberstring STR_VAR_1, 2000
	goto Rogue_Heal_PreBoss_9

Rogue_Heal_PreBoss_13:
	checkconstantmoney 4000
	bufferconstantnumberstring STR_VAR_1, 4000
	goto Rogue_Heal_PreBoss_9

Rogue_Heal_PreBoss_14:
	checkconstantmoney 8000
	bufferconstantnumberstring STR_VAR_1, 8000
	goto Rogue_Heal_PreBoss_9

Rogue_Heal_PreBoss_15:
	checkconstantmoney 12000
	bufferconstantnumberstring STR_VAR_1, 12000
	goto Rogue_Heal_PreBoss_9

Rogue_Heal_PreBoss_16:
	checkconstantmoney 16000
	bufferconstantnumberstring STR_VAR_1, 16000
	goto Rogue_Heal_PreBoss_9

Rogue_Heal_PreBoss_17:
	checkconstantmoney 20000
	bufferconstantnumberstring STR_VAR_1, 20000
	goto Rogue_Heal_PreBoss_9

Rogue_Heal_PreBoss_19:
	call Common_EventScript_OutOfCenterPartyHeal
	setflag FLAG_ROGUE_FREE_HEAL_USED
	goto Rogue_Heal_PreBoss_1

Rogue_Heal_PreBoss_22:
	msgbox Rogue_Heal_PreBoss_Text_1, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Heal_PreBoss_25
	goto Rogue_Heal_PreBoss_21

Rogue_Heal_PreBoss_25:
	playse SE_VEND
	switch VAR_ROGUE_DIFFICULTY
	case 0, Rogue_Heal_PreBoss_29
	case 1, Rogue_Heal_PreBoss_30
	case 2, Rogue_Heal_PreBoss_31
	case 3, Rogue_Heal_PreBoss_32
	case 4, Rogue_Heal_PreBoss_33
	case 5, Rogue_Heal_PreBoss_34
	case 6, Rogue_Heal_PreBoss_35
	removeconstantmoney 24000
Rogue_Heal_PreBoss_27:
	call Common_EventScript_OutOfCenterPartyHeal
	goto Rogue_Heal_PreBoss_21

Rogue_Heal_PreBoss_29:
	removeconstantmoney 1000
	goto Rogue_Heal_PreBoss_27

Rogue_Heal_PreBoss_30:
	removeconstantmoney 2000
	goto Rogue_Heal_PreBoss_27

Rogue_Heal_PreBoss_31:
	removeconstantmoney 4000
	goto Rogue_Heal_PreBoss_27

Rogue_Heal_PreBoss_32:
	removeconstantmoney 8000
	goto Rogue_Heal_PreBoss_27

Rogue_Heal_PreBoss_33:
	removeconstantmoney 12000
	goto Rogue_Heal_PreBoss_27

Rogue_Heal_PreBoss_34:
	removeconstantmoney 16000
	goto Rogue_Heal_PreBoss_27

Rogue_Heal_PreBoss_35:
	removeconstantmoney 20000
	goto Rogue_Heal_PreBoss_27


Rogue_GymIdentity_PreBoss::
	msgbox Rogue_GymIdentity_PreBoss_Text_0, MSGBOX_DEFAULT
	goto_if_set FLAG_ROGUE_EASY_TRAINERS, Rogue_GymIdentity_PreBoss_1
	msgbox Rogue_GymIdentity_PreBoss_Text_3, MSGBOX_DEFAULT
	showmoneybox 0, 0
	switch VAR_ROGUE_DIFFICULTY
	case 0, Rogue_GymIdentity_PreBoss_6
	case 1, Rogue_GymIdentity_PreBoss_7
	case 2, Rogue_GymIdentity_PreBoss_8
	case 3, Rogue_GymIdentity_PreBoss_9
	case 4, Rogue_GymIdentity_PreBoss_10
	case 5, Rogue_GymIdentity_PreBoss_11
	case 6, Rogue_GymIdentity_PreBoss_12
	checkconstantmoney 24000
	bufferconstantnumberstring STR_VAR_1, 24000
Rogue_GymIdentity_PreBoss_4:
	compare VAR_RESULT, YES
	goto_if_eq Rogue_GymIdentity_PreBoss_15
	msgbox Rogue_GymIdentity_PreBoss_Text_5, MSGBOX_DEFAULT
Rogue_GymIdentity_PreBoss_14:
	hidemoneybox
	return

Rogue_GymIdentity_PreBoss_1:
	msgbox Rogue_GymIdentity_PreBoss_Text_1, MSGBOX_DEFAULT
	msgbox Rogue_GymIdentity_PreBoss_Text_2, MSGBOX_YESNO
	return

Rogue_GymIdentity_PreBoss_6:
	checkconstantmoney 1000
	bufferconstantnumberstring STR_VAR_1, 1000
	goto Rogue_GymIdentity_PreBoss_4

Rogue_GymIdentity_PreBoss_7:
	checkconstantmoney 2000
	bufferconstantnumberstring STR_VAR_1, 2000
	goto Rogue_GymIdentity_PreBoss_4

Rogue_GymIdentity_PreBoss_8:
	checkconstantmoney 4000
	bufferconstantnumberstring STR_VAR_1, 4000
	goto Rogue_GymIdentity_PreBoss_4

Rogue_GymIdentity_PreBoss_9:
	checkconstantmoney 8000
	bufferconstantnumberstring STR_VAR_1, 8000
	goto Rogue_GymIdentity_PreBoss_4

Rogue_GymIdentity_PreBoss_10:
	checkconstantmoney 12000
	bufferconstantnumberstring STR_VAR_1, 12000
	goto Rogue_GymIdentity_PreBoss_4

Rogue_GymIdentity_PreBoss_11:
	checkconstantmoney 16000
	bufferconstantnumberstring STR_VAR_1, 16000
	goto Rogue_GymIdentity_PreBoss_4

Rogue_GymIdentity_PreBoss_12:
	checkconstantmoney 20000
	bufferconstantnumberstring STR_VAR_1, 20000
	goto Rogue_GymIdentity_PreBoss_4

Rogue_GymIdentity_PreBoss_15:
	msgbox Rogue_GymIdentity_PreBoss_Text_4, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_GymIdentity_PreBoss_18
	goto Rogue_GymIdentity_PreBoss_14

Rogue_GymIdentity_PreBoss_18:
	playse SE_VEND
	switch VAR_ROGUE_DIFFICULTY
	case 0, Rogue_GymIdentity_PreBoss_22
	case 1, Rogue_GymIdentity_PreBoss_23
	case 2, Rogue_GymIdentity_PreBoss_24
	case 3, Rogue_GymIdentity_PreBoss_25
	case 4, Rogue_GymIdentity_PreBoss_26
	case 5, Rogue_GymIdentity_PreBoss_27
	case 6, Rogue_GymIdentity_PreBoss_28
	removeconstantmoney 24000
Rogue_GymIdentity_PreBoss_20:
	updatemoneybox
	waitse
	goto Rogue_GymIdentity_PreBoss_14

Rogue_GymIdentity_PreBoss_22:
	removeconstantmoney 1000
	goto Rogue_GymIdentity_PreBoss_20

Rogue_GymIdentity_PreBoss_23:
	removeconstantmoney 2000
	goto Rogue_GymIdentity_PreBoss_20

Rogue_GymIdentity_PreBoss_24:
	removeconstantmoney 4000
	goto Rogue_GymIdentity_PreBoss_20

Rogue_GymIdentity_PreBoss_25:
	removeconstantmoney 8000
	goto Rogue_GymIdentity_PreBoss_20

Rogue_GymIdentity_PreBoss_26:
	removeconstantmoney 12000
	goto Rogue_GymIdentity_PreBoss_20

Rogue_GymIdentity_PreBoss_27:
	removeconstantmoney 16000
	goto Rogue_GymIdentity_PreBoss_20

Rogue_GymIdentity_PreBoss_28:
	removeconstantmoney 20000
	goto Rogue_GymIdentity_PreBoss_20


Rogue_Battle_BreederF::
	trainerbattle_single TRAINER_ROGUE_BREEDER_F, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	call Rogue_PostBattle_Internal
	return


Rogue_Battle_BreederM::
	trainerbattle_single TRAINER_ROGUE_BREEDER_M, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	call Rogue_PostBattle_Internal
	return


Rogue_Battle_RichF::
	trainerbattle_single TRAINER_ROGUE_RICH_F, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	call Rogue_PostBattle_Internal
	return


Rogue_Battle_RichM::
	trainerbattle_single TRAINER_ROGUE_RICH_M, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	call Rogue_PostBattle_Internal
	return


Rogue_Battle_CooltrainerF::
	trainerbattle_single TRAINER_ROGUE_COOLTRAINER_F, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	call Rogue_PostBattle_Internal
	return


Rogue_Battle_CooltrainerM::
	trainerbattle_single TRAINER_ROGUE_COOLTRAINER_M, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	call Rogue_PostBattle_Internal
	return


Rogue_Battle_PokefanF::
	trainerbattle_single TRAINER_ROGUE_POKEFAN_F, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	call Rogue_PostBattle_Internal
	return


Rogue_Battle_PokefanM::
	trainerbattle_single TRAINER_ROGUE_POKEFAN_M, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	call Rogue_PostBattle_Internal
	return


Rogue_Battle_SchoolkidF::
	trainerbattle_single TRAINER_ROGUE_SCHOOL_KID_F, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	call Rogue_PostBattle_Internal
	return


Rogue_Battle_SchoolkidM::
	trainerbattle_single TRAINER_ROGUE_SCHOOL_KID_M, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	call Rogue_PostBattle_Internal
	return


Rogue_Battle_TuberF::
	trainerbattle_single TRAINER_ROGUE_TUBER_F, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	call Rogue_PostBattle_Internal
	return


Rogue_Battle_TuberM::
	trainerbattle_single TRAINER_ROGUE_TUBER_M, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	call Rogue_PostBattle_Internal
	return


Rogue_Battle_PoshF::
	trainerbattle_single TRAINER_ROGUE_POSH_F, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	call Rogue_PostBattle_Internal
	return


Rogue_Battle_PoshM::
	trainerbattle_single TRAINER_ROGUE_POSH_M, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	call Rogue_PostBattle_Internal
	return


Rogue_Battle_MagmaF::
	trainerbattle_single TRAINER_ROGUE_MAGMA_F, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	call Rogue_PostBattle_Internal
	return


Rogue_Battle_MagmaM::
	trainerbattle_single TRAINER_ROGUE_MAGMA_M, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	call Rogue_PostBattle_Internal
	return


Rogue_Battle_AquaF::
	trainerbattle_single TRAINER_ROGUE_AQUA_F, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	call Rogue_PostBattle_Internal
	return


Rogue_Battle_AquaM::
	trainerbattle_single TRAINER_ROGUE_AQUA_M, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	call Rogue_PostBattle_Internal
	return


Rogue_Give_Item0::
	setvar VAR_0x8000, 0
	copyvar VAR_0x8001, VAR_ROGUE_ITEM0
	call Rogue_GiveItem_Internal
	return


Rogue_Give_Item1::
	setvar VAR_0x8000, 1
	copyvar VAR_0x8001, VAR_ROGUE_ITEM1
	call Rogue_GiveItem_Internal
	return


Rogue_Give_Item2::
	setvar VAR_0x8000, 2
	copyvar VAR_0x8001, VAR_ROGUE_ITEM2
	call Rogue_GiveItem_Internal
	return


Rogue_Give_Item3::
	setvar VAR_0x8000, 3
	copyvar VAR_0x8001, VAR_ROGUE_ITEM3
	call Rogue_GiveItem_Internal
	return


Rogue_Give_Item4::
	setvar VAR_0x8000, 4
	copyvar VAR_0x8001, VAR_ROGUE_ITEM4
	call Rogue_GiveItem_Internal
	return


Rogue_Give_Item5::
	setvar VAR_0x8000, 5
	copyvar VAR_0x8001, VAR_ROGUE_ITEM5
	call Rogue_GiveItem_Internal
	return


Rogue_Give_Item6::
	setvar VAR_0x8000, 6
	copyvar VAR_0x8001, VAR_ROGUE_ITEM6
	call Rogue_GiveItem_Internal
	return


Rogue_Give_Item7::
	setvar VAR_0x8000, 7
	copyvar VAR_0x8001, VAR_ROGUE_ITEM7
	call Rogue_GiveItem_Internal
	return


Rogue_Give_Item8::
	setvar VAR_0x8000, 8
	copyvar VAR_0x8001, VAR_ROGUE_ITEM8
	call Rogue_GiveItem_Internal
	return


Rogue_Give_Item9::
	setvar VAR_0x8000, 9
	copyvar VAR_0x8001, VAR_ROGUE_ITEM9
	call Rogue_GiveItem_Internal
	return


Rogue_Give_Item10::
	setvar VAR_0x8000, 10
	copyvar VAR_0x8001, VAR_ROGUE_ITEM10
	call Rogue_GiveItem_Internal
	return


Rogue_Give_Item11::
	setvar VAR_0x8000, 11
	copyvar VAR_0x8001, VAR_ROGUE_ITEM11
	call Rogue_GiveItem_Internal
	return


Rogue_Give_Item12::
	setvar VAR_0x8000, 12
	copyvar VAR_0x8001, VAR_ROGUE_ITEM12
	call Rogue_GiveItem_Internal
	return


Rogue_Give_Item13::
	setvar VAR_0x8000, 13
	copyvar VAR_0x8001, VAR_ROGUE_ITEM13
	call Rogue_GiveItem_Internal
	return


Rogue_Give_Item14::
	setvar VAR_0x8000, 14
	copyvar VAR_0x8001, VAR_ROGUE_ITEM14
	call Rogue_GiveItem_Internal
	return


Rogue_Give_Item15::
	setvar VAR_0x8000, 15
	copyvar VAR_0x8001, VAR_ROGUE_ITEM15
	call Rogue_GiveItem_Internal
	return


Rogue_Give_Item16::
	setvar VAR_0x8000, 16
	copyvar VAR_0x8001, VAR_ROGUE_ITEM16
	call Rogue_GiveItem_Internal
	return


Rogue_Give_Item17::
	setvar VAR_0x8000, 17
	copyvar VAR_0x8001, VAR_ROGUE_ITEM17
	call Rogue_GiveItem_Internal
	return


Rogue_Give_Item18::
	setvar VAR_0x8000, 18
	copyvar VAR_0x8001, VAR_ROGUE_ITEM18
	call Rogue_GiveItem_Internal
	return


Rogue_PostBattle_Internal_Text_0:
	.string "Good luck with your next battle!$"

Rogue_QuickSaveLoad_Text_0:
	.string "Adventure has been reloaded.$"

Rogue_RetireFromRun_Text_0:
	.string "Are you sure you'd like to return to the\n"
	.string "HUB?$"

Rogue_Shop_Balls_Text_0:
	.string "Welcome to the POKéBALL Shop!$"

Rogue_Shop_Balls_Text_1:
	.string "Come again soon!$"

Rogue_Shop_Medicine_Text_0:
	.string "Welcome to the MEDICINE Shop!$"

Rogue_Shop_TMs_Text_0:
	.string "Welcome to the TM Shop!$"

Rogue_Shop_BattleEnhancers_Text_0:
	.string "Welcome to the TRAVELLING BATTLE Shop!\nI stock ENHANCEMENT ITEMS.$"

Rogue_Shop_HeldItems_Text_0:
	.string "Welcome to the TRAVELLING BATTLE Shop!\nI stock HELD ITEMS.$"

Rogue_Shop_RareHeldItems_Text_0:
	.string "Welcome to the TRAVELLING BATTLE Shop!\nI stock RARE HELD ITEMS.$"

RogueSafari_Sign_South_Text_0:
	.string "SOUTH AREA home to:\n"
	.string "NORMAL, FIGHTING, FAIRY POKéMON$"

RogueSafari_Sign_South_Text_1:
	.string "SOUTH AREA home to:\n"
	.string "NORMAL, FIGHTING POKéMON$"

RogueSafari_Sign_SouthWest_Text_0:
	.string "SOUTH WEST AREA home to:\n"
	.string "GRASS, POISON, DARK POKéMON$"

RogueSafari_Sign_NorthWest_Text_0:
	.string "NORTH WEST AREA home to:\n"
	.string "DRAGON, STEEL, PSYCHIC POKéMON$"

RogueSafari_Sign_North_Text_0:
	.string "NORTH AREA home to:\n"
	.string "FLYING, GHOST, FIRE POKéMON$"

RogueSafari_Sign_NorthEast_Text_0:
	.string "NORTH EAST AREA home to:\n"
	.string "ROCK, GROUND, ELECTRIC POKéMON$"

RogueSafari_Sign_SouthEast_Text_0:
	.string "SOUTH EAST AREA home to:\n"
	.string "WATER, BUG, ICE POKéMON$"

Rogue_Tutor_MoveRelearn_Text_0:
	.string "I'm the LVL MOVE TUTOR.\p"
	.string "I can teach POKéMON moves if you'd\n"
	.string "like?\p"
	.string "For a cost of course.$"

Rogue_Tutor_MoveRelearn_Text_1:
	.string "Would you like me to teach your POKéMON\n"
	.string "a move for ¥1000?$"

Rogue_Tutor_EggMoveLearn_Text_0:
	.string "I'm the EGG MOVE TUTOR.\p"
	.string "I can teach POKéMON moves if you'd\n"
	.string "like?$"

Rogue_Tutor_EggMoveLearn_Text_1:
	.string "Would you like me to teach your POKéMON\n"
	.string "a move for ¥2000?$"

Rogue_Tutor_TutorMoveLearn_Text_0:
	.string "HYAH!\p"
	.string "I'm the SPECIAL MOVE TUTOR!\p"
	.string "I can teach POKéMON moves some radical\n"
	.string "moves!$"

Rogue_Tutor_TutorMoveLearn_Text_1:
	.string "Want me to teach your POKéMON a move\n"
	.string "for ¥3000?$"

Rogue_Tutor_NotEnoughMoneyKind_Text_0:
	.string "I'm afraid you don't have enough money.\p"
	.string "Come back once you've gotten a bit more\n"
	.string "and I'll gladly teach some moves to\l"
	.string "your POKéMON.$"

Rogue_Tutor_NotEnoughMoney_Text_0:
	.string "You don't have enough money!\p"
	.string "Come back once you've got enough.$"

Rogue_Tutor_MoveRelearnCancel_Text_0:
	.string "You know where to find me, if you change\n"
	.string "your mind.$"

Rogue_Tutor_MoveDelete_Text_0:
	.string "Uh..\n"
	.string "Oh, yes, I'm the MOVE DELETER.\p"
	.string "I can make POKéMON forget their moves,\n"
	.string "if you'd like?$"

Rogue_Tutor_MoveDeleteNoMoves_Text_0:
	.string "This POKéMON needs move moves before I\n"
	.string "can make it forget any.$"

Rogue_Tutor_MoveDeleteEgg_Text_0:
	.string "As much as wish I could..\n"
	.string "I can't unteach an EGG.$"

Rogue_Tutor_MoveDeleteCancel_Text_0:
	.string "Oh.. OK then.$"

Rogue_Battle_PreBoss_Text_0:
	.string "I can help you level up if you'd like.\p"
	.string "Lvl. {STR_VAR_1} is the current cap.\n"
	.string "Would you like to battle?$"

Rogue_Battle_PreBoss_Text_1:
	.string "Good luck with the big battle!$"

Rogue_Battle_PreBoss_Text_2:
	.string "Let me know if you change your mind.$"

Rogue_Heal_PreBoss_Text_0:
	.string "Urgh...\n"
	.string "You want me to heal you or something?$"

Rogue_Heal_PreBoss_Text_1:
	.string "It's gonna cost you ¥{STR_VAR_1}.$"

Rogue_Heal_PreBoss_Text_2:
	.string "It's gonna cost you ¥{STR_VAR_1}.\n"
	.string "But you don't even have enough money...$"

Rogue_Heal_PreBoss_Text_3:
	.string "Look kid...\n"
	.string "I'll give you a single free heal. After\l"
	.string "that you're gonna have to pay.$"

Rogue_Heal_PreBoss_Text_4:
	.string "You wanna use your free heal now?$"

Rogue_Heal_PreBoss_Text_5:
	.string "What...\n"
	.string "You're still here?$"

Rogue_Heal_PreBoss_Text_6:
	.string "So thanks for wasting my time I guess...$"

Rogue_GymIdentity_PreBoss_Text_0:
	.string "A mysterious statue!$"

Rogue_GymIdentity_PreBoss_Text_1:
	.string "It reads...\p"
	.string "'I can provide you a glimpse of the\n"
	.string "future'$"

Rogue_GymIdentity_PreBoss_Text_2:
	.string "Do you want to read on?$"

Rogue_GymIdentity_PreBoss_Text_3:
	.string "It reads...\p"
	.string "'For a cost, I can provide you a glimpse\n"
	.string "of the future'$"

Rogue_GymIdentity_PreBoss_Text_4:
	.string "Do you want to insert ¥{STR_VAR_1}?$"

Rogue_GymIdentity_PreBoss_Text_5:
	.string "Oh no!\p"
	.string "I requires ¥{STR_VAR_1} which is\n"
	.string "more than what you have!$"

Rogue_Battle_BreederF_Text_0:
	.string "Let's battle!$"

Rogue_Battle_BreederF_Text_1:
	.string "Well fought!$"
