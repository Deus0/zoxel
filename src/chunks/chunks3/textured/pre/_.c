#include "chunk.c"
#include "mesh.c"
entity prefab_chunk_textured;
entity prefab_chunk_mesh_textured;

void spawn_prefabs_chunks_textured(ecs *world) {
    prefab_chunk_mesh_textured = spawn_prefab_chunk_mesh_textured(world, prefab_chunk_mesh);
    prefab_chunk_textured = spawn_prefab_chunk_textured(world, prefab_chunk_invisible);
}
