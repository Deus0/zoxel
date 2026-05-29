#include "block.c"
#include "texture.c"
entity prefab_block;
entity prefab_vox_texture;  // QUESTION: Should I move this to textures?
extern byte block_vox_depth;

void spawn_prefabs_blocks(ecs *world) {
    prefab_block = spawn_prefab_block(world);
    prefab_vox_texture = spawn_prefab_vox_texture(world, prefab_texture, block_vox_depth);
}
