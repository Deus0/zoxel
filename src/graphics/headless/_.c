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
static inline void zox_gpu_bind_fbo(uint id) { }
static inline void zox_gpu_link_fbo(uint fbo, uint texture) { }
static inline void zox_gpu_link_fbo_rbo(uint fbo, uint rbo) { }
static inline void zox_gpu_dispose_fbo(uint fbo) { }

// RBOs

static inline void zox_gpu_set_rbo_size(uint id, int2 size) { }
static inline void zox_gpu_dispose_rbo(uint id) { }

// Shaders
void zox_gpu_dispose_shader(uint id) { }

// Triangles

static inline void zox_gpu_render(uint length) { }
static inline void zox_gpu_render_triangles_instanced(uint indicies, uint length) { }

byte zox_init_glew() { return EXIT_SUCCESS; }

#include "fun/_.c"

zox_begin_module(Headless)

zox_end_module(Headless)

#endif
