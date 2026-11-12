// Observers are timed while profiling (--profile, tsts that profile): each registers through a
//  wrapper that calls it, and adds up its time and calls (zox_observer_timings)
typedef struct {
    ecs_iter_action_t callback;
    const char* name;
    double total_ms;
    uint calls;
} ZoxObserverTiming;

#define zox_observer_timings_max 512
ZoxObserverTiming zox_observer_timings[zox_observer_timings_max];
int zox_observer_timings_count = 0;
byte zox_time_observers = 0;

static inline double zox_observer_now_ms() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec * 1000.0 + now.tv_nsec / 1000000.0;
}

static void zox_observer_timed(ecs_iter_t* it) {
    ZoxObserverTiming* timing = (ZoxObserverTiming*) it->callback_ctx;
    if (!zox_time_observers) {
        timing->callback(it);
        return;
    }
    const double start = zox_observer_now_ms();
    timing->callback(it);
    timing->total_ms += zox_observer_now_ms() - start;
    timing->calls++;
}

void zox_observer_internal(
    ecs* world,
    const char* name,
    ecs_iter_action_t callback,
    ecs_entity_t event,
    const char* expr,
    const char* file,
    int line,
    ...);

#define zox_observe_expr(function, tag, _expr) {\
    zox_observer_internal(world, #function, function, tag, _expr, __FILE__, __LINE__);\
}

#define zox_observe(function, tag, ...)\
    zox_observe_expr(function, tag, #__VA_ARGS__)



void zox_observer_internal(
    ecs* world,
    const char* name,
    ecs_iter_action_t callback,
    ecs_entity_t event,
    const char* expr,
    const char* file,
    int line,
    ...)
{
    ecs_observer_desc_t desc = {
        .query = {
            .expr = expr
        },
        .callback = callback,
        .events = { event }
    };
    if (zox_observer_timings_count < zox_observer_timings_max) {
        ZoxObserverTiming* timing = &zox_observer_timings[zox_observer_timings_count++];
        *timing = (ZoxObserverTiming) { .callback = callback, .name = name };
        desc.callback = zox_observer_timed;
        desc.callback_ctx = timing;
    }
    const entity e = ecs_observer_init(world, &desc);
    if (!e) {
        zox_loge(
            "[ecs_observer_init] Failed [%s] at %s:%d",
            name,
            file,
            line);
        return;
    }
    zox_set_name(e, name);
    if (zox_dbg_observers) {
        zox_log(
            "[Observer] %s at %s:%d",
            name,
            file,
            line);
    }
}

#define zox_observer(callback, event, ...) \
    zox_observer_internal( \
        world, \
        #callback, \
        callback, \
        event, \
        #__VA_ARGS__, \
        __FILE__, \
        __LINE__)

#define zox_on_add(callback, ...) \
    zox_observer( \
        callback, \
        EcsOnAdd, \
        __VA_ARGS__)
