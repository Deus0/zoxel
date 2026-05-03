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

entity spawn_terrain(ecs *world, entity p, entity tilemap, float3 position, byte depth, float scale) {

    // zox_log("SPAWNING TERRAIN");

    zox_instance(p);
    zox_name("terrain");

    zox_set(e, Position3D, { position });
    zox_set(e, BlockScale, { scale });
    zox_set(e, NodeDepth, { depth });
    // zox_set(e, Scale1D, { scale })

    if (tilemap) {
        zox_set(e, TilemapLink, { tilemap });
        // did tilemap have a voxlink previously
        zox_set(tilemap, VoxLink, { e })    // link tilemap to terrain
    }

    local_terrain = e;

    return e;
}

// todo: pass in through struct
entity spawn_terrain_streaming(ecs *world, entity realm, entity p
) {
    if (!zox_has(realm, TilemapLink)) {
        zox_log_error("! realm has no TilemapLink [%lu]", realm);
        return 0;
    }

    zox_geter_value(realm, TilemapLink, entity, tilemap)
    if (zox_valid(tilemap) && zox_has(tilemap, RealmLink)) {
        zox_set(tilemap, RealmLink, { realm })
    } else {
        zox_log_error("invalid tilemap: %lu", tilemap);
    }

    float terrain_scale = 0.5f; // 0.5f | 1
    entity e = spawn_terrain(world, p, tilemap, float3_zero, terrain_depth, terrain_scale);

    ChunkLinks chunks = (ChunkLinks) { NULL };
    chunks.value = create_int3_hashmap(128);
    zox_set_ptr(e, ChunkLinks, chunks);

    Chunk2Links chunks2 = (Chunk2Links) { NULL };
    chunks2.value = create_int2_hashmap(32);
    zox_set_ptr(e, Chunk2Links, chunks2);

    return e;
}
