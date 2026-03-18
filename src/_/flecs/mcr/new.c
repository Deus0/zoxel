#ifdef zox_flecs_4

    #define zox_new()\
        ecs_new(world)

#else

    #define zox_new()\
        ecs_new(world, 0)

#endif

#define zox_make_new()\
    entity e = zox_new();

#define zox_make_neww(name)\
    entity name = zox_new();

