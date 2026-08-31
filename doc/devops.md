# Devops

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
- SDL3Mixer not compiling for windows
