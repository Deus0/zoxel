# States and Change Events

How an entity says "this changed, react to it". The goal: few components, few systems, simple to follow. Pick the
first form in the list that fits.

## The forms, simplest first

| Form | Components | Systems | Use when |
|---|---|---|---|
| 1. A tag its reader removes | 1 tag | 0 (the reader's own) | one system reacts |
| 2. The core `Dirty` / `Update` | 0 new | 0 new (shared) | several react, nothing else dirties that entity |
| 3. An observer on a tag | 0-1 | 0 | a one-off event, few a frame, the reaction light (camera attach) |
| 4. A pair, `zoxc_dirty(T)` | 2 tags | 2 | several readers in different phases, and the entity has other changes |
| 5. A byte state, `zoxc_state(T)` | 1 byte | 1 (increment) | a machine with steps, or code reads "still pending" |

### 1. A tag its reader removes

```c
zox_tag(GenerateSound);                 // com
zoxd_tag_event(GenerateSound);
zox_add(e, GenerateSound);              // a writer
zox_system(sound_generate_system, zoxp_update, [in] ..., [none] GenerateSound);
// in the system, per entity:
zox_remove(e, GenerateSound);
```

- `[none]` terms go last: fields are read by position (`zox_sys_in` order).
- Two readers in one phase: the later one removes it (`ActiveStateDirty`: `element_active_system`, then
  `element_active_link_system` removes it). Say so in a comment at the removal.
- Tags set on a prefab are copied to its instances, so a prefab can start one dirty (the editor's inspector).

### 2. The core `Dirty` / `Update`

- Writers `zox_add(e, Dirty)`. `dirty_system` (zoxp_remove) swaps it for `Update`, `update_system` removes `Update`
  a frame later.
- Readers match the component that gives it meaning plus the tag: `[in] SoundData, [none] Sound, [none] core.Update`.
- **`Dirty`** is on from the writer's merge to the end of that frame: the same frame, as a byte's "active" was.
  **`Update`** is the whole next frame: safe for readers in an earlier phase than the writer (`TriggerSound`: sounds
  generated in zoxp_update, played on `Update`).
- Check nothing else dirties that entity first: ui elements get `Dirty` for their text, so a ui event needs its own
  tag (form 1 or 4).

### 3. An observer on a tag

The coding standards' way for one-off events: a non fragmenting tag (`zoxd_nf_tag`) added when it happens, an
observer acts and removes it (`camera_attach_event`: `zox_on_add(camera_attach_event, [in] core.Target,
[none] cameras.AttachDirty)`, its loop starts `zox_remove(it->entities[i], AttachDirty)`). No system runs while
nothing happens. A perf test (perf_fly, release) couldn't tell 3 such systems from observers (frames vary 25 to 37ms
run to run), so pick by fewer components and systems.


`zox_on_add(fn, [in] core.ZoxName, [none] settings.Setting, [none] core.Dirty)`, run at the merge, on the main
thread. Settings use it (doc/tech/settings.md).

- Observe **`Dirty`**, not `Update`: `Update` stays on while an entity is dirtied again on the next frame, so its
  OnAdd doesn't fire again (a dragged slider applied only its first value, tst settings_drag).
- Keep heavy work in systems (they run on workers, an observer doesn't).
- An observer runs the moment the tag lands, a frame sooner than a system on `Update`: check what it reads is ready
  (the grid's sizes weren't).

### 4. A pair, `zoxc_dirty(T)`

`zoxc_dirty(T)` makes `TDirty` and `TUpdate`, `zoxc_system_dirty(T)` / `zoxd_system_dirty(T)` their two systems
(src/_/flecs/mcr/dirty.c). Only when one tag isn't enough: readers in several phases on an entity with other kinds
of change. Each pair costs 2 tags and 2 systems.

### 5. A byte state, `zoxc_state(T)`

A byte: `zox_dirty_trigger` (1) → `active` (2) → `end` (3) → `none` (0), one step a frame by its increment system
(src/_/flecs/comm/state.c). Keep one when the steps are used:

- something waits on `end` (the player state machine moves on at its third frame),
- writers check "nonzero" to wait out a change in flight (the game state's 3 frame guard),
- or code reads it as "still pending" (streaming).

## What they cost

`tst flecs_event_costs` (src/_/flecs/tst/event_costs.c) times each in a world of its own, single threaded, 4000
frames (debug build, 2026-11-21):

| | Cost |
|---|---|
| an idle system (matches nothing) | 0.007us a frame |
| an idle observer | 0.001us a frame |
| an idle byte state (its increment system on 20 entities) | 0.44us a frame, more with more entities |
| an event handled by an observer that removes its tag | 1.3us |
| an event handled by a system that removes its tag | 2.7us |
| an event through a pair (TDirty, TUpdate, a reader) | 4.1us |

- Byte states cost while nothing happens: their increment system walks every entity that has one, every frame.
  Tags cost nothing until added.
- An idle system is nearly free (flecs skips an empty query), so a system count matters for reading the pipeline
  (and merges, tst pipeline_syncs) more than for speed.
- For an event, an observer is cheapest, a pair dearest. Rerun the test before deciding on new numbers.

### Many events a frame

`tst flecs_event_scaling`: [n] entities tagged each frame by a system (deferred, as the game does), then handled
(us a frame, debug build, 2026-11-21):

| events a frame | observer | system | system, 4 threads | pair | observer, non fragmenting tag |
|---|---|---|---|---|---|
| 1 | 4.7 | 6.3 | 17.0 | 7.8 | 4.7 |
| 10 | 15.3 | 16.2 | 31.6 | 19.0 | 15.6 |
| 100 | 141 | 86 | 107 | 124 | 131 |
| 1000 | 1284 | 821 | 809 | 1149 | 1226 |
| 10000 | 12399 | 8466 | 7600 | 11755 | 12731 |

- Observers scale linearly: ~1.3us an event at any count, no blow up. A system takes a table's entities in one
  call: ~0.85us an event from ~100 a frame, a third cheaper. They cross at ~10 events a frame.
- Threads barely help here: the cost is the tag's add and remove (commands merged on the main thread), not the
  handler. A handler with real work (a mesh, a light) gains from a system's threads, an observer can't.
- So: an **observer** for events that are few a frame (ui, settings, a camera, a sound, a player's action); a
  **system** for ones that come in bulk (chunks streaming, meshes, lights: hundreds a frame) or carry heavy work.

## Timing

- A tag added in a system lands at the next merge: a reader in the same phase, before that merge, misses it this
  frame (and sees it next run, unless it's `Dirty`, gone by then). Match `Update` or remove-it-yourself tags there.
- A byte's "active" was the frame it was set (when its increment ran before the readers). `Dirty` matches that,
  `Update` is a frame later.
- Changing timing changes behaviour: tags in place of `GameStateDirty` let the game clean up (its camera gone)
  before the players saw the end (tst menu_flow, end_game_cleanup). Test the flows a state drives.

## Where things stand (2026-11-21)

- Tags removed by their reader: `GenerateSound`, `ProcessSound`, `CombineVox`, `InspectorDirty`,
  `HierarchyUIDirty`, `GlyphSpawnedDirty`, `AnimateTextEnded`, `ActiveStateDirty`
- Core `Dirty` / `Update`: `TriggerSound` (on sounds), every setting's change (observers on `Dirty`), texts
- Observers on a non fragmenting tag they remove: `AttachDirty` (camera_attach_event), `WindowSizeDirty`
  (viewport_resize_event), `ListDirty` (scrollbar_handle_event); settings (on the core `Dirty`)
- Pairs: `Skeleton`, `InsideBlock`, `ListPosition`, `LayoutPosition`, `LayoutSize` (several readers); `Grid` (one
  reader, but as an observer it laid its icons out before their sizes settled: tst navigator_inventory's tooltips
  showed the wrong item; its system reads them a frame on)
- Bytes kept:
  - machines: `PlayerStateDirty`, `GameStateDirty`, `Generate`, `GenerateCharacter`, `PickedUp`
  - action timers: `ActivateBegin`, `WarmupState`, `Activate`, `CooldownState`, `TriggerActionA/B/E`,
    `ActiveActionDirty`
  - pending reads: `StreamDirty`, `StreamDirty2`, `NpcSpawnZoneDirty`, `LightNodeDirty`
  - render events: `RenderDepthDirty`, `RenderDistanceDirty` (many writers and readers in lod streaming: a pair
    would cost 2 systems more each, for no fewer components)
- Gone: `ItemDirty`, `Bounds3Dirty` (and `bounds3_enable_system`), `SettingDirty`

## Tests

| Test | Covers |
|---|---|
| sound_trigger | a sound generated reaches the mixer (GenerateSound, then Dirty / Update) |
| settings_drag | an observer on Dirty fires every frame a setting changes |
| navigator, options_toggle | ActiveStateDirty: frames and toggles light |
| menu_flow, end_game_cleanup | the game and player state machines |
| pipeline_syncs, lint_systems | merges and systems |
