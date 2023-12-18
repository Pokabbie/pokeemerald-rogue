Route120_EventScript_Kecleon1::
	lock
	faceplayer
	setvar VAR_0x8009, 1
	goto EventScript_Kecleon
	end

Route120_EventScript_Kecleon2::
	lock
	faceplayer
	setvar VAR_0x8009, 2
	goto EventScript_Kecleon
	end

Route120_EventScript_Kecleon3::
	lock
	faceplayer
	setvar VAR_0x8009, 3
	goto EventScript_Kecleon
	end

Route120_EventScript_Kecleon4::
	lock
	faceplayer
	setvar VAR_0x8009, 4
	goto EventScript_Kecleon
	end

Route120_EventScript_Kecleon5::
	lock
	faceplayer
	setvar VAR_0x8009, 5
	goto EventScript_Kecleon
	end

Route119_EventScript_Kecleon1::
	lock
	faceplayer
	setvar VAR_0x8009, 6
	goto EventScript_Kecleon
	end

Route119_EventScript_Kecleon2::
	lock
	faceplayer
	setvar VAR_0x8009, 7
	goto EventScript_Kecleon
	end

EventScript_Kecleon::
	end

EventScript_AskUseDevonScope::
	end

EventScript_BattleKecleon::
	end

EventScript_RemoveKecleon::
	goto Common_EventScript_RemoveStaticPokemon
	end

Movement_KecleonAppears:
	set_visible
	delay_4
	set_invisible
	delay_4
	set_visible
	delay_4
	set_invisible
	delay_4
	set_visible
	delay_8
	set_invisible
	delay_8
	set_visible
	delay_8
	set_invisible
	delay_8
	set_visible
	delay_16
	set_invisible
	delay_16
	set_visible
	step_end
