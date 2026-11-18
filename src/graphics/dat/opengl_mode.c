// the opengl the app asked sdl for (core or es), set by the platform and --gles, sdl falls back to es
#define zox_opengl_core 0
#define zox_opengl_es 1
#define zox_opengl_compatibility 2

#if  defined(zox_android)
    byte opengl_mode = zox_opengl_es;
    byte sdl_gl_major = 3;
    byte sdl_gl_minor = 0;
#elif defined(zox_windows)
    byte opengl_mode = zox_opengl_core;
    byte sdl_gl_major = 3;
    byte sdl_gl_minor = 0;
#elif defined(zox_gles2)
    byte opengl_mode = zox_opengl_es;
    byte sdl_gl_major = 2;
    byte sdl_gl_minor = 0;
#else
    byte opengl_mode = zox_opengl_core;
    byte sdl_gl_major = 3;
    byte sdl_gl_minor = 0;
#endif
