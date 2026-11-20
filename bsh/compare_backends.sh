#!/usr/bin/env bash
# Compares the opengl and vulkan builds' screenshots, test by test (a material or view each), on the real gpu
#   bash bsh/compare_backends.sh                  the default tests, real gpu (--gpu: lavapipe hid radv's bugs)
#   bash bsh/compare_backends.sh --soft           software rendering (llvmpipe and lavapipe)
#   bash bsh/compare_backends.sh sky_clouds ...   only these tests
#   --nobuild: the builds as they are (bin/tst_compare_gl, bin/tst_compare_vulkan)
# Each test runs once a backend (zox_tst_seed 666), into tst/compare/<backend>/; every screenshot it saves is
# compared with imagemagick:
#   - diff: the images' difference after a blur (dither, grain and a pixel's shift don't count), 0 same to 1
#   - noise: each image's fine detail (it against its blurred self); a backend much noisier than the other is a bug
#     (the vulkan sky's undefined alpha on radv, 2026-11-20)
# Prints a markdown table (for doc/metrics.md), exit 1 when a diff is over 0.08 or a noise ratio over 1.5
set -uo pipefail
cd "$(dirname "$0")/.." || exit 2
tests=()
mode=(--gpu)
build=1
for arg in "$@"; do
    case "${arg}" in
        --soft) mode=() ;;
        --nobuild) build=0 ;;
        *) tests+=("${arg}") ;;
    esac
done
# one a material or view: sky, vox models, instances (slimes), particles, lines, under water tint (render
#  textures), the game (terrain, ui, post processing), bodies (bones), text
[[ ${#tests[@]} == 0 ]] && tests=(sky_clouds model_views instance_views particles_render lines_render underwater screenshot body_views font_glyphs)
if ! command -v magick > /dev/null; then
    echo "!! needs imagemagick (magick)"
    exit 2
fi
export zox_tst_seed=666
run_backend() {
    local backend="$1"; shift
    local first=1
    for tst in "${tests[@]}"; do
        local args=("${mode[@]}")
        [[ ${first} == 0 || ${build} == 0 ]] && args+=(--nobuild)
        [[ "${backend}" == "vulkan" ]] && args+=(--vulkan)
        zox_tst_dir="tst/compare/${backend}" zox_tst_out="tst_compare_${backend}" \
            bash bsh/tst.sh "${tst}" "${args[@]}" > /dev/null 2>&1
        echo "  ${backend} ${tst}: exit $?"
        first=0
    done
}
echo "> running [${tests[*]}]"
mkdir -p tst/compare/opengl tst/compare/vulkan
run_backend opengl
run_backend vulkan
# a number from compare's "a (b)" output, b normalised 0-1
rmse() {
    magick compare -metric RMSE "$1" "$2" null: 2>&1 | sed -n 's/.*(\([0-9.e-]*\)).*/\1/p'
}
blurred() {
    magick "$1" -blur 0x2 "$2"
}
failed=0
echo ""
echo "| test | screenshot | diff | noise opengl | noise vulkan |"
echo "|---|---|---|---|---|"
for tst in "${tests[@]}"; do
    for gl_shot in tst/compare/opengl/${tst}*.ppm; do
        [[ -f "${gl_shot}" ]] || continue
        local_name="$(basename "${gl_shot}")"
        vk_shot="tst/compare/vulkan/${local_name}"
        if [[ ! -f "${vk_shot}" ]]; then
            echo "| ${tst} | ${local_name} | no vulkan shot | | |"
            failed=1
            continue
        fi
        work="$(mktemp -d)"
        blurred "${gl_shot}" "${work}/gl.png"
        blurred "${vk_shot}" "${work}/vk.png"
        diff="$(rmse "${work}/gl.png" "${work}/vk.png")"
        noise_gl="$(rmse "${gl_shot}" "${work}/gl.png")"
        noise_vk="$(rmse "${vk_shot}" "${work}/vk.png")"
        rm -r "${work}"
        flag=""
        awk -v d="${diff:-1}" 'BEGIN { exit !(d > 0.08) }' && flag=" !! differ" && failed=1
        awk -v a="${noise_gl:-0}" -v b="${noise_vk:-0}" 'BEGIN { exit !(b > a * 1.5 + 0.005 || a > b * 1.5 + 0.005) }' \
            && flag="${flag} !! noise" && failed=1
        printf '| %s | %s | %.4f | %.4f | %.4f |%s\n' "${tst}" "${local_name}" "${diff:-1}" "${noise_gl:-0}" "${noise_vk:-0}" "${flag}"
    done
done
exit ${failed}
