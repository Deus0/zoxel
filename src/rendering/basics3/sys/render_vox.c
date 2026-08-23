// for unique meshes - not atm used
zox_sys2(VoxRenderSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    if (!zox_valid(material_colored3D)) {
        return;
    }
    guint material_link = zox_getv(material_colored3D, MaterialGPULink);
    if (!material_link) {
        return;
    }
    zox_geter(material_colored3D, MaterialColored3D, material_attributes);
    byte has_set_material = 0;
    zox_sys_begin();
    zox_sys_in(MeshIndicies);
    zox_sys_in(MeshGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_in(TransformMatrix);
    zox_sys_in(RenderDisabled);
    camera_filtering_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(MeshIndicies, meshIndicies);
        zox_sys_i(MeshGPULink, meshGPULink);
        zox_sys_i(ColorsGPULink, colorsGPULink);
        zox_sys_i(TransformMatrix, transformMatrix);
        if (disabled->value ||
            !meshIndicies->length ||
            !meshGPULink->value.x ||
            !meshGPULink->value.y ||
            !colorsGPULink->value)
        {
            continue;
        }
        camera_filtering_check();
        if (!has_set_material) {
            has_set_material = 1;
/*#ifdef zox_transparent_voxes
            zox_gpu_enable_blend();
            zox_gpu_disable_culling();
#endif*/
            zox_gpu_material(material_link);
            zox_gpu_float4x4(material_attributes->camera_matrix, render_camera_matrix);
            zox_gpu_float4(material_attributes->fog_data, get_fog_value());
            zox_gpu_float(material_attributes->brightness, 1);
        }
        zox_gpu_bind_buffer_element(meshGPULink->value.x);
        opengl_enable_vertex_buffer(material_attributes->vertex_position, meshGPULink->value.y);
        opengl_enable_color_buffer(material_attributes->vertex_color, colorsGPULink->value);
        zox_gpu_float4x4(material_attributes->transform_matrix, transformMatrix->value);
        zox_gpu_render3(meshIndicies->length);
        if (dbg_log) {
            zox_log("Rendering Vox Mesh [%s] - Tris [%i]", zox_sys_e_name, meshIndicies->length / 3);
        }
    }
    if (has_set_material) {
        zox_gpu_disable_attribute(material_attributes->vertex_color);
        zox_gpu_disable_attribute(material_attributes->vertex_position);
        zox_gpu_reset_mesh();
        zox_disable_material();
/*#ifdef zox_transparent_voxes
        zox_gpu_disable_blend();
        zox_gpu_enable_culling();
#endif*/
    }
} zox_sys_end(VoxRenderSystem);
