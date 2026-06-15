#define zoxc_int(name) \
    zoxc(name, int)

#define zoxd_int(name)\
    zoxd(name)\
    entity_array_d_add(component_ids_int, ecs_id(name));

#define zox_component_string_int(component) \
    " i [%i]", component->value

zox_base_type(int)
