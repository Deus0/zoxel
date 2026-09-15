#define zoxc_byte4(T)\
    zoxc(T, byte4)

#define zoxd_byte4(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_byte4, ecs_id(T));\
    zoxd_reflect_4(T, ecs_u8_t)

#define zox_component_string_byte4(component)\
    " b4 [%ix%ix%ix%i]", \
    component->value.x, \
    component->value.y, \
    component->value.z, \
    component->value.w

zox_base_type(byte4)
