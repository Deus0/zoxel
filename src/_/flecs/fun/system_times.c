// every system's total time spent (seconds), measured by flecs (ecs_measure_system_time in init.c)
//  - for profilers: the difference between two frames is a system's time that frame
//  - multi threaded systems' times add up every worker's (cpu time), divide by zox_worker_count for real time
typedef void (*zox_system_time_fun)(ecs* world, entity system, double seconds, byte multi_threaded, void* ctx);

void zox_each_system_time(ecs* world, zox_system_time_fun fun, void* ctx) {
    ecs_iter_t it = ecs_each_id(world, EcsSystem);
    while (ecs_each_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            const ecs_system_t* system = ecs_system_get(world, it.entities[i]);
            if (system) {
                fun(world, it.entities[i], system->time_spent, system->multi_threaded, ctx);
            }
        }
    }
}

// flecs times each system (on in flecs_profiler builds, profiling tests turn it on)
void zox_measure_system_times(ecs* world) {
    ecs_measure_system_time(world, 1);
    ecs_measure_frame_time(world, 1);
}

// the world's running totals (seconds): its frames, merging deferred commands, notifying observers
typedef struct {
    double frame;
    double merge;
    double emit;
} ZoxWorldTimes;

ZoxWorldTimes zox_world_times(ecs* world) {
    const ecs_world_info_t* info = ecs_get_world_info(world);
    return (ZoxWorldTimes) { info->frame_time_total, info->merge_time_total, info->emit_time_total };
}

// threads running multi threaded systems
int zox_worker_count(ecs* world) {
    const int count = ecs_get_stage_count(world);
    return count > 0 ? count : 1;
}
