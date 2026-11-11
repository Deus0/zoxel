#!/usr/bin/env bash
set -euo pipefail
# Patches our imported flecs.c (v4.2.0) until fixed upstream
#
#   Stale reachable cache in flecs_emit_forward
#   - When a parent changes table (i.e. a Dirty tag is added) in the same command flush
#     that a child with a new table is created, the cached table record of the parent
#     still points at its old table
#   - Debug builds abort: flecs.c assert(r->table == elem->table)
#   - Release builds emit using the old tables column index, reading the wrong memory
#   - The cached source can also be deleted in the same flush
#   - Rebuilding from a stale parent cache also reads freed table records (segfault)
#   - Fix: never trust the cache, always rebuild it when used (correct, a bit slower)
#     - multithreaded lookups return NULL, which falls back to the slow search
#     - forward emit skips dead sources and uses their live table record
#
#   Nested deletes skip target cleanup (ZOXEL PATCH cleanup)
#   - Deleting X deletes entities through non fragmenting (R, X) pairs with
#     (OnDeleteTarget, Delete), i.e. a 3D ui with (UIHolderLink, npc)
#   - Those deletes ran inside flecs_on_delete_clear_ids, after the marked ids were
#     counted, so ids they marked (i.e. (*, ui)) were never cleared or released
#   - The leaked (*, ui) record stays MarkedForDelete, pair ids ignore generations,
#     so when the ui id is recycled, deleting the new entity skips all (*, e) cleanup
#     - links to it are never removed, owned links never delete their holder
#     - repro: bash bsh/tst.sh links_cleanup
#   - Fix: run the non fragmenting deletes in their own pass, before ids are released,
#     and repeat clearing until no new ids were marked
#
# Requirements: perl

file=inc/flecs/flecs.c
marker="ZOXEL PATCH v3: stale reachable cache"

if ! test -f "${file}"; then
    echo "!! [${file}] not found, call bsh/flecs.sh first"
    exit 1
fi

if grep -q "ZOXEL PATCH v[12]:" "${file}"; then
    echo "!! An older Flecs patch is applied, delete [inc/flecs] and call bsh/flecs.sh again"
    exit 1
fi

if ! command -v perl >/dev/null 2>&1; then
    echo "!! [perl] not found"
    exit 1
fi

