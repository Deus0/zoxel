#define zoxc_ushort(T) \
    zoxc(T, ushort)

#define zoxd_ushort(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_ushort, ecs_id(T)); \
    zoxd_reflect_1(T, ecs_u16_t)

#define zox_component_string_ushort(component) \
    " u [%u]", \
    component->value

zox_base_type(ushort)

#define zoxd_ushort_dest(T)\
    zoxd_dest_old(T)\
    entity_array_d_add(component_ids_ushort, ecs_id(T));
