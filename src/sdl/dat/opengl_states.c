// different opengl modes with sdl
#define zox_opengl_core 0
#define zox_opengl_es 1
#define zox_opengl_compatibility 2

byte override_opengl_es = 0;
#ifdef zox_android
    byte opengl_mode = zox_opengl_es;
    byte sdl_gl_major = 3;
    byte sdl_gl_minor = 0;
    byte is_shaders_es = 1;
    ushort shader_opengl_version = 300;
#elifdef zox_windows
    byte opengl_mode = zox_opengl_core;
    byte sdl_gl_major = 3;
    byte sdl_gl_minor = 0;
    byte is_shaders_es = 1;
    ushort shader_opengl_version = 300;
#elifdef zox_gles2
    byte opengl_mode = zox_opengl_es;
    byte sdl_gl_major = 2;
    byte sdl_gl_minor = 0;
    byte is_shaders_es = 0;
    ushort shader_opengl_version = 100;
#else
    byte opengl_mode = zox_opengl_core;
    byte sdl_gl_major = 3;
    byte sdl_gl_minor = 0;
    byte is_shaders_es = 1;
    ushort shader_opengl_version = 300;
#endif
