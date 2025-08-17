// todo: place roof
// todo: place walls
// todo: place in neighbor chunks too
// todo: every 5 seconds, build a new block along connects dungeon bricks (dark blocks)

typedef struct {
    byte3 position;
    ecs_entity_t chunk;
} TerrainPlacePosition;

void DungeonBlockSystem(ecs_iter_t *it) {

    zox_sys_world()
    zox_sys_begin()
    zox_sys_in(TimerState)
    zox_sys_in(ChunkLink)
    zox_sys_in(DungeonWallType)

    for (int i = 0; i < it->count; i++) {

        zox_sys_e()
        zox_sys_i(ChunkLink, chunkLink)
        zox_sys_i(TimerState, timerState)
        zox_sys_i(DungeonWallType, place)

        if (timerState->value == 0) {
            continue;
        }

        ecs_entity_t chunk = chunkLink->value;
        zox_geter_value(chunk, VoxLink, entity, terrain);
        zox_geter(terrain, ChunkLinks, chunks);
        zox_geter_value(terrain, NodeDepth, byte, terrain_depth);
        byte3 terrain_sizec = byte3_single(powers_of_two[terrain_depth]);
        // int3 chunk_bounds = int3_single(powers_of_two[node_depth]);
        zox_geter_value(e, VoxelLocalPosition, byte3, positionl);
        // get closest grass block
        // byte3 position = (byte3) { rand() % size.x, rand() % size.y, rand() % size.z };
        byte place_type = 0;
        if (rand() % 100 >= 2) {
            place_type = place->value; // zox_block_dark;
        }
        // find next z position
        const byte radius = 3;
        const byte height = 5;

        // chunk stuff
        // zox_geter_value(chunk, NodeDepth, byte, node_depth);
        zox_geter_value(chunk, ChunkSize, int3, chunk_size);
        zox_geter_value(chunk, ChunkPosition, int3, chunk_position);

        int3 positionv = positionl_to_positionv(
            positionl,
            chunk_position,
            chunk_size,
            terrain_sizec
        );

        entity place_chunk;
        int3 place_positionv;
        byte3 place_positionl;
        int3 place_positionc;
        // VoxelNode* place_node;

        // placing
        byte find = 0;
        int3 check_positionv;
        for (int y = 0; y <= height && !find; y++) {
            check_positionv.y = positionv.y + y;
            for (int x = - radius; x <= radius && !find; x++) {
                check_positionv.x = positionv.x + x;
                for (int z = - radius; z <= radius && !find; z++) {
                    check_positionv.z = positionv.z + z;
                    // dodge dungeon core
                    if (x == 0 && y ==  0 && z == 0) {
                        continue;
                    }
                    // skip middle blocks
                    if (int_abs(x) != radius && int_abs(z) != radius && (y != 0 && y != height)) {
                        continue;
                    }
                    // door
                    if (x == 0 && z == radius && y != 0 && y != height) {
                        continue;
                    }

                    // now we have positionv:
                    //  get chunk positionc
                    int3 check_positionc = positionv_to_positionc(
                        check_positionv,
                        terrain_sizec
                    );
                    entity check_chunk = int3_hashmap_get(chunks->value, check_positionc);
                    if (!check_chunk) {
                        return;
                    }
                    zox_geter_value(check_chunk, ChunkSize, int3, check_sizec);
                    zox_geter_value(check_chunk, NodeDepth, byte, check_node_depth);

                    byte3 check_positionl = positionv_to_positionl(
                        check_positionv,
                        check_positionc,
                        check_sizec,
                        terrain_sizec
                    );
                    if (!byte3_in_bounds(check_positionl, int3_to_byte3(check_sizec))) {
                        continue;
                    }

                    zox_geter(check_chunk, VoxelNode, check_node);
                    const VoxelNode* check_subnode = get_node(
                        (VoxelNode*) check_node,
                        check_node_depth,
                        check_positionl
                    );
                    if (!check_subnode || check_subnode->value == place_type) {
                        continue;
                    }
                    // can only place in air
                    if (place_type && check_subnode->value) {
                        place_type = 0;
                        // continue;
                    }
                    place_positionv = check_positionv;
                    place_positionl = check_positionl;
                    place_positionc = check_positionc;
                    place_chunk = check_chunk;
                    find = 1;
                }
            }
        }

        if (!find) {
            continue;
        }

        if (!place_chunk || !zox_has(place_chunk, VoxelNodeQueue)) {
            zox_log_error("Cannot place: Invalid VoxelNodeQueue [%ix%ix%i]",
                place_positionc.x, place_positionc.y, place_positionc.z);
            continue;
        }

        // add to queue
        zox_muter(place_chunk, VoxelNodeQueue, queue);
        a_VoxelNodeQueue(queue,
            (VoxelNodeUpdate) {
                .value = place_type,
                .positionl = place_positionl,
                .positionv = place_positionv
            });

        /*zox_mut_begin(place_chunk, VoxelNode, place_node); // get node function
        // float3 positionf = positionv_to_real_position(voxel_position, int3_to_byte3(chunk_bounds), default_vox_scale);
        place_block(world,
            place_chunk,
            place_node,
            place_positionl,
            place_positionv,
            place_type
        );
        zox_mut_end(place_chunk, VoxelNode);*/
        // zox_log("+ Dungeon Block Placing: %s [%ix%ix%i]: %i", zox_get_name(e), position.x, position.y, position.z, place_type)
    }
} zoxd_system(DungeonBlockSystem)
