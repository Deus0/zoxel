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

