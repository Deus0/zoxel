// 2D lines' material, an entity (restored with the rest), its uniform locations a component on it
typedef struct {
    gint position;
    gint color;
    gint depth;
    gint camera_matrix;
} MaterialLine2D;
zoxc_custom(MaterialLine2D);

MaterialLine2D create_MaterialLine2D(guint material) {
    return (MaterialLine2D) {
        .position = zox_gpu_get_material_attribute(material, "position"),
        .color = zox_gpu_get_material_property(material, "color"),
        .depth = zox_gpu_get_material_property(material, "depth"),
        .camera_matrix = zox_gpu_get_material_property(material, "camera_matrix")
    };
}

entity material_line2D = 0;

void spawn_shader_line2D(ecs *world) {
    guint material;
    material_line2D = spawn_material_source(world, "material_line2D", "line2.vert", "line2.frag", &material);
    if (material) {
        const MaterialLine2D data = create_MaterialLine2D(material);
        zox_set_data(material_line2D, MaterialLine2D, data);
    }
}

void gpu_restore_lines2(ecs* world) {
    zox_gpu_relocate(MaterialLine2D);
}
