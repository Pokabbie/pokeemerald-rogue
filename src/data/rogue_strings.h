

const u8 gText_RogueVersion[] = _("v1.1.2");

#ifdef ROGUE_EXPANSION
const u8 gText_RogueVersionPrefix[] = _("EX ");
#else
const u8 gText_RogueVersionPrefix[] = _("Vanilla ");
#endif

#ifdef ROGUE_DEBUG
const u8 gText_RogueVersionSuffix[] = _(" (DEBUG)");
#else
const u8 gText_RogueVersionSuffix[] = _(" - The Routing Update");
#endif

#ifdef ROGUE_EXPANSION
const u8 gText_ItemLinkCable[] = _("Link Cable");
#else
const u8 gText_ItemLinkCable[] = _("LINK CABLE");
#endif

#ifdef ROGUE_DEBUG
const u8 gText_RogueDebug_Header[] = _("ROGUE DEBUG");
const u8 gText_RogueDebug_Room[] = _("\nRoom: ");
const u8 gText_RogueDebug_BossRoom[] = _("\nBossRoom: ");
const u8 gText_RogueDebug_Difficulty[] = _("\nDfcy: ");
const u8 gText_RogueDebug_PlayerLvl[] = _("\nPly lvl: ");
const u8 gText_RogueDebug_WildLvl[] = _("\nWld lvl: ");
const u8 gText_RogueDebug_WildCount[] = _("\nWld Opt: ");
const u8 gText_RogueDebug_ItemCount[] = _("\nItm Opt: ");
const u8 gText_RogueDebug_TrainerCount[] = _("\nTrn Opt: ");
const u8 gText_RogueDebug_Seed[] = _("\nSeed: ");

const u8 gText_RogueDebug_AdvHeader[] = _("ROGUE ADVPATH");
const u8 gText_RogueDebug_AdvCount[] = _("\nCount: ");
const u8 gText_RogueDebug_X[] = _("\nX: ");
const u8 gText_RogueDebug_Y[] = _("\nY: ");
#endif