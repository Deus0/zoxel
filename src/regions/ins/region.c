// TODO: Add quadtree data and chunk2 tag
entity spawn_region(ecs* world, entity prefab, entity parent, int2 position, int2 region_size) {
    zox_instance(prefab);
    zox_set_parent(world, e, parent);
    {
        char name[64];
        sprintf(name, "region_%ix%i", position.x, position.y);
        zox_name(name);
    }
    int3 voxel_position = (int3) { position.x * region_size.x, 0, position.y * region_size.y };
    zox_set(e, VoxelPosition, { voxel_position });
    zox_set(e, VoxelSize, { (int3) { region_size.x, 0, region_size.y } });
    // zox_set(e, RenderDistance, { rdistance });
    // zox_log("+ Tunk2 [%ix%i]", position.x, position.y);
    return e;
}
