/* Zox Hierarchy Wrapper
=================

- zox_get_parent(world, child)
    Returns the direct parent of `child`, or 0 if the entity has no parent.

    Example:
        entity p = zox_get_parent(world, child);

- zox_set_parent(world, child, parent)
    Assigns `child` to `parent`.
    Returns 1 on success, 0 if either entity is invalid or dead.

    Example:
        zox_set_parent(world, child, parent);

- zox_get_children(world, parent, buffer, capacity)
    Writes the direct children of `parent` into a caller-provided buffer
        - Pass in a entity buffer
        - Returns a uint count

    Example:
        entity children[32];
        uint count = zox_get_children(world, parent, children, 32);

*/

// NOTE: Broken
#define zox_non_fragment_parent

#ifdef zox_non_fragment_parent
    #define zox_parent_id zox_id(EcsParent)
    #define zox_parent_query_id EcsChildOf
#else
    #define zox_parent_id EcsChildOf
    #define zox_parent_query_id EcsChildOf
#endif

#define zox_children(world, e) \
    ecs_children(world, e)
#define zox_children_next(it) \
    ecs_children_next(&it)

#define zox_children_by_id(e, T) \
    ecs_query_iter(world, ecs_query(world, { \
        .terms = { \
            { ecs_pair(zox_parent_query_id, e) }, \
            { ecs_id(T) } \
        } \
    }))

#define zox_query_next(it) \
    ecs_query_next(&it)

// TODO: Make use wrapped flecs children query instead for many
byte is_warn_capacity = 1;
uint zox_children_capacity = 256;

// Returns the direct parent (zox_parent_id target), or 0 if none
static inline entity zox_get_parent(
    ecs *world,
    entity child)
{
    if (!zox_alive(child)) {
        return 0;
    }
    return ecs_get_parent(world, child);
}

static inline byte zox_is_parent(
    ecs* world,
    entity child,
    entity parent)
{
#ifdef zox_non_fragment_parent
    const EcsParent* current = zox_get(child, EcsParent);
    return (current && current->value == parent)
        || (!current && !parent);
#else
    return ecs_has_pair(world, child, EcsChildOf, parent);
#endif
}

// Removes the parent relationship from `child`
// Returns 1 if a parent was removed, 0 otherwise
byte zox_remove_parent(
    ecs *world,
    entity child)
{
    if (!zox_valid(child)) {
        zox_logw("Trying to remove parent from invalid child");
        return 0;
    }
#ifdef zox_non_fragment_parent
    if (ecs_has_id(world, child, zox_id(EcsParent))) {
        zox_remove(child, EcsParent);
        return 1;
    }
#else
    if (ecs_has_pair(world, child, EcsChildOf, EcsWildcard)) {
        ecs_remove_pair(world, child, EcsChildOf, EcsWildcard);
        return 1;
    }
#endif
    return 0;
}

// Returns 1 if sets parent
static inline byte zox_set_parent(
    ecs* world,
    entity child,
    entity parent)
{
    if (!zox_valid(child)) {
        zox_logw("Trying to set parent from invalid child");
        return 0;
    }
    if (parent &&
        (!zox_alive(parent) ||
        !zox_valid(parent)))
    {
        zox_logw("Trying to set parent from invalid parent");
        return 0;
    }
    // Removes previous parent pair
#ifdef zox_non_fragment_parent
    if (!parent || !zox_valid(parent)) {
        return zox_remove_parent(world, child);
        /*if (current && current->value) {
            zox_remove(child, EcsParent);
        }*/
        // return 1;
    }
    const EcsParent* current = zox_get(child, EcsParent);
    if (current && current->value == parent) {
        return 1;
    }
    zox_setv(child, EcsParent, parent);
#else
    if (!parent) {
        return zox_remove_parent(world, child);
    }
    if (ecs_has_pair(world, child, EcsChildOf, parent)) {
        return 1;
    }
    if (ecs_has_pair(world, child, EcsChildOf, EcsWildcard)) {
        ecs_remove_pair(world, child, EcsChildOf, EcsWildcard);
    }
    ecs_add_pair(world, child, EcsChildOf, parent);
#endif
    return 1;
}

static inline uint zox_get_children_count(
    ecs* world,
    entity parent)
{
    if (!zox_alive(parent)) {
        return 0;
    }
    uint count = 0;
    iter it = ecs_children(world, parent);
    while (ecs_children_next(&it)) {
        count += it.count;
    }
    return count;
}

static inline uint zox_get_children_count_by_id(
    ecs* world,
    entity parent,
    entity id)
{
    if (!ecs_is_alive(world, parent)) {
        return 0;
    }
    uint count = 0;
    ecs_iter_t it = ecs_children(world, parent);
    while (ecs_children_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            entity e2 = it.entities[i];
            if (zox_has_id(e2, id)) {
                count++;
            }
        }
    }
    return count;
}

// Fills the buffer with the found children from the flecs query
static inline uint zox_get_children(
    ecs *world,
    entity parent,
    entity* entities,
    uint capacity)
{
    if (!ecs_is_alive(world, parent)) {
        return 0;
    }
    if (!entities || capacity <= 0) {
        zox_logw("[%s]'s No Capacity [zox_get_children]", zox_get_name(parent));
        return 0;
    }
    ecs_iter_t it = ecs_children(world, parent);
    uint count = 0;
    while (ecs_children_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            // If Buffer is Full
            if (count >= capacity) {
                if (is_warn_capacity) {
                    zox_logw("[%s]'s Exceeded Capacity [%i] [zox_get_children] ", zox_get_name(parent), capacity);
                }
                // return count;
            } else {
                entities[count] = it.entities[i];
                count++;
            }
        }
    }
    return count;
}

// Fills the buffer with the found children from the flecs query
static inline uint zox_get_children_by_id(
    ecs *world,
    entity parent,
    entity* entities,
    uint capacity,
    entity id)
{
    if (!zox_alive(parent)) {
        return 0;
    }
    if (!entities || capacity <= 0) {
        zox_logw("[%s]'s No Capacity [zox_get_children_by_id]", zox_get_name(parent));
        return 0;
    }
    uint count = 0;
    iter it = zox_children(world, parent);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            entity e2 = it.entities[i];
            // If Buffer is Full
            if (count >= capacity) {
                if (is_warn_capacity) {
                    zox_logw("[%s]'s Exceeded Capacity [%i] [zox_get_children_by_id]", zox_get_name(parent), capacity);
                }
            } else {
                if (zox_has_id(e2, id)) {
                    entities[count] = e2;
                    count++;
                }
            }
        }
    }
    return count;
}

static inline entity zox_get_child_by_id(
    ecs* world,
    entity parent,
    entity id)
{
    if (!ecs_is_alive(world, parent)) {
        zox_loge("Cannot get children from invalid parent [%s]",
            zox_getn(id));
        return 0;
    }
    ecs_iter_t it = ecs_children(world, parent);
    while (ecs_children_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            entity e = it.entities[i];
            if (zox_has_id(e, id)) {
                return e;
            }
        }
    }
    return 0;
}


static inline entity zox_get_child_by_index(
    ecs* world,
    entity parent,
    uint index)
{
    if (!ecs_is_alive(world, parent)) {
        zox_loge("Cannot get children from invalid parent of index [%i]", index);
        return 0;
    }
    uint count = 0;
    ecs_iter_t it = ecs_children(world, parent);
    while (ecs_children_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            entity e = it.entities[i];
            if (count == index) {
                return e;
            }
            count++;
        }
    }
    return 0;
}


static inline entity zox_get_parent_root_recursive(
    ecs *world,
    entity parent,
    entity child)
{
    if (!parent || !ecs_is_alive(world, parent)) {
        return child;
    }
    return zox_get_parent_root_recursive(world, zox_get_parent(world, parent), parent);
}


static inline entity zox_get_parent_root(
    ecs* world,
    entity e)
{
    return zox_get_parent_root_recursive(
        world,
        zox_get_parent(world, e),
        e);
}

static inline entity zox_get_parent_by_id(
    ecs* world,
    entity e,
    entity id)
{
    if (!ecs_is_alive(world, e)) {
        return 0;
    }
    entity parent = zox_get_parent(world, e);
    if (!parent || !ecs_is_alive(world, parent)) {
        return 0;
    }
    if (zox_has_id(parent, id)) {
        return parent;
    } else {
        return zox_get_parent_by_id(world, parent, id);
    }
}

entity zox_get_child_by_id_recursive(
    ecs* world,
    entity parent,
    entity id)
{
    if (!ecs_is_alive(world, parent)) {
        zox_loge("Cannot get children from invalid parent [%s]", zox_get_name(id));
        return 0;
    }
    ecs_iter_t it = ecs_children(world, parent);
    while (ecs_children_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            entity e = it.entities[i];
            if (zox_has_id(e, id)) {
                return e;
            }
            entity e2 = zox_get_child_by_id_recursive(world, e, id);
            if (e2) {
                return e2;
            }
        }
    }
    return 0;
}

// Fills the buffer with the found children from the flecs query
uint zox_get_children_by_id_recursive(
    ecs* world,
    entity parent,
    entity* entities,
    uint capacity,
    entity id,
    uint count)
{
    if (!zox_alive(parent)) {
        return 0;
    }
    if (!entities || capacity <= 0) {
        zox_logw("[%s]'s No Capacity [zox_get_children_by_id]", zox_get_name(parent));
        return 0;
    }
    iter it = zox_children(world, parent);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            entity e2 = it.entities[i];
            // If Buffer is Full
            if (count >= capacity) {
                if (is_warn_capacity) {
                    zox_logw("[%s]'s Exceeded Capacity [%i] [zox_get_children_by_id]", zox_get_name(parent), capacity);
                }
            } else {
                if (zox_has_id(e2, id)) {
                    entities[count] = e2;
                    count++;
                }
                count = zox_get_children_by_id_recursive(world, e2, entities, capacity, id, count);
            }
        }
    }
    return count;
}
