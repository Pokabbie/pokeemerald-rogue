# JSON files are run through customjson, which is a Pokabbie specific tool that converts JSON data to an output file

AUTO_GEN_TARGETS += $(DATA_SRC_SUBDIR)/rogue/*.h
AUTO_GEN_TARGETS += include/constants/generated/*.h
AUTO_GEN_TARGETS += $(ROGUEPORYSCRIPTSDIR)/Generated/*.pory

$(DATA_SRC_SUBDIR)/rogue/battle_music.h: $(DATA_SRC_SUBDIR)/rogue/battle_music.json
	$(CUSTOMJSON) battle_music_c $^ $@

$(DATA_SRC_SUBDIR)/rogue/trainers.h: $(DATA_SRC_SUBDIR)/rogue/trainers.json $(DATA_SRC_SUBDIR)/rogue/trainers/*.json
	$(CUSTOMJSON) trainers_c $(DATA_SRC_SUBDIR)/rogue/trainers.json $@

$(DATA_SRC_SUBDIR)/rogue/quests.h: $(DATA_SRC_SUBDIR)/rogue/quests.json $(DATA_SRC_SUBDIR)/rogue/quests/*.json
	$(CUSTOMJSON) quests_c $(DATA_SRC_SUBDIR)/rogue/quests.json $@

$(DATA_SRC_SUBDIR)/rogue/pokemon_nicknames.h: $(DATA_SRC_SUBDIR)/rogue/pokemon_nicknames.txt
	$(CUSTOMJSON) nicknames_c $^ $@

include/constants/generated/quests.h: $(DATA_SRC_SUBDIR)/rogue/quests.json $(DATA_SRC_SUBDIR)/rogue/quests/*.json
	$(CUSTOMJSON) quests_h $(DATA_SRC_SUBDIR)/rogue/quests.json $@

include/constants/generated/quest_consts.h: $(DATA_SRC_SUBDIR)/rogue/quests.json $(DATA_SRC_SUBDIR)/rogue/quests/*.json
	$(CUSTOMJSON) quest_consts_h $(DATA_SRC_SUBDIR)/rogue/quests.json $@

$(ROGUEPORYSCRIPTSDIR)/Generated/trainers.pory: $(DATA_SRC_SUBDIR)/rogue/trainers.json $(DATA_SRC_SUBDIR)/rogue/trainers/*.json
	$(CUSTOMJSON) trainers_pory $(DATA_SRC_SUBDIR)/rogue/trainers.json $@

$(ROGUEPORYSCRIPTSDIR)/Generated/quests.pory: $(DATA_SRC_SUBDIR)/rogue/quests.json $(DATA_SRC_SUBDIR)/rogue/quests/*.json
	$(CUSTOMJSON) quests_pory $(DATA_SRC_SUBDIR)/rogue/quests.json $@

$(DATA_SRC_SUBDIR)/rogue/custom_mons.h: $(DATA_SRC_SUBDIR)/rogue/custom_mons.json
	$(CUSTOMJSON) custom_mons_c $^ $@

include/constants/generated/custom_mons.h: $(DATA_SRC_SUBDIR)/rogue/custom_mons.json
	$(CUSTOMJSON) custom_mons_h $^ $@

$(DATA_SRC_SUBDIR)/rogue/decorations.h: $(DATA_SRC_SUBDIR)/rogue/decorations.json $(DATA_SRC_SUBDIR)/rogue/decorations/*.json
	$(CUSTOMJSON) decoration_c $(DATA_SRC_SUBDIR)/rogue/decorations.json $@

include/constants/generated/decorations.h: $(DATA_SRC_SUBDIR)/rogue/decorations.json $(DATA_SRC_SUBDIR)/rogue/decorations/*.json
	$(CUSTOMJSON) decoration_h $(DATA_SRC_SUBDIR)/rogue/decorations.json $@