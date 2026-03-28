#define zoxc_uint(name) \
    zoxc(name, uint)

#define zoxd_uint(name)\
    zoxd(name)\
    add_to_entity_array_d(component_ids_uint, ecs_id(name));

#define zox_component_string_uint(component) \
    " u [%u]", component->value

zox_base_type(uint)

#define zoxd_uint_dest(name)\
    zoxd_dest(name)\
    add_to_entity_array_d(component_ids_uint, ecs_id(name));
