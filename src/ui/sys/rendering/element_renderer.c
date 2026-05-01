// TODO: Give uis a position2 and transform and use that instead directly
// TODO: Render layers in order instead of positioning with 3D

// NOTE: Needs to skip GPU calls for non layers since called per layer
zox_sys2(ElementRenderSystem) {
    zox_sys_world();

    entity mat = material_textured2D;
    if (!zox_valid(mat)) {
        return;
    }

    byte init = 0;
    float position_z = 0; // ((int) renderer_layer); // * shader_depth_multiplier;

    zox_geter(mat, MaterialTextured2D, attributes);

    zox_sys_begin();
    zox_sys_in(Position2);
    zox_sys_in(Rotation2D);
    zox_sys_in(Scale1D);
    zox_sys_in(Layer2D);
    zox_sys_in(RenderDisabled);
    zox_sys_in(Brightness);
    zox_sys_in(Alpha);
    zox_sys_in(MeshGPULink);
    zox_sys_in(UvsGPULink);
    zox_sys_in(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position2, position2);
        zox_sys_i(Rotation2D, rotation2D);
        zox_sys_i(Scale1D, scale1D);
        zox_sys_i(RenderDisabled, rdisabled);
        zox_sys_i(Layer2D, layer);
        zox_sys_i(Brightness, brightness);
        zox_sys_i(Alpha, alpha);
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
            continue;
        }

        if (!init) {
            init = 1;

            zox_geter_value(mat, MaterialGPULink, uint, mlink)
            zox_gpu_blend_enable();
            zox_gpu_material(mlink);
            zox_gpu_float4x4(attributes->camera_matrix, render_camera_matrix);
        }

        // per mesh data
        opengl_set_mesh_indicies(mesh->value.x);
        opengl_bind_texture(texture->value);

        zox_gpu_bind_buffer_array(mesh->value.y);
        zox_gpu_enable_attribute_float2(attributes->vertex_position);
        //glEnableVertexAttribArray(attributes->vertex_position);
        //glVertexAttribPointer(attributes->vertex_position, 2, GL_FLOAT, GL_FALSE, 0, 0);

        zox_gpu_bind_buffer_array(uvs->value);
        zox_gpu_enable_attribute_float2(attributes->vertex_uv);
        //glEnableVertexAttribArray(attributes->vertex_uv);
        //glVertexAttribPointer(attributes->vertex_uv, 2, GL_FLOAT, GL_FALSE, 0, 0);

        zox_gpu_float3(attributes->position, (float3) { position2->value.x, position2->value.y, position_z });
        zox_gpu_float(attributes->angle, rotation2D->value);
        zox_gpu_float(attributes->scale, scale1D->value);
        zox_gpu_float(attributes->brightness, brightness->value);
        zox_gpu_float(attributes->alpha, alpha->value);

        zox_gpu_render(6);
    }

    if (!init) {
        return;
    }

    // cleanup material data
    zox_gpu_disable_buffer(attributes->vertex_uv);
    zox_gpu_disable_buffer(attributes->vertex_position);
    zox_gpu_blend_disable();
    opengl_unset_mesh();
    opengl_disable_texture(1);
    zox_disable_material();

} zox_sys_end(ElementRenderSystem);
