/*
 *  Zoxel Headless
 *
 *      - Stub Functions for no gpu
 *      - boy this is taking time
 *
 * */
#ifndef zoxm_headless
#define zoxm_headless

// stub functions

void zox_gpu_clear_viewport() { }

// Settings

static inline void zox_gpu_enable_culling() { }
static inline void zox_gpu_disable_culling() { }
static inline void zox_gpu_enable_depth_test() { }
static inline void zox_gpu_disable_depth_test() { }
static inline void zox_gpu_enable_blend() { }
static inline void zox_gpu_disable_blend() { }

// Shaders

uint2 zox_gpu_compile_shader(const gchar* vert_buffer, const gchar* frag_buffer) { }
static uint create_shader_vertex() { return 0; }
static uint create_shader_fragment() { return 0; }
static uint create_shader_compute() { return 0; }

static inline uint spawn_gpu_generic_buffer() { return 0; }

// Materials

static inline void zox_dispose_material(uint value) { }
static inline uint zox_gpu_get_material_property(uint id, const char* name) { }
static inline uint zox_gpu_get_material_attribute(uint id, const char* name) { }

// Textures
void zox_gpu_bind_texture(uint id) { }
void zox_gpu_set_texture_color_rgb(uint id, int2 size, const void* ptr) { }
void zox_gpu_set_texture_color_rgba(uint id, int2 size, const void* ptr) { }
void zox_gpu_dispose_texture(uint id) { }


// Frame Buffers

static inline void zox_gpu_bind_fbo(uint id) { }
static inline void zox_gpu_link_fbo(uint fbo, uint texture) { }
static inline void zox_gpu_link_fbo_rbo(uint fbo, uint rbo) { }
static inline void zox_gpu_dispose_fbo(uint fbo) { }

// RBOs

static inline void zox_gpu_set_rbo_size(uint id, int2 size) { }
static inline void zox_gpu_dispose_rbo(uint id) { }

// UBOs

void zox_gpu_ubo_set_matricies(uint id, uint length, const void* ptr) { }
void zox_gpu_ubo_initialize_matricies(uint id, uint binding_point, uint length) { }
void zox_gpu_ubo_reset() { }
int zox_get_safe_ubo_size() { }

// Shaders
void zox_gpu_dispose_shader(uint id) { }

// Attributes
void zox_gpu_enable_attribute_float2(uint id) { }
void zox_gpu_set_attribute_float2(uint id, const void* ptr) { }
void zox_gpu_enable_set_attribute_float2(uint id,  const void* ptr) { }
void zox_gpu_enable_attribute_float3(uint id) { }
void zox_gpu_set_attribute_float3(uint id, const void* ptr) { }
void zox_gpu_set_attribute_color(uint id, const void* ptr) { }

// Triangles

static inline void zox_gpu_render(uint length) { }
static inline void zox_gpu_render_triangles_instanced(uint indicies, uint length) { }

// Lines

static inline void zox_gpu_line_thickness(float thickness) { }
static inline void zox_gpu_render_lines(uint length) { }

// Points

static inline void zox_gpu_render_points(uint length) { }
static inline void zox_gpu_render_points_instanced(uint length) { }

#include "fun/_.c"

zox_begin_module(Headless)

zox_end_module(Headless)

#endif
