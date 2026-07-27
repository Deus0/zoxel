zox_sys2(MeshUpdateSystem) {
    zox_sys_begin();
    zox_sys_in(MeshIndicies);
    zox_sys_in(MeshVertices);
    zox_sys_in(MeshGPULink);
    zox_sys_out(MeshDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(MeshGPULink, gpu_mesh);
        zox_sys_i(MeshIndicies, indicies);
        zox_sys_i(MeshVertices, verts);
        zox_sys_o(MeshDirty, dirty);
        if (dirty->value != mesh_state_upload) {
            continue;
        }
        zox_gpu_element_buffer(gpu_mesh->value.x, indicies->length, sizeof(int), indicies->value);
        zox_gpu_array_buffer(gpu_mesh->value.y, verts->length, sizeof(float3), verts->value);
        dirty->value = 0;
    }
} zox_sys_end(MeshUpdateSystem);
