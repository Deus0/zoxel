// TODO: Use chunk bbasse... wth lol
entity spawn_prefab_vox(
    ecs *world,
    byte depth)
{
    zox_prefab();
    zox_prefab_name("vox");
    zox_add(e, Vox);
    zox_add(e, Chunk);
    zox_add(e, Chunk3);
    zox_setv(e, ChunkPosition, int3_zero);
    zox_setv(e, ChunkSize, int3_zero);
    zox_prefab_set(e, ChunkNeighbors, { { 0 } } );
    // render_depth_uninitialized
    zox_setv(e, RenderDistance, 0);
    zox_setv(e, RenderDepth, 0);
    zox_setv(e, BlockScale, default_vox_scale);
    // allocations
    zox_setv(e, NodeDepth, 0);
    zox_setv(e, VoxelNode, 0);
    zox_setv(e, VoxelNodeLock, 0);
    zox_setv(e, SidesOctree, 0);
    // Dirty State
    // Transforms
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, Rotation3D, float4_identity);
    zox_setv(e, Scale1, 1);
    zox_setv(e, TransformMatrix, float4x4_identity);
    // Rendering
    zox_add(e, Mesh);
    zox_setv(e, Brightness, 1);
    zox_setv(e, RenderDisabled, 0);
    zox_setv(e, MeshIndicies, 0);
    zox_setv(e, MeshRenderCount, 0);
    zox_setv(e, MeshVertices, 0);
    zox_setv(e, MeshGPULink, guint2_zero);
    // zox_prefab_set(e, BuildMesh, { 0 });
    // vox
    zox_setv(e, BlockScale, 0); // vox_model_scale });
    zox_setv(e, NodeDepth, 0); // depth
    zox_add(e, ColorChunk);
    zox_setv(e, ColorRGBs, 0);
    zox_setv(e, MeshColorRGBs, 0);
    zox_setv(e, ColorsGPULink, 0);
    // Events
    zox_add(e, PreInitialize);
    // zox_setv(e, MeshDirty, 0);
    return e;
}

entity spawn_prefab_static_vox(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("vox");
    // vox
    zox_setv(e, BlockScale, 0); // vox_model_scale });
    zox_setv(e, NodeDepth, 0); // depth
    zox_add(e, ColorChunk);
    zox_setv(e, ColorRGBs, 0);
    zox_setv(e, MeshColorRGBs, 0);
    zox_setv(e, ColorsGPULink, 0);
    return e;
}
