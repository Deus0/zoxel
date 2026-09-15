#define zoxc_int3(T) \
    zoxc(T, int3)

#define zoxd_int3(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_int3, ecs_id(T)); \
    zoxd_reflect_3(T, ecs_i32_t)

#define zox_component_string_int3(component) \
    " i3 [%ix%ix%i]", component->value.x, component->value.y, component->value.z

zox_base_type(int3)
