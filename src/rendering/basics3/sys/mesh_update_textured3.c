// NOTE: Uplaods Terrain Chunks
zox_sys2(TexturedMeshUploadSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MeshIndicies);
    zox_sys_in(MeshVertices);
    zox_sys_in(MeshUVs);
    zox_sys_in(MeshColorRGBs);
    zox_sys_in(MeshGPULink);
    zox_sys_in(UvsGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_out(TexturedMeshDirty);
    zox_sys_out(MeshRenderCount);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MeshGPULink, gpu_mesh);
        zox_sys_i(UvsGPULink, gpu_uvs);
        zox_sys_i(ColorsGPULink, gpu_colors);
        zox_sys_i(MeshIndicies, indicies);
        zox_sys_i(MeshVertices, verts);
        zox_sys_i(MeshUVs, uvs);
        zox_sys_i(MeshColorRGBs, colors);
        zox_sys_o(TexturedMeshDirty, upload);
        zox_sys_o(MeshRenderCount, count);
        if (!upload->value) { // mesh_state_upload) {
            continue;
        }
        if (!gpu_mesh->value.x || !gpu_mesh->value.y || !gpu_uvs->value || !gpu_colors->value) {
            zox_loge("[%s] Missing GPU Link", zox_get_name(e));
            count->value = 0;
            continue;
        }
        if (verts->length != uvs->length) {
            zox_sys_world();
            zox_sys_e();
            zox_loge("[%s] mesh verts [%i] / uvs [%i] missmatch", zox_get_name(e), verts->length, uvs->length);
            continue;
        }
        /*if (!indicies->length || !verts->length || !uvs->length) {
            count->value = 0;
            continue;
        }*/
        zox_gpu_element_buffer(gpu_mesh->value.x, indicies->length, sizeof(int), indicies->value);
        zox_gpu_array_buffer(gpu_mesh->value.y, verts->length, sizeof(float3), verts->value);
        zox_gpu_array_buffer(gpu_uvs->value, verts->length, sizeof(float2), uvs->value);
        zox_gpu_array_buffer(gpu_colors->value, verts->length, sizeof(color_rgb), colors->value);
        count->value = indicies->length;
        upload->value = 0;
        if (dbg_log) {
            zox_log("Uploaded Chunk Mesh [%s] Tris [%i]", zox_get_name(e), indicies->length);
        }
    }
} zox_sys_end(TexturedMeshUploadSystem);
