/*
 * +------------------------------------------------------------------+
 * | Zox Module: Blocks                                                 |
 * |                                                                  |
 * |  Voxels - Materials - Faces - Properties - Block State             |
 * |                                                                  |
 * +------------------------------------------------------------------+
 *
 * Used by:
 *
 *      - Chunks
 *      - Baked Vox Textures
 *
 * */
#ifndef zoxm_blocks
#define zoxm_blocks

#define zox_blocks_dirty_start 1
#define zox_blocks_dirty_indexes 1
#define zox_blocks_dirty_tilemaps 2
#define zox_blocks_dirty_end 0

float block_place_range = 2;

#include "com/_.c"
#include "dat/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(Blocks) {
    zox_define_components_blocks(world);
    zox_define_systems_blocks(world);
    add_hook_spawn_prefabs(spawn_prefabs_blocks);
} zox_end_module(Blocks);

#endif
