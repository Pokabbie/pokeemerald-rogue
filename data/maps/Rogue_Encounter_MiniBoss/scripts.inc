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
	switch VAR_ROGUE_SPECIAL_ENCOUNTER_DATA
	case TRAINER_ROGUE_MINI_BOSS_MAXIE, Rogue_BattleDefeated_GiveMon_3
	case TRAINER_ROGUE_MINI_BOSS_ARCHIE, Rogue_BattleDefeated_GiveMon_3
	case TRAINER_ROGUE_MINI_BOSS_MIRROR, Rogue_BattleDefeated_GiveMon_4
	message Rogue_BattleDefeated_GiveMon_Text_2
Rogue_BattleDefeated_GiveMon_1:
	waitmessage
	bufferspeciesname STR_VAR_1, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1
	bufferspeciesname STR_VAR_2, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2
	multichoice 17, 6, MULTI_EXPAND2_EXIT, FALSE
	switch VAR_RESULT
	case 0, Rogue_BattleDefeated_GiveMon_8
	case 1, Rogue_BattleDefeated_GiveMon_9
	case MULTI_B_PRESSED, Rogue_BattleDefeated_GiveMon_10
Rogue_BattleDefeated_GiveMon_6:
	compare VAR_RESULT, YES
	goto_if_eq Rogue_BattleDefeated_GiveMon_11
	msgbox Rogue_BattleDefeated_GiveMon_Text_5, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Rogue_BattleDefeated_GiveMon_16
	return

Rogue_BattleDefeated_GiveMon_3:
	message Rogue_BattleDefeated_GiveMon_Text_0
	goto Rogue_BattleDefeated_GiveMon_1

Rogue_BattleDefeated_GiveMon_4:
	message Rogue_BattleDefeated_GiveMon_Text_1
	goto Rogue_BattleDefeated_GiveMon_1

Rogue_BattleDefeated_GiveMon_8:
	copyvar VAR_TEMP_0, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1
	call Rogue_MiniBoss_CheckPartySize
	goto Rogue_BattleDefeated_GiveMon_6

Rogue_BattleDefeated_GiveMon_9:
	copyvar VAR_TEMP_0, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2
	call Rogue_MiniBoss_CheckPartySize
	goto Rogue_BattleDefeated_GiveMon_6

Rogue_BattleDefeated_GiveMon_10:
	setvar VAR_RESULT, NO
	goto Rogue_BattleDefeated_GiveMon_6

Rogue_BattleDefeated_GiveMon_11:
	givemon VAR_TEMP_0, 1
	bufferspeciesname STR_VAR_1, VAR_TEMP_0
	playfanfare MUS_OBTAIN_ITEM
	message Rogue_BattleDefeated_GiveMon_Text_3
	waitmessage
	waitfanfare
	msgbox Rogue_BattleDefeated_GiveMon_Text_4, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_BattleDefeated_GiveMon_14
	return

Rogue_BattleDefeated_GiveMon_14:
	call Common_EventScript_GetGiftMonPartySlot
	call Common_EventScript_NameReceivedPartyMon
	return

Rogue_BattleDefeated_GiveMon_16:
	call Rogue_BattleDefeated_GiveMon
	return


Rogue_BattleDefeated_MiniBoss::
	call Rogue_ClearWeather
	switch VAR_ROGUE_SPECIAL_ENCOUNTER_DATA
	case TRAINER_ROGUE_MINI_BOSS_MAXIE, Rogue_BattleDefeated_MiniBoss_3
	case TRAINER_ROGUE_MINI_BOSS_ARCHIE, Rogue_BattleDefeated_MiniBoss_3
	case TRAINER_ROGUE_MINI_BOSS_MIRROR, Rogue_BattleDefeated_MiniBoss_4
	msgbox Rogue_BattleDefeated_MiniBoss_Text_2, MSGBOX_DEFAULT
Rogue_BattleDefeated_MiniBoss_1:
	copyvar VAR_0x8001, VAR_ROGUE_ITEM0
	call Rogue_GiveItem_Internal
	special Rogue_ChooseMiniBossRewardMons
	compare VAR_ROGUE_ACTIVE_CAMPAIGN, ROGUE_CAMPAIGN_POKEBALL_LIMIT
	goto_if_ne Rogue_BattleDefeated_MiniBoss_7
Rogue_BattleDefeated_MiniBoss_6:
	special HealPlayerParty
	special RemoveCameraObject
	warp MAP_ROGUE_HUB_TRANSITION, 0
	waitstate
	release
	return

Rogue_BattleDefeated_MiniBoss_3:
	msgbox Rogue_BattleDefeated_MiniBoss_Text_0, MSGBOX_DEFAULT
	goto Rogue_BattleDefeated_MiniBoss_1

