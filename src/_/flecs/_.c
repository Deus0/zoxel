// Type Renames
typedef ecs_entity_t entity;
typedef ecs_world_t ecs;
typedef ecs_iter_t iter;
typedef ecs_query_t query;
// Globals
byte running = 1;
int zox_exit_code = 0; // returned by main, i.e. tests set 1 on fail
byte cores_count = 1;
byte target_fps = 0;
uint ecs_run_count = 0;
ecs* local_world;
// Testing
byte profiler = 1;
byte zox_disable_threads = 0;
// Logs
byte zox_log_pipelines = 0;         // 1 for big frames, 2 for all
byte zox_log_system_stats = 0;
byte flecs_log_level = 0;           // 1 for debug_pipelines
byte zox_dbg_observers = 0;

#include "fun/spinlock.c"
#include "mcr/_.c"
#include "dat/_.c"
#include "comm/_.c"
#include "com/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "comm/state.c"

ecs* zox_ecs(int argc, char* argv[]) {
    zox_log("[zox_ecs]");
    ecs* world = ecs_init_w_args(
        argc,
        argv);
    initialize_zox_phases(world);
    // flecs script's async tasks, unused: an immediate system every frame, 2 merges
    entity progress_tasks = ecs_lookup(world, "flecs.script.ProgressTasks");
    if (!progress_tasks) {
        progress_tasks = ecs_lookup(world, "ProgressTasks");
    }
    if (progress_tasks) {
        ecs_enable(world, progress_tasks, false);
    }
    zox_unlinks_initialize(world);
    local_world = world;
    return world;
}
