//! Links to locations inside a base material->
typedef struct {
    gint vertex_position;
    uint camera_matrix;
    uint position;
    //uint positionY;
    uint angle;
    uint scale;
    uint brightness;
} Material2D;

void initialize_material2D_properties(Material2D *material2D, uint material) {
    material2D->camera_matrix = zox_gpu_get_material_property(material, "camera_matrix");
    material2D->angle = zox_gpu_get_material_property(material, "angle");
    material2D->scale = zox_gpu_get_material_property(material, "scale");
    material2D->brightness = zox_gpu_get_material_property(material, "brightness");
    material2D->position = zox_gpu_get_material_property(material, "position");
    material2D->vertex_position = zox_gpu_get_material_attribute(material, "vertex_position");
}
