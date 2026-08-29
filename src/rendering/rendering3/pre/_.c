// static mesh 3
entity spawn_prefab_mesh3(ecs* world) {
    zox_prefab();
    zox_prefab_name("mesh3");
    zox_add(e, Mesh);
    zox_add(e, Mesh3);
    // Data
    zox_setv(e, TransformMatrix, float4x4_identity);
    zox_setv(e, RenderDisabled, 0);
    // Mesh Data
    zox_add(e, Mesh);
    zox_add(e, MeshIndicies);
    zox_add(e, MeshVertices);
    zox_add(e, MeshColorRGBs);
    // Gpu Links
    zox_setv(e, MeshGPULink, 0);
    zox_setv(e, ColorsGPULink, 0);
    zox_setv(e, MeshRenderCount, 0);
    // Starting Events
    zox_add(e, PreInitialize);
    return e;
}

entity prefab_mesh3;

void zox_prefabs_rendering3(ecs* world) {
    prefab_mesh3 = spawn_prefab_mesh3(world);
}
