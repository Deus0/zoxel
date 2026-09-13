entity prefab_chunk_base;
entity prefab_chunk;
entity prefab_chunk_invisible;
entity prefab_chunk_mesh;
entity prefab_static_chunk;
entity prefab_chunk_textured;
entity prefab_chunk_mesh_textured;

void prefab_add_mesh_basic(ecs *world, entity e) {
    zox_add(e, Mesh);
    zox_add(e, MeshIndicies);
    zox_add(e, MeshRenderCount);
    zox_add(e, MeshVertices);
    zox_add(e, MeshGPULink);
}

#include "mesh.c"
#include "chunk.c"
#include "chunk_base.c"
#include "chunk3_textured.c"
#include "static_chunk.c"

void spawn_prefabs_chunks(ecs *world) {
    // Meshes
    prefab_chunk_mesh = spawn_prefab_chunk_mesh(
        world,
        prefab_mesh3);
    prefab_chunk_mesh_textured = spawn_prefab_chunk_mesh_textured(
        world,
        prefab_chunk_mesh);
    // Chunks
    prefab_chunk_base = spawn_prefab_chunk_base(world);
    prefab_chunk = spawn_prefab_chunk(world, prefab_chunk_base);
    prefab_static_chunk = spawn_prefab_static_chunk(world);
    prefab_chunk_invisible = spawn_prefab_chunk_invisible(world);
    prefab_chunk_textured = spawn_prefab_chunk_textured(
        world,
        prefab_chunk_invisible);
}
