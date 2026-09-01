// NOTE: Uplaods Terrain Chunks
void textured_mesh_update_system(iter* it) {
    zox_sys_on_begin();
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
        if (!gpu_mesh->value.x ||
            !gpu_mesh->value.y ||
            !gpu_uvs->value ||
            !gpu_colors->value)
        {
            continue;
        }
        if (verts->length != uvs->length) {
            // zox_sys_world();
            // zox_sys_e();
            zox_loge("[%s] mesh verts [%i] / uvs [%i] missmatch",
                zox_sys_e_name,
                verts->length,
                uvs->length);
            count->value = 0;
            continue;
        }
        zox_gpu_element_buffer(
            gpu_mesh->value.x,
            indicies->length,
            sizeof(int),
            indicies->value);
        zox_gpu_array_buffer(
            gpu_mesh->value.y,
            verts->length,
            sizeof(float3),
            verts->value);
        zox_gpu_array_buffer(
            gpu_uvs->value,
            verts->length,
            sizeof(float2),
            uvs->value);
        zox_gpu_array_buffer(
            gpu_colors->value,
            verts->length,
            sizeof(color_rgb),
            colors->value);
        count->value = indicies->length;
        zox_remove(e, MeshDirty);
        zox_add(e, MeshBuilt);
        if (dbg_log) {
            zox_log("Uploaded Chunk Mesh [%s] Tris [%i]",
                zox_get_name(e),
                count->value / 3);
        }
    }
    zox_sys_on_end();
} zoxd_system(textured_mesh_update_system);
