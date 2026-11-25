entity prefab_chunk_base;
entity prefab_chunk;
entity prefab_chunk_invisible;
entity prefab_chunk_mesh;
entity prefab_static_chunk;
entity prefab_chunk_textured;
entity prefab_chunk_mesh_textured;
entity prefab_chunk_mesh_gpu;
entity prefab_chunk_material;
entity prefab_chunk3_parent;

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
#include "chunk3_parent.c"

void spawn_prefabs_chunks(ecs *world) {
    // Meshes
    prefab_chunk_mesh = spawn_prefab_chunk_mesh(
        world,
        prefab_mesh3);
    prefab_chunk_mesh_textured = spawn_prefab_chunk_mesh_textured(
        world,
        prefab_chunk_mesh);
    prefab_chunk_mesh_gpu = spawn_prefab_chunk_mesh_gpu(world, prefab_chunk_mesh_textured);
    prefab_chunk_material = spawn_prefab_chunk_material(world);
    // Chunks
    prefab_chunk_base = spawn_prefab_chunk_base(world);
    prefab_chunk = spawn_prefab_chunk(world, prefab_chunk_base);
    prefab_static_chunk = spawn_prefab_static_chunk(world);
    prefab_chunk_invisible = spawn_prefab_chunk_invisible(world);
    prefab_chunk_textured = spawn_prefab_chunk_textured(
        world,
        prefab_chunk_invisible);
    prefab_chunk3_parent = spawn_prefab_chunk3_parent(world);
}
