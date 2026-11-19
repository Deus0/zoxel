// gl's switches (blend, depth, cull..): kept, and made part of the pipeline at the next draw (fun/draw.c)

static inline void zox_gpu_enable_culling() { vk_cull = 1; }
static inline void zox_gpu_disable_culling() { vk_cull = 0; }
static inline void zox_gpu_enable_depth_test() { vk_depth_test = 1; }
static inline void zox_gpu_disable_depth_test() { vk_depth_test = 0; }
static inline void zox_gpu_set_depth_mask(byte is_depth_mask) { vk_depth_write = is_depth_mask; }
static inline void zox_gpu_set_color_mask(byte is_color) { vk_color_write = is_color; }
static inline void zox_gpu_enable_blend() { vk_blend = 1; }
static inline void zox_gpu_enable_blend2() { vk_blend = 1; }
static inline void zox_gpu_disable_blend() { vk_blend = 0; }
static inline void zox_gpu_enable_polyoffset() { vk_bias_fill = 1; }
static inline void zox_gpu_disable_polyoffset() { vk_bias_fill = 0; }
static inline void zox_gpu_enable_polyoffset_lines() { vk_bias_lines = 1; }
static inline void zox_gpu_disable_polyoffset_lines() { vk_bias_lines = 0; }
static inline void zox_gpu_enable_points() { }

static inline void zox_gpu_polyoffset(float a, float b) {
    vk_bias_factor = a;
    vk_bias_units = b;
    vk_dynamic_dirty = 1;
}

static inline void zox_gpu_line_thickness(float thickness) {
    vk_line_width = thickness;
    vk_dynamic_dirty = 1;
}

// gl's viewport: from the bottom left
static inline void zox_gpu_set_viewport(float2 position, float2 size) {
    vk_viewport = (float4) { position.x, position.y, size.x, size.y };
    vk_viewport_set = 1;
    vk_dynamic_dirty = 1;
}
