entity shader_matrixui = 0;
entity material_matrixui = 0;

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
    attributes_matrixui attributes = (attributes_matrixui) {
        zox_gpu_get_material_attribute(material, "vertex_position"),
        zox_gpu_get_material_attribute(material, "vertex_uv"),
        zox_gpu_get_material_property(material, "matrix"),
        zox_gpu_get_material_property(material, "camera_matrix"),
        zox_gpu_get_material_property(material, "texture"),
        zox_gpu_get_material_property(material, "brightness"),
        zox_gpu_get_material_property(material, "alpha")
    };
    zox_set_data(e, attributes_matrixui, attributes);
    return e;
}

// NOTE: Needs to skip GPU calls for non layers since called per layer
zox_sys2(ElementRenderMatrixSystem) {
    byte is_log = 0;
    zox_sys_world();
    if (!zox_valid(material_matrixui)) {
        return;
    }
    byte init = 0;
    // float position_z = 0;
    zox_geter(material_matrixui, attributes_matrixui, attributes);
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
            uint mid = zox_getv(material_matrixui, MaterialGPULink);
            zox_gpu_material(mid);
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
        zox_gpu_float4x4(attributes->matrix, matrix->value);
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
} zox_sys_end(ElementRenderMatrixSystem);
