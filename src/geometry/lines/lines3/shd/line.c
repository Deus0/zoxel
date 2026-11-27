// uses custom fadeout with fog!
//  - its shader and material are entities (restored with the rest), their uniform locations a component on the material
typedef struct {
    gint position;
    gint color;
    gint camera_matrix;
} MaterialLine3D;
zoxc_custom(MaterialLine3D);

MaterialLine3D create_MaterialLine3D(guint material) {
    return (MaterialLine3D) {
        .position = zox_gpu_get_material_attribute(material, "position"),
        .color = zox_gpu_get_material_property(material, "color"),
        .camera_matrix = zox_gpu_get_material_property(material, "camera_matrix"),
    };
}

// instanced: every line in one draw, like particles (line3D_instanced.vert)
//  - line_t is per vertex (0, 1), the line's start, end and color are per instance
typedef struct {
    gint t;
    gint start;
    gint end;
    gint color;
    gint camera_matrix;
} MaterialLine3DInstanced;
zoxc_custom(MaterialLine3DInstanced);

MaterialLine3DInstanced create_MaterialLine3DInstanced(guint material) {
    return (MaterialLine3DInstanced) {
        .t = zox_gpu_get_material_attribute(material, "line_t"),
        .start = zox_gpu_get_material_attribute(material, "line_start"),
        .end = zox_gpu_get_material_attribute(material, "line_end"),
        .color = zox_gpu_get_material_attribute(material, "line_color"),
        .camera_matrix = zox_gpu_get_material_property(material, "camera_matrix"),
    };
}

entity material_line3D = 0;
entity material_line3D_instanced = 0;
guint line3D_instanced_t_buffer;
guint line3D_instanced_data_buffer;     // LineData3D (start, end) per line
guint line3D_instanced_color_buffer;    // Color per line
int line3D_instanced_capacity = 0;      // lines the buffers hold, grows

// the line material bound, its fog and camera set: its locations
static inline const MaterialLine3D* line3D_material_begin(ecs* world) {
    const MaterialLine3D* line3D = zox_valid(material_line3D) ? zox_get(material_line3D, MaterialLine3D) : NULL;
    if (line3D) {
        zox_gpu_material(zox_getv(material_line3D, MaterialGPULink));
        zox_gpu_enable_attribute(line3D->position);
        zox_gpu_float4x4(line3D->camera_matrix, render_camera_matrix);
    }
    return line3D;
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

// the instance buffers, plain gl buffers: made with the gpu, let go of with it (gpu_dispose / gpu_restore hooks)
void spawn_line3D_instanced_buffers(ecs* world) {
    (void) world;
    // a line's two vertices, the same for every instance
    const float ts[2] = { 0, 1 };
    line3D_instanced_t_buffer = zox_gpu_create_buffer();
    zox_gpu_bind_buffer_array(line3D_instanced_t_buffer);
    zox_gpu_set_buffer_array(ts, sizeof(ts));
    line3D_instanced_data_buffer = zox_gpu_create_buffer();
    line3D_instanced_color_buffer = zox_gpu_create_buffer();
    zox_gpu_bind_buffer_array(0);
    line3D_instanced_capacity = 0;
    line3D_instanced_reserve(256);
}

void dispose_line3D_instanced_buffers(ecs* world) {
    (void) world;
    zox_gpu_dispose_buffer(line3D_instanced_t_buffer);
    zox_gpu_dispose_buffer(line3D_instanced_data_buffer);
    zox_gpu_dispose_buffer(line3D_instanced_color_buffer);
    line3D_instanced_t_buffer = line3D_instanced_data_buffer = line3D_instanced_color_buffer = 0;
    line3D_instanced_capacity = 0;
}

void initialize_shader_line3D(ecs *world) {
    guint material;
    material_line3D = spawn_material_source(world, "material_line3D", "line3D.vert", "line3D.frag", &material);
    if (material) {
        const MaterialLine3D data = create_MaterialLine3D(material);
        zox_set_data(material_line3D, MaterialLine3D, data);
    }
#if !defined(zox_disable_instancing)
    const entity e = spawn_material_source(world, "material_line3D_instanced", "line3D_instanced.vert", "line3D_instanced.frag", &material);
    if (material) {
        const MaterialLine3DInstanced data = create_MaterialLine3DInstanced(material);
        zox_set_data(e, MaterialLine3DInstanced, data);
        material_line3D_instanced = e;
        spawn_line3D_instanced_buffers(world);
    }
#endif
}

void gpu_dispose_lines3(ecs* world) {
    if (material_line3D_instanced) {
        dispose_line3D_instanced_buffers(world);
    }
}

void gpu_restore_lines3(ecs* world) {
    zox_gpu_relocate(MaterialLine3D);
    zox_gpu_relocate(MaterialLine3DInstanced);
    if (material_line3D_instanced) {
        spawn_line3D_instanced_buffers(world);
    }
}
