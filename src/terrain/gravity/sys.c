// a body with TerrainGravity takes its Gravity3D from the field at its centre, every frame (a cube toggled under a
//  body that stays still flips it too); outside a full detail chunk it keeps what it had
//  - multithreaded, reads the fields: they're only written in observers (on_gravity_dirty), between systems
void terrain_gravity_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_out(Gravity3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        zox_sys_o(Gravity3D, gravity);
        const entity terrain = zox_get_link(world, e, TerrainLink);
        if (!zox_valid(terrain) || !zox_has(terrain, ChunkLinks)) {
            continue;
        }
        const byte depth = zox_getv(terrain, NodeDepth);
        const int length = octree_size(depth);
        const int3 block = real_position_to_block_position(position->value, zox_getv(terrain, BlockScale));
        const int3 chunk_position = block_position_to_chunk_position(block, depth);
        const entity chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, chunk_position);
        if (!zox_valid(chunk) || !zox_has(chunk, GravityOctree) || zox_getv(chunk, NodeDepth) != depth) {
            continue;
        }
        const byte3 local = { block.x - chunk_position.x * length, block.y - chunk_position.y * length,
            block.z - chunk_position.z * length };
        gravity->value = gravity_pull(getv_GravityOctree(zox_get(chunk, GravityOctree), depth, local));
    }
    zox_sys_on_end();
} zoxd_system(terrain_gravity_system);
