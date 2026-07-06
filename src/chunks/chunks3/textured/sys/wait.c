
// TODO: Maybe a new state just for MeshReady

extern byte is_chunk_lights_busy(ecs*, entity);
extern byte is_chunk_generating_sunlights(ecs*, entity);
//TODO: Busy State for Chunks

// NOTE: Waits until neighbors are done building before pushing to GPU
byte is_chunk_busy(ecs* world, entity e) {
    if (!zox_valid(e)) {
        return 0;
    }
    return zox_getv(e, Busy) || is_chunk_lights_busy(world, e);
        // || zox_getv(e, Generate)
        // || zox_getv(e, ChunkMeshDirty)
        // || zox_getv(e, VoxelNodeDirty)
        // || zox_getv(e, RenderDepthDirty)
        // || zox_getv(e, SidesOctreeDirty)
        // || zox_getv(e, RenderDistanceDirty)
        // || zox_getv(e, MeshColorsGenerate)
        // || zox_getv(e, MeshColorsDirty)
        // || is_chunk_generating_sunlights(world, e);
}

// NOTE: Queue method was slower than just using entire chunks
// NOTE: Need to check neighbor is updating, if they are
// NOTE: Just checks entire terrain chunk, makes them all update at same time
zox_sys2(ChunkMeshSlowSystem) {
    byte dbg_log = 0;
    byte dbg_skip = 1;
    if (dbg_skip) {
        zox_sys_begin();
        zox_sys_out(MeshReady);
        zox_sys_out(MeshDirty);
        for (int i = 0; i < it->count; i++) {
            zox_sys_o(MeshReady, mesh_ready);
            zox_sys_o(MeshDirty, mesh_dirty);
            if (mesh_ready->value) {
                mesh_dirty->value = mesh_state_trigger;
                mesh_ready->value = 0;
            }
        }
        return;
    }
    // Group by Terrain
    byte still_updating = 0;
    byte vox_count = 0;
    entity_array_d* voxes = create_entity_array_d(1);
    byte_array_d* voxes_busy = create_byte_array_d(1);
    // ecs
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(MeshReady);
    zox_sys_out(MeshDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(MeshReady, mesh_ready);
        zox_sys_o(MeshDirty, mesh_dirty);
        if (!mesh_ready->value) {
            continue;
        }
        // Check All Terrain chunks to see if it fixes it
        // for all children, Brute force this biatch
        // cache for now! later move to a system of terrains
        entity terrain = zox_get_parent(world, e);
        int vox_index = entity_array_d_index(voxes, terrain);
        if (vox_index == -1) {
            still_updating = 0;
            iter it2 = zox_children(world, terrain);
            while (zox_children_next(it2)) {
                for (int j = 0; j < it2.count && !still_updating; j++) {
                    entity e2 = it2.entities[j];
                    if (!zox_valid(e2) || !zox_has(e2, Chunk3)) {
                        continue;
                    }
                    if (is_chunk_busy(world, e2)) {
                        still_updating = 1;
                        break;
                    }
                }
            }
            entity_array_d_add(voxes, terrain);
            byte_array_d_add(voxes_busy, still_updating);
            if (dbg_log) {
                zox_log("Terrain Updating [%s]:%i? Updating [%i]", zox_get_name(e), vox_count, still_updating);
            }
            vox_count++;
        } else {
            // Grab from previous
            still_updating = voxes_busy->data[vox_index];
            if (dbg_log >= 2) {
                zox_log("Terrain Updating [%s]:%i? Updating [%i]", zox_get_name(e), vox_index, still_updating);
            }
        }
        if (!still_updating) {
            mesh_dirty->value = mesh_state_trigger;
            mesh_ready->value = 0;
        } else {
            if (dbg_log >= 2) {
                zox_log("Chunk [%s] still waiting on neighbor", zox_get_name(e));
            }
        }
    }
    dispose_entity_array_d(voxes);
    dispose_byte_array_d(voxes_busy);
} zox_sys_end(ChunkMeshSlowSystem);

