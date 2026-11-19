// The zox_gpu_* calls not on vulkan yet: they do nothing, so the game runs but draws only the clear color.
// Each part of the port moves its calls out of here (doc/plans/vulkan.md)

byte zox_fbos_supported = 0;
extern byte zox_disable_post_processing;

static inline byte zox_gpu_is_es() { return 0; }
static inline byte check_opengl_error_unlogged() { return 0; }
int check_opengl_error(char* function_name) { return 0; }
static inline int zox_gpu_error_bits() { return 0; }
int zox_get_safe_ubo_size() { return 1024; }
byte zox_init_glew() { return EXIT_SUCCESS; }

// part 2: shaders and materials
guint2 zox_gpu_compile_shader(const gchar* vert_buffer, const gchar* frag_buffer) { return guint2_zero; }
guint spawn_gpu_material_program(guint2 shader) { return 0; }
static inline void zox_gpu_dispose_shader(guint id) { }
static inline void zox_gpu_dispose_program(guint program) { }
static inline void zox_dispose_material(guint value) { }
static inline gint zox_gpu_get_material_property(guint id, const char* name) { return -1; }
static inline gint zox_gpu_get_material_attribute(guint id, const char* name) { return -1; }
static inline guint zox_gpu_get_material_uniform(guint id, const char* name) { return 0; }
static inline void zox_gpu_material(guint material) { }
static inline void zox_disable_material() { }
static inline void zox_gpu_log_program_inputs(guint program, const char *shader_name) { }
static inline void zox_gpu_int(gint index, int value) { }
static inline void zox_gpu_float(gint index, float value) { }
static inline void zox_gpu_float2(gint index, float2 value) { }
static inline void zox_gpu_float3(gint index, float3 value) { }
static inline void zox_gpu_float4(gint index, float4 value) { }
static inline void zox_gpu_float4x4(gint index, float4x4 value) { }
static inline void zox_gpu_float4x4_array(gint index, const float4x4* values, int count) { }
static inline void zox_gpu_float3_array(gint index, const float3* values, gsizei count) { }
static inline void zox_gpu_color_rgb(gint index, color_rgb value) { }
static inline void zox_gpu_color(gint index, color value) { }
static inline void zox_gpu_program_int(guint program, const char* name, int value) { }
static inline void zox_gpu_program_float4(guint program, const char* name, float4 value) { }
static inline void zox_gpu_program_float4_array(guint program, const char* name, const float4* values, int count) { }
static inline void zox_gpu_program_float4x4(guint program, const char* name, float4x4 value) { }

