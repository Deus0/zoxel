void prefab_add_mesh_basic(ecs *world, entity e) {
    zox_prefab_set(e, MeshIndicies, { 0 });
    zox_prefab_set(e, MeshRenderCount, { 0 });
    zox_prefab_set(e, MeshVertices, { 0 });
    zox_prefab_set(e, MeshGPULink, { { 0, 0 } });
}

#include "chunk.c"
#include "chunk_base.c"
#include "chunk_invisible.c"
#include "mesh.c"
entity prefab_chunk_base;
entity prefab_chunk;
entity prefab_chunk_invisible;
entity prefab_chunk_mesh;

void spawn_prefabs_chunks(ecs *world) {
    prefab_chunk_base = spawn_prefab_chunk_base(world);
    prefab_chunk = spawn_prefab_chunk(world, prefab_chunk_base);
    prefab_chunk_invisible = spawn_prefab_chunk_invisible(world);
    prefab_chunk_mesh = spawn_prefab_chunk_mesh(world);
}
