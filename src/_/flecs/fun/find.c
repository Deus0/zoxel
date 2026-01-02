#define clear_memory_component(name, component)\
    dispose_##name(component);

entity find_array_element_with_id(
    ecs* world,
    entity* value,
    int length,
    entity id
) {
    for (int i = 0; i < length; i++) {
        if (zox_has_id(value[i], id)) {
            return value[i];
        }
    }
    return 0;
}

#define find_array_element_with_tag2(value, length, tag, e)\
    entity e = 0;\
    for (int i = 0; i < length; i++) {\
        if (zox_has(value[i], tag)) {\
            e = value[i];\
            break;\
        }\
    }

#define find_array_element_with_tag(component, tag, e)\
    entity e = 0;\
    for (int i = 0; i < component->length; i++) {\
        if (zox_has(component->value[i], tag)) {\
            e = component->value[i];\
            break;\
        }\
    }

#define find_array_element_with_tag_id(component, tag_id, name)\
    entity name = 0;\
    for (int i = 0; i < component->length; i++) {\
        if (zox_has_id(component->value[i], tag_id)) {\
            name = component->value[i];\
            break;\
        }\
    }
