extern guint spawn_gpu_texture_buffer();

zoxc_guint(TextureGPULink);

void add_gpu_texture(ecs *world, entity e) {
    zox_prefab_set(e, TextureGPULink, { 0 });
}

void spawn_gpu_texture(ecs *world, entity e) {
    zox_set(e, TextureGPULink, { spawn_gpu_texture_buffer() });
}

ECS_DTOR(TextureGPULink, ptr, {
    zox_gpu_dispose_texture(ptr->value);
})
