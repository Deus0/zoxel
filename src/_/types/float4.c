#define zoxc_float4(name) \
    zoxc(name, float4)

#define zoxd_float4(name)\
    zoxd(name)\
    add_to_entity_array_d(component_ids_float4, ecs_id(name));

#define zox_component_string_float4(component) \
    " f4 [%.1fx%.1fx%.1fx%.1f]", component->value.x, component->value.y, component->value.z, component->value.w

zox_base_type(float4)
