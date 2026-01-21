#define zox_has_id(e, id) \
    ecs_has_id(world, e, id)

#define zox_set_id(e, id, size, data) \
    ecs_set_id(world, e, id, size, &data);
