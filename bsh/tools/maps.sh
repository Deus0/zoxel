#!/usr/bin/env bash
# The maps of 3 x 3 regions round region 0, 0 as images, to see what world generation made (the game's --maps,
#   src/towns/dbg/maps_dump.c): height, biome, vegetation, town, lod, overview; blocks with --chunks
#   bash bsh/tools/maps.sh [--seed <n>] [--chunks] [--nobuild] [extra game args]
#   - the same seed draws the same maps (666 unless --seed)
#   - tunks only by default (their maps, no chunks or meshes); --chunks streams chunks too for the top blocks map
#   - writes tst/maps/<map>.ppm, and <map>.png beside each with imagemagick; a pixel a block, x right, z down
#   - a world of its own, no player: a stream point in the middle of region 0, 0, far 68 (a circle reaching the 9
#     regions' corners)
set -uo pipefail
cd "$(dirname "$0")/../.."
seed=666
build=1
mode="--maps"
args=()
while [[ $# -gt 0 ]]; do
    case "$1" in
        --seed) seed="$2"; shift 2 ;;
        --chunks) mode="--maps-chunks"; shift ;;
        --nobuild) build=0; shift ;;
        *) args+=("$1"); shift ;;
    esac
done
if [[ ${build} == "1" ]]; then
    bash bsh/build.sh zoxel --debug --logs > /dev/null || { echo "!! build failed"; exit 1; }
fi
bin=$(ls -t bin/zoxel_*_opengl_*_dev/zoxel-dev.bin 2>/dev/null | head -1)
if [[ -z "${bin}" ]]; then
    echo "!! no dev binary in bin/zoxel_*_dev"
    exit 1
fi
out="$(pwd)/tst/maps"
mkdir -p "${out}"
rm -f "${out}"/*.ppm "${out}"/*.png
(cd "$(dirname "${bin}")" && nice -n 10 timeout 900 "./$(basename "${bin}")" ${mode} --maps-seed "${seed}" --maps-out "${out}" \
    --background --nosaves --far 68 "${args[@]}" 2>&1 | grep -E "\[maps\]|ERROR|WARN")
if command -v magick > /dev/null; then
    for ppm in "${out}"/*.ppm; do
        [[ -f "${ppm}" ]] && magick "${ppm}" "${ppm%.ppm}.png"
    done
fi
ls "${out}"
