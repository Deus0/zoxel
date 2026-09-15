#define zoxc_float2(T) \
    zoxc(T, float2)

#define zoxd_float2(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_float2, ecs_id(T)); \
    zoxd_reflect_2(T, ecs_f32_t)

#define zox_component_string_float2(component) \
    " f2 [%.2fx%.2f]", \
    component->value.x, \
    component->value.y

zox_base_type(float2)
