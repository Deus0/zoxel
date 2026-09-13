// TODO: Just use base mesh instead
entity spawn_prefab_chunk_mesh(
    ecs* world,
    entity prefab)
{
    entity e = zox_prefab_from_parent(world, prefab);
    zox_set_unique_name(e, "chunk_mesh");
    zox_add(e, ChunkMesh);
    zox_setv(e, RenderDepth, 0);
    // NOTE: Start Building and Disabled
    zox_add(e, Disabled);
    zox_add(e, BuildMesh);
    return e;
}

entity spawn_prefab_chunk_mesh_textured(
    ecs* world,
    entity prefab)
{
    entity e = zox_prefab_from_parent(world, prefab);
    zox_set_unique_name(e, "chunk_mesh_textured");
    zox_add(e, TexturedMesh3);
    zox_setv(e, MeshUVs, 0);
    zox_setv(e, UvsGPULink, 0);
    return e;
}
