extern entity get_root_canvas_camera(ecs *world, const entity e);

// this is more like a non blend pass!

// NOTE: Needs to skip GPU calls for non layers since called per layer
zox_sys2(RenderTextureRenderSystem) {

    entity mat = material_render_texture;

    zox_sys_world();
    if (!zox_valid(mat)) {
        zox_log_error("Invalid [material_render_texture]");
        return;
    }

    byte init = 0;
    zox_geter(mat, MaterialAttributesRenderTexture, attributes);

    zox_sys_begin();
    zox_sys_in(TransformMatrix);
    zox_sys_in(Layer2D);
    zox_sys_in(RenderDisabled);
    zox_sys_in(MeshGPULink);
    zox_sys_in(UvsGPULink);
    zox_sys_in(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TransformMatrix, matrix);
        zox_sys_i(Layer2D, layer);
        zox_sys_i(RenderDisabled, rdisabled);
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(UvsGPULink, uvs);
        zox_sys_i(TextureGPULink, texture);

        if (layer->value != renderer_layer || rdisabled->value) {
            continue;
        }

        entity rcamera = get_root_canvas_camera(world, e);
        if (rcamera != renderer_camera) {
            continue;
        }

        if (!mesh->value.x || !mesh->value.y || !uvs->value || !texture->value) {
            // zox_logw("RenderTexture has invalid gpu link(s)");
            continue;
        }

        if (!init) {
            init = 1;

            zox_geter_value(mat, MaterialGPULink, uint, mlink);
            zox_gpu_material(mlink);
            zox_gpu_float4x4(attributes->camera_matrix, render_camera_matrix);
            zox_gpu_blend_disable();
        }

        // Bind data in GPU
        opengl_set_mesh_indicies(mesh->value.x);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->value.y);
        glEnableVertexAttribArray(attributes->vertex_position);
        glVertexAttribPointer(attributes->vertex_position, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, uvs->value);
        glEnableVertexAttribArray(attributes->vertex_uv);
        glVertexAttribPointer(attributes->vertex_uv, 2, GL_FLOAT, GL_FALSE, 0, 0);
        opengl_bind_texture(texture->value);
        zox_gpu_float4x4(attributes->transform_matrix, matrix->value);

        // Call our render of our quad
        zox_gpu_render(6);
    }

    if (!init) {
        return;
    }

    zox_gpu_disable_buffer(attributes->vertex_uv);
    zox_gpu_disable_buffer(attributes->vertex_position);
    opengl_unset_mesh();
    opengl_disable_texture(1);
    zox_disable_material();

} zox_sys_end(RenderTextureRenderSystem);
