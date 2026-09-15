#define zoxc_ulong(T) \
    zoxc(T, ulong)

#define zoxd_ulong(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_ulong, ecs_id(T)); \
    zoxd_reflect_1(T, ecs_u64_t)

#define zox_component_string_ulong(component) \
    " u [%lu]", \
    component->value

zox_base_type(ulong)
