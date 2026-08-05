CXX ?= g++
CC ?= gcc

CXXFLAGS := -std=c++17 -O2 -Wall -Wno-switch -Wno-comment -Wno-unused-function
CFLAGS := -std=c17 -O2 -Wall -Wno-switch -Wno-comment -Wno-unused-function

ROOT_DIR ?= ../../../../
DATA_SRC_SUBDIR ?= $(ROOT_DIR)src/data

INCLUDES := -I . -I $(ROOT_DIR)include -I $(ROOT_DIR)src
DEFINES := -D ROGUE_BAKING=1 -D ROGUE_EXPANSION=1


CXXSRCS := Main.cpp
CSRCS := GameData.c
OBJECTS := Main.o GameData.o

HEADERS := BakeHelpers.h BakeHelpers_Vanilla.h BakeHelpers_EX.h

ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif

.PHONY: all clean

all: querybaker$(EXE) querybaker_run
	@:

querybaker_c: $(CSRCS) $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) $(DEFINES) $(CSRCS)

querybaker_cxx: $(CXXSRCS) $(HEADERS)
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $(DEFINES) $(CXXSRCS)

querybaker$(EXE): querybaker_c querybaker_cxx
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)
	

# Now run it!
querybaker_run: querybaker$(EXE)
	@echo "Baking Local Data..."
	./querybaker$(EXE) $(DATA_SRC_SUBDIR)/rogue_bake_data.h $(DATA_SRC_SUBDIR)/rogue_exported_stats.json
	@echo "Finished Local Data Bake."


clean:
	$(RM) querybaker querybaker$(EXE)
	$(RM) $(OBJECTS)
