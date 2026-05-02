extern uint spawn_gpu_material_program(uint2 shader);

zoxc_uint(MaterialGPULink);
zoxc_uint(MaterialInstancedGPULink);

void add_gpu_material(ecs *world, entity e) {
    zox_prefab_set(e, MaterialGPULink, { 0 });
    zox_prefab_set(e, ShaderLink, { 0 });
}

uint spawn_gpu_material(ecs *world, entity e, uint2 shader) {

    if (!shader.x || !shader.y) {
        zox_log_error("[spawn_gpu_material] has invalid shader");
        return 0;
    }

    uint gpu_material = spawn_gpu_material_program(shader);

    if (gpu_material) {
        zox_set(e, MaterialGPULink, { gpu_material });
    }

    return gpu_material;
}

ECS_DTOR(MaterialGPULink, ptr, {
    zox_dispose_material(ptr->value);
})
