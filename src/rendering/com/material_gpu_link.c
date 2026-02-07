extern uint spawn_gpu_material_program(uint2 shader);

zoxc(MaterialGPULink, uint);
zoxc(MaterialInstancedGPULink, uint);

void add_gpu_material(ecs *world, entity e) {
    if (!headless) {
        zox_prefab_set(e, MaterialGPULink, { 0 });
        zox_prefab_set(e, ShaderLink, { 0 });
    }
}

uint spawn_gpu_material(ecs *world, entity e, uint2 shader) {

    if (headless) {
        return 0;
    }

    if (!shader.x || !shader.y) {
        zox_log_error("[spawn_gpu_material] has invalid shader");
        return 0;
    }

    uint gpu_material = spawn_gpu_material_program(shader);

    if (!gpu_material) {
        zox_log_error("[spawn_gpu_material_program] failed");
    } else {
        zox_set(e, MaterialGPULink, { gpu_material });
    }

    return gpu_material;
}

ECS_DTOR(MaterialGPULink, ptr, {
    if (ptr->value) {
        glDeleteProgram(ptr->value);
    }
})
