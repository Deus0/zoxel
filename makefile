# ==== Zoxel ====
# 	- remembers last picked game


SRC_DIR 	:= src
SRC    		:= src/main.c
SRCS 		:= $(shell find $(SRC_DIR) -name "*.c") # Change Detection
# CC      	:= gcc
LIBS 		:= -lm -lpthread
GAMES_DIR	:= $(SRC_DIR)/../gam
DFLAGS		:= -Iinc

# shell paths
pkg_config = $(shell which pkg-config)

# Build Args
build_args = ""
ifneq ($(filter system,$(MAKECMDGOALS)),)
	build_args += " --system"
endif
ifneq ($(filter static,$(MAKECMDGOALS)),)
	build_args += " --static"
endif
ifneq ($(filter sdl2,$(MAKECMDGOALS)),)
	build_args += " --sdl2"
endif
ifneq ($(filter sdl3,$(MAKECMDGOALS)),)
	build_args += " --sdl3"
endif
ifneq ($(filter verbose,$(MAKECMDGOALS)),)
	build_args +=" --verbose"
endif

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
target_release  	:= bin/$(GAME).bin
target_dev 	:= bin/$(GAME)-dev.bin
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

$(target_release): $(SRCS)
	@ echo "> Building [$(GAME)]"
	@ bash bsh/build.sh $(GAME) ${build_args} --release

prerelease: $(SRCS)
	@ echo "> Building [$(GAME)]"
	@ bash bsh/build.sh $(GAME) ${build_args} --logs --timings --release

package: flecs
	@ echo "> Building + Packaging [$(GAME)]"
	@ bash bsh/build.sh $(GAME) ${build_args} --release --package

package-windows: flecs
	@ echo "> Building + Packaging [$(GAME)]"
	@ bash bsh/windows.sh $(GAME) opengl sdl windows --release --package

build: flecs $(target_release)

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

$(target_dev): $(SRCS)
	bash bsh/build.sh $(GAME) ${build_args} --debug --logs --profiler # --timings


dev: $(target_dev)

profiler: $(SRCS)
	bash bsh/build.sh $(GAME) ${build_args} --debug --logs --timings --profiler

#dever: $(SRCS)
#	@ mkdir -p bin
#	$(CC) $(cflags_dever) $(SRC) -o $@ $(LIBS) $(DFLAGS)

#devmem: $(SRCS)
#	@ mkdir -p bin
#	$(CC) $(cflags_devmem) $(SRC) -o $@ $(LIBS) $(DFLAGS)

#gdbmem: devmem
#	gdb -ex "set debuginfod enabled off" -ex run --args ./$(target_dev)

# Run with windows (and debug)
runw:
	echo "Remember to enter continue"
	wine bin/$(GAME).exe

runwd:
	echo "##########################"
	echo "Enter CONTINUE [continue]"
	echo "##########################"
	winedbg --gdb bin/$(GAME)-dev.exe

# Run

run: flecs prerelease
	@ echo "> Running [$(GAME)]"
	@ sleep 1
	@ echo "-------------------"
	@ ./$(target_release)

run-release: flecs $(target_release)
	@ echo "> Running [$(GAME)]"
	@ sleep 1
	@ echo "-------------------"
	@ ./$(target_release)

run-gdb: build
	@ echo "> Running [$(GAME)]"
	@ sleep 1
	@ echo "-------------------"
	gdb -ex "set debuginfod enabled off" -ex run --args ./$(target_release)

rund: dev
	./$(target_dev)

# flecs profiler
runfp: devfp
	./$(target_dev)

runv: dev
	./$(target_dev) --verbose

# we can test using software rendering
run-gles2: build-gles2
	@LIBGL_ALWAYS_SOFTWARE=1 \
	MESA_LOADER_DRIVER_OVERRIDE=llvmpipe \
	MESA_SOFTWARE_DEVICE=llvmpipe \
	make run

gdb:
	gdb -ex "set debuginfod enabled off" -ex run --args ./$(target_dev)

rund-gdb: dev
	@ make gdb

gdbv:
	gdb -ex "set debuginfod enabled off" -ex run --args ./$(target_dev) --verbose -su

val: dev
	valgrind --track-origins=yes ./$(target_dev)

# Track memory leaks
valt: dev
	valgrind --track-origins=yes ./$(target_dev)

gdbp:
	$(MAKE) pick ACTION=gdb


# XR

xr:
	@ echo "> Building XR [$(GAME)]"
	@ sleep 1
	bash bsh/android.sh $(GAME) --xr --debug --logs --unsigned --install --run --log

runxr:
	@ echo "> Running XR [$(GAME)]"
	@ sleep 1
	bash bsh/android_rund.sh $(GAME)


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
