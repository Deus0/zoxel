// TODO: Add quadtree data and chunk2 tag
entity spawn_prefab_chunk2(ecs *world) {
    const byte length = 32;
    zox_prefab();
    zox_prefab_name("chunk2");
    zox_add_tag(e, Chunk2);
    zox_prefab_set(e, Chunk2Position, { int2_zero });
    zox_prefab_set(e, Chunk2Size, { int2_single(length) });
    zox_prefab_set(e, Chunk2Neighbors, { { 0, 0, 0, 0 } });
    // allocations
    zox_prefab_set(e, NodeDepth, { 0 });

    zox_prefab_set(e, RenderDistance, { render_depth_spawning });
    zox_prefab_set(e, RenderDistanceDirty, { zox_dirty_none });
    zox_prefab_set(e, RenderDepth, { render_depth_spawning });
    zox_prefab_set(e, RenderDepthDirty, { zox_dirty_none });
    return e;
}
