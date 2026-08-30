// uploads bone indexes to shader
zox_sys2(BoneIndexUploadSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BoneIndexes);
    zox_sys_out(BoneIndexGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(BoneIndexes, weights);
        zox_sys_o(BoneIndexGPULink, gpu);
        if (!gpu->value) {
            if (dbg_log) {
                zox_loge("[%s]'s Bones GPU Link is 0",
                    zox_sys_e_name);
            }
            continue;
        }
        zox_gpu_bind_buffer_array(gpu->value);
        zox_gpu_set_buffer_array(weights->value, weights->length * sizeof(byte));
        zox_gpu_bind_buffer_array(0);
        zox_remove(e, SkeletonMeshDirty);
        zox_add(e, MeshDirty);
        if (dbg_log) {
            zox_log("[%s]'s Bone Weights Uploaded",
                zox_sys_e_name);
        }
    }
} zox_sys_end(BoneIndexUploadSystem);
