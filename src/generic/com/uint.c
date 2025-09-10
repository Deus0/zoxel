#define zoxc_uint(name) \
    zoxc(name, uint)

#define zoxd_uint(name)\
    zoxd(name)\
    add_to_entity_array_d(component_ids_uint, ecs_id(name));

#define zox_component_string_uint(component) \
    " i [%i]", component->value

zox_base_type(uint)
