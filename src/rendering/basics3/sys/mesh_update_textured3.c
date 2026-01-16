// Uplaods Terrain Chunks
zox_sys2(Mesh3DTexturedUploadSystem) {
    zox_sys_begin();
    zox_sys_in(MeshIndicies);
    zox_sys_in(MeshVertices);
    zox_sys_in(MeshUVs);
    zox_sys_in(MeshColorRGBs);
    zox_sys_in(MeshGPULink);
    zox_sys_in(UvsGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_in(MeshDirty);
    zox_sys_out(MeshIndiciesGpu);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(MeshDirty, mesh_dirty);
        zox_sys_i(MeshGPULink, gpu_mesh);
        zox_sys_i(UvsGPULink, gpu_uvs);
        zox_sys_i(ColorsGPULink, gpu_colors);
        zox_sys_i(MeshIndicies, indicies);
        zox_sys_i(MeshVertices, verts);
        zox_sys_i(MeshUVs, uvs);
        zox_sys_i(MeshColorRGBs, colors);
        zox_sys_o(MeshIndiciesGpu, count);

        if (mesh_dirty->value != mesh_state_upload) {
            continue;
        }

        if (!gpu_mesh->value.x || !gpu_mesh->value.y || !gpu_uvs->value || !gpu_colors->value) {
            count->value = 0;
            continue;
        }

        if (!indicies->length || !verts->length || !uvs->length) {
            count->value = 0;
            continue;
        }

        if (verts->length != uvs->length) {
            zox_sys_world();
            zox_sys_e();
            zox_log_error("[%s] mesh verts [%i] / uvs [%i] missmatch", zox_get_name(e), verts->length, uvs->length)
            continue;
        }

        zox_gpu_element_buffer(
            gpu_mesh->value.x,
            indicies->length,
            sizeof(int),
            indicies->value
        );

        zox_gpu_array_buffer(
            gpu_mesh->value.y,
            verts->length,
            sizeof(float3),
            verts->value
        );

        zox_gpu_array_buffer(
            gpu_uvs->value,
            verts->length,
            sizeof(float2),
            uvs->value
        );

        zox_gpu_array_buffer(
            gpu_colors->value,
            verts->length,
            sizeof(color_rgb),
            colors->value
        );

        count->value = indicies->length;

        // zox_sys_world();
        // zox_sys_e();
        // zox_log("uploaded chunk mesh [%s]", zox_get_name(e));
    }
} zox_sys_end(Mesh3DTexturedUploadSystem);