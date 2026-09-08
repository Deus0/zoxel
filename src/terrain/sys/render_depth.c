// NOTE: When Depth Increases -> Generate our Chunks
// We can just do this with TunkLod system??
zox_sys2(ChunkLodSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepth);
    zox_sys_out(ChunkLodDirty);
    zox_sys_out(NodeDepth);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepth, render_depth);
        zox_sys_o(ChunkLodDirty, render_depth_dirty);
        zox_sys_o(NodeDepth, octree_depth);
        // Delays our lod changes until generation finishes
        if (render_depth_dirty->value == zox_chunk_lod_dirty_generating) {
            if (!zox_has(e, GenerateChunk) &&
                !zox_has(e, VoxelNodeDirty))
            {
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
                zox_log("Chunk [%s] Depth Increased -> %i",
                    zox_get_name(e),
                    render_depth->value);
            }
            // Set to Generate as Depth Increased
            if (!zox_has(e, Loaded)) {
                zox_setv(e, GenerateChunk, zox_generate_terrain_start);
            } else {
                // Skips Generation
                zox_setv(e, GenerateChunk, zox_generate_terrain_sunlight);
            }
        }
        render_depth_dirty->value = zox_chunk_lod_dirty_generating;
        if (dbg_log) {
            zox_log("Chunk Lod [%s] -> dirty_generating",
                zox_getn(e));
        }
    }
} zox_sys_end(ChunkLodSystem);
