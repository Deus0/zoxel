extern guint spawn_gpu_material_program(guint2 shader);

zoxc_guint(MaterialGPULink);
zoxc_guint(MaterialInstancedGPULink);

void add_gpu_material(ecs *world, entity e) {
    zox_prefab_set(e, MaterialGPULink, { 0 });
    zox_prefab_set(e, ShaderLink, { 0 });
}

guint spawn_gpu_material(ecs *world, entity e, guint2 shader) {

    if (!shader.x || !shader.y) {
        zox_log_error("[spawn_gpu_material] has invalid shader");
        return 0;
    }

    guint gpu_material = spawn_gpu_material_program(shader);

    if (gpu_material) {
        zox_set(e, MaterialGPULink, { gpu_material });
    }

    return gpu_material;
}

ECS_DTOR(MaterialGPULink, ptr, {
    zox_dispose_material(ptr->value);
})
