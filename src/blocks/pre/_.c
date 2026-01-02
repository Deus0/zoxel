#include "block.c"
entity prefab_block;

void spawn_prefabs_blocks(ecs *world) {
    prefab_block = spawn_prefab_block(world);
}
