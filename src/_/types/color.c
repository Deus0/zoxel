#define zoxc_color(name) \
    zoxc(name, color)

#define zoxd_color(name)\
    zoxd(name)\
    add_to_entity_array_d(component_ids_color, ecs_id(name));

#define zox_component_string_color(component) \
    " c[%i.%i.%i.%i]", component->value.r, component->value.g, component->value.b, component->value.a

zox_base_type(color)
