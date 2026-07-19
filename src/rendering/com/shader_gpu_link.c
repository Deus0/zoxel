zoxc_guint2(ShaderGPULink);

void add_gpu_shader(ecs *world, entity e) {
    zox_prefab_set(e, ShaderGPULink, { { 0, 0 } });
}

ECS_DTOR(ShaderGPULink, ptr, {
    zox_gpu_dispose_shader(ptr->value.x);
    zox_gpu_dispose_shader(ptr->value.y);
})

guint2 get_shader_value(ecs *world, entity shader) {
    if (!zox_valid(shader)) {
        return (guint2) { 0, 0 };
    }
    return zox_get_value(shader, ShaderGPULink);
}