Rogue_BattleDefeated_MiniBoss_4:
	msgbox Rogue_BattleDefeated_MiniBoss_Text_1, MSGBOX_DEFAULT
	goto Rogue_BattleDefeated_MiniBoss_1

Rogue_BattleDefeated_MiniBoss_7:
	call Rogue_BattleDefeated_GiveMon
	goto Rogue_BattleDefeated_MiniBoss_6


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
	switch VAR_ROGUE_SPECIAL_ENCOUNTER_DATA
	case TRAINER_ROGUE_MINI_BOSS_MAXIE, Rogue_MiniBoss_KickOffBattle_2
	case TRAINER_ROGUE_MINI_BOSS_ARCHIE, Rogue_MiniBoss_KickOffBattle_2
	case TRAINER_ROGUE_MINI_BOSS_WALLY, Rogue_MiniBoss_KickOffBattle_3
	case TRAINER_ROGUE_MINI_BOSS_MIRROR, Rogue_MiniBoss_KickOffBattle_4
	case TRAINER_ROGUE_MINI_BOSS_RIVAL, Rogue_MiniBoss_KickOffBattle_5
	msgbox Rogue_MiniBoss_KickOffBattle_Text_8
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, 0, Rogue_MiniBoss_KickOffBattle_Text_9
	return

Rogue_MiniBoss_KickOffBattle_2:
	msgbox Rogue_MiniBoss_KickOffBattle_Text_0
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, 0, Rogue_MiniBoss_KickOffBattle_Text_1
	return

Rogue_MiniBoss_KickOffBattle_3:
	msgbox Rogue_MiniBoss_KickOffBattle_Text_2
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, 0, Rogue_MiniBoss_KickOffBattle_Text_3
	return

Rogue_MiniBoss_KickOffBattle_4:
	msgbox Rogue_MiniBoss_KickOffBattle_Text_4
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, 0, Rogue_MiniBoss_KickOffBattle_Text_5
	return

Rogue_MiniBoss_KickOffBattle_5:
	msgbox Rogue_MiniBoss_KickOffBattle_Text_6
	trainerbattle TRAINER_BATTLE_SINGLE_NO_INTRO_TEXT, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, 0, Rogue_MiniBoss_KickOffBattle_Text_7
	return


Rogue_MiniBoss_CheckPartySize_Text_0:
	.string "It looks like you're going to need to\n"
	.string "release a POKéMON to make room.$"

Rogue_BattleDefeated_GiveMon_Text_0:
	.string "Will you take one of these POKéMON with\n"
	.string "you?$"

Rogue_BattleDefeated_GiveMon_Text_1:
	.string "... ... ...!$"

Rogue_BattleDefeated_GiveMon_Text_2:
	.string "Would you like to take one of these\n"
	.string "POKéMON with you?$"

Rogue_BattleDefeated_GiveMon_Text_3:
	.string "{PLAYER} obtained a {STR_VAR_1}.$"

Rogue_BattleDefeated_GiveMon_Text_4:
	.string "Would you like to give {STR_VAR_1} a\n"
	.string "nickname?$"

Rogue_BattleDefeated_GiveMon_Text_5:
	.string "Are you sure?$"

Rogue_BattleDefeated_MiniBoss_Text_0:
	.string "I suppose I should give you some\n"
	.string "rewards for your lucky performance...$"

Rogue_BattleDefeated_MiniBoss_Text_1:
	.string "... ... ...?$"

Rogue_BattleDefeated_MiniBoss_Text_2:
	.string "I've got a few rewards for you.$"

Rogue_MiniBoss_KickOffBattle_Text_0:
	.string "{STR_VAR_1}: Don't think I'll let you\n"
	.string "through lightly!$"

Rogue_MiniBoss_KickOffBattle_Text_1:
	.string "Well that was unexpected...$"

Rogue_MiniBoss_KickOffBattle_Text_2:
	.string "{STR_VAR_1}: Oh hey!\n"
	.string "Will you battle with me!$"

Rogue_MiniBoss_KickOffBattle_Text_3:
	.string "What a fun battle!$"

Rogue_MiniBoss_KickOffBattle_Text_4:
	.string "{PLAYER}: ... ... ...$"

Rogue_MiniBoss_KickOffBattle_Text_5:
	.string "...!$"

Rogue_MiniBoss_KickOffBattle_Text_6:
	.string "{STR_VAR_1}: I've think I've crafted\n"
	.string "the best team! Let me show you!$"

Rogue_MiniBoss_KickOffBattle_Text_7:
	.string "Wow! I didn't expect that!$"

Rogue_MiniBoss_KickOffBattle_Text_8:
	.string "{STR_VAR_1}: Show me everything you\n"
	.string "got!$"

Rogue_MiniBoss_KickOffBattle_Text_9:
	.string "Well colour me impressed!$"
