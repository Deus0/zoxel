# Flecs Patches

flecs.c is patched by bsh/flecs_fix.sh after downloading, each patch has a ZOXEL PATCH marker in flecs.c, keep this file in sync, aim to upstream them.

## How to (re)apply

- bash bsh/flecs_fix.sh (needs only patch), applies bsh/flecs_patches/*.patch in order, skips ones already in
- each patch is a diff against a clean flecs.c v4.2.0, the same diff an upstream PR needs
- a new fix: edit inc/flecs/flecs.c, diff -u a clean copy (bash bsh/flecs.sh downloads one) against it into the
  next numbered .patch, and add its section below
- when updating flecs: reapply, if a patch fails to match, check whether upstream fixed it

## Patches

### Stale reachable cache in flecs_emit_forward

- Marker: ZOXEL PATCH v3: stale reachable cache
- Problem: When a parent changes table (i.e. a Dirty tag is added) in the same command flush that a child with a new table is created, the cached table record of the parent still points at its old table. Debug builds abort: flecs.c assert(r->table == elem->table). Release builds emit using the old tables column index, reading the wrong memory. The cached source can also be deleted in the same flush. Rebuilding from a stale parent cache also reads freed table records (segfault).
- Fix: never trust the cache, always rebuild it when used (correct, a bit slower). Multithreaded lookups return NULL, which falls back to the slow search. Forward emit skips dead sources and uses their live table record.
- Repro: no dedicated test yet (it showed as asserts and segfaults while playing), write one before upstreaming
- Upstream: not reported yet

### Nested deletes skip target cleanup

- Marker: ZOXEL PATCH cleanup
- Problem: Deleting X deletes entities through non fragmenting (R, X) pairs with (OnDeleteTarget, Delete), i.e. a 3D ui with (UIHolderLink, npc). Those deletes ran inside flecs_on_delete_clear_ids, after the marked ids were counted, so ids they marked (i.e. (*, ui)) were never cleared or released. The leaked (*, ui) record stays MarkedForDelete, pair ids ignore generations, so when the ui id is recycled, deleting the new entity skips all (*, e) cleanup - links to it are never removed, owned links never delete their holder.
- Fix: run the non fragmenting deletes in their own pass, before ids are released, and repeat clearing until no new ids were marked.
- Repro: bash bsh/tst.sh links_cleanup
- Upstream: not reported yet

### Non fragmenting children deleted after their parent

- Marker: ZOXEL PATCH nf parent
- Problem: Deleting a tree (a realm), ids marked while clearing (ChildOf, character) are cleared after the tables that held their parents, so a non fragmenting child (EcsParent, a character's stats) is deleted when its parent already is. flecs_non_fragmenting_childof_move asserted the parent was alive (ECS_INTERNAL_ERROR).
- Fix: the parent's child record still exists, keep its bookkeeping, skip the steps that use the dead parent entity (the spawner check, renaming under its path), the child is being deleted.
- Repro: bash bsh/tst.sh menu_flow (ending a game with npcs' stats non fragmenting)
- Upstream: not reported yet

### Deleting an entity walked every non fragmenting component record

- Marker: ZOXEL PATCH nf root list
- Problem: flecs_entity_remove_non_fragmenting checked each record in the world's non fragmenting list for the entity, a pair link makes a record per target, 14k in game, so a delete was O(records). Deleting a chunk column and its npcs (thousands of entities) took 100-160ms merges flying (tst perf_fly).
- Fix: a second list of roots (components, (R, *) wildcards, pairs whose relationship keeps no targets), an entity's pairs come from its (R, *) record's sparse targets, a delete visits ~70 records, not 14k. Worst frame flying 160ms -> 22ms (--maxnpcs). Applied by bsh/flecs_patches/04_nf_root_list.patch.
- Found with the sampler (`ZOX_SAMPLE`, doc/commands.md): 82% of slow frames in this one function
- Repro: release build, `--tst perf_fly --maxnpcs --profile` (bash bsh/tst.sh perf_fly --stress); a plain flecs repro is many pair targets (a record each) then deleting entities that have any non fragmenting id
- Upstream: not reported yet

### Instancing a prefab walked every non fragmenting component record

- Marker: ZOXEL PATCH nf root instantiate
- Problem: flecs_instantiate_dont_fragment checked every record in the world's non fragmenting list (sparse_has on the prefab for each of ~14k), for every instance. It was 38% of slow frames flying (npcs and their parts spawning), sparse_has alone 29%.
- Fix: walks the root list (from the nf root list patch), and the prefab's own targets of each relationship from its (R, *) record, the per record copy is unchanged (flecs_instantiate_dont_fragment_one). Applied by bsh/flecs_patches/05_nf_root_instantiate.patch.
- Repro: release build, `--tst perf_fly --maxnpcs --profile` with `ZOX_SAMPLE` (doc/commands.md), worst merge flying 18.6ms -> 11ms, slow frames (12ms+) 57 -> 9
- Upstream: not reported yet

## Writing an upstream PR

- A minimal repro in plain flecs C (no zoxel macros), against flecs master, ideally as a flecs test
- The failing assert, crash, or timings before and after
- The fix as a diff against flecs master (bsh/flecs_fix.sh's perl and python edits show what changed)
- Once reported, put the issue or PR link in its Upstream line, and remove the patch when a flecs release has it
