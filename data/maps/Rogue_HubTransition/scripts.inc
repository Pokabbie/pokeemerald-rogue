Rogue_HubTransition_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, Rogue_HubTransition_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE
	.byte 0

Rogue_HubTransition_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE:
	applymovement OBJ_EVENT_ID_PLAYER, LookNorth
	waitmovement 0
	return


LookNorth:
	face_up
	step_end

Rogue_Battle_BreederF::
	trainerbattle_single TRAINER_ROGUE_BREEDER_F, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	msgbox Rogue_Battle_BreederF_Text_2, MSGBOX_AUTOCLOSE
	return


Rogue_Battle_BreederM::
	trainerbattle_single TRAINER_ROGUE_BREEDER_M, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	msgbox Rogue_Battle_BreederF_Text_2, MSGBOX_AUTOCLOSE
	return


Rogue_Battle_RichF::
	trainerbattle_single TRAINER_ROGUE_RICH_F, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	msgbox Rogue_Battle_BreederF_Text_2, MSGBOX_AUTOCLOSE
	return


Rogue_Battle_RichM::
	trainerbattle_single TRAINER_ROGUE_RICH_M, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	msgbox Rogue_Battle_BreederF_Text_2, MSGBOX_AUTOCLOSE
	return


Rogue_Battle_CooltrainerF::
	trainerbattle_single TRAINER_ROGUE_COOLTRAINER_F, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	msgbox Rogue_Battle_BreederF_Text_2, MSGBOX_AUTOCLOSE
	return


Rogue_Battle_CooltrainerM::
	trainerbattle_single TRAINER_ROGUE_COOLTRAINER_M, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	msgbox Rogue_Battle_BreederF_Text_2, MSGBOX_AUTOCLOSE
	return


Rogue_Battle_PokefanF::
	trainerbattle_single TRAINER_ROGUE_POKEFAN_F, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	msgbox Rogue_Battle_BreederF_Text_2, MSGBOX_AUTOCLOSE
	return


Rogue_Battle_PokefanM::
	trainerbattle_single TRAINER_ROGUE_POKEFAN_M, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	msgbox Rogue_Battle_BreederF_Text_2, MSGBOX_AUTOCLOSE
	return


Rogue_Battle_SchoolkidF::
	trainerbattle_single TRAINER_ROGUE_SCHOOL_KID_F, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	msgbox Rogue_Battle_BreederF_Text_2, MSGBOX_AUTOCLOSE
	return


Rogue_Battle_SchoolkidM::
	trainerbattle_single TRAINER_ROGUE_SCHOOL_KID_M, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	msgbox Rogue_Battle_BreederF_Text_2, MSGBOX_AUTOCLOSE
	return


Rogue_Battle_TuberF::
	trainerbattle_single TRAINER_ROGUE_TUBER_F, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	msgbox Rogue_Battle_BreederF_Text_2, MSGBOX_AUTOCLOSE
	return


Rogue_Battle_TuberM::
	trainerbattle_single TRAINER_ROGUE_TUBER_M, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	msgbox Rogue_Battle_BreederF_Text_2, MSGBOX_AUTOCLOSE
	return


Rogue_Battle_PoshF::
	trainerbattle_single TRAINER_ROGUE_POSH_F, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	msgbox Rogue_Battle_BreederF_Text_2, MSGBOX_AUTOCLOSE
	return


Rogue_Battle_PoshM::
	trainerbattle_single TRAINER_ROGUE_POSH_M, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	msgbox Rogue_Battle_BreederF_Text_2, MSGBOX_AUTOCLOSE
	return


Rogue_Battle_MagmaF::
	trainerbattle_single TRAINER_ROGUE_MAGMA_F, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	msgbox Rogue_Battle_BreederF_Text_2, MSGBOX_AUTOCLOSE
	return


Rogue_Battle_MagmaM::
	trainerbattle_single TRAINER_ROGUE_MAGMA_M, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	msgbox Rogue_Battle_BreederF_Text_2, MSGBOX_AUTOCLOSE
	return


Rogue_Battle_AquaF::
	trainerbattle_single TRAINER_ROGUE_AQUA_F, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	msgbox Rogue_Battle_BreederF_Text_2, MSGBOX_AUTOCLOSE
	return


Rogue_Battle_AquaM::
	trainerbattle_single TRAINER_ROGUE_AQUA_M, Rogue_Battle_BreederF_Text_0, Rogue_Battle_BreederF_Text_1
	msgbox Rogue_Battle_BreederF_Text_2, MSGBOX_AUTOCLOSE
	return


Rogue_Give_Item0::
	finditem VAR_ROGUE_ITEM0, 3
	return


Rogue_Give_Item1::
	finditem VAR_ROGUE_ITEM1, 3
	return


Rogue_Battle_BreederF_Text_0:
	.string "Let's battle 0!$"

Rogue_Battle_BreederF_Text_1:
	.string "Oh shoot..$"

Rogue_Battle_BreederF_Text_2:
	.string "Good luck!$"
