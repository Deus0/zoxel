// uploads bone indexes to shader
zox_sys2(BoneIndexUploadSystem) {
    zox_sys_begin();
    zox_sys_in(MeshDirty);
    zox_sys_in(BoneIndexes);
    zox_sys_out(BoneIndexGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(MeshDirty, mdirty);
        zox_sys_i(BoneIndexes, boneIndexes);
        zox_sys_o(BoneIndexGPULink, boneIndexGPULink);

        if (mdirty->value != mesh_state_upload) {
            continue;
        }

        if (boneIndexes->length == 0) {
            // zox_log(" ! boneIndexes 0\n")
            if (boneIndexGPULink->value != 0) {
                clear_regular_buffer(&boneIndexGPULink->value);
            }
            continue;
        }

        if (boneIndexGPULink->value == 0) {
            boneIndexGPULink->value = spawn_gpu_generic_buffer();
        }

        zox_gpu_bind_buffer_array(boneIndexGPULink->value);
        zox_gpu_set_buffer_array(boneIndexes->value, boneIndexes->length * sizeof(byte));
        zox_gpu_bind_buffer_array(0);
    }
} zox_sys_end(BoneIndexUploadSystem);
