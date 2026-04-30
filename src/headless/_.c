/*
 *  Zoxel Headless
 *
 *      - Stub Functions for no gpu
 *
 * */
#ifndef zoxm_headless
#define zoxm_headless

// stub functions
uint2 zox_gpu_compile_shader(const gchar* vert_buffer, const gchar* frag_buffer) { }

static uint create_shader_vertex() { return 0; }
static uint create_shader_fragment() { return 0; }
static uint create_shader_compute() { return 0; }
static inline uint spawn_gpu_generic_buffer() { return 0; }

void zox_dispose_material(uint value) { }
void zox_gpu_dispose_texture(uint value) { }
void zox_gpu_bind_buffer_element(uint v) { }
void zox_gpu_bind_buffer_array(uint v) { }
void zox_gpu_set_buffer_element(const void* ptr, uint length) { }
void zox_gpu_set_buffer_array(const void* ptr, uint length) { }
void zox_gpu_dispose_buffer(uint link) { }

zox_begin_module(Headless)

zox_end_module(Headless)

#endif
