/*
 * +------------------------------------------------------------------+
 * | Zox Module: Musics                                               |
 * |                                                                  |
 * |  Notes - Songs - Jingles - Playlists                             |
 * |                                                                  |
 * |  Uses: Sounds                                                    |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
// TODO: make sub modules:
//      - notes
//      - songs
//      - playlists
//      - io

#include "set/_.c"
#include "com/_.c"
#include "dat/_.c"
#include "cnv/_.c"
#include "playlists/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "sys/_.c"

void import_musics(ecs* world) {
    zox_module(musics);
    zox_components_musics(world);
    zox_systems_music(world);
    add_hook_terminal_command(process_arguments_musics);
    add_hook_spawn_prefabs(spawn_prefabs_musics);
    zox_import_module(Playlists);
}