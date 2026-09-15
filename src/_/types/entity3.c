typedef struct {
    entity x, y, z;
} entity3;
typedef struct {
    entity x, y, z, w;
} entity4;

#define zoxc_entity3(T)\
    zoxc(T, entity3)

#define zoxd_entity3(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_entity3, ecs_id(T)); \
    zoxd_reflect_3(T, ecs_entity_t)

#define zox_component_string_entity3(c) \
    " e [%s : %s : %s]", \
    zox_valid(c->value.x) ? zox_get_name(c->value.x) : "0",\
    zox_valid(c->value.y) ? zox_get_name(c->value.y) : "0",\
    zox_valid(c->value.z) ? zox_get_name(c->value.z) : "0"

zox_base_type(entity3)
