// NOTE: Creates a prefab child of another to build on
static inline entity zox_prefab_from_parent(ecs* world, entity parent) {
    entity e = ecs_new_w_pair(world, EcsIsA, parent);
    ecs_add_id(world, e, EcsPrefab);
    return e;
    // entity e = ecs_new(world);
    // ecs_add_pair(world, e, EcsIsA, parent);     // inherit components
    // ecs_add_pair(world, e, EcsChildOf, parent); // make it a child
}

#define zox_make_prefab(e)\
    ecs_add_id(world, e, EcsPrefab);

#define zox_prefab()\
    entity e = zox_new();\
    zox_make_prefab(e)

// flecs 4 overrides by default
#define zox_prefab_add(e, T) ecs_add(world, e, T)

#define zox_prefab_setv(e, T, v) {\
    zox_prefab_add(e, T); \
    zox_set(e, T, { v }); \
}

#define zox_prefab_set(e, T, ...) {\
    zox_prefab_add(e, T); \
    ecs_set_id(world, e, ecs_id(T), sizeof(T), &(T)__VA_ARGS__); \
}

#define zox_prefab_set_ptr(e, T, c) \
    zox_prefab_add(e, T); \
    ecs_set_ptr(world, e, T, &c);

// Retrieve the prefab (parent) from the entity
entity zox_get_prefab(ecs* world, entity e) {
    return ecs_get_target(world, e, EcsIsA, 0);
}

#define zox_getp(world, e)\
    ecs_get_target(world, e, EcsIsA, 0)

#define zox_is_prefab(e)\
    ecs_has_id(world, e, EcsPrefab)
