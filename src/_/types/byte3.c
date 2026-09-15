#define zoxc_byte3(T)\
    zoxc(T, byte3)

#define zoxd_byte3(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_byte3, ecs_id(T));\
    zoxd_reflect_3(T, ecs_u8_t)

#define zox_component_string_byte3(component)\
    " b3 [%ix%ix%i]", \
    component->value.x, \
    component->value.y, \
    component->value.z

zox_base_type(byte3)
