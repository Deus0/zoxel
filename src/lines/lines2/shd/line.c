guint2 line2D_shader;
guint line2D_shader_frag;
guint line2D_material;
gint line2D_position_location;
guint line2D_color_location;
guint line2D_depth_location;
guint line2D_camera_matrix_location;

void spawn_shader_line2D(ecs *world) {
    char* vert = get_shader_source(world, "line2.vert");
    char* frag = get_shader_source(world, "line2.frag");
    line2D_shader = zox_gpu_compile_shader(vert, frag);
    if (guint2_equals(line2D_shader, guint2_zero)) {
        zox_log_error("line2D_shader has failed")
        return;
    }
    line2D_material = spawn_gpu_material_program((guint2) { line2D_shader.x, line2D_shader.y });
    if (!line2D_material) {
        zox_log_error("=> [spawn_shader_line2D] Failed:\n%s", vert);
        return;
    }
    line2D_position_location = zox_gpu_get_material_attribute(line2D_material, "position");
    line2D_color_location = zox_gpu_get_material_property(line2D_material, "color");
    line2D_depth_location = zox_gpu_get_material_property(line2D_material, "depth");
    line2D_camera_matrix_location = zox_gpu_get_material_property(line2D_material, "camera_matrix");
}