// NOTE: I like the idea of Floodfill Busy checks, because npcs might edit parts of the map at once

    // Group by Neighbor Floodfill
    /*uint queue_count = 0;
    uint max_queues = 128;
    uint queue_chunks_count = 0;
    uint max_chunks_queues = 512;
    entity_array_d* queues[max_queues];
    byte queues_busy[max_queues];*/

        // If no neighbor waiting in mesh gen, we can skip the group update
        /*byte neighbors_waiting = 0;
        for (byte j = 0; j < 6; j++) {
            entity neighbor = neighbors->value[j];
            byte neighbor_dirty = zox_getv(e, MeshDirty);
            if (neighbor_dirty >= mesh_state_trigger_terrain && neighbor_dirty <= mesh_state_trigger_slow_end) {
                neighbors_waiting = 1;
                break;
            }
        }
        if (!neighbors_waiting) {
            still_updating = 0;
            // check if neighbor updating still
            for (byte j = 0; j < 6; j++) {
                entity neighbor = neighbors->value[j];
                if (is_chunk_busy(world, neighbor)) {
                    still_updating = 1;
                    break;
                }
            }
            mesh_dirty->value = still_updating ? mesh_state_trigger_terrain : mesh_state_trigger;
        }*/
        // Queue Method
        /*still_updating = 0;
        entity_array_d* my_queue = NULL;
        // FIrst check if in a previous queue
        for (uint j = 0; j < queue_count; j++) {
            entity_array_d* queue = queues[j];
            if (entity_array_d_has(queue, e)) {
                my_queue = queue;
                still_updating = queues_busy[j];
                break;
            }
        }
        // Create and Add to Systems cache list
        if (my_queue == NULL) {
            if (queue_count >= max_queues || queue_chunks_count >= max_chunks_queues) {
                // NOTE: We can only use so much in an array
                if (dbg_log) {
                    zox_logw("Max Queues Reached [%i] of [%i] - Individual Queues [%i] of [%i]", i, it->count, queue_chunks_count, max_chunks_queues);
                }
                continue;
            } else {
                my_queue = create_entity_array_d(initial_dynamic_array_size);
                queues[queue_count] = my_queue;
                get_waiting_neighbors(world, my_queue, e);
                byte queue_busy = 0;
                for (size_t j = 0; j < my_queue->size && !queue_busy; j++) {
                    entity neighbor = my_queue->data[j];
                    if (is_chunk_busy(world, neighbor)) {
                        queue_busy = 1;
                    }
                }
                for (size_t j = 0; j < my_queue->size && !queue_busy; j++) {
                    entity neighbor = my_queue->data[j];
                    if (is_chunk_neighbors_busy(world, neighbor, 3)) {
                        still_updating = 1;
                    }
                }
                queues_busy[queue_count] = queue_busy;
                still_updating = queue_busy;
                if (dbg_log) {
                    zox_log("New Wait Queue on Chunks is [%i of %i] Long: Busy [%i]", queue_count, my_queue->size, queue_busy);
                }
                queue_chunks_count += my_queue->size;
                queue_count++;
            }
        }*/
        // Add to System cache list
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
        // mesh_dirty->value = still_updating ? mesh_state_trigger_terrain : mesh_state_trigger;

    /*for (uint j = 0; j < queue_count; j++) {
        dispose_entity_array_d(queues[j]);
    }*/

/*void get_waiting_neighbors(ecs* world, entity_array_d* chunks, entity e) {
    if (!zox_valid(e)) {
        return;
    }
    if (entity_array_d_has(chunks, e)) {
        return;
    }
    byte mesh_dirty = zox_getv(e, MeshDirty);
    byte is_mesh_waiting = mesh_dirty >= mesh_state_trigger_terrain && mesh_dirty <= mesh_state_trigger_slow_end;
    if (is_mesh_waiting) {
        // we check more neighbors
        entity_array_d_add(chunks, e);
        zox_geter(e, ChunkNeighbors, neighbors);
        for (byte j = 0; j < 6; j++) {
            entity neighbor = neighbors->value[j];
            get_waiting_neighbors(world, chunks, neighbor);
        }
    }
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
}*/
