#define zoxc_uint2(name) \
    zoxc(name, uint2)

#define zoxd_uint2(name)\
    zoxd(name)\
    entity_array_d_add(component_ids_uint2, ecs_id(name));

#define zox_component_string_uint2(component) \
    " u2 [%ux%u]", component->value.x, component->value.y

zox_base_type(uint2)

#define zoxd_uint2_dest(name)\
    zoxd_dest(name)\
    entity_array_d_add(component_ids_uint2, ecs_id(name));
