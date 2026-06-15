#define zox_base_type(T)\
    entity_array_d* component_ids_##T;\
    \
    void initialize_component_ids_##T() {\
        component_ids_##T = create_entity_array_d(initial_dynamic_array_size);\
    }\
    \
    void dispose_component_ids_##T() {\
        dispose_entity_array_d(component_ids_##T);\
    }\
    \
    byte is_component_type_##T(entity id) {\
        return entity_array_d_has(component_ids_##T, id);\
    } \
    \
    zoxc(stub_##T, T); \
    \
    uint get_type_label_##T(ecs* world, entity e, entity id, char *b, uint s, uint i) {\
        \
        const stub_##T* c = ((const stub_##T*) ecs_get_id(world, e, id));\
        \
        if (c) { \
            i += snprintf(b + i, s - i, zox_component_string_##T(c));\
        } else { \
            i += snprintf(b + i, s - i, "!");\
        } \
        \
        return i; \
    }
