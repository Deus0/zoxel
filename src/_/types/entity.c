#define zoxc_entity(T)\
    zoxc(T, entity)

#define zoxd_entity(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_entity, ecs_id(T)); \
    zoxd_reflect_1(T, ecs_entity_t)

#define zox_component_string_entity(c) \
    " e [%s]", zox_valid(c->value) ? zox_get_name(c->value) : "0"

zox_base_type(entity)

// add to tilemap link
#define zox_component_parented(T) zoxc_entity(T)\
    void on_destroyed_##T(iter *it) {\
        zox_sys_world()\
        zox_sys_begin()\
        zox_sys_out(T)\
        for (int i = 0; i < it->count; i++) {\
            zox_sys_o(T, component)\
            zox_delete(component->value);\
            component->value = 0;\
        }\
    }
