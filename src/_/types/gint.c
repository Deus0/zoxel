#define zoxc_gint(name) \
    zoxc(name, gint)

#define zoxd_gint(name)\
    zoxd(name)\
    entity_array_d_add(component_ids_gint, ecs_id(name));

#define zox_component_string_gint(component) " gi [%i]", component->value

zox_base_type(gint)

#define zoxd_gint_dest(name)\
    zoxd_dest_old(name)\
    entity_array_d_add(component_ids_gint, ecs_id(name));
