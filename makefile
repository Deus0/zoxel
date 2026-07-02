# ==== Zoxel ====
# 	- remembers last picked game


SRC_DIR 	:= src
SRC    		:= src/main.c
SRCS 		:= $(shell find $(SRC_DIR) -name "*.c") # Change Detection
CC      	:= gcc
LIBS 		:= -lm -lpthread
GAMES_DIR	:= $(SRC_DIR)/../gam
DFLAGS		:= -Iinc

# shell paths
pkg_config = $(shell which pkg-config)

# 🧱 Release build — for speed and glory
# 03 breaks my sounds for now
CFLAGS      	:= -fPIC -O3 -march=native -flto=auto -DNDEBUG -Dzox_debug -Dzox_opengl

# 🐛 Debug build — for truth and stacktraces
#	-g3 -g -Werror  -std=c11
cflags_dev 	:= -fPIC -O0 -g3 -Wall -ggdb3 -Dzox_debug -Dzox_opengl

# more checks
cflags_dever	:= $(cflags_dev)  -Wextra -Wpedantic -pedantic-errors -Werror  -fdiagnostics-color=always # -std=c99

# std=c99 gnu99
# memory leak catching
# more checks
cflags_devmem	:= $(cflags_dever) -Wpedantic -fsanitize=address -fno-omit-frame-pointer -D_POSIX_C_SOURCE=200809L


### Choose your Game!
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
TARGET  	:= bin/$(GAME).bin
TARGET_DEV 	:= bin/$(GAME)-dev.bin
DFLAGS 		+= -Dzox_game=$(GAME)

# Add SDL
LIBS += $(shell $(pkg_config) --libs sdl2 SDL2_image SDL2_mixer)
DFLAGS += -Dzox_sdl -Dzox_sdl_mixer -Dzox_sdl_images

# Find and add GL Libaries
LIBS_GL := $(shell $(pkg_config) --libs egl glesv2)
ifeq ($(LIBS_GL),)
	LIBS_GL := $(shell $(pkg_config) --libs gl)
endif
LIBS += $(LIBS_GL)

# Flecs Direct Support Now
ifeq ("$(wildcard inc/flecs/flecs.c)","")
    # System Flecs Linker Flag
    LIBS += -lflecs
else
    # Use Source Directly
    SRC += inc/flecs/flecs.c
    DFLAGS += -Dflecssource # -Iinc/flecs
endif

.PHONY: game help clean build dev pick run rund runp runpd gdb val flecs flecs-package
# .DEFAULT_GOAL := build


# Build

$(TARGET): $(SRCS)
	@ echo "> Building [$(GAME)]"
	@ bash bsh/linux.sh $(GAME) opengl sdl $(shell uname -m) --release
	# $(CC) $(CFLAGS) $(SRC) -o $@ $(LIBS) $(DFLAGS)

package: flecs
	@ echo "> Building + Packaging [$(GAME)]"
	@ bash bsh/linux.sh $(GAME) opengl sdl $(shell uname -m) --release --package

package-windows: flecs
	@ echo "> Building + Packaging [$(GAME)]"
	@ bash bsh/windows.sh $(GAME) opengl sdl windows --release --package

build: flecs $(TARGET)

build-gles2: flecs
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS) $(DFLAGS) -Dzox_gles2

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
	bash bsh/flecs.sh

# Dev

$(TARGET_DEV): $(SRCS)
	@ mkdir -p bin
	bash bsh/linux.sh $(GAME) opengl sdl $(shell uname -m) --debug

dev: $(TARGET_DEV)

# flecs profiler
devfp: $(SRCS)
	@ mkdir -p bin
	$(CC) $(cflags_dev) $(SRC) -o $(TARGET_DEV) $(LIBS) $(DFLAGS) -Dzox_use_flecs_profiler

dever: $(SRCS)
	@ mkdir -p bin
	$(CC) $(cflags_dever) $(SRC) -o $@ $(LIBS) $(DFLAGS)

devmem: $(SRCS)
	@ mkdir -p bin
	$(CC) $(cflags_devmem) $(SRC) -o $@ $(LIBS) $(DFLAGS)

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

# flecs profiler
runfp: devfp
	./$(TARGET_DEV)

runv: dev
	./$(TARGET_DEV) --verbose

run-gles2: build-gles2
	@LIBGL_ALWAYS_SOFTWARE=1 \
	MESA_LOADER_DRIVER_OVERRIDE=llvmpipe \
	MESA_SOFTWARE_DEVICE=llvmpipe \
	make run

gdb: dev
	gdb -ex "set debuginfod enabled off" -ex run --args ./$(TARGET_DEV)

gdbv: dev
	gdb -ex "set debuginfod enabled off" -ex run --args ./$(TARGET_DEV) --verbose -su

val: dev
	valgrind --track-origins=yes ./$(TARGET_DEV)

# Track memory leaks
valt: dev
	valgrind --track-origins=yes ./$(TARGET_DEV)

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
