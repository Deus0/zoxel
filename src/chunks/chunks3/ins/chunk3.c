// A simple colored vox at position
entity spawn_chunk3(
    ecs *world,
    entity prefab,
    float3 position,
    float scale,
    byte depth,
    float block_scale,
    int3 size)
{
    zox_instance(prefab);
    zox_name("chunk3")
    zox_setv(e, Position3D, position);
    zox_setv(e, Scale1, scale);
    zox_setv(e, BlockScale, block_scale);
    zox_setv(e, ChunkSize, size);
    zox_setv(e, NodeDepth, depth);
    zox_setv(e, RenderDepth, depth);
    return e;
}
