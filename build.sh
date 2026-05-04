#!/usr/bin/env bash
set -euo pipefail

GAME_DIR="gam"
timer=0 #1
big_timer=3

echo "... welcome traveler"
sleep ${big_timer}
clear

if [[ ! -d "$GAME_DIR" ]]; then
  echo "Error: $GAME_DIR folder not found."
  exit 1
fi

select_option() {

    local prompt="$1"
    shift
    local options=("$@")
    local choice

    printf '%s\n' "$prompt" >&2

    for i in "${!options[@]}"; do
        printf '%d) %s\n' "$((i + 1))" "${options[$i]}" >&2
    done

    read -rp "Enter number: " choice

    if ! [[ "$choice" =~ ^[0-9]+$ ]] || (( choice < 1 || choice > ${#options[@]} )); then
        echo "Invalid selection" >&2
        exit 1
    fi

    printf '%s\n' "${options[$((choice - 1))]}"

    sleep ${timer}
}

echo " # ! # ! # "
echo "    - -    "
echo "    zOx    "
echo "    - -    "
echo " # ! # ! # "
sleep ${big_timer}
clear

# 1) Pick Game
echo "Select game:"
mapfile -t GAMES < <(find "$GAME_DIR" -mindepth 1 -maxdepth 1 -type d -exec basename {} \;)
if [[ ${#GAMES[@]} -eq 0 ]]; then
  echo "Error: No game folders found in $GAME_DIR"
  exit 1
fi
for i in "${!GAMES[@]}"; do
  echo "$((i+1))) ${GAMES[$i]}"
done
read -rp "Enter number: " GAME_INDEX
if ! [[ "$GAME_INDEX" =~ ^[0-9]+$ ]] || (( GAME_INDEX < 1 || GAME_INDEX > ${#GAMES[@]} )); then
  echo "Invalid selection"
  exit 1
fi
GAME="${GAMES[$((GAME_INDEX-1))]}"
clear

OS=$(select_option "Select Platform:" linux windows android webgl)
echo ""
clear

GLB=$(select_option "Select Graphics:" opengl vulkan headless)
echo ""
clear

GFX=$(select_option "Select Windowing" sdl glut glfw headless)
echo ""
clear

PRF=$(select_option "Select Profile" release development)
echo ""
clear


BUILD_SCRIPT="bsh/${OS}-${GFX}-${GLB}.sh"

if [[ ! -f "$BUILD_SCRIPT" ]]; then
  echo "Warning: $BUILD_SCRIPT is not supported or does not exist."
  exit 1
fi
clear

#echo ""
# read -rp "CPU (arm64, x86_64): " CPU
# read -rp "OS (linux, windows, android, webgl): " OS
# read -rp "Window (glut, sdl, glfw): " GFX
# read -rp "Graphics (opengl, vulkan, headless): " GLB


echo "Building ${GAME}"
echo "=> Platform [${OS}]"
echo "=> Graphics [${GLB}]"
echo "=> Windowing [${GFX}]"
echo "=> Profile [${PRF}]"

echo " # ! # ! # "
echo "+ Calling [$BUILD_SCRIPT] +"
echo "# Args [${GAME} --${PRF}] #"
echo " # ! # ! # "

bash "$BUILD_SCRIPT" ${GAME} --${PRF}
