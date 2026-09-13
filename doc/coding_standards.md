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

## System Files

Systems use lowercase snake_case function names.

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

