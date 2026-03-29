#define zox_has_id(e, id) \
    ecs_has_id(world, e, id)

#define zox_set_id(e, id, size, data) \
    ecs_set_id(world, e, id, size, &data);

#define zox_id(name)\
    ecs_id(name)

#define zox_get_id(e, id)\
    ecs_get_id(world, e, id)

#define zox_geter_id(e, id, type, name)\
    type name = zox_get_id(e, id)->value

#define zox_has_id_flag(id, flag)\
    ECS_HAS_ID_FLAG(id, flag)
