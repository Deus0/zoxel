#define zox_disable(e) ecs_enable(world, e, 0)
#define zox_enable(e) ecs_enable(world, e, 1)
#define zox_set_enabled(e, b) ecs_enable(world, e, b)

#define zox_is_enabled(e)   (!ecs_has_id(world, e, EcsDisabled))
#define zox_is_disabled(e)  (ecs_has_id(world, e, EcsDisabled))
