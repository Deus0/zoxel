// World Items / cube textured
// NOTE: For Unique Materials on a Mesh
zox_sys2(TexturedRenderSystem) {
    byte dbg_log = 0;
    byte dbg_gl = 0;
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
        zox_sys_i(TransformMatrix, matrix);
        zox_sys_i(MeshIndicies, indicies);
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(MaterialGPULink, material);
        zox_sys_i(TextureGPULink, texture);
        zox_sys_i(UvsGPULink, uvs);
        zox_sys_i(ColorsGPULink, colors);
        zox_sys_i(MaterialTextured3D, attributes);
        if (disabled->value || !indicies->length) {
            if (dbg_log >= 2) {
                zox_log("Invisible Mesh [%s]",
                    zox_getn(e));
            }
            continue;
        }
        if (!material->value ||
            !texture->value ||
            !mesh->value.x ||
            !mesh->value.y ||
            !uvs->value ||
            !colors->value
        ) {
            continue;
        }
        camera_filtering_check();
        zox_gpu_material(material->value);
        zox_gpu_bind_texture(texture->value);
        // glActiveTexture(GL_TEXTURE0);
        zox_gpu_float4x4(
            attributes->camera_matrix,
            render_camera_matrix);
        zox_gpu_float4(
            attributes->fog_data,
            get_fog_value());
        zox_gpu_float(attributes->brightness, 1);
        zox_gpu_float4x4(
            attributes->transform_matrix,
            matrix->value);
        zox_gpu_bind_buffer_element(mesh->value.x);
        opengl_enable_vertex_buffer(
            attributes->vertex_position,
            mesh->value.y);
        opengl_enable_uv_buffer(
            attributes->vertex_uv,
            uvs->value);
        opengl_enable_color_buffer(
            attributes->vertex_color,
            colors->value);
        // Render!
        zox_gpu_render3(indicies->length);
        // disabling
        zox_gpu_disable_attribute(attributes->vertex_color);
        zox_gpu_disable_attribute(attributes->vertex_uv);
        zox_gpu_disable_attribute(attributes->vertex_position);
        zox_gpu_reset_mesh();
        zox_gpu_reset_texture();
        zox_disable_material();
        if (dbg_gl) {
            if (check_opengl_error_unlogged()) {
                zox_loge("TexturedRenderSystem");
            }
        }
        if (dbg_log) {
            float3 position = matrix_to_position(matrix->value);
            zox_log("Rendering Mesh [%s] at [%fx%fx%f] - Triangles [%i]",
                zox_getn(e),
                position.x,
                position.y,
                position.z,
                indicies->length);
            zox_log("   - GPU: Mesh [%ix%i] UVs [%i] Colors [%i] Texture [%i]",
                mesh->value.x,
                mesh->value.y,
                uvs->value,
                colors->value,
                texture->value);
            zox_log("   - Attributes: %i %i %i",
                attributes->vertex_position,
                attributes->vertex_uv,
                attributes->vertex_color);
        }
    }
} zox_sys_end(TexturedRenderSystem);
