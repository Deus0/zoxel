# Onboarding

Welcome to Zoxel. This guide is for your first day: you know C, and you want to
understand how the project fits together and start doing tasks on it.

Read it top to bottom once. After that, use the links to the other docs as you
need them.

## 1. The Big Picture

- Zoxel is a procedural voxel RPG written in **C (gnu99)**.
- It's built on **[Flecs](https://www.flecs.dev)**, an ECS (Entity Component System) library.
- It uses **SDL3** for windows, input and audio, and renders with **OpenGL / OpenGL ES** or **Vulkan**:
  both backends draw everything and their screenshots match. OpenGL is the default,
  build with `--vulkan` for Vulkan ([Vulkan](tech/vulkan.md)).
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
  See [Architecture](engine/architecture.md) for when to use an event and when to use a system.

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

### Platforms

The engine builds for these, all from the same code:

| Platform | Arch | Rendering | Build |
|---|---|---|---|
| Linux (and SteamOS) | x64, arm | OpenGL, Vulkan | `bash bsh/build.sh` (`--vulkan`), release zips with `bash bsh/docker.sh` |
| Windows | x64 | OpenGL, Vulkan | `bash bsh/windows.sh` in git bash (installs gcc with winget if it's missing) |
| Android | arm | OpenGL ES, Vulkan | `bash bsh/android.sh zoxel --release --signed` (`--vulkan`), see [Android](engine/android.md) |
| Android XR (OpenXR) | arm | OpenGL ES, Vulkan | `bash bsh/android.sh zoxel --release --signed --xr` |
| Web (emscripten) | wasm | WebGL 2 | `bash bsh/web.sh zoxel --release` |

- Windows headers define macros with common names (`far`, `near`, `min`, `max`): don't use them as identifiers,
  or the Windows build breaks while Linux is fine. Build with `bsh/windows.sh` when you touch shared code.
- Platform code sits behind `#ifdef` (`zox_windows`, `zox_linux`, `zox_android`, `zox_web`, `zox_xr`), so keep it out of
  gameplay modules.
- Every target, its output and CI's nightly builds: [DevOps](engine/devops.md).

Useful game flags (pass them to the binary, the full list is in [Commands](engine/commands.md)):

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
gam/                the games (zoxel, zixel, zedit, zeland...) built on the engine
bsh/                build, package and test scripts
doc/                these docs: engine/ (how the engine works), tech/ (each module), gdd/ (game design),
                    plans/, todo.md and done.md
res/                resources (textures, models, sounds)
```

There are about 90 modules (`src` has 92 folders). [modules.c](../src/modules.c) includes them in depth order,
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
    [none] voxel_terrains.TerrainChunk,   // must have, but not read (a filter)
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

### Hierarchies

Entities form trees with parents and children ([Children](tech/hierarchy.md) has the details):

- `zox_set_parent(world, child, parent)` makes a child, and `zox_set_parent_non_fragment` does the same without a
  table per parent, for things there are many of (stats, skills, ui). Children are deleted with their parent.
- Find children with `zox_get_children_by_id(world, e, out, capacity, zox_id(Tag))`, by a tag or component they
  have, or with `zox_get_child_by_id`.
- **Transforms follow parents.** A child's `Position3D` and `Rotation3D` come from its parent's, plus its
  `LocalPosition3D` / `LocalRotation3D` (zero if it has none). `DisableParentTransform` keeps its own.
- **Prefabs have children too.** Parent an entity to a prefab inside its prefab function (the gamepad prefab's
  buttons, a projectile's particle trail), and every instance gets its own copy. Prefer this to spawning children by
  hand after each instance.
- **Inside a system, an instance's copied children aren't there yet** (flecs defers the spawn), so you can't set
  data on them then. Give the child what it needs from its parent instead (a trail takes its projectile's `Color`).
- A prefab of a prefab (a variant, `zox_prefab_child` / `zox_prefab_variant`) builds on its base by inheritance,
  not as its child, and its instances get the base's children too.

## 5. House Rules

These matter more than they first look:

- **Read [Architecture](engine/architecture.md) "Patterns we stick to" first.** Reactions are observers on event tags
  (the core `Dirty` most often), entities point at each other with links, data lives in entities, and each feature sits in the module that
  owns it, with its tests. New code follows them; reviews check for them.

- **Fix the cause, not the symptom.** Find out why something breaks before changing code, then fix it where it
  starts, in a way that still holds as the game grows. A quick override where it shows (forcing a value at draw
  time, a special case for one entity, a flag to skip a check) hides the bug, and it breaks later features: the sky
  forced to no rotation in xr would have broken worlds that rotate, and the real cause was its shader's noise, per
  screen pixel. If you can't find the cause, say so and test to narrow it down, rather than patching round it.

- **Use the `zox_` wrappers, never Flecs directly.** `ecs_*` / `Ecs*` belong only in [src/_/flecs](../src/_/flecs).
  If a wrapper is missing, add one there.
- **Few components, each one value.** Reduction is a key part of the design: reuse what exists before making a new
  one. Look in core first ([src/_/com](../src/_/com/_.c): `Target`, `Count`, `Busy`, timing's
  `DestroyInTime`), then the modules you use (`Color`, `Bounds3D`, `Gravity3D`, `Scale3`, a link). Derive data you can
  work out (a heading from a target, a count from the time) rather than storing it. Keep each component one value:
  never pack several into a struct or a vector of different meanings. Organise with generic components and tags: a
  burn is a debuff tagged `Burn` with `Damage`, `Rate` and `DamageTime`, not a `BurnRate`; the realm spawns it once
  and skills link it. Prefer tags to byte states (`SkillHit`, not a
  stage of 2): systems filter on tags. More in [Coding Standards](coding_standards.md) (Generic Components and Tags). Spot old code that could share or drop components? Suggest it, but ask first:
  it changes the design.
- **Links, not raw entity fields.** To point one entity at another, use `zox_link`, see [Links](engine/links.md).
  For parents and children, see [Children](tech/hierarchy.md).
- **Cleanup goes in unlink callbacks** (`zox_on_unlink`), not at every place that deletes something.
  Deletion code shouldn't need to know what to clean up.
- **Events for "when X happens", systems for "while X is true"**, see [Architecture](engine/architecture.md). An event is
  a non fragmenting tag and an observer that removes it (measured cheapest); heavy work stays in systems, they run
  on threads. [States](tech/states.md) has which form to use for a change.
- **Don't depend on frame rate.** Scale by `zox_delta_time`, or use rates per second, not "N per frame".
  Several streaming bugs came from this.
- **Systems are multithreaded.** Flecs pipelines order them; don't add locks to dodge a race.

## 6. Logs

The log stays short: someone reading it should see what matters, not every boot step. A minute of play logs about
ten lines.

| Call | For | Shown |
|---|---|---|
| `zox_loge(...)` | something is wrong (counted: a test fails after 100) | always |
| `zox_logw(...)` | something is off but the game goes on | always |
| `zox_log(...)` | rare lines worth reading every run | always |
| `zox_logv(...)` | details: paths, settings chosen, events like a splash | only with `--verbose` |

- Expected cases aren't errors: a missing optional folder, an icon wayland refuses. Use `zox_logv`, or nothing.
- Nothing every frame. To trace a system while you debug it, a local flag keeps it quiet otherwise:
  `byte dbg_log = 0;` then `if (dbg_log) zox_log(...)`.
- Release builds drop every log (their arguments are never run). Debug builds with `--logs` show them, `--verbose`
  adds the details.
- Tests log with a `[tst]` prefix; tst.sh reads those lines, so keep that prefix for test output.
- More in [Coding Standards](coding_standards.md) (Logs).

## 7. Tests

There's an automated test harness. Run it after changes, and **before every commit**:

```bash
bash bsh/tst.sh list            # list tests
bash bsh/tst.sh player_spawn    # build debug + run one test
bash bsh/tst.sh all             # run every test
bash bsh/tst.sh stream_npcs --repeat 5   # for intermittent bugs
bash bsh/tst.sh <name> --gdb    # backtrace on a crash
bash bsh/tst.sh <name> --vulkan # the same test on the vulkan backend
```

- Exit code 0 means pass. A failed run's log is kept in `tst/<name>_failed_*.log`.
- Tests run the real game with `--tst <name>`, hidden, with software rendering, capped memory and a timeout.
  A bad run can't freeze the machine. `--gpu` / `--visible` use the real GPU.
- Tests are written in a module's `tst/` folder and registered with `zox_tst_add(name, ready, check, timeout)`.
  The harness is [src/_/tst/_.c](../src/_/tst/_.c).
- When you fix a bug, add a test that would have caught it.
- Writing tests, patterns, tst.sh's options and every test by area: [tests.md](engine/tests.md).
- To reproduce a bad save: `zox_tst_save=<realm save folder> bash bsh/tst.sh continue_game`.
  It loads a copy and never writes to the real save.

### Small and fast: test only what you check

A full run is over 300 tests, so a second saved in one test is minutes saved a day. Reduce a test to what it checks:
everything else it spawns or waits for is time, and another way for it to fail for a reason that isn't its own.

What a test starts with, cheapest first (times measured 2026-11-20, debug build):

| A test needs | Ready | e.g. |
|---|---|---|
| nothing: pure logic | the first frame (the game's boot, ~0.5s) | maths, octrees, hashmaps |
| bare entities, the components a system reads | a few frames | transform_hierarchy, mass_spawn |
| the menus (`zox_tst_set_setup(name, NULL, 1)`): a camera, ui, sounds, no realm | under a second | gpu_restore, music_timing |
| a realm and a chunk3 parent of its own: lit chunks, no terrain (`zox_tst_chunk3_parent_ready`) | a few seconds | water_light |
| a world of its own and a streamer | a few seconds | air_chunk |
| a player on the test level (`zox_tst_level_setup`) | ~6s | held_item |
| a player in the whole game | ~6s, then the game streams | stream_npcs |

- **No player unless it's about the player.** A player brings a realm, terrain, npcs and towns. A camera is
  `main_cameras[0]` in the menus; a render test needs what it draws, not who's watching.
- **A camera in a void.** To look at what a camera draws (post processing, lids, lines), use the menus' camera with
  the skybox `Disabled` and a plain clear `Color` (tst eyelids, 0.2s; it waited for a player for 9s).
- **Wait on a condition, not a time.** "Until the counter goes up" or "until the note moves on", not "3 seconds" or
  "60 frames", with a cap so a broken build still fails fast. A fixed few frames isn't a condition either: some things
  draw a few frames late, and the test turns flaky (vox_instance_render failed 1 run in 3 that way).
- **Spawn many in one go** with `zox_bulk_spawn` (src/_/flecs/fun/system_times.c), straight into their table, unless
  spawning one at a time is what's tested (mass_spawn). Test ready functions are immediate, so they can.
- **No cleanup at the end.** `zox_tst_end` exits straight after the check.
- **Write what you look at, once.** A screenshot or wav to check by eye is cheap. Rendering extra ones nothing checks
  costs every run: put those behind a flag (like `--readme`).
- **Measure it.** `tst/<name>.log` has timestamps: from `[tst] running` to `PASSED` is the test's own time.

[Coding Standards](coding_standards.md) (Test Placement) has where a test goes and the lowest module it can live in.

## 8. Doing a Task

Tasks are tracked in plain markdown:

| File | Holds |
|---|---|
| [todo.md](todo.md) | tasks to do, refactors in its own section |
| [todo.md](todo.md#optimizations) | performance work |
| [todo.md](todo.md#future) | bigger ideas for later |
| [gdd.md](gdd.md#goals) / [todo.md](todo.md#new) | bigger features and new modules |
| [done.md](done.md) | finished work: move items here, marked `-x`, with a line on the cause and fix |

The loop:

1. Pick a task from [todo.md](todo.md).
2. Find the module: search for a component or system name from the task (`grep -rn NpcSpawnZone src`).
3. Read that module's `_.c` and `sys/_.c` first, to see what it owns and what runs.
4. Make the change in the module's style. Logs help: `zox_log` behind a `dbg_log` flag in a debug build (section 6).
5. Run the related test, then `bash bsh/tst.sh all`.
6. Move the item to [done.md](done.md), then commit the task: its code, tests and docs in one commit.

## 9. Git

- Work on `master`, a commit per task: the task's code, its tests and its docs together, not a commit per step or
  per fix along the way (fold those into the task's commit before pushing). Short lower case messages (`fixed windows
  build errors`). Several people commit here: check `git status` and `git log` first, and only commit your own
  files, by path.
- **Commit messages:**
  - The header (first line) is one contained idea or task, not a list: usually under 5 words, lower case
    (`jumping into doors`, `gravity any direction`). No commas or "and" joining several changes: those go in the
    dot points, or in commits of their own.
  - Keep it to the change itself: nothing about how it was made, and no trailers at the end.
  - Details go after a blank line, as dot points:

  ```text
  windows crash logs

  - the crash handler writes the faulting module and its offset
  - test logs are unbuffered, so a crash keeps its last lines
  ```

- Pull with rebase, so the history stays one line: `git config pull.rebase true` (a plain pull merged and split it).
- **Two remotes in one.** `origin` fetches from GitHub, but pushes to GitHub *and* Codeberg:

  ```bash
  git remote -v
  # origin  git@github.com:deus0/zoxel.git (fetch)
  # origin  git@github.com:deus0/zoxel.git (push)
  # origin  git@codeberg.org:deus/zoxel.git (push)
  ```

  So `git push origin master` sends to both, but `git pull` only sees GitHub. If a push lands on one and is
  refused by the other (it's behind, someone pushed only there), the two drift apart and every later push stalls.
- **When they've drifted**, don't force push. Bring the other one in, then push to both:

  ```bash
  git fetch git@codeberg.org:deus/zoxel.git master   # what Codeberg has (FETCH_HEAD)
  git log --oneline HEAD..FETCH_HEAD                  # commits only it has
  git merge FETCH_HEAD                                # or rebase unpushed work onto it
  git fetch origin && git merge origin/master         # and GitHub's, if it has any of its own
  git push origin master                              # now a fast forward on both
  ```

  To check they match: `git ls-remote git@github.com:deus0/zoxel.git master` and the same for Codeberg give the
  same hash.

## 10. Where Next

- [Architecture](engine/architecture.md): the patterns we stick to (observers on event tags, links, data in
  entities, deferred spawns, code in its module), events vs systems
- [Coding Standards](coding_standards.md): folders, naming, components and systems in depth
- [Links](engine/links.md): `zox_link`, single and many links, unlink callbacks
- [Children](tech/hierarchy.md): parenting and child entities
- [Characters](tech/characters.md): how characters are built (vox, renderer, skeleton)
- [Bones and Animation](tech/bones.md): joints, the walk, crouching, swings, XR's body, looking at it (gifs, `--bones`)
- [Body and Equipment](tech/body_equips.md): body parts in slots, how a body is merged and boned, gear (hats, shirts)
- [Stats](tech/stats.md): levels, health and regens, base stats and the attributes raising them
- [Audio](tech/audio.md): the mixer, 3D sounds, effects, voices and music
- [Sounds](tech/sounds.md): what things sound like hit, realm sound entities, materials, grunts
- [Nodes](tech/nodes.md): node graphs, how dialogue and quests walk them
- [Tests](engine/tests.md): writing tests, patterns, every test by area
- [Metrics](engine/metrics.md): rendering speed per backend and how alike they draw, a report a run
- [Tools](engine/tools.md): scripts that look into the game (world generation's maps as images), bsh/tools
- [Systems](engine/systems.md): the phases in a frame, kinds of systems, budgets, profiling
- [Thread Safety](engine/thread_safety.md): workers, merges (sync points), octree phases and locks, `tst pipeline_syncs`
- [Flecs Patches](engine/flecs_patches.md): our fixes to flecs (bsh/flecs_fix.sh), why, repros, for upstream PRs
- [Spawning](engine/spawning.md): how entities are made, what it costs, keeping it cheap
- [Engine](engine/_.md) and [Tech](tech/_.md): the indexes of every engine and module doc
- [Vulkan](tech/vulkan.md): the vulkan backend, how it works and how to test it
- [Android](engine/android.md): building, installing and debugging the apks
- [New Game](howto/newgame.md): making a game in `gam/`, its modes, states and tests
- [Fast Tests](howto/tests.md): small, fast tests for edge cases, a function on data set by hand
- [Game Design](gdd.md): the blend, the pillars, what's playable and planned
- [Settings](tech/settings.md): a module's settings, how a change applies, save and load
- [States](tech/states.md): change events, tags over byte states, which form to use, timing
- [Commands](engine/commands.md): every command line flag for the game binary
- [Help](help.md): make targets
- [Flecs docs](https://www.flecs.dev/flecs/): the ECS underneath, for when the wrappers aren't enough
