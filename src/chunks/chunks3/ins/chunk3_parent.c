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

// a chunk3 parent of colored chunks: a vox model bigger than a chunk (a boss), its palette on it (ColorRGBs; and
//  MaterialIDs, only with more than solid, set after), its chunks take them from it (chunk_colors_build_system)
entity spawn_chunk3_parent_colored(ecs* world, entity prefab, const ColorRGBs* palette, byte depth, float scale) {
    entity e = zox_ins(world, prefab);
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, BlockScale, scale);
    zox_setv(e, NodeDepth, depth);
    zox_set_ptr(e, ChunkLinks, (ChunkLinks) { .value = create_int3_hashmap(64) });
    ColorRGBs colors = { 0 };
    resize_ColorRGBs(&colors, palette->length);
    memcpy(colors.value, palette->value, palette->length * sizeof(color_rgb));
    zox_set_ptr(e, ColorRGBs, colors);  // copied in
    dispose_ColorRGBs(&colors);
    return e;
}

// a colored chunk of a chunk3 parent at a position (prefab a vox's): its own voxels, its palette its parent's, linked
//  both ways to the neighbours there now (its faces against them hidden from its first mesh), placed by its centre
//  (a vox's mesh is centred)
entity spawn_chunk3_parent_colored_chunk(ecs* world, entity prefab, entity parent, int3 position) {
    const byte depth = zox_getv(parent, NodeDepth);
    const float scale = zox_getv(parent, BlockScale);
    const int length = octree_size(depth);
    entity e = zox_ins(world, prefab);
    zox_setv(e, NodeDepth, depth);
    zox_setv(e, RenderDepth, depth);
    zox_setv(e, ChunkSize, int3_single(length));
    zox_setv(e, BlockScale, scale);
    zox_setv(e, ChunkPosition, position);
    zox_add(e, VoxMesh);    // drawn by vox_render_system, its see through child by vox_transparent_render_system
    if (zox_vox_gpu_meshing && zox_gpu_compute_supported()) {
        zox_add(e, GpuMesh);    // meshed on the gpu (sys/vox_gpu.c chunk_colors_gpu_system)
        zox_setv(e, MeshFaceRoom, 0);
        zox_setv(e, MeshCommandGPULink, guint2_zero);
        zox_setv(e, MeshFence, 0);
    }
    zox_setv(e, LocalPosition3D, ((float3) { (position.x + 0.5f) * length * scale, (position.y + 0.5f) * length * scale,
        (position.z + 0.5f) * length * scale }));
    initialize_voxel_lock(world, e);
    zox_set_parent(world, e, parent);
    int3_hashmap* chunks = zox_get_mut(parent, ChunkLinks)->value;
    ChunkNeighbors neighbors = { 0 };
    for (byte d = 0; d < 6; d++) {
        const entity neighbor = int3_hashmap_get(chunks, move_position(position, d));
        if (zox_valid(neighbor)) {
            neighbors.value[d] = neighbor;
            zox_get_mut(neighbor, ChunkNeighbors)->value[reverse_direction(d)] = e;
        }
    }
    zox_set_ptr(e, ChunkNeighbors, neighbors);
    int3_hashmap_add(chunks, position, e);
    return e;
}
