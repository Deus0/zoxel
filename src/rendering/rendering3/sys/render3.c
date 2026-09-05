// NOTE: Rendering Skybox atm and debug cube
zox_sys2(Basic3RenderSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TransformMatrix);
    zox_sys_in(Color);
    zox_sys_in(Alpha);
    zox_sys_in(Brightness);
    zox_sys_in(MeshGPULink);
    zox_sys_in(MeshIndicies);
    zox_sys_in(MaterialGPULink);
    zox_sys_in(MaterialBasic3D);
    camera_filtering_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TransformMatrix, matrix);
        zox_sys_i(Color, fill);
        zox_sys_i(Alpha, alpha);
        zox_sys_i(Brightness, brightness);
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(MeshIndicies, indicies);
        zox_sys_i(MaterialGPULink, material);
        zox_sys_i(MaterialBasic3D, attributes);
        if (!indicies->length) {
            continue;
        }
        camera_filtering_check();
        if (!material->value) {
            zox_loge("(Basic3) Gpu Link [material] broken [%s]", zox_getn(e));
            continue;
        }
        if (!mesh->value.x ||
            !mesh->value.y)
        {
            // zox_loge("(Basic3) Gpu links [mesh] broken [%s]", zox_getn(e));
            continue;
        }
        if (alpha->value < 1) {
            zox_gpu_enable_blend();
        }
        zox_gpu_material(material->value);
        zox_gpu_float4x4(attributes->camera_matrix, render_camera_matrix);
        zox_gpu_float4(attributes->fog_data, get_fog_value());
        float4 colorf = color_to_float4(fill->value);
        colorf.x *= brightness->value;
        colorf.y *= brightness->value;
        colorf.z *= brightness->value;
        colorf.w *= alpha->value;
        zox_gpu_float4(attributes->color, colorf);
        zox_gpu_float4x4(attributes->transform_matrix, matrix->value);
        zox_gpu_bind_buffer_element(mesh->value.x);
        opengl_enable_vertex_buffer(attributes->vertex_position, mesh->value.y);
        zox_gpu_render3(indicies->length);
        if (dbg_log) {
            float3 position = matrix_to_position(matrix->value);
            zox_log("Rendering BasicMesh3D [%s] - Triangles [%i] Render Camera [%s]", zox_getn(e),  indicies->length, zox_getn(renderer_camera));
            zox_log("   - At [%fx%fx%f]", position.x, position.y, position.z);
            zox_log("   - GPU: Mesh [%ix%i]", mesh->value.x, mesh->value.y);
            zox_log("   - Attributes: %i %i %i", attributes->vertex_position, attributes->transform_matrix, attributes->color);
        }
        zox_gpu_disable_attribute(attributes->vertex_position);
        zox_gpu_reset_mesh();
        zox_disable_material();
        if (alpha->value < 1) {
            zox_gpu_disable_blend();
        }
    }
} zox_sys_end(Basic3RenderSystem);
