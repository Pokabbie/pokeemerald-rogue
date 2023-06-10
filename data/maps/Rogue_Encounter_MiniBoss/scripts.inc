Rogue_Encounter_MiniBoss_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Rogue_MiniBoss_OnLoad
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, Rogue_Encounter_MiniBoss_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE
	.byte 0

Rogue_Encounter_MiniBoss_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE:
	map_script_2 VAR_TEMP_1, 0, Rogue_Common_TurnPlayerEast
	.2byte 0


Rogue_MiniBoss_OnLoad::
	call Rogue_SetDesiredWeather
	return


Rogue_Battle_MiniBoss::
	lock
	applymovement OBJ_EVENT_ID_PLAYER, MiniBoss_PlayerToBattleField
	waitmovement 0
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, MiniBoss_CameraToBattleField
	waitmovement 0
	call Rogue_MiniBoss_IntroSpeech
	applymovement 1, MiniBoss_BossToFight
	applymovement OBJ_EVENT_ID_PLAYER, MiniBoss_PlayerToFight
	waitmovement 0
	playbgm MUS_ENCOUNTER_INTENSE, FALSE
	call Rogue_MiniBoss_KickOffBattle
	call Rogue_BattleDefeated_MiniBoss
	return


Rogue_MiniBoss_CheckPartySize::
	specialvar VAR_TEMP_1, CalculatePlayerPartyCount
	specialvar VAR_TEMP_2, Rogue_CalcMaxPartySize
	compare VAR_TEMP_1, VAR_TEMP_2
	goto_if_ge Rogue_MiniBoss_CheckPartySize_2
Rogue_MiniBoss_CheckPartySize_1:
	setvar VAR_RESULT, YES
	return

Rogue_MiniBoss_CheckPartySize_2:
	msgbox Rogue_MiniBoss_CheckPartySize_Text_0
	special ChoosePartyMon
	waitstate
	compare VAR_0x8004, PARTY_NOTHING_CHOSEN
	goto_if_eq Rogue_MiniBoss_CheckPartySize_4
	special Rogue_ReleaseMonInSlot
	goto Rogue_MiniBoss_CheckPartySize_1

Rogue_MiniBoss_CheckPartySize_4:
	setvar VAR_RESULT, NO
	return


Rogue_BattleDefeated_GiveMon::
	message Rogue_BattleDefeated_GiveMon_Text_0
	waitmessage
	bufferspeciesname STR_VAR_1, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1
	bufferspeciesname STR_VAR_2, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2
	multichoice 17, 6, MULTI_EXPAND2_EXIT, FALSE
	switch VAR_RESULT
	case 0, Rogue_BattleDefeated_GiveMon_3
	case 1, Rogue_BattleDefeated_GiveMon_4
	case MULTI_B_PRESSED, Rogue_BattleDefeated_GiveMon_5
Rogue_BattleDefeated_GiveMon_1:
	compare VAR_RESULT, YES
	goto_if_eq Rogue_BattleDefeated_GiveMon_6
	msgbox Rogue_BattleDefeated_GiveMon_Text_3, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Rogue_BattleDefeated_GiveMon_11
	return

Rogue_BattleDefeated_GiveMon_3:
	copyvar VAR_TEMP_0, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1
	call Rogue_MiniBoss_CheckPartySize
	goto Rogue_BattleDefeated_GiveMon_1

Rogue_BattleDefeated_GiveMon_4:
	copyvar VAR_TEMP_0, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2
	call Rogue_MiniBoss_CheckPartySize
	goto Rogue_BattleDefeated_GiveMon_1

Rogue_BattleDefeated_GiveMon_5:
	setvar VAR_RESULT, NO
	goto Rogue_BattleDefeated_GiveMon_1

Rogue_BattleDefeated_GiveMon_6:
	givemon VAR_TEMP_0, 1
	bufferspeciesname STR_VAR_1, VAR_TEMP_0
	playfanfare MUS_OBTAIN_ITEM
	message Rogue_BattleDefeated_GiveMon_Text_1
	waitmessage
	waitfanfare
	msgbox Rogue_BattleDefeated_GiveMon_Text_2, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_BattleDefeated_GiveMon_9
	return

Rogue_BattleDefeated_GiveMon_9:
	call Common_EventScript_GetGiftMonPartySlot
	call Common_EventScript_NameReceivedPartyMon
	return

Rogue_BattleDefeated_GiveMon_11:
	call Rogue_BattleDefeated_GiveMon
	return


Rogue_BattleDefeated_MiniBoss::
	call Rogue_ClearWeather
	copyvar VAR_0x8001, VAR_ROGUE_ITEM0
	call Rogue_GiveItem_Internal
	special Rogue_ChooseMiniBossRewardMons
	compare VAR_ROGUE_ACTIVE_CAMPAIGN, ROGUE_CAMPAIGN_POKEBALL_LIMIT
	goto_if_ne Rogue_BattleDefeated_MiniBoss_2
Rogue_BattleDefeated_MiniBoss_1:
	special HealPlayerParty
	special RemoveCameraObject
	warp MAP_ROGUE_HUB_TRANSITION, 0
	waitstate
	release
	return

Rogue_BattleDefeated_MiniBoss_2:
	call Rogue_BattleDefeated_GiveMon
	goto Rogue_BattleDefeated_MiniBoss_1


MiniBoss_PlayerToBattleField:
	walk_right
	step_end

MiniBoss_CameraToBattleField:
	walk_right
	walk_right
	walk_right
	step_end

MiniBoss_PlayerToFight:
	walk_right
	face_right
	step_end

MiniBoss_BossToFight:
	walk_left
	step_end

Rogue_MiniBoss_IntroSpeech::
	return


Rogue_MiniBoss_KickOffBattle::
	buffertrainername STR_VAR_1, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA
	msgbox Rogue_MiniBoss_KickOffBattle_Text_0
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, 0, Rogue_MiniBoss_KickOffBattle_Text_1
	return


Rogue_MiniBoss_CheckPartySize_Text_0:
	.string "It looks like you're going to need to\n"
	.string "release a POKéMON to make room.$"

Rogue_BattleDefeated_GiveMon_Text_0:
	.string "Which of these POKéMON would you like\n"
	.string "to take with you?$"

Rogue_BattleDefeated_GiveMon_Text_1:
	.string "{PLAYER} obtained a {STR_VAR_1}.$"

Rogue_BattleDefeated_GiveMon_Text_2:
	.string "Would you like to give {STR_VAR_1} a\n"
	.string "nickname?$"

Rogue_BattleDefeated_GiveMon_Text_3:
	.string "Are you sure?$"

Rogue_MiniBoss_KickOffBattle_Text_0:
	.string "{STR_VAR_1}: Show me everything you\n"
	.string "got!$"

Rogue_MiniBoss_KickOffBattle_Text_1:
	.string "Well colour me impressed!$"
