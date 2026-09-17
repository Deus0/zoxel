#define zox_observe_expr(function, tag, _expr) {\
    ecs_observer_desc_t desc = (ecs_observer_desc_t) {\
        .query = { .expr = _expr },\
        .callback = function,\
        .events = { tag },\
    };\
    ecs_observer_init(world, &desc);\
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
    zox_log(
        "[Observer] %s at %s:%d",
        name,
        file,
        line);
}

#define zox_observer(T, callback, event, ...) \
    zox_observer_internal( \
        world, \
        #T, \
        callback, \
        event, \
        #__VA_ARGS__, \
        __FILE__, \
        __LINE__)

#define zox_on_add(T, callback, ...) \
    zox_observer( \
        T, \
        callback, \
        EcsOnAdd, \
        __VA_ARGS__)
