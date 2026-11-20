// The zox_gpu_* calls not on vulkan yet: they do nothing (no compute).
// Each part of the port moves its calls out of here (doc/plans/vulkan.md)


static inline byte zox_gpu_is_es() { return 0; }
static inline byte check_opengl_error_unlogged() { return 0; }
int check_opengl_error(char* function_name) { return 0; }
static inline int zox_gpu_error_bits() { return 0; }
byte zox_init_glew() { return EXIT_SUCCESS; }

// part 5: compute (gpu meshing); off, chunks mesh on the cpu
static inline byte zox_gpu_compute_supported() { return 0; }
guint zox_gpu_compute_program(const char* source) { return 0; }
guint zox_gpu_draw_program(const char* vertex_source, const char* fragment_source) { return 0; }
static inline void zox_gpu_compute_use(guint program) { }
static inline void zox_gpu_compute_dispatch(uint groups_x, uint groups_y, uint groups_z) { }
static inline void zox_gpu_compute_uniform_int(guint program, const char* name, int value) { }
static inline void zox_gpu_compute_uniform_uint(guint program, const char* name, uint value) { }
static inline void zox_gpu_compute_uniform_uint_array(guint program, const char* name, const uint* values, int count) { }
guint zox_gpu_storage_buffer(size_t size, const void* data) { return 0; }
static inline void zox_gpu_storage_bind(guint binding, guint buffer) { }
static inline void zox_gpu_storage_upload(guint buffer, size_t size, const void* data) { }
static inline void zox_gpu_storage_write(guint buffer, size_t offset, size_t size, const void* data) { }
static inline byte zox_gpu_storage_read(guint buffer, size_t offset, size_t size, void* out) { return 0; }
guint zox_gpu_storage_mesh_vao(guint vertices, guint indices, guint location, guint extra, guint extra_location) { return 0; }
static inline void zox_gpu_dispose_vao(guint vao) { }
static inline void zox_gpu_draw_indirect(guint vao, guint command) { }
