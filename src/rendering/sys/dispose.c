void MeshColorsGPUDisposeSystem(iter *it) {
    zox_sys_begin()
    zox_sys_out(ColorsGPULink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(ColorsGPULink, colors);
        zox_gpu_dispose_buffer(colors->value);
        colors->value = 0;
    }
} zoxd_system(MeshColorsGPUDisposeSystem);

void MaterialDisposeSystem(iter *it) {
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
} zoxd_system(MaterialDisposeSystem);

zox_sys2(MeshGPUDisposeSystem) {
    zox_sys_begin()
    zox_sys_out(MeshGPULink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(MeshGPULink, mesh);
        zox_gpu_dispose_buffer(mesh->value.x);
        zox_gpu_dispose_buffer(mesh->value.y);
        mesh->value = guint2_zero;
    }
} zox_sys_end(MeshGPUDisposeSystem);

zox_sys2(ShaderDisposeSystem) {
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
} zox_sys_end(ShaderDisposeSystem);

zox_sys2(MeshUvsGPUDisposeSystem) {
    zox_sys_begin()
    zox_sys_out(UvsGPULink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(UvsGPULink, uvs)
        zox_gpu_dispose_buffer(uvs->value);
        uvs->value = 0;
    }
} zox_sys_end(MeshUvsGPUDisposeSystem);

zox_sys2(TextureGPUDisposeSystem) {
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
} zox_sys_end(TextureGPUDisposeSystem);


void zox_systems_rendering_dispose(ecs *world) {
    zox_gpu_dispose_system(
        MeshGPUDisposeSystem,
        [out] MeshGPULink
    );
    zox_gpu_dispose_system(
        MeshUvsGPUDisposeSystem,
        [out] rendering.UvsGPULink
    );
    zox_gpu_dispose_system(
        MeshColorsGPUDisposeSystem,
        [out] rendering.ColorsGPULink
    );
    zox_gpu_dispose_system(
        TextureGPUDisposeSystem,
        [out] TextureGPULink
    );
    zox_gpu_dispose_system(
        ShaderDisposeSystem,
        [out] ShaderGPULink
    );
    zox_gpu_dispose_system(
        MaterialDisposeSystem,
        [out] MaterialGPULink
    );
}
