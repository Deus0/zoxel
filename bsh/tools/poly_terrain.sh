#!/usr/bin/env bash
# A poly terrain's heights as an image, to see what the generator made (the game's --poly-map,
#   src/terrains/poly/dbg/_.c)
#   bash bsh/tools/poly_terrain.sh [--seed <n>] [--nobuild] [extra game args]
#   - the same seed draws the same image (666 unless --seed)
#   - writes tst/poly/heights.ppm, and heights.png with imagemagick; 4 pixels a cell, x right, z down, lit from the
#     north west, the sea blue over the ground
set -uo pipefail
cd "$(dirname "$0")/../.."
seed=666
build=1
args=()
while [[ $# -gt 0 ]]; do
    case "$1" in
        --seed) seed="$2"; shift 2 ;;
        --nobuild) build=0; shift ;;
        *) args+=("$1"); shift ;;
    esac
done
if [[ ${build} == "1" ]]; then
    bash bsh/build.sh zeland --debug --logs > /dev/null || { echo "!! build failed"; exit 1; }
fi
bin=$(ls -t bin/zeland_*_opengl_*_dev/zeland-dev.bin 2>/dev/null | head -1)
if [[ -z "${bin}" ]]; then
    echo "!! no dev binary in bin/zeland_*_dev"
    exit 1
fi
out="$(pwd)/tst/poly"
mkdir -p "${out}"
rm -f "${out}"/*.ppm "${out}"/*.png
(cd "$(dirname "${bin}")" && nice -n 10 timeout 120 "./$(basename "${bin}")" --poly-map --poly-seed "${seed}" \
    --poly-out "${out}" --background --nosaves "${args[@]}" 2>&1 | grep -E "\[poly\]|ERROR|WARN")
if command -v magick > /dev/null; then
    for ppm in "${out}"/*.ppm; do
        [[ -f "${ppm}" ]] && magick "${ppm}" "${ppm%.ppm}.png"
    done
fi
ls "${out}"
