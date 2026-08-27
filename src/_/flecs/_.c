typedef ecs_entity_t entity;
typedef ecs_world_t ecs;
typedef ecs_iter_t iter;
typedef ecs_query_t query;
// Globals
byte running = 1;
byte cores_count = 1;
ecs *local_world;
byte target_fps = 0;
uint ecs_run_count = 0;
// Testing
byte profiler = 1;
byte zox_disable_threads = 0;
byte zox_log_pipelines = 0;     // 1 for big frames, 2 for all
byte zox_log_system_stats = 0;
byte flecs_log_level = 0;   // 1 for debug_pipelines

#include "fun/spinlock.c"
#include "mcr/_.c"
#include "dat/_.c"
#include "comm/_.c"
#include "com/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "comm/state.c"

ecs* open_ecs(int argc, char* argv[]) {
    return ecs_init_w_args(argc, argv);
}

ecs* initialize_ecs(int argc, char* argv[]) {
    ecs *new_world = open_ecs(argc, argv);
    local_world = new_world;
    return new_world;
}
