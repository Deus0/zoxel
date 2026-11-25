// a chunk3 parent of textured chunks in a realm, its chunks depth deep, its blocks scale big, no chunks yet
//  (spawn_chunk3_parent_chunk)
//  - what its voxels are: its blocks (BlockManagerLink, a realm's BlockLinks) and their tiles (Tilemap), its chunks
//    take both from it; a colored parent (vox models) would link a palette the same way
entity spawn_chunk3_parent_textured(ecs* world, entity prefab, entity realm, entity blocks, entity tilemap, byte depth,
    float scale)
{
    entity e = zox_ins(world, prefab);
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, BlockScale, scale);
    zox_setv(e, NodeDepth, depth);
    zox_set_ptr(e, ChunkLinks, (ChunkLinks) { .value = create_int3_hashmap(32 * 32) });
    if (zox_valid(realm)) {
        zox_set_parent(world, e, realm);
        zox_link(world, e, RealmLink, realm);
    }
    if (zox_valid(blocks)) {
        zox_link(world, e, BlockManagerLink, blocks);
    }
    if (zox_valid(tilemap)) {
        zox_link(world, e, Tilemap, tilemap);
    }
    return e;
}

// a chunk of a chunk3 parent at a position, drawn render_depth deep (its lod), its voxels that deep too: its blocks and
//  tilemap its parent's
entity spawn_chunk3_parent_chunk(ecs* world, entity prefab, entity parent, int3 position, byte render_depth) {
    const entity blocks = zox_get_link(world, parent, BlockManagerLink);
    const entity tilemap = zox_get_link(world, parent, Tilemap);
    const entity chunk = spawn_chunk3_textured(world, prefab, blocks, parent, tilemap, 0, position,
        zox_getv(parent, NodeDepth), zox_getv(parent, BlockScale), 0, render_depth);
    int3_hashmap_add(zox_get_mut(parent, ChunkLinks)->value, position, chunk);
    return chunk;
}
