# JSON files are run through customjson, which is a Pokabbie specific tool that converts JSON data to an output file

$(DATA_SRC_SUBDIR)/rogue/battle_music.h: $(DATA_SRC_SUBDIR)/rogue/battle_music.json
	$(CUSTOMJSON) battle_music_c $^ $@

$(DATA_SRC_SUBDIR)/rogue/trainers.h: $(DATA_SRC_SUBDIR)/rogue/trainers.json
	$(CUSTOMJSON) trainers_c $^ $@

$(DATA_SRC_SUBDIR)/rogue/quests.h: $(DATA_SRC_SUBDIR)/rogue/quests.json
	$(CUSTOMJSON) quests_c $^ $@

$(DATA_SRC_SUBDIR)/rogue/pokemon_nicknames.h: $(DATA_SRC_SUBDIR)/rogue/pokemon_nicknames.txt
	$(CUSTOMJSON) nicknames_c $^ $@

include/constants/generated/quests.h: $(DATA_SRC_SUBDIR)/rogue/quests.json
	$(CUSTOMJSON) quests_h $^ $@

$(ROGUEPORYSCRIPTSDIR)/Generated/trainers.pory: $(DATA_SRC_SUBDIR)/rogue/trainers.json
	$(CUSTOMJSON) trainers_pory $^ $@

$(ROGUEPORYSCRIPTSDIR)/Generated/quests.pory: $(DATA_SRC_SUBDIR)/rogue/quests.json
	$(CUSTOMJSON) quests_pory $^ $@
