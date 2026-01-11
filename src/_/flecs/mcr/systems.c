// Timing Systems

#define zox_max_systems 512
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

/*

Example System T:

zox_sys(T) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in();
    // iterate
} zox_sys_end(T);

*/

#define zox_sys2(T)\
    void T(iter *it) {\
        double system_time_begin = get_time_ms();

#define zox_sys_untimed(T)\
    void T(iter *it) {

#define zox_sys_end(T)\
    double system_delta_time = get_time_ms() - system_time_begin;\
    double current_system_delta_time = ecs_get(it->world, it->system, SystemDelta)->value; \
    if (system_delta_time > current_system_delta_time) { \
        ecs_set(it->world, it->system, SystemDelta, { system_delta_time }); \
    } \
} ECS_SYSTEM_DECLARE(T)

#define zox_sys_end_untimed(T)\
    } ECS_SYSTEM_DECLARE(T)

#define zox_sys(T)\
    void T(iter *it) {

#define zoxd_system(T)\
    ECS_SYSTEM_DECLARE(T);

#define zoxd_system2(T)\
    ECS_SYSTEM_DECLARE(T)

#if defined(zox_enable_log_new_system) && !defined(zox_disable_logs)
    #define zox_log_new_system(msg, ...) zox_log(msg, ##__VA_ARGS__)
#else
    #define zox_log_new_system(msg, ...) { }
#endif

#ifdef zox_flecs_4

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

#else

    #define zox_system_internal(id_, phase, multi_threaded_, ctx_, ...) { \
        ecs_entity_desc_t edesc = {0}; \
        edesc.id = ecs_id(id_);\
        edesc.name = #id_;\
        edesc.add[0] = ((phase) ? ecs_pair(EcsDependsOn, (phase)) : 0);\
        edesc.add[1] = (phase);\
        ecs_system_desc_t desc = {0}; \
        desc.entity = ecs_entity_init(world, &edesc);\
        desc.query.filter.expr = #__VA_ARGS__; \
        desc.callback = id_; \
        desc.multi_threaded = multi_threaded_;\
        desc.ctx = ctx_;\
        ecs_id(id_) = ecs_system_init(world, &desc); \
        zox_system_on_new(world, zox_id(id_));\
    };

#endif

#define zox_system(id_, phase, ...)\
    zox_system_internal(id_, phase, 1, 0, __VA_ARGS__)

#define zox_system_1(id_, phase, ...)\
    zox_system_internal(id_, phase, 0, 0, __VA_ARGS__)

#define zox_system_m(id_, multi_threaded_, ...)\
    zox_system_internal(id_, EcsOnUpdate, multi_threaded_, 0, __VA_ARGS__)
