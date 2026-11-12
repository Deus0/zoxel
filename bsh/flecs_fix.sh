#!/usr/bin/env bash
set -euo pipefail
# Patches our imported flecs.c (v4.2.0) until fixed upstream
# Each patch is described in doc/flecs_patches.md (why, repro, upstream status), keep it in sync
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
#   Non fragmenting children deleted after their parent (ZOXEL PATCH nf parent)
#   - Deleting a tree (a realm), ids marked while clearing (ChildOf, character) are cleared after the
#     tables that held their parents, so a non fragmenting child (EcsParent, a character's stats) is
#     deleted when its parent already is
#   - flecs_non_fragmenting_childof_move asserted the parent was alive (ECS_INTERNAL_ERROR)
#   - Fix: the parent's child record still exists, keep its bookkeeping, skip the steps that use the
#     dead parent entity (the spawner check, renaming under its path), the child is being deleted
#     - repro: bash bsh/tst.sh menu_flow (ending a game with npcs' stats non fragmenting)
#
#   Deleting an entity walked every non fragmenting component record (ZOXEL PATCH nf root list)
#   - flecs_entity_remove_non_fragmenting checked each record in the world's non fragmenting list for the
#     entity, a pair link makes a record per target, 14k in game, so a delete was O(records)
#   - deleting a chunk column and its npcs (thousands of entities) took 100-160ms merges flying (tst perf_fly)
#   - Fix: a second list of roots (components, (R, *) wildcards), an entity's pairs come from its (R, *)
#     record's sparse targets, a delete visits ~70 records, worst flying 160ms -> 22ms (--maxnpcs)
#     - bsh/flecs_patches/04_nf_root_list.patch, repro: bash bsh/tst.sh perf_fly --stress (release, --maxnpcs)
#
#   Instancing a prefab walked every non fragmenting component record (ZOXEL PATCH nf root instantiate)
#   - flecs_instantiate_dont_fragment, the same walk for each instance, 38% of slow frames flying (npcs spawning)
#   - Fix: the roots and the prefab's own targets, bsh/flecs_patches/05_nf_root_instantiate.patch
#
# The patches are diffs in bsh/flecs_patches, applied in order with patch (no perl or python):
#   - one per fix, made against a clean flecs.c v4.2.0 (bsh/flecs.sh downloads it)
#   - a patch that's already in is skipped, one that doesn't apply stops (flecs changed? check upstream)
#   - a new fix: edit inc/flecs/flecs.c, then diff -u a clean copy against it into the next numbered .patch
#
# Requirements: patch

file=inc/flecs/flecs.c
if [[ ! -f "${file}" ]]; then
    echo "!! [${file}] not found, download it with bash bsh/flecs.sh"
    exit 1
fi
if ! command -v patch >/dev/null 2>&1; then
    echo "!! [patch] not found"
    exit 1
fi

for p in bsh/flecs_patches/*.patch; do
    name="$(basename "${p}" .patch)"
    if patch -s -p1 -R --dry-run -f < "${p}" >/dev/null 2>&1; then
        echo "+ Flecs [${name}] already patched"
    elif patch -s -p1 -N -f < "${p}"; then
        echo "+ Flecs Patched [${name}]"
    else
        echo "!! Flecs patch [${name}] didn't apply (flecs version changed? fixed upstream?), see doc/flecs_patches.md"
        exit 1
    fi
done
