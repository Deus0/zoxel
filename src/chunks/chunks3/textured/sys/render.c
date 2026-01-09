// #define zoxel_render3D_uvs_system_overdebug
// later store commands per material to optimize this process
// TODO: optimize transform so it doesn't update every frame? StaticTransform tag

#define catch_opengl_error(type)\
if (check_opengl_error_unlogged()) {\
        zox_log(" > could not render "type" [%i]: indicies [%i] - [%ix%i]\n", i, meshIndicies->length, meshGPULink->value.x, meshGPULink->value.y);\
        break;\
}

// TODO: For each chunk render, we use linked tilemap GPU data

zox_sys2(Chunk3TexturedRenderSystem) {
    byte has_set_material = 0;
    entity tilemap_ref = 0;
    const MaterialGPULink *gpu_material = NULL;
    const TextureGPULink *gpu_texture = NULL;
    const MaterialTextured3D *material_attributes = NULL;

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TransformMatrix);
    zox_sys_in(MeshGPULink);
    zox_sys_in(UvsGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_in(MeshIndiciesGpu);
    zox_sys_in(TilemapLink);
    zox_sys_in(RenderDisabled);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(MeshIndiciesGpu, count);
        zox_sys_i(MeshGPULink, gpu_mesh);
        zox_sys_i(TilemapLink, tilemap);
        zox_sys_i(UvsGPULink, gpu_uvs);
        zox_sys_i(ColorsGPULink, gpu_colors);
        zox_sys_i(TransformMatrix, matrix);

        if (disabled->value || !count || !gpu_mesh->value.x) {
            continue;
        }

        if (tilemap_ref != tilemap->value) {
            if (!zox_valid(tilemap->value)) {
                continue;
            }
            tilemap_ref = tilemap->value;

            gpu_material = zox_get(tilemap_ref, MaterialGPULink);
            gpu_texture = zox_get(tilemap_ref, TextureGPULink);
            material_attributes = zox_get(tilemap_ref, MaterialTextured3D);

            if (!gpu_material->value || !gpu_texture->value || !material_attributes) {
                continue;
            }
        }

        if (!has_set_material) {
            has_set_material = 1;
            zox_gpu_material(gpu_material->value);
            opengl_bind_texture(gpu_texture->value);
            zox_gpu_float4x4(material_attributes->camera_matrix, render_camera_matrix);
            zox_gpu_float4(material_attributes->fog_data, get_fog_value());
            zox_gpu_float(material_attributes->brightness, 1);
        }

        zox_gpu_float4x4(material_attributes->transform_matrix, matrix->value);
        opengl_set_mesh_indicies(gpu_mesh->value.x);
        opengl_enable_vertex_buffer(material_attributes->vertex_position, gpu_mesh->value.y);
        opengl_enable_uv_buffer(material_attributes->vertex_uv, gpu_uvs->value);
        opengl_enable_color_buffer(material_attributes->vertex_color, gpu_colors->value);
        zox_gpu_render(count->value);

    }

    if (has_set_material) {
        zox_gpu_disable_buffer(material_attributes->vertex_color);
        zox_gpu_disable_buffer(material_attributes->vertex_uv);
        zox_gpu_disable_buffer(material_attributes->vertex_position);
        opengl_unset_mesh();
        opengl_disable_texture(0);
        zox_disable_material();
    }

} zox_sys_end(Chunk3TexturedRenderSystem);