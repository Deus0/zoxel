#!/usr/bin/env bash
# Builds the zedit editor with logs kept (a release build strips zox_log, an editor wants them), any other
#   bsh/build.sh arguments passed on
#   bash bsh/zedit.sh              build (release, with logs)
#   bash bsh/zedit.sh --run        build and run it
#   bash bsh/zedit.sh --debug      a debug build (logs are on there anyway)
#   bash bsh/zedit.sh --vulkan     the vulkan backend
# Later: games could set their own build defaults (logs on for zedit) instead of this wrapper.
set -uo pipefail
cd "$(dirname "$0")/.." || exit 1
bash bsh/build.sh zedit --logs "$@"
