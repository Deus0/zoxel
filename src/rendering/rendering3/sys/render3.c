// NOTE: Rendering Skybox atm and debug cube
// where each main camera last drew a RenderAtCamera mesh (tst coop_skybox)
float3 render_at_camera_drawn[max_cameras];

void basic3_render_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_gl = 0;
    byte dbg_log = 0;
    if (dbg_gl && check_opengl_error_unlogged()) {
        zox_loge("pre basic3_render_system");
    }
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
        if (!mesh->value.x ||
            !mesh->value.y)
        {
            // zox_loge("(Basic3) Gpu links [mesh] broken [%s]", zox_getn(e));
            continue;
        }
        camera_filtering_check();
        if (!material->value) {
            zox_loge("(Basic3) Gpu Link [material] broken [%s]", zox_getn(e));
            continue;
        }
        if (attributes->vertex_position < 0 ||
            attributes->mvp_matrix < 0)
        {
            static byte warned = 0;
            if (!warned) {
                zox_loge("(Basic3) missing required locations [%s] pos [%i] mvp_matrix [%i] color [%i] fog [%i]",
                    zox_getn(e),
                    attributes->vertex_position, attributes->mvp_matrix,
                    attributes->color,
                    attributes->fog_data);
                warned = 1;
            }
            continue;
        }
        if (alpha->value < 1) {
            zox_gpu_enable_blend();
        }
        zox_gpu_material(material->value);
        float4x4 model = matrix->value;
        // the skybox: one for every camera, each drawn round its own (in split screen the second player saw the
        //  first's from outside, the world past its walls)
        if (zox_has(e, RenderAtCamera)) {
            model.w = (float4) { render_camera_position.x, render_camera_position.y, render_camera_position.z, model.w.w };
            for (int c = 0; c < main_cameras_count; c++) {
                if (main_cameras[c] == renderer_camera) {
                    render_at_camera_drawn[c] = (float3) { model.w.x, model.w.y, model.w.z };
                }
            }
        }
        float4x4 mvp_matrix = float4x4_multiply(
            model,
            render_camera_matrix);
        zox_gpu_float4x4(
            attributes->mvp_matrix,
            mvp_matrix);
        // zox_gpu_float4x4(attributes->camera_matrix, render_camera_matrix);
        if (attributes->fog_data) {
            zox_gpu_float4(attributes->fog_data, get_fog_value());
        }
        if (attributes->color) {
            float4 colorf = color_to_float4(fill->value);
            colorf.x *= brightness->value;
            colorf.y *= brightness->value;
            colorf.z *= brightness->value;
            colorf.w *= alpha->value;
            zox_gpu_float4(attributes->color, colorf);
        }
        // zox_gpu_float4x4(attributes->transform_matrix, matrix->value);
        zox_gpu_bind_buffer_element(mesh->value.x);
        opengl_enable_vertex_buffer(attributes->vertex_position, mesh->value.y);
        zox_gpu_render3(indicies->length);
        if (dbg_gl && check_opengl_error_unlogged()) {
            zox_loge("basic3_render_system [%s]",
                zox_sys_e_name);
        }
        if (dbg_log) {
            float3 position = matrix_to_position(matrix->value);
            zox_log("Rendering BasicMesh3D [%s] - Triangles [%i] Render Camera [%s] - Alpha [%f]",
                zox_getn(e),
                indicies->length,
                zox_getn(renderer_camera),
                alpha->value);
            zox_log("   - At [%fx%fx%f]", position.x, position.y, position.z);
            zox_log("   - GPU: Mesh [%ix%i]", mesh->value.x, mesh->value.y);
            zox_log("   - Locations: pos [%i] mvp_matrix [%i] color [%i] fog [%i]",
                attributes->vertex_position, attributes->mvp_matrix,
                attributes->color,
                attributes->fog_data);
        }
        zox_gpu_disable_attribute(attributes->vertex_position);
        zox_gpu_reset_mesh();
        zox_disable_material();
        if (alpha->value < 1) {
            zox_gpu_disable_blend();
        }
    }
    zox_sys_on_end();
} zoxd_system(basic3_render_system);
