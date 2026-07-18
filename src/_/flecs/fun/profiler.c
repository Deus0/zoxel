void initialize_flecs_profiler(ecs* world) {
    //if (is_using_flecs_stats) {
/*#ifdef zox_use_flecs_profiler
        zox_log("Flecs Profiler Enabled");
        zox_import_module(FlecsStats);
        ecs_measure_system_time(world, 1);
        ecs_measure_frame_time(world, 1);

        //ECS_IMPORT(world, FlecsMonitor);
        // ecs_tracing_enable(1);
        // ecs_log_set_level(0);
#else
    zox_logw("Profiler Define Missing.");
#endif*/
}

#ifdef FLECS_PROFILER

typedef struct {
    ecs_entity_t system;
    uint64_t start;
    double ms;
} ecs_debug_profile_entry_t;

#define DEBUG_MAX_SYSTEMS 4096

static ecs_debug_profile_entry_t debug_profile[DEBUG_MAX_SYSTEMS];
static int32_t debug_profile_count = 0;


static ecs_debug_profile_entry_t *debug_find_system(ecs_entity_t sys)
{
    for (int32_t i = 0; i < debug_profile_count; i++) {
        if (debug_profile[i].system == sys) {
            return &debug_profile[i];
        }
    }

    if (debug_profile_count >= DEBUG_MAX_SYSTEMS) {
        return NULL;
    }

    ecs_debug_profile_entry_t *e =
    &debug_profile[debug_profile_count++];

    e->system = sys;
    e->start = 0;
    e->ms = 0;

    return e;
}


static void debug_profiler_begin(
    ecs_world_t *world,
    ecs_entity_t system)
{
    ecs_debug_profile_entry_t *e =
    debug_find_system(system);

    if (e) {
        e->start = ecs_os_get_time();
    }
}


static void debug_profiler_end(
    ecs_world_t *world,
    ecs_entity_t system)
{
    ecs_debug_profile_entry_t *e =
    debug_find_system(system);

    if (e && e->start) {

        uint64_t end = ecs_os_get_time();

        e->ms =
        (double)(end - e->start) *
        1000.0 /
        (double)ecs_os_get_time_resolution();
    }
}


void debug_print_ecs_frame(ecs_world_t *world)
{
    printf("\n-- system timings --\n");

    for (int32_t i = 0; i < debug_profile_count; i++) {

        ecs_debug_profile_entry_t *e =
        &debug_profile[i];

        printf("%-40s %8.3f ms\n",
               ecs_get_name(world, e->system),
               e->ms);
    }
}

#endif
