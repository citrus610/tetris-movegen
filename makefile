CXX = g++

ifeq ($(PROF), true)
CXXPROF += -pg -no-pie
else
CXXPROF += -s
endif

ifeq ($(BUILD), debug)
CXXFLAGS += -fdiagnostics-color=always -std=c++20 -Wall -Og -pg -no-pie
else
CXXFLAGS += -DNDEBUG -std=c++20 -O3 -flto $(CXXPROF) -march=native
endif

ifeq ($(PEXT), true)
CXXFLAGS += -DPEXT
endif

.PHONY: all test clean makedir

all: test

test: makedir
	@$(CXX) $(CXXFLAGS) src/*.cpp -o bin/test.exe

clean: makedir
	@rm -rf bin
	@make makedir

makedir:
	@mkdir -p bin

.DEFAULT_GOAL := test