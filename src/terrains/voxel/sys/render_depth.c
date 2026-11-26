// NOTE: When Depth Increases -> Generate our Chunks
// We can just do this with TunkLod system??
void chunk_lod_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepth);
    zox_sys_out(ChunkLodDirty);
    zox_sys_out(NodeDepth);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepth, render_depth);
        zox_sys_o(ChunkLodDirty, dirty);
        zox_sys_o(NodeDepth, octree_depth);
        // Delays our lod changes until generation finishes
        if (dirty->value == zox_chunk_lod_dirty_generating) {
            if (!zox_has(e, GenerateChunk) &&
                !zox_has(e, VoxelNodeDirty) &&
                !zox_has(e, VoxelNodePostDirty) &&
                !zox_has(e, ChunkUpdate))
            {
                dirty->value = zox_chunk_lod_dirty_spawn;
            }
            continue;
        }
        if (dirty->value != zox_chunk_lod_dirty_generate) {
            continue;
        }
        // Coarser meshes reuse the voxels already generated at the finer depth.
        if (render_depth->value > octree_depth->value) {
            // Keep the old voxels until adjacent CPU meshes have uploaded their open borders.
            // GPU grids wait on generation for lighting, so they cannot be a prerequisite here.
            if (!zox_chunks3_gpu_meshing && chunk_neighbors_rebuilding(world, e)) {
                continue;
            }
            chunk_meshes_stop_building(world, e);
            if (!zox_no_lights) {
                zox_muter(e, LightOctree, lights);
                zox_muter(e, LightLock, lock);
                spin_lock(&lock->value);
                lights->value = darklight;
                collapse_LightOctree(lights);
                spin_unlock(&lock->value);
                if (zox_has(e, SunnyChunk)) {
                    zox_add(zox_get_parent(world, e), GenerateSunlight);
                }
            }
            octree_depth->value = render_depth->value;
            zox_setv(e, GenerateChunk, zox_has(e, Loaded) ? zox_generate_terrain_sunlight : zox_generate_terrain_start);
        }
        dirty->value = zox_chunk_lod_dirty_generating;
        if (dbg_log) {
            zox_log("Chunk Lod [%s] -> dirty_generating",
                zox_getn(e));
        }
    }
    zox_sys_on_end();
} zoxd_system(chunk_lod_system);
