#include "updates.c"

// for now until I move it all to ecs
// the 2D square meshes and instance material, let go of and made again with the gpu
void gpu_dispose_rendering2(ecs* world) {
    (void) world;
    dispose_shader2D_instance_material();
    dispose_square_mesh();
}

void gpu_restore_rendering2(ecs* world) {
    initialize_square_mesh_textured();
    load_instance2D_material(world);
}

// spawns after, when opengl loads
void spawn_shaders_basics2D(ecs *world) {
    spawn_material_textured2D(world);
    initialize_square_mesh_textured();
    load_instance2D_material(world);
}
