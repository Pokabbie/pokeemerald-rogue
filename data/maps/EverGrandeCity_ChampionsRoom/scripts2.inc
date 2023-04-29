EverGrandeCity_ChampionsRoom_EventScript_Blue::
	playbgm MUS_NONE, FALSE
	msgbox EverGrandeCity_ChampionsRoom_EventScript_Blue_Text_0
	playse SE_EXIT
	addobject 4
	setobjectxy 4, 6, 3
	waitse
	playbgm MUS_RG_ENCOUNTER_RIVAL, FALSE
	msgbox EverGrandeCity_ChampionsRoom_EventScript_Blue_Text_1
	applymovement 4, EverGrandeCity_ChampionsRoom_EventScript_Blue_Approach1
	waitmovement 0
	applymovement 4, EverGrandeCity_ChampionsRoom_EventScript_Blue_Approach2
	applymovement 1, EverGrandeCity_ChampionsRoom_EventScript_Wallace_Approach
	waitmovement 0
	msgbox EverGrandeCity_ChampionsRoom_EventScript_Blue_Text_2
	trainerbattle_no_intro TRAINER_BLUE, EverGrandeCity_ChampionsRoom_EventScript_Blue_Text_3
	msgbox EverGrandeCity_ChampionsRoom_EventScript_Blue_Text_4
	applymovement 4, EverGrandeCity_ChampionsRoom_EventScript_Blue_Victory
	applymovement 1, EverGrandeCity_ChampionsRoom_EventScript_Wallace_Victory
	playse SE_WALL_HIT
	waitmovement 0
	msgbox EverGrandeCity_ChampionsRoom_EventScript_Blue_Text_5
	return


EverGrandeCity_ChampionsRoom_EventScript_Blue_Approach1:
	walk_down
	step_end

EverGrandeCity_ChampionsRoom_EventScript_Blue_Approach2:
	walk_down
	step_end

EverGrandeCity_ChampionsRoom_EventScript_Wallace_Approach:
	walk_left
	step_end

EverGrandeCity_ChampionsRoom_EventScript_Wallace_Victory:
	walk_fast_right
	face_down
	step_end

EverGrandeCity_ChampionsRoom_EventScript_Blue_Victory:
	jump_2_right
	step_end

EverGrandeCity_ChampionsRoom_EventScript_Blue_Text_0:
	.string "Hold it right there!$"

EverGrandeCity_ChampionsRoom_EventScript_Blue_Text_1:
	.string "No way is this dork the new Champion!$"

EverGrandeCity_ChampionsRoom_EventScript_Blue_Text_2:
	.string "Not whilst I'm still standing!$"

EverGrandeCity_ChampionsRoom_EventScript_Blue_Text_3:
	.string "No way…\n"
	.string "This can't be!$"

EverGrandeCity_ChampionsRoom_EventScript_Blue_Text_4:
	.string "… … …\n"
	.string "I can't believe it…\n"
	.string "I can't believe I lost… … …\n"
	.string "Again… … …$"

EverGrandeCity_ChampionsRoom_EventScript_Blue_Text_5:
	.string "WALLACE: As I was saying…\p"
	.string "I now proclaim you to be the new\n"
	.string "HOENN region…$"
