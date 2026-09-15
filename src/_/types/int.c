#define zoxc_int(T) \
    zoxc(T, int)

#define zoxd_int(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_int, ecs_id(T)); \
    zoxd_reflect_1(T, ecs_i32_t)

#define zox_component_string_int(component) \
    " i [%i]", component->value

zox_base_type(int)
