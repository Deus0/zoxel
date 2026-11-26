// static mesh 3
entity spawn_prefab_mesh3(ecs* world) {
    zox_prefab();
    zox_prefab_name("mesh3");
    zox_add(e, Mesh);
    zox_add(e, Mesh3);
    // Transform
    zox_setv(e, TransformMatrix, float4x4_identity);
    // Mesh Data
    zox_add(e, Mesh);
    zox_add(e, MeshIndicies);
    zox_add(e, MeshVertices);
    zox_add(e, MeshColors);
    // Gpu Links
    zox_setv(e, MeshGPULink, 0);
    zox_setv(e, ColorsGPULink, 0);
    zox_setv(e, MeshRenderCount, 0);
    // Properties
    zox_setv(e, RenderDisabled, 0);
    // Starting Events
    zox_add(e, PreInitialize);
    return e;
}

// a lit mesh: a colored mesh (VoxMesh) drawn with the lit material (colored3D_lit) once spawned, flat shaded by the sun
entity spawn_prefab_lit_mesh(ecs* world) {
    zox_prefab();
    zox_prefab_name("lit_mesh");
    zox_add(e, Mesh);
    zox_add(e, VoxMesh);
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, Rotation3D, quaternion_identity);
    zox_setv(e, Scale1, 1);
    zox_setv(e, LocalPosition3D, float3_zero);
    zox_setv(e, LocalRotation3D, quaternion_identity);
    zox_setv(e, TransformMatrix, float4x4_identity);
    zox_setv(e, RenderDisabled, 0);
    zox_setv(e, Brightness, 1);
    zox_add(e, MeshIndicies);
    zox_add(e, MeshVertices);
    zox_add(e, MeshColors);
    zox_setv(e, MeshGPULink, guint2_zero);
    zox_setv(e, ColorsGPULink, 0);
    zox_setv(e, MeshRenderCount, 0);
    zox_add(e, PreInitialize);  // its gpu buffers made, then uploaded (MeshDirty)
    return e;
}
