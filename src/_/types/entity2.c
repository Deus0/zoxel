typedef struct {
    entity x, y;
} entity2;

#define zoxc_entity2(name)\
    zoxc(name, entity2)

#define zoxd_entity2(name)\
    zoxd(name);\
    entity_array_d_add(component_ids_entity2, ecs_id(name));

#define zox_component_string_entity2(c) \
    " e [%s : %s]",\
    zox_valid(c->value.x) ? zox_get_name(c->value.x) : "0",\
    zox_valid(c->value.y) ? zox_get_name(c->value.y) : "0"

zox_base_type(entity2)
