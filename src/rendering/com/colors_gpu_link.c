zoxc_guint(ColorsGPULink);

void spawn_gpu_colors(ecs *world, entity e) {
    zox_set(e, ColorsGPULink, { zox_gpu_create_buffer() });
}

ECS_DTOR(ColorsGPULink, ptr, {
    zox_gpu_dispose_buffer(ptr->value);
})
