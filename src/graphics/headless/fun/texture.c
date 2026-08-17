// Textures
void zox_gpu_bind_texture(guint id) { }
void zox_gpu_set_texture_color_rgb(guint id, int2 size, const void* ptr) { }
void zox_gpu_set_texture_color_rgba(guint id, int2 size, const void* ptr) { }
void zox_gpu_clear_texture_rgba(guint id) { }
void zox_gpu_dispose_texture(guint id) { }
static inline void zox_gpu_reset_texture() { }
static inline guint spawn_gpu_texture_buffer() { return 0; }
static inline void zox_gpu_clear_texture_rgb(guint id) { }