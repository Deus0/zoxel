#define zoxc_byte2(T) \
    zoxc(T, byte2)

#define zoxd_byte2(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_byte2, ecs_id(T));\
    zoxd_reflect_2(T, ecs_u8_t)

#define zox_component_string_byte2(c) \
    " b2 [%ix%i]", \
    c->value.x, \
    c->value.y

zox_base_type(byte2)