Sandbox_Trainer_Derp::
	trainerbattle_single TRAINER_VIEWIER_DERP, Sandbox_Trainer_Derp_Text_0, Sandbox_Trainer_Derp_Text_1
	msgbox Sandbox_Trainer_Derp_Text_2, MSGBOX_AUTOCLOSE
	return


Sandbox_Trainer_Mawile::
	trainerbattle_single TRAINER_VIEWIER_MAWILE, Sandbox_Trainer_Mawile_Text_0, Sandbox_Trainer_Mawile_Text_1
	msgbox Sandbox_Trainer_Mawile_Text_2, MSGBOX_AUTOCLOSE
	return


Sandbox_Trainer_Greez::
	trainerbattle_single TRAINER_VIEWIER_GREEZ, Sandbox_Trainer_Greez_Text_0, Sandbox_Trainer_Greez_Text_1
	msgbox Sandbox_Trainer_Greez_Text_2, MSGBOX_AUTOCLOSE
	return


Sandbox_Trainer_Dolphin::
	trainerbattle_single TRAINER_VIEWIER_DOLPHIN, Sandbox_Trainer_Dolphin_Text_0, Sandbox_Trainer_Dolphin_Text_1
	msgbox Sandbox_Trainer_Dolphin_Text_2, MSGBOX_AUTOCLOSE
	return


Sandbox_Trainer_Wall::
	trainerbattle_single TRAINER_VIEWIER_WALL, Sandbox_Trainer_Wall_Text_0, Sandbox_Trainer_Wall_Text_1
	msgbox Sandbox_Trainer_Wall_Text_2, MSGBOX_AUTOCLOSE
	return


Sandbox_Trainer_Alex::
	trainerbattle_single TRAINER_VIEWIER_ALEX, Sandbox_Trainer_Alex_Text_0, Sandbox_Trainer_Alex_Text_1
	msgbox Sandbox_Trainer_Alex_Text_2, MSGBOX_AUTOCLOSE
	return


Sandbox_Trainer_Gmaf::
	trainerbattle_single TRAINER_VIEWIER_GMAF, Sandbox_Trainer_Gmaf_Text_0, Sandbox_Trainer_Gmaf_Text_1
	msgbox Sandbox_Trainer_Gmaf_Text_2, MSGBOX_AUTOCLOSE
	return


Sandbox_Trainer_Pokabbie::
	trainerbattle_single TRAINER_VIEWIER_POKABBIE, Sandbox_Trainer_Pokabbie_Text_0, Sandbox_Trainer_Pokabbie_Text_1
	bufferitemname STR_VAR_1, ITEM_WHIPPED_DREAM
	msgbox Sandbox_Trainer_Pokabbie_Text_2, MSGBOX_AUTOCLOSE
	return


Sandbox_Trainer_RoxanneGhost::
	lock
	faceplayer
	playbgm MUS_ENCOUNTER_INTENSE, FALSE
	msgbox Sandbox_Trainer_RoxanneGhost_Text_0
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq Sandbox_Trainer_RoxanneGhost_2
	compare VAR_RESULT, FEMALE
	goto_if_eq Sandbox_Trainer_RoxanneGhost_3
Sandbox_Trainer_RoxanneGhost_1:
	setflag FLAG_HIDE_ROXANNE_GHOST
	removeobject 17, MAP_RUSTBORO_CITY
	msgbox Sandbox_Trainer_RoxanneGhost_Text_2
	release
	return

Sandbox_Trainer_RoxanneGhost_2:
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, TRAINER_STREAMER_GHOST_ROXANNE_M, 0, Sandbox_Trainer_RoxanneGhost_Text_1
	goto Sandbox_Trainer_RoxanneGhost_1

Sandbox_Trainer_RoxanneGhost_3:
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, TRAINER_STREAMER_GHOST_ROXANNE_F, 0, Sandbox_Trainer_RoxanneGhost_Text_1
	goto Sandbox_Trainer_RoxanneGhost_1


Sandbox_Trainer_NormanGhostInternal::
	playbgm MUS_ENCOUNTER_INTENSE, FALSE
	msgbox Sandbox_Trainer_NormanGhostInternal_Text_0
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, TRAINER_VIEWIER_DOLPHIN, 0, Sandbox_Trainer_NormanGhostInternal_Text_1
	setflag FLAG_HIDE_NORMAN_GHOST
	removeobject 12, MAP_PETALBURG_CITY_GYM
	msgbox Sandbox_Trainer_NormanGhostInternal_Text_2
	return


Sandbox_Trainer_NormanGhost::
	lock
	faceplayer
	call Sandbox_Trainer_NormanGhostInternal
	release
	return


Sandbox_Trainer_NormanGhost_ScriptLeft::
	lock
	goto_if_unset FLAG_HIDE_NORMAN_GHOST, Sandbox_Trainer_NormanGhost_ScriptLeft_2
Sandbox_Trainer_NormanGhost_ScriptLeft_1:
	release
	return

Sandbox_Trainer_NormanGhost_ScriptLeft_2:
	lock
	applymovement 12, GhostFaceLeft
	waitmovement 0
	call Sandbox_Trainer_NormanGhostInternal
	goto Sandbox_Trainer_NormanGhost_ScriptLeft_1


Sandbox_Trainer_NormanGhost_ScriptRight::
	lock
	goto_if_unset FLAG_HIDE_NORMAN_GHOST, Sandbox_Trainer_NormanGhost_ScriptRight_2
Sandbox_Trainer_NormanGhost_ScriptRight_1:
	release
	return

Sandbox_Trainer_NormanGhost_ScriptRight_2:
	applymovement 12, GhostFaceRight
	waitmovement 0
	call Sandbox_Trainer_NormanGhostInternal
	goto Sandbox_Trainer_NormanGhost_ScriptRight_1


