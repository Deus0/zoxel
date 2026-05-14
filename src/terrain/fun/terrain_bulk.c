/*#ifdef zox_bulk_spawn_terrain

void set_chunk_neighbors_six_directions(ecs *world, entity e, entity chunk_left,entity chunk_right, entity chunk_down, entity chunk_up, entity chunk_back, entity chunk_front) {
    zox_muter(e, ChunkNeighbors, chunkNeighbors);
    chunkNeighbors->value[0] = chunk_left;
    chunkNeighbors->value[1] = chunk_right;
    chunkNeighbors->value[2] = chunk_down;
    chunkNeighbors->value[3] = chunk_up;
    chunkNeighbors->value[4] = chunk_back;
    chunkNeighbors->value[5] = chunk_front;
}

entity create_terrain_bulk(ecs *world, const entity prefab, const int3 center_position) {
    int chunks_total_length = calculate_terrain_chunks_count(terrain_spawn_distance, terrain_vertical);
    entity terrain_world = spawn_terrain(world, prefab_terrain, tilemap, float3_zero, 1);
    entity chunks[chunks_total_length];
    int3 chunk_positions[chunks_total_length];
    for (int i = -terrain_spawn_distance; i <= terrain_spawn_distance; i++) {
        for (int k = -terrain_spawn_distance; k <= terrain_spawn_distance; k++) {
            for (int j = -terrain_vertical; j <= terrain_vertical; j++) {
                int3 chunk_position = (int3) { i, j, k };
                chunk_positions[index] = chunk_position;
            }
        }
    }
    ChunkPosition *chunkPositions = malloc(sizeof(ChunkPosition) * chunks_total_length);
    Position3D *position3Ds = malloc(sizeof(Position3D) * chunks_total_length);
    RenderDepth *renderDepths = malloc(sizeof(RenderDepth) * chunks_total_length);
    VoxLink *voxLinks = malloc(sizeof(VoxLink) * chunks_total_length);
    MeshGPULink *meshGPULinks = malloc(sizeof(MeshGPULink) * chunks_total_length);
    UvsGPULink *uvsGPULinks = malloc(sizeof(UvsGPULink) * chunks_total_length);
    ColorsGPULink *colorsGPULinks = malloc(sizeof(ColorsGPULink) * chunks_total_length);
    for (int i = 0; i < chunks_total_length; i++) {
        int3 chunk_position = chunk_positions[i];
        chunkPositions[i].value = chunk_position;
        position3Ds[i].value = float3_scale(float3_from_int3(chunk_position), real_chunk_scale);
        renderDepths[i].value = get_camera_chunk_distance_xz(int3_zero, chunk_position);
        voxLinks[i].value = terrain_world;

        meshGPULinks[i].value = spawn_gpu_mesh_buffers();
        uvsGPULinks[i].value = zox_gpu_create_buffer();
        colorsGPULinks[i].value = zox_gpu_create_buffer();
    }
    // const entity *particles2DArray =
    entity *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t) {
        .count = chunks_total_length,
        .ids = {
            ecs_pair(EcsIsA, prefab),
            ecs_id(ChunkPosition),
            ecs_id(Position3D),
            ecs_id(RenderDepth),
            ecs_id(VoxLink),
            ecs_id(MeshGPULink),
            ecs_id(UvsGPULink),
            ecs_id(ColorsGPULink)
        },
        // provide data for each id
        .data = (void*[]) {
            NULL,           // Prefab pair, what is it used for?
            chunkPositions,
            position3Ds,
            renderDepths,
            voxLinks,
            meshGPULinks,
            uvsGPULinks,
            colorsGPULinks
        }
    });
    for (int i = 0; i < chunks_total_length; i++) chunks[i] = entities[i];
    free(chunkPositions);
    free(position3Ds);
    free(renderDepths);
    free(voxLinks);
    free(meshGPULinks);
    free(uvsGPULinks);
    free(colorsGPULinks);
    // now for all of them, set their neighbors
    for (int i = -terrain_spawn_distance; i <= terrain_spawn_distance; i++) {
        for (int k = -terrain_spawn_distance; k <= terrain_spawn_distance; k++) {
            for (int j = -terrain_vertical; j <= terrain_vertical; j++) {
                set_chunk_neighbors_six_directions(world,
                    chunks[get_chunk_index_2(i, j, k, terrain_spawn_distance, terrain_vertical)],
                    i == -terrain_spawn_distance ? 0 : chunks[get_chunk_index_2(i - 1, j, k, terrain_spawn_distance, terrain_vertical)],
                    i == terrain_spawn_distance ? 0 : chunks[get_chunk_index_2(i + 1, j, k, terrain_spawn_distance, terrain_vertical)],
                    j == -terrain_vertical ? 0 : chunks[get_chunk_index_2(i, j - 1, k, terrain_spawn_distance, terrain_vertical)],
                    j == terrain_vertical ? 0 : chunks[get_chunk_index_2(i, j + 1, k, terrain_spawn_distance, terrain_vertical)],
                    k == -terrain_spawn_distance ? 0 : chunks[get_chunk_index_2(i, j, k - 1, terrain_spawn_distance, terrain_vertical)],
                    k == terrain_spawn_distance ? 0 : chunks[get_chunk_index_2(i, j, k + 1, terrain_spawn_distance, terrain_vertical)]);
            }
        }
    }

    ChunkLinks *chunkLinks = zox_get_mut(terrain_world, ChunkLinks)
    chunkLinks->value = create_int3_hashmap(chunks_total_length);

    for (int i = 0; i < chunks_total_length; i++) {
        int3_hashmap_add(chunkLinks->value, chunk_positions[i], chunks[i]);
    }

    zox_modified(terrain_world, ChunkLinks);

    return terrain_world;
}
#endif*/
