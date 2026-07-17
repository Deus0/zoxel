#define zoxc_ushort(name) zoxc(name, ushort)

#define zoxd_ushort(name)\
    zoxd(name)\
    entity_array_d_add(component_ids_ushort, ecs_id(name));

#define zox_component_string_ushort(component) \
    " u [%u]", component->value

zox_base_type(ushort)

#define zoxd_ushort_dest(name)\
    zoxd_dest(name)\
    entity_array_d_add(component_ids_ushort, ecs_id(name));
