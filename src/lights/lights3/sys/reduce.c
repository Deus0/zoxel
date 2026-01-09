// Groups same nodes when dirty
zox_sys2(LightNodeReduceSystem) {
    zox_sys_begin();
    zox_sys_in(LightNodeDirty);
    zox_sys_out(LightNode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LightNodeDirty, dirty);
        zox_sys_o(LightNode, lnode);
        if (dirty->value == zox_dirty_active) {
            optimize_LightNode(lnode);
            reduce_LightNode(lnode);
        }
    }
} zox_sys_end(LightNodeReduceSystem);