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

// a chunk mesh meshed on the gpu (zox_chunks3_gpu_meshing, rendering/com/gpu_mesh.c): its cpu arrays stay empty
entity spawn_prefab_chunk_mesh_gpu(ecs* world, entity prefab) {
    entity e = zox_prefab_from_parent(world, prefab);
    zox_set_unique_name(e, "chunk_mesh_gpu");
    zox_add(e, GpuMesh);
    zox_setv(e, MeshCommandGPULink, 0);
    zox_setv(e, MeshFaceRoom, 0);
    zox_setv(e, MeshFence, 0);
    return e;
}

// a chunk's child for one block material, its sides, its lod meshes under it (fun/materials.c)
entity spawn_prefab_chunk_material(ecs* world) {
    zox_prefab();
    zox_prefab_name("chunk_material");
    zox_add(e, ChunkMaterial);
    zox_setv(e, BlockMaterial, zox_block_material_solid);
    zox_setv(e, SidesOctree, 0);
    zox_setv(e, SidesOctreeLock, 0);
    return e;
}
