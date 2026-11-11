#ifdef zox_debug

    #define zox_delete(e) {\
        if (!zox_valid(e)) zox_loge("Invalid E in zox_delete [%s]", zox_getn(e)); \
        assert(zox_valid(e) && zox_alive(e)); \
        ecs_delete(world, e); \
    }

#else

    #define zox_delete(e) {\
        ecs_delete(world, e); \
    }

#endif

#define zox_delete_safe(e)\
    if (zox_valid(e)) {\
        zox_delete(e)\
    }

#define zox_delete_and_set(e)\
    if (e) {\
        zox_delete(e)\
        e = 0;\
    }

// Queues changes until zox_defer_end, like inside a system (tests use it to batch like the game does)
//  - world becomes its first stage, so zox_ calls in between are queued, zox_defer_end merges them
#define zox_defer_begin()\
    ecs* zox_defer_world = world;\
    ecs_readonly_begin(zox_defer_world, false);\
    world = ecs_get_stage(zox_defer_world, 0)

#define zox_defer_end()\
    world = zox_defer_world;\
    ecs_readonly_end(world)
