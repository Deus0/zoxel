zoxc_uint(ColorsGPULink);

void add_gpu_colors(ecs *world, entity e) {
    if (!headless) zox_prefab_set(e, ColorsGPULink, { 0 })
}

void spawn_gpu_colors(ecs *world, entity e) {
    if (!headless) zox_set(e, ColorsGPULink, { spawn_gpu_generic_buffer() })
}

ECS_DTOR(ColorsGPULink, ptr, {
    zox_gpu_dispose_buffer(ptr->value);
})
