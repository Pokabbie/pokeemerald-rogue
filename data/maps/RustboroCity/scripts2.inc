RustboroCity_EventScript_MayBrineyHint_PostBattle::
	msgbox RustboroCity_EventScript_MayBrineyHint_PostBattle_Text_0
	giveitem ITEM_QUICK_CLAW
	goto RustboroCity_EventScript_MayBrineyHint
	return


RustboroCity_EventScript_BrendanBrineyHint_PostBattle::
	msgbox RustboroCity_EventScript_BrendanBrineyHint_PostBattle_Text_0
	giveitem ITEM_QUICK_CLAW
	goto RustboroCity_EventScript_BrendanBrineyHint
	return


RustboroCity_EventScript_MayBrineyHint_PostBattle_Text_0:
	.string "MAY: Wow you've gotten pretty good\n"
	.string "{PLAYER}{KUN}!\p"
	.string "I know!\p"
	.string "I've got something that might help you\n"
	.string "get even stronger!\p"
	.string "Here take this!$"

RustboroCity_EventScript_BrendanBrineyHint_PostBattle_Text_0:
	.string "BRENDAN: Wow you've gotten pretty good\n"
	.string "{PLAYER}{KUN}!\p"
	.string "I know!\p"
	.string "I've got something that might help you\n"
	.string "get even stronger!\p"
	.string "Here take this!$"
