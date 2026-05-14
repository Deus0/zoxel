zox_sys2(CharacterRenderDepthSystem) {
    zox_sys_begin();
    zox_sys_in(RenderDepthDirty);
    zox_sys_out(ChunkMeshDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDepthDirty, dirty);
        zox_sys_o(ChunkMeshDirty, mdirty);
        if (dirty->value == zox_dirty_active) {
            mdirty->value = zox_dirty_trigger;
            // todo: set mesh instance link by renderDepth?
            // if instance link
        }
    }
} zox_sys_end(CharacterRenderDepthSystem);
