#!/usr/bin/env bash
set -euo pipefail

# Runs zoxel tests included in src directory
# each module might have a tst folder
# scans them first and logs which ones do

# Runs zoxel tests included in src directory.
# A module is identified by its _.c file containing:
#   zox_begin_module
#   zox_module
#
# Reports:
#   - total modules found
#   - modules with tst folders
#   - non-_.c files inside each tst folder

total_modules=0
modules_with_tests=0

while IFS= read -r module_file; do
    module_dir="$(dirname "$module_file")"
    module_name="$(basename "$module_dir")"

    if grep -qE 'zox_begin_module|zox_module' "$module_file"; then
        ((total_modules += 1))

        tst_dir="${module_dir}/tst"

        if [[ -d "$tst_dir" ]]; then
            ((modules_with_tests += 1))

            test_count="$(
                find "$tst_dir" -maxdepth 1 -type f ! -name '_.c' | wc -l
            )"

            echo "${module_name}: ${test_count} tests"
        fi
    fi
done < <(
    find src -type f -name '_.c' | sort
)

echo
echo "Modules found: ${total_modules}"
echo "Modules with tst folders: ${modules_with_tests}"
