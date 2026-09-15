#define zoxc_custom(T) \
    ECS_COMPONENT_DECLARE(T)

#define zoxc(T, type)\
    typedef struct {\
        type value;\
    } T;\
    zoxc_custom(T)

// zox_statistics_components++;
#define zoxd(T) \
    ECS_COMPONENT_DEFINE(world, T)

#define zox_event_type(name, return_type, ...)\
    typedef struct {\
        return_type (*value)(__VA_ARGS__);\
    } name;

#define zoxc_function(name, return_type, ...)\
    typedef struct { \
        return_type (*value)(__VA_ARGS__); \
    } name; \
    zoxc_custom(name)

#define zox_define_destruction(name)\
    ecs_set_hooks(world, name, { .dtor = ecs_dtor(name) });

#define zox_define_entity_parent_component2(name, ...)\
    zoxd(name)\
    zox_observe(on_destroyed##_##name, EcsOnRemove, __VA_ARGS__)

#define zox_define_entity_parent_component(name) \
    zox_define_entity_parent_component2(name, [out] name)

#define zox_debug_component(T)\
    zox_log(" + component [%s] > [%s]\n", #T, zox_get_name(T));

#define zoxc_listener(T, n, ...) \
\
typedef struct { \
    void (*fun)(__VA_ARGS__); \
} T##Entry; \
\
typedef struct { \
    T##Entry value[n]; \
    uint count; \
} T; \
\
byte add_to_##T(T* component, void (*fun)(__VA_ARGS__)) { \
    if (component->count >= n) {\
        zox_logw("Listener [%s] OOB [%i] > [%i]",\
            #T, component->count, n); \
        return 0; \
    }\
    /*zox_logw("ADD listener [%s] index [%i] ptr [%p]", \
        #T, component->count, (void *) fun);*/ \
    component->value[component->count].fun = fun; \
    component->count++; \
    return 1; \
} \
\
zoxc_custom(T)

#define zoxd_inherited(T) \
    ecs_add_pair(world, ecs_id(T), EcsOnInstantiate, EcsInherit)
