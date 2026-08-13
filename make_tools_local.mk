
MAKEFLAGS += --no-print-directory

# Inclusive List. Tools that only get built locally, once we have already built the game once (These tools can reference generated files/game data)
TOOLDIRS := tools/Pokabbie/Build/QueryBaker

.PHONY: all $(TOOLDIRS)

all: $(TOOLDIRS)
	@:

$(TOOLDIRS):
	@$(MAKE) -C $@
