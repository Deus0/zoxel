#define zoxc_dest(name, T)\
    zoxc(name, T)

#define zoxd_dest(T)\
    zoxd(T);\
    zox_observe(on_destroyed_##T, EcsOnDelete, [out] T)

#define zoxd_dest_old(T)\
    zoxd(T);\
    ecs_set_hooks(world, T, { .dtor = ecs_dtor(T) });

