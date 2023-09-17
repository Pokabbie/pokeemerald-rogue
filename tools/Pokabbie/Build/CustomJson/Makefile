CXX ?= g++

CXXFLAGS := -std=c++17 -O2 -Wall -Wno-switch -Werror

SRCS := main.cpp TrainerExporter.cpp

HEADERS := main.h StringUtils.h nlohmann\json.hpp

ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif

.PHONY: all clean

all: customjson$(EXE)
	@:

customjson$(EXE): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $@ $(LDFLAGS)

clean:
	$(RM) customjson customjson$(EXE)
