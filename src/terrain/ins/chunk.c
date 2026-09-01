// NOTE: Somethings here can be calculated at terrain level
entity spawn_terrain_chunk(
    ecs* world,
    entity prefab,
    entity realm,
    entity terrain,
    entity tilemap,
    lint terrain_seed,
    int3 position,
    byte terrain_depth,
    float terrain_scalev,
    byte render_distance,
    byte render_depth)
{
    // Terrain level
    byte length = octree_size(terrain_depth);
    int3 size = int3_single(length);
    // scale needs to be based on chunk itself
    lint seed = position_seed(terrain_seed, position);
    float chunk_scalev = get_chunk_scale(
        render_depth,
        terrain_depth,
        terrain_scalev);
    // we should just pass in positionf - local position of parent!
    float3 bounds = calculate_vox_bounds(size, chunk_scalev);
    float3 positionf = float3_scale(
        float3_from_int3(position),
        length * terrain_scalev);
    entity e = zox_ins(world, prefab);
    {
        char name[64];
        sprintf(name, "terrain_chunk_%ix%ix%i",
            position.x,
            position.y,
            position.z);
        zox_set_unique_name(e, name);
    }
    // Links
    zox_set_parent(world, e, terrain);
    zox_setv(e, TilemapLink, tilemap);
    zox_setv(e, RealmLink, realm);
    zox_setv(e, BlockManagerLink, realm);
    // Data
    zox_setv(e, Seed, seed);
    zox_setv(e, ChunkPosition, position);
    zox_setv(e, ChunkSize, size);
    zox_setv(e, BlockScale, chunk_scalev);    // set from parent
    zox_setv(e, Position3D, positionf);
    zox_setv(e, TransformMatrix, float4x4_position(positionf));
    zox_setv(e, RenderDistance, render_distance);
    zox_setv(e, RenderDepth, render_depth);
    zox_setv(e, Bounds3D, bounds);
    // Initialize our spinlocks
    initialize_voxel_lock(world, e);
    spinlock llock;
    spinlock_init(&llock);
    zox_setv(e, LightNodeLock, llock);
    // Events
    // zox_setv(e, ChunkLodDirty, zox_chunk_lod_dirty_start);
    return e;
}
