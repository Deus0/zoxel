# Zox Code Standards

This document defines the current source organization, naming conventions, and general shape of Zox code.

The goal is not to make the codebase wear a suit and carry a clipboard. The goal is to make it easy to walk into any part of Zox, understand what lives there, and know where the next piece of code belongs.

Zox has a particular architecture. These conventions exist to reinforce that architecture, not to manufacture ceremony.

Sometimes the code describes cameras, particles, and network packets.

Sometimes it describes coffee.

Sometimes it describes an ancient entity beneath the sea that should absolutely not have been given a component called `IsAwake`.

The structure remains the same.

## Aggregation Files

Aggregation files are named `_.c`.

They collect the source files belonging to a directory or module and let the directory describe itself without growing a giant include list in every parent file.

Think of `_.c` as the sign on the door.

```c
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
```

The `_` directory is reserved for foundational definitions such as shared definitions, macros, structs, and similar declarations.

It is not a junk drawer.

If something starts accumulating in `_` simply because nobody knows where else to put it, that is probably a sign that it belongs somewhere more specific.

Aggregation `.c` files are source aggregation units, not headers. Do not add traditional `#ifndef` include guards unless there is a specific reason to support repeated inclusion.

The structure should read naturally from the outside:

```text
coffee/
    _.c
    com/
    sys/
    tst/
```

becomes:

```text
coffee/
    _.c
     ├── com/_.c
     ├── sys/_.c
     └── tst/_.c
```

The aggregation file is the little signpost pointing deeper into the forest.

Whether the forest contains coffee beans or eldritch horrors is a separate architectural concern.

## Module Folders

Each major subsystem lives in its own module folder.

A module may contain submodules when the subsystem has enough internal structure to deserve its own little kingdom.

```text
cthulhu/
    _.c
    com/
    dat/
    pre/
    ins/
    fun/
    sys/
    dbg/
    tst/
```

Not every module needs every directory.

Some modules are tiny. Some grow tentacles.

Some acquire submodules:

```text
coffee/
    _.c
    beans/
        _.c
        com/
        sys/
    brewer/
        _.c
        ins/
        sys/
```

This is intentional.

Submodules should represent meaningful responsibility. Do not create extra layers merely because a file became long.

The architecture should explain the code, not make the code explain the architecture.

A module folder should answer:

> What does this part of Zox own?

A submodule should answer:

> What distinct piece of that responsibility lives here?

If the answer is "I needed somewhere to put three functions," the hierarchy is probably getting ahead of itself.

## Module Files

The main module file is normally `module/_.c`, or the root source file where the surrounding structure uses a different arrangement.

A module file should act as the map at the entrance.

It should contain:

* A short module header describing its purpose.
* Shared module-level variables when needed.
* Aggregation includes for the module's source sections.
* A single module import or initialization function.
* Registration of components, systems, tags, prefabs, and hooks as appropriate.

Example:

```c
/*
 * +--------------------------------------------------------------+
 * | Zox Module: Cthulhu                                          |
 * |                                                              |
 * |  Dreams - Tentacles - Ancient Things                         |
 * |                                                              |
 * |  Uses: Input - Audio - Entities                              |
 * |                                                              |
 * +--------------------------------------------------------------+
 */

zox_tag(Cthulhu);
zox_tag(Tentacle);
zox_tag(AncientOne);

#include "com/_.c"
#include "dat/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"

void import_cthulhu(ecs* world) {
    zox_module(cthulhu);

    zoxd_tag(Cthulhu);
    zoxd_tag(Tentacle);
    zoxd_tag(AncientOne);

    add_hook_spawn_prefabs(zox_prefabs_cthulhu);
}
```

A module import function should be easy to scan.

It tells you what the module exists for, what it depends on, what it registers, and where its actual implementation lives.

The interesting machinery belongs elsewhere.

The module file is the conductor, not the entire orchestra.

Or, in this particular case, the cult leader is not the entire cult.

Module import functions use lowercase `import_<module>()` names.

Use `zox_module(module)` inside the import function to establish the Flecs module.

Module imports should remain orchestration code. Detailed implementation belongs in the relevant submodule files.

### Module Dependencies

Module files should include a short `Uses:` line when useful to make architectural dependencies visible at a glance.

```c
 * |  Uses: Dreams - Audio - Entities                              |
```

The `Uses:` line is a module-level dependency summary.

