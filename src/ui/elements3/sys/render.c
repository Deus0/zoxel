// NOTE: Renders our 3D uis
zox_sys2(Element3RenderSystem) {
    byte dbg_log = 0;
    byte dbg_gl = 0;
    entity material = material_textured3D;
    if (!material) {
        zox_loge("[material_textured3D] missing in Element3RenderSystem.");
        return;
    }
    zox_sys_world();
    byte has_set_material = 0;
    guint material_link = 0;
    const MaterialTextured3D* attributes = NULL;
    zox_sys_begin();
    zox_sys_in(RenderDisabled);
    zox_sys_in(TransformMatrix);
    zox_sys_in(MeshGPULink);
    zox_sys_in(UvsGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_in(TextureGPULink);
    zox_sys_in(MeshIndicies);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(TransformMatrix, matrix);
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(UvsGPULink, uvs);
        zox_sys_i(ColorsGPULink, colors);
        zox_sys_i(TextureGPULink, texture);
        zox_sys_i(MeshIndicies, indicies);
        if (disabled->value) { // || !indicies->length) {
            continue;
        }
        entity ui_holder = zox_get_parent_by_id(world, e, zox_id(UIHolderLink));
        if (zox_valid(ui_holder)) {
            if (zox_getv(ui_holder, RenderDisabled)) {
                continue;
            }
        }
#ifdef zox_safety_checks
        if (!mesh->value.x || !mesh->value.y) {
            zox_loge("Gpu links [mesh] broken on Element3 [%s]", zox_getn(e));
            continue;
        }
#endif
        if (!has_set_material) {
            has_set_material = 1;
            material_link = zox_getv(material, MaterialGPULink);
            attributes = zox_get(material, MaterialTextured3D);
            zox_gpu_enable_blend();
            zox_gpu_material(material_link);
            zox_gpu_float4x4(attributes->camera_matrix, render_camera_matrix);
            zox_gpu_float4(attributes->fog_data, get_fog_value());
            zox_gpu_float(attributes->brightness, 1);
            if (dbg_log) {
                entity shader = zox_getv(material, ShaderLink);
                zox_log("Initializing Element3 Material [%s]: %i - Shader [%s]", zox_getn(material), material_link, zox_getn(shader));
            }
            if (dbg_gl) {
                if (check_opengl_error_unlogged()) {
                    zox_loge("Element3RenderSystem");
                }
            }
        }
        zox_gpu_float4x4(attributes->transform_matrix, matrix->value);
        zox_gpu_bind_buffer_element(mesh->value.x);
        opengl_enable_vertex_buffer(attributes->vertex_position, mesh->value.y);
        opengl_enable_uv_buffer(attributes->vertex_uv, uvs->value);
        opengl_enable_color_buffer(attributes->vertex_color, colors->value);
        zox_gpu_bind_texture(texture->value);
        zox_gpu_render3(6); // indicies->length);
        catch_basic3D_errors("Element3RenderSystem");
        if (dbg_log) {
            float3 position = matrix_to_position(matrix->value);
            zox_log("Rendered Element3 [%s] Tris [%i] at [%fx%fx%f]", zox_getn(e), indicies->length, position.x, position.y, position.z);
            zox_log("   - Mesh [%ix%i] UVs [%i] Colors [%i] Texture [%i]", mesh->value.x, mesh->value.y, uvs->value, colors->value, texture->value);
        }
    }
    if (has_set_material) {
        zox_gpu_disable_blend();
        zox_gpu_disable_attribute(attributes->vertex_color);
        zox_gpu_disable_attribute(attributes->vertex_uv);
        zox_gpu_disable_attribute(attributes->vertex_position);
        zox_gpu_reset_texture();
        zox_gpu_reset_mesh();
        zox_disable_material();
    }
} zox_sys_end(Element3RenderSystem);
