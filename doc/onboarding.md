# Onboarding

Welcome to Zoxel. This guide is for your first day: you know C, and you want to
understand how the project fits together and start doing tasks on it.

Read it top to bottom once. After that, use the links to the other docs as you
need them.

## 1. The Big Picture

- Zoxel is a procedural voxel RPG written in **C (gnu99)**.
- It's built on **[Flecs](https://www.flecs.dev)**, an ECS (Entity Component System) library.
- It uses **SDL3** for windows, input and audio, and **OpenGL / OpenGL ES** for rendering.
- One codebase builds several games. The games live in [gam](../gam), and `zoxel` is the main one.

### ECS in 60 seconds

If ECS is new to you, this is all you need to start:

- An **entity** is just an id (`entity` is a 64 bit integer). It has no data of its own.
- A **component** is a small piece of data attached to an entity, e.g. `Position3D`, `RenderDistance`.
  A component with no data is a **tag**, e.g. `Npc`, `Chunk3`.
- A **system** is a function that runs every frame over every entity that has a set of components.
  You don't loop over the world yourself: you declare "I want entities with A and B", and Flecs hands you arrays of them.
- A **prefab** is a template entity. New entities are instanced from prefabs, so they start with the right components.
- An **event** reacts once when something happens, rather than checking every frame.
  See [Architecture](architecture.md) for when to use an event and when to use a system.

The whole game is entities: the window, the player, the camera, every chunk of
terrain, every NPC, every UI button.

## 2. Build and Run

Do this once, to fetch and patch Flecs into `inc/flecs`:

```bash
bash bsh/flecs.sh
```

Then:

| Command | What it does |
|---|---|
| `make build` / `make run` | release build / build + run it |
| `make dev` / `make rund` | debug build with logs, then runs it |
| `make gdb` | debug build under gdb |
| `bash bsh/build.sh zoxel --debug --logs` | debug build only, without running |
| `make help` | all make targets, also in [help.md](help.md) |

Binaries go in `bin/zoxel_<os>_<arch>_..._dev/` (debug) and similar folders for release.
`bin`, `inc`, `lib`, `obj` and `ext` aren't in git.

Release builds strip `zox_log`, so use a debug build when you want to see logs.

Useful game flags (pass them to the binary, the full list is in [Commands](commands.md)):

| Flag | Effect |
|---|---|
| `--autostart` | skips the menus, straight into a new game |
| `--tiny` / `--medium` / `--large` | terrain stream size |
| `--windowed` / `--fullscreen` | window mode |
| `--headless` | no window |
| `--background` | hidden window, no sound (what tests use) |
| `--novsync` / `--vsync` | vsync |

## 3. Where Things Live

```text
src/
    main.c          the entry point: init flecs, add modules, spawn the game, run the loop
    modules.c       which modules the engine imports
    _/              the foundation: flecs wrapper, maths, collections, octrees, logs, memory
    core/           core engine modules (timing, settings, the test harness)
    <module>/       one folder per subsystem: npcs, terrain, chunks, ui, skills, combat...
gam/                the games (zoxel, zixel, zedit...) built on the engine
bsh/                build, package and test scripts
doc/                these docs, plus the todo / bugs / done lists
res/                resources (textures, models, sounds)
```

There are about 90 modules. [modules.c](../src/modules.c) includes them in depth order,
from foundations (`core`, `transforms`, `inputs`) up to gameplay (`npcs`, `quests`, `combat`),
so a module can use anything included before it.

### Inside a module

Every folder has a `_.c` that `#include`s its contents. There are no headers:
the whole game compiles as **one translation unit** (`src/main.c`, plus `flecs.c`). So
include order matters. Something defined in a module is visible to everything
included after it.

The usual folders inside a module:

| Folder | Holds |
|---|---|
| `_.c` | the module header comment, includes, and `import_<module>(world)` |
| `com/` | components and tags (`zoxc_*` declares, `zoxd_*` defines) |
| `dat/` | plain C structs and data |
| `set/` | settings and global variables |
| `pre/` | prefabs (templates for new entities) |
| `ins/` | instancing: `spawn_*` functions that create entities from prefabs |
| `fun/` | plain helper functions |
| `eve/` | events (one-shot reactions) |
| `sys/` | systems, and `sys/_.c` registers them with `zox_system(...)` |
| `io/` | saving and loading files |
| `dbg/` | debug tools and spawners |
| `tst/` | automated tests |

[src/npcs](../src/npcs) is a small, readable example: start with its
[_.c](../src/npcs/_.c), then [sys/_.c](../src/npcs/sys/_.c), then
[sys/can_spawn.c](../src/npcs/sys/can_spawn.c).

[Coding Standards](coding_standards.md) has the full rules for folders, naming and layout.

## 4. Reading a System

Systems are registered in a module's `sys/_.c`:

```c
zox_system(
    characters_can_spawn_system,       // the system
    zoxp_update,                    // the pipeline phase it runs in
    [in] rendering.RenderDepth,     // read only
    [in] rendering.RenderDistance,
    [out] npcs.NpcSpawnZone,        // written
    [out] npcs.NpcSpawnZoneDirty,
    [none] terrains.TerrainChunk,   // must have, but not read (a filter)
);
```

And the system reads its fields **in the same order** as they were declared:

