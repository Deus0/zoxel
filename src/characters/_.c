#ifndef zoxm_characters
#define zoxm_characters

byte zox_no_humanoids = 0;

#include "set/_.c"
#include "com/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "dbg/_.c"
#include "jump/_.c"
#include "characters2/_.c"
#include "characters3/_.c"
#include "fun/buttons.c"

zox_begin_module(Characters) {
    add_hook_terminal_command(process_arguments_characters);
    add_hook_spawn_prefabs(spawn_prefabs_characters);
    define_components_characters(world);
    zox_import_module(Jumps);
    zox_import_module(Characters2);
    zox_import_module(Characters3);
} zox_end_module(Characters);

#endif
