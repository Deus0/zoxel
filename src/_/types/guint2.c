#define zoxc_guint2(T) \
    zoxc(T, guint2)

#define zoxd_guint2(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_guint2, ecs_id(T)); \
    zoxd_reflect_2(T, ecs_u32_t)

#define zoxd_guint2_dest(T)\
    zoxd_dest(T)\
    entity_array_d_add(component_ids_guint2, ecs_id(T)); \
    zoxd_reflect_2(T, ecs_u32_t)

#define zox_component_string_guint2(component) \
    " gu2 [%ux%u]", \
    component->value.x, \
    component->value.y

zox_base_type(guint2)

#define zoxd_guint2_dest_old(T)\
    zoxd_dest_old(T)\
    entity_array_d_add(component_ids_guint2, ecs_id(T));

