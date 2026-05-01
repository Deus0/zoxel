// #define zoxel_render3D_uvs_system_overdebug
// later store commands per material to optimize this process
// TODO: optimize transform so it doesn't update every frame? StaticTransform tag

/*#define catch_opengl_error(type)\
if (check_opengl_error_unlogged()) {\
        zox_log(" > could not render "type" [%i]: indicies [%i] - [%ix%i]\n", i, meshIndicies->length, meshGPULink->value.x, meshGPULink->value.y);\
        break;\
}*/

// TODO: For each chunk render, we use linked tilemap GPU data

zox_sys2(Chunk3TexturedRenderSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TransformMatrix);
    zox_sys_in(MeshGPULink);
    zox_sys_in(UvsGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_in(MeshIndiciesGpu);
    zox_sys_in(TilemapLink);
    zox_sys_in(RenderDisabled);

    byte init = 0;
    const MaterialTextured3D *attributes = NULL;

    camera_filtering_begin();

    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(MeshIndiciesGpu, count);
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(TilemapLink, tilemap);
        zox_sys_i(UvsGPULink, gpu_uvs);
        zox_sys_i(ColorsGPULink, gpu_colors);
        zox_sys_i(TransformMatrix, matrix);

        if (disabled->value || !count || !mesh->value.x) {
            continue;
        }

        if (!zox_valid(tilemap->value)) {
            continue;
        }

        camera_filtering_check();

        if (!init) {
            zox_geter_value(tilemap->value, MaterialGPULink, uint, material);
            zox_geter_value(tilemap->value, TextureGPULink, uint, texture);
            attributes = zox_get(tilemap->value, MaterialTextured3D);

            if (!material || !texture || !attributes) {
                continue;
            }

            zox_gpu_material(material);
            opengl_bind_texture(texture);
            zox_gpu_float4x4(attributes->camera_matrix, render_camera_matrix);
            zox_gpu_float4(attributes->fog_data, get_fog_value());
            zox_gpu_float(attributes->brightness, 1);

            init = 1;
        }

        zox_gpu_float4x4(attributes->transform_matrix, matrix->value);
        zox_gpu_bind_buffer_element(mesh->value.x);
        opengl_enable_vertex_buffer(attributes->vertex_position, mesh->value.y);
        opengl_enable_uv_buffer(attributes->vertex_uv, gpu_uvs->value);
        opengl_enable_color_buffer(attributes->vertex_color, gpu_colors->value);

        zox_gpu_render(count->value);
    }

    if (!init) {
        return;
    }

    zox_gpu_disable_attribute(attributes->vertex_color);
    zox_gpu_disable_attribute(attributes->vertex_uv);
    zox_gpu_disable_attribute(attributes->vertex_position);
    zox_gpu_reset_mesh();
    opengl_reset_texture();
    zox_disable_material();

} zox_sys_end(Chunk3TexturedRenderSystem);
