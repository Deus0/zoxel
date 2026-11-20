
#define zox_render2D_system(name, ...) {\
    zox_system_1(name, 0, __VA_ARGS__)\
    add_to_render2D_loop(ecs_id(name));\
}

#define zox_render3_system(order, T,...) {\
    zox_system_1(T, 0, __VA_ARGS__)\
    add_to_render3D_loop(ecs_id(T));\
    zox_set(zox_id(T), RenderOrder, { order });\
}

// a render3 system whose entities can be drawn in parts on threads (RenderParallel, vulkan): multi threaded, so
// flecs splits a part's entities (zox_run_part); still run by the camera loop, not the pipeline (phase 0)
#define zox_render3_system_parallel(order, T,...) {\
    zox_system(T, 0, __VA_ARGS__)\
    add_to_render3D_loop(ecs_id(T));\
    zox_set(zox_id(T), RenderOrder, { order });\
    zox_add(zox_id(T), RenderParallel);\
}
