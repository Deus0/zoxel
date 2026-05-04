#define zoxc_userdata(T) \
    zox_tag(T); \
    zoxc_entities(T##Links) \
    zoxc_entity(T##Link)

#define zoxd_userdata(T) \
    zoxd_tag(T); \
    zoxd_entities(T##Links); \
    zoxd_entity(T##Link)

// macro for creating prefab data for user data
#define zoxf_user_prefabs(T, name, label)\
    entity prefab_##name;\
    \
    entity spawn_prefab_##name(ecs *world) {\
        zox_prefab_child(prefab_userd); \
        zox_prefab_name(label); \
        zox_add_tag(e, T); \
        prefab_##name = e; \
        return e; \
    }\
    \
    /* generic meta spawn function*/\
    entity spawn_meta_##name(ecs *world, entity prefab, const char *name) {\
        zox_prefab_child(prefab); \
        zox_set_unique_name(e, name); \
        set_ZoxName(world, e, name); \
        return e;\
    }\
    \
    entity spawn_user_##name(ecs *world, entity prefab, entity user) {\
        entity e = zox_ins(prefab); \
        const char* meta_name = zox_get_name(prefab); \
        zox_set_unique_name(e, meta_name); \
        zox_set(e, UserLink, { user }); \
        return e; \
    }