It does not need to reproduce every low-level include.

It exists so that someone opening the file can get the broad shape of the module before descending into the machinery.

Modules should be thought of in architectural depth, from foundational systems toward higher-level gameplay and tooling.

The depth grouping is a readability aid, not a sacred dependency graph carved into stone.

You should not need a university degree in archaeology to discover which module owns the coffee machine.

## Components

Components use PascalCase names.

```c
CoffeeTemperature
CoffeeAmount
CoffeeBeans
DreamStrength
TentacleCount
IsAwake
```

The name should describe the data or state represented by the component, not the system that happens to use it.

For example:

```c
CoffeeTemperature
```

is better than:

```c
CoffeeMachineTemperatureSystemData
```

when the component represents temperature regardless of what system changes it.

Components are defined using the established `zoxc_*` definitions.

For example:

```c
zoxc_byte(IsAwake);
zoxc_byte(CthulhuMood);
zoxc_float(CoffeeTemperature);
zoxc_float(CoffeeAmount);
```

Use the appropriate `zoxc_*` definition for the component's underlying data type.

Component names use PascalCase even though ordinary C functions use lowercase snake_case.

This distinction is deliberate:

```c
CoffeeTemperature     // component
coffee_temperature_system()  // system
import_coffee()       // module import
```

The type tells you what something is.

The function name tells you what something does.

Components should generally remain small and direct. They are pieces of ECS state, not miniature object hierarchies hiding inside a struct.

A component should answer a simple question:

> What piece of state does this entity have?

For example:

```c
zoxc_float(CoffeeTemperature);
zoxc_float(CoffeeAmount);
zoxc_byte(CoffeeReady);
```

Then systems decide what that state means in motion.

A sleepy programmer sees:

```c
CoffeeReady
```

A programmer who has been awake for 30 hours sees:

```c
CthulhuIsCoffee
```

The former is probably the component you want.

Components are the pieces of state. Systems are where those pieces begin to move.

Avoid encoding unnecessary behavior into component definitions. Keep the data model clean so that systems can operate on it naturally.

### Generic Components and Tags

Fewer components is a goal. Before adding one, look for a generic one that already holds that kind of value, and
organise with tags:

* **Generic values, tags for meaning.** A burn's rate is `Damage` (a float, combat's) tagged `DamagePerSecond`, lasting
  `DamageTime`, not a `BurnRate` and a `BurnTime`. A skill that burns is tagged `Burn`.
* **Reuse core's.** `Target` (the entity it targets: a camera's character, a pointer's ui, an npc's sight), `Count`,
  `Busy`, timing's `DestroyInTime`; and the modules' `Color`, `Bounds3D`, `Gravity3D`, `Scale3`.
* **Tags, not byte states.** Systems filter on tags: `SkillHit` added when a hit is tried, not a stage byte of 2.
* **One value each.** Never pack different meanings into a struct or a vector. A `float2` of one range (min, max) is
  fine; a rate and a duration in one is not.
* **Derive, don't store.** A heading from a target position, an attacker's position through its link, a particle
  count from the time.

```c
zox_add(skill, Burn);                   // its hits burn
zox_set(skill, Damage, { 0.5f });       // generic
zox_add(skill, DamagePerSecond);        // what the Damage means
zox_set(skill, DamageTime, { 5 });
```

## System Files

Systems use lowercase snake_case function names.

- `void name_system(iter* it)`, a plain function, registered with `zoxd_system(name_system)` after it
- `zox_sys_on_begin()` first and `zox_sys_on_end()` last (its time, for the profiler)
- checked by `bash bsh/tst.sh lint_systems` (part of `all`): systems a macro names from a component
  (`T##IncrementSystem`) are left alone

Example:

```c
// NOTE: Cools coffee toward room temperature
void coffee_cool_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_begin();
    zox_sys_in(CoffeeVolume);
    zox_sys_out(CoffeeTemperature);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(CoffeeVolume, volume);
        zox_sys_o(CoffeeTemperature, temperature);
        if (temperature->value > 20.0f) {
            temperature->value -= zox_delta_time * 2.0f * volume->value;
        } else if (temperature->value < 20.0f) {
            temperature->value = 20.0f;
        }
    }
    zox_sys_on_end();
} zoxd_system(coffee_cool_system);
```

Do not use Flecs-style PascalCase system function names merely because Flecs can derive a system name from the C function name.

