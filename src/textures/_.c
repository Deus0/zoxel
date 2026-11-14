/*
 * +------------------------------------------------------+
 * | Zox Module: Textures                                 |
 * |                                                      |
 * |  Images - Generation - Materials - IO - State        |
 * |                                                      |
 * +------------------------------------------------------+
 */
byte texture_upload_rate = 0; // 8;

// TODO: Move font texture functions into textures
#include "dat/_.c"
#include "com/_.c"
#include "gen/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "io/_.c"
#include "sys/_.c"
#include "dbg/_.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

// todo: rename Textures as TextureLinks, and Textures to Textures again
void import_textures(ecs* world) {
    zox_module(textures);
    zox_components_textures(world);
    zox_systems_textures(world);
#ifdef zox_tests
    zox_tsts_textures();
#endif
    add_hook_spawn_prefabs(spawn_prefabs_textures_core);
    zox_add_module(textures_io);
}
