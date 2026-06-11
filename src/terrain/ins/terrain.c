int get_chunk_index(int i, int j, int rows) {
    return (i + rows + 1) * (rows + rows + 1) + (j + rows + 1);
}
// vertical_rows + vertical_rows +  - rows + rows + 1
int calculate_terrain_chunks_count(int rows, int slabs) {
    return (slabs + slabs + 1) * (rows + rows + 1) * (rows + rows + 1);
}

int get_chunk_index_2(int i, int j, int k, int rows, int vertical) {
    i += rows;
    j += vertical;
    k += rows;
    return i * (rows + rows + 1) + j * (rows + rows + 1) * (rows + rows + 1) + k;
}

int get_chunk_index_3(int3 position, int rows, int vertical) {
    position.x += rows;
    position.y += vertical;
    position.z += rows;
    return position.x * (rows + rows + 1) + position.y  * (rows + rows + 1) * (rows + rows + 1) + position.z;
}

entity spawn_terrain(ecs *world, entity prefab, entity realm, lint seed) {
    if (!zox_valid(realm) || !zox_has(realm, TilemapLink)) {
        zox_loge("Realm has no TilemapLink [%lu]", realm);
        return 0;
    }
    zox_geter_value(realm, TilemapLink, entity, tilemap);
    byte depth = terrain_depth;
    float3 position = float3_zero;
    float scale = 0.5f; // 0.5f | 1
    zox_instance(prefab);
    zox_name("terrain");
    zox_set_parent(world, e, realm);
    // Link them link this for now
    zox_set(e, RealmLink, { realm });
    zox_set(realm, TerrainLink, { e });
    zox_set(e, Seed, { seed });
    zox_set(e, Position3D, { position });
    zox_set(e, BlockScale, { scale });
    zox_set(e, NodeDepth, { depth });
    // Initialize Hashmaps
    zox_set_ptr(e, RegionLinks, (RegionLinks) { .value = create_int2_hashmap(32) });
    zox_set_ptr(e, TunkLinks, (TunkLinks) { .value = create_int2_hashmap(32) });
    zox_set_ptr(e, ChunkLinks, (ChunkLinks) { .value = create_int3_hashmap(32 * 32) });
    if (tilemap) {
        zox_set(e, TilemapLink, { tilemap });
        // did tilemap have a voxlink previously
        zox_set(tilemap, VoxLink, { e })    // link tilemap to terrain
    }
    local_terrain = e;
    return e;
}
