
entity spawn_shader(ecs *world, int shader_index) {
    guint2 shader = zox_gpu_compile_shader(shader_verts[shader_index], shader_frags[shader_index]);
    if (guint2_equals(shader, guint2_zero)) {
        zox_log_error("shader [%i] has failed with [zox_gpu_compile_shader]", shader_index);
        return 0;
    }
    zox_instance(prefab_shader);
    zox_name("shader");
    zox_set(e, ShaderSourceIndex, { shader_index });
    zox_set(e, ShaderGPULink, { shader });
    return e;
}

entity spawn_shader_source(ecs *world, char* name, char* svert, char* sfrag) {
    byte shader_index = get_new_shader_source_index();
    char* vert = get_shader_source(world, svert);
    char* frag = get_shader_source(world, sfrag);
    shader_verts[shader_index] = vert;
    shader_frags[shader_index] = frag;
    entity e = spawn_shader(world, shader_index);
    if (!e) {
        zox_log_error("[%s] failed to spawn", name)
        return 0;
    }
    zox_name(name);
    return e;
}

entity spawn_shader_new(ecs *world, int shader_index, guint2* ids) {
    *ids = zox_gpu_compile_shader(shader_verts[shader_index], shader_frags[shader_index]);
    if (guint2_equals(*ids, guint2_zero)) {
        zox_log_error("shader [%i] has failed with [zox_gpu_compile_shader]", shader_index);
        return 0;
    }
    zox_instance(prefab_shader);
    zox_name("shader");
    zox_set(e, ShaderSourceIndex, { shader_index });
    zox_set(e, ShaderGPULink, { *ids });
    return e;
}