// part 3: buffers, attributes, pipeline state and draws
static inline guint zox_gpu_create_buffer() { return 0; }
static inline guint2 spawn_gpu_mesh_buffers() { return guint2_zero; }
static inline void zox_gpu_bind_buffer_element(guint v) { }
static inline void zox_gpu_bind_buffer_array(guint v) { }
static inline void zox_gpu_set_buffer_element(const void* ptr, gsize length) { }
static inline void zox_gpu_set_buffer_array(const void* ptr, gsize length) { }
static inline void zox_gpu_dispose_buffer(guint link) { }
static inline void zox_gpu_set_sub_buffer_float3(gsize length, const void* ptr) { }
static inline void zox_gpu_set_sub_buffer_color(gsize length, const void* ptr) { }
static inline void zox_gpu_set_sub_buffer_bytes(gsize bytes, const void* ptr) { }
static inline void zox_gpu_array_buffer(guint link, gsize length, byte byte_size, void* ptr) { }
static inline void zox_gpu_element_buffer(guint link, gsize length, byte byte_size, void* ptr) { }
static inline void opengl_enable_vertex_buffer(guint shader_index, guint vertex_buffer) { }
static inline void opengl_enable_uv_buffer(guint shader_index, guint uv_buffer) { }
static inline void opengl_enable_color_buffer(guint shader_index, guint color_buffer) { }
static inline void zox_gpu_array_buffer_byte(guint shader_index, guint buffer) { }
static inline void zox_gpu_array_buffer_byte4(guint shader_index, guint buffer) { }
static inline void zox_gpu_reset_mesh() { }
static inline void zox_gpu_disable_attribute(guint id) { }
static inline void zox_gpu_enable_attribute(guint id) { }
static inline void zox_gpu_attribute_divisor(guint id, guint value) { }
static inline void zox_gpu_enable_attribute_float2(guint id) { }
static inline void zox_gpu_set_attribute_float2(guint id, const void* ptr) { }
static inline void zox_gpu_enable_set_attribute_float2(guint id, const void* ptr) { }
static inline void zox_gpu_enable_attribute_float3(guint id, const void* ptr) { }
static inline void zox_gpu_set_attribute_float3(guint id, const void* ptr) { }
static inline void zox_gpu_set_attribute_color(guint id, const void* ptr) { }
static inline void zox_gpu_set_attribute_float3_strided(guint id, gsizei stride, gsize offset) { }
static inline void zox_gpu_set_attribute_floatx(guint id, const void* ptr, byte length) { }
static inline void zox_gpu_set_viewport(float2 position, float2 size) { }
static inline void zox_gpu_enable_culling() { }
static inline void zox_gpu_disable_culling() { }
static inline void zox_gpu_enable_depth_test() { }
static inline void zox_gpu_disable_depth_test() { }
static inline void zox_gpu_set_depth_mask(byte is_depth_mask) { }
static inline void zox_gpu_set_color_mask(byte is_color) { }
static inline void zox_gpu_enable_blend() { }
static inline void zox_gpu_enable_blend2() { }
static inline void zox_gpu_disable_blend() { }
static inline void zox_gpu_enable_polyoffset() { }
static inline void zox_gpu_disable_polyoffset() { }
static inline void zox_gpu_polyoffset(float a, float b) { }
static inline void zox_gpu_enable_polyoffset_lines() { }
static inline void zox_gpu_disable_polyoffset_lines() { }
static inline void zox_gpu_render(gsizei length) { }
static inline void zox_gpu_render3(gsizei length) { }
static inline void zox_gpu_render_as_lines(gsizei length) { }
static inline void zox_gpu_render_triangles_instanced(gsizei indicies, gsizei length) { }
static inline void zox_gpu_enable_points() { }
static inline void zox_gpu_render_points(gsizei length) { }
static inline void zox_gpu_render_points_instanced(gsizei length) { }
static inline void zox_gpu_line_thickness(float thickness) { }
static inline void zox_gpu_render_lines(gsizei length) { }
static inline void zox_gpu_render_lines_instanced(gsizei count) { }

// part 3: textures
static inline guint spawn_gpu_texture_buffer() { return 0; }
static inline void zox_gpu_bind_texture(guint id) { }
static inline void zox_gpu_reset_texture() { }
static inline void zox_gpu_set_texture_color_rgb(guint id, int2 size, const void* ptr) { }
static inline void zox_gpu_set_texture_color_rgba(guint id, int2 size, const void* ptr) { }
static inline void zox_gpu_clear_texture_rgb(guint id) { }
static inline void zox_gpu_clear_texture_rgba(guint id) { }
static inline void zox_gpu_dispose_texture(guint id) { }

// part 4: uniform buffers (instanced voxes, bones), render textures
static inline void zox_gpu_ubo_set_matricies(guint id, gsizei length, const void* ptr) { }
static inline void zox_gpu_ubo_stream_matricies(guint id, gsizei length, gsizei capacity, const void* ptr) { }
static inline void zox_gpu_ubo_initialize_matricies(guint id, guint binding_point, gsizei length) { }
static inline void zox_gpu_ubo_block_binding(guint program, guint block_index, guint binding) { }
static inline void zox_gpu_ubo_reset() { }
static inline guint zox_gpu_create_fbo() { return 0; }
static inline void zox_gpu_bind_fbo(guint id) { }
static inline void zox_gpu_reset_fbo() { }
static inline void zox_gpu_fbo_to_texture(guint fbo, guint texture) { }
static inline void zox_gpu_link_fbo_rbo(guint fbo, guint rbo) { }
static inline void zox_gpu_dispose_fbo(guint id) { }
static inline guint zox_gpu_create_rbo() { return 0; }
static inline void zox_gpu_set_rbo_size(guint id, int2 size) { }
static inline void zox_gpu_dispose_rbo(guint id) { }

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
