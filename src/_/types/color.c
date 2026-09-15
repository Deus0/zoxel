#define zoxc_color(T) \
    zoxc(T, color)

#define zoxd_color(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_color, ecs_id(T));\
    zoxd_reflect_4(T, ecs_u8_t)

#define zox_component_string_color(component) \
    " c[%i.%i.%i.%i]", \
    component->value.r, \
    component->value.g, \
    component->value.b, \
    component->value.a

zox_base_type(color)
