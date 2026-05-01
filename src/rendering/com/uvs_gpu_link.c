zoxc_uint(UvsGPULink);

void add_gpu_uvs(ecs *world, entity e) {
    zox_prefab_set(e, UvsGPULink, { 0 })
}

void spawn_gpu_uvs(ecs *world, entity e) {
    zox_set(e, UvsGPULink, { spawn_gpu_generic_buffer() })
}

ECS_DTOR(UvsGPULink, ptr, {
    zox_gpu_dispose_buffer(ptr->value);
})
