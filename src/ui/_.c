#ifndef zoxm_ui
#define zoxm_ui

#include "set/_.c"
#include "com/_.c"
#include "dat/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "zigels/_.c"
#include "texts/_.c"
#include "elements2/_.c"
#include "elements3/_.c"
#include "windows/_.c"
#include "navigation/_.c"
#include "touch/_.c"

zox_begin_module(Elements)
    add_hook_terminal_command(arguments_ui);;
    define_components_elements(world);
    define_systems_elements(world);
    // initialize_settings_elements(world);
    add_hook_spawn_prefabs(initialize_settings_elements);
    add_hook_spawn_prefabs(spawn_prefabs_elements);
    zox_import_module(Zigels);
    zox_import_module(Texts);
    zox_import_module(Elements2);
    zox_import_module(Elements3);
    zox_import_module(Windows);
    zox_import_module(Navigation);
    zox_import_module(Touch);
zox_end_module(Elements)

#endif