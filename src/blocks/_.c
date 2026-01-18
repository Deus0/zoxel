/*
 *  Blocks Module
 *
 *      - Data for the Cubes
 *      - Used by Chunks
 *      - Some Textured, Some Baked
 *
 * */
#ifndef zoxm_blocks
#define zoxm_blocks

#include "com/_.c"
#include "dat/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/terrain_texture_set_system.c"

zox_begin_module(Blocks)
    define_components_blocks(world);
    zox_system(
        TerrainTextureSetSystem,
        EcsOnUpdate,
        [in] realms.RealmLink,
        [out] textures.GenerateTexture,
        [out] textures.TilemapSize,
        [out] textures.TextureLinks,
        [none] textures.Tilemap
    );
    add_hook_spawn_prefabs(spawn_prefabs_blocks);
zox_end_module(Blocks)

#endif