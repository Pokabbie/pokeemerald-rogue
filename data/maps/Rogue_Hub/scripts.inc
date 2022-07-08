Rogue_Hub_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, RogueHub_ResetRun
	.byte 0


RogueHub_EventScript_ExitSign::
	msgbox RogueHub_EventScript_ExitSign_Text_0, MSGBOX_SIGN
	return


RogueHub_ResetRun::
	removemoney 100000000000000
	addmoney 1000
	givemon SPECIES_EEVEE, 5, ITEM_ORAN_BERRY
	return


ScriptWithPokemart::
	lock
	message ScriptWithPokemart_Text_0
	waitmessage
	pokemart MyMartItems
	msgbox ScriptWithPokemart_Text_1
	release
	return


	.align 2
MyMartItems:
	.2byte ITEM_LAVA_COOKIE
	.2byte ITEM_MOOMOO_MILK
	.2byte ITEM_RARE_CANDY
	.2byte ITEM_LEMONADE
	.2byte ITEM_BERRY_JUICE
	.2byte ITEM_NONE

RogueHub_EventScript_ExitSign_Text_0:
	.string "The journey begins beyond this point..$"

ScriptWithPokemart_Text_0:
	.string "Welcome to my store.$"

ScriptWithPokemart_Text_1:
	.string "Come again soon.$"
