// tst: block_data_queue, a voxel's block data goes in with it and goes with it (chunks3/fun/block_data.c)
//  - queues a block with data 6 into air in the player's chunk: the chunk's BlockDatas has it
//  - saves the chunk as chunk3_save_system does, load_chunk reads it back with its data
//  - queues air there: its data is gone
//  - run: bash bsh/tst.sh block_data_queue
byte zox_tst_bdq_stage = 0;
int zox_tst_bdq_frames = 0;
entity zox_tst_bdq_chunk = 0;
byte3 zox_tst_bdq_at;
byte zox_tst_bdq_set = 0, zox_tst_bdq_loaded = 0, zox_tst_bdq_cleared = 1;

byte zox_tst_block_data_queue_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const entity realm = zox_get_link(world, character, RealmLink);
    zox_tst_bdq_frames++;
    zox_tst_progress(zox_tst_bdq_stage * 1000 + zox_tst_bdq_frames);
    if (zox_tst_bdq_stage == 0) {
        zox_tst_waiting = "air in the player's chunk";
        const byte depth = zox_getv(terrain, NodeDepth);
        const int length = octree_size(depth);
        const int3 chunk_position = real_position_to_chunk_position(zox_getv(character, Position3D), length, zox_getv(terrain, BlockScale));
        const entity chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, chunk_position);
        if (!zox_valid(chunk) || !zox_has(chunk, BlockDatas)) {
            return 0;
        }
        for (int y = 0; y < length; y++) {
            const byte3 at = { 5, y, 5 };
            if (!getv_VoxelNode(zox_get(chunk, VoxelNode), depth, at)) {
                zox_tst_bdq_chunk = chunk;
                zox_tst_bdq_at = at;
                zox_muter(chunk, VoxelNodeQueue, queue);
                a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = realm_filler_voxel(world, realm), .data = 6, .position = at });
                zox_tst_bdq_stage = 1;
                zox_tst_bdq_frames = 0;
                return 0;
            }
        }
        return zox_tst_bdq_frames > 600;
    }
    if (zox_tst_bdq_frames < 10) {
        return 0;
    }
    const entity chunk = zox_tst_bdq_chunk;
    if (zox_tst_bdq_stage == 1) {
        zox_tst_bdq_set = get_block_data(zox_get(chunk, BlockDatas), zox_tst_bdq_at);
        // saved as chunk3_save_system does, then loaded back by load_chunk
        char filename[128];
        get_chunk_filename(filename, zox_getv(chunk, ChunkPosition));
        char* path = join_path(zox_get(realm, FolderPath)->value, filename);
        FILE* file = fopen(path, "wb");
        if (file) {
            save_voxel_node(file, zox_get(chunk, VoxelNode));
            save_block_datas(file, zox_get(chunk, BlockDatas));
            fclose(file);
        }
        free(path);
        VoxelNode node;
        create_VoxelNode(&node);
        BlockDatas datas = { 0 };
        spinlock_init(&datas.lock);
        if (load_chunk(world, realm, zox_getv(chunk, ChunkPosition), &node, &datas)) {
            zox_tst_bdq_loaded = get_block_data(&datas, zox_tst_bdq_at);
        }
        dispose_BlockDatas(&datas);
        collapse_VoxelNode(&node);
        zox_muter(chunk, VoxelNodeQueue, queue);
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = 0, .position = zox_tst_bdq_at });
        zox_tst_bdq_stage = 2;
        zox_tst_bdq_frames = 0;
        return 0;
    }
    zox_tst_bdq_cleared = !get_block_data(zox_get(chunk, BlockDatas), zox_tst_bdq_at);
    zox_tst_bdq_stage = 3;
    return 1;
}

void zox_tst_block_data_queue_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_bdq_stage == 3, "it stopped at stage [%i]", zox_tst_bdq_stage);
    zox_tst_check(zox_tst_bdq_set == 6, "queued with data 6, the chunk has [%i]", zox_tst_bdq_set);
    zox_tst_check(zox_tst_bdq_loaded == 6, "saved and loaded, its data is [%i]", zox_tst_bdq_loaded);
    zox_tst_check(zox_tst_bdq_cleared, "removed, its data stayed");
}
