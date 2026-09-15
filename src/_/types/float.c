#define zoxc_float(T) \
    zoxc(T, float)

#define zoxd_float(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_float, ecs_id(T)); \
    zoxd_reflect_1(T, ecs_f32_t)

#define zox_component_string_float(component) \
    " f [%.2f]", \
    component->value

zox_base_type(float)
