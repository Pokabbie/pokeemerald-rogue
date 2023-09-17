CXX ?= g++

CXXFLAGS := -std=c++17 -O2 -Wall -Wno-switch -Werror

SRCS := main.cpp

HEADERS := StringUtils.h

ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif

.PHONY: all clean

all: memorystats$(EXE)
	@:

memorystats$(EXE): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $@ $(LDFLAGS)

clean:
	$(RM) memorystats memorystats$(EXE)
