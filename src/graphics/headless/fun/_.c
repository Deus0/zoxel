#include "uniform.c"
#include "buffers.c"
#include "material.c"
#include "texture.c"
#include "settings.c"
#include "viewport.c"
#include "ubo.c"
#include "points.c"
#include "lines.c"
#include "attributes.c"
#include "shaders.c"

byte test_graphics() { return EXIT_SUCCESS; }

static inline void zox_gpu_reset_fbo() { }
static inline byte check_opengl_error_unlogged() { return 0; }