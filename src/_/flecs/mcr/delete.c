#ifdef zox_debug

    #define zox_delete(e) {\
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
