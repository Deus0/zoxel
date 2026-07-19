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
entity prefab_invisible_vox;
entity prefab_static_vox;
entity prefab_vox_instanced;
entity prefab_vox_file;
entity prefab_vox_generated;
entity prefab_static_vox_instanced;

void zox_define_prefabs_voxes(ecs *world) {
    // Moveable Voxes
    prefab_vox = spawn_prefab_vox(world, prefab_chunk, block_vox_depth);
    // Level 1
    prefab_block_vox_meta = spawn_prefab_block_vox_meta(world, prefab_block);
    prefab_vox_file = spawn_prefab_vox_file(world, prefab_chunk_base);
    prefab_vox_instanced = spawn_prefab_vox_instanced(world, prefab_renderer_instance);
    prefab_static_vox = spawn_prefab_vox(world, prefab_static_chunk, block_vox_depth);
    prefab_invisible_vox = spawn_prefab_vox(world, prefab_static_chunk, block_vox_depth);
    prefab_static_vox_instanced = spawn_prefab_vox_instanced(world, prefab_static_instance_mesh);
    // Level 2
    prefab_block_vox = spawn_prefab_block_vox(world, prefab_static_vox);
    prefab_vox_generated = spawn_prefab_vox_generated(world, prefab_static_vox);
    prefab_block_vox_instanced = spawn_prefab_block_vox_instanced(world, prefab_static_vox_instanced);
    if (prefab_vox_texture) {
        zox_prefab_set(prefab_vox_texture, VoxLink, { 0 });
    }
}
