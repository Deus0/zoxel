# Devops

## Get a build
- latest daily build: github releases, "nightly" (linux x64 and windows x64 zips)
- any commit or branch: github actions, its build run, artifacts (kept 14 days)

## Build locally

| target | command | output |
|---|---|---|
| linux x64, dev | bash bsh/build.sh | bin/zoxel_linux_x64_opengl_sdl_dev/ |
| linux x64, release | bash bsh/docker.sh --release --x64 | zip/zoxel_linux_x64_opengl_sdl_<date>.zip |
| windows x64 | bash bsh/windows.sh --release | zip/zoxel_windows_x64_opengl_sdl_<date>.zip |
| all platforms + itch.io | bash bsh/release.sh | zip/, then bsh/itch.sh uploads |

- a clean checkout fetches what it needs, ext/ lib/ inc/ zip/ are gitignored
	- flecs: bash bsh/flecs.sh, downloads 4.2.0 to inc/flecs and applies our [patches](flecs_patches.md)
	- SDL: build.sh downloads (bsh/libs-download.sh) and compiles it (bsh/libs-compile.sh, cmake) into lib/<os>_<arc>
- linux uses SDL3, windows SDL2, cross compiled with mingw
- docker.sh builds in ubuntu 24.04, older system libs so the build runs on more distros
- bsh/deps.sh lists the apt packages, docker.sh and CI both use it, add new packages there

## CI
- github actions, .github/workflows/build.yml, ubuntu:24.04 containers (same as docker.sh)
- every push and pull request: builds linux x64 and windows x64, uploads the zips as artifacts
- daily at 03:17 utc: builds master, replaces the nightly release, skipped if master didn't change
	- start it by hand: actions, build, run workflow, on master
- caches ext/ + lib/ and inc/flecs, a lib or flecs script change rebuilds them
- codeberg doesn't run it yet, see doc/todo.md Devops

## Todo
- we should put zoxel.bin in a sub folder with the right lib files
- Fix x86 on arm builds
	- add ext/mesa compile to libs-compile.sh
		- builds libEGL.so and libGLESv2.so
	- link lib/mesa to linux-x64 in build.sh
    	- don't package Mesa runtime libraries
- Fix Webgl builds
- Fix android on arm builds
- Remove the mandatory arguments on ./bsh/android.sh
- Make sure to tag docker libs properly as release

## Android
-x Terrain Material doesnt restore on minimize + restore
-x Post Processing Broken
-x Screen Rotation event not working

## Windows
- sdl_mixer is gone (audio is sdl's own, mixed in the engine, see [audio.md](audio.md)),
  libs-download.sh and libs-compile.sh still fetch and build it
