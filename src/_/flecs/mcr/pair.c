// #define zoxl_links

static inline void zox_link_internal(
    ecs* world,
    entity e,
    entity relation,
    entity target)
{
#ifdef zox_safety_checks
    if (!zox_valid(e) || !zox_valid(target)) {
        zox_loge(
            "Invalid Link [%s:%llu] Target [%llu]",
                zox_getn(e),
                e,
                (unsigned long long) target);
        return;
    }
#endif
#ifdef zoxl_links
    zox_log(
        "LINK [%s:%lu] [%s] -> [%s:%lu]",
        zox_getn(e), e,
        zox_getn(relation),
        zox_getn(target), target);
#endif
    ecs_add_pair(world, e, relation, target);
}

static inline void zox_unlink_internal(
    ecs* world,
    entity e,
    entity relation,
    entity target)
{
#ifdef zox_safety_checks
    if (!zox_valid(e) || !zox_valid(target)) {
        zox_loge(
            "Invalid Unlink [%llu] Target [%llu]",
                (unsigned long long) e,
                (unsigned long long) target);
        return;
    }
#endif
#ifdef zoxl_links
    zox_log(
        "UNLINK [%s:%lu] [%s] -> [%s:%lu]",
        zox_getn(e), e,
        zox_getn(relation),
        zox_getn(target), target);
#endif
    ecs_remove_pair(world, e, relation, target);
}

static inline byte zox_is_linked_internal(
    const ecs *world,
    entity e,
    entity relation,
    entity target)
{
    return ecs_has_pair(world, e, relation, target);
}

static inline entity zox_get_link_internal(
    ecs *world,
    entity e,
    entity relation)
{
    return ecs_get_target(world, e, relation, 0);
}

static inline byte zox_is_linked_any_internal(
    const ecs *world,
    entity e,
    entity relation)
{
    return ecs_has_pair(world, e, relation, EcsWildcard);
}

#define zox_link(world, e, T, target) \
    zox_link_internal(world, e, zox_id(T), target)
#define zox_unlink(world, e, T, target) \
    zox_unlink_internal(world, e, zox_id(T), target)
#define zox_is_linked(world, e, T, target) \
    zox_is_linked_internal(world, e, zox_id(T), target)
#define zox_get_link(world, e, T) \
    zox_get_link_internal(world, e, zox_id(T))
#define zox_is_linked_any(world, e, T) \
    zox_is_linked_any_internal(world, e, zox_id(T))

/*
 * Zox Pair Query Wrapper
 *
 * zox_links(e, T)
 *     Returns an iterator over all targets linked from `e` by `T`.
 *
 * Example:
 *
 *     iter it = zox_links(e, ViewportCamera);
 *     while (zox_query_next(it)) {
 *         for (int i = 0; i < it.count; i++) {
 *             entity camera = zox_pair_target(it, 0);
 *             ...
 *         }
 *     }
 *
 */

/*#define zox_pair_query(T) \
    ecs_pair(zox_id(T), EcsWildcard)

#define zox_links(e, T) \
    ecs_query_iter(world, ecs_query(world, { \
        .terms = { { zox_pair_query(T), .src.id = e } } \
    }))

#define zox_query_next(it) \
    ecs_query_next(&it)*/

/*static inline entity zox_pair_target_internal(
    ecs *world,
    iter *it,
    int field)
{
    return ecs_pair_second(
        world,
        ecs_field_id(it, field)
    );
}

#define zox_pair_target(it, field) \
    zox_pair_target_internal(world, &(it), field)*/

static inline void zox_unlink_all_internal(
    ecs* world,
    entity e,
    entity relation)
{
#ifdef zox_safety_checks
    if (!zox_valid(e)) {
        zox_loge(
            "Invalid Unlink All [%llu]",
            (unsigned long long) e);
        return;
    }
#endif
#ifdef zoxl_links
    zox_log(
        "UNLINK ALL [%s:%lu] [%s]",
        zox_getn(e), e,
        zox_getn(relation));
#endif
    ecs_remove_pair(world, e, relation, EcsWildcard);
}

#define zox_unlink_all(world, e, T) \
    zox_unlink_all_internal(world, e, zox_id(T))

static inline entity zox_get_link_index_internal(
    ecs* world,
    entity e,
    entity relation,
    int index)
{
    return ecs_get_target(world, e, relation, index);
}

#define zox_get_link_index(world, e, T, index) \
    zox_get_link_index_internal(world, e, zox_id(T), index)
