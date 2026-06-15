#define zoxc_color_rgb(name) \
    zoxc(name, color_rgb)

#define zoxd_color_rgb(name)\
    zoxd(name)\
    entity_array_d_add(component_ids_color_rgb, ecs_id(name));

#define zox_component_string_color_rgb(component)\
    " c[%i.%i.%i]", component->value.r, component->value.g, component->value.b

zox_base_type(color_rgb)
