# Modularity: modules compiled separately

Today the whole game is one translation unit: `src/main.c` includes every module's `_.c` (src/modules.c), and the game
(`gam/<game>/_.c`) last. One file means no headers and free inlining, but every change rebuilds all ~190k lines, one
compiler error hides the rest, and nothing stops a module using one that's included after it (`extern` round the
order).

The goal: each module is its own object file (`obj/<build>/<module>.o`), built in parallel and only when it or a
header it reads changed, then linked. The game plays and the tests pass the same as before, on every platform.

This is a core design change: each phase below is agreed before it starts, and the codebase is never left half done
on master (every step builds and passes its tests).

## Numbers (2026-11-25)

| | |
|---|---|
| modules (src/modules.c) | 88, plus `_` (the foundation, the `core` flecs module) and the game (`gam/zoxel`) |
| .c files, lines | 3127, ~190k |
| non static functions | ~3800 |
| `static inline` functions | ~2100 |
| globals (not static) | ~2750, 61 static |
| component and tag declarations (`zoxc_*`, `zox_tag`) | ~1500, from 56 macros |
| systems (`zoxd_system`) | 620 |
| hooks (`zox_hook`) | 10 |
| `extern` lines | ~200 (the most: ui 26, huds 22, `_` 14, terrain 10) |

## The shape it ends in

```text
src/_/_.h            the foundation's header: defines.c, includes.c, maths, collections, the flecs wrappers' macros
src/_/_.c            the foundation's code, its own object
src/<module>/_.h     everything the module shares: typedefs, structs, #defines, component and tag ids (extern),
                     shared globals (extern), prototypes, the static inline helpers other modules call
src/<module>/_.c     includes its own _.h (and the _.h of each module it uses), then its folders as now
src/main.c           includes the headers it needs, and main()
gam/<game>/_.c       the same as a module
```

- One header per module, not per file: the folders (`com/`, `fun/`, `sys/`..) stay as they are.
- A module's `_.h` includes the `_.h` of the modules it uses, each guarded (`#pragma once`).
- A module may only include headers of modules before it in src/modules.c (as now), so there's no cycle.
- Anything only one module uses becomes `static` (functions, globals): it can't clash, and the compiler can inline it.
- Tests stay in their module (`tst/`, under `zox_tests`), compiled into its object.

## Rules for every step

- One module per commit, in src/modules.c order, starting with `_`.
- After a step: `bash bsh/build.sh zoxel --debug --logs`, the release build, the windows build
  (`bash bsh/build.sh zoxel --windows --x64 --debug --logs --out zoxel_win` cross compiles with mingw on linux), then
  the module's own tests and `bash bsh/tst.sh all` before each phase ends.
- No behaviour changes in these commits: only moving declarations, adding `static`, `extern` and prototypes.
  If something has to change to split (a backward dependency, below), that's its own commit first, with a test.
- Keep the names: a function keeps its name when it moves to a header.
- Don't add new macros unless a step below names one; extend the existing `zoxc_*` ones.

## Phase 0: measure and map

1. Time a clean debug build and a release build (`time bash bsh/build.sh zoxel --debug --logs`). Write the numbers
   here, under Results.
2. Record performance: `bash bsh/tst.sh perf_terrain --gpu` and `perf_stream` (doc/engine/metrics.md), 3 runs each, a
   fixed seed. Separate objects lose inlining across modules; these are what's compared at the end.
