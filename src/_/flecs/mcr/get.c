#define zox_get(e, T) \
    ecs_get(world, e, T)

#define zox_getv(e, T) \
    ecs_get(world, e, T)->value

#define zox_getl(e, T)\
    ecs_get(world, e, T)->length

#define zox_get_path(e)\
    ecs_get_path_w_sep(world, 0, e, ".", NULL)

