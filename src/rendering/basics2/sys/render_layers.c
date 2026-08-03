// TODO: Give uis a position2 and transform and use that instead directly
// TODO: Render layers in order instead of positioning with 3D
// NOTE: Needs to skip GPU calls for non layers since called per layer
/*zox_sys2(ElementRenderSystem) {
    byte dbg_log = 0;
    byte dbg_gl = 0;
    float depth_per_layer = 0.001f;
    float units_per_layer = 1.0f;
    float depth_per_layer2 = zox_depth_per_layer;
    float depth_begin = depth_per_layer2;
    entity base_material = material_textured2D;
    entity material = 0;
    const MaterialTextured2D* attributes = NULL;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDisabled);
    zox_sys_in(Position2);
    zox_sys_in(Rotation2);
    zox_sys_in(Scale1);
    zox_sys_in(Layer2D);
    zox_sys_in(Brightness);
    zox_sys_in(Alpha);
    zox_sys_in(MeshGPULink);
    zox_sys_in(UvsGPULink);
    zox_sys_in(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(Position2, position);
        zox_sys_i(Rotation2, rotation);
        zox_sys_i(Scale1, scale);
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
        entity root_camera = zox_get_mesh2_camera(world, e);
        if (root_camera != renderer_camera) {
            if (dbg_log) {
                zox_logw("Not Rendering [%s] at L[%i]", zox_get_name(e), renderer_layer);
            }
            continue;
        }
#ifdef zox_safety_checks
        if (!mesh->value.x || !mesh->value.y || !uvs->value || !texture->value) {
            if (dbg_log) {
                zox_logw("Mesh Invalid [%s] at L[%i]", zox_get_name(e), renderer_layer);
            }
            continue;
        }
#endif
        entity new_material = zox_has(e, MaterialLink) ? zox_getv(e, MaterialLink) : base_material;
        if (material != new_material) {
#ifdef zox_safety_checks
            if (!zox_valid(new_material)) {
                zox_loge("Invalid UI Material");
                continue;
            }
            if (!zox_has(new_material, MaterialTextured2D)) {
                zox_loge("[%s]'s Material [%s] has no MaterialTextured2D", zox_getn(e), zox_getn(new_material));
                continue;
            }
#endif
            material = new_material;
            attributes = zox_get(material, MaterialTextured2D);
            guint material_id = zox_getv(material, MaterialGPULink);
            zox_gpu_material(material_id);
            zox_gpu_float4x4(attributes->camera_matrix, render_camera_matrix);
            zox_gpu_enable_blend();
            if (zox_new_ui_renderer) {
                // glEnable(GL_POLYGON_OFFSET_FILL);
                zox_gpu_enable_depth_test();
            }
        }
        // per mesh data
        // float units = units_per_layer * layer->value;
        // float depth = depth_per_layer * layer->value;
        // glPolygonOffset(-units, -depth);
        zox_gpu_bind_buffer_element(mesh->value.x);
        zox_gpu_bind_buffer_array(mesh->value.y);
        zox_gpu_enable_attribute_float2(attributes->vertex_position);
        zox_gpu_bind_buffer_array(uvs->value);
        zox_gpu_enable_attribute_float2(attributes->vertex_uv);
        // depth
        // float depth2 = layer->value * depth_per_layer2;
        zox_gpu_float3(attributes->position, (float3) { position->value.x, position->value.y, 0 });
        // Enable Texture
        zox_gpu_bind_texture(texture->value);
        zox_gpu_int(attributes->texture, 0);
        zox_gpu_float(attributes->angle, rotation->value);
        zox_gpu_float(attributes->scale, scale->value);
        zox_gpu_float(attributes->brightness, brightness->value);
        zox_gpu_float(attributes->alpha, alpha->value);
        zox_gpu_render(6);
        if (dbg_gl) {
            if (check_opengl_error_unlogged()) {
                zox_loge("Element2RenderSystem");
            }
        }
        if (dbg_log >= 2) {
            // TODO: Move this to gpu code, debug texture - important
            GLint w = 0;
            GLint h = 0;
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
            zox_log("Rendering [%s] Texture %i size %ix%i", zox_get_name(e), texture->value, w, h);
        }
        if (dbg_log == 1) {
            float2 local_position = zox_has(e, LocalPosition2) ? zox_getv(e, LocalPosition2) : float2_zero;
            zox_log("Rendering [%s] at L[%i] At [%.01fx%.01f]: Brightness [%f] Alpha [%f] Local [%.01fx%.01f]", zox_get_name(e), layer->value, position->value.x, position->value.y, brightness->value, alpha->value, local_position.x, local_position.y);
            zox_log("   - gpu: Mesh [%ix%i] UVs [%i] Texture [%i] Material [%s]", mesh->value.x, mesh->value.y, uvs->value, texture->value, zox_getn(material));
        }
        zox_sys_increment();
    }
    if (material) {
        zox_gpu_disable_attribute(attributes->vertex_uv);
        zox_gpu_disable_attribute(attributes->vertex_position);
        zox_gpu_reset_mesh();
        zox_gpu_reset_texture();
        zox_disable_material();
        zox_gpu_disable_blend();
        if (zox_new_ui_renderer) {
            // glDisable(GL_POLYGON_OFFSET_FILL);
            zox_gpu_disable_depth_test();
        }
    }
} zox_sys_end(ElementRenderSystem);*/
