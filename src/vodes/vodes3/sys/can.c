void can_have_vodes_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ChunkLodDirty);
    zox_sys_in(RenderDepth);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ChunkLodDirty, dirty);
        zox_sys_i(RenderDepth, render_depth);
        if (!dirty->value) {
            continue;
        }
        // either voxel voxel_octree is dirty, or we are spawning for first time based on distance changes
        entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain)) {
            continue;
        }
        byte max_depth = zox_getv(terrain, NodeDepth);
        byte can_spawn_vodes = render_depth->value == max_depth;
        byte old_can = zox_has(e, CanVodes);
        if (!can_spawn_vodes) {
            if (old_can) {
                zox_remove(e, CanVodes);
                if (dbg_log) {
                    zox_log("[%s] Can Not Vodes", zox_sys_e_name);
                }
            }
        } else {
            if (!old_can) {
                zox_add(e, CanVodes);
                if (dbg_log) {
                    zox_log("[%s] CanVodes", zox_sys_e_name);
                }
            }
        }
    }
} zoxd_system(can_have_vodes_system);