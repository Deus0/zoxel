// zox_flecs_3 | zox_flecs_4
#define zox_flecs_4
// #define zox_enable_log_new_system

byte running = 1;
#include "fun/spinlock.c"
#include "mcr/_.c"
#include "dat/_.c"
#include "comm/_.c"
#include "com/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "comm/state.c"

ecs* open_ecs(int argc, char* argv[]) {
    ecs* new_world = ecs_init_w_args(argc, argv);
    return new_world;
}

ecs* initialize_ecs(int argc, char* argv[]) {
    ecs *new_world = open_ecs(argc, argv);
    local_world = new_world;
    return new_world;
}

// NOTE: zox_get_prefab(e, name); to get the prefab parent