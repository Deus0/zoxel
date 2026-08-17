// NOTE: Uplaods Terrain Chunks
zox_sys2(TexturedMeshUploadSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MeshGPULink);
    zox_sys_in(UvsGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_out(MeshIndicies);
    zox_sys_out(MeshVertices);
    zox_sys_out(MeshUVs);
    zox_sys_in(MeshColorRGBs);
    zox_sys_out(MeshRenderCount);
    zox_sys_out(MeshDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MeshGPULink, gpu_mesh);
        zox_sys_i(UvsGPULink, gpu_uvs);
        zox_sys_i(ColorsGPULink, gpu_colors);
        zox_sys_o(MeshIndicies, indicies);
        zox_sys_o(MeshVertices, verts);
        zox_sys_o(MeshUVs, uvs);
        zox_sys_i(MeshColorRGBs, colors);
        zox_sys_o(MeshRenderCount, count);
        zox_sys_o(MeshDirty, upload);
        if (upload->value != mesh_state_upload) {
            continue;
        }
// #ifdef zox_safety_checks
        if (!gpu_mesh->value.x || !gpu_mesh->value.y) {
            //zox_loge("Invalid [MeshGPULink] on [%s]", zox_sys_e_name);
            //count->value = 0;
            continue;
        }
        if (!gpu_uvs->value) {
            //zox_loge("Invalid [UvsGPULink] on [%s]", zox_sys_e_name);
            //count->value = 0;
            continue;
        }
        if (!gpu_colors->value) {
            //zox_loge("Invalid [ColorsGPULink] on [%s]", zox_sys_e_name);
            //count->value = 0;
            continue;
        }
        if (verts->length != uvs->length) {
            // zox_sys_world();
            // zox_sys_e();
            zox_loge("[%s] mesh verts [%i] / uvs [%i] missmatch", zox_sys_e_name, verts->length, uvs->length);
            count->value = 0;
            continue;
        }
// #endif
        zox_gpu_element_buffer(gpu_mesh->value.x, indicies->length, sizeof(int), indicies->value);
        zox_gpu_array_buffer(gpu_mesh->value.y, verts->length, sizeof(float3), verts->value);
        zox_gpu_array_buffer(gpu_uvs->value, verts->length, sizeof(float2), uvs->value);
        zox_gpu_array_buffer(gpu_colors->value, verts->length, sizeof(color_rgb), colors->value);
        count->value = indicies->length;
        zox_add(e, MeshBuilt);
        zox_remove(e, MeshDirty);
        if (dbg_log) {
            zox_log("Uploaded Chunk Mesh [%s] Tris [%i]", zox_get_name(e), count->value / 3);
        }
    }
} zox_sys_end(TexturedMeshUploadSystem);
