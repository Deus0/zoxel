#define zoxc_ulong(name) \
    zoxc(name, ulong)

#define zoxd_ulong(name)\
    zoxd(name)\
    entity_array_d_add(component_ids_ulong, ecs_id(name));

#define zox_component_string_ulong(component) \
    " u [%lu]", component->value

zox_base_type(ulong)
