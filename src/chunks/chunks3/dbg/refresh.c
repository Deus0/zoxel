void zox_dbg_terrain_refresh(ecs* world, ClickEventData data) {
    if (!zox_valid(local_terrain)) {
        return;
    }
    entity e = local_terrain;
    uint count = zox_get_children_count_by_id(world, e, zox_id(Chunk3));
    // zox_geter(e, ChunkLinks, chunks);
    zox_log("Refreshing Terrain Chunks [%i]", count);
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            if (zox_valid(e2) && zox_has(e2, Chunk3)) {
                // zox_set(e2, VoxelNodeDirty, { zox_dirty_trigger });
                // zox_set(e, RenderDepthDirty, { zox_dirty_trigger });
                // zox_set(e2, BuildChunkMesh, { zox_dirty_trigger });
                zox_set(e2, MeshDirty, { mesh_state_trigger });
            }
        }
    }
}

// NOTE: The issue is the neighbor chunk didn't rebuild lights when it updated
void zox_dbg_terrain_refresh_mesh_colors(ecs* world, ClickEventData data) {
    if (!zox_valid(local_terrain)) {
        return;
    }
    entity e = local_terrain;
    uint count = zox_get_children_count_by_id(world, e, zox_id(Chunk3));
    // zox_geter(e, ChunkLinks, chunks);
    zox_log("Refreshing Terrain Chunks [%i]", count);
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            if (zox_valid(e2) && zox_has(e2, Chunk3)) {
                zox_set(e2, MeshColorsGenerate, { zox_dirty_trigger });
                // zox_set(e2, MeshColorsDirty, { zox_dirty_trigger });
            }
        }
    }
}
