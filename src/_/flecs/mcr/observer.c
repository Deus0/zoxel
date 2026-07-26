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
