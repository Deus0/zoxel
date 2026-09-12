byte load_voxel_node(ecs* world, FILE* in, VoxelNode* node) {
    // read value of node
    if (fread(&node->value, sizeof(byte), 1, in) != 1) {
        zox_log("[load_voxel_node:1] read error.");
        return 0;
    }
    // read has children
    byte has_children = 0;
    if (fread(&has_children, sizeof(byte), 1, in) != 1) {
        zox_log("[load_voxel_node:2] read error.");
        return 0;
    }
    if (has_children) {
        if (!has_children_VoxelNode(node)) {
            open_one_VoxelNode(node);
        }
        VoxelNode* children = (VoxelNode*) node->ptr;
        for (int i = 0; i < 8; i++) {
            load_voxel_node(world, in, &children[i]);
        }
    } else {
        if (has_children_VoxelNode(node)) {
            // close_VoxelNode(world, node);
            collapse_VoxelNode(node);
        }
    }
    return 1;
}

// returns 1 if loaded
byte load_chunk(
    ecs *world,
    entity savegame,
    int3 position,
    VoxelNode* node)
{
    char filename[128];
    get_chunk_filename(filename, position);
    // sprintf(filename, "chunk_%i_%i_%i.dat", position.x, position.y, position.z);
    zox_geter(savegame, FolderPath, game_path);
    if (!game_path) {
        zox_loge("[load_chunk] Invalid FolderPath [%s]",
            zox_getn(savegame));
        return 0;
    }
    char* path = join_path(game_path->value, filename);
    if (!path) {
        zox_loge("[load_chunk] join_path failed [%s] folder=[%s] filename=[%s]",
            zox_getn(savegame),
            game_path->value,
            filename);
        return 0;
    }
    if (!file_exists(path)) {
        free(path);
        return 0;
    }
    // check if file exist
    FILE* file = fopen(path, "rb");
    if (!file) {
        zox_loge("Failed to open filepath [%s]", path);
        free(path);
        return 0;
    }
    // zox_log("Loading chunk from file: %s", path);
    // write_lock_VoxelNode(node);
    byte success = load_voxel_node(world, file, node);
    // write_unlock_VoxelNode(node);
    if (fclose(file) != 0) {
        zox_log_error("Failed to close file: %s", path);
    }
    free(path);
    return success;
}

// NOTE: If Loading Chunk has linked minivoxes, this will destroy them
// WARNING: No longer supported RELOADING due to this - Assumes empty chunk
// TODO: Load at a LOD Level
//      - Initialize just flags as Saved
//      - We can then grab the file contents at higher depths when LOD updates
zox_sys2(Chunk3LoadSystem) {
    byte dbg_log = 0;
    zox_sys_world();    // used when closing possible nodes
    zox_sys_begin();
    zox_sys_in(ChunkPosition);
    zox_sys_out(NodeDepth);
    zox_sys_out(VoxelNode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ChunkPosition, position);
        zox_sys_o(VoxelNode, voxels);
        zox_sys_o(NodeDepth, depth);
        entity terrain = zox_get_parent(world, e);
#ifdef zox_safety_checks
        if (!zox_valid(terrain)) {
            zox_loge("[Chunk3LoadSystem] Invalid Terrain");
            continue;
        }
#endif
        /*entity game = zox_get_parent(world, e);
        if (!zox_valid(game)) {
            zox_loge("[Chunk3LoadSystem] Invalid [game]");
            continue;
        }*/
        entity realm = zox_get_link(world, terrain, RealmLink);
#ifdef zox_safety_checks
        if (!zox_valid(realm)) {
            zox_loge("[Chunk3LoadSystem] [%s] Invalid [realm] on %s",
                zox_getn(e),
                zox_getn(terrain));
            continue;
        }
        if (!zox_has(realm, FolderPath)) {
            zox_loge("[Chunk3LoadSystem] Realm [%s] has no FolderPath",
                zox_getn(realm));
            continue;
        }
#endif
        if (load_chunk(world, realm, position->value, voxels)) {
            depth->value = terrain_depth;
            zox_add(e, Loaded);
            zox_setv(e, GenerateChunk, zox_generate_terrain_sunlight);
            zox_add(e, VoxelNodeDirty);
            if (dbg_log) {
                zox_log("Loaded Chunk [%s] with Depth [%i]",
                    zox_sys_e_name,
                    depth->value);
            }
        }
    }
} zox_sys_end(Chunk3LoadSystem);
