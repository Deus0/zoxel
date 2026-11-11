// Reading an entity's ids generically, i.e. for the editor's inspector
//  - ids are components, tags and pairs (links, parents)

// how many ids e has
static inline int zox_get_ids_count(
    ecs* world,
    entity e)
{
    const ecs_type_t* type = ecs_get_type(world, e);
    return type ? type->count : 0;
}

// e's id at index, 0 past the end
static inline entity zox_get_id_at(
    ecs* world,
    entity e,
    int index)
{
    const ecs_type_t* type = ecs_get_type(world, e);
    return type && index >= 0 && index < type->count ? type->array[index] : 0;
}

// an id without its flags (i.e. override), the component itself
static inline entity zox_id_component(entity id) {
    return id & ECS_COMPONENT_MASK;
}

static inline entity zox_pair_first(
    ecs* world,
    entity pair)
{
    return ecs_pair_first(world, pair);
}

static inline entity zox_pair_second(
    ecs* world,
    entity pair)
{
    return ecs_pair_second(world, pair);
}

// a component's size in bytes, 0 for tags
static inline uint zox_component_size(
    ecs* world,
    entity component)
{
    const EcsComponent* data = (const EcsComponent*) ecs_get(world, component, EcsComponent);
    return data ? data->size : 0;
}
