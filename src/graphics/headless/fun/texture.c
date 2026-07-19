// Textures

void zox_gpu_bind_texture(uint id) { }
void zox_gpu_set_texture_color_rgb(uint id, int2 size, const void* ptr) { }
void zox_gpu_set_texture_color_rgba(uint id, int2 size, const void* ptr) { }
void zox_gpu_clear_texture_rgba(uint id) { }}
void zox_gpu_dispose_texture(uint id) { }

static inline void opengl_bind_texture(uint texture_buffer) { }
static inline void opengl_reset_texture() { }
static inline guint spawn_gpu_texture_buffer() { return 0; }
