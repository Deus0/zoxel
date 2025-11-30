entity prefab_shader;

entity spawn_prefab_shader(ecs *world) {
    zox_prefab();
    zox_prefab_name("shader");
    zox_add_tag(e, Shader);
    zox_prefab_set(e, ShaderSourceIndex, { 0 })
    add_gpu_shader(world, e); // adds ShaderGPULink
    prefab_shader = e;
    return e;
}

entity spawn_shader(ecs *world, const int shader_index) {
    uint2 shader = zox_gpu_compile_shader(shader_verts[shader_index], shader_frags[shader_index]);
    if (uint2_equals(shader, uint2_zero)) {
        zox_log_error("shader [%i] has failed with [zox_gpu_compile_shader]", shader_index);
        return 0;
    }
    zox_instance(prefab_shader);
    zox_name("shader");
    zox_set(e, ShaderSourceIndex, { shader_index });
    zox_set(e, ShaderGPULink, { shader });
    return e;
}
