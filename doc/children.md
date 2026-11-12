# Children

A child is an entity owned by its parent. The claims below are tested by `bash bsh/tst.sh children`. Children are deleted with their parent, i.e. a character's skeleton, a model group's variants, or a music's notes.

Children are wrapped in `src/_/flecs/mcr/children.c`. Game code only uses the `zox_` wrappers. Never call `ecs_*` or use `Ecs*` ids outside `src/_/flecs`. If a wrapper is missing, add it there.

For pointing at something you don't own, use a link instead. See [links.md](links.md).

## Parenting

```c
entity note = spawn_note(world, prefab_note, ...);
zox_set_parent(world, note, music);     // returns 1 on success

entity parent = zox_get_parent(world, note);                 // 0 if none
if (zox_is_parent(world, note, music)) { }

zox_remove_parent(world, note);         // or zox_set_parent(world, note, 0)
```

## Tables and fragmenting

`zox_set_parent` uses flecs `ChildOf`, which **fragments**: every parent's children get their own tables.
Making tables is slow (merges, and every observer is notified), so many parents that come and go make lag
spikes. Crossing a chunk made 400 to 1100 tables (npcs' name labels and their glyphs, their stats), see
`bash bsh/tst.sh tables_stream` and `--profile`.

`zox_set_parent_non_fragment` uses flecs `EcsParent`, which doesn't: children share tables.

| Use non fragmenting for | Why it's safe |
|---|---|
| a terrain's chunks, a chunk's meshes | they're deleted with their own parent, nothing else deletes them |

**Don't** use it for children that are deleted another way too. Flecs (4.2) asserts a non fragmenting child's
parent is alive while deleting it, and crashes when:

- a cascade (deleting a realm) deletes the parent first, and then reaches the child by another path,
  i.e. a character's stats are instances of the realm's stat prefabs, deleting the realm deletes the
  character, then the stats as the prefabs' instances
- prefabs and their instances, children spawned from prefabs, mixed with ChildOf in the same tree

Tried and reverted (tests crashed ending a game): characters' stats, skills and items, ui elements, texts and
glyphs.

What the crash is (2026-11-12, flecs 4.2.0), likely a flecs bug:
- ending a game deletes the realm, flecs cleans up `(*, realm)` and deletes a character in that cleanup,
  but not the character's non fragmenting children (its stats)
- reaching a stat later, `flecs_non_fragmenting_childof_move` asserts its parent is alive, an
  `ECS_INTERNAL_ERROR` (flecs' own invariant, not misuse)
- the stats are instances of the realm's stat prefabs (prefabs of prefabs, with links), small repros of a
  realm, a character and its stats (`nf_realm`, `nf_realm_spawn_deferred`) don't crash, the extra
  ingredient isn't found yet, needed to report it upstream
- deleting the realm's tree deepest first (children before parents) hit a real flecs rule instead: a stat
  a prefab still points at can't be deleted before the prefab (`INVALID_OPERATION`) The `nf_*` tests (`src/_/flecs/tst/non_fragment.c`) cover what works: cascades, deferred deletes,
mixed children, prefab instances in one tree. Add a case there before trying a new use.

Other ways to stop fragmenting: link instead of parenting (links never fragment), or keep the data as
components on the entity instead of child entities (i.e. stats).

## Getting children

Children are copied into a buffer you pass in. If there are more than `capacity`, the extras are skipped with a warning.

```c
// all children
entity children[32];
uint count = zox_get_children(world, parent, children, 32);
for (uint i = 0; i < count; i++) {
    // ...
}

// children with a component or tag
entity models[64];
uint count = zox_get_children_by_id(world, model_group, models, 64, zox_id(Model));

// the first child with a component or tag
entity tooltip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));

// by index
entity first = zox_get_child_by_index(world, parent, 0);

// counts
uint count = zox_get_children_count(world, parent);
uint models_count = zox_get_children_count_by_id(world, parent, zox_id(Model));
```

### Recursive

```c
// first match anywhere below parent
entity bone = zox_get_child_by_id_recursive(world, skeleton, zox_id(Bone));

// all matches anywhere below parent, pass 0 as the starting count
entity bones[64];
uint count = zox_get_children_by_id_recursive(world, skeleton, bones, 64, zox_id(Bone), 0);
```

### Walking up

```c
// nearest ancestor with a component or tag
entity canvas = zox_get_parent_by_id(world, element, zox_id(Canvas));

// top of the tree
entity root = zox_get_parent_root(world, element);
```

## Ordered children

Children aren't kept in order by default. When order matters, i.e. notes played in sequence, mark the parent with `zox_add_ordered_children` before adding children.

```c
// spawning
entity spawn_music(ecs* world, entity prefab, float speed) {
    zox_instance(prefab);
    zox_name("music");
    zox_add_ordered_children(e);
    return e;
}

entity spawn_note(ecs* world, entity prefab, entity music, ...) {
    zox_instance(prefab);
    zox_name("note");
    zox_set_parent(world, e, music);
    // ...
    return e;
}

// reading, in insertion order
ordered_children notes = zox_get_ordered_children(world, music);
for (int i = 0; i < notes.length; i++) {
    entity note = notes.value[i];
    // ...
}
```

- `zox_get_ordered_children` returns an empty list if the parent isn't valid or doesn't have ordered children.
- The list is a view into the ECS, so don't keep it across frames. Deleting children while looping is fine inside systems, since they run on staged worlds and deletes are deferred.

## Examples in the code

- Model variants are children of their `ModelGroup`: `get_model_variant()` in `src/models/ins/vox_children.c`
- Notes are ordered children of their music: `src/musics/pre/note.c`, `src/musics/sys/play.c`
- Character children (voxes, renderers, skeletons): [character.md](character.md)
