Rogue_Route_Field0_MapScripts::
	.byte 0


Rogue_Battle_Event_Obj_1::
	trainerbattle_single TRAINER_CINDY_1, Rogue_Battle_Event_Obj_1_Text_0, Rogue_Battle_Event_Obj_1_Text_1
	msgbox Rogue_Battle_Event_Obj_1_Text_2, MSGBOX_AUTOCLOSE
	return


Rogue_Battle_Event_Obj_1_Text_0:
	.string "Let's battle!$"

Rogue_Battle_Event_Obj_1_Text_1:
	.string "Oh shoot..$"

Rogue_Battle_Event_Obj_1_Text_2:
	.string "...$"
