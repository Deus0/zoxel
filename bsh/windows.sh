#!/usr/bin/env bash
set -euo pipefail

bash bsh/build.sh --windows --x64 --package "$@"
