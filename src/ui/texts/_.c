// TODO: Convert TextData to Ascii!

uint texts_children_capacity = 250;
byte zox_texts_min_resolution = 32;

// todo: move labels to here as prefab
#include "dat/_.c"
#include "com/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "ins/_.c"
#include "dbg/_.c"

void import_texts(ecs* world) {
    zox_module(texts);
    define_components_texts(world);
    define_systems_texts(world);
    add_hook_spawn_prefabs(spawn_prefabs_texts);
}
