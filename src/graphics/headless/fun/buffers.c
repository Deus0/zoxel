// Buffers
uint zox_gpu_create_buffer() { }
void zox_gpu_bind_buffer_element(uint v) { }
void zox_gpu_bind_buffer_array(uint v) { }
void zox_gpu_set_buffer_element(const void* ptr, uint length) { }
void zox_gpu_set_buffer_array(const void* ptr, uint length) { }
void zox_gpu_dispose_buffer(uint id) { }
static inline void zox_gpu_set_sub_buffer_float3(uint length, const void* ptr) { }
static inline void zox_gpu_set_sub_buffer_color(uint length, const void* ptr) { }


static inline void opengl_enable_vertex_buffer(uint shader_index, uint vertex_buffer) { }
static inline void opengl_enable_uv_buffer(uint shader_index, uint uv_buffer) { }
static inline void opengl_enable_color_buffer(uint shader_index, uint color_buffer) { }
static inline void zox_gpu_disable_buffer(uint shader_index) { }
static inline void zox_gpu_array_buffer(uint link, int length, byte byte_size, void* ptr) { }
static inline void zox_gpu_element_buffer(uint link, int length, byte byte_size, void* ptr) { }
