#define zoxc_gint(T) \
    zoxc(T, gint)

#define zoxd_gint(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_gint, ecs_id(T)); \
    zoxd_reflect_1(T, ecs_i32_t)

#define zoxd_gint_dest(T)\
    zoxd_dest_old(T)\
    entity_array_d_add(component_ids_gint, ecs_id(T)); \
    zoxd_reflect_1(T, ecs_i32_t)

#define zox_component_string_gint(component) \
    " gi [%i]", \
    component->value

zox_base_type(gint)
