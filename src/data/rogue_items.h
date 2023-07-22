#include "graphics.h"

extern const u8 gItemDesc_LinkCable[];
extern const u8 gItemDesc_QuestLog[];
extern const u8 gItemDesc_HealingFlask[];
extern const u8 gItemDesc_RidingWhistle[];
extern const u8 gItemDesc_GoldRidingWhistle[];
extern const u8 gItemDesc_ShopPriceCharm[];
extern const u8 gItemDesc_ShopPriceCurse[];
extern const u8 gItemDesc_FlinchCharm[];
extern const u8 gItemDesc_FlinchCurse[];
extern const u8 gItemDesc_CritCharm[];
extern const u8 gItemDesc_CritCurse[];
extern const u8 gItemDesc_ShedSkinCharm[];
extern const u8 gItemDesc_ShedSkinCurse[];
extern const u8 gItemDesc_WildIVCharm[];
extern const u8 gItemDesc_WildIVCurse[];
extern const u8 gItemDesc_CatchingCharm[];
extern const u8 gItemDesc_CatchingCurse[];
extern const u8 gItemDesc_GraceCharm[];
extern const u8 gItemDesc_GraceCurse[];
extern const u8 gItemDesc_WildCharm[];
extern const u8 gItemDesc_WildCurse[];
extern const u8 gItemDesc_PriorityCharm[];
extern const u8 gItemDesc_PriorityCurse[];
extern const u8 gItemDesc_EndureCharm[];
extern const u8 gItemDesc_EndureCurse[];
extern const u8 gItemDesc_PartyCurse[];
extern const u8 gItemDesc_EverstoneCurse[];
extern const u8 gItemDesc_BattleItemCurse[];
extern const u8 gItemDesc_SpeciesClauseCurse[];
extern const u8 gItemDesc_ItemShuffleCurse[];

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
        .description = gItemDesc_LinkCable,
        .pocket = POCKET_HELD_ITEMS,
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
        .description = gItemDesc_QuestLog,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_BAG_MENU,
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
        .description = gItemDesc_HealingFlask,
        .pocket = POCKET_KEY_ITEMS,
        .registrability = TRUE,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_HealingFlask,
        .iconImage = gItemIcon_Potion,
        .iconPalette = gItemIconPalette_HealingFlask,
    },

    [ITEM_BASIC_RIDING_WHISTLE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Poké Whistle"),
#else
        .name = _("POKé WHISTLE"),
#endif
        .itemId = ITEM_BASIC_RIDING_WHISTLE, 
        .price = 0,
        .description = gItemDesc_RidingWhistle,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_RideMon,
        .secondaryId = 0, // TODO - defines for whistle types
        .registrability = TRUE,
        .iconImage = gItemIcon_FameChecker,
        .iconPalette = gItemIconPalette_RogueCharm,
    },

    [ITEM_GOLD_RIDING_WHISTLE - ITEM_ROGUE_ITEM_FIRST] =
    {
#ifdef ROGUE_EXPANSION
        .name = _("Gold Whistle"),
#else
        .name = _("GOLD WHISTLE"),
#endif
        .itemId = ITEM_GOLD_RIDING_WHISTLE,
        .price = 0,
        .description = gItemDesc_GoldRidingWhistle,
        .pocket = POCKET_KEY_ITEMS,
        .type = ITEM_USE_FIELD,
        .fieldUseFunc = ItemUseOutOfBattle_RideMon,
        .secondaryId = 1, // TODO - defines for whistle types
        .registrability = TRUE,
        .iconImage = gItemIcon_FameChecker,
        .iconPalette = gItemIconPalette_RogueCharm,
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
        .description = gItemDesc_ShopPriceCharm,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .description = gItemDesc_FlinchCharm,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .description = gItemDesc_CritCharm,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .description = gItemDesc_ShedSkinCharm,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .description = gItemDesc_WildIVCharm,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .description = gItemDesc_CatchingCharm,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .description = gItemDesc_GraceCharm,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .description = gItemDesc_WildCharm,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .description = gItemDesc_PriorityCharm,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .description = gItemDesc_EndureCharm,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .description = gItemDesc_ShopPriceCurse,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .itemId = ITEM_FLINCH_CURSE,
        .price = 0,
        .description = gItemDesc_FlinchCurse,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .description = gItemDesc_CritCurse,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .description = gItemDesc_ShedSkinCurse,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .description = gItemDesc_WildIVCurse,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .description = gItemDesc_CatchingCurse,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .description = gItemDesc_GraceCurse,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .description = gItemDesc_WildCurse,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .itemId = ITEM_PARTY_CURSE,
        .price = 0,
        .description = gItemDesc_PartyCurse,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .itemId = ITEM_EVERSTONE_CURSE,
        .price = 0,
        .description = gItemDesc_EverstoneCurse,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .description = gItemDesc_BattleItemCurse,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .description = gItemDesc_SpeciesClauseCurse,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .description = gItemDesc_ItemShuffleCurse,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .description = gItemDesc_PriorityCurse,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
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
        .itemId = ITEM_ENDURE_CURSE,
        .price = 0,
        .description = gItemDesc_EndureCurse,
        .pocket = POCKET_CHARMS,
        .type = ITEM_USE_BAG_MENU,
        .registrability = FALSE,
        .iconImage = gItemIcon_RogueCurse,
        .iconPalette = gItemIconPalette_RogueCurse,
    },
};
