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
                zox_set(e2, ChunkMeshDirty, { zox_dirty_trigger });
            }
        }
    }
}
