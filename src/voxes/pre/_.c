void add_components_mesh_colored(ecs *world, entity e) {
    zox_add_tag(e, ColorChunk);
    zox_prefab_set(e, ColorRGBs, { 0 });
    zox_prefab_set(e, MeshColorRGBs, { 0 });
    add_gpu_colors(world, e);
}

#include "vox.c"
#include "vox_generated.c"
#include "vox_file.c"
#include "vox_instanced.c"
#include "block_vox.c"
#include "block_vox_instanced.c"
#include "block_vox_meta.c"
entity prefab_block_vox_meta;
entity prefab_block_vox;
entity prefab_block_vox_instanced;
entity prefab_vox;
entity prefab_vox_instanced;
entity prefab_vox_generated;
entity prefab_vox_file;

void zox_define_prefabs_voxes(ecs *world) {
    // Add VoxLink to chunk prefabs
    zox_prefab_set(prefab_chunk_base, VoxLink, { 0 });
    zox_prefab_set(prefab_chunk, VoxLink, { 0 });
    // Level 1
    prefab_block_vox_meta = spawn_prefab_block_vox_meta(world, prefab_block);
    prefab_vox_file = spawn_prefab_vox_file(world, prefab_chunk_base);
    prefab_vox = spawn_prefab_vox(world, prefab_chunk, block_vox_depth );
    prefab_vox_instanced = spawn_prefab_vox_instanced(world, prefab_renderer_instance);
    // Level 2
    prefab_vox_generated = spawn_prefab_vox_generated(world, prefab_vox);
    prefab_block_vox = spawn_prefab_block_vox(world, prefab_vox);
    prefab_block_vox_instanced = spawn_prefab_block_vox_instanced(world, prefab_vox_instanced);
}
