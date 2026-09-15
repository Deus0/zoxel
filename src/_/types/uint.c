#define zoxc_uint(T) \
    zoxc(T, uint)

#define zoxd_uint(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_uint, ecs_id(T)); \
    zoxd_reflect_1(T, ecs_u32_t)


#define zox_component_string_uint(component) \
    " u [%u]", \
    component->value

zox_base_type(uint)

#define zoxd_uint_dest(T)\
    zoxd_dest_old(T)\
    entity_array_d_add(component_ids_uint, ecs_id(T));
