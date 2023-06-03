EverGrandeCity_ChampionsRoom_ApproachPause::
	applymovement OBJ_EVENT_ID_PLAYER, EverGrandeCity_ChampionsRoom_EventScript_Blue_ApproachPause
	waitmovement 0
	return


EverGrandeCity_ChampionsRoom_EventScript_Steven::
	playbgm MUS_ENCOUNTER_CHAMPION, FALSE
	msgbox EverGrandeCity_ChampionsRoom_EventScript_Steven_Text_0, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_STEVEN_CHAMPION, EverGrandeCity_ChampionsRoom_EventScript_Steven_Text_1
	goto EverGrandeCity_ChampionsRoom_EventScript_Defeated
	return


EverGrandeCity_ChampionsRoom_EventScript_WallaceRun::
	applymovement 5, EverGrandeCity_ChampionsRoom_EventScript_Wallace_RunAway
	return


EverGrandeCity_ChampionsRoom_EventScript_Blue::
	special HealUnfaintedPlayerParty
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
	random 6
	switch VAR_RESULT
	case 0, EverGrandeCity_ChampionsRoom_EventScript_Blue_3
	case 1, EverGrandeCity_ChampionsRoom_EventScript_Blue_4
	case 2, EverGrandeCity_ChampionsRoom_EventScript_Blue_5
	case 3, EverGrandeCity_ChampionsRoom_EventScript_Blue_6
	case 4, EverGrandeCity_ChampionsRoom_EventScript_Blue_7
	case 5, EverGrandeCity_ChampionsRoom_EventScript_Blue_8
EverGrandeCity_ChampionsRoom_EventScript_Blue_1:
	msgbox EverGrandeCity_ChampionsRoom_EventScript_Blue_Text_4
	applymovement 4, EverGrandeCity_ChampionsRoom_EventScript_Blue_Victory
	applymovement 1, EverGrandeCity_ChampionsRoom_EventScript_Steven_Victory
	playse SE_WALL_HIT
	waitmovement 0
	msgbox EverGrandeCity_ChampionsRoom_EventScript_Blue_Text_5
	return

EverGrandeCity_ChampionsRoom_EventScript_Blue_3:
	trainerbattle_no_intro TRAINER_BLUE_1, EverGrandeCity_ChampionsRoom_EventScript_Blue_Text_3
	goto EverGrandeCity_ChampionsRoom_EventScript_Blue_1

EverGrandeCity_ChampionsRoom_EventScript_Blue_4:
	trainerbattle_no_intro TRAINER_BLUE_2, EverGrandeCity_ChampionsRoom_EventScript_Blue_Text_3
	goto EverGrandeCity_ChampionsRoom_EventScript_Blue_1

EverGrandeCity_ChampionsRoom_EventScript_Blue_5:
	trainerbattle_no_intro TRAINER_BLUE_3, EverGrandeCity_ChampionsRoom_EventScript_Blue_Text_3
	goto EverGrandeCity_ChampionsRoom_EventScript_Blue_1

EverGrandeCity_ChampionsRoom_EventScript_Blue_6:
	trainerbattle_no_intro TRAINER_BLUE_4, EverGrandeCity_ChampionsRoom_EventScript_Blue_Text_3
	goto EverGrandeCity_ChampionsRoom_EventScript_Blue_1

EverGrandeCity_ChampionsRoom_EventScript_Blue_7:
	trainerbattle_no_intro TRAINER_BLUE_5, EverGrandeCity_ChampionsRoom_EventScript_Blue_Text_3
	goto EverGrandeCity_ChampionsRoom_EventScript_Blue_1

EverGrandeCity_ChampionsRoom_EventScript_Blue_8:
	trainerbattle_no_intro TRAINER_BLUE_6, EverGrandeCity_ChampionsRoom_EventScript_Blue_Text_3
	goto EverGrandeCity_ChampionsRoom_EventScript_Blue_1


EverGrandeCity_HallOfFame_EventScript_EnterHallOfFame_BlueBaby::
	return


EverGrandeCity_ChampionsRoom_EventScript_Blue_ApproachPause:
	face_left
	delay_16
	delay_16
	delay_16
	face_up
	delay_16
	delay_16
	delay_16
	face_left
	delay_16
	face_up
	delay_16
	face_left
	delay_16
	face_up
	delay_16
	delay_16
	step_end

EverGrandeCity_ChampionsRoom_EventScript_Blue_Approach1:
	walk_down
	step_end

EverGrandeCity_ChampionsRoom_EventScript_Blue_Approach2:
	walk_down
	step_end

EverGrandeCity_ChampionsRoom_EventScript_Wallace_Approach:
	walk_left
	step_end

EverGrandeCity_ChampionsRoom_EventScript_Wallace_RunAway:
	delay_16
	delay_16
	face_down
	delay_16
	delay_16
	emote_question_mark
	delay_16
	face_right
	delay_16
	face_left
	delay_16
	face_right
	delay_16
	face_left
	delay_16
	face_down
	delay_16
	walk_fast_right
	walk_fast_right
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	step_end

EverGrandeCity_ChampionsRoom_EventScript_Steven_Victory:
	walk_fast_right
	face_down
	step_end

EverGrandeCity_ChampionsRoom_EventScript_Blue_Victory:
	jump_2_right
	face_right
	emote_exclamation_mark
	delay_16
	delay_16
	walk_fast_left
	delay_16
	delay_16
	delay_16
	walk_slow_up
	walk_slow_left
	walk_slow_up
	walk_slow_up
	set_invisible
	step_end

EverGrandeCity_ChampionsRoom_EventScript_Steven_Text_0:
	.string "STEVEN: Ahem…\p"
	.string "{PLAYER}{KUN}!\p"
	.string "Welcome!\p"
	.string "WALLACE?\p"
	.string "He and I just had just had a little\n"
	.string "disagreement, don't mind him.\p"
	.string "You see {PLAYER}{KUN}, much like you, I am a\n"
	.string "trainer of great passion!\p"
	.string "For many years I held the title of\n"
	.string "CHAMPION.\p"
	.string "WALLACE merely filled in during my\n"
	.string "absence.\p"
	.string "And now…\n"
	.string "I see the fire in your eyes.\p"
	.string "If you wish to take the title from\n"
	.string "HOENN's true CHAMPION…\p"
	.string "Come at me with your full force!$"

EverGrandeCity_ChampionsRoom_EventScript_Steven_Text_1:
	.string "I, the true HOENN CHAMPION, fall in\n"
	.string "defeat…\p"
	.string "I possess nothing but respect for you,\n"
	.string "{PLAYER}{KUN}!\p"
	.string "You are a truly noble POKéMON TRAINER!$"

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
	.string "STEVEN: As I was saying…\p"
	.string "I now proclaim you to be the new\n"
	.string "HOENN region…$"
