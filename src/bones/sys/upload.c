// uploads bone indexes to shader
zox_sys2(BoneIndexUploadSystem) {
    zox_sys_begin();
    zox_sys_in(MeshDirty);
    zox_sys_in(BoneIndexes);
    zox_sys_out(BoneIndexGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(MeshDirty, mesh_dirty);
        zox_sys_i(BoneIndexes, weights);
        zox_sys_o(BoneIndexGPULink, gpu);
        if (mesh_dirty->value != mesh_state_upload) {
            continue;
        }
        if (weights->length == 0) {
            zox_gpu_dispose_buffer(gpu->value);
            gpu->value = 0;
            continue;
        }
        if (gpu->value == 0) {
            gpu->value = zox_gpu_create_buffer();
        }
        zox_gpu_bind_buffer_array(gpu->value);
        zox_gpu_set_buffer_array(weights->value, weights->length * sizeof(byte));
        zox_gpu_bind_buffer_array(0);
    }
} zox_sys_end(BoneIndexUploadSystem);
