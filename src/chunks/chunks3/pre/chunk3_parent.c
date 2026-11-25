// a chunk3 parent: a vox whose children are chunks, found by position (ChunkLinks), meshed and lit together, chunks
//  side by side at different lods (sides.c, the lod swap); terrain is one that streams them (prefab_terrain)
entity spawn_prefab_chunk3_parent(ecs* world) {
    zox_prefab();
    zox_prefab_name("chunk3_parent");
    zox_setv(e, BlockScale, default_vox_scale);
    zox_setv(e, NodeDepth, 0);
    zox_add(e, ChunkLinks);
    // gpu meshing's (lights3/sys/gpu_mesh.c), made on its first gpu mesh: here so that's a set, not an add (an add at
    //  its first mesh was lost, its program and grids made twice, one leaked)
    zox_setv(e, ComputeGPULink, 0);
    zox_setv(e, GridGPULink, guint2_zero);
    return e;
}
