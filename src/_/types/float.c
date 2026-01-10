#define zoxc_float(name) \
    zoxc(name, float)

#define zoxd_float(name)\
    zoxd(name)\
    add_to_entity_array_d(component_ids_float, ecs_id(name));

#define zox_component_string_float(component) \
    " f [%.2f]", component->value

zox_base_type(float)
