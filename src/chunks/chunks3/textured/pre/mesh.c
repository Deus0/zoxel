entity spawn_prefab_chunk_mesh_textured(ecs* world, entity prefab) {
    entity e = zox_prefab_from_parent(world, prefab);
    zox_add(e, TexturedMesh3D);
    zox_setv(e, MaterialLink, 0);
    zox_setv(e, MeshUVs, 0);
    zox_setv(e, UvsGPULink, 0);
    return e;
}
