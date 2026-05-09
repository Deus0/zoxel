#define zox_instance(prefab)\
    const entity e = ecs_new_w_pair(world, EcsIsA, prefab);

#define zox_ins(prefab)\
    ecs_new_w_pair(world, EcsIsA, prefab)

#define zox_instance_named(prefab, name)\
    const entity name = ecs_new_w_pair(world, EcsIsA, prefab);

#define zox_prefab_child(prefab)\
    zox_instance(prefab)\
    zox_make_prefab(e)

#define zox_prefab_child_named(prefab, name)\
    zox_instance_named(prefab, name)\
    zox_make_prefab(name)

#define zox_clone(prefab)\
    entity e = ecs_clone(world, 0, prefab, 1);
