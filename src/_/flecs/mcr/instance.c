#define zox_instance(prefab) entity e = ecs_new_w_pair(world, EcsIsA, prefab);

// #define zox_ins(prefab) ecs_new_w_pair(world, EcsIsA, prefab)

#define zox_instance_named(prefab, name)\
    entity name = ecs_new_w_pair(world, EcsIsA, prefab);

// A prefab of a prefab: made a prefab first, then its base (zox_prefab_variant), an instance made a
//  prefab afterwards didn't pass on non fragmenting children (the player's slots, tst prefab_variants)
#define zox_prefab_child(prefab)\
    entity e = zox_prefab_variant(world, prefab);

#define zox_prefab_child_named(prefab, name)\
    entity name = zox_prefab_variant(world, prefab);

#define zox_clone(prefab)\
    entity e = ecs_clone(world, 0, prefab, 1);

static inline entity zox_ins(ecs* world, entity prefab) {
    return ecs_new_w_pair(world, EcsIsA, prefab);
}
