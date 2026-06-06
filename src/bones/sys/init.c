zox_sys2(BonesInitializeSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(InitializeEntity);
    zox_sys_out(BoneIndexGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(InitializeEntity, state);
        zox_sys_o(BoneIndexGPULink, gpu);
        if (state->value == zox_dirty_active) {
            gpu->value = zox_gpu_create_buffer();
            if (is_paint_skeletons) {
                zox_sys_e();
                zox_add_tag(e, PaintedSkeleton);
            }
        }
    }
} zox_sys_end(BonesInitializeSystem);
