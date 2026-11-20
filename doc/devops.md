# Devops

## Get a build
- daily builds: github and codeberg releases, "zoxel-nightly" (tag nightly), the newest only (versioned releases
  later); vulkan is the flagship, opengl the fallback:
	- vulkan: windows x64, linux x64 and linux arm zips, android arm apk
	- opengl: windows x64, linux x64 and linux arm zips, android arm apks (phone and xr)
- any commit or branch: github actions, its build run, artifacts (kept 14 days)

## Build locally

| target | command | output |
|---|---|---|
| linux x64, vulkan, dev | bash bsh/build.sh zoxel --vulkan --debug --out zoxel_vulkan | bin/zoxel_vulkan/ ([plans/vulkan.md](plans/vulkan.md)) |
| linux x64 / arm, vulkan | bash bsh/docker.sh --release --x64 --vulkan (CI: build.sh --release --package --docker --vulkan) | zip/zoxel_linux_x64_vulkan_sdl_<date>.zip (shaderc in it) |
| android arm, vulkan | bash bsh/android.sh zoxel --release --signed --vulkan | zip/zoxel_android_arm_vulkan_<date>.apk |
| windows x64, vulkan | bash bsh/windows.sh --release --vulkan | zip/zoxel_windows_x64_vulkan_sdl_<date>.zip |
| windows x64, opengl | bash bsh/windows.sh --release | zip/zoxel_windows_x64_opengl_sdl_<date>.zip |
| linux x64, opengl, dev | bash bsh/build.sh | bin/zoxel_linux_x64_opengl_sdl_dev/ |
| linux x64, opengl | bash bsh/docker.sh --release --x64 | zip/zoxel_linux_x64_opengl_sdl_<date>.zip |
| android arm, opengl es | bash bsh/android_prepare.sh, then bash bsh/android.sh zoxel --release --signed | zip/zoxel_android_arm_<date>.apk |
| android arm, unsigned | bash bsh/android.sh zoxel --release --unsigned | zip/zoxel_android_arm_unsigned_<date>.apk |
| web (webgl 2, emscripten) | bash bsh/web.sh zoxel --release (--logs: logs to the page's console, --package: a zip) | bin/zoxel_web/ (index.html), zip/zoxel_web_<date>.zip |
| all platforms + itch.io | bash bsh/release.sh | zip/, then bsh/itch.sh uploads |

- a clean checkout fetches what it needs, ext/ lib/ inc/ zip/ are gitignored
	- flecs: bash bsh/flecs.sh, downloads 4.2.0 to inc/flecs and applies our [patches](flecs_patches.md)
	- SDL: build.sh downloads (bsh/libs-download.sh) and compiles it (bsh/libs-compile.sh, cmake) into lib/<os>_<arc>
- linux and windows use SDL3, windows cross compiled with mingw
- web: web.sh fetches emscripten's sdk into web/ (gitignored) the first time, sdl3 is emscripten's port
	- python: emsdk and emcc need it; on windows web.sh finds a real one or installs it with winget (user scope,
	  bsh/deps_windows.sh install_windows_python), only for web builds
	- play it: serve bin/zoxel_web (python -m http.server 8080 in it), open http://127.0.0.1:8080
	- one thread (no -pthread), saves in the browser's IndexedDB (src/_/platforms/web/pre.js), the canvas fills the
	  page (SDL_WINDOW_FILL_DOCUMENT), a click locks the mouse again after alt tab
- docker.sh builds in ubuntu 24.04, older system libs so the build runs on more distros
- bsh/deps.sh lists the apt packages, docker.sh and CI both use it, add new packages there

## CI
- github actions, .github/workflows/build.yml, ubuntu:24.04 containers (same as docker.sh)
- every push and pull request: builds vulkan first, windows x64, linux x64, linux arm (github's arm runner,
  natively), android arm; then opengl, windows x64, linux x64, linux arm, android arm and android xr (--xr, OpenXR from bsh/xr.sh);
  uploads the zips and apks as artifacts
- daily at 03:17 utc: builds master, swaps the nightly release's files (new ones uploaded first, then the old
  ones deleted, the tag moved), skipped if master didn't change
	- start it by hand: actions, build, run workflow, on master
- caches ext/ + lib/ and inc/flecs, a lib or flecs script change rebuilds them
- android: android_prepare.sh downloads the sdk, ndk and gradle into and/ (cached), android.sh signs the apk
	- with your key once it's saved as github secrets: bash bsh/android_keys.sh --github (needs gh, github-cli)
		- android_keys.sh makes the key once in ~/.config/zox_keys (android.sh calls it too), back those files up:
		  a lost key means installed apks can't be updated, only reinstalled
	- without the secrets each run makes a new key: the apk installs, but uninstall the old one before updating
- android.sh patches SDL's android-project (copied to and/<game>/) for each build:
	- app id org.<game> (org.zoxel), xr builds org.zoxel.xr, debug builds .debug on the end, the java namespace stays org.libsdl.app
	- versionCode is the build date (20261118), versionName 2026.11.18, so a newer apk updates an older one
	- launcher icons: gam/<game>/android/res/mipmap-*/ic_launcher.png replace sdl's, made from res/textures/game.bmp
	  (the window icon) with a clear background, after changing it: bash bsh/android_icons.sh (needs imagemagick)
- codeberg: forgejo actions, .forgejo/workflows/build.yml, daily at 06:17 utc (after github's), skipped if master
  didn't change, or start it by hand (actions, build, run workflow)
	- builds windows x64 vulkan, linux x64 vulkan and windows x64 opengl there, copies the rest from github's nightly release, then swaps
	  codeberg's nightly release's files as github's does (the tag moved by a git push), the notes say both commits
	- turn on actions: the repo's settings, units, overview, enable actions
	- needs the CODEBERG_TOKEN secret: codeberg, settings, applications, a token with write:repository, saved in
	  the repo's settings, actions, secrets
	- codeberg's hosted runners are small (codeberg-medium: 4 cpus, 8G, ~10 minutes by fair use, no arm), so only three
	  builds run there
	- ubuntu 24.04 as catthehacker/ubuntu:act-24.04 (the actions need node), forgejo's upload/download-artifact
	  forks (github's v4 refuses to run off github)

## Todo
- speed up codeberg's builds (.forgejo/workflows/build.yml), to fit its runners' ~10 minutes and build more there
	- time each step of a run, see where it goes (apt, libs compile, flecs, the build)
	- a prebuilt image with deps.sh's packages, no apt-get each run
	- check its cache works there (ext, lib, inc/flecs), else SDL is compiled every run
	- then build the rest there too: linux x64 opengl, android (gradle and the ndk are slow), linux arm needs an arm
	  runner (a self hosted one?)
- we should put zoxel.bin in a sub folder with the right lib files
- Fix x86 on arm builds
	- add ext/mesa compile to libs-compile.sh
		- builds libEGL.so and libGLESv2.so
	- link lib/mesa to linux-x64 in build.sh
    	- don't package Mesa runtime libraries
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
