#define zoxc_color_rgb(T) \
    zoxc(T, color_rgb)

#define zoxd_color_rgb(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_color_rgb, ecs_id(T));\
    zoxd_reflect_3(T, ecs_u8_t)

#define zox_component_string_color_rgb(component)\
    " c[%i.%i.%i]", \
    component->value.r, \
    component->value.g, \
    component->value.b

zox_base_type(color_rgb)