Sandbox_EventScript_SetupMirrorGfxId::
	checkplayergender
	compare VAR_RESULT, MALE
	goto_if_eq Sandbox_EventScript_SetupMirrorGfxId_2
	compare VAR_RESULT, FEMALE
	goto_if_eq Sandbox_EventScript_SetupMirrorGfxId_3
Sandbox_EventScript_SetupMirrorGfxId_1:
	return

Sandbox_EventScript_SetupMirrorGfxId_2:
	setvar VAR_OBJ_GFX_ID_1, OBJ_EVENT_GFX_RIVAL_BRENDAN_NORMAL
	goto Sandbox_EventScript_SetupMirrorGfxId_1

Sandbox_EventScript_SetupMirrorGfxId_3:
	setvar VAR_OBJ_GFX_ID_1, OBJ_EVENT_GFX_RIVAL_MAY_NORMAL
	goto Sandbox_EventScript_SetupMirrorGfxId_1


GhostFaceRight:
	walk_up
	face_right
	step_end

GhostFaceLeft:
	walk_up
	face_left
	step_end

Sandbox_Trainer_Derp_Text_0:
	.string "Fancy seeing you here Conner.\p"
	.string "Going to awaken RAYQUAZA?\p"
	.string "NOT ON MY WATCH!$"

Sandbox_Trainer_Derp_Text_1:
	.string "Seems I’ve been defeated by the\n"
	.string "Legendary LaterManner.\p"
	.string "Now...\p"
	.string "Go win this.\n"
	.string "You deserve it, champ$"

Sandbox_Trainer_Derp_Text_2:
	.string "DERPSTER: Go win this.\n"
	.string "You deserve it, champ$"

Sandbox_Trainer_Mawile_Text_0:
	.string "You thought I was a grunt, but it was I,\n"
	.string "MAWEELAY!$"

Sandbox_Trainer_Mawile_Text_1:
	.string "Why couldn't you do that to Blue?$"

Sandbox_Trainer_Mawile_Text_2:
	.string "MAWEELAY: Why couldn't you do that to\n"
	.string "Blue?$"

Sandbox_Trainer_Greez_Text_0:
	.string "I was about to stal- I mean see the\n"
	.string "female grunt.\p"
	.string "Wanna battle?$"

Sandbox_Trainer_Greez_Text_1:
	.string "But like… The female grunt tho.$"

Sandbox_Trainer_Greez_Text_2:
	.string "GREEZ: But like… The female grunt tho.$"

Sandbox_Trainer_Dolphin_Text_0:
	.string "I swear I can solve this puzzle first\n"
	.string "try!$"

Sandbox_Trainer_Dolphin_Text_1:
	.string "I failed the puzzle…$"

Sandbox_Trainer_Dolphin_Text_2:
	.string "DOLPHIN: I failed the puzzle…$"

Sandbox_Trainer_Wall_Text_0:
	.string "Look what stumbled into my net?$"

Sandbox_Trainer_Wall_Text_1:
	.string "There goes my catch$"

Sandbox_Trainer_Wall_Text_2:
	.string "WALL: There goes my catch$"

Sandbox_Trainer_Alex_Text_0:
	.string "One of us is going to Pokemon Center\n"
	.string "and it is not me!$"

Sandbox_Trainer_Alex_Text_1:
	.string "NOOOO MY LOVELY LOPUNNY!!!!$"

Sandbox_Trainer_Alex_Text_2:
	.string "ALEX: NOOOO MY LOVELY LOPUNNY!!!!$"

Sandbox_Trainer_Gmaf_Text_0:
	.string "Having fun? Time to crank it up a notch.$"

Sandbox_Trainer_Gmaf_Text_1:
	.string "Damn, i see you big dog!$"

Sandbox_Trainer_Gmaf_Text_2:
	.string "GMAF: Damn, i see you big dog!$"

Sandbox_Trainer_Pokabbie_Text_0:
	.string "You think I'd pass up the opportunity\n"
	.string "to add myself? Pfft!$"

Sandbox_Trainer_Pokabbie_Text_1:
	.string "You have to admit…\n"
	.string "Mareep is pretty cool!$"

Sandbox_Trainer_Pokabbie_Text_2:
	.string "POKABBIE: Did you know you can give\n"
	.string "your Mareep the same capabilities as\l"
	.string "Pikin by using a {STR_VAR_1} on it.\p"
	.string "Pikin has the same base stats and\n"
	.string "Ampharos but it now has to some extra\l"
	.string "coverage moves along with the\l"
	.string "Lightning Rod ability.\p"
	.string "If it knows Meteor Beam, it can also\n"
	.string "mega evolve into it's ideal floating\l"
	.string "fluffy from!\p"
	.string "When mega evolved it has the same base\n"
	.string "stats as Mega Ampharos, but it has\l"
	.string "Eletric Steel typing with the ability\l"
	.string "Levitate.\p"
	.string "It's pretty cool, no?$"

Sandbox_Trainer_RoxanneGhost_Text_0:
	.string "Let's gooooo!$"

Sandbox_Trainer_RoxanneGhost_Text_1:
	.string "I MISS CLICKED!$"

Sandbox_Trainer_RoxanneGhost_Text_2:
	.string "The ghost of past runs faded away…$"

Sandbox_Trainer_NormanGhostInternal_Text_0:
	.string "I'M A SWIMMER!\p"
	.string "WHY THE FRICK AM I AT THE NORMAL\n"
	.string "GYM???$"

Sandbox_Trainer_NormanGhostInternal_Text_1:
	.string "Where's the nearest ocean…?$"

Sandbox_Trainer_NormanGhostInternal_Text_2:
	.string "Dolphin ran away to fail another\n"
	.string "puzzle…$"
