
void TextureDirtyBeginSystem(iter *it) {
    if (headless) return;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(InitializeElement);
    zox_sys_out(TextureDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(InitializeElement, initialize);
        zox_sys_o(TextureDirty, dirty);
        if (initialize->value != zox_dirty_active) {
            continue;
        }
        //if (!zox_has(it->entities[i], GenerateTexture))
        if (dirty->value) dirty->value = zox_dirty_trigger;
    }
} zoxd_system2(TextureDirtyBeginSystem);