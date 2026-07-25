zoxc_guint(TextureGPULink);

void spawn_gpu_texture(ecs *world, entity e) {
    zox_set(e, TextureGPULink, { spawn_gpu_texture_buffer() });
}

ECS_DTOR(TextureGPULink, ptr, {
    zox_gpu_dispose_texture(ptr->value);
})
