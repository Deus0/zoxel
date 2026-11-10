#!/usr/bin/env bash
set -euo pipefail

mkdir -p tst

# fetch tests from modules/submodules in src
# check if input from terminal
#   - check if input is in src tests
# if no input, let user select using a number entered
#   - print the tests found
