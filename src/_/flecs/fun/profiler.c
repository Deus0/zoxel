void initialize_flecs_profiler(ecs* world) {
    //if (is_using_flecs_stats) {
/*#ifdef zox_use_flecs_profiler
        zox_log("Flecs Profiler Enabled");
        zox_import_module(FlecsStats);

        //ECS_IMPORT(world, FlecsMonitor);
        // ecs_tracing_enable(1);
        // ecs_log_set_level(0);
#else
    zox_logw("Profiler Define Missing.");
#endif*/
}

#ifdef FLECS_PROFILER

#define DEBUG_MAX_SYSTEMS 4096

typedef struct {
    ecs_entity_t system;
    ecs_time_t start;
    double ms;
} ecs_debug_profile_entry_t;

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
    e->start = (ecs_time_t){0};
    e->ms = 0;

    return e;
}

static void debug_profiler_begin(
    ecs_world_t *world,
    ecs_entity_t system)
{
    (void)world;

    ecs_debug_profile_entry_t *e = debug_find_system(system);

    if (e) {
        ecs_os_get_time(&e->start);
    }
}

static void debug_profiler_end(
    ecs_world_t *world,
    ecs_entity_t system)
{
    (void)world;

    ecs_debug_profile_entry_t *e = debug_find_system(system);

    if (!e) {
        return;
    }

    ecs_time_t end;
    ecs_os_get_time(&end);

    e->ms =
        (ecs_time_to_double(end) -
         ecs_time_to_double(e->start))
        * 1000.0;
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
