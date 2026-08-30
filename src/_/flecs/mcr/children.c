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

#define zox_children(world, e) ecs_children(world, e)
#define zox_children_next(it) ecs_children_next(&it)

#define zox_children_by_id(e, T) ecs_query_iter(world, ecs_query(world, { .terms = { { ecs_pair(EcsChildOf, e) }, { ecs_id(T) } } }))
#define zox_query_next(it) ecs_query_next(&it)

// TODO: Make use wrapped flecs children query instead for many
byte is_warn_capacity = 1;
uint zox_children_capacity = 256;

// Returns the direct parent (ChildOf target), or 0 if none
static inline entity zox_get_parent(
    ecs *world,
    entity child)
{
    if (!ecs_is_alive(world, child)) {
        return 0;
    }
    return ecs_get_parent(world, child);
}

static inline byte zox_is_parent(
    ecs* world,
    entity child,
    entity parent)
{
    return ecs_has_pair(world, child, EcsChildOf, parent);
}

// Returns 1 if sets parent
static inline byte zox_set_parent(
    ecs *world,
    entity child,
    entity parent)
{
    if (!ecs_is_alive(world, child)) {
        zox_logw("Trying to set parent from invalid child");
        return 0;
    }
    // Special case for removing parents
    if (parent == 0) {
        if (ecs_has_pair(world, child, EcsChildOf, EcsWildcard)) {
            ecs_remove_pair(world, child, EcsChildOf, EcsWildcard);
        }
        return 1;
    }
    if (!ecs_is_alive(world, parent)) {
        zox_logw("Trying to set parent from invalid parent");
        return 0;
    }
    // Removes previous parent pair
    if (ecs_has_pair(world, child, EcsChildOf, EcsWildcard)) {
        ecs_remove_pair(world, child, EcsChildOf, EcsWildcard);
    }
    // zox_log("Setting [%s] new Parent [%s]", zox_get_name(child), zox_get_name(parent));
    ecs_add_pair(world, child, EcsChildOf, parent);
    return 1;
}

static inline uint zox_get_children_count(
    ecs* world,
    entity parent)
{
    if (!ecs_is_alive(world, parent)) {
        return 0;
    }
    uint count = 0;
    ecs_iter_t it = ecs_children(world, parent);
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

// Removes the parent relationship from `child`
// Returns 1 if a parent was removed, 0 otherwise
byte zox_remove_parent(
    ecs *world,
    entity child)
{
    if (!ecs_is_alive(world, child)) {
        zox_logw("Trying to remove parent from invalid child");
        return 0;
    }
    if (!ecs_has_pair(world, child, EcsChildOf, EcsWildcard)) {
        return 0; // no parent to remove
    }
    ecs_remove_pair(world, child, EcsChildOf, EcsWildcard);
    return 1;
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
