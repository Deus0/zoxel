void spawn_world_dungeon_core(ecs *world, spawned_block_data *data) {
    if (!zox_has(data->block, BlockPrefabLink)) {
        return;
    }
    if (!data->octree) {
        // zox_log_error("null node in [spawned_block_vox]")
        return;
    }
    entity prefab = zox_getv(data->block, BlockPrefabLink);
    if (!zox_has(prefab, DungeonCore)) {
        return;
    }
    zox_instance(prefab);
    zox_set_parent(world, e, data->chunk);
    zox_set(e, ChunkLink, { data->chunk });
    // todo: make this set as local position instead - VoxelLocalPosition
    zox_set(e, VoxelLocalPosition, { data->positionl });
    link_node_VoxelNode(data->octree, e);
}
