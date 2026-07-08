// Groups same nodes when dirty
zox_sys2(LightNodeReduceSystem) {
    zox_sys_begin();
    zox_sys_in(LightNodeDirty);
    zox_sys_out(LightNode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LightNodeDirty, dirty);
        zox_sys_o(LightNode, lights);
        if (dirty->value == zox_dirty_active) {
            optimize_by_max_LightNode(lights);
            reduce_LightNode(lights);
        }
    }
} zox_sys_end(LightNodeReduceSystem);