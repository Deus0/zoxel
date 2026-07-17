#define zox_disable(e) ecs_enable(world, e, 0)

#define zox_enable(e) ecs_enable(world, e, 1)

#define zox_is_enabled(e) ecs_is_enabled(world, e)
