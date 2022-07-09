Rogue_Hub_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, RogueHub_ResetRun
	.byte 0


RogueHub_TrainerBattle::
	trainerbattle_single TRAINER_WALLACE, RogueHub_TrainerBattle_Text_0, RogueHub_TrainerBattle_Text_1
	return


RogueHub_ResetRun::
	removemoney 100000000000000
	addmoney 10000000
	givemon SPECIES_EEVEE, 5, ITEM_ORAN_BERRY
	givemon SPECIES_GOLBAT, 5, ITEM_ORAN_BERRY
	givemon SPECIES_MACHOKE, 5, ITEM_ORAN_BERRY
	givemon SPECIES_MEWTWO, 100, ITEM_ORAN_BERRY
	return


RogueHub_EventScript_ExitSign::
	lock
	message RogueHub_EventScript_ExitSign_Text_0
	waitmessage
	pokemart MyMartItems
	msgbox RogueHub_EventScript_ExitSign_Text_1
	release
	return


	.align 2
MyMartItems:
	.2byte ITEM_LAVA_COOKIE
	.2byte ITEM_MOOMOO_MILK
	.2byte ITEM_RARE_CANDY
	.2byte ITEM_LEMONADE
	.2byte ITEM_BERRY_JUICE
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_SUN_STONE
	.2byte ITEM_MOON_STONE
	.2byte ITEM_SOOTHE_BELL
	.2byte ITEM_EXP_SHARE
	.2byte ITEM_NONE

RogueHub_TrainerBattle_Text_0:
	.string "a$"

RogueHub_TrainerBattle_Text_1:
	.string "o$"

RogueHub_EventScript_ExitSign_Text_0:
	.string "Welcome to my store.$"

RogueHub_EventScript_ExitSign_Text_1:
	.string "Come again soon.$"
