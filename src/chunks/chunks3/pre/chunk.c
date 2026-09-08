entity spawn_prefab_chunk(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("chunk");
    zox_setv(e, BlockScale, default_vox_scale );
    // Transforms
    zox_setv(e, Position3D, float3_zero );
    zox_setv(e, Rotation3D, float4_identity );
    zox_setv(e, TransformMatrix, float4x4_identity );
    // Rendering
    zox_setv(e, Brightness, 1);
    zox_setv(e, RenderDisabled, 0);
    zox_add(e, Mesh);
    zox_setv(e, MeshIndicies, 0);
    zox_setv(e, MeshRenderCount, 0);
    zox_setv(e, MeshVertices, 0);
    zox_setv(e, MeshGPULink, 0);
    // Events
    zox_add(e, PreInitialize);
    return e;
}