3. Make a dependency map, `bsh/tools/module_deps.sh`, plain bash with what the build already has (gcc, binutils,
   grep, awk). While the game is one file, one compile sees everything, macros expanded:
   - prototypes: compile `src/main.c` once with `-c -g -aux-info obj/protos.txt` (the debug build's flags). Each
     line is `/* src/<module>/..c:<line>:NF */ extern <prototype>;`, also for functions a macro generated, and
     `static` ones are marked static. Its own headers' lines (`/usr/..`) are skipped
   - globals and functions: `nm -l --defined-only` on that object. `T` is a function, `D` / `B` a global, lower case
     ones are already static; `-l` adds the `file:line` each came from, so the module is the path's second part
   - components and tags: `grep -rhoE '(zoxc[a-z_0-9]*|zox_tag[a-z_]*)\(\w+' src/<module>` (they're
     variables too, `nm` shows them, but this names the macro)
   - `#define`s and typedefs: `grep -rnE '^\s*#define \w+|^typedef|^} \w+;' src/<module>`
   - for each name, `grep -rlw <name> src gam` gives the modules that use it. Write `module uses module: names` lines to
     `doc/data/module_deps.txt`, and per module a list of its names nobody else uses (`doc/data/module_private/<m>.txt`)
   - an edge from a module to one after it in src/modules.c is a **backward dependency**: list them all at the end
   - grep for every name is slow (thousands of names, 3000 files): `grep -rowhE` once per module with all its names in
     one pattern file (`-f`), then count
4. A generator, `bsh/tools/module_header.sh <module>`, writes a draft `src/<module>/_.h` from the same data (phase 3
   uses it): the module's prototypes other modules call (from protos.txt), `extern` lines for its shared globals
   (from nm, the type read from the definition's line), and a list of the `#define`s, typedefs and `static inline`
   functions others use, to move by hand. The draft is checked and fixed by hand, never committed unread.
5. Each backward dependency gets fixed before its module is split, one of:
   - move the code down to the lower module, if it only uses lower things
   - a hook (`zox_hook`, src/_/collections/macros/hook.c): the lower module runs it, the higher one adds to it
   - a function pointer component (`zoxc_function`)
   - for a test: move it to the lowest module that has everything it checks (coding_standards.md, Test Placement)

   Note each in doc/data/module_deps.txt as it's fixed. An `extern` to a later module still links once modules are
   separate, so these don't block a split, but each is a cycle to remove.

## Phase 1: the macros get a header form

Every macro that both declares and defines must be able to do just one. One switch decides which:

```c
// in a module's _.c, before it includes its own _.h:
#define zox_define_<module>
#include "_.h"
```

and in the module's `_.h`, its component lines are wrapped:

```c
#pragma once
#ifdef zox_define_npcs
    #define zox_here    // this file defines the ids
#endif
zox_tag(CharactersSpawned);
zoxc_byte(NpcSpawnZone);
#undef zox_here
```

1. In src/_/flecs/mcr/components.c and ids.c, the declaring macros check `zox_here`:
   - `zoxc_custom(T)`: `ECS_COMPONENT_DECLARE(T)` with `zox_here`, else `extern ECS_COMPONENT_DECLARE(T)`
   - `zox_tag(T)`: `ECS_DECLARE(T)` declares two variables (`T` and `ecs_id(T)`); the extern form is
     `extern ecs_entity_t T; extern ecs_entity_t ecs_id(T)`
   - the typedef part (`zoxc(T, type)`'s struct) is emitted either way
2. Functions the macros generate (`zoxc_arrayd`'s `dispose_T`, `resize_T`.., `zoxel_dynamic_array`'s
   `create_..`, the octree and hashmap macros): make each `static inline`. They then go in every object that uses
   them, and none clash at link time. `ECS_CTOR` / `ECS_DTOR` are already static.
3. `zoxd_system(T)` (`ECS_SYSTEM_DECLARE`): most systems are only named in their own module. Make it
   `static ECS_SYSTEM_DECLARE(T)`, and for the few used elsewhere (phase 0's map shows them) add a header line
   `extern ECS_SYSTEM_DECLARE(T)` and a plain one in the module.
4. `zox_hook` / `zox_hookr`: split into `zox_hook_h(name, args)` (the typedef and four prototypes, for headers) and
   the existing `zox_hook` (definitions, in the module's `_.c`). This is the one new macro.
5. Build the unity build exactly as before: with no `_.h` files yet, `zox_here` is never undefined in a header, so
   nothing changes. Commit.

Check every macro in src/_ that writes a variable or a non static function at file scope:
`grep -rn "^#define" src/_ ` then read each that ends in a definition. List them here as they're done.

## Phase 2: the foundation (`src/_`)

1. Create `src/_/_.h`. Move into it, in this order: `src/defines.c`, `src/includes.c` (as includes), then from each
   of src/_'s folders the macros, typedefs, structs and `static inline` functions (logs, collections, maths,
   flecs/mcr, flecs/comm, types, octrees..). Leave function bodies that aren't `static inline` in their `.c`, and
   put a prototype in `_.h`.
2. Globals in src/_ (e.g. `zox_disable_process_skips`, `zox_systems`, `zox_delta_time`): each used outside src/_
   gets `extern` in `_.h`; each used only in src/_ becomes `static`. Find users with
   `grep -rlw <name> src gam | grep -v "^src/_/"`.
3. `src/_/_.c` becomes: `#include "_.h"`, then its folders as now.
4. `src/main.c` stays the unity file for everything else for now: `#include "_/_.h"` at the top, `_.c` no longer
   included.
5. build.sh: `sources="src/_/_.c src/main.c inc/flecs/flecs.c"`. Build every platform (also check bsh/web.sh and
   bsh/android.sh, they name `main.c` themselves), run the tests. Commit.

From here the build is: split modules as separate objects, plus `main.c` holding all unsplit modules as one.

## Phase 3: each module, in src/modules.c order

For a module `m` (start with `transforms`, then `networking`, `screens`.. as listed):

1. Read `src/m/_.c` and each folder's `_.c` to see what's in it. Rerun `bsh/tools/module_deps.sh` (the code moved
   since), then `bsh/tools/module_header.sh m` for the draft header.
2. Create `src/m/_.h` from the draft:
   - `#pragma once`, then `#include "../_/_.h"` and the `_.h` of each module it uses (doc/data/module_deps.txt)
   - every `#define`, `typedef`, `struct` and `enum` in the module
   - its `com/` declarations (`zoxc_*`, `zox_tag`) moved here, inside the `zox_here` switch (phase 1). The
     `zox_components_m(world)` function with the `zoxd_*` lines stays in `com/_.c`
   - its `set/` globals: `extern` lines here, definitions stay in `set/`
   - a prototype for each non static function another module calls
   - each `static inline` function another module calls, moved here whole, from `fun/` (or wherever it is)
3. In the module's `.c` files: each function and global no other module uses becomes `static`
   (`doc/data/module_private/m.txt`; a `sed` on each one's definition line, the line `nm -l` gave).
4. `src/m/_.c` starts with `#define zox_define_m` and `#include "_.h"`.
5. In src/modules.c, `#include "m/_.c"` becomes `#include "m/_.h"`.
6. build.sh: add `src/m/_.c` to `sources`.
7. Build. The errors are the list of what's missing:
   - "undeclared" in `m`: it uses something of a lower module that isn't in that module's header yet. Add it there
   - "undefined reference" at link: a function or global is `static` but used elsewhere, or a prototype names a
     function that's only in another unsplit module (fine, it links), or it's `static inline` in a `.c` someone else
     uses: move it to the header
   - "multiple definition": a non static function or global with the same name in two objects: make one `static`
     if it's private, otherwise rename the newer one
   - a warning "declared static but never defined": a prototype in the header for a function that was made static
8. Run the module's tests (`bash bsh/tst.sh list | grep <m>`, and the tests in `src/m/tst`) on linux, and build
   windows. Commit: `m compiled separately`.

Submodules (`chunks/chunks3`, `ui/elements2`, `lights/lights3`, `particles/particles3`..) stay in their parent's
object and its header, as they're included now.

## Phase 4: the game and main

1. `gam/<game>/_.c` gets a `_.h` the same way (zoxel, then the other games in gam/). `main.c` includes it.
2. `main.c` ends up with only headers and `main()`.
3. Remove the `zox_nexus_game` include trick from src/defines.c: build.sh adds `gam/<game>/_.c` to `sources`.

## Phase 5: the build

1. `bsh/build.sh` compiles each source to `obj/<build name>/<module>.o` with `-MMD -MP` (a `.d` file listing its
   headers), only when the `.c` or any header in its `.d` is newer than the `.o`. Run them in parallel:
   `printf '%s\n' ${sources} | xargs -P "$(nproc)" -I{} <compile one>`. Then link. Keep `flecs.c`'s object between
   builds (it changes only with bsh/flecs.sh).
2. The build name in `obj/` includes the flags (debug, release, vulkan, windows, tests): an object built with
   other flags is never reused.
3. Release builds add `-flto` to compile and link, to inline across modules again. Compare phase 0's perf numbers;
   if release is slower, keep a `--unity` option that builds one file (a generated `obj/unity.c` including every
   module's `_.c`) for releases.
4. web.sh (emcc) and android.sh (the ndk): the same list of sources. Android's build file lists sources once.
5. makefile: `SRC` from the same list (build.sh can print it).

## Phase 6: keep it that way

- `bash bsh/lint_builds.sh` (it already compiles variants): build each module alone, `gcc -fsyntax-only` on
  `src/m/_.c`, so a missing include shows in the module that's missing it.
- A check that fails on a new backward dependency: a module's `_.h` including a later module's `_.h`, or an
  `extern` line naming a later module's function (module_deps.sh, in tst.sh's lint step).
- doc/coding_standards.md: a section on module headers (what goes in `_.h`, what's `static`), and
  doc/onboarding.md section 3: "one translation unit" becomes "one object per module".

## Things to watch

- `#ifdef` settings that cross modules (a `#define` in one module that a later one tests with `#ifdef`): they must
  be in the header, or a module will compile the other branch silently. `grep -rn "^#define zox_" src` and check
  each name's `#ifdef` users.
- `static` locals in `static inline` functions moved to a header are one per object now, not one in total
  (`static int zox_span_index` in `zox_sys_on_begin` is per function and fine; a counter meant to be shared is not):
  make it a global with `extern` instead.
- Systems name components by path strings (`[in] rendering.RenderDepth`): that's at run time and doesn't change, but
  a module's components must still be defined (`zoxd_*`) before a later module's systems name them: import order
  in `import_zox` stays as is.
- Windows macros (`far`, `near`, `min`, `max`): `windows.h` is in `includes.c`, so every object sees them now.
- Globals the tests set (`zox_chunks3_gpu_meshing = 1` in a test) need `extern` in the owning module's header.
- Build flags must be the same for every object (`-Dzox_tests`, `-Dzox_debug`, `-Dzox_vulkan`..): build.sh passes one
  `dflags` to all.

## Order

`_`, then src/modules.c's list: transforms, networking, screens, inputs, graphics, apps, names, saves, realms, games,
players, colors, nodes, slots, cameras, rendering, sounds, animations, raycasts, shapes, textures, geometry, musics,
ui, plots, genetics, neurals, blocks, chunks, physics, lights, models, bones, weathers, streaming, regions,
particles, tunks, terrain, sky, biomes, heights, vegetation, mountains, towns, vrays, characters, pets, huds, space,
markers, combat, projectiles, stats, skills, items, parts, equipment, lores, achievements, actions, classes, jobs,
races, clans, quests, dialogues, pickups, crafting, farming, turrets, maps, ai, vodes, dungeons, npcs, controllers,
imports, editor, debug, xr, then the game.

Tick each here with its commit as it's done.

## Results

(clean build times, incremental build times, perf before and after)
