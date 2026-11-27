entity_array_d* disposed_meshes = NULL;
entity_array_d* disposed_uvs = NULL;
entity_array_d* disposed_colors = NULL;
entity_array_d* disposed_textures = NULL;
entity_array_d* disposed_shaders = NULL;
entity_array_d* disposed_materials = NULL;

// lets go of every mesh, texture, shader and material on the gpu
void gpu_dispose_rendering(ecs* world) {
    zox_each_begin(MeshGPULink);
        zox_each_o(MeshGPULink, mesh);
        if (!mesh->value.x && !mesh->value.y) continue;
        zox_gpu_dispose_buffer(mesh->value.x);
        zox_gpu_dispose_buffer(mesh->value.y);
        mesh->value = guint2_zero;
        zox_disposed_add(&disposed_meshes, e);
    zox_each_end();
    zox_each_begin(UvsGPULink);
        zox_each_o(UvsGPULink, uvs);
        if (!uvs->value) continue;
        zox_gpu_dispose_buffer(uvs->value);
        uvs->value = 0;
        zox_disposed_add(&disposed_uvs, e);
    zox_each_end();
    zox_each_begin(ColorsGPULink);
        zox_each_o(ColorsGPULink, colors);
        if (!colors->value) continue;
        zox_gpu_dispose_buffer(colors->value);
        colors->value = 0;
        zox_disposed_add(&disposed_colors, e);
    zox_each_end();
    zox_each_begin(TextureGPULink);
        zox_each_o(TextureGPULink, texture);
        if (!texture->value) continue;
        zox_gpu_dispose_texture(texture->value);
        texture->value = 0;
        zox_disposed_add(&disposed_textures, e);
    zox_each_end();
    zox_each_begin(ShaderGPULink);
        zox_each_o(ShaderGPULink, shader);
        if (!shader->value.x && !shader->value.y) continue;
        zox_gpu_dispose_shader(shader->value.x);
        zox_gpu_dispose_shader(shader->value.y);
        shader->value = guint2_zero;
        zox_disposed_add(&disposed_shaders, e);
    zox_each_end();
    zox_each_begin(MaterialGPULink);
        zox_each_o(MaterialGPULink, material);
        if (!material->value) continue;
        zox_dispose_material(material->value);
        material->value = 0;
        zox_disposed_add(&disposed_materials, e);
    zox_each_end();
    render_view_dispose();
}

// makes them again: shaders before the materials linking them; meshes and textures upload again (MeshDirty, TextureDirty)
void gpu_restore_rendering(ecs* world) {
    render_view_create();
    zox_disposed_begin(disposed_meshes, MeshGPULink, mesh);
        mesh->value = spawn_gpu_mesh_buffers();
    zox_disposed_end(disposed_meshes);
    zox_disposed_begin(disposed_uvs, UvsGPULink, uvs);
        uvs->value = zox_gpu_create_buffer();
    zox_disposed_end(disposed_uvs);
    zox_disposed_begin(disposed_colors, ColorsGPULink, colors);
        colors->value = zox_gpu_create_buffer();
    zox_disposed_end(disposed_colors);
    zox_disposed_begin(disposed_textures, TextureGPULink, texture);
        texture->value = spawn_gpu_texture_buffer();
    zox_disposed_end(disposed_textures);
    zox_disposed_begin(disposed_shaders, ShaderGPULink, shader);
        const ShaderSourceIndex* index = zox_get(e, ShaderSourceIndex);
        if (!index || index->value >= zox_max_shaders) continue;
        shader->value = zox_gpu_compile_shader(shader_verts[index->value], shader_frags[index->value]);
    zox_disposed_end(disposed_shaders);
    zox_disposed_begin(disposed_materials, MaterialGPULink, material);
        const entity shader = zox_get_link(world, e, ShaderLink);
        if (!shader) continue;
        const guint2 gpu_shader = zox_getv(shader, ShaderGPULink);
        if (!gpu_shader.x || !gpu_shader.y) continue;
        material->value = spawn_gpu_material_program(gpu_shader);
    zox_disposed_end(disposed_materials);
    zox_each_add(Mesh, MeshDirty);
}
