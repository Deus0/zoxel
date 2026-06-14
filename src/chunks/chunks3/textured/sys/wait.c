extern byte is_chunk_lights_busy(ecs*, entity );

// NOTE: Waits until neighbors are done building before pushing to GPU
byte is_chunk_busy(ecs* world, entity e) {
    if (!zox_valid(e)) {
        return 0;
    }
    return zox_getv(e, Generate) ||
        zox_getv(e, ChunkMeshDirty) ||
        zox_getv(e, VoxelNodeDirty); // ||
        // zox_getv(e, SidesOctreeDirty) ||
        // zox_getv(e, RenderDepthDirty) ||
        // zox_getv(e, RenderDistanceDirty) ||
        // zox_getv(e, MeshColorsGenerate) ||
        // zox_getv(e, MeshColorsDirty) ||
        // is_chunk_lights_busy(world, e);
}

byte is_chunk_neighbors_busy(ecs* world, entity e, byte length) {
    if (!zox_valid(e) || !zox_has(e, ChunkNeighbors) || !length) {
        return 0;
    }
    length--;
    zox_geter(e, ChunkNeighbors, neighbors);
    for (byte j = 0; j < 6; j++) {
        entity neighbor = neighbors->value[j];
        if (is_chunk_busy(world, neighbor)) {
            return 1;
        }
        if (is_chunk_neighbors_busy(world, neighbor, length)) {
            return 1;
        }
    }
    return 0;
}

// NOTE: Just checks entire terrain chunk, makes them all update at same time
zox_sys2(ChunkMeshSlowSystem) {
    byte dbg_log = 0;
    entity terrain = 0;
    byte terrain_updating = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ChunkNeighbors);
    zox_sys_out(MeshDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_o(MeshDirty, mesh_dirty);
        if (!(mesh_dirty->value >= mesh_state_trigger_terrain && mesh_dirty->value <= mesh_state_trigger_slow_end)) {
            continue;
        }
        // Check All Terrain chunks to see if it fixes it
        // for all children, Brute force this biatch
        // cache for now! later move to a system of terrains
        entity new_terrain = zox_get_parent(world, e);
        if (terrain != new_terrain) {
            terrain = new_terrain;
            iter it2 = zox_children(world, terrain);
            while (zox_children_next(it2)) {
                for (int j = 0; j < it2.count && !terrain_updating; j++) {
                    entity e2 = it2.entities[j];
                    if (!zox_valid(e2) || !zox_has(e2, Chunk3)) {
                        continue;
                    }
                    if (is_chunk_busy(world, e2)) {
                        terrain_updating = 1;
                        break;
                    }
                }
            }
        }
        /*for (byte j = 0; j < 6; j++) {
            entity neighbor = neighbors->value[j];
            // check if neighbor updating still
            if (is_chunk_busy(world, neighbor)) {
                still_updating = 1;
                break;
            }
            // check if neighbor updating still
            if (is_chunk_neighbors_busy(world, neighbor, 3)) {
                still_updating = 1;
                break;
            }
        }*/
        if (terrain_updating) {
            if (dbg_log) {
                zox_log("Chunk [%s] still waiting on neighbor", zox_get_name(e));
            }
            mesh_dirty->value = mesh_state_trigger_terrain;
        } else {
            mesh_dirty->value = mesh_state_trigger;
            // NOTE: Sets all neighbors to trigger too!
            /*for (byte j = 0; j < 6; j++) {
                entity neighbor = neighbors->value[j];
                // check if neighbor updating still
                if (!zox_valid(neighbor)) {
                    continue;
                }
                byte neighbor_mesh_dirty = zox_getv(neighbor, MeshDirty);
                // check if neighbor updating still
                if (neighbor_mesh_dirty >= mesh_state_trigger_terrain && neighbor_mesh_dirty <= mesh_state_trigger_slow_end) {
                    zox_set(neighbor, MeshDirty, { mesh_state_trigger });
                }
            }*/
        }
    }
} zox_sys_end(ChunkMeshSlowSystem);