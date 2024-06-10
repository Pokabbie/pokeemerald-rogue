#define CREDITS_FLAG_NONE                   0
#define CREDITS_FLAG_TITLE                  (1 << 0)
#define CREDITS_FLAG_BREAK                  (1 << 2)

#define ENTRIES_PER_PAGE 5

// The index doesn't matter too much here, just make sure it's unique and that's fine
#define FOR_EACH_CREDIT(method) \
    method(0, "", 0) \
    method(1, "Pokémon Emerald Rogue", CREDITS_FLAG_TITLE) \
    method(2, "Credits", CREDITS_FLAG_TITLE) \
    method(3, "", CREDITS_FLAG_BREAK) \
    \
    method(100, "", 0) \
    method(101, "Hack Creator", CREDITS_FLAG_TITLE) \
    method(102, "Pokabbie", 0) \
    method(103, "", CREDITS_FLAG_BREAK) \
    \
    method(200, "", 0) \
    method(201, "Pokémon Emerald", CREDITS_FLAG_TITLE) \
    method(202, "Game Freak", 0) \
    method(203, "", CREDITS_FLAG_BREAK) \
    \
    method(300, "", 0) \
    method(301, "Made possible by...", CREDITS_FLAG_TITLE) \
    method(302, "", CREDITS_FLAG_BREAK) \
    \
    method(400, "Community Moderators", CREDITS_FLAG_TITLE) \
    method(401, "Erma", 0) \
    method(402, "Raven", 0) \
    method(403, "TailsMK4", 0) \
    method(404, "LightningStrike7", 0) \
    method(405, "Nacholord", 0) \
    method(406, "", CREDITS_FLAG_BREAK) \
    \
    method(500, "Community Artists", CREDITS_FLAG_TITLE) \
    method(501, "Zefa", 0) \
    method(502, "SuperBren614", 0) \
    method(503, "LightningStrike7", 0) \
    method(504, "Hex", 0) \
    method(505, "", CREDITS_FLAG_BREAK) \
    \
    method(600, "RH Hideout - Emerald Expansion", CREDITS_FLAG_TITLE) \
    method(601, "UltimaSoul", 0) \
    method(602, "ghoulslash", 0) \
    method(603, "DizzyEggg", 0) \
    method(604, "AsparagusEduardo", 0) \
    method(605, "ExpoSeed", 0) \
    method(606, "Pyredrid", 0) \
    method(607, "MandL127", 0) \
    method(608, "Lunos", 0) \
    method(609, "ShantyTown", 0) \
    method(610, "mvit", 0) \
    method(611, "Doesnt", 0) \
    method(612, "Cancer Fairy", 0) \
    method(613, "MrDollSteak", 0) \
    method(614, "Avara", 0) \
    method(615, "Kurausukun", 0) \
    method(616, "TheLaw", 0) \
    method(617, "BreadCrumbs", 0) \
    method(618, "Syreldar", 0) \
    method(619, "Pyredrid", 0) \
    method(620, "", CREDITS_FLAG_BREAK) \
    \
    method(700, "", 0) \
    method(701, "Key Items Menu", CREDITS_FLAG_TITLE) \
    method(702, "Merrp", 0) \
    method(703, "", CREDITS_FLAG_BREAK) \
    \
    method(800, "", 0) \
    method(801, "Follow Me + Sideways Stairs", CREDITS_FLAG_TITLE) \
    method(802, "ghoulslash", 0) \
    method(803, "", CREDITS_FLAG_BREAK) \
    \
    method(900, "", 0) \
    method(901, "DS Songs", CREDITS_FLAG_TITLE) \
    method(902, "CyanSMP64", 0) \
    method(903, "", CREDITS_FLAG_BREAK) \
    \
    method(1000, "Additional Sprites", CREDITS_FLAG_TITLE) \
    method(1001, "AveonTrainer", 0) \
    method(1002, "PurpleZaffre", 0) \
    method(1003, "UlithiumDragon", 0) \
    method(1004, "HighNoonMoon", 0) \
    method(1005, "xDracolich", 0) \
    method(1006, "", CREDITS_FLAG_BREAK) \
    \
    method(1100, "ZacWeavile", 0) \
    method(1101, "Gnomowladny", 0) \
    method(1102, "Beliot419", 0) \
    method(1103, "Brumirage", 0) \
    method(1104, "Kyledove", 0) \
    method(1105, "", CREDITS_FLAG_BREAK) \
    \
    method(1200, "Kymotionian", 0) \
    method(1201, "cSc-A7X", 0) \
    method(1202, "2and2makes5", 0) \
    method(1203, "Pokegirl4ever", 0) \
    method(1204, "Fernandojl", 0) \
    method(1205, "Silver-Skie", 0) \
    method(1206, "Kid1513", 0) \
    method(1207, "TyranitarDark", 0) \
    method(1208, "Getsuei-H", 0) \
    method(1209, "Milomilotic11", 0) \
    method(1210, "Kyt666", 0) \
    method(1211, "kdiamo11", 0) \
    method(1212, "Chocosrawloid", 0) \
    method(1213, "SyleDude", 0) \
    method(1214, "Gallanty", 0) \
    method(1215, "Gizamimi-Pichu", 0) \
    method(1216, "princess-phoenix", 0) \
    method(1217, "LunarDusk6", 0) \
    method(1218, "Larryturbo", 0) \
    method(1219, "Kidkatt", 0) \
    method(1220, "Zender1752", 0) \
    method(1221, "SageDeoxys", 0) \
    method(1222, "Lasee0", 0) \
    method(1223, "Ezerart", 0) \
    method(1224, "Wolfang62", 0) \
    method(1225, "DarkusShadow", 0) \
    method(1226, "Anarlaurendil", 0) \
    method(1227, "Lasse00", 0) \
    method(1228, "shaderr31", 0) \
    method(1229, "CarmaNekko", 0) \
    method(1230, "EduarPokeN", 0) \
    method(1231, "TintjeMadelintje101", 0) \
    method(1232, "", CREDITS_FLAG_BREAK) \
    \
    method(1301, "", 0) \
    method(1302, "Special Thanks to", CREDITS_FLAG_TITLE) \
    method(1303, "Kate", 0) \
    method(1304, "", CREDITS_FLAG_BREAK) \
    \
    method(1400, "", 0) \
    method(1401, "And to you, the community", 0) \
    method(1402, "", CREDITS_FLAG_BREAK) \
    \
    method(1403, "", 0) \
    method(1404, "Thank you!", 0) \
    method(1405, "", CREDITS_FLAG_BREAK) \
    \
    method(1406, "", 0) \
    method(1407, "For making Rogue larger than", 0) \
    method(1408, "I could ever have imagined!", 0) \
    method(1409, "", CREDITS_FLAG_BREAK) \
    \
    method(1410, "", 0) \
    method(1411, "Good luck on your future runs!", 0) \
    method(1412, "{EMOJI_HEART}", 0) \
    method(1413, "", CREDITS_FLAG_BREAK) \
    \
    method(1500, "", 0) \
    method(1501, "", CREDITS_FLAG_BREAK) \

#define CREDITS_DECLARE_STRINGS(idx, str, flags) static const u8 sCreditsText_ ## idx [] = _(str);
FOR_EACH_CREDIT(CREDITS_DECLARE_STRINGS)
#undef CREDITS_DECLARE_STRINGS

#define CREDITS_DECLARE_ENTRY(idx, str, flags) { sCreditsText_ ## idx , flags },

static const struct CreditsEntry sCreditsEntryPointerTable[] =
{
    FOR_EACH_CREDIT(CREDITS_DECLARE_ENTRY)
};

#undef CREDITS_DECLARE_ENTRY
