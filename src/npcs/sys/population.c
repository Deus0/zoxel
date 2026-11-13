// A region's population map, a step after its towns and dungeons (zox_generate_region_population): each chunk
//  column's npc density from its towns (fun/population.c), chunks spawn by it (spawn.c)
void population_map_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BlockPosition2);
    zox_sys_in(BlockSize2);
    zox_sys_out(GenerateRegion);
    zox_sys_out(PopulationMap);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(BlockPosition2, block_position);
        zox_sys_i(BlockSize2, block_size);
        zox_sys_o(GenerateRegion, generate);
        zox_sys_o(PopulationMap, map);
        if (generate->value != zox_generate_region_population) {
            continue;
        }
        const entity terrain = zox_get_parent(world, e);
        const int chunk_length = zox_valid(terrain) ? octree_size(zox_getv(terrain, NodeDepth)) : 16;
        entity towns[zox_children_capacity];
        const uint towns_count = zox_get_children_by_id(world, e, towns, zox_children_capacity, zox_id(Town));
        int2 centers[zox_children_capacity];
        byte2 sizes[zox_children_capacity];
        for (uint j = 0; j < towns_count; j++) {
            centers[j] = zox_getv(towns[j], BlockPosition2);
            sizes[j] = zox_getv(towns[j], TownSize);
        }
        const int2 size = population_map_size(block_size->value, chunk_length);
        resize_PopulationMap(map, size.x * size.y);
        for (int y = 0; y < size.y; y++) {
            for (int x = 0; x < size.x; x++) {
                // the middle of the chunk column
                const int2 global = {
                    block_position->value.x + x * chunk_length + chunk_length / 2,
                    block_position->value.y + y * chunk_length + chunk_length / 2
                };
                map->value[x + y * size.x] = population_at(global, centers, sizes, towns_count);
            }
        }
        generate->value = zox_generate_region_end;
    }
    zox_sys_on_end();
} zoxd_system(population_map_system);
