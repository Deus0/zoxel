#define zoxc_userdata(T) \
    zox_tag(T); \
    zoxc_entities(T##Links) \
    zoxc_entity(T##Link)

#define zoxd_userdata(T) \
    zoxd_tag(T); \
    zox_define_entities_component(T##Links); \
    zoxd_entity(T##Link)

// macro for creating prefab data for user data
#define zoxf_user_prefabs(T, name, label)\
    entity prefab_##name;\
    \
    entity spawn_prefab_##name(ecs *world) {\
        zox_prefab(); \
        zox_prefab_name(label); \
        zox_add_tag(e, T); \
        zox_prefab_add(e, ZoxName); \
        zox_prefab_set(e, UserLink, { 0 }); \
        zox_prefab_set(e, TextureLink, { 0 }); \
        zox_prefab_set(e, ActivateBegin, { 0 }); \
        zox_prefab_set(e, Activate, { 0 }); \
        zox_prefab_set(e, UserDataDirty, { 0 }); \
        prefab_##name = e; \
        return e; \
    }\
    \
    /* generic meta spawn function*/\
    entity spawn_meta_##name( \
        ecs *world, \
        const entity prefab, \
        const char *name \
    ) {\
        zox_prefab_child(prefab); \
        zox_set_name(e, name); \
        ZoxName zname = (ZoxName) { 0 };\
        set_zox_name(&zname, name); \
        zox_set_ptr(e, ZoxName, zname); \
        return e;\
    }\
    \
    entity spawn_meta_##name##_zox_name( \
        ecs *world, \
        const entity prefab, \
        const ZoxName *name \
    ) {\
        zox_prefab_child(prefab); \
        zox_name("meta_"label); \
        ZoxName *zoxName = &((ZoxName) { 0, NULL }); \
        clone_ZoxName(zoxName, name); \
        zox_set(e, ZoxName, { zoxName->length, zoxName->value }); \
        return e;\
    }\
    \
    entity spawn_user_##name( \
        ecs *world, \
        entity prefab, \
        entity user \
    ) {\
        zox_instance(prefab); \
        zox_name(zox_get_name(prefab)); \
        zox_set(e, UserLink, { user }); \
        return e; \
    }
