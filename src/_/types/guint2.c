#define zoxc_guint2(name) zoxc(name, guint2)

#define zoxd_guint2(name)\
    zoxd(name)\
    entity_array_d_add(component_ids_guint2, ecs_id(name));

#define zox_component_string_guint2(component) \
    " u2 [%ux%u]", component->value.x, component->value.y

zox_base_type(guint2)

#define zoxd_guint2_dest(name)\
    zoxd_dest(name)\
    entity_array_d_add(component_ids_guint2, ecs_id(name));
