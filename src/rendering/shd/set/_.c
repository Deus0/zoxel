#if defined(zox_android)
    byte is_shaders_es = 1;
    ushort shader_opengl_version = 300;
#elif defined(zox_windows)
    byte is_shaders_es = 1;
    ushort shader_opengl_version = 300;
#elif defined(zox_gles2)
    byte is_shaders_es = 0;
    ushort shader_opengl_version = 100;
#else
    byte is_shaders_es = 1;
    ushort shader_opengl_version = 300;
#endif
