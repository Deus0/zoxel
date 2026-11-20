#!/usr/bin/env bash
# Compares cpu and gpu meshed terrain's screenshots (doc/plans/gpu_meshing.md), test by test, on the real gpu
#   bash bsh/compare_meshing.sh                 the default tests, real gpu
#   bash bsh/compare_meshing.sh --soft          software rendering (llvmpipe)
#   bash bsh/compare_meshing.sh --vulkan        the vulkan build
#   bash bsh/compare_meshing.sh screenshot ...  only these tests
#   --nobuild: the build as it is
# Each test runs twice with the same build and seed (zox_tst_seed 666): cpu meshing, then --gpu_meshing, into
# tst/compare_meshing/<cpu|gpu>/; every screenshot is compared with imagemagick:
#   - diff: the images' difference after a blur (dither and a pixel's shift don't count), 0 same to 1
#   - a diff image beside them (tst/compare_meshing/<shot>_diff.png), differences in red
# Some difference is expected: gpu faces are a voxel each, their light blended across the whole face (no diagonal
# seams), the cpu's big quads lit by their 4 corners. Prints a markdown table, exit 1 when a diff is over 0.08
set -uo pipefail
cd "$(dirname "$0")/.." || exit 2
tests=()
mode=(--gpu)
backend=()
build=1
for arg in "$@"; do
    case "${arg}" in
        --soft) mode=() ;;
        --vulkan) backend=(--vulkan) ;;
        --nobuild) build=0 ;;
        *) tests+=("${arg}") ;;
    esac
done
[[ ${#tests[@]} == 0 ]] && tests=(terrain_meshing_view)
if ! command -v magick > /dev/null; then
    echo "!! needs imagemagick (magick)"
    exit 2
fi
export zox_tst_seed=666
out="tst/compare_meshing"
run_meshing() {
    local meshing="$1"; shift
    local first=1
    for tst in "${tests[@]}"; do
        local args=("${mode[@]}" "${backend[@]}")
        [[ ${first} == 0 || ${build} == 0 || "${meshing}" == "gpu" ]] && args+=(--nobuild)
        [[ "${meshing}" == "gpu" ]] && args+=(--gpu_meshing)
        zox_tst_dir="${out}/${meshing}" zox_tst_out="tst_compare_meshing" \
            bash bsh/tst.sh "${tst}" "${args[@]}" > /dev/null 2>&1
        echo "  ${meshing} ${tst}: exit $?"
        first=0
    done
}
echo "> running [${tests[*]}]"
mkdir -p "${out}/cpu" "${out}/gpu"
run_meshing cpu
run_meshing gpu
rmse() {
    magick compare -metric RMSE "$1" "$2" null: 2>&1 | sed -n 's/.*(\([0-9.e-]*\)).*/\1/p'
}
failed=0
echo ""
echo "| test | screenshot | diff |"
echo "|---|---|---|"
for tst in "${tests[@]}"; do
    for cpu_shot in "${out}"/cpu/${tst}*.ppm; do
        [[ -f "${cpu_shot}" ]] || continue
        name="$(basename "${cpu_shot}" .ppm)"
        gpu_shot="${out}/gpu/${name}.ppm"
        if [[ ! -f "${gpu_shot}" ]]; then
            echo "| ${tst} | ${name} | no gpu shot |"
            failed=1
            continue
        fi
        work="$(mktemp -d)"
        magick "${cpu_shot}" -blur 0x2 "${work}/cpu.png"
        magick "${gpu_shot}" -blur 0x2 "${work}/gpu.png"
        diff="$(rmse "${work}/cpu.png" "${work}/gpu.png")"
        rm -r "${work}"
        magick "${cpu_shot}" "${out}/cpu/${name}.png"
        magick "${gpu_shot}" "${out}/gpu/${name}.png"
        magick compare "${cpu_shot}" "${gpu_shot}" "${out}/${name}_diff.png" 2> /dev/null
        flag=""
        awk -v d="${diff:-1}" 'BEGIN { exit !(d > 0.08) }' && flag=" !! differ" && failed=1
        printf '| %s | %s | %.4f |%s\n' "${tst}" "${name}" "${diff:-1}" "${flag}"
    done
done
exit ${failed}
