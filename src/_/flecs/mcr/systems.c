// Timing Systems

#define zoxd_system(T) \
    ECS_SYSTEM_DECLARE(T)

#define zox_max_systems 1024
entity zox_systems[zox_max_systems];
int zox_systems_count = 0;

void zox_system_on_new(ecs* world, entity system) {
    if (zox_systems_count == zox_max_systems) {
        zox_log_error("Max Systems hit, increase limit [%i]", zox_systems_count);
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
            SystemDelta* system_delta = ecs_get_mut(it->world, it->system, SystemDelta); \
            system_delta->value = system_delta_time; \
        } \
    } \
    \
    if (is_count_process && process_count) { \
        int current = ecs_get(it->world, it->system, SystemProcessed)->value;\
        if (process_count > current) {\
            SystemProcessed* system_processed = ecs_get_mut(it->world, it->system, SystemProcessed); \
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

#define zox_system_internal(id_, phase, multi_threaded_, ctx_, ...) { \
    ecs_entity_desc_t edesc = {0}; \
    ecs_id_t add_ids[3] = {\
        ((phase) ? ecs_pair(EcsDependsOn, (phase)) : 0), \
        (phase), \
        0 \
    };\
    edesc.id = ecs_id(id_);\
    edesc.name = #id_;\
    edesc.add = add_ids;\
    ecs_system_desc_t desc = {0}; \
    desc.entity = ecs_entity_init(world, &edesc);\
    desc.query.expr = #__VA_ARGS__; \
    desc.callback = id_; \
    desc.multi_threaded = multi_threaded_;\
    desc.ctx = ctx_;\
    ecs_id(id_) = ecs_system_init(world, &desc); \
    zox_system_on_new(world, zox_id(id_));\
};


#define zox_system(id_, phase, ...)\
    zox_system_internal(id_, phase, 1, 0, __VA_ARGS__)

#define zox_system_1(id_, phase, ...)\
    zox_system_internal(id_, phase, 0, 0, __VA_ARGS__)

#define zox_system_m(id_, multi_threaded_, ...)\
    zox_system_internal(id_, EcsOnUpdate, multi_threaded_, 0, __VA_ARGS__)
