zoxc_uint(UboGPULink);

void add_gpu_ubo(ecs *world, entity e) {
    zox_prefab_set(e, UboGPULink, { 0 });
}

void spawn_gpu_ubo(ecs *world, entity e) {
    zox_set(e, UboGPULink, { zox_gpu_create_buffer() });
}

// actually this gets called during flecs table changes
ECS_DTOR(UboGPULink, ptr, {
    // if (ptr->value != 0) glDeleteBuffers(1, &ptr->value);
})
