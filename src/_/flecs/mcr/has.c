#define zox_has(e, T) ecs_has(world, e, T)

#define zox_alive(e) (e && ecs_is_alive(world, e))

// NOTE: Checks whether id is inside a ecs world
#define zox_valid(e) (e && ecs_is_valid(world, e))

// It used to check if is alive too
// #define zox_valid(e) (e && ecs_is_valid(world, e) && ecs_is_alive(world, e))
