typedef ecs_entity_t entity;
typedef ecs_world_t ecs;
typedef ecs_iter_t iter;
typedef ecs_query_t query;

// Testing
byte zox_log_pipelines = 0;     // 1 for big frames, 2 for all

byte zox_disable_threads = 0;
byte target_fps = 0;
byte zox_log_system_stats = 0;
byte flecs_log_level = 0;   // 1 for debug_pipelines
byte profiler = 1;
byte cores_count = 1;
#include "fun/spinlock.c"
#include "mcr/_.c"
byte running = 1;
uint ecs_run_count = 0;
ecs *local_world;

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
