// TODO: For each chunk render, we use linked tilemap GPU data
// draws chunk meshes with their tilemap; see through ones (TransparentMesh: water) blended, both faces, after the rest

// the draw order: as they come, or far to near from the camera (see through ones: water seen through water blends
//  over it, not hidden by the nearer one's depth), an insertion sort of a table's meshes
static inline void render_chunk_order(const TransformMatrix* matrices, int count, byte far_first, int* order) {
    float distances[count];
    for (int n = 0; n < count; n++) {
        order[n] = n;
        distances[n] = far_first ? float3_distance_squared(matrix_to_position(matrices[n].value), render_camera_position) : 0;
    }
    for (int n = 1; far_first && n < count; n++) {
        const int index = order[n];
        int m = n - 1;
        while (m >= 0 && distances[order[m]] < distances[index]) {
            order[m + 1] = order[m];
            m--;
        }
        order[m + 1] = index;
    }
}

// returns how many it drew (the systems count them, SystemProcessed)
static inline uint render_chunk_meshes(iter* it, byte transparent) {
    uint drawn = 0;
    byte dbg_log = 0;
    byte dbg_gl = 0;
    byte initialized_material = 0;
    const MaterialTextured3D* attributes = NULL;
    zox_sys_world();
    camera_filtering_begin();
    zox_sys_begin();
    zox_sys_in(RenderDisabled);
    zox_sys_in(TransformMatrix);
    // zox_sys_in(MaterialLink);
    zox_sys_in(MeshGPULink);
    zox_sys_in(UvsGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_in(MeshRenderCount);
    int order[it->count];
    render_chunk_order(TransformMatrix_, it->count, transparent, order);
    for (int n = 0; n < it->count; n++) {
        const int i = order[n];
        zox_sys_e();
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(TransformMatrix, matrix);
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(UvsGPULink, gpu_uvs);
        zox_sys_i(ColorsGPULink, gpu_colors);
        zox_sys_i(MeshRenderCount, count);
        if (disabled->value || !count->value) {
            if (dbg_log >= 2) {
                zox_log("Chunk Textured !Rendering [%i:%i]",
                    disabled->value,
                    count->value);
            }
            continue;
        }
        if (!mesh->value.x || !mesh->value.y) {
            if (dbg_log) {
                zox_loge("Mesh Invalid [%s]",
                    zox_getn(e));
            }
            continue;
        }
        if (!gpu_uvs->value) {
            if (dbg_log) {
                zox_loge("Chunk Mesh UVs Invalid [%s]",
                    zox_getn(e));
            }
            continue;
        }
        if (!gpu_colors->value) {
            if (dbg_log) {
                zox_loge("Chunk Mesh Colors Invalid [%s]",
                    zox_getn(e));
            }
            continue;
        }
        camera_filtering_check();
        // TODO: Swapping / Grouping Tilemaps
        if (!initialized_material) {
            entity material = zox_get_link(world, e, MaterialLink);
#ifdef zox_safety_checks
            if (!zox_valid(material)) {
                zox_loge("Tilemap (material) Invalid for Chunk %s",
                    zox_getn(e));
                continue;
            }
#endif
            guint material_id = zox_getv(material, MaterialGPULink);
            guint texture_id = zox_getv(material, TextureGPULink);
            attributes = zox_get(material, MaterialTextured3D);
            if (!material_id || !texture_id || !attributes) {
                if (dbg_log) {
                    zox_loge("Invalid Tilemap [%s] in Terrain Renderer",
                        zox_getn(material));
                }
                continue;
            }
            if (transparent) {
                zox_gpu_enable_blend();
                zox_gpu_disable_culling();
            }
            zox_gpu_material(material_id);
            zox_gpu_bind_texture(texture_id);
            /*zox_gpu_float4x4(
                attributes->camera_matrix,
                render_camera_matrix);*/
            zox_gpu_float(
                attributes->brightness,
                1);
            initialized_material = 1;
        }
        // a mesh with its own texture (a liquid's, not the tilemap's) binds it
        if (transparent) {
            const entity own = zox_get_link(world, e, MeshTextureLink);
            const guint own_id = zox_valid(own) && zox_has(own, TextureGPULink) ? zox_getv(own, TextureGPULink) : 0;
            if (own_id) {
                zox_gpu_bind_texture(own_id);
            }
        }
        float4x4 mvp_matrix = float4x4_multiply(
            matrix->value,
            render_camera_matrix);
        zox_gpu_float4x4(
            attributes->mvp_matrix,
            mvp_matrix);
        zox_gpu_bind_buffer_element(mesh->value.x);
        opengl_enable_vertex_buffer(
            attributes->vertex_position,
            mesh->value.y);
        opengl_enable_uv_buffer(
            attributes->vertex_uv,
            gpu_uvs->value);
        opengl_enable_color_buffer(
            attributes->vertex_color,
            gpu_colors->value);
        /*zox_gpu_float4x4(
            attributes->transform_matrix,
            matrix->value);*/
        // Rendering!
        zox_gpu_render3(count->value);
        __atomic_add_fetch(&zox_drawn_chunks, 1, __ATOMIC_RELAXED);    // its parts can run on threads
        if (dbg_gl) {
            if (check_opengl_error_unlogged()) {
                zox_loge("chunk3_textured_render_system");
            }
        }
        if (dbg_log) {
            entity chunk = zox_get_parent(world, e);
            float3 position = matrix_to_position(matrix->value);
            float3 scale = matrix_to_scale(matrix->value);
            zox_log("Rendering Chunk [%s] Mesh [%s] Tris [%i] at [%fx%fx%f] Scale [%fx%fx%f]",
                zox_getn(chunk),
                zox_getn(e),
                count->value,
                position.x,
                position.y,
                position.z,
                scale.x,
                scale.y,
                scale.z);
        }
        drawn++;
    }
    if (initialized_material) {
        zox_gpu_disable_attribute(attributes->vertex_color);
        zox_gpu_disable_attribute(attributes->vertex_uv);
        zox_gpu_disable_attribute(attributes->vertex_position);
        zox_gpu_reset_texture();
        zox_gpu_reset_mesh();
        zox_disable_material();
        if (transparent) {
            zox_gpu_disable_blend();
            zox_gpu_enable_culling();
        }
    }
    return drawn;
}

void chunk3_textured_render_system(iter* it) {
    zox_sys_on_begin();
    process_count += render_chunk_meshes(it, 0);
    zox_sys_on_end();
} zoxd_system(chunk3_textured_render_system);

void chunk3_transparent_render_system(iter* it) {
    zox_sys_on_begin();
    process_count += render_chunk_meshes(it, 1);
    zox_sys_on_end();
} zoxd_system(chunk3_transparent_render_system);

