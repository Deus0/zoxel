#define zoxc_int3(name) \
    zoxc(name, int3)

#define zoxd_int3(name)\
    zoxd(name)\
    entity_array_d_add(component_ids_int3, ecs_id(name));

#define zox_component_string_int3(component) \
    " i3 [%ix%ix%i]", component->value.x, component->value.y, component->value.z

zox_base_type(int3)
