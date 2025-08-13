/*void RenderDepthDirtySystem(iter *it) {
    zox_sys_begin()
    zox_sys_in(RenderDepthDirty)
    // zox_sys_in(RenderDepth)
    zox_sys_out(GenerateChunk)
    // zox_sys_out(ChunkMeshDirty)
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDepthDirty, dirty)
        //zox_sys_i(RenderDepth, renderDepth)
        zox_sys_o(GenerateChunk, generateChunk)
        // zox_sys_o(ChunkMeshDirty, chunkMeshDirty)
        if (dirty->value == zox_dirty_active) {
            generateChunk->value = 1;
        }
    }
} zoxd_system2(RenderDepthDirtySystem);*/

/*else if (chunkLodDirty->value == chunk_lod_state_mesh_trigger) {
    if (renderDepth->value != render_depth_invisible) {
        chunkMeshDirty->value = chunk_dirty_state_trigger;
    }
}*/