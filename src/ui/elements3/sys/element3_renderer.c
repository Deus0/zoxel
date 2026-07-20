// extern byte can_render_ui(ecs *world, entity e);
zox_sys2(Element3DRenderSystem) {
    byte dbg_log = 0;
    if (!material_textured3D) {
        zox_loge("[material_textured3D] missing in Element3DRenderSystem.");
        return;
    }
    zox_sys_world();
    byte has_set_material = 0;
    guint material_link = zox_getv(material_textured3D, MaterialGPULink);
    const MaterialTextured3D* material_attributes = zox_get(material_textured3D, MaterialTextured3D);
    zox_sys_begin();
    zox_sys_in(TransformMatrix);
    zox_sys_in(MeshGPULink);
    zox_sys_in(UvsGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_in(MeshIndicies);
    zox_sys_in(TextureGPULink);
    zox_sys_in(RenderDisabled);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(MeshIndicies, indicies);
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(TransformMatrix, matrix);
        zox_sys_i(UvsGPULink, uvsGPULink);
        zox_sys_i(ColorsGPULink, colorsGPULink);
        zox_sys_i(TextureGPULink, textureGPULink);
        if (disabled->value || !indicies->length) {
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
            zox_loge("Gpu links [mesh] broken on Element3D [%s]", zox_getn(e));
            continue;
        }
#endif
        if (!has_set_material) {
            has_set_material = 1;
            zox_gpu_enable_blend();
            zox_gpu_material(material_link);
            zox_gpu_float4x4(material_attributes->camera_matrix, render_camera_matrix);
            zox_gpu_float4(material_attributes->fog_data, get_fog_value());
            zox_gpu_float(material_attributes->brightness, 1);
        }
        zox_gpu_float4x4(material_attributes->transform_matrix, matrix->value);
        zox_gpu_bind_buffer_element(mesh->value.x);
        opengl_enable_vertex_buffer(material_attributes->vertex_position, mesh->value.y);
        opengl_enable_uv_buffer(material_attributes->vertex_uv, uvsGPULink->value);
        opengl_enable_color_buffer(material_attributes->vertex_color, colorsGPULink->value);
        opengl_bind_texture(textureGPULink->value);
        zox_gpu_render(indicies->length);
        catch_basic3D_errors("Element3DRenderSystem");
        if (dbg_log) {
            zox_log("Rendered Element3D [%s] Tris [%i]", zox_getn(e), indicies->length);
        }
    }
    if (has_set_material) {
        zox_gpu_disable_blend();
        zox_gpu_disable_attribute(material_attributes->vertex_color);
        zox_gpu_disable_attribute(material_attributes->vertex_uv);
        zox_gpu_disable_attribute(material_attributes->vertex_position);
        opengl_reset_texture();
        zox_gpu_reset_mesh();
        zox_disable_material();
    }
} zox_sys_end(Element3DRenderSystem);
