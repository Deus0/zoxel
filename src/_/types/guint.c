#define zoxc_guint(name) \
    zoxc(name, guint)

#define zoxd_guint(name)\
    zoxd(name)\
    entity_array_d_add(component_ids_guint, ecs_id(name));

#define zox_component_string_guint(component) \
    " u [%u]", component->value

zox_base_type(guint)

#define zoxd_guint_dest(name)\
    zoxd_dest(name)\
    entity_array_d_add(component_ids_guint, ecs_id(name));
