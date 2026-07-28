// TODO: For each chunk render, we use linked tilemap GPU data
zox_sys2(Chunk3TexturedRenderSystem) {
    byte dbg_log = 0;
    byte dbg_gl = 0;
    byte initialized_material = 0;
    const MaterialTextured3D* attributes = NULL;
    zox_sys_world();
    camera_filtering_begin();
    zox_sys_begin();
    // zox_sys_in(Active);
    zox_sys_in(RenderDisabled);
    zox_sys_in(TransformMatrix);
    zox_sys_in(MeshGPULink);
    zox_sys_in(UvsGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_in(MeshRenderCount);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        // zox_sys_i(Active, active);
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(TransformMatrix, matrix);
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(UvsGPULink, gpu_uvs);
        zox_sys_i(ColorsGPULink, gpu_colors);
        zox_sys_i(MeshRenderCount, count);
        if (disabled->value || !count->value) {
            continue;
        }
#ifdef zox_safety_checks
        if (!mesh->value.x || !mesh->value.y) {
            zox_loge("Mesh Invalid [%s]", zox_getn(e));
            continue;
        }
        if (!gpu_uvs->value) {
            zox_loge("Chunk Mesh UVs Invalid [%s]", zox_getn(e));
            continue;
        }
        if (!gpu_colors->value) {
            zox_loge("Chunk Mesh Colors Invalid [%s]", zox_getn(e));
            continue;
        }
#endif
        camera_filtering_check();
        // TODO: Swapping / Grouping Tilemaps
        if (!initialized_material) {
            entity chunk = zox_get_parent(world, e);
#ifdef zox_safety_checks
            if (!zox_valid(chunk)) {
                zox_loge("Chunk Invalid for ChunkMesh %s", zox_getn(e));
                continue;
            }
#endif
            entity terrain = zox_get_parent(world, chunk);
#ifdef zox_safety_checks
            if (!zox_valid(terrain)) {
                zox_loge("terrain Invalid for ChunkMesh %s", zox_getn(e));
                continue;
            }
            if (!zox_has(terrain, TilemapLink)) {
                zox_loge("e [%s] terrain has no TilemapLink %s", zox_getn(e), zox_getn(terrain));
                continue;
            }
#endif
            entity tilemap = zox_getv(terrain, TilemapLink);
#ifdef zox_safety_checks
            if (!zox_valid(tilemap)) {
                zox_loge("Tilemap Invalid for Chunk %s", zox_getn(e));
                continue;
            }
#endif
            guint material = zox_getv(tilemap, MaterialGPULink);
            guint texture = zox_getv(tilemap, TextureGPULink);
            attributes = zox_get(tilemap, MaterialTextured3D);
            if (!material || !texture || !attributes) {
                zox_loge("Invalid Tilemap [%s] in Terrain Renderer", zox_getn(tilemap));
                continue;
            }
            zox_gpu_material(material);
            zox_gpu_float4x4(attributes->camera_matrix, render_camera_matrix);
            zox_gpu_float4(attributes->fog_data, get_fog_value());
            zox_gpu_float(attributes->brightness, 1);
            zox_gpu_bind_texture(texture);
            initialized_material = 1;
        }
        zox_gpu_bind_buffer_element(mesh->value.x);
        opengl_enable_vertex_buffer(attributes->vertex_position, mesh->value.y);
        opengl_enable_uv_buffer(attributes->vertex_uv, gpu_uvs->value);
        opengl_enable_color_buffer(attributes->vertex_color, gpu_colors->value);
        zox_gpu_float4x4(attributes->transform_matrix, matrix->value);
        // Rendering!
        zox_gpu_render3(count->value);
        if (dbg_gl) {
            if (check_opengl_error_unlogged()) {
                zox_loge("Chunk3TexturedRenderSystem");
            }
        }
        if (dbg_log) {
            entity chunk = zox_get_parent(world, e);
            float3 position = matrix_to_position(matrix->value);
            float3 scale = matrix_to_scale(matrix->value);
            zox_log("Rendering Chunk [%s] Mesh [%s] Tris [%i] at [%fx%fx%f] Scale [%fx%fx%f]", zox_getn(chunk), zox_getn(e), count->value, position.x, position.y, position.z, scale.x, scale.y, scale.z);
        }
        zox_sys_increment();
    }
    if (initialized_material) {
        zox_gpu_disable_attribute(attributes->vertex_color);
        zox_gpu_disable_attribute(attributes->vertex_uv);
        zox_gpu_disable_attribute(attributes->vertex_position);
        zox_gpu_reset_texture();
        zox_gpu_reset_mesh();
        zox_disable_material();
    }
} zox_sys_end(Chunk3TexturedRenderSystem);
