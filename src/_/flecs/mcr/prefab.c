#define zox_make_prefab(e)\
    ecs_add_id(world, e, EcsPrefab);

#define zox_prefab()\
    zox_make_new()\
    zox_make_prefab(e)

// flecs 4 overrides by default
#define zox_prefab_add(e, T) ecs_add(world, e, T);


#define zox_prefab_set(e, T, ...) {\
    zox_prefab_add(e, T) \
    ecs_set_id(world, e, ecs_id(T), sizeof(T), &(T)__VA_ARGS__); \
}

#define zox_prefab_set_ptr(e, T, c) \
    zox_prefab_add(e, T) \
    ecs_set_ptr(world, e, T, &c);

// Retrieve the prefab (parent) from the entity
entity zox_get_prefab(ecs* world, entity e) {
    return ecs_get_target(world, e, EcsIsA, 0);
}

#define zox_getp(world, e)\
    ecs_get_target(world, e, EcsIsA, 0)
