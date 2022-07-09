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

Rogue_Battle_Trainer0::
	trainerbattle_single TRAINER_CINDY_1, Rogue_Battle_Trainer0_Text_0, Rogue_Battle_Trainer0_Text_1
	msgbox Rogue_Battle_Trainer0_Text_2, MSGBOX_AUTOCLOSE
	return


Rogue_Battle_Trainer1::
	trainerbattle_single TRAINER_DAPHNE, Rogue_Battle_Trainer1_Text_0, Rogue_Battle_Trainer0_Text_1
	msgbox Rogue_Battle_Trainer1_Text_1, MSGBOX_AUTOCLOSE
	return


Rogue_Give_Item0::
	finditem VAR_ROGUE_ITEM0, 3
	return


Rogue_Give_Item1::
	finditem VAR_ROGUE_ITEM1, 3
	return


Rogue_Battle_Trainer0_Text_0:
	.string "Let's battle 0!$"

Rogue_Battle_Trainer0_Text_1:
	.string "Oh shoot..$"

Rogue_Battle_Trainer0_Text_2:
	.string "... 0$"

Rogue_Battle_Trainer1_Text_0:
	.string "Let's battle 1!$"

Rogue_Battle_Trainer1_Text_1:
	.string "... 1$"
