#define zoxc_float3(T) \
    zoxc(T, float3)

#define zoxd_float3(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_float3, zox_id(T)); \
    zoxd_reflect_3(T, ecs_f32_t)

#define zox_component_string_float3(component) \
    " f3 [%.2fx%.2fx%.2f]", \
    component->value.x, \
    component->value.y, \
    component->value.z

zox_base_type(float3)
