#define zox_set_enabled_id(e, b) ecs_enable(world, e, b)

#define zox_set_enabled(T, enable) \
    zox_set_enabled_id(zox_id(T), enable)

#define zox_disable(e) zox_set_enabled_id(e, 0)
#define zox_enable(e) zox_set_enabled_id(e, 1)

#define zox_is_enabled(e)   (!ecs_has_id(world, e, EcsDisabled))
#define zox_is_disabled(e)  (ecs_has_id(world, e, EcsDisabled))