Use normal Zox C naming:

```c
coffee_cool_system()
brew_coffee_system()
awaken_cthulhu_system()
```

not:

```c
CoffeeCoolSystem()
BrewCoffeeSystem()
AwakenCthulhuSystem()
```

ECS components and tags retain their own established naming convention independently.

A system can therefore read naturally:

```c
CoffeeTemperature
```

is the component.

```c
coffee_cool_system()
```

is the system acting upon it.

This gives the code two different voices without making either one shout.

### System Logic

System files should keep the actual system logic in ordinary C wherever practical.

Macros are useful for repeated ECS boilerplate, but they should not turn a straightforward system into a puzzle box.

Good:

```c
for (int i = 0; i < it->count; i++) {
    zox_sys_i(CoffeeAmount, coffee);

    coffee->value += zox_delta_time * 0.25f;

    if (coffee->value > 1.0f) {
        coffee->value = 1.0f;
    }
}
```

The intent is visible.

You can see the coffee.

You can see the cup filling.

You can see the inevitable limit.

System comments should describe the system's purpose or lifecycle when that behavior is not immediately obvious.

Good:

```c
// NOTE: Wakes the entity when dream energy falls below the threshold
```

Less useful:

```c
// Dream system
```

The first tells you something.

The second merely points at the thing you are already looking at.

### System Placement

Put systems with the things they operate on.

If a subsystem has several related systems, group them in its `sys/` directory rather than scattering them across unrelated parts of the module.

For example:

```text
cthulhu/
    sys/
        cthulhu_wake.c
        cthulhu_dream.c
        cthulhu_tentacle.c
```

Or:

```text
coffee/
    sys/
        coffee_brew.c
        coffee_cool.c
        coffee_drink.c
```

The exact filenames may vary, but the system's home should be obvious.

When you think:

> "I need to find the system that controls this."

the filesystem should answer before the coffee goes cold.

### Test Placement

Put a test in the module whose code it checks, not the module whose data happens to exercise it.

Use the lowest module that owns the behaviour and can see everything the test uses. Higher modules may still produce the data: a test can watch whatever the game spawns.

For example, `icon_bake` checks that `vox_texture_system` waits for its model. Body part items give it the most icons to watch, but it lives in `chunks/chunks3/tst/`, beside the system, not in `parts/tst/`.

```text
coffee/
    sys/
        coffee_brew.c
    tst/
        coffee_brew.c    <- checks the brewing, wherever the beans came from
```

Always use the lowest module possible. A test never goes higher just because it needs a world to run in:

- no player needed: most tests only need terrain streamed, so use a world of its own and a streamer or camera
  (`zox_tst_world_ready`, `spawn_streamer`), and a bare body (Position3D, Bounds3D, Velocity3D) for physics
- a player needed: `zox_tst_player_spawn_ready` and `zox_tst_player_character` are declared in `src/_/tst`, so a
  lower module waits on the player the game spawns without seeing `CharacterLink`
- needing something from a higher module (i.e. `town_at`): the lowest module that sees it, or split the test

`controllers/tst` is for controller tests only: input driving the player, the camera, the menus.

Keep a test as small as it can be: use only what it checks, so the whole suite stays quick.

- pure logic first: no world, no gpu, ready on the first frame (maths, collections, octrees)
- then bare entities: spawn just the components the system reads (transform_hierarchy, terrain_collisions)
- then a world of its own and a streamer (air_chunk.c), not a new game with a player, npcs and towns
- a whole game and the player only when the test is about them
- wait on a condition, not a fixed time; a few frames, not seconds

Tests make their own data, they don't assume the game's:

- spawn the blocks, npcs, items and quests a test needs (a brick block, one villager), not the realm's generated
  ones, so a change to the game's content never breaks a test that isn't about it
