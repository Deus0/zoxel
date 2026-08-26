zox_sys2(BonesInitializeSystem) {
    byte dbg_log = dbg_initialize_mesh || 0;
    zox_sys_world();
    zox_sys_begin();
    // zox_sys_in(Initialize);
    zox_sys_out(BoneIndexGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        // zox_sys_i(Initialize, initialize);
        zox_sys_o(BoneIndexGPULink, gpu);
        /*if (initialize->value != zox_dirty_active) {
            continue;
        }*/
        gpu->value = zox_gpu_create_buffer();
        if (is_paint_skeletons) {
            zox_add(e, PaintedSkeleton);
        }
        if (dbg_log) {
            zox_log("Mesh Initialized on GPU [%s]", zox_getn(e));
        }
    }
} zox_sys_end(BonesInitializeSystem);
