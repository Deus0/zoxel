// this is more like a non blend pass!
// NOTE: Needs to skip GPU calls for non layers since called per layer
/*zox_sys2(RenderTextureRenderSystem) {
    return;
    byte dbg_log = 0;
    float depth_per_layer = 0.001f;
    float depth_begin = depth_per_layer;
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
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(UvsGPULink, uvs);
        zox_sys_i(TextureGPULink, texture);
        if (disabled->value) {
            continue;
        }
        if (layer->value != renderer_layer) {
            continue;
        }
        entity root_camera = zox_get_root_canvas_camera(world, e);
        if (root_camera != renderer_camera) {
            continue;
        }
#ifdef zox_safety_checks
        if (!mesh->value.x || !mesh->value.y || !uvs->value || !texture->value) {
            zox_logw("RenderTexture has invalid gpu link(s)");
            continue;
        }
#endif
        if (!init) {
            init = 1;
            zox_geter_value(mat, MaterialGPULink, uint, mlink);
            zox_gpu_material(mlink);
            zox_gpu_float4x4(attributes->camera_matrix, render_camera_matrix);
            zox_gpu_disable_blend();
            if (zox_has(mat, CameraBlur)) {
                zox_geter_value(mat,  CameraBlur, float, blur);
                zox_gpu_float(attributes->blur_strength, blur);
                // zox_log("Set Camera Blur [%s] %f", zox_get_name(camera->value), blur);
            }
            // zox_gpu_float(attributes->blur_strength, 1);
        }
        float depth = depth_begin + layer->value * depth_per_layer;
        float4x4 matrix2 = matrix->value;
        matrix2.w.z += depth;
        // Bind data in GPU
        zox_gpu_bind_buffer_element(mesh->value.x);
        zox_gpu_bind_buffer_array(mesh->value.y);
        zox_gpu_enable_attribute_float2(attributes->vertex_position);
        zox_gpu_bind_buffer_array(uvs->value);
        zox_gpu_enable_attribute_float2(attributes->vertex_uv);
        zox_gpu_bind_texture(texture->value);
        zox_gpu_float4x4(attributes->transform_matrix, matrix2);
        // Call our render of our quad
        zox_gpu_render(6);
        if (dbg_log) {
            zox_log("%s Rendering Texture  Layer [%i] Depth [%f]", zox_get_name(e), layer->value, depth);
        }
    }
    if (init) {
        zox_gpu_disable_attribute(attributes->vertex_uv);
        zox_gpu_disable_attribute(attributes->vertex_position);
        zox_gpu_reset_mesh();
        zox_gpu_disable_blend();
        zox_gpu_reset_texture();
        zox_disable_material();
    }
} zox_sys_end(RenderTextureRenderSystem);*/
