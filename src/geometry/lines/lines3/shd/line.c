// uses custom fadeout with fog!
guint2 line3D_shader;
guint line3D_material;
guint line3D_position_location;
guint line3D_color_location;
guint line3D_camera_matrix_location;
guint line3D_fog_data_location;

// instanced: every line in one draw, like particles (line3D_instanced.vert)
//  - line_t is per vertex (0, 1), the line's start, end and color are per instance
guint2 line3D_instanced_shader;
guint line3D_instanced_material;
guint line3D_instanced_t_location;
guint line3D_instanced_start_location;
guint line3D_instanced_end_location;
guint line3D_instanced_color_location;
guint line3D_instanced_camera_matrix_location;
guint line3D_instanced_fog_data_location;
guint line3D_instanced_t_buffer;
guint line3D_instanced_data_buffer;     // LineData3D (start, end) per line
guint line3D_instanced_color_buffer;    // Color per line
int line3D_instanced_capacity = 0;      // lines the buffers hold, grows

void dispose_shader_line3D() {
    zox_gpu_dispose_shader(line3D_shader.x);
    zox_gpu_dispose_shader(line3D_shader.y);
    zox_dispose_material(line3D_material);
    if (line3D_instanced_material) {
        zox_gpu_dispose_shader(line3D_instanced_shader.x);
        zox_gpu_dispose_shader(line3D_instanced_shader.y);
        zox_dispose_material(line3D_instanced_material);
        zox_gpu_dispose_buffer(line3D_instanced_t_buffer);
        zox_gpu_dispose_buffer(line3D_instanced_data_buffer);
        zox_gpu_dispose_buffer(line3D_instanced_color_buffer);
        line3D_instanced_capacity = 0;
    }
}

// the instance buffers hold at least count lines
static inline void line3D_instanced_reserve(int count) {
    if (count <= line3D_instanced_capacity) {
        return;
    }
    int capacity = line3D_instanced_capacity ? line3D_instanced_capacity : 256;
    while (capacity < count) {
        capacity *= 2;
    }
    zox_gpu_bind_buffer_array(line3D_instanced_data_buffer);
    zox_gpu_set_buffer_array(NULL, sizeof(LineData3D) * capacity);
    zox_gpu_bind_buffer_array(line3D_instanced_color_buffer);
    zox_gpu_set_buffer_array(NULL, sizeof(Color) * capacity);
    zox_gpu_bind_buffer_array(0);
    line3D_instanced_capacity = capacity;
}

int initialize_shader_line3D_instanced(ecs *world) {
    char* vert = get_shader_source(world, "line3D_instanced.vert");
    char* frag = get_shader_source(world, "line3D_instanced.frag");
    line3D_instanced_shader = zox_gpu_compile_shader(vert, frag);
    if (guint2_equals(line3D_instanced_shader, guint2_zero)) {
        zox_loge("line3D_instanced_shader has failed")
        return EXIT_FAILURE;
    }
    line3D_instanced_material = spawn_gpu_material_program(line3D_instanced_shader);
    if (!line3D_instanced_material) {
        zox_loge("=> [initialize_shader_line3D_instanced] Failed:\n%s", vert);
        return EXIT_FAILURE;
    }
    line3D_instanced_t_location = zox_gpu_get_material_attribute(line3D_instanced_material, "line_t");
    line3D_instanced_start_location = zox_gpu_get_material_attribute(line3D_instanced_material, "line_start");
    line3D_instanced_end_location = zox_gpu_get_material_attribute(line3D_instanced_material, "line_end");
    line3D_instanced_color_location = zox_gpu_get_material_attribute(line3D_instanced_material, "line_color");
    line3D_instanced_camera_matrix_location = zox_gpu_get_material_property(line3D_instanced_material, "camera_matrix");
    line3D_instanced_fog_data_location = zox_gpu_get_material_property(line3D_instanced_material, "fog_data");
    // a line's two vertices, the same for every instance
    const float ts[2] = { 0, 1 };
    line3D_instanced_t_buffer = zox_gpu_create_buffer();
    zox_gpu_bind_buffer_array(line3D_instanced_t_buffer);
    zox_gpu_set_buffer_array(ts, sizeof(ts));
    line3D_instanced_data_buffer = zox_gpu_create_buffer();
    line3D_instanced_color_buffer = zox_gpu_create_buffer();
    zox_gpu_bind_buffer_array(0);
    line3D_instanced_reserve(256);
    return EXIT_SUCCESS;
}

int initialize_shader_line3D(ecs *world) {
    char* vert = get_shader_source(world, "line3D.vert");
    char* frag = get_shader_source(world, "line3D.frag");
    line3D_shader = zox_gpu_compile_shader(vert, frag);
    if (guint2_equals(line3D_shader, guint2_zero)) {
        zox_loge("line3D_shader has failed")
        return EXIT_FAILURE;
    }
    line3D_material = spawn_gpu_material_program((guint2) { line3D_shader.x, line3D_shader.y });
    if (!line3D_material) {
        zox_loge("=> [initialize_shader_line3D] Failed:\n%s", vert);
        return EXIT_FAILURE;
    }
    line3D_position_location = zox_gpu_get_material_attribute(line3D_material, "position");
    line3D_color_location = zox_gpu_get_material_property(line3D_material, "color");
    line3D_camera_matrix_location = zox_gpu_get_material_property(line3D_material, "camera_matrix");
    line3D_fog_data_location = zox_gpu_get_material_property(line3D_material, "fog_data");
#if !defined(zox_disable_instancing)
    if (initialize_shader_line3D_instanced(world) != EXIT_SUCCESS) {
        line3D_instanced_material = 0;
    }
#endif
    return EXIT_SUCCESS;
}
