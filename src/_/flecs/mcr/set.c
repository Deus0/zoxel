#define zox_set_data(e, T, data) ecs_set_id(world, e, ecs_id(T), sizeof(T), &data);

#ifdef zox_debug
    #define zox_set(e, T, ...) {\
        if (!zox_valid(e)) zox_loge("Invalid E in zox_set [%s]", zox_getn(e)); \
        assert(zox_valid(e) && zox_alive(e)); \
        ecs_set_id(world, e, ecs_id(T), sizeof(T), &(T)__VA_ARGS__); \
    }
#else

    #define zox_set(e, T, ...) ecs_set_id(world, e, ecs_id(T), sizeof(T), &(T)__VA_ARGS__); \

#endif

#define zox_setv(e, T, v) \
    zox_set(e, T, { v })

#define zox_set_ptr(e, T, c) \
    ecs_set_ptr(world, e, T, &c)
