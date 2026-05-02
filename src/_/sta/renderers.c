#define zox_render_backend_headless 0
#define zox_render_backend_opengl 1
#define zox_render_backend_vulkan 2
#define zox_render_backend_directx 3

#ifdef zox_opengl
    byte render_backend = zox_render_backend_opengl;
#elif zox_headless
    byte render_backend = zox_render_backend_headless;
#elif zox_vulkan
    byte render_backend = zox_render_backend_vulkan;
#else
    byte render_backend = zox_render_backend_opengl;
#endif
