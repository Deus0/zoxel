# Notes

## Unsorted
=> GPU Constrained actually, memory barely 
used - 200-400mb used

## Render Cameras
- RenderBufferLink is on camera
- FrameBufferLink is on camera and texture?

## ADB
- adb shell pidof org.libsdl.app
- adb logcat --pid=20922 -v threadtime (20922 is the process id)
- adb logcat --pid=20922 -v threadtime "SDL:I" "*:S"
- adb logcat --pid="$(adb shell pidof -s org.libsdl.app | tr -d '\r')" -v threadtime "SDL:I" "*:S"
- adb install -r /home/deus/projects/zoxel/zip/zoxel_android_arm_2026_08_17.apk

