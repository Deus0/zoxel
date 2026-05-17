#define zox_instance(prefab)\
    entity e = ecs_new_w_pair(world, EcsIsA, prefab);

#define zox_ins(prefab)\
    ecs_new_w_pair(world, EcsIsA, prefab)

#define zox_instance_named(prefab, name)\
    entity name = ecs_new_w_pair(world, EcsIsA, prefab);

#define zox_prefab_child(prefab)\
    zox_instance(prefab)\
    zox_make_prefab(e)

#define zox_prefab_child_named(prefab, name)\
    zox_instance_named(prefab, name)\
    zox_make_prefab(name)

#define zox_clone(prefab)\
    entity e = ecs_clone(world, 0, prefab, 1);

// NOTE: Creates a prefab child of another to build on
entity zox_prefab_from_parent(ecs* world, entity parent) {
    entity e = ecs_new_w_pair(world, EcsIsA, parent);
    ecs_add_id(world, e, EcsPrefab);
    return e;
    // entity e = ecs_new(world);
    // ecs_add_pair(world, e, EcsIsA, parent);     // inherit components
    // ecs_add_pair(world, e, EcsChildOf, parent); // make it a child
}
