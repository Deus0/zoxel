#define zox_sys_query()\
    ecs_query_t* query = it->ctx;\
    if (!query) {\
        return;\
    }

#define zox_sys_query_begin()\
    iter it2 = ecs_query_iter(world, query);

#define zox_sys_query_loop()\
    ecs_query_next(&it2)

// #define zox_sys_query_end() ;


#ifdef zox_flecs_4

    #define zox_sys_query_end() \
        ;

// Seems to be called internally now?
// ecs_query_fini(query);

#else

    #define zox_sys_query_end()\
        ecs_iter_fini(&it2);

#endif
