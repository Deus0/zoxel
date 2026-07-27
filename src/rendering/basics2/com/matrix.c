typedef struct {
    gint vertex_position;
    gint vertex_uv;
    gint matrix;
    gint camera_matrix;
    gint texture;
    gint brightness;
    gint alpha;
} attributes_matrixui;
zoxc_custom(attributes_matrixui);

static inline attributes_matrixui create_attributes_matrixui(guint id) {
    return (attributes_matrixui) {
        .vertex_position = zox_gpu_get_material_attribute(id, "vertex_position"),
        .vertex_uv = zox_gpu_get_material_attribute(id, "vertex_uv"),
        .matrix = zox_gpu_get_material_property(id, "matrix"),
        .camera_matrix = zox_gpu_get_material_property(id, "camera_matrix"),
        .texture = zox_gpu_get_material_property(id, "zexture"),
        .brightness = zox_gpu_get_material_property(id, "brightness"),
        .alpha = zox_gpu_get_material_property(id, "alpha")
    };
}
