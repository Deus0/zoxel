void prefab_add_mesh_basic(ecs *world, entity e) {
    zox_add(e, Mesh);
    zox_add(e, MeshIndicies);
    zox_add(e, MeshRenderCount);
    zox_add(e, MeshVertices);
    zox_add(e, MeshGPULink);
}

#include "chunk.c"
#include "chunk_base.c"
#include "chunk_invisible.c"
#include "mesh.c"
#include "static_chunk.c"
entity prefab_chunk_base;
entity prefab_chunk;
entity prefab_chunk_invisible;
entity prefab_chunk_mesh;
entity prefab_static_chunk;

void spawn_prefabs_chunks(ecs *world) {
    prefab_chunk_base = spawn_prefab_chunk_base(world);
    prefab_chunk = spawn_prefab_chunk(world, prefab_chunk_base);
    prefab_chunk_invisible = spawn_prefab_chunk_invisible(world);
    prefab_chunk_mesh = spawn_prefab_chunk_mesh(world);
    prefab_static_chunk = spawn_prefab_static_chunk(world);
}
