void mesh_colors_gpu_dispose_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_begin()
    zox_sys_out(ColorsGPULink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(ColorsGPULink, colors);
        zox_gpu_dispose_buffer(colors->value);
        colors->value = 0;
    }
    zox_sys_on_end();
} zoxd_system(mesh_colors_gpu_dispose_system);

void material_dispose_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_begin();
    zox_sys_out(MaterialGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(MaterialGPULink, material);
#ifdef zox_log_gpu_management
        zox_log(" > disposing [%s] [%i]\n", zox_getn(it->entities[i]), material->value)
#endif
        zox_dispose_material(material->value);
        material->value = 0;
    }
    zox_sys_on_end();
} zoxd_system(material_dispose_system);

void mesh_gpu_dispose_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_begin()
    zox_sys_out(MeshGPULink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(MeshGPULink, mesh);
        zox_gpu_dispose_buffer(mesh->value.x);
        zox_gpu_dispose_buffer(mesh->value.y);
        mesh->value = guint2_zero;
    }
    zox_sys_on_end();
} zoxd_system(mesh_gpu_dispose_system);

void shader_dispose_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_begin()
    zox_sys_out(ShaderGPULink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(ShaderGPULink, shader)
#ifdef zox_log_gpu_management
        zox_log(" + disposing [%s] [%ix%i]\n", zox_get_name(it->entities[i]), shader->value.x, shader->value.x)
#endif
        zox_gpu_dispose_shader(shader->value.x);
        zox_gpu_dispose_shader(shader->value.y);
        shader->value = guint2_zero;
    }
    zox_sys_on_end();
} zoxd_system(shader_dispose_system);

void mesh_uvs_gpu_dispose_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_begin()
    zox_sys_out(UvsGPULink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(UvsGPULink, uvs)
        zox_gpu_dispose_buffer(uvs->value);
        uvs->value = 0;
    }
    zox_sys_on_end();
} zoxd_system(mesh_uvs_gpu_dispose_system);

void texture_gpu_dispose_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_begin();
    zox_sys_out(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(TextureGPULink, texture);
        if (!texture->value) {
            continue;
        }
        zox_gpu_dispose_texture(texture->value);
        texture->value = 0;
    }
    zox_sys_on_end();
} zoxd_system(texture_gpu_dispose_system);


void zox_systems_rendering_dispose(ecs *world) {
    zox_gpu_dispose_system(
        mesh_gpu_dispose_system,
        [out] MeshGPULink
    );
    zox_gpu_dispose_system(
        mesh_uvs_gpu_dispose_system,
        [out] rendering.UvsGPULink
    );
    zox_gpu_dispose_system(
        mesh_colors_gpu_dispose_system,
        [out] rendering.ColorsGPULink
    );
    zox_gpu_dispose_system(
        texture_gpu_dispose_system,
        [out] TextureGPULink
    );
    zox_gpu_dispose_system(
        shader_dispose_system,
        [out] ShaderGPULink
    );
    zox_gpu_dispose_system(
        material_dispose_system,
        [out] MaterialGPULink
    );
}
