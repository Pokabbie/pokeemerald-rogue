Rogue_Manager_MapScripts::
	.byte 0


Rogue_PostBattle_Internal::
	msgbox Rogue_PostBattle_Internal_Text_0, MSGBOX_AUTOCLOSE
	return


Rogue_GiveItem_Internal::
	finditem VAR_0x8001, 3
	return


Rogue_Shop_Utilty::
	lock
	faceplayer
	message Rogue_Shop_Utilty_Text_0
	waitmessage
	goto_if_set FLAG_ROGUE_RUN_ACTIVE, Rogue_Shop_Utilty_2
	copyvar VAR_0x8000, VAR_ROGUE_FURTHEST_DIFFICULTY
Rogue_Shop_Utilty_1:
	switch VAR_0x8000
	case 0, Rogue_Shop_Utilty_7
	case 1, Rogue_Shop_Utilty_8
	case 2, Rogue_Shop_Utilty_9
	case 3, Rogue_Shop_Utilty_10
	case 4, Rogue_Shop_Utilty_11
	case 5, Rogue_Shop_Utilty_12
	pokemart Rogue_Shop_Utility6
Rogue_Shop_Utilty_5:
	msgbox Rogue_Shop_Utilty_Text_1
	release
	return

Rogue_Shop_Utilty_2:
	copyvar VAR_0x8000, VAR_ROGUE_DIFFICULTY
	goto Rogue_Shop_Utilty_1

Rogue_Shop_Utilty_7:
	pokemart Rogue_Shop_Utility0
	goto Rogue_Shop_Utilty_5

Rogue_Shop_Utilty_8:
	pokemart Rogue_Shop_Utility1
	goto Rogue_Shop_Utilty_5

Rogue_Shop_Utilty_9:
	pokemart Rogue_Shop_Utility2
	goto Rogue_Shop_Utilty_5

Rogue_Shop_Utilty_10:
	pokemart Rogue_Shop_Utility3
	goto Rogue_Shop_Utilty_5

Rogue_Shop_Utilty_11:
	pokemart Rogue_Shop_Utility4
	goto Rogue_Shop_Utilty_5

Rogue_Shop_Utilty_12:
	pokemart Rogue_Shop_Utility5
	goto Rogue_Shop_Utilty_5


	.align 2
Rogue_Shop_Utility0:
	.2byte ITEM_POTION
	.2byte ITEM_POKE_BALL
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_NONE

	.align 2
Rogue_Shop_Utility1:
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_REPEL
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_BURN_HEAL
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_AWAKENING
	.2byte ITEM_ICE_HEAL
	.2byte ITEM_NONE

	.align 2
Rogue_Shop_Utility2:
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_REPEL
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_BURN_HEAL
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_AWAKENING
	.2byte ITEM_ICE_HEAL
	.2byte ITEM_NONE

	.align 2
Rogue_Shop_Utility3:
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_MAX_POTION
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_REPEL
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_MAX_REPEL
	.2byte ITEM_FULL_HEAL
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_BURN_HEAL
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_AWAKENING
	.2byte ITEM_ICE_HEAL
	.2byte ITEM_NONE

	.align 2
Rogue_Shop_Utility4:
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_MAX_POTION
	.2byte ITEM_FULL_RESTORE
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_REPEL
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_MAX_REPEL
	.2byte ITEM_FULL_HEAL
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_BURN_HEAL
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_AWAKENING
	.2byte ITEM_ICE_HEAL
	.2byte ITEM_NONE

	.align 2
Rogue_Shop_Utility5:
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_MAX_POTION
	.2byte ITEM_FULL_RESTORE
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_NET_BALL
	.2byte ITEM_DIVE_BALL
	.2byte ITEM_NEST_BALL
	.2byte ITEM_REPEAT_BALL
	.2byte ITEM_TIMER_BALL
	.2byte ITEM_LUXURY_BALL
	.2byte ITEM_REPEL
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_MAX_REPEL
	.2byte ITEM_FULL_HEAL
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_BURN_HEAL
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_AWAKENING
	.2byte ITEM_ICE_HEAL
	.2byte ITEM_NONE

	.align 2
Rogue_Shop_Utility6:
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_MAX_POTION
	.2byte ITEM_FULL_RESTORE
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_MASTER_BALL
	.2byte ITEM_NET_BALL
	.2byte ITEM_DIVE_BALL
	.2byte ITEM_NEST_BALL
	.2byte ITEM_REPEAT_BALL
	.2byte ITEM_TIMER_BALL
	.2byte ITEM_LUXURY_BALL
	.2byte ITEM_REPEL
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_MAX_REPEL
	.2byte ITEM_FULL_HEAL
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_BURN_HEAL
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_AWAKENING
	.2byte ITEM_ICE_HEAL
	.2byte ITEM_NONE

Rogue_Shop_Specials::
	return


Rogue_Shop_KeyItems::
	return


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

Rogue_Shop_Utilty_Text_0:
	.string "Welcome to the Utility Shop!$"

Rogue_Shop_Utilty_Text_1:
	.string "Come again soon!$"

Rogue_Battle_BreederF_Text_0:
	.string "Let's battle!$"

Rogue_Battle_BreederF_Text_1:
	.string "Well fought!$"
