#include <jni.h>
#ifdef zox_vulkan
    #include <vulkan/vulkan.h>
#else
    #include <EGL/egl.h>
    #include <GLES3/gl3.h>
#endif
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
