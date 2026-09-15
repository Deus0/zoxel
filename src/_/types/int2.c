#define zoxc_int2(T) \
    zoxc(T, int2)

#define zoxd_int2(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_int2, ecs_id(T)); \
    zoxd_reflect_2(T, ecs_i32_t)

#define zox_component_string_int2(component) \
    " i2 [%ix%i]", component->value.x, component->value.y

zox_base_type(int2)
