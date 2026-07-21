// NOTE: When Depth Increases -> Generate our Chunks
zox_sys2(ChunkLodSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Loaded);
    zox_sys_in(RenderDepth);
    zox_sys_out(ChunkLodDirty);
    zox_sys_out(NodeDepth);
    zox_sys_out(GenerateChunk);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Loaded, loaded);
        zox_sys_i(RenderDepth, render_depth);
        zox_sys_o(ChunkLodDirty, render_depth_dirty);
        zox_sys_o(NodeDepth, octree_depth);
        zox_sys_o(GenerateChunk, generate);
        // Delays our lod changes until generation finishes
        if (render_depth_dirty->value == zox_chunk_lod_dirty_generating) {
            if (!generate->value && !zox_getv(e, VoxelNodeDirty)) {
                render_depth_dirty->value = zox_chunk_lod_dirty_spawn;
            }
            continue;
        }
        if (render_depth_dirty->value != zox_chunk_lod_dirty_generate) {
            continue;
        }
        // NOTE: This just updates the mesh
        // TODO: This is where we should set the chunk render LOD mesh
        if (render_depth->value > octree_depth->value) {
            // Set New Depth
            octree_depth->value = render_depth->value;
            if (dbg_log) {
                zox_log("Chunk [%s] Depth Increased -> %i", zox_get_name(e), render_depth->value);
            }
            // Set to Generate as Depth Increased
            if (!loaded->value) {
                generate->value = zox_generate_terrain_start;
            } else {
                // Skips Generation
                generate->value = zox_generate_terrain_sunlight;
            }
        } else {
            if (dbg_log) {
                zox_log("Chunk [%s] Depth Decreased -> %i", zox_get_name(e), render_depth->value);
            }
            // NOTE: Rebuilds Mesh Whenever Render Depth is Dirty, unless generating
            // zox_set(e, VoxelNodeDirty, { zox_dirty_trigger });
            // zox_set(e, BuildChunkMesh, { zox_dirty_trigger });
            // NOTE: This tells to build the new side, we can do this during LODDirty step instead
        }
        render_depth_dirty->value = zox_chunk_lod_dirty_generating;
    }
} zox_sys_end(ChunkLodSystem);
