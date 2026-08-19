#define zox_new() \
    ecs_new(world)

#define zox_id(name)\
    ecs_id(name)

#define zox_has_id(e, id) \
    ecs_has_id(world, e, id)

#define zox_add_id(e, id) \
    ecs_add_id(world, e, id)

#define zox_set_id(e, id, T, data) \
    ecs_set_id(world, e, id, sizeof(T), &data)

#define zox_get_id(e, id) \
    ecs_get_id(world, e, id)

#define zox_geter_id(e, id, type, name) \
    type name = zox_get_id(e, id)->value

#define zox_has_id_flag(id, flag) \
    ECS_HAS_ID_FLAG(id, flag)

#define zox_add(e, T) \
    ecs_add(world, e, T)

#define zox_add_id(e, id) \
    ecs_add_id(world, e, id)

#define zox_remove(e, T) \
    ecs_remove(world, e, T)

#define zox_remove_id(e, id) \
    ecs_remove_id(world, e, id)

#define zox_has(e, T) \
    ecs_has(world, e, T)

#define zox_alive(e) \
    (e && ecs_is_alive(world, e))

// NOTE: Checks whether id is inside a ecs world
#define zox_valid(e) \
    (e && ecs_is_valid(world, e))

#define zox_dont_fragment(T) \
    ;
// NOTE: Disabled for now
    //zox_add_id(zox_id(T), EcsDontFragment)

typedef struct {
    float value;
} zox_float;

typedef struct {
    int value;
} zox_int;

typedef struct {
    byte value;
} zox_byte;

#define zox_get_idv(e, id, T) \
    (((zox_##T*) ecs_get_id(world, e, id))->value)
