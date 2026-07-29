/*
 *  Zoxel Headless
 *
 *      - Stub Functions for no gpu
 *      - boy this is taking time
 *
 * */
#ifndef zoxm_headless
#define zoxm_headless

// Shaders
guint2 zox_gpu_compile_shader(const gchar* vert_buffer, const gchar* frag_buffer) { return guint2_zero; }
// static uint create_shader_vertex() { return 0; }
// static uint create_shader_fragment() { return 0; }
static uint create_shader_compute() { return 0; }


// Frame Buffers

static inline uint zox_gpu_create_rbo() { return 0; }
static inline uint zox_gpu_create_fbo() {return 0; }
static inline void zox_gpu_bind_fbo(guint id) { }
static inline void zox_gpu_fbo_to_texture(guint fbo, guint texture) { }
static inline void zox_gpu_link_fbo_rbo(guint fbo, guint rbo) { }
static inline void zox_gpu_dispose_fbo(guint fbo) { }

// RBOs

static inline void zox_gpu_set_rbo_size(guint id, int2 size) { }
static inline void zox_gpu_dispose_rbo(guint id) { }

// Shaders
void zox_gpu_dispose_shader(guint id) { }

// Triangles

static inline void zox_gpu_render(guint length) { }
static inline void zox_gpu_render3(guint length) { }
static inline void zox_gpu_render_as_lines(guint length) { }
static inline void zox_gpu_render_triangles_instanced(guint indicies, uint length) { }

byte zox_init_glew() { return EXIT_SUCCESS; }

#include "fun/_.c"

zox_begin_module(Headless) {
    zox_log("Zox Headless is Enabled");
} zox_end_module(Headless);

#endif
