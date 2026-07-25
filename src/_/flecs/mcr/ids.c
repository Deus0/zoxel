#define zox_has_id(e, id) \
    ecs_has_id(world, e, id)

#define zox_add_id(e, id) \
    ecs_add_id(world, e, id)

#define zox_set_id(e, id, T, data) \
    ecs_set_id(world, e, id, sizeof(T), &data)

#define zox_id(name)\
    ecs_id(name)

#define zox_get_id(e, id) ecs_get_id(world, e, id)

#define zox_geter_id(e, id, type, name)\
    type name = zox_get_id(e, id)->value

#define zox_has_id_flag(id, flag)\
    ECS_HAS_ID_FLAG(id, flag)

typedef struct {
    float value;
} zox_float;

typedef struct {
    int value;
} zox_int;

typedef struct {
    byte value;
} zox_byte;

// Generic getter with base type
#define zox_get_idv(e, id, T) (((zox_##T*) ecs_get_id(world, e, id))->value)
