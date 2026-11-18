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
    // the materials relinked (gpu_restore_rendering): their locations again, the instance blocks bound again
    zox_gpu_relocate(MaterialBasic3D);
    zox_gpu_relocate(MaterialColored3D);
    zox_gpu_relocate(MaterialTextured3D);
    zox_gpu_relocate(MaterialBone);
#ifndef zox_disable_rendering_instances
    zox_gpu_relocate(MaterialVoxInstance);
    zox_gpu_relocate(MaterialVoxInstanceSway);
    if (zox_valid(material_vox_instance)) bind_instance_matrices(zox_getv(material_vox_instance, MaterialGPULink));
    if (zox_valid(material_vox_instance_sway)) bind_instance_matrices(zox_getv(material_vox_instance_sway, MaterialGPULink));
    zox_each_begin(UboGPULink);
        zox_each_o(UboGPULink, ubo);
        ubo->value = generate_ubo(zox_instance_matrices_binding);
    zox_each_end();
#endif
}
