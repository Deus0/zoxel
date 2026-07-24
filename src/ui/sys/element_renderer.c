// TODO: Give uis a position2 and transform and use that instead directly
// TODO: Render layers in order instead of positioning with 3D

// NOTE: Needs to skip GPU calls for non layers since called per layer
zox_sys2(ElementRenderSystem) {
    byte is_log = 1;
    float depth_per_layer = 0.001f;
    float depth_begin = depth_per_layer;
    entity base_material = material_textured2D;
    entity material = 0;
    const MaterialTextured2D* attributes = NULL;
    zox_gpu_enable_blend();
    if (zox_new_ui_renderer) {
        zox_gpu_enable_depth_test();
    }
    zox_sys_world();
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
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(Layer2D, layer);
        zox_sys_i(Brightness, brightness);
        zox_sys_i(Alpha, alpha);
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(UvsGPULink, uvs);
        zox_sys_i(TextureGPULink, texture);
        if (!zox_new_ui_renderer && layer->value != renderer_layer) {
            continue;
        }
        if (disabled->value || !alpha->value) {
            continue;
        }
        entity root_camera = zox_get_root_canvas_camera(world, e);
        if (root_camera != renderer_camera) {
            if (is_log) {
                zox_logw("Not Rendering [%s] at L[%i]", zox_get_name(e), renderer_layer);
            }
            continue;
        }
#ifdef zox_safety_checks
        if (!mesh->value.x || !mesh->value.y || !uvs->value || !texture->value) {
            if (is_log) {
                zox_logw("Mesh Invalid [%s] at L[%i]", zox_get_name(e), renderer_layer);
            }
            continue;
        }
#endif
        entity new_material = zox_has(e, MaterialLink) ? zox_getv(e, MaterialLink) : base_material;
        if (material != new_material) {
            material = new_material;
#ifdef zox_safety_checks
            if (!zox_valid(material)) {
                zox_loge("Invalid UI Material");
                material = 0;
                continue;
            }
            if (!zox_has(material, MaterialTextured2D)) {
                zox_loge("Invalid UI Material [%s] no MaterialTextured2D", zox_getn(material));
                material = 0;
                continue;
            }
#endif
            attributes = zox_get(material, MaterialTextured2D);
            guint material_id = zox_getv(material, MaterialGPULink);
            zox_gpu_material(material_id);
            zox_gpu_float4x4(attributes->camera_matrix, render_camera_matrix);
        }
        // per mesh data
        float depth = depth_begin + layer->value * depth_per_layer;
        zox_gpu_bind_buffer_element(mesh->value.x);
        zox_gpu_bind_texture(texture->value);
        zox_gpu_bind_buffer_array(mesh->value.y);
        zox_gpu_enable_attribute_float2(attributes->vertex_position);
        zox_gpu_bind_buffer_array(uvs->value);
        zox_gpu_enable_attribute_float2(attributes->vertex_uv);
        zox_gpu_float3(attributes->position, (float3) { position2->value.x, position2->value.y, depth });
        zox_gpu_float(attributes->angle, rotation2D->value);
        zox_gpu_float(attributes->scale, scale1D->value);
        zox_gpu_float(attributes->brightness, brightness->value);
        zox_gpu_float(attributes->alpha, alpha->value);
        zox_gpu_render(6);
        if (is_log) {
            if (zox_has(e, DebugEntity)) {
                zox_log("Rendering [%s] at L[%i] At [%.01fx%.01fx%f]", zox_get_name(e), layer->value, position2->value.x, position2->value.y, depth);
            }
        }
        zox_sys_increment();
    }
    if (material) {
        // zox_gpu_disable_attribute(attributes->vertex_uv);
        // zox_gpu_disable_attribute(attributes->vertex_position);
        zox_gpu_reset_mesh();
        zox_gpu_reset_texture();
        zox_disable_material();
    }
    zox_gpu_disable_blend();
    if (zox_new_ui_renderer) {
        zox_gpu_disable_depth_test();
    }
} zox_sys_end(ElementRenderSystem);
