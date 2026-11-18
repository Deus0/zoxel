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
    zox_gpu_relocate(attributes_matrixui);
    zox_gpu_relocate(MaterialTextured2D);
    zox_each_begin(MaterialBlur);
        const MaterialGPULink* material = zox_get(e, MaterialGPULink);
        MaterialVignette* vignette = zox_get_mut(e, MaterialVignette);
        MaterialTint* tint = zox_get_mut(e, MaterialTint);
        if (!material || !material->value || !vignette || !tint) continue;
        zox_each_o(MaterialBlur, blur);
        blur->value = zox_gpu_get_material_property(material->value, "blur");
        vignette->value = zox_gpu_get_material_property(material->value, "vignette");
        tint->value = zox_gpu_get_material_property(material->value, "tint");
    zox_each_end();
}

// spawns after, when opengl loads
void spawn_shaders_basics2D(ecs *world) {
    spawn_material_textured2D(world);
    initialize_square_mesh_textured();
    load_instance2D_material(world);
}
