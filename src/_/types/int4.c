#define zoxc_int4(T) \
     zoxc(T, int4)

#define zoxd_int4(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_int4, ecs_id(T)); \
    zoxd_reflect_4(T, ecs_i32_t)

#define zox_component_string_int4(component) \
    " i4 [%ix%ix%ix%i]", \
    component->value.x, \
    component->value.y, \
    component->value.z, \
    component->value.w

zox_base_type(int4)
