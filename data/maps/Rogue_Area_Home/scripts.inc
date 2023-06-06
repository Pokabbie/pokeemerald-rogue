Rogue_Area_Home_MapScripts::
	.byte 0


Rogue_Area_InteractWithWorkbench::
	specialvar VAR_RESULT, GetPlayerFacingDirection
	compare VAR_RESULT, DIR_SOUTH
	goto_if_ne Rogue_Area_InteractWithWorkbench_1
	return

Rogue_Area_InteractWithWorkbench_1:
	lock
	message Rogue_Area_InteractWithWorkbench_Text_0
	waitmessage
	multichoicedynamic 20, 10, TRUE, MULTI_HUB_AREA_UPGRADES, NO
	special RogueHub_GetUpgradeFromMultichoiceResult
	compare VAR_RESULT, HUB_UPGRADE_NONE
	goto_if_ne Rogue_Area_InteractWithWorkbench_4
Rogue_Area_InteractWithWorkbench_3:
	release
	return

Rogue_Area_InteractWithWorkbench_4:
	copyvar VAR_0x8004, VAR_RESULT
	call Rogue_Area_ApplyHubUpgrade
	goto Rogue_Area_InteractWithWorkbench_3


Rogue_Area_ApplyHubUpgrade::
	special RogueHub_ApplyHubUpgrade
	compare VAR_RESULT, TRUE
	goto_if_eq Rogue_Area_ApplyHubUpgrade_1
	return

Rogue_Area_ApplyHubUpgrade_1:
	fadescreen FADE_TO_BLACK
	playbgm MUS_NONE, FALSE
	delay 15
	playse SE_BRIDGE_WALK
	waitse
	playse SE_BRIDGE_WALK
	waitse
	playse SE_BRIDGE_WALK
	waitse
	delay 15
	playse SE_M_DIG
	waitse
	playse SE_M_DIG
	waitse
	playse SE_M_DIG
	waitse
	delay 15
	playse SE_M_BUBBLE2
	waitse
	playse SE_M_BUBBLE2
	waitse
	playse SE_M_BUBBLE2
	waitse
	delay 15
	playse SE_M_VICEGRIP
	waitse
	delay 15
	playfanfare MUS_LEVEL_UP
	waitfanfare
	msgbox Rogue_Area_ApplyHubUpgrade_Text_0, MSGBOX_DEFAULT
	special ReloadWarpSilent
	return


Rogue_Area_InteractWithWorkbench_Text_0:
	.string "What upgrade would you like to build?$"

Rogue_Area_ApplyHubUpgrade_Text_0:
	.string "Upgrade message TODO$"
