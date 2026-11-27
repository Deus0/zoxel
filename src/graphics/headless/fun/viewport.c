void zox_gpu_clear_viewport() { }
static inline void zox_gpu_clear_rect(float2 position, float2 size) { }
static inline void zox_gpu_set_clear_color(float4 c) { }
static inline void zox_gpu_set_viewport(float2 position, float2 size) { }

static inline void zox_gpu_scissor(const int4* rect) { }
static inline float4 zox_gpu_frag_viewport(float2 position, float2 size) { return (float4) { 0, 0, 1, 1 }; }
