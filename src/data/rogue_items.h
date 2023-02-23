#include "graphics.h"

const struct RogueItem gRogueItems[] =
{
    [ITEM_LINK_CABLE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Link Cable"),
#else
        .name = _("LINK CABLE"),
#endif
        .itemId = ITEM_LINK_CABLE,
        .price = 2100,
        .description = _(
            "Makes certain\n"
            "species of POKéMON\n"
            "evolve when held."),
        .pocket = POCKET_ITEMS,
        .iconImage = gItemIcon_ExpShare,
        .iconPalette = gItemIconPalette_ExpShare,
    },

    [ITEM_QUEST_LOG - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Quest Log"),
#else
        .name = _("QUEST LOG"),
#endif
        .itemId = ITEM_QUEST_LOG,
        .price = 0,
        .description = _(
            "A digital log for\n"
            "tracking Quests\n"
            "and their rewards."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .fieldUseFunc = ItemUseOutOfBattle_QuestLog,
        .iconImage = gItemIcon_FameChecker,
        .iconPalette = gItemIconPalette_FameChecker,
    },

    [ITEM_HEALING_FLASK - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Healing Flask"),
#else
        .name = _("HEALING FLASK"),
#endif
        .itemId = ITEM_HEALING_FLASK,
        .price = 0,
        .description = _(
            "todo flash desc."),
        .pocket = POCKET_KEY_ITEMS,
        .importance = 0,
        .registrability = TRUE,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_HealingFlask,
        .iconImage = gItemIcon_Potion,
        .iconPalette = gItemIconPalette_HyperPotion,
    },


    [ITEM_SHOP_PRICE_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Discount Charm"),
#else
        .name = _("DISCOUNT CHARM"),
#endif
        .itemId = ITEM_SHOP_PRICE_CHARM,
        .price = 0,
        .description = _(
            "Shop prices are\n"
            "reduced by 40%$"),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_FLINCH_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Flinch Charm"),
#else
        .name = _("FLINCH CHARM"),
#endif
        .itemId = ITEM_FLINCH_CHARM,
        .price = 0,
        .description = _(
            "Friendly moves\n"
            "have an extra 10%\n"
            "chance to flinch."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_CRIT_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Crit Charm"),
#else
        .name = _("CRIT CHARM"),
#endif
        .itemId = ITEM_CRIT_CHARM,
        .price = 0,
        .description = _(
            "Friendly POKéMON\n"
            "have an increased\n"
            "crit rate."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_SHED_SKIN_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Shed Skin Charm"),
#else
        .name = _("SHED SKIN CHARM"),
#endif
        .itemId = ITEM_SHED_SKIN_CHARM,
        .price = 0,
        .description = _(
            "Friendly POKéMON\n"
            "have a 20% chance\n"
            "to cure status."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_WILD_IV_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Strength Charm"),
#else
        .name = _("STRENGTH CHARM"),
#endif
        .itemId = ITEM_WILD_IV_CHARM,
        .price = 0,
        .description = _(
            "Caught POKéMON are\n"
            "slightly stronger."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_CATCHING_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Catching Charm"),
#else
        .name = _("CATCHING CHARM"),
#endif
        .itemId = ITEM_CATCHING_CHARM,
        .price = 0,
        .description = _(
            "BALL effectiveness\n"
            "is increased by\n"
            "100%."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_GRACE_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Grace Charm"),
#else
        .name = _("GRACE CHARM"),
#endif
        .itemId = ITEM_GRACE_CHARM,
        .price = 0,
        .description = _(
            "Friendly moves\n"
            "have an extra 75%\n"
            "for extra effects."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_WILD_ENCOUNTER_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Wild Charm"),
#else
        .name = _("WILD CHARM"),
#endif
        .itemId = ITEM_WILD_ENCOUNTER_CHARM,
        .price = 0,
        .description = _(
            "Routes contain 2\n"
            "additional wild\n"
            "POKéMON."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_MOVE_PRIORITY_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Priority Charm"),
#else
        .name = _("PRIORITY CHARM"),
#endif
        .itemId = ITEM_MOVE_PRIORITY_CHARM,
        .price = 0,
        .description = _(
            "Friendly moves\n"
            "have 10% chance to\n"
            "activate Quick Claw."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_ENDURE_CHARM - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Endure Charm"),
#else
        .name = _("ENDURE CHARM"),
#endif
        .itemId = ITEM_ENDURE_CHARM,
        .price = 0,
        .description = _(
            "Friendly POKéMON\n"
            "have a 20% chance\n"
            "to endure OHKOs."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCharm,
        .iconPalette = gItemIconPalette_RogueCharm,
    },


    [ITEM_SHOP_PRICE_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Discount Curse"),
#else
        .name = _("DISCOUND CURSE"),
#endif
        .itemId = ITEM_SHOP_PRICE_CURSE,
        .price = 0,
        .description = _(
            "Shop prices are\n"
            "increased by 40%"),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_FLINCH_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Flinch Curse"),
#else
        .name = _("FLINCH CURSE"),
#endif
        .itemId = ITEM_SHOP_PRICE_CURSE,
        .price = 0,
        .description = _(
            "Opponent moves\n"
            "have an extra 10%\n"
            "chance to flinch."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_CRIT_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Crit Curse"),
#else
        .name = _("CRIT CURSE"),
#endif
        .itemId = ITEM_CRIT_CURSE,
        .price = 0,
        .description = _(
            "Opponent POKéMON\n"
            "have an increased\n"
            "crit rate."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_SHED_SKIN_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Shed Skin Curse"),
#else
        .name = _("SHED SKIN CURSE"),
#endif
        .itemId = ITEM_SHED_SKIN_CURSE,
        .price = 0,
        .description = _(
            "Opponent POKéMON\n"
            "have a 15% chance\n"
            "to cure status."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_WILD_IV_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Strength Curse"),
#else
        .name = _("STRENGTH CURSE"),
#endif
        .itemId = ITEM_WILD_IV_CURSE,
        .price = 0,
        .description = _(
            "Caught POKéMON are\n"
            "slightly weaker."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_CATCHING_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Catching Curse"),
#else
        .name = _("CATCHING CURSE"),
#endif
        .itemId = ITEM_CATCHING_CURSE,
        .price = 0,
        .description = _(
            "BALL effectiveness\n"
            "is decreased by\n"
            "25%."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_GRACE_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Grace Curse"),
#else
        .name = _("GRACE CURSE"),
#endif
        .itemId = ITEM_GRACE_CURSE,
        .price = 0,
        .description = _(
            "Opponent moves\n"
            "have an extra 50%\n"
            "for extra effects."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_WILD_ENCOUNTER_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Wild Curse"),
#else
        .name = _("WILD CURSE"),
#endif
        .itemId = ITEM_WILD_ENCOUNTER_CURSE,
        .price = 0,
        .description = _(
            "Routes contain 1\n"
            "less wild POKéMON."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_PARTY_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Capacity Curse"),
#else
        .name = _("CAPACITY CURSE"),
#endif
        .itemId = ITEM_WILD_ENCOUNTER_CURSE,
        .price = 0,
        .description = _(
            "The maximum number\n"
            "of POKéMON in your\n"
            "party is reduced."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_EVERSTONE_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Everstone Curse"),
#else
        .name = _("EVERSTONE CURSE"),
#endif
        .itemId = ITEM_WILD_ENCOUNTER_CURSE,
        .price = 0,
        .description = _(
            "'LVL UP' Evolutions\n"
            "turn into 'LVL whilst\n"
            "holding LINK CABLE'."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_BATTLE_ITEM_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Embargo Curse"),
#else
        .name = _("EMBARGO CURSE"),
#endif
        .itemId = ITEM_BATTLE_ITEM_CURSE,
        .price = 0,
        .description = _(
            "Items cannot be\n"
            "used in battle.\n"
            "(Except POKéBALLS)"),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_SPECIES_CLAUSE_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Species Curse"),
#else
        .name = _("SPECIES CURSE"),
#endif
        .itemId = ITEM_SPECIES_CLAUSE_CURSE,
        .price = 0,
        .description = _(
            "POKéMON can't be\n"
            "caught if species\n"
            "already in party."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_ITEM_SHUFFLE_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Shuffle Curse"),
#else
        .name = _("SHUFFLE CURSE"),
#endif
        .itemId = ITEM_ITEM_SHUFFLE_CURSE,
        .price = 0,
        .description = _(
            "Entering a Battle\n"
            "will shuffle Held\n"
            "Items between {PKMN}."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_MOVE_PRIORITY_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Priority Curse"),
#else
        .name = _("PRIORITY CURSE"),
#endif
        .itemId = ITEM_MOVE_PRIORITY_CURSE,
        .price = 0,
        .description = _(
            "Opponent moves\n"
            "have 10% chance to\n"
            "activate Quick Claw."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },

    [ITEM_ENDURE_CURSE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Endure Curse"),
#else
        .name = _("ENDURE CURSE"),
#endif
        .itemId = ITEM_MOVE_PRIORITY_CURSE,
        .price = 0,
        .description = _(
            "Opponent POKéMON\n"
            "have a 20% chance\n"
            "to endure OHKOs."),
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
        .importance = 0,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },
};