```c
void characters_can_spawn_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepth);        // field 0
    zox_sys_in(RenderDistance);     // field 1
    zox_sys_out(NpcSpawnZone);      // field 2
    zox_sys_out(NpcSpawnZoneDirty); // field 3
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();                              // e = the current entity
        zox_sys_i(RenderDistance, distance);      // a const pointer
        zox_sys_o(NpcSpawnZone, active);          // a writable pointer
        if (distance->value <= character_spawn_distance) {
            active->value = 1;
        }
    }
    zox_sys_on_end();
} zoxd_system(characters_can_spawn_system);
```

Newer systems are written as plain functions, `void npc_spawn_system(iter* it) { ... } zoxd_system(npc_spawn_system);`,
with the same `zox_sys_*` macros inside. [Coding Standards](coding_standards.md) shows that style.

The fields are **positional**: `zox_sys_in` / `zox_sys_out` must match the order in
`zox_system`, and `[none]` terms go last.

Outside systems, you get and set components on one entity:

```c
zox_getv(e, RenderDistance)          // the value
zox_get(e, VoxelNode)                // a const pointer, for bigger components
zox_setv(e, RenderDistance, 3)       // set a value
zox_has(e, Npc) / zox_add(e, Npc) / zox_remove(e, Npc)
zox_valid(e)                         // is it still alive?
zox_delete(e)
zox_log("x is [%i]", x) / zox_logw(...) / zox_loge(...)   // log, warning, error
```

## 5. House Rules

These matter more than they first look:

- **Use the `zox_` wrappers, never Flecs directly.** `ecs_*` / `Ecs*` belong only in [src/_/flecs](../src/_/flecs).
  If a wrapper is missing, add one there.
- **Small components.** Prefer several singular components over one big struct.
- **Links, not raw entity fields.** To point one entity at another, use `zox_link`, see [Links](links.md).
  For parents and children, see [Children](children.md).
- **Cleanup goes in unlink callbacks** (`zox_on_unlink`), not at every place that deletes something.
  Deletion code shouldn't need to know what to clean up.
- **Events for "when X happens", systems for "while X is true"**, see [Architecture](architecture.md).
- **Don't depend on frame rate.** Scale by `zox_delta_time`, or use rates per second, not "N per frame".
  Several streaming bugs came from this.
- **Systems are multithreaded.** Flecs pipelines order them; don't add locks to dodge a race.

## 6. Tests

There's an automated test harness. Run it after changes, and **before every commit**:

```bash
bash bsh/tst.sh list            # list tests
bash bsh/tst.sh player_spawn    # build debug + run one test
bash bsh/tst.sh all             # run every test
bash bsh/tst.sh stream_npcs --repeat 5   # for intermittent bugs
bash bsh/tst.sh <name> --gdb    # backtrace on a crash
```

- Exit code 0 means pass. A failed run's log is kept in `bin/tst/<name>_failed_*.log`.
- Tests run the real game with `--tst <name>`, hidden, with software rendering, capped memory and a timeout.
  A bad run can't freeze the machine. `--gpu` / `--visible` use the real GPU.
- Tests are written in a module's `tst/` folder and registered with `zox_tst_add(name, ready, check, timeout)`.
  The harness is [src/core/tst/_.c](../src/core/tst/_.c).
- When you fix a bug, add a test that would have caught it.
- Writing tests, patterns and every test by area: [tests.md](tests.md).
- To reproduce a bad save: `zox_tst_save=<realm save folder> bash bsh/tst.sh continue_game`.
  It loads a copy and never writes to the real save.

## 7. Doing a Task

Tasks are tracked in plain markdown:

| File | Holds |
|---|---|
| [todo.md](todo.md) | tasks to do |
| [bugs.md](bugs.md) | known bugs |
| [refactors.md](refactors.md) | code that needs restructuring |
| [optimize.md](optimize.md) | performance work |
| [goals.md](goals.md) / [new.md](new.md) | bigger features and new modules |
| [done.md](done.md) | finished work: move items here, marked `-x`, with a line on the cause and fix |

The loop:

1. Pick a task from [todo.md](todo.md) or [bugs.md](bugs.md).
2. Find the module: search for a component or system name from the task (`grep -rn NpcSpawnZone src`).
3. Read that module's `_.c` and `sys/_.c` first, to see what it owns and what runs.
4. Make the change in the module's style. Logs help: `zox_log` in a debug build.
5. Run the related test, then `bash bsh/tst.sh all`.
6. Move the item to [done.md](done.md), then commit.

## 8. Where Next

- [Architecture](architecture.md): events vs systems
- [Coding Standards](coding_standards.md): folders, naming, components and systems in depth
- [Links](links.md): `zox_link`, single and many links, unlink callbacks
- [Children](children.md): parenting and child entities
- [Characters](character.md): how characters are built (vox, renderer, skeleton)
- [Audio](audio.md): the mixer, 3D sounds, effects, voices and music
- [Nodes](nodes.md): node graphs, how dialogue and quests walk them
- [Tests](tests.md): writing tests, patterns, every test by area
- [Systems](systems.md): the phases in a frame, kinds of systems, budgets, profiling
- [Thread Safety](thread_safety.md): workers, merges (sync points), octree phases and locks, `tst pipeline_syncs`
- [Flecs Patches](flecs_patches.md): our fixes to flecs (bsh/flecs_fix.sh), why, repros, for upstream PRs
- [Spawning](spawning.md): how entities are made, what it costs, keeping it cheap
- [Game Design](gdd.md): the blend, the pillars, what's playable and planned
- [Commands](commands.md): every command line flag for the game binary
- [Help](help.md): make targets
- [Notes](notes.md): assorted engine notes
- [Flecs docs](https://www.flecs.dev/flecs/): the ECS underneath, for when the wrappers aren't enough
