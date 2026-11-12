# Devops

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
