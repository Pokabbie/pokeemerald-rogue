Rogue_Boss_12_MapScripts::
	.byte 0


Rogue_Battle_Boss12::
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToBattleFieldBoss12
	waitmovement 0
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, CameraToBattleFieldBoss12
	waitmovement 0
	compare VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, TRAINER_ROGUE_BOSS_RED
	goto_if_eq Rogue_Battle_Boss12_1
	buffertrainername STR_VAR_1, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA
	msgbox Rogue_Battle_Boss12_Text_3
	call Rogue_SetDesiredWeather
	applymovement 1, BossToFightBoss12
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToFightBoss12
	waitmovement 0
	playbgm MUS_ENCOUNTER_INTENSE, FALSE
	msgbox Rogue_Battle_Boss12_Text_4
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, 0, Rogue_Battle_Boss12_Text_5
	call Rogue_BattleDefeated_Boss12
	return

Rogue_Battle_Boss12_1:
	buffertrainername STR_VAR_1, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA
	msgbox Rogue_Battle_Boss12_Text_0
	call Rogue_SetDesiredWeather
	applymovement 1, BossToFightBoss12
	applymovement OBJ_EVENT_ID_PLAYER, PlayerToFightBoss12
	waitmovement 0
	playbgm MUS_ENCOUNTER_INTENSE, FALSE
	msgbox Rogue_Battle_Boss12_Text_1
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, 0, Rogue_Battle_Boss12_Text_2
	call Rogue_BattleDefeated_Boss12
	return


Rogue_BattleDefeated_Boss12::
	call Rogue_ClearWeather
	message Rogue_BattleDefeated_Boss12_Text_0
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	playbgm MUS_VICTORY_LEAGUE, FALSE
	buffertrainername STR_VAR_1, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA
	compare VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, TRAINER_ROGUE_BOSS_RED
	goto_if_eq Rogue_BattleDefeated_Boss12_2
	msgbox Rogue_BattleDefeated_Boss12_Text_3
	playbgm MUS_NONE, TRUE
	msgbox Rogue_BattleDefeated_Boss12_Text_4
Rogue_BattleDefeated_Boss12_1:
	waitmessage
	special HealPlayerParty
	special RemoveCameraObject
	warp MAP_ROGUE_HUB_TRANSITION, 0
	waitstate
	return

Rogue_BattleDefeated_Boss12_2:
	msgbox Rogue_BattleDefeated_Boss12_Text_1
	playbgm MUS_NONE, TRUE
	msgbox Rogue_BattleDefeated_Boss12_Text_2
	goto Rogue_BattleDefeated_Boss12_1


PlayerToBattleFieldBoss12:
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

CameraToBattleFieldBoss12:
	walk_up
	walk_up
	walk_up
	step_end

PlayerToFightBoss12:
	walk_up
	step_end

BossToFightBoss12:
	walk_down
	step_end

Rogue_Battle_Boss12_Text_0:
	.string "{STR_VAR_1}: ... ... ...$"

Rogue_Battle_Boss12_Text_1:
	.string "!$"

Rogue_Battle_Boss12_Text_2:
	.string "...!$"

Rogue_Battle_Boss12_Text_3:
	.string "{STR_VAR_1}: A new challenger!$"

Rogue_Battle_Boss12_Text_4:
	.string "Don't hold back now!$"

Rogue_Battle_Boss12_Text_5:
	.string "Well fought trainer!$"

Rogue_BattleDefeated_Boss12_Text_0:
	.string "{PLAYER} received a CHAMPION BADGE!$"

Rogue_BattleDefeated_Boss12_Text_1:
	.string "{STR_VAR_1}: ... ... ...\p"
	.string "... ... ...$"

Rogue_BattleDefeated_Boss12_Text_2:
	.string "... ... ...\p"
	.string "!$"

Rogue_BattleDefeated_Boss12_Text_3:
	.string "{STR_VAR_1}: Well fought\n"
	.string "{PLAYER}!\p"
	.string "You showed great skill and prowess, the\n"
	.string "way you battle alongside your POKéMON!\p"
	.string "And with that said...\p"
	.string "I hearby declare you..\n"
	.string "A POKéMON CHAMPION!$"

Rogue_BattleDefeated_Boss12_Text_4:
	.string "Is what I would've said...\p"
	.string "I'm afraid another battle awaits before\n"
	.string "you may be bestowed that title...\p"
	.string "Good luck!$"
