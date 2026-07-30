byte save_voxel_node(FILE* out, const VoxelNode* node) {
    if (!node) {
        return 1;
    }
    if (fwrite(&node->value, sizeof(byte), 1, out) != 1) {
        zox_log_error("[save_voxel_node:1] Failed to write node value.");
        return 1;
    }
    byte has_children = has_children_VoxelNode(node);
    if (fwrite(&has_children, sizeof(byte), 1, out) != 1) {
        zox_log_error("[save_voxel_node:2] Failed to write has_children.");
        return 1;
    }
    byte result = 0;
    if (has_children) {
        if (!node->ptr) {
            zox_log_error("Node has children but ptr is null!");
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

zox_sys2(Chunk3SaveSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNodeEdited);
    zox_sys_in(VoxelNodeDirty);
    zox_sys_in(VoxelNode);
    zox_sys_in(ChunkPosition);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(VoxelNodeEdited, edited);
        zox_sys_i(VoxelNodeDirty, dirty);
        zox_sys_i(VoxelNode, node);
        zox_sys_i(ChunkPosition, position);
        if (dirty->value != zox_dirty_active || !edited->value) {
            continue; // these shouldn't be here
        }
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
            zox_log_error("Error saving [%s]", path);
            free(path);
            continue;
        }
        // read_lock_VoxelNode(node);
        save_voxel_node(file, node);
        // read_unlock_VoxelNode(node);
        if (fclose(file) != 0) {
            zox_log_error("Failed to close file: %s", path);
        }
        free(path);
    }
} zox_sys_end(Chunk3SaveSystem);
