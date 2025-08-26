#define zoxc_float2(name) \
    zoxc(name, float2)

#define zoxd_float2(name)\
    zoxd(name)\
    add_to_entity_array_d(component_ids_float2, ecs_id(name));

#define zox_component_string_float2(component) \
    " f2 [%.2fx%.2f]", component->value.x, component->value.y

zox_base_type(float2)
