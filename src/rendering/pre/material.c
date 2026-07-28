entity prefab_material;

entity spawn_prefab_material(ecs *world) {
    zox_prefab();
    zox_prefab_name("material");
    zox_add_tag(e, Material);
    zox_prefab_set(e, MaterialGPULink, { 0 });
    zox_prefab_set(e, ShaderLink, { 0 });
    prefab_material = e;
    return e;
}

entity spawn_material(ecs *world, entity shader, guint* output_material) {
    zox_instance(prefab_material);
    zox_name("material");
    zox_set(e, ShaderLink, { shader });
    guint2 shader_value = zox_getv(shader, ShaderGPULink);
    guint gpu_material = spawn_gpu_material_program(shader_value);
    if (!gpu_material) {
        zox_geter(shader, ShaderSourceIndex, index);
        zox_logv("### [%s] ###", zox_get_name(shader));
        zox_logv("   .vert\n%s", shader_verts[index->value]);
        zox_logv("   .frag\n%s", shader_frags[index->value]);
        zox_logv("### ### ### ### ###");
    } else {
        zox_setv(e, MaterialGPULink, gpu_material);
        *output_material = gpu_material;
    }
    return e;
}

void restore_material(ecs *world, entity e, guint2 shader) {
    guint gpu_material = spawn_gpu_material_program(shader);
    zox_setv(e, MaterialGPULink, gpu_material);
}
