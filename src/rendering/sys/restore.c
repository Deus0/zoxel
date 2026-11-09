void mesh_gpu_restore_system(iter* it) {
    zox_sys_on_begin();
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
    zox_sys_on_end();
} zoxd_system(mesh_gpu_restore_system);

void mesh_dirty_restore_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_add(e, MeshDirty);
    }
    zox_sys_on_end();
} zoxd_system(mesh_dirty_restore_system);

void texture_restore_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_begin();
    zox_sys_out(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(TextureGPULink, texture);
        texture->value = spawn_gpu_texture_buffer();
    }
    zox_sys_on_end();
} zoxd_system(texture_restore_system);

void shader_restore_system(iter* it) {
    zox_sys_on_begin();
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
    zox_sys_on_end();
} zoxd_system(shader_restore_system);

void uvs_gpu_link_restore_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_begin()
    zox_sys_out(UvsGPULink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(UvsGPULink, uvs)
        uvs->value = zox_gpu_create_buffer();
    }
    zox_sys_on_end();
} zoxd_system(uvs_gpu_link_restore_system);

void material_restore_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(MaterialGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(MaterialGPULink, material);
        entity shader = zox_get_link(world, e, ShaderLink);
        if (!shader) {
#ifdef zox_log_gpu_management
            zox_logw("No shader [%s]",
                zox_get_name(it->entities[i]));
#endif
            continue;
        }
        guint2 gpu_shader = zox_getv(shader, ShaderGPULink);
#ifdef zox_log_gpu_management
        zox_log(" > restoring [%s] - shader [%ix%i]\n",
            zox_get_name(it->entities[i]),
                gpu_shader.x,
                gpu_shader.y)
#endif
        if (!gpu_shader.x || !gpu_shader.y) {
            continue;
        }
        material->value = spawn_gpu_material_program(gpu_shader);
    }
    zox_sys_on_end();
} zoxd_system(material_restore_system);

void colors_gpu_link_restore_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_begin()
    zox_sys_out(ColorsGPULink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(ColorsGPULink, colors)
        colors->value = zox_gpu_create_buffer();
    }
    zox_sys_on_end();
} zoxd_system(colors_gpu_link_restore_system);

void zox_systems_rendering_restore(ecs* world) {
    zox_gpu_restore_system(
        mesh_gpu_restore_system,
        [out] rendering.MeshGPULink
    );
    zox_gpu_restore_system(
        uvs_gpu_link_restore_system,
        [out] rendering.UvsGPULink
    );
    zox_gpu_restore_system(
        colors_gpu_link_restore_system,
        [out] rendering.ColorsGPULink
    );
    zox_gpu_restore_system(
        texture_restore_system,
        [out] rendering.TextureGPULink
    );
    zox_gpu_restore_system(
        shader_restore_system,
        [in] rendering.ShaderSourceIndex,
        [out] rendering.ShaderGPULink
    );
    zox_gpu_restore_system(
        material_restore_system,
        [out] rendering.MaterialGPULink,
        // [none] rendering.Material
    );
    zox_gpu_restore_system(
        mesh_dirty_restore_system,
        [none] rendering.Mesh,
    );
}