patch_reachable_cache() {
if grep -q "${marker}" "${file}"; then
    echo "> Flecs Patch Found [stale reachable cache]"
    return 0
fi

echo "-> Patching [${file}] [stale reachable cache]"

perl -0pi -e '
s{        const ecs_table_record_t \*tr = elem->tr;
        ecs_assert\(tr != NULL, ECS_INTERNAL_ERROR, NULL\);
        ecs_component_record_t \*rc_cr = tr->hdr.cr;
        ecs_record_t \*r = elem->record;

        ecs_assert\(rc_cr->id == elem->id, ECS_INTERNAL_ERROR, NULL\);
        ecs_assert\(r != NULL, ECS_INTERNAL_ERROR, NULL\);
        ecs_assert\(flecs_entities_get\(world, elem->src\) == r,
            ECS_INTERNAL_ERROR, NULL\);
        ecs_dbg_assert\(r->table == elem->table, ECS_INTERNAL_ERROR, NULL\);
}{        /* ZOXEL PATCH v3: stale reachable cache (bsh/flecs_fix.sh)
         * The source can change table, or be deleted, in the same command
         * flush after the cache was built. Use its live record and look up
         * the table record in its current table. */
        if (!flecs_entities_is_alive(world, elem->src)) {
            continue;
        }
        ecs_record_t *r = flecs_entities_get(world, elem->src);
        if (!r || !r->table) {
            continue;
        }
        ecs_component_record_t *rc_cr = flecs_components_get(world, elem->id);
        const ecs_table_record_t *tr = (rc_cr && r->table) ?
            flecs_component_get_table(rc_cr, r->table) : NULL;
        if (!tr) {
            /* id was removed from the source since the cache was built */
            continue;
        }
}' "${file}"

if ! grep -q "${marker}" "${file}"; then
    echo "!! Patch failed, [flecs_emit_forward] didnt match (flecs version changed?)"
    return 1
fi

# Always rebuild the reachable cache instead of trusting its generation
perl -0pi -e '
s{    ecs_reachable_cache_t \*rc = &cr->pair->reachable;
    if \(rc->current == rc->generation\) \{
        return;
    \}

    if \(depth >= FLECS_DAG_DEPTH_MAX\) \{}{    ecs_reachable_cache_t *rc = &cr->pair->reachable;
    /* ZOXEL PATCH v3: always rebuild, the generation can miss invalidations */

    if (depth >= FLECS_DAG_DEPTH_MAX) \{}
' "${file}"

perl -0pi -e '
s{    ecs_reachable_cache_t \*rc = &cr->pair->reachable;
    if \(rc->current != rc->generation\) \{
        if \(world->flags & EcsWorldMultiThreaded\) \{
            return NULL;
        \}

        flecs_reachable_cache_ensure\(world, cr, 0, true\);
    \}

    return &rc->ids;}{    ecs_reachable_cache_t *rc = &cr->pair->reachable;
    /* ZOXEL PATCH v3: never trust the cache, multithreaded uses the slow search */
    if (world->flags & EcsWorldMultiThreaded) {
        return NULL;
    }
    flecs_reachable_cache_ensure(world, cr, 0, true);
    return &rc->ids;}
' "${file}"

if [ "$(grep -c "ZOXEL PATCH v3" "${file}")" != "3" ]; then
    echo "!! Patch failed, reachable cache functions didnt match (flecs version changed?)"
    return 1
fi
echo "+ Flecs Patched [stale reachable cache]"
}

cleanup_marker="ZOXEL PATCH cleanup"

patch_nested_cleanup() {
if grep -q "${cleanup_marker}" "${file}"; then
    echo "> Flecs Patch Found [nested cleanup]"
    return 0
fi

echo "-> Patching [${file}] [nested cleanup]"

# clear entities from a marked id onwards, not always from the first
perl -0pi -e '
s{static bool flecs_on_delete_clear_entities\(
    ecs_world_t \*world,
    bool force_delete\)
\{
    /\* Iterate in reverse order so that DAGs get deleted bottom to top \*/
    int32_t i, last = ecs_vec_count\(&world->store.marked_ids\), first = 0;}{static bool flecs_on_delete_clear_entities(
    ecs_world_t *world,
    bool force_delete,
    int32_t first_id)
\{
    /* ZOXEL PATCH cleanup: starts at first_id, earlier ids are cleared */
    /* Iterate in reverse order so that DAGs get deleted bottom to top */
    int32_t i, last = ecs_vec_count(&world->store.marked_ids), first = first_id;}
' "${file}"

# move the non fragmenting deletes out of clear_ids, into their own pass
perl -0pi -e '
s{            /\* Run OnDeleteTarget traits for non-fragmenting relationships \*/
            ecs_id_t component_id = cr->id;
            if \(ECS_IS_PAIR\(component_id\) && 
               \(ECS_PAIR_FIRST\(component_id\) == EcsWildcard\) &&
               \(cr->flags & EcsIdMatchDontFragment\)\) 
            \{
                flecs_on_delete_clear_sparse\(world, cr\);
            \}

            /\* Run OnDelete traits for non-fragmenting components \*/
            if \(ids\[i\].action == EcsDelete\) \{
                if \(cr->flags & EcsIdDontFragment\) \{
                    flecs_component_delete_sparse\(world, cr\);
                \}
            \}
}{            /* ZOXEL PATCH cleanup: non fragmenting deletes moved to
             * flecs_on_delete_clear_sparse_ids, as ids they mark here are lost */
}' "${file}"

perl -0pi -e '
s{static bool flecs_on_delete_clear_ids\(}{/* ZOXEL PATCH cleanup: runs OnDelete / OnDeleteTarget traits of non
 * fragmenting components for marked ids [first, last). Deletes made here can
 * mark new ids, the caller clears those too. */
static void flecs_on_delete_clear_sparse_ids(
    ecs_world_t *world,
    int32_t first,
    int32_t last)
{
    int32_t twice = 2;
    do {
        int32_t i;
        for (i = first; i < last; i ++) {
            /* deletes can grow the array, reobtain it */
            ecs_marked_id_t *ids = ecs_vec_first(&world->store.marked_ids);
            ecs_component_record_t *cr = ids[i].cr;
            ecs_entity_t action = ids[i].action;
            ecs_id_t component_id = cr->id;

            /* Same order as release, normal ids before wildcard ids */
            if (ecs_id_is_wildcard(component_id) != (twice == 1)) {
                continue;
            }

            /* Run OnDeleteTarget traits for non-fragmenting relationships */
            if (ECS_IS_PAIR(component_id) &&
               (ECS_PAIR_FIRST(component_id) == EcsWildcard) &&
               (cr->flags & EcsIdMatchDontFragment))
            {
                flecs_on_delete_clear_sparse(world, cr);
            }

            /* Run OnDelete traits for non-fragmenting components */
            if (action == EcsDelete) {
                if (cr->flags & EcsIdDontFragment) {
                    flecs_component_delete_sparse(world, cr);
                }
            }
        }
    } while (-- twice);
}

static bool flecs_on_delete_clear_ids(}' "${file}"

perl -0pi -e '
s{        /\* Delete all entities from the to-be-deleted tables/components \*/
        flecs_on_delete_clear_entities\(world, force_delete\);}{        /* Delete all entities from the to-be-deleted tables/components
         * ZOXEL PATCH cleanup: and non fragmenting holders, until no new ids */
        int32_t zox_first = 0, zox_last;
        do {
            flecs_on_delete_clear_entities(world, force_delete, zox_first);
            zox_last = ecs_vec_count(&world->store.marked_ids);
            flecs_on_delete_clear_sparse_ids(world, zox_first, zox_last);
            zox_first = zox_last;
        } while (ecs_vec_count(&world->store.marked_ids) > zox_first);}' "${file}"

if [ "$(grep -c "${cleanup_marker}" "${file}")" != "4" ]; then
    echo "!! Patch failed, flecs_on_delete functions didnt match (flecs version changed?)"
    return 1
fi
echo "+ Flecs Patched [nested cleanup]"
}

patch_reachable_cache || exit 1
patch_nested_cleanup || exit 1
