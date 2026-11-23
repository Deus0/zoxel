#!/usr/bin/env bash
# Flies a camera over the terrain to look at lods changing (tst lod_flyby): builds, runs it, saves
#   tst/flyby/lod_flyby/flyby.gif (the flight), blink_<n>.png (its worst frames: before, the blink in magenta, after)
#   and the numbers: blinks (a pixel changing for a frame and back) and light pops (brighter or darker, the same colour)
#   bash bsh/tools/flyby.sh                    opengl, software (the tests' default)
#   bash bsh/tools/flyby.sh --vulkan --gpu     vulkan on the real gpu (gpu meshing)
#   bash bsh/tools/flyby.sh --1080p            the game's size (the tests' window is small for looking at)
#   zox_tst_flyby_speed=2 bash bsh/tools/flyby.sh    chunks a second (2 by default)
#   bash bsh/tools/flyby.sh --far 8          a nearer far lod (16 by default, the game's)
#   zox_tst_gif_fps=60 bash bsh/tools/flyby.sh       frames a second taken (30 by default)
#   its own build and logs (bin/tst_flyby, tst/flyby): runs beside a full test run
set -uo pipefail
cd "$(dirname "$0")/../.."
# a slow frame rate (software rendering, the tests' default) flies slowly: room for it
export zox_tst_timeout="${zox_tst_timeout:-400}"
export zox_tst_dir="${zox_tst_dir:-tst/flyby}"
# the game's lods (terrain_lod_far 16, the tests' --tiny is 3): the lower ones far out are what change as it flies
[[ " $* " == *" --far "* || " $* " == *" --tiny "* || " $* " == *" --medium "* || " $* " == *" --large "* ]] || set -- "$@" --far 16
output=$(bash bsh/tst.sh lod_flyby "$@" 2>&1)
code=$?
echo "${output}" | grep -a "lod_flyby\|PASSED\|FAIL\|exit \[\|build failed" | grep -av "^ *|"
exit ${code}
