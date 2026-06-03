SHELL := /bin/bash
include ./Makefile.inc

GAME_SOURCES=$(wildcard src/*.cpp)
OBJECTS_FOLDER=./obj

GAME_OBJECTS=$(GAME_SOURCES:src/%.cpp=obj/%.o)
GAME_OUTPUT_FILE=pong.out

all: game

game: $(GAME_OUTPUT_FILE)

$(GAME_OUTPUT_FILE): $(GAME_OBJECTS)
	$(LINKER) $(GAME_OBJECTS) $(LDFLAGS) -o $(GAME_OUTPUT_FILE)

obj/%.o: src/%.cpp
	@mkdir -p $(OBJECTS_FOLDER)
	$(COMPILER) -c $< $(COMPILERFLAGS) -o $@

clean:
	rm -rf $(OBJECTS_FOLDER)

pvs: clean pvs_test

pvs_test:
	@rm -rf ./bin
	@rm -rf ./obj
	@tput setaf 4
	@echo -e "\n\nRUNNING PVS-STUDIO TEST\n\n"
	@tput sgr0
	@pvs-studio-analyzer trace -- make
	@pvs-studio-analyzer analyze
	@rm -f strace_out
	@plog-converter -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t tasklist -o report.tasks PVS-Studio.log
	@rm -f PVS-Studio.log

	@if [ -s "report.tasks" ]; then \
		tput setaf 1; \
		echo -e "\n\nPVS-STUDIO ENCOUNTERED AN ERROR\n"; \
		cat report.tasks; \
	else \
		tput setaf 2; \
		echo -e "\n\nPVS-STUDIO TEST FINISHED SUCCESSFULLY\n"; \
	fi
	@tput sgr0

.PHONY: all game clean pvs pvs_test
