#define zoxc_float(name) \
    zoxc(name, float)

#define zoxd_float(name)\
    zoxd(name)\
    entity_array_d_add(component_ids_float, ecs_id(name));

#define zox_component_string_float(component) \
    " f [%.2f]", component->value

zox_base_type(float)
