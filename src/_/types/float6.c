#define zoxc_float6(T) \
    zoxc(T, float6)

#define zoxd_float6(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_float6, ecs_id(T));

#define zox_component_string_float6(component) \
    " f6 [%.2fx%.2fx%.2fx%.2fx%.2fx%.2f]", \
    component->value.x, \
    component->value.y, \
    component->value.z, \
    component->value.w, \
    component->value.u, \
    component->value.v

zox_base_type(float6)
