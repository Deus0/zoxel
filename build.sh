#!/usr/bin/env bash
set -euo pipefail

STATE_FILE=".build_settings"
GAME_DIR="gam"
big_timer=0

# TODO: Rename the settings to more readable
#echo "=> Platform [${OS}]"
#echo "=> Graphics [${GLB}]"
#echo "=> Windowing [${GFX}]"
#echo "=> Profile [${PRF}]"

flash_logo() {
    echo " # ! # ! # "
    echo "    - -    "
    echo "    zOx    "
    echo "    - -    "
    echo " # ! # ! # "
    sleep ${big_timer}
    clear
}

if [[ ! -d "$GAME_DIR" ]]; then
  echo "Error: $GAME_DIR folder not found."
  exit 1
fi

# Loads last used settings
load_settings() {
  source "$STATE_FILE"
  echo "Saved config found:"
  echo "GAME=$GAME"
  echo "OS=$OS"
  echo "GLB=$GLB"
  echo "GFX=$GFX"
  echo "ARC=$ARC"
  echo "PRF=$PRF"
}

# User picks new settings
pick_settings() {
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

  ARC=$(select_option "Select Architecture" x64 arm64)
  echo ""
  clear

  PRF=$(select_option "Select Profile" release development)
  echo ""
  clear
}

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
}

echo "... welcome traveler"
sleep ${big_timer}
clear

flash_logo

if [[ -f "$STATE_FILE" ]]; then
  echo "Last Used Settings:"
  echo ""
  cat "$STATE_FILE"
  echo ""
  read -rp "...Use saved config? (y/n): " USE_SAVED
  sleep ${big_timer}
  clear
else
  USE_SAVED="n"
fi

# Loads Settings
if [[ "$USE_SAVED" == "y" ]]; then
  load_settings
else
  pick_settings
fi

BUILD_SCRIPT="bsh/${OS}.sh"

if [[ ! -f "$BUILD_SCRIPT" ]]; then
  echo "Warning: $BUILD_SCRIPT is not supported or does not exist."
  exit 1
fi
clear

flash_logo

# Saves Settings
cat > "$STATE_FILE" <<EOF
GAME="$GAME"
OS="$OS"
GLB="$GLB"
GFX="$GFX"
ARC="$ARC"
PRF="$PRF"
EOF

echo "Building..."
echo ""
cat "$STATE_FILE"
echo ""
echo "...[$BUILD_SCRIPT ${GAME} ${GLB} ${GFX} ${ARC} --${PRF}]"
echo ""

bash "$BUILD_SCRIPT" ${GAME} ${GLB} ${GFX} ${ARC} --${PRF} --package
