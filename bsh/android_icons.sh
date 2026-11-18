#!/usr/bin/env bash
set -euo pipefail
# remakes the android launcher icons from the window icon (res/textures/game.bmp, 64x64), its black background clear
#   bash bsh/android_icons.sh [game]    writes gam/<game>/android/res/mipmap-*/ic_launcher.png (android.sh copies them)
# needs imagemagick (magick), only when the icon changes, the pngs are committed
game_name="${1:-zoxel}"
out="gam/${game_name}/android/res"
for size in mdpi:48 hdpi:72 xhdpi:96 xxhdpi:144 xxxhdpi:192; do
    mkdir -p "${out}/mipmap-${size%%:*}"
    # the 4 corners flood filled clear, then scaled up as pixels
    magick res/textures/game.bmp -alpha set -fill none \
        -draw 'color 0,0 floodfill' -draw 'color 63,0 floodfill' -draw 'color 0,63 floodfill' -draw 'color 63,63 floodfill' \
        -filter point -resize "${size##*:}x${size##*:}" -strip "PNG32:${out}/mipmap-${size%%:*}/ic_launcher.png"
done
echo "+ Icons [${out}/mipmap-*/ic_launcher.png]"
