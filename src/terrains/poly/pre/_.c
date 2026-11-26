entity prefab_poly_terrain;

static void spawn_prefabs_poly_terrains(ecs* world) {
    zox_prefab();
    zox_prefab_name("poly_terrain");
    zox_add(e, PolyTerrain);
    zox_add(e, PolyHeights);
    zox_setv(e, PolyCells, poly_terrain_cells);
    zox_setv(e, Size2, poly_terrain_size);
    zox_setv(e, Seed, 0);
    // its clock, its realm's (ClockLink): characters on it move by it, stopped paused
    zox_setv(e, ClockTime, 0);
    zox_setv(e, ClockDelta, 0);
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, Rotation3D, quaternion_identity);
    zox_setv(e, Scale1, 1);
    zox_setv(e, TransformMatrix, float4x4_identity);
    zox_add(e, DisableParentTransform);
    zox_setv(e, RenderDisabled, 0);
    zox_setv(e, Brightness, 1);
    zox_add(e, Mesh);
    zox_add(e, VoxMesh);
    zox_add(e, MeshIndicies);
    zox_add(e, MeshVertices);
    zox_add(e, MeshColorRGBs);
    zox_setv(e, MeshGPULink, guint2_zero);
    zox_setv(e, ColorsGPULink, 0);
    zox_setv(e, MeshRenderCount, 0);
    zox_add(e, PreInitialize);
    prefab_poly_terrain = e;
}