- a test level, not the game's world: tiny (one chunk of render distance), flat, no block voxes unless the test
  is about them, its npcs spawned where the test wants them (think a test level in a game's dev build)
- only a test about the game's content (its generation, its realm data) reads the game's data

When you think:

> "Which test covers this system?"

the answer should be one folder away from the system.

## Naming

Zox uses different naming conventions for different kinds of things.

The distinction should remain consistent.

Components and tags use PascalCase:

```c
CoffeeTemperature
CoffeeAmount
Cthulhu
Tentacle
IsAwake
```

Functions use lowercase snake_case:

```c
coffee_brew_system()
cthulhu_wake_system()
import_coffee()
```

Module names use lowercase:

```c
coffee
cthulhu
```

Aggregation files use:

```text
_.c
```

The result is visually distinct:

```c
CoffeeTemperature
coffee_temperature_system()
import_coffee()
```

That distinction makes the code easier to scan without requiring special syntax or ceremony.

Never name anything `near` or `far`. Windows' headers define both as empty macros (minwindef.h), so
`const float far = 100;` becomes `const float = 100;` and the Windows build fails. Name what it is instead:
`near_plane`, `far_distance`, `close`, `distant`. `bash bsh/tst.sh lint_builds` compiles with them defined.

## Macros End With ;

A macro used as a statement doesn't hold its own `;`, the call does, so it reads like a function call.

- `#define zox_get(e, T) ecs_get(world, e, T)`, called `const Health* health = zox_get(e, Health);`
- Expression macros used inline (`zox_getv(e, Seed) * 2`) take none, of course.
- Old macros that hold their own `;` are fixed as they're touched.

## Entities, Not Ids

Refer to things by their entity, not by an id or kind number looked up later.

- Link the entity (`zox_link`, `zox_set_link`), don't store an enum index and search for a match.
- Data a kind shares (a material's sound, a block's stats) is an entity the kind links, not a row picked by id.
- Tags mark what an entity is (`HitSoundVoiced`, `BlockStone`), not a byte compared against constants.
- e.g. a block links its hit sound entity (`HitSoundLink`, [sounds.md](sounds.md)), no lookup by material id.

## Flecs

Flecs is used through the `zox_` macros in `src/_/flecs/mcr/`, raw flecs stays inside `src/_/flecs/`.

- Types: `ecs`, `entity`, `iter`, `query` (`src/_/flecs/_.c`), not `ecs_world_t`, `ecs_entity_t`, `ecs_iter_t`,
  `ecs_query_t`. A parameter can keep the name the macros expect: `query* query` for `zox_sys_query_begin()`.
- Calls: `zox_add`, `zox_setv`, `zox_get_link`.., not `ecs_*` in module code. Missing one? Add a macro to `mcr/`.
- A flecs feature the code doesn't use yet (custom events, `ecs_enqueue`, new traits) needs an ok first.
- Observers over systems for reacting to a change. Register with `zox_on_add` / `zox_observer`, write the callback
  like a system: `zox_sys_world()`, `zox_sys_e()` in the loop (i.e. `src/ai/eve/settings.c`).
- One-off events: a non fragmenting tag (`zoxd_nf_tag`) added when it happens, an observer acts and removes it. It
  runs on the main thread at the merge, so it can spawn, i.e. `ActionSwapped` (`src/actions/ui`) plays the swap
  sound for a tap handled on a worker thread. Non fragmenting tags stay out of system queries.
- Don't move systems around to make a side effect work: no `zox_system` to `zox_system_1` (main thread), new sync
  points or phase changes for it. Use an observer, or ask; `tst pipeline_syncs` counts the merges.
- flecs.c itself is patched by `bsh/flecs_fix.sh`, each patch in [flecs_patches.md](flecs_patches.md).

## Commits

- Short messages: one lower case line saying what changed, i.e. `terrain scales with the stream height`.
- The details (why, how, what a value means, what a test checks) go in the code's comments, where the next reader
  finds them, not in the commit.

## General Principle

Zox code should be organized around responsibility.

Folders describe architectural areas.

Aggregation files collect those areas.

Components describe state.

Systems transform that state.

Modules assemble the pieces.

Functions perform the work.

Macros remove repetition where useful.

The conventions are there to make the code predictable without sanding all the character off it.

A good Zox directory should read almost like a sentence:

```text
coffee/
    _.c
    com/
    pre/
    ins/
    fun/
    sys/
```

A more complicated module can grow naturally:

```text
cthulhu/
    _.c
    dream/
        _.c
        com/
        sys/
    tentacles/
        _.c
        dat/
        sys/
```

The structure tells you where the code lives.

The names tell you what the code is.

The systems tell you what happens.

And somewhere, probably three modules away, an entity has:

```c
zoxc_byte(IsAwake);
```

which is a perfectly reasonable thing to have until you remember that the entity is Cthulhu.

Then it becomes a little less reasonable.

That is fine.

The architecture should still make sense.

