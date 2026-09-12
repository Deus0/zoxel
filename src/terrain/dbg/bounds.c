void toggle_debug_bounds_delve(ecs *world, const VoxelNode *node, byte mode) {
    if (is_closed_VoxelNode(node)) {
        return;
    } else if (is_linked_VoxelNode(node)) {
        const entity e = get_entity_VoxelNode(node);
        if (zox_valid(e)) {
            zox_set(e, DebugCubeLines, { mode });
        }
    } else if (has_children_VoxelNode(node)) {
        VoxelNode* kids = (VoxelNode*) node->ptr;
        for (int i = 0; i < octree_length; i++) {
            toggle_debug_bounds_delve(world, &kids[i], mode);
        }
    }
}

void zox_dbg_toggle_gizmos_vodes(ecs *world, ClickEventData data) {
    if (!zox_valid(local_terrain) || !zox_valid(prefab_block_vox) || !zox_has(prefab_block_vox, DebugCubeLines)) {
        return;
    }
    zox_geter_value(prefab_block_vox, DebugCubeLines, byte, mode);
    cycle_cubeline_debug(&mode);
    zox_set(prefab_block_vox, DebugCubeLines, { mode });
    zox_set(prefab_block_vox_instanced, DebugCubeLines, { mode });
    zox_geter(local_terrain, ChunkLinks, chunkLinks);
    for (uint i = 0; i < chunkLinks->value->size; i++) {
        int3_hashmap_pair* pair = chunkLinks->value->data[i];
        uint checks = 0;
        while (pair != NULL && checks < max_safety_checks_hashmap) {
            entity chunk = pair->value;
            if (zox_valid(chunk)) {
                if (zox_has(chunk, BlocksSpawned)) {
                    zox_geter(chunk, VoxelNode, node);
                    toggle_debug_bounds_delve(world, node, mode);
                }
            }
            pair = pair->next;
            checks++;
        }
    }
}

/*void key_down_toggle_debug_voxes_bounds(ecs *world, int32_t keycode) {
    if (keycode == zox_key_F6) {
        toggle_debug_block_voxes_bounds(world);
    }
}*/
