#define zoxc_guint(T) \
    zoxc_dest(T, guint)

#define zoxd_guint(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_guint, ecs_id(T)); \
    zoxd_reflect_1(T, ecs_u32_t)

#define zoxd_guint_dest(T)\
    zoxd_dest(T)\
    entity_array_d_add(component_ids_guint, ecs_id(T)); \
    zoxd_reflect_1(T, ecs_u32_t)

#define zox_component_string_guint(component) \
    " gu [%u]", \
    component->value

zox_base_type(guint)

#define zoxd_guint_dest_old(T)\
    zoxd_dest_old(T)\
    entity_array_d_add(component_ids_guint, ecs_id(T));
