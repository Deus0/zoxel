#define zoxc_double(T) \
    zoxc(T, double)

#define zoxd_double(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_double, ecs_id(T)); \
    zoxd_reflect_1(T, ecs_f64_t)

#define zox_component_string_double(component) \
    " d [%.2lf]", \
    component->value

zox_base_type(double)