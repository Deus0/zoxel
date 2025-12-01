# ==== Zoxel ====
# 	- remembers last picked game
# Choose your game module
LAST_GAME_FILE := .game

ifeq ($(origin game), undefined)
    ifeq ($(shell test -f $(LAST_GAME_FILE) && echo yes),)
        GAME := zoxel
    else
        GAME := $(shell cat $(LAST_GAME_FILE))
    endif
else
    GAME := $(game)
endif


SRC_DIR 	:= src
SRC    		:= src/main.c
SRCS 		:= $(shell find $(SRC_DIR) -name "*.c") # Change Detection
CC      	:= gcc

# 🧱 Release build — for speed and glory
# 03 breaks my sounds for now
CFLAGS      	:= -fPIC -O3 -march=native -flto=auto -DNDEBUG -Dzox_debug

# 🐛 Debug build — for truth and stacktraces
#	-g3 -g -Werror  -std=c11
cflags_dev 	:= -fPIC -O0 -g3 -Wall -ggdb3 -Dzox_debug

# more checks
cflags_dever	:= $(cflags_dev)  -Wextra -Wpedantic -pedantic-errors -Werror  -fdiagnostics-color=always # -std=c99

# std=c99 gnu99
# memory leak catching
# more checks
cflags_devmem	:= $(cflags_dever) -Wpedantic -fsanitize=address -fno-omit-frame-pointer -D_POSIX_C_SOURCE=200809L

LDFLAGS 	:= -lm -lpthread -lGL -lSDL2 -lSDL2_image -lSDL2_mixer \
			-Dzox_sdl -Dzox_sdl_mixer -Dzox_sdl_images \
			-Dzox_game=$(GAME)
TARGET  	:= bin/$(GAME)
TARGET_DEV 	:= bin/$(GAME)-debug
GAMES_DIR	:= $(SRC_DIR)/nexus

# Flecs Direct Support Now
ifeq ("$(wildcard inc/flecs/flecs.c)","")
    # System Flecs Linker Flag
    LDFLAGS += -lflecs
else
    # Use Source Directly
    SRC += inc/flecs/flecs.c
    LDFLAGS += -Iinc/flecs -Dflecssource
endif

.PHONY: game help clean build dev pick run rund runp runpd gdb val flecs flecs-package
# .DEFAULT_GOAL := build


# Build

$(TARGET): $(SRCS)
	@ echo "> Building [$(GAME)]"
	@ mkdir -p bin
	@ echo "-------------------"
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LDFLAGS)
	@ echo " - completed -"
	@ echo "-------------------"

build: flecs $(TARGET)

# Extra

game:
	@ echo "> You are working on [$(GAME)]"

help:
	@ cat doc/help.md

clean:
	@ echo "> You are removing [bin dist]"
	@ rm -rf bin dist

# installs flecs source directly
flecs:
	@if pkg-config --exists flecs; then \
		echo "> Flecs Package Installed"; \
	else \
		if test -f inc/flecs/flecs.c; then \
			echo "> Flecs Source Installed"; \
		else \
			echo "+ Installing Flecs Source..."; \
			mkdir -p inc/flecs; \
			cd inc/flecs && \
			wget https://raw.githubusercontent.com/SanderMertens/flecs/v4.0.5/distr/flecs.h -O flecs.h && \
			wget https://raw.githubusercontent.com/SanderMertens/flecs/v4.0.5/distr/flecs.c -O flecs.c || { echo >&2 "Download failed"; exit 1; } \
		fi; \
	fi

# Dev

$(TARGET_DEV): $(SRCS)
	@ mkdir -p bin
	$(CC) $(cflags_dev) $(SRC) -o $@ $(LDFLAGS)

dev: $(TARGET_DEV)

dever: $(SRCS)
	@ mkdir -p bin
	$(CC) $(cflags_dever) $(SRC) -o $@ $(LDFLAGS)

devmem: $(SRCS)
	@ mkdir -p bin
	$(CC) $(cflags_devmem) $(SRC) -o $@ $(LDFLAGS)

gdbmem: devmem
	gdb -ex "set debuginfod enabled off" -ex run --args ./$(TARGET_DEV)

# Run

run: build
	@ echo "> Running [$(GAME)]"
	@ sleep 1
	@ echo "-------------------"
	@ ./$(TARGET)

rund: dev
	./$(TARGET_DEV)

runv: dev
	./$(TARGET_DEV) --verbose

gdb: dev
	gdb -ex "set debuginfod enabled off" -ex run --args ./$(TARGET_DEV)

gdbv: dev
	gdb -ex "set debuginfod enabled off" -ex run --args ./$(TARGET_DEV) --verbose

val: dev
	valgrind ./$(TARGET_DEV)

gdbp:
	$(MAKE) pick ACTION=gdb

# Dep

flecs-package:
	cd ../flecsing && make clean && make download && make refresh



# Pick

ACTION ?= build

pick:
	@echo "Scanning the Games...";\
	i=1; \
	for d in $(GAMES_DIR)/*/; do \
		gamename=$$(echo $$d | sed 's|$(GAMES_DIR)/||; s|/$$||'); \
		echo "$$i) $$gamename"; \
		eval "game_$$i=$$gamename"; \
		i=$$((i + 1)); \
	done; \
	read -p "Pick a game by number: " choice; \
	selected_game=$$(eval echo \$$$$(echo game_$$choice)); \
	if [ -n "$$selected_game" ]; then \
		echo "$$selected_game" > $(LAST_GAME_FILE); \
		echo "You selected: $$selected_game $$ACTION"; \
		$(MAKE) game=$$selected_game $$ACTION; \
	else \
		echo "Invalid selection."; \
		exit 1; \
	fi

runp:
	$(MAKE) pick ACTION=run

runpd:
	$(MAKE) pick ACTION=rund
