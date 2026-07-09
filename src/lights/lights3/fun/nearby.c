// NOTE: Grabs neighbors and corners
void fetch_nearby_chunks(ecs* world, entity e, const entity* neighbors, entity* chunks) {
    memset(chunks, 0, sizeof(entity*) * 27);
    if (!zox_valid(e)) {
        return;
    }
    chunks[OCTREE_INDEX3(sbyte3_zero)] = e;
    for (byte i = 0; i < 6; i++) {
        entity adjacent = neighbors[i];
        if (!zox_valid(adjacent)) {
            continue;
        }
        sbyte3 adjacent_position = neighbor_offsets[i];
        byte adjacent_index = OCTREE_INDEX3(adjacent_position);
        chunks[adjacent_index] = adjacent;
        // fetch the corners
        const entity* adjacent_neighbors = zox_getv(adjacent, ChunkNeighbors);
        for (byte j = 0; j < 6; j++) {
            entity neighbor = adjacent_neighbors[j];
            if (!zox_valid(neighbor)) {
                continue;
            }
            sbyte3 neighbor_position = sbyte3_add(adjacent_position, neighbor_offsets[j]);
            // NOTE: Keeps chunks inside the bounds
            if (!nearby_position_valid(neighbor_position)) {
                continue;
            }
            // NOTE: Avoid doubling up getters
            byte neighbor_index = OCTREE_INDEX3(neighbor_position);
            if (chunks[neighbor_index]) {
                continue;
            }
            chunks[neighbor_index] = neighbor;
        }
    }
}


void fetch_nearby_lights(ecs* world, const LightNode* root, const entity* chunks, const LightNode** lights) {
    // memset(lights, 0, sizeof(const LightNode*) * 27);
    byte root_index = OCTREE_INDEX3(sbyte3_zero);
    lights[root_index] = root;
    for (byte i = 0; i < 27; i++) {
        if (i == root_index) {
            continue;
        }
        entity e = chunks[i];
        if (zox_valid(e)) {
            lights[i] = zox_get(e, LightNode);
        } else {
            lights[i] = NULL;
        }
    }
}

// NOTE: Grabs neighbors and corners
void fetch_neightbor_lights(ecs* world, const LightNode* root, const entity* neighbors, const LightNode** lights) {
    lights[OCTREE_INDEX(0, 0, 0)] = root;
    for (byte i = 0; i < 6; i++) {
        entity adjacent = neighbors[i];
        if (!zox_valid(adjacent)) {
            continue;
        }
        const LightNode* nlights = zox_valid(adjacent) && zox_has(adjacent, LightNode) ? zox_gett(adjacent, LightNode) : NULL;
        sbyte3 position1 = neighbor_offsets[i];
        byte index = OCTREE_INDEX3(position1);
        lights[index] = nlights;
        // fetch the corners
        const entity* more_neighbors = zox_getv(adjacent, ChunkNeighbors);
        for (byte j = 0; j < 6; j++) {
            entity neighbor = more_neighbors[j];
            if (!zox_valid(neighbor)) {
                continue;
            }
            sbyte3 position2 = sbyte3_add(position1, neighbor_offsets[j]);
            // NOTE: Keeps chunks inside the bounds
            if (position2.x < -1 || position2.x > 1 ||
                position2.y < -1 || position2.y > 1 ||
                position2.z < -1 || position2.z > 1) {
                continue;
            }
            byte index2 = OCTREE_INDEX3(position2);
            // NOTE: Avoid doubling up getters
            if (lights[index2]) {
                continue;
            }
            const LightNode* nlights =
                zox_valid(neighbor) && zox_has(neighbor, LightNode)
                ? zox_gett(neighbor, LightNode)
                : NULL;
            lights[index2] = nlights;
        }
    }
}
