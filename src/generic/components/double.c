#define zoxc_double(name) \
    zoxc(name, double)

#define zoxd_double(name)\
zoxd(name)\
add_to_entity_array_d(component_ids_double, ecs_id(name));

#define zox_component_string_double(component) " d [%.2lf]", component->value

zox_base_type(double)