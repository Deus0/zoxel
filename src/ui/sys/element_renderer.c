// TODO: Give uis a position2 and transform and use that instead directly
// TODO: Render layers in order instead of positioning with 3D

// NOTE: Needs to skip GPU calls for non layers since called per layer
zox_sys2(ElementRenderSystem) {
    byte is_log = 0;
    zox_sys_world();
    entity mat = material_textured2D;
    if (!zox_valid(mat)) {
        return;
    }
    byte init = 0;
    float position_z = 0;
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
        entity root_camera = zox_get_root_canvas_camera(world, e);
        if (root_camera != renderer_camera) {
            if (is_log) {
                zox_logw("Not Rendering [%s] at L[%i]", zox_get_name(e), renderer_layer);
            }
            continue;
        }
        if (!mesh->value.x || !mesh->value.y || !uvs->value || !texture->value) {
            if (is_log) {
                zox_logw("Mesh Invalid [%s] at L[%i]", zox_get_name(e), renderer_layer);
            }
            continue;
        }
        if (!init) {
            init = 1;
            zox_geter_value(mat, MaterialGPULink, uint, mlink);
            zox_gpu_material(mlink);
            zox_gpu_enable_blend();
            zox_gpu_float4x4(attributes->camera_matrix, render_camera_matrix);
        }
        // per mesh data
        zox_gpu_bind_buffer_element(mesh->value.x);
        zox_gpu_bind_texture(texture->value);
        zox_gpu_bind_buffer_array(mesh->value.y);
        zox_gpu_enable_attribute_float2(attributes->vertex_position);
        zox_gpu_bind_buffer_array(uvs->value);
        zox_gpu_enable_attribute_float2(attributes->vertex_uv);
        zox_gpu_float3(attributes->position, (float3) { position2->value.x, position2->value.y, position_z });
        zox_gpu_float(attributes->angle, rotation2D->value);
        zox_gpu_float(attributes->scale, scale1D->value);
        zox_gpu_float(attributes->brightness, brightness->value);
        zox_gpu_float(attributes->alpha, alpha->value);
        zox_gpu_render(6);
        if (is_log) {
            zox_log("Rendering [%s] at L[%i]", zox_get_name(e), renderer_layer);
        }
        zox_sys_increment();
    }
    if (!init) {
        return;
    }
    // cleanup material data
    zox_gpu_disable_attribute(attributes->vertex_uv);
    zox_gpu_disable_attribute(attributes->vertex_position);
    zox_gpu_disable_blend();
    zox_gpu_reset_mesh();
    zox_gpu_disable_blend();
    zox_gpu_reset_texture();
    zox_disable_material();
} zox_sys_end(ElementRenderSystem);
