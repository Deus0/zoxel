typedef struct {
    entity x, y, z;
} entity3;

#define zoxc_entity3(name)\
    zoxc(name, entity3)

#define zoxd_entity3(name)\
    zoxd(name);\
    add_to_entity_array_d(component_ids_entity3, ecs_id(name));

#define zox_component_string_entity3(c) \
    " e [%s : %s : %s]", \
    zox_valid(c->value.x) ? zox_get_name(c->value.x) : "0",\
    zox_valid(c->value.y) ? zox_get_name(c->value.y) : "0",\
    zox_valid(c->value.z) ? zox_get_name(c->value.z) : "0"

zox_base_type(entity3)
