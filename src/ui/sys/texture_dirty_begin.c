// NOTE: IDK WHAT THIS DOES WHY
zox_sys2(TextureDirtyBeginSystem) {
    zox_sys_begin();
    zox_sys_in(InitializeEntity);
    zox_sys_out(TextureDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(InitializeEntity, initialize);
        zox_sys_o(TextureDirty, dirty);
        if (initialize->value != zox_dirty_active) {
            continue;
        }
        //if (!zox_has(it->entities[i], GenerateTexture))
        if (dirty->value) {
            dirty->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(TextureDirtyBeginSystem);
