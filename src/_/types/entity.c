#define zoxc_entity(name)\
    zoxc(name, entity)

#define zoxd_entity(name)\
    zoxd(name)\
    entity_array_d_add(component_ids_entity, ecs_id(name));

#define zox_component_string_entity(c) \
    " e [%s]", zox_valid(c->value) ? zox_get_name(c->value) : "0"

zox_base_type(entity)

// add to tilemap link
#define zox_component_parented(name) zoxc_entity(name)\
    void on_destroyed##_##name(iter *it) {\
        zox_sys_world()\
        zox_sys_begin()\
        zox_sys_out(name)\
        for (int i = 0; i < it->count; i++) {\
            zox_sys_o(name, component)\
            zox_delete(component->value);\
            component->value = 0;\
        }\
    }
