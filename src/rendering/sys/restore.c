zox_sys2(MeshGPURestoreSystem) {
    byte dbg_log = 0;
    zox_sys_begin()
    zox_sys_out(MeshGPULink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(MeshGPULink, mesh)
        mesh->value = spawn_gpu_mesh_buffers();
        if (dbg_log) {
            zox_log("Mesh Restored [%s]:[%ux%u]", zox_sys_e_name, mesh->value.x, mesh->value.y);
        }
    }
} zox_sys_end(MeshGPURestoreSystem);

zox_sys2(MeshDirtyRestoreSystem) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_add(e, MeshDirty);
    }
} zox_sys_end(MeshDirtyRestoreSystem);

zox_sys2(TextureRestoreSystem) {
    zox_sys_begin();
    zox_sys_out(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(TextureGPULink, texture);
        texture->value = spawn_gpu_texture_buffer();
    }
} zox_sys_end(TextureRestoreSystem);

zox_sys2(ShaderRestoreSystem) {
    zox_sys_begin()
    zox_sys_in(ShaderSourceIndex);
    zox_sys_out(ShaderGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ShaderSourceIndex, shaderSourceIndex);
        zox_sys_o(ShaderGPULink, shader);
        if (shaderSourceIndex->value >= zox_max_shaders) {
            continue;
        }
        shader->value = zox_gpu_compile_shader(shader_verts[shaderSourceIndex->value], shader_frags[shaderSourceIndex->value]);
#ifdef zox_log_gpu_management
        zox_log(" + restoring [%s] [%i] gpu [%ix%i]\n", zox_get_name(it->entities[i]), shaderSourceIndex->value, shader->value.x, shader->value.y)
#endif
    }
} zox_sys_end(ShaderRestoreSystem);

zox_sys2(UvsGPULinkRestoreSystem) {
    zox_sys_begin()
    zox_sys_out(UvsGPULink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(UvsGPULink, uvs)
        uvs->value = zox_gpu_create_buffer();
    }
} zox_sys_end(UvsGPULinkRestoreSystem);

zox_sys2(MaterialRestoreSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ShaderLink);
    zox_sys_out(MaterialGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ShaderLink, shaderLink);
        zox_sys_o(MaterialGPULink, materialGPULink);
#ifdef zox_log_gpu_management
        if (!shaderLink->value) zox_log(" ! no shader_link [%s]\n", zox_get_name(it->entities[i]));
#endif
        if (!shaderLink->value) {
            continue;
        }
        guint2 shader = zox_getv(shaderLink->value, ShaderGPULink);
#ifdef zox_log_gpu_management
        zox_log(" > restoring [%s] - shader [%ix%i]\n", zox_get_name(it->entities[i]), shader.x, shader.y)
#endif
        if (!shader.x || !shader.y) {
            continue;
        }
        materialGPULink->value = spawn_gpu_material_program(shader);
    }
} zox_sys_end(MaterialRestoreSystem);

zox_sys2(ColorsGPULinkRestoreSystem) {
    zox_sys_begin()
    zox_sys_out(ColorsGPULink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(ColorsGPULink, colors)
        colors->value = zox_gpu_create_buffer();
    }
} zox_sys_end(ColorsGPULinkRestoreSystem);

void zox_systems_rendering_restore(ecs* world) {
    zox_gpu_restore_system(
        MeshGPURestoreSystem,
        [out] MeshGPULink
    );
    zox_gpu_restore_system(
        UvsGPULinkRestoreSystem,
        [out] rendering.UvsGPULink
    );
    zox_gpu_restore_system(
        ColorsGPULinkRestoreSystem,
        [out] rendering.ColorsGPULink
    );
    zox_gpu_restore_system(
        TextureRestoreSystem,
        [out] TextureGPULink
    );
    zox_gpu_restore_system(
        ShaderRestoreSystem,
        [in] ShaderSourceIndex,
        [out] ShaderGPULink
    );
    zox_gpu_restore_system(
        MaterialRestoreSystem,
        [in] ShaderLink,
        [out] MaterialGPULink
    );
    zox_gpu_restore_system(
        MeshDirtyRestoreSystem,
        [none] rendering.Mesh,
    );
}
