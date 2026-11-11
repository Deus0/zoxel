#define zoxc_dest(name, T)\
    zoxc(name, T)

#define zoxd_dest(T)\
    zoxd(T);\
    zox_observe(on_destroyed_##T, EcsOnRemove, [out] T) /* OnRemove fires on delete too, flecs never emits EcsOnDelete (a cleanup policy) */

#define zoxd_dest_old(T)\
    zoxd(T);\
    ecs_set_hooks(world, T, { .dtor = ecs_dtor(T) });

