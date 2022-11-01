Rogue_Boss_13_MapScripts::
	.byte 0


Rogue_Battle_Boss13::
	lockall
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, CameraToBattleFieldBoss13
	waitmovement 0
	compare VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, TRAINER_ROGUE_BOSS_RED
	goto_if_eq Rogue_Battle_Boss13_2
	compare VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, TRAINER_ROGUE_BOSS_POKABBIE
	goto_if_eq Rogue_Battle_Boss13_3
	buffertrainername STR_VAR_1, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA
	msgbox Rogue_Battle_Boss13_Text_6
	call Rogue_SetDesiredWeather
	applymovement 1, BossToFightBoss13
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToFightBoss13
	waitmovement 0
	msgbox Rogue_Battle_Boss13_Text_7
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, 0, Rogue_Battle_Boss13_Text_5
Rogue_Battle_Boss13_1:
	call Rogue_BattleDefeated_Boss13
	return

Rogue_Battle_Boss13_2:
	msgbox Rogue_Battle_Boss13_Text_0
	call Rogue_SetDesiredWeather
	applymovement 1, BossToFightBoss13
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToFightBoss13
	waitmovement 0
	msgbox Rogue_Battle_Boss13_Text_1
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, 0, Rogue_Battle_Boss13_Text_2
	goto Rogue_Battle_Boss13_1

Rogue_Battle_Boss13_3:
	buffertrainername STR_VAR_1, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA
	msgbox Rogue_Battle_Boss13_Text_3
	call Rogue_SetDesiredWeather
	applymovement 1, BossToFightBoss13
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToFightBoss13
	waitmovement 0
	msgbox Rogue_Battle_Boss13_Text_4
	playmoncry SPECIES_DITTO, CRY_MODE_ENCOUNTER
	waitmoncry
	applymovement 2, DittoToFightBoss13
	waitmovement 0
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, 0, Rogue_Battle_Boss13_Text_5
	goto Rogue_Battle_Boss13_1


Rogue_BattleDefeated_Boss13::
	fadescreen FADE_TO_BLACK
	special GameClear
	waitstate
	releaseall
	return


CameraToBattleFieldBoss13:
	walk_up
	walk_up
	walk_up
	step_end

PlayerToFightBoss13:
	walk_up
	step_end

BossToFightBoss13:
	face_down
	step_end

DittoToFightBoss13:
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	step_end

Rogue_Battle_Boss13_Text_0:
	.string "...\p"
	.string "...\p"
	.string "...$"

Rogue_Battle_Boss13_Text_1:
	.string "!$"

Rogue_Battle_Boss13_Text_2:
	.string "...!$"

Rogue_Battle_Boss13_Text_3:
	.string "...\p"
	.string "...\p"
	.string "...\p"
	.string "POKABBIE: You did well to make it here...$"

Rogue_Battle_Boss13_Text_4:
	.string "Oh!\n"
	.string "It's {PLAYER}!\p"
	.string "My bad, I thought you were someone\n"
	.string "else.\p"
	.string "What's that?\p"
	.string "We should battle?\p"
	.string "Don't be silly!\p"
	.string "I'm not the one you'll be battling...$"

Rogue_Battle_Boss13_Text_5:
	.string "Congratulations {PLAYER}!$"

Rogue_Battle_Boss13_Text_6:
	.string "...\p"
	.string "...\p"
	.string "...\p"
	.string "{STR_VAR_1}: You did well to make it\n"
	.string "here...\p"
	.string "Now...\p"
	.string "For one last time...$"

Rogue_Battle_Boss13_Text_7:
	.string "Let's battle!$"
