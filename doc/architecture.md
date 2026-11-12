# Architecture

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
  (see [nodes.md](nodes.md)), or a state component going `zox_dirty_trigger` → `zox_dirty_active`.
- When an event doesn't seem to fire, log at the top of the callback first: was it called at all?

## Allocated Components

Most components are plain values. A few hold memory, and free it when they're removed:

- **Arrays** (`zoxc_arrayd`, `zoxc_text` i.e. `TextData`, `zoxc_entities`): a length and a pointer
- **Node trees** (`VoxelNode`): voxel octrees, see the `octree_*` tests
- **Hashmaps**: lookups by id or name, i.e. files by name

See [coding_standards.md](coding_standards.md#components) for declaring them.
