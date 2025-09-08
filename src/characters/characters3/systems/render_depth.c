void CharacterRenderDepthSystem(iter *it) {
    zox_sys_begin()
    zox_sys_in(RenderDepthDirty)
    // zox_sys_in(RenderDepth)
    zox_sys_out(ChunkMeshDirty)
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDepthDirty, renderDepthDirty)
        // zox_sys_i(RenderDepth, renderDepth)
        zox_sys_o(ChunkMeshDirty, chunkMeshDirty)
        if (renderDepthDirty->value == zox_dirty_active) {
            // zox_log_error("character is dirty")
            chunkMeshDirty->value = zox_dirty_trigger;
            // todo: set mesh instance link by renderDepth?
            // if instance link
        }
    }
} zoxd_system(CharacterRenderDepthSystem)