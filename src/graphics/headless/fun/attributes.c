// Attributes
static inline void zox_gpu_disable_attribute(guint id) { }
static inline void zox_gpu_enable_attribute(guint id) { }
static inline void zox_gpu_attribute_divisor(guint id, guint value) { }
void zox_gpu_enable_attribute_float2(guint id) { }
void zox_gpu_set_attribute_float2(guint id, const void* ptr) { }
void zox_gpu_enable_set_attribute_float2(guint id,  const void* ptr) { }
void zox_gpu_enable_attribute_float3(guint id, const void* ptr) { }
void zox_gpu_set_attribute_float3(guint id, const void* ptr) { }
void zox_gpu_set_attribute_color(guint id, const void* ptr) { }
static inline void zox_gpu_int(gint index, int value) { }