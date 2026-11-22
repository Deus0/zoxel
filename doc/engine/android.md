# Android

Building, installing and debugging the android apks (phone and xr). Release builds and signing are in
[devops.md](devops.md), the vulkan backend in [../tech/vulkan.md](../tech/vulkan.md).

## Setup
- `bash bsh/android_prepare.sh` downloads the sdk, ndk and gradle into `and/` (cached), needs jdk 17
- `--signed` makes a key once in `~/.config/zox_keys` (bsh/android_keys.sh), back it up

## Building
`bash bsh/android.sh <game> [flags]`, the apk goes in `zip/`.

| Flag | Does |
|---|---|
| `--debug` / `--release` | the build profile (not both) |
| `--signed` / `--unsigned` | sign the apk with your key (not both) |
| `--opengl` / `--vulkan` / `--headless` | the graphics backend (opengl es by default) |
| `--xr` | an OpenXR headset build (`org.<game>.xr`) |
| `--install` | install it on the connected device |
| `--run` | start it after installing |
| `--logcat` / `--logs` | show the device's log after starting |
| `--verbose` | more build output |
| `--mixer` | build with SDL_mixer |

Common runs:
```bash
bash bsh/android.sh zoxel --debug --install --logs            # debug on a phone
bash bsh/android.sh zoxel --release --signed --install        # release on a phone
bash bsh/android.sh zoxel --release --signed --xr --install   # release on a headset
bash bsh/android.sh zoxel --vulkan --debug --logs --install   # vulkan, android 7 (api 24) up
```

The app id is `org.<game>`, with `.xr` and `.debug` added for those builds, so they install side by side.

## Installing and logs
- `bash bsh/android_install.sh [game]` installs the newest `zip/<game>_android_arm*.apk`
- `bash bsh/android_rund.sh` starts the app and follows its log (SDL, OpenXR, Zoxel, crashes)

By hand with adb:
```bash
adb install -r zip/zoxel_android_arm_<date>.apk
adb shell pidof -s org.zoxel                          # the process id
adb logcat --pid="$(adb shell pidof -s org.zoxel | tr -d '\r')" -v threadtime "Zoxel:I" "SDL:I" "*:S"
adb logcat -c                                         # clear old lines
```
