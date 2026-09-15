#define zoxc_uint2(T) \
    zoxc(name, uint2)

#define zoxd_uint2(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_uint2, ecs_id(T)); \
    zoxd_reflect_2(T, ecs_u32_t)

#define zox_component_string_uint2(component) \
    " u2 [%ux%u]", \
    component->value.x, \
    component->value.y

zox_base_type(uint2)

#define zoxd_uint2_dest(T)\
    zoxd_dest_old(T)\
    entity_array_d_add(component_ids_uint2, ecs_id(T));
