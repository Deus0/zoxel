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

entity spawn_terrain(
    ecs *world,
    entity realm,
    lint seed)
{
    if (!zox_valid(realm)) {
        zox_loge("Realm Invalid [%lu]", realm);
        return 0;
    }
    entity tilemap = zox_get_link(world, realm, Tilemap);
    byte depth = terrain_depth;
    float3 position = float3_zero;
    float scale = 0.5f; // 0.5f | 1
    entity e = zox_ins(world, prefab_terrain);
    zox_name("terrain");
    zox_setv(e, Seed, seed);
    zox_setv(e, Position3D, position);
    zox_setv(e, BlockScale, scale);
    zox_setv(e, NodeDepth, depth);
    // Initialize Hashmaps
    zox_set_ptr(e, RegionLinks, (RegionLinks) { .value = create_int2_hashmap(32) });
    zox_set_ptr(e, TunkLinks, (TunkLinks) { .value = create_int2_hashmap(32) });
    zox_set_ptr(e, ChunkLinks, (ChunkLinks) { .value = create_int3_hashmap(32 * 32) });
    // Link them link this for now
    zox_set_parent(world, e, realm);
    zox_setv(e, RealmLink, realm);
    zox_link(world, realm, Terrain, e);
    if (tilemap) {
        zox_link(world, e, Tilemap, tilemap);
        zox_link(world, tilemap, Terrain, e);
    }
    local_terrain = e;
    return e;
}
