void prefab_add_mesh_basic(ecs *world, entity e) {
    zox_prefab_set(e, MeshIndicies, { 0 });
    zox_prefab_set(e, MesnRenderCount, { 0 });
    zox_prefab_set(e, MeshVertices, { 0 });
    add_gpu_mesh(world, e);
}

#include "chunk.c"
#include "chunk_base.c"
entity prefab_chunk_base;
entity prefab_chunk;

void spawn_prefabs_chunks(ecs *world) {
    prefab_chunk_base = spawn_prefab_chunk_base(world);
    prefab_chunk = spawn_prefab_chunk(world, prefab_chunk_base);
}
