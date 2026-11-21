// particles' material, an entity (restored with the rest), its uniform locations a component on it
typedef struct {
    gint position;
    gint color;
    gint size;
    gint camera_matrix;
    gint fog_data;
    gint thickness;
} MaterialParticle3D;
zoxc_custom(MaterialParticle3D);

MaterialParticle3D create_MaterialParticle3D(guint material) {
    return (MaterialParticle3D) {
        .position = zox_gpu_get_material_attribute(material, "position"),
        .color = zox_gpu_get_material_attribute(material, "color"),
        .size = zox_gpu_get_material_attribute(material, "size"),
        .camera_matrix = zox_gpu_get_material_property(material, "camera_matrix"),
        .fog_data = zox_gpu_get_material_property(material, "fog_data"),
        .thickness = zox_gpu_get_material_property(material, "thickness")
    };
}

entity material_particle3D = 0;
// instance buffers, plain gl buffers: made with the gpu, let go of with it (gpu_dispose / gpu_restore hooks)
guint particle3D_instanced_position_buffer;
guint particle3D_instanced_color_buffer;
guint particle3D_instanced_size_buffer;

void spawn_particle3D_buffers(ecs* world) {
    (void) world;
#if !defined(zox_disable_particles_gpu_instancing)
    particle3D_instanced_position_buffer = zox_gpu_create_buffer();
    zox_gpu_bind_buffer_array(particle3D_instanced_position_buffer);
    zox_gpu_set_buffer_array(NULL, sizeof(Position3D) * zox_max_particles3D);
    particle3D_instanced_color_buffer = zox_gpu_create_buffer();
    zox_gpu_bind_buffer_array(particle3D_instanced_color_buffer);
    zox_gpu_set_buffer_array(NULL, sizeof(Color) * zox_max_particles3D);
    particle3D_instanced_size_buffer = zox_gpu_create_buffer();
    zox_gpu_bind_buffer_array(particle3D_instanced_size_buffer);
    zox_gpu_set_buffer_array(NULL, sizeof(Scale1) * zox_max_particles3D);
    zox_gpu_bind_buffer_array(0);
#endif
}

void dispose_particle3D_buffers(ecs* world) {
    (void) world;
    zox_gpu_dispose_buffer(particle3D_instanced_position_buffer);
    zox_gpu_dispose_buffer(particle3D_instanced_color_buffer);
    zox_gpu_dispose_buffer(particle3D_instanced_size_buffer);
    particle3D_instanced_position_buffer = particle3D_instanced_color_buffer = particle3D_instanced_size_buffer = 0;
}

void initialize_shader_particle3D(ecs *world) {
    guint material;
    material_particle3D = spawn_material_source(world, "material_particle3D", "particle3D.vert", "particle3D.frag", &material);
    if (material) {
        const MaterialParticle3D data = create_MaterialParticle3D(material);
        zox_set_data(material_particle3D, MaterialParticle3D, data);
        spawn_particle3D_buffers(world);
    }
}

void gpu_dispose_particles3(ecs* world) {
    dispose_particle3D_buffers(world);
}

void gpu_restore_particles3(ecs* world) {
    zox_gpu_relocate(MaterialParticle3D);
    if (material_particle3D) {
        spawn_particle3D_buffers(world);
    }
}
