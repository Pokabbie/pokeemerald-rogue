# JSON files are run through customjson, which is a Pokabbie specific tool that converts JSON data to an output file

# AUTO_GEN_TARGETS += $(DATA_SRC_SUBDIR)/rogue/trainers.h
# $(DATA_SRC_SUBDIR)/rogue/trainers.h: trainers $(DATA_SRC_SUBDIR)/rogue/trainers.json
# 	$(CUSTOMJSON) $^ $@

# $(C_BUILDDIR)/rogue/trainers.o: c_dep += $(DATA_SRC_SUBDIR)/rogue/trainers.h

$(DATA_SRC_SUBDIR)/rogue/trainers.h: $(DATA_SRC_SUBDIR)/rogue/trainers.json
	$(CUSTOMJSON) trainers_c $^ $@
