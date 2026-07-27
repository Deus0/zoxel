typedef struct {
    gint vertex_position;
    gint vertex_uv;
    gint position;
    gint angle;
    gint scale;
    gint camera_matrix;
    gint texture;
    gint brightness;
    gint alpha;
} MaterialTextured2D;
zoxc_custom(MaterialTextured2D);

MaterialTextured2D create_MaterialTextured2D(guint material) {
    return (MaterialTextured2D) {
        .vertex_position = zox_gpu_get_material_attribute(material, "vertex_position"),
        .vertex_uv = zox_gpu_get_material_attribute(material, "vertex_uv"),
        .position = zox_gpu_get_material_property(material, "position"),
        .angle = zox_gpu_get_material_property(material, "angle"),
        .scale = zox_gpu_get_material_property(material, "scale"),
        .camera_matrix = zox_gpu_get_material_property(material, "camera_matrix"),
        .texture = zox_gpu_get_material_property(material, "zexture"),
        .brightness = zox_gpu_get_material_property(material, "brightness"),
        .alpha = zox_gpu_get_material_property(material, "alpha") };
}
