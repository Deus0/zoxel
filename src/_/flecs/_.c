// zox_flecs_3 | zox_flecs_4
#define zox_flecs_4

byte running = 1;
uint ecs_run_count = 0;
byte zox_log_system_stats = 0;
byte zox_log_pipelines = 0;
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
