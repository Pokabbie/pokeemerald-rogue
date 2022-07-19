Rogue_Boss_13_MapScripts::
	.byte 0


Rogue_Battle_Boss13::
	lockall
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, CameraToBattleFieldBoss13
	waitmovement 0
	msgbox Rogue_Battle_Boss13_Text_0
	applymovement 1, BossToFightBoss13
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToFightBoss13
	waitmovement 0
	playbgm MUS_ENCOUNTER_CHAMPION, FALSE
	msgbox Rogue_Battle_Boss13_Text_1
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, TRAINER_STEVEN, 0, Rogue_Battle_Boss13_Text_2
	call Rogue_BattleDefeated_Boss13
	return


Rogue_BattleDefeated_Boss13::
	setflag FLAG_IS_CHAMPION
	setrespawn HEAL_LOCATION_ROGUE_HUB
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

Rogue_Battle_Boss13_Text_0:
	.string "...\p"
	.string "...\p"
	.string "...\p"
	.string "STEVEN: You did well to make it here...\p"
	.string "Now...\p"
	.string "For one last time...$"

Rogue_Battle_Boss13_Text_1:
	.string "Let's battle!$"

Rogue_Battle_Boss13_Text_2:
	.string "Congratulations {PLAYER}!$"
