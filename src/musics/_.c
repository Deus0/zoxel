#ifndef zoxm_musics
#define zoxm_musics

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

zox_begin_module(Musics)
    add_hook_on_boot(on_boot_musics);
    add_hook_terminal_command(process_arguments_musics);
    add_hook_spawn_prefabs(spawn_prefabs_musics);
    define_components_musics(world);
    define_systems_music(world);
    zox_import_module(Playlists);
zox_end_module(Musics)

#endif