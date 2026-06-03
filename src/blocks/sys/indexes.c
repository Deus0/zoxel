// NOTE: Sets all Realm Block Indexes after dirty
zox_sys2(RealmBlocksDirtySystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BlocksDirty);
    zox_sys_in(BlockLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(BlocksDirty, state);
        zox_sys_i(BlockLinks, blocks);
        if (state->value != zox_dirty_active) {
            continue;
        }
        if (dbg_log) {
            zox_log("Realm [%s] Blocks Dirty [%i]", zox_get_name(e), blocks->length);
        }
        for (int j = 0; j < blocks->length; j++) {
            entity block = blocks->value[j];
            zox_set(block, BlockIndex, { j + 1 });
            if (dbg_log) {
                zox_log(" - Realm Block [%s]:[%i]", zox_get_name(block), j + 1);
            }
        }
    }
} zox_sys_end(RealmBlocksDirtySystem);
