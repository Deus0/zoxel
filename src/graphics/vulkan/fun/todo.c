// gl only calls the renderer makes (errors, es, glew): nothing to do on vulkan


static inline byte zox_gpu_is_es() { return 0; }
static inline byte check_opengl_error_unlogged() { return 0; }
int check_opengl_error(char* function_name) { return 0; }
static inline int zox_gpu_error_bits() { return 0; }
byte zox_init_glew() { return EXIT_SUCCESS; }
