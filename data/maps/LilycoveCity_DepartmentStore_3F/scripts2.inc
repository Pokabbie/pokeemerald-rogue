LilycoveCity_DepartmentStore_3F_EventScript_BottomClerkLeft::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart LilycoveCity_DepartmentStore_3F_Pokemart_MegaStones
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	return


LilycoveCity_DepartmentStore_3F_EventScript_BottomClerkRight::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart LilycoveCity_DepartmentStore_3F_Pokemart_ZCrystals
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	return


	.align 2
LilycoveCity_DepartmentStore_3F_Pokemart_MegaStones:
	.2byte ITEM_LAVA_COOKIE
	.2byte ITEM_MOOMOO_MILK
	.2byte ITEM_RARE_CANDY
	.2byte ITEM_LEMONADE
	.2byte ITEM_BERRY_JUICE
	.2byte ITEM_NONE

	.align 2
LilycoveCity_DepartmentStore_3F_Pokemart_ZCrystals:
	.2byte ITEM_RARE_CANDY
	.2byte ITEM_NONE
