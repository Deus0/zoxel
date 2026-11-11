#!/usr/bin/env bash
# Converts raw/ assets into res/ data the game loads, kept out of game builds (make convert)
#   bash bsh/convert.sh --all       fonts and music
#   bash bsh/convert.sh --fonts     raw/fonts/monocraft.ttf (default_font_ttf) > res/fonts/monocraft.zox
#   bash bsh/convert.sh --music     raw/midis/${music_midi} > res/music/music.zox
#   - fonts: builds the game with --fonts (freetype), runs it once headless to convert, rebuilds
#       set zox_tst_egl_device like for tests, i.e. zox_tst_egl_device=0 on x86
#   - music: builds src/musics/cnv/midi.c on its own (libsmf: pacman -S libsmf / apt install libsmf-dev)
set -euo pipefail
cd "$(dirname "$0")/.."
music_midi="${music_midi:-songa.mid}"
do_fonts=0
do_music=0
for arg in "$@"; do
    case "${arg}" in
        --all) do_fonts=1; do_music=1 ;;
        --fonts) do_fonts=1 ;;
        --music) do_music=1 ;;
        *) echo "!! unknown option [${arg}], use --all, --fonts or --music"; exit 2 ;;
    esac
done
if [[ ${do_fonts} == "0" && ${do_music} == "0" ]]; then
    echo "usage: bash bsh/convert.sh --all | --fonts | --music"
    exit 2
fi
mkdir -p bin
failed=0

convert_fonts() {
    local font_out="res/fonts/new_font.zox"
    local font_final="res/fonts/monocraft.zox"
    echo "> fonts: building the converter (freetype)"
    if ! bash bsh/build.sh zoxel --debug --logs --fonts > bin/convert_fonts.log 2>&1; then
        grep -m 10 "error" bin/convert_fonts.log || true
        echo "!! fonts: converter build failed, see [bin/convert_fonts.log]"
        return 1
    fi
    rm -f "${font_out}"
    echo "> fonts: converting"
    bash bsh/tst.sh list --nobuild > /dev/null 2>&1 || true
    if [[ ! -s "${font_out}" ]]; then
        echo "!! fonts: no [${font_out}] was saved, see [bin/tst/list.log]"
        return 1
    fi
    local expected_size actual_size
    expected_size=$(stat -c %s "${font_final}" 2> /dev/null || echo 0)
    actual_size=$(stat -c %s "${font_out}")
    if [[ "${expected_size}" != "0" && "${expected_size}" != "${actual_size}" ]]; then
        echo "!! fonts: [${font_out}] is [${actual_size}] bytes, expected [${expected_size}], keeping the old font"
        return 1
    fi
    mv "${font_out}" "${font_final}"
    echo "+ fonts: [${font_final}]"
    echo "> fonts: rebuilding without the converter"
    bash bsh/build.sh zoxel --debug --logs > bin/convert_fonts.log 2>&1
}

convert_music() {
    if ! pkg-config --exists smf glib-2.0; then
        echo "!! music: needs libsmf and glib2 (pacman -S libsmf glib2 / apt install libsmf-dev)"
        return 1
    fi
    echo "> music: building the midi converter"
    # shellcheck disable=SC2046
    gcc -std=gnu99 src/musics/cnv/midi.c -o bin/midi_converter -lm $(pkg-config --cflags --libs glib-2.0 smf)
    echo "> music: converting [raw/midis/${music_midi}]"
    ./bin/midi_converter "raw/midis/${music_midi}" res/music/music.zox
    echo "+ music: [res/music/music.zox]"
}

if [[ ${do_fonts} == "1" ]]; then
    convert_fonts || failed=1
fi
if [[ ${do_music} == "1" ]]; then
    convert_music || failed=1
fi
exit ${failed}
