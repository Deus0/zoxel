entity_array_d* disposed_bone_indexes = NULL;

// skeleton meshes' bone index buffers and the vox instance material's matrices ubo, let go of with the gpu
void gpu_dispose_rendering3(ecs* world) {
    zox_each_begin(BoneIndexGPULink);
        zox_each_o(BoneIndexGPULink, bones);
        if (!bones->value) continue;
        zox_gpu_dispose_buffer(bones->value);
        bones->value = 0;
        zox_disposed_add(&disposed_bone_indexes, e);
    zox_each_end();
    zox_each_begin(UboGPULink);
        zox_each_o(UboGPULink, ubo);
        zox_gpu_dispose_buffer(ubo->value);
        ubo->value = 0;
    zox_each_end();
}

// made again, the bone indexes upload again (SkeletonMeshDirty)
void gpu_restore_rendering3(ecs* world) {
    zox_disposed_begin(disposed_bone_indexes, BoneIndexGPULink, bones);
        bones->value = zox_gpu_create_buffer();
        zox_add(e, SkeletonMeshDirty);
    zox_disposed_end(disposed_bone_indexes);
#ifndef zox_disable_rendering_instances
    zox_each_begin(UboGPULink);
        const MaterialVoxInstance* data = zox_get(e, MaterialVoxInstance);
        if (!data) continue;
        zox_each_o(UboGPULink, ubo);
        ubo->value = generate_ubo(data->matrices);
    zox_each_end();
#endif
}
