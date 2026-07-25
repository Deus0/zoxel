entity shader_matrixui = 0;
entity material_matrixui = 0;

static inline attributes_matrixui create_attributes_matrixui(guint id) {
    return (attributes_matrixui) {
        zox_gpu_get_material_attribute(id, "vertex_position"),
        zox_gpu_get_material_attribute(id, "vertex_uv"),
        zox_gpu_get_material_property(id, "matrix"),
        zox_gpu_get_material_property(id, "camera_matrix"),
        zox_gpu_get_material_property(id, "texture"),
        zox_gpu_get_material_property(id, "brightness"),
        zox_gpu_get_material_property(id, "alpha")
    };
}

entity spawn_material_matrixui(ecs *world) {
    byte shader_index = get_new_shader_source_index();
    uint material;
    char* vert = get_shader_source(world, "matrixui.vert");
    char* frag = get_shader_source(world, "textured2D.frag");
    shader_verts[shader_index] = vert;
    shader_frags[shader_index] = frag;
    entity shader = spawn_shader(world, shader_index);
    shader_matrixui = shader;
    if (!shader) {
        zox_log_error("=> [spawn_shader_textured2D] Failed:\n%s", vert);
        return 0;
    }
    zox_set_unique_name(shader, "shader_matrixui");
    entity e = spawn_material(world, shader, &material);
    material_matrixui = e;
    if (!e) {
        return 0;
    }
    zox_set(e, ShaderLink, { shader });
    attributes_matrixui base_attributes = create_attributes_matrixui(material);
    zox_set_data(e, attributes_matrixui, base_attributes);
    // zox_set_data(e, attributes_matrixui, create_attributes_matrixui(material));
    return e;
}

// NOTE: It will switch materials for unique ones when needed
// TODO: Replace the old ui system and just use this
// NOTE: Needs to skip GPU calls for non layers since called per layer
zox_sys2(ElementRenderMatrixSystem) {
    byte is_log = 0;
    float depth_per_layer = 0.001f;
    float depth_begin = depth_per_layer;
    const attributes_matrixui* attributes = NULL;
    entity base_material = material_matrixui;
    entity material = 0;
    zox_gpu_enable_blend();
    if (zox_new_ui_renderer) {
        zox_gpu_enable_depth_test();
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TransformMatrix);
    zox_sys_in(Layer2D);
    zox_sys_in(RenderDisabled);
    zox_sys_in(Brightness);
    zox_sys_in(Alpha);
    zox_sys_in(MeshGPULink);
    zox_sys_in(UvsGPULink);
    zox_sys_in(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TransformMatrix, matrix);
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(Layer2D, layer);
        zox_sys_i(Brightness, brightness);
        zox_sys_i(Alpha, alpha);
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(UvsGPULink, uvs);
        zox_sys_i(TextureGPULink, texture);
        if (disabled->value) {
            continue;
        }
        if (!zox_new_ui_renderer && layer->value != renderer_layer) {
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
        entity new_material = zox_has(e, MaterialLink) ? zox_getv(e, MaterialLink) : base_material;
        if (material != new_material) {
            material = new_material;
#ifdef zox_safety_checks
            if (!zox_valid(material)) {
                zox_loge("Invalid UI Material TransformUI");
                material = 0;
                continue;
            }
            if (!zox_has(material, attributes_matrixui)) {
                zox_loge("Invalid UI Material [%s] no [attributes_matrixui]", zox_getn(material));
                material = 0;
                continue;
            }
#endif
            attributes = zox_get(material, attributes_matrixui);
            guint material_id = zox_getv(material, MaterialGPULink);
            zox_gpu_material(material_id);
            zox_gpu_float4x4(attributes->camera_matrix, render_camera_matrix);
            if (zox_has(material, MaterialBlur) && zox_has(material, CameraBlur)) {
                guint property_id = zox_getv(material, MaterialBlur);
                float property_value = zox_getv(material, CameraBlur);
                zox_gpu_float(property_id, property_value);
                // zox_log("MaterialBlur Set [%i] to [%f]", property_id, property_value);
            }
            if (zox_has(material, MaterialVignette) && zox_has(material, CameraVignette)) {
                guint property_id = zox_getv(material, MaterialVignette);
                float property_value = zox_getv(material, CameraVignette);
                zox_gpu_float(property_id, property_value);
                // zox_log("Material Vignette Set [%i] to [%f]", property_id, property_value);
            }
        }
        float depth = depth_begin + layer->value * depth_per_layer;
        float4x4 matrix2 = matrix->value;
        matrix2.w.z += depth;
        // per mesh data
        zox_gpu_bind_buffer_element(mesh->value.x);
        zox_gpu_bind_texture(texture->value);
        zox_gpu_bind_buffer_array(mesh->value.y);
        zox_gpu_enable_attribute_float2(attributes->vertex_position);
        zox_gpu_bind_buffer_array(uvs->value);
        zox_gpu_enable_attribute_float2(attributes->vertex_uv);
        zox_gpu_float4x4(attributes->matrix, matrix2);
        zox_gpu_float(attributes->brightness, brightness->value);
        zox_gpu_float(attributes->alpha, alpha->value);
        zox_gpu_render(6);
        if (is_log) {
            zox_log("Rendering [%s] at L[%i]", zox_get_name(e), renderer_layer);
        }
        zox_sys_increment();
    }
    if (material) {
        zox_gpu_disable_attribute(attributes->vertex_uv);
        zox_gpu_disable_attribute(attributes->vertex_position);
        zox_gpu_reset_mesh();
        zox_gpu_reset_texture();
        zox_disable_material();
    }
    zox_gpu_disable_blend();
    if (zox_new_ui_renderer) {
        zox_gpu_disable_depth_test();
    }
} zox_sys_end(ElementRenderMatrixSystem);
