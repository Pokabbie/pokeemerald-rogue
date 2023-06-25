#define CREDITS_FLAG_NONE                   0
#define CREDITS_FLAG_TITLE                  (1 << 0)
#define CREDITS_FLAG_BREAK                  (1 << 2)

#define ENTRIES_PER_PAGE 5

// The index doesn't matter too much here, just make sure it's unique and that's fine
#define FOR_EACH_CREDIT(method) \
    method(0, "", 0) \
    method(1, "POKéMON EMERALD ROGUE", CREDITS_FLAG_TITLE) \
    method(2, "Credits", CREDITS_FLAG_TITLE) \
    method(3, "", CREDITS_FLAG_BREAK) \
    \
    method(4, "Hack Creator", CREDITS_FLAG_TITLE) \
    method(5, "Pokabbie", 0) \
    method(5005, "", CREDITS_FLAG_BREAK) \
    method(6, "POKéMON EMERALD", CREDITS_FLAG_TITLE) \
    method(7, "Game Freak", 0) \
    method(8, "", CREDITS_FLAG_BREAK) \
    \
    method(9, "Made possible by...", CREDITS_FLAG_TITLE) \
    method(10, "", CREDITS_FLAG_BREAK) \
    \
    method(11, "Community Moderators", CREDITS_FLAG_TITLE) \
    method(12, "Erma", 0) \
    method(13, "Raven", 0) \
    method(14, "TailsMK4", 0) \
    method(15, "", CREDITS_FLAG_BREAK) \
    \
    method(16, "Emerald Expansion", CREDITS_FLAG_TITLE) \
    method(17, "DizzyEggg", 0) \
    method(18, "ghoulslash", 0) \
    method(19, "UltimaSoul", 0) \
    method(20, "Lunos", 0) \
    method(21, "mvit", 0) \
    method(22, "Doesnt", 0) \
    method(23, "Cancer Fairy", 0) \
    method(24, "MrDollSteak", 0) \
    method(25, "Avara", 0) \
    method(26, "Kurausukun", 0) \
    method(27, "TheLaw", 0) \
    method(28, "BreadCrumbs", 0) \
    method(29, "Syreldar", 0) \
    method(30, "Pyredrid", 0) \
    method(31, "MandL27", 0) \
    method(32, "", CREDITS_FLAG_BREAK) \
    \
    method(33, "Additional Sprites", CREDITS_FLAG_TITLE) \
    method(34, "AveonTrainer", 0) \
    method(35, "PurpleZaffre", 0) \
    method(36, "", CREDITS_FLAG_BREAK) \
    \
    method(37, "Kymotionian", 0) \
    method(38, "cSc-A7X", 0) \
    method(39, "2and2makes5", 0) \
    method(40, "Pokegirl4ever", 0) \
    method(41, "Fernandojl", 0) \
    method(42, "Silver-Skie", 0) \
    method(43, "Kid1513", 0) \
    method(44, "TyranitarDark", 0) \
    method(45, "Getsuei-H", 0) \
    method(46, "Milomilotic11", 0) \
    method(47, "Kyt666", 0) \
    method(48, "kdiamo11", 0) \
    method(49, "Chocosrawloid", 0) \
    method(50, "SyleDude", 0) \
    method(51, "Gallanty", 0) \
    method(52, "Gizamimi-Pichu", 0) \
    method(53, "princess-phoenix", 0) \
    method(54, "LunarDusk6", 0) \
    method(55, "Larryturbo", 0) \
    method(56, "Kidkatt", 0) \
    method(57, "Zender1752", 0) \
    method(58, "SageDeoxys", 0) \
    method(59, "Lasee0", 0) \
    method(5900, "Ezerart", 0) \
    method(5901, "Wolfang62", 0) \
    method(5902, "DarkusShadow", 0) \
    method(5903, "Anarlaurendil", 0) \
    method(5904, "Lasse00", 0) \
    method(60, "", CREDITS_FLAG_BREAK) \
    \
    method(61, "Additional Songs", CREDITS_FLAG_TITLE) \
    method(62, "CyanSMP64", 0) \
    method(63, "", CREDITS_FLAG_BREAK) \
    \
    method(64, "Special Thanks to", CREDITS_FLAG_TITLE) \
    method(65, "Kate", 0) \
    method(66, "", CREDITS_FLAG_BREAK) \
    \
    method(67, "And to you, the community", 0) \
    method(68, "", CREDITS_FLAG_BREAK) \
    \
    method(69, "Thank you!", 0) \
    method(70, "", CREDITS_FLAG_BREAK) \
    \
    method(71, "For making Rogue larger than", 0) \
    method(72, "I could ever have imagined!", 0) \
    method(73, "", CREDITS_FLAG_BREAK) \
    \
    method(74, "Good luck on your future runs!", 0) \
    method(75, "{EMOJI_HEART}", 0) \
    method(76, "", CREDITS_FLAG_BREAK) \
    \

#define CREDITS_DECLARE_STRINGS(idx, str, flags) static const u8 sCreditsText_ ## idx [] = _(str);
FOR_EACH_CREDIT(CREDITS_DECLARE_STRINGS)
#undef CREDITS_DECLARE_STRINGS

#define CREDITS_DECLARE_ENTRY(idx, str, flags) { sCreditsText_ ## idx , flags },

static const struct CreditsEntry sCreditsEntryPointerTable[] =
{
    FOR_EACH_CREDIT(CREDITS_DECLARE_ENTRY)
};

#undef CREDITS_DECLARE_ENTRY
