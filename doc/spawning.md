# Spawning

How entities are made, what it costs, and how to keep it cheap. Learned finding lag spikes crossing chunks
(2026-11-12).

## The formula

```c
const entity e = zox_ins(world, prefab);       // an instance of a prefab
zox_set(e, Position3D, { position });          // its values
zox_set_parent_non_fragment(world, e, parent); // its parent (see below)
```

- Spawn from a prefab, set what differs. Prefabs hold everything shared.
- A realm's spawns are prefabs (mostly: some spawn their vox data, biomes aren't), i.e. a character kind is a
  prefab of a base prefab, instances copy its children (stats, body slot) when spawned.
- Spawn in `zoxp_spawn`, from `zox_system_1` systems (deferred, main thread), see [systems.md](systems.md).

## What it costs

Most of a spawn's cost comes after the system, when flecs merges the frame's changes:

- **Tables.** Every new combination of components is a new table, and every new table is checked by every
  observer and query. `ChildOf` gives every parent's children their own tables, so many parents spawning
  children made hundreds of tables a frame. Use non fragmenting parents (`zox_set_parent_non_fragment`), see
  [children.md](children.md).
- **Moves.** Each component or tag added or removed moves an entity between tables, each move notifies
  observers. A spawn that sets ten components moves ten times, state tags (`Dirty`, `PreInitialize`) move twice.
- **Names.** Naming an entity indexes it, names are for debugging, `ZoxName` is for labels in game.
- **Children.** An npc brings its stats, slots, body, skeleton and bones, a label with a glyph per letter: one
  npc is dozens of entities.

## Keeping it cheap

- **Prefabs carry structure, systems set values.** Give prefabs their children once (realm generation gives
  character prefabs their stats, `fill_character_base_stats`), roll per instance values after (a set doesn't
  move an entity).
- **One kind of children through a prefab's tree**, non fragmenting (children.md).
- **Spread spawning over frames.** Several spawners landing in one frame (a chunk's block voxes, npcs, their
  labels' glyphs) are the worst frames, each can do its share a frame and leave the rest.
- **Bulk spawning** (`zox_bulk_spawn`): many entities straight into their table with their data, 2000 npcs in 3ms
  against 77ms one at a time (`tst mass_spawn`). It can't run deferred (inside normal systems), only from an
  immediate system or between frames, so it isn't used in game yet. Children need their parent set after
  (`zox_bulk_parent`), bulk data skips registering them. A batch can be capped by its size.
- **Fewer entities.** A label as one mesh, not a glyph entity per letter (todo).

## Measuring

- `--profile` logs slow frames' spawned prefabs ("spawned most"), commands and tables made.
- `bash bsh/tst.sh perf_stream`, `tables_stream`, `mass_spawn`, `npc_stats`, `npc_skeletons`.
