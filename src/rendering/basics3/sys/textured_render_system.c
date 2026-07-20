// World Items
zox_sys2(TexturedRenderSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TransformMatrix);
    zox_sys_in(MeshGPULink);
    zox_sys_in(UvsGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_in(MeshIndicies);
    zox_sys_in(RenderDisabled);
    zox_sys_in(MaterialGPULink);
    zox_sys_in(TextureGPULink);
    zox_sys_in(MaterialTextured3D);
    camera_filtering_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(MeshIndicies, indicies);
        zox_sys_i(MeshGPULink, meshGPULink);
        zox_sys_i(MaterialGPULink, materialGPULink);
        zox_sys_i(TextureGPULink, textureGPULink);
        zox_sys_i(TransformMatrix, matrix);
        zox_sys_i(UvsGPULink, uvsGPULink);
        zox_sys_i(ColorsGPULink, colorsGPULink);
        zox_sys_i(MaterialTextured3D, material_attributes);
        if (disabled->value) continue;
        if (!indicies->length) continue;
        if (!meshGPULink->value.x) continue;
        if (!materialGPULink->value) continue;
        if (!textureGPULink->value) continue;
        camera_filtering_check();
        zox_gpu_material(materialGPULink->value);
        opengl_bind_texture(textureGPULink->value);
        zox_gpu_float4x4(material_attributes->camera_matrix, render_camera_matrix);
        zox_gpu_float4(material_attributes->fog_data, get_fog_value());
        zox_gpu_float(material_attributes->brightness, 1);
        zox_gpu_float4x4(material_attributes->transform_matrix, matrix->value);
        zox_gpu_bind_buffer_element(meshGPULink->value.x);
        opengl_enable_vertex_buffer(material_attributes->vertex_position, meshGPULink->value.y);
        opengl_enable_uv_buffer(material_attributes->vertex_uv, uvsGPULink->value);
        opengl_enable_color_buffer(material_attributes->vertex_color, colorsGPULink->value);
        zox_gpu_render(indicies->length);
        // disabling
        zox_gpu_disable_attribute(material_attributes->vertex_color);
        zox_gpu_disable_attribute(material_attributes->vertex_uv);
        zox_gpu_disable_attribute(material_attributes->vertex_position);
        zox_gpu_reset_mesh();
        opengl_reset_texture();
        zox_disable_material();
        if (dbg_log) {
            float3 position = matrix_to_position(matrix->value);
            zox_log("Rendering Cube [%s] at [%fx%fx%f] - Triangles [%i]", zox_getn(e), position.x, position.y, position.z, indicies->length);
        }
        // catch_basic3D_errors("! TexturedRenderSystem");
    }
} zox_sys_end(TexturedRenderSystem);
