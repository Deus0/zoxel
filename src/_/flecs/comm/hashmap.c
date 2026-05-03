#define zoxc_hashmap(T, key)\
    \
    zoxc(T, key##_hashmap*); \
    \
    void dispose_hashmap_##key(ecs *world, key##_hashmap* hashmap) {\
        for (size_t j = 0; j < hashmap->size; j++) {\
            key##_hashmap_pair *pair = hashmap->data[j];\
            while (pair) {\
                entity e = pair->value;\
                if (zox_valid(e)) {\
                    /*zox_log("Disposing Chunk: %s", zox_get_name(e));*/ \
                    zox_delete(e); \
                }\
                pair = pair->next;\
            }\
        }\
        key##_##hashmap_dispose(hashmap);\
    }\
    \
    void on_destroyed_##T(iter *it) {\
        zox_sys_world();\
        zox_sys_begin();\
        zox_sys_out(T);\
        for (int i = 0; i < it->count; i++) {\
            zox_sys_o(T, component);\
            if (!component->value) {\
                continue; \
            } \
            \
            /*zox_log("Disposing of Hashmap Component %s [%s]", #T, #key);*/ \
            key##_hashmap* hashmap = component->value;\
            dispose_hashmap_##key(world, hashmap);\
            component->value = NULL;\
        }\
    }

#define zoxd_hashmap2(T, ...)\
    zoxd(T);\
    zox_observe(on_destroyed_##T, EcsOnRemove, __VA_ARGS__)

#define zoxd_hashmap(T)\
    zoxd_hashmap2(T, [out] T)
