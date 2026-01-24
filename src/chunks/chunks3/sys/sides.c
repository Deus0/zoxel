// Hmm how do we want to handle this?
zox_sys2(SidesOctreeDirtySystem) {
    zox_sys_begin();
    zox_sys_in(SidesOctreeDirty);
    zox_sys_out(SidesOctree);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(SidesOctreeDirty, state);
        zox_sys_o(SidesOctree, octree);

        if (state->value == zox_dirty_active) {
            // optimize_SidesOctree(octree);
            reduce_SidesOctree(octree);
        }
    }
} zox_sys_end(SidesOctreeDirtySystem);