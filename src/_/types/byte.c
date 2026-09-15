#define zoxc_byte(T) \
    zoxc(T, byte)

#define zoxd_byte(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_byte, zox_id(T));\
    zoxd_reflect_1(T, ecs_u8_t)


#define zox_component_string_byte(component) \
    " b [%i]", \
    component->value

zox_base_type(byte)