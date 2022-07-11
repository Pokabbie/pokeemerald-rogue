Rogue_Hub_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, RogueHub_ResetRun
	.byte 0


RogueHub_EventScript_ExitSign::
	msgbox RogueHub_EventScript_ExitSign_Text_0, MSGBOX_SIGN
	return


RogueHub_ResetRun::
	return


RogueHub_Shop_Utilty::
	lock
	message RogueHub_Shop_Utilty_Text_0
	waitmessage
	pokemart BasicItemMart
	msgbox RogueHub_Shop_Utilty_Text_1
	release
	return


RogueHub_Shop_Rare::
	lock
	message RogueHub_Shop_Rare_Text_0
	waitmessage
	pokemart RareItemMart
	msgbox RogueHub_Shop_Utilty_Text_1
	release
	return


	.align 2
BasicItemMart:
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_REPEL
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_MAX_ELIXIR
	.2byte ITEM_MAX_ETHER
	.2byte ITEM_POKE_BALL
	.2byte ITEM_REPEAT_BALL
	.2byte ITEM_LUXURY_BALL
	.2byte ITEM_NET_BALL
	.2byte ITEM_DIVE_BALL
	.2byte ITEM_NONE

	.align 2
RareItemMart:
	.2byte ITEM_SUPER_ROD
	.2byte ITEM_MACH_BIKE
	.2byte ITEM_ACRO_BIKE
	.2byte ITEM_RARE_CANDY
	.2byte ITEM_LEAF_STONE
	.2byte ITEM_WATER_STONE
	.2byte ITEM_FIRE_STONE
	.2byte ITEM_SOOTHE_BELL
	.2byte ITEM_EXP_SHARE
	.2byte ITEM_MOON_STONE
	.2byte ITEM_SUN_STONE
	.2byte ITEM_KINGS_ROCK
	.2byte ITEM_METAL_COAT
	.2byte ITEM_DEEP_SEA_SCALE
	.2byte ITEM_DEEP_SEA_TOOTH
	.2byte ITEM_NONE

RogueHub_EventScript_ExitSign_Text_0:
	.string "The journey begins beyond this point..$"

RogueHub_Shop_Utilty_Text_0:
	.string "Welcome to the Utility Shop!$"

RogueHub_Shop_Utilty_Text_1:
	.string "Come again soon.$"

RogueHub_Shop_Rare_Text_0:
	.string "Welcome to the Rare Items Shop!$"
