// Timing Systems

#define zoxd_system(T) \
    ECS_SYSTEM_DECLARE(T)

#define zox_max_systems 1024
entity zox_systems[zox_max_systems];
int zox_systems_count = 0;

void zox_system_on_new(ecs* world, entity system) {
    if (zox_systems_count == zox_max_systems) {
        zox_loge("Max Systems hit, increase limit [%i]", zox_systems_count);
        return;
    }
    zox_systems[zox_systems_count++] = system;
    // zox_log_new_system("+ new system [%s]", #id_);
    // zox_statistics_systems++;
}

#ifdef zox_time_systems

#define zox_sys_on_begin()\
    uint process_count = 0; \
    double system_time_begin = get_time_ms(); \
    byte is_count_process = ecs_has(\
        it->world,\
        it->system,\
        SystemProcessed)\

#define zox_sys_on_end()\
    double system_delta_time = get_time_ms() - system_time_begin;\
    if (ecs_has(it->world, it->system, SystemDelta)) { \
        double current_delta = ecs_get(it->world, it->system, SystemDelta)->value; \
        if (system_delta_time > current_delta) { \
            SystemDelta* system_delta = ecs_get_mut(\
                it->world, it->system, SystemDelta); \
            system_delta->value = system_delta_time; \
        } \
    } \
    \
    if (is_count_process && process_count) { \
        int current = ecs_get(it->world, it->system, SystemProcessed)->value;\
        if (process_count > current) {\
            SystemProcessed* system_processed = ecs_get_mut(\
                it->world, it->system, SystemProcessed); \
            system_processed->value = process_count; \
            /* ecs_set(it->world, it->system, SystemProcessed, { process_count }); */ \
        }\
    }

#else

#define zox_sys_on_begin(T)\
    uint process_count = 0;

#define zox_sys_on_end(T)\
    (void) process_count

#endif

#define zox_sys_increment()\
    process_count++

// A time budget per worker thread per frame, for heavy systems (better than counting processed entities,
//  two heavy entities were still a lag spike):
//  - each item adds its time, once a thread spent its budget this frame it leaves the rest for next frame
//  - a thread always does at least one item a frame, so work never stalls
//  - zox_sys_budget(name, ms) at the top, zox_sys_budget_spent(name) to skip, start/end around each item
#define zox_sys_budget(name, ms) \
    static __thread uint name##_budget_frame = UINT32_MAX; \
    static __thread double name##_budget_spent = 0; \
    if (name##_budget_frame != ecs_run_count) { \
        name##_budget_frame = ecs_run_count; \
        name##_budget_spent = 0; \
    } \
    const double name##_budget_ms = (ms); \
    double name##_budget_start = 0

#define zox_sys_budget_spent(name) \
    (name##_budget_spent >= name##_budget_ms)

#define zox_sys_budget_start(name) \
    name##_budget_start = get_time_ms()

#define zox_sys_budget_end(name) \
    name##_budget_spent += get_time_ms() - name##_budget_start

#define zox_sys2(T)\
    void T(iter *it) {\
        zox_sys_on_begin();

#define zox_sys_end(T)\
    zox_sys_on_end(); \
} zoxd_system(T);

#define zox_sys_untimed(T)\
    void T(iter *it) {

#define zox_sys_end_untimed(T)\
    } zoxd_system(T)

#define zox_sys(T)\
    void T(iter *it) {
static inline entity zox_system_entity(ecs* world, entity id, const char* name, entity phase) {
    ecs_entity_desc_t edesc = {0};
    edesc.id = id;
    edesc.name = name;
    entity e = ecs_entity_init(world, &edesc);
    if (phase) {
        ecs_add_pair(world, e, EcsDependsOn, phase);
        ecs_add_id(world, e, phase);
    }
    return e;
}

#define zox_system_internal(id_, phase, multi_threaded_, ctx_, ...) { \
    ecs_system_desc_t desc = {0}; \
    desc.entity = zox_system_entity(world, ecs_id(id_), #id_, (phase)); \
    desc.query.expr = #__VA_ARGS__; \
    desc.callback = id_; \
    desc.multi_threaded = multi_threaded_; \
    desc.ctx = ctx_; \
    ecs_id(id_) = ecs_system_init(world, &desc); \
    zox_system_on_new(world, zox_id(id_)); \
};

#define zox_system_cached(id_, phase, ...) { \
    ecs_system_desc_t desc = {0}; \
    desc.entity = zox_system_entity(world, ecs_id(id_), #id_, (phase)); \
    desc.query.expr = #__VA_ARGS__; \
    desc.query.cache_kind = EcsQueryCacheAll; \
    desc.callback = id_; \
    desc.multi_threaded = 1; \
    desc.ctx = 0; \
    ecs_id(id_) = ecs_system_init(world, &desc); \
    zox_system_on_new(world, zox_id(id_)); \
}

#define zox_system(id_, phase, ...)\
    zox_system_internal(id_, phase, 1, 0, __VA_ARGS__)

#define zox_system_1(id_, phase, ...)\
    zox_system_internal(id_, phase, 0, 0, __VA_ARGS__)

// a single threaded system that isn't deferred: its changes happen as it makes them, so it can bulk spawn
//  (zox_bulk_spawn), flecs merges everything before it runs (a sync point)
//  - use zox_sys_world2() in it (the world), zox_sys_world() gives a stage, bulk spawning asserts on it
//  - careful: its sync point changes ordering, one in zoxp_spawn stopped the pause menu opening
#define zox_system_immediate(id_, phase, ...) { \
    ecs_system_desc_t desc = {0}; \
    desc.entity = zox_system_entity(world, ecs_id(id_), #id_, (phase)); \
    desc.query.expr = #__VA_ARGS__; \
    desc.callback = id_; \
    desc.immediate = true; \
    ecs_id(id_) = ecs_system_init(world, &desc); \
    zox_system_on_new(world, zox_id(id_)); \
}

#define zox_system_m(id_, multi_threaded_, ...)\
    zox_system_internal(id_, zoxp_update, multi_threaded_, 0, __VA_ARGS__)
