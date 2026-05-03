#define zoxc_arrayd_with_remove(T, type)\
    zoxc_arrayd(T, type)\
    zoxc_arrayd_removes(T, type)

#define zoxc_entities(T)\
    zoxc_arrayd_with_remove(T, entity)\
    \
    void dispose2_##T(ecs *world, const T* component) { \
        \
        if (!component->value || !component->length) {\
            /*zox_log("Cannot dispose, component empty %s", #T);*/\
            return;\
        }\
        \
        /*zox_log("Disposing of entities [%i] %s", component->length, #T);*/\
        for (int j = 0; j < component->length; j++) {\
            zox_delete_safe(component->value[j])\
        }\
    }\
    \
    void on_destroyed_##T(iter *it) {\
        zox_sys_world();\
        zox_sys_begin();\
        zox_sys_in(T);\
        for (int i = 0; i < it->count; i++) {\
            zox_sys_i(T, component);\
            if (component->value) {\
                dispose2_##T(world, component);\
            }\
        }\
    }\
    \
    byte is_in_##T(T *component, entity data) {\
        \
        if (!component || !component->value) {\
            return 0;\
        }\
        \
        for (int i = 0; i < component->length; i++) {\
            if (component->value[i] == data) {\
                return 1;\
            }\
        }\
        return 0;\
    }\
    \
    byte add_unique_to_##T(T *component, entity data) { \
        \
        if (!is_in_##T(component, data)) {\
            return add_to_##T(component, data);\
        } else {\
            return 0;\
        }\
    }
