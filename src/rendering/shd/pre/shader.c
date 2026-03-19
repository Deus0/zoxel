entity spawn_prefab_shader(ecs *world) {

    zox_prefab();
    zox_prefab_name("shader");
    zox_add_tag(e, Shader);
    zox_prefab_set(e, ShaderSourceIndex, { 0 })
    add_gpu_shader(world, e); // adds ShaderGPULink

    return e;
}
