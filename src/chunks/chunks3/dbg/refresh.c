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
                zox_add(e2, VoxelNodeDirty);
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
                // float4x4 matrix = zox_getv(e2, TransformMatrix);
                // zox_set(e2, VoxelNodeDirty, { 1 });
                // zox_set(e2, BuildChunkSides, { 1 });
                entity meshes[8];
                uint meshes_length = zox_get_children_by_id(world, e2, meshes, 8, zox_id(ChunkMesh));
                for (int k = 0; k < meshes_length; k++) {
                    entity e3 = meshes[k];
                    zox_add(e3, BuildMeshColors);
                    // zox_set(e3, BuildMesh, { 1 });
                    // zox_set(e3, TransformMatrix, { matrix });
                    // zox_set(e2, TexturedMeshDirty, { 1 });
                    // zox_set(e2, MeshColorsDirty, { zox_dirty_trigger });
                }
            }
        }
    }
}
