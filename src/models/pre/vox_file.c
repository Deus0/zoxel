entity spawn_prefab_vox_file(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("vox_file");
    zox_add(e, Vox);
    zox_add(e, VoxMesh);
    zox_add(e, ColorChunk);
    zox_add(e, VoxelNode);
    zox_setv(e, Brightness, 1);
    zox_setv(e, RenderDisabled, 1);
    zox_add(e, ChunkSize);
    zox_setv(e, BlockScale, vox_model_scale);
    zox_set(e, ColorRGBs, { 0 });
    zox_add(e, Mesh);
    zox_set(e, MeshIndicies, { 0 });
    zox_set(e, MeshVertices, { 0 });
    zox_set(e, MeshColorRGBs, { 0 });
    return e;
}
