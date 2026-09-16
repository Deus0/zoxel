byte save_voxel_node(FILE* out, const VoxelNode* node) {
    if (!node) {
        return 1;
    }
    if (fwrite(&node->value, sizeof(byte), 1, out) != 1) {
        zox_loge("[save_voxel_node:1] Failed to write node value.");
        return 1;
    }
    byte has_children = has_children_VoxelNode(node);
    if (fwrite(&has_children, sizeof(byte), 1, out) != 1) {
        zox_loge("[save_voxel_node:2] Failed to write has_children.");
        return 1;
    }
    byte result = 0;
    if (has_children) {
        if (!node->ptr) {
            zox_loge("Node has children but ptr is null!");
            return 1;
        }
        VoxelNode* children = (VoxelNode*) node->ptr;
        for (int i = 0; i < 8; i++) {
            byte error = save_voxel_node(out, &children[i]);
            if (error) {
                result = 1;
            }
        }
    }
    return result;
}

void chunk3_save_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNode);
    zox_sys_in(ChunkPosition);
    zox_sys_out(VoxelNodeLock);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(VoxelNode, node);
        zox_sys_i(ChunkPosition, position);
        zox_sys_o(VoxelNodeLock, lock);
        // later add id/int3 there
        entity terrain = zox_get_parent(world, e);
#ifdef zox_safety_checks
        if (!zox_valid(terrain)) {
            zox_loge("[Chunk3SaveSystem] Invalid Terrain");
            continue;
        }
#endif
        entity realm = zox_get_parent(world, terrain);
#ifdef zox_safety_checks
        if (!zox_valid(realm)) {
            zox_loge("[Chunk3SaveSystem] Invalid Realm");
            continue;
        }
#endif
        char filename[128];
        get_chunk_filename(filename, position->value);
        zox_geter(realm, FolderPath, game_path);
        char* path = join_path(game_path->value, filename);
        FILE* file = fopen(path, "wb");
        if (file == NULL) {
            zox_loge("Error saving [%s]", path);
            free(path);
            continue;
        }
        spin_lock(&lock->value);
        save_voxel_node(file, node);
        spin_unlock(&lock->value);
        if (fclose(file) != 0) {
            zox_loge("Failed to close file: %s", path);
        }
        free(path);
    }
    zox_sys_on_end();
} zoxd_system(chunk3_save_system);
