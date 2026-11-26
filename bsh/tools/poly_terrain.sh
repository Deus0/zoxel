#!/usr/bin/env bash
# A poly terrain's heightmap and four screenshots (the game's --poly-map,
#   src/terrains/poly/dbg/_.c)
#   bash bsh/tools/poly_terrain.sh [--seed <n>] [--nobuild] [extra game args]
#   - the same seed draws the same image (666 unless --seed)
#   - writes tst/poly/heights.ppm, and heights.png with imagemagick; 4 pixels a cell, x right, z down, lit from the
#     north west, the sea blue over the ground
#   - writes four poly_view images and a 2 x 2 views.png sheet
set -uo pipefail
cd "$(dirname "$0")/../.."
seed=666
build=1
args=()
view_args=()
while [[ $# -gt 0 ]]; do
    case "$1" in
        --seed) seed="$2"; shift 2 ;;
        --nobuild) build=0; view_args+=(--nobuild); shift ;;
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
rm -f "${out}/heights."{ppm,png} "${out}"/poly_views/poly_view_{1,2,3,4}.ppm "${out}"/poly_view_{1,2,3,4}.png "${out}/views.png" || exit 1
(cd "$(dirname "${bin}")" && nice -n 10 timeout 120 "./$(basename "${bin}")" --poly-map --poly-seed "${seed}" \
    --poly-out "${out}" --background --nosaves "${args[@]}" 2>&1 | grep -E "\[poly\]|ERROR|WARN")
[[ -s "${out}/heights.ppm" ]] || exit 1
zox_tst_game=zeland zox_tst_dir=tst/poly zox_tst_seed="${seed}" \
    bash bsh/tst.sh poly_views "${view_args[@]}" || exit 1
magick "${out}/heights.ppm" "${out}/heights.png" || exit 1
for view in poly_view_{1,2,3,4}; do
    magick "${out}/poly_views/${view}.ppm" "${out}/${view}.png" || exit 1
done
magick \
    \( "${out}/poly_view_1.png" "${out}/poly_view_2.png" +append \) \
    \( "${out}/poly_view_3.png" "${out}/poly_view_4.png" +append \) \
    -append "${out}/views.png" || exit 1
ls "${out}/heights."{ppm,png} "${out}"/poly_views/poly_view_{1,2,3,4}.ppm "${out}"/poly_view_{1,2,3,4}.png "${out}/views.png"
