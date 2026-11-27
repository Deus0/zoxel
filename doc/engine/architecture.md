# Architecture

## Patterns we stick to

These keep the codebase small and the same everywhere. Follow them in new code, and move old code to them when you
touch it.

### Reactions are observers on event tags

- **A reaction is an observer on an event tag**: whatever makes the change adds the tag, and each module that
  reacts observes it, filtered by what it cares about. Any event tag works (`Dead`, `DeathDirty`, `InTown`,
  `DeviceLost`, `HierarchyUIDirty`, `ListDirty`...), declared as one (`zoxd_tag_event`, or a non fragmenting
  `zoxd_nf_tag` an observer removes; costs below in "Which kind of event").
- **The core `Dirty` is the most common one** (`src/_/com`, non fragmenting, `dirty_cleanup_event` removes it): use
  it for "this entity changed" when no more specific tag says what changed:
  `zox_on_add(fn, [in] players.PlayerState, [none] players.Player, [none] core.Dirty)`.
- **No listener lists** (the old `*Event` components a module added function pointers to, i.e. `GameStateEvent`,
  `PlayerStateEvent`), **no global function pointer hooks**, **no system that scans for a change** ("is it dirty
  this frame?"). A system that only reacts is an observer.
- **Register observers in the module's `eve/_.c`** (its `zox_events_*` function), next to the reactions they call.
- **A change made inside an observer can't add the same event tag again**: the entity still has it, so nothing fires.
  Set a target instead, a component that's only there while a change waits (`PlayerStateTarget`), and let the
  owner's system apply it next frame and **remove it once used**. Don't keep it with a "none" value to manage.
- **Main thread systems (`zox_system_1`) only for spawning or gpu uploads.** Everything else is a normal, threaded
  system; deletions and spawns count as spawning (`game_state_system` deletes the realm).

### Links, not entity fields

- **Point one entity at another with a link**: `zox_link`, declared with `zoxd_nf_link` (one target, linking again
  replaces it) or `zoxd_nf_links` (many). Not a raw `entity` in a component or a `Target` you manage by hand.
  Links clean up better: they're flecs's own relationships, so a deleted target unlinks itself, nothing points at a
  dead entity, and any extra cleanup goes in `zox_on_unlink`. See [Links](links.md).
- **Links have no order.** When order matters (a list's rows), use ordered children (`ChildIndex`), and links for
  what each one points at (a virtual list's slots: `RowTargetLink` to the entity a slot shows, `ListStartLink` to
  the first shown row).

### Data lives in entities

- **Rely on flecs: keep data in entities and walk them** (queries, children, links). Don't copy rows into an array
  or malloc per refresh. A list of thousands of entities spawns only the rows that show and walks the tree to fill
  them, it doesn't mirror the tree.
- **Derive what you can** (a count from a walk, a search from its bar's text field) instead of storing a copy that
  can go stale.
- **Few components, one value each**, generic ones reused (onboarding, House Rules).

### Spawns take a frame

A spawn (or a set, an add, a delete) made inside a system, an observer or a click doesn't happen straight away: flecs
queues it and applies it at the end of that step (its merge), so the rest of that code still sees the world as it
was. In practice:

- **A just spawned entity's values can't be read yet** (`zox_getv` on it gives nothing). Pass the values you already
  have (`scrollview_fit_list_size(world, list, size, ...)`), or react in an observer once it's there.
- **Put the event tag on the entity set up last**: the queued changes land entity by entity, so a window's `Dirty`
  fires before its list has its components, the list's after (the hierarchy, the files window, the search bar).
- **A just deleted entity still looks alive in the same code**: hide an element and reuse it (the search bar) rather
  than delete and relayout around it at once.

### Code goes where it belongs

- **A feature lives in the module that owns that kind of thing, with its tests there**: docking in `src/ui/docks`, the search bar
  in `src/ui/windows`, masks in `src/ui/masks`, the dropdown and virtual list in `src/ui/elements2`; the editor only
  uses them. Don't add code to whichever module happens to need it first.
- **Load order is the module order** (`src/modules.c`, each `_.c`): don't reach into another module's includes to
  get at something early. Move the code to the module that owns it.
- **`zox_` wrappers, not `ecs_*`**, outside `src/_/flecs`. Add a wrapper there when one is missing (`zox_get_roots`).

## Events vs Systems

Use an **event** when something happens once because of a specific change or interactions.

Use a **system** when something needs to be checked, processed, or updated repeatedly over time.

A useful rule is:

> **“When X happens, do Y” → Event**
> **“While X is true, keep doing Y” → System**

Examples of event-based logic:

```text
UI selected
→ show tooltip

UI deselected
→ close tooltip

Button pressed
→ perform action

Item activated
→ open item information
```

Examples of system-based logic:

```text
While selected
→ update animation

Every frame
→ update movement

While active
→ process timer

Every tick
→ update physics
```

For UI code in particular, prefer events for one-shot interactions. Avoid creating a system that continuously scans entities just to detect a state transition that could have been handled when the transition occurred.

For example, a skill tooltip should preferably work as:

```text
Skill selected
→ skill tooltip event
→ set tooltip text
```

rather than:

```text
Every frame
→ scan skill icons
→ check SelectState
→ detect selected skill
→ set tooltip text
```

A useful question when confused is:

> **Could this logic be described as “when X happens, do Y”?**

If yes, it is probably an event.

This is also useful for reducing ECS system overhead. Systems should generally be reserved for work that genuinely needs continuous or periodic processing, while events handle discrete changes and one-shot interactions.


## Events on tags

An add event (`zox_on_add`) only fires when the tag **wasn't already there**.

- A tag that lingers, or can be added again before it's removed, misses events. `Update` is one: a
  process dirtied again keeps its `Update`, so quest nodes hooked on it never ran.
- Hook events on a tag added once per transition, i.e. `Begin` when a process moves to a node
  (see [nodes.md](../tech/nodes.md)), or a state component going `zox_dirty_trigger` → `zox_dirty_active`.
- When an event doesn't seem to fire, log at the top of the callback first: was it called at all?

## Which kind of event (measured)

`tst flecs_event_costs` and `tst flecs_event_scaling` time each way (numbers in [tech/states.md](../tech/states.md)):

- **An event is a non fragmenting tag (`zoxd_nf_tag`) and an observer that removes it.** It never moves the entity
  between tables, so it costs ~1.25us an event at any count and any entity size: the cheapest way at every count
  on real entities (1.5 to 2.7 times under a system).
- **A normal tag moves the entity** to another table when added and again when removed, copying every component:
  on an entity with 20 components an event costs twice as much, and a system handling 10000 a frame 3.4us each.
- **Events come from where they happen, top down.** The module that does a thing emits its event (`TownLeft` from
  `in_town_system`); others observe it. Not an `OnRemove` that deleting the entity fires too, filtered after by
  reading another module's state (the game ending, Dead): that couples modules, and was a bandaid each time.
- **A system for the work, not the event.** Systems run on worker threads and spread heavy work (meshing, lighting,
  generation); an observer runs on the main thread at the merge. When the reaction is heavy, the event marks what
  to work on and a system does it.
- **Byte states cost while idle**: their increment system walks every entity that has one, every frame. Tags cost
  nothing until added; an idle system or observer is nearly free (flecs skips empty queries).
- An observer fires at the merge after the tag lands, a frame sooner than a system on `Update`: check what it reads
  is ready (a grid laid out before its icons' sizes settled showed wrong tooltips).
- Changing an event's timing changes behaviour: test the flows it drives (the game's end needed its 3 frame state).

## Allocated Components

Most components are plain values. A few hold memory, and free it when they're removed:

- **Arrays** (`zoxc_arrayd`, `zoxc_text` i.e. `TextData`, `zoxc_entities`): a length and a pointer
- **Node trees** (`VoxelNode`): voxel octrees, see the `octree_*` tests
- **Hashmaps**: lookups by id or name, i.e. files by name

See [coding_standards.md](../coding_standards.md#components) for declaring them.
