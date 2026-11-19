# Realm clock

A realm has its own time, its terrain (the voxel planet) follows it, and the things in the terrain follow that. So
different realms can run at separate times, and pausing a game stops its realm while the ui, menus and streaming go
on. All of it is ECS: components on the entities, one system.

## Now (2026-11-20)

- Components (`src/_/timing/com`): `ClockTime` (seconds passed), `ClockDelta` (this frame's seconds), `ClockScale`
  (its rate of the clock it follows), `ClockLink` (the clock it follows, a non fragmenting link).
- `clock_system` (`src/_/timing/sys/clock.c`, first in `zoxp_update`): each clock's delta is the frame's
  (`zox_delta_time`) times the scales up its chain of links, worked out from the top each frame (no lag between
  levels).
- Who has one: realms (the root, `ClockScale` 1), terrains (linked to their realm), characters and projectiles
  (linked to their terrain as they spawn).
- Pause: `game_state_realm_clock` (games) sets the realm's `ClockScale` to 0 when the game pauses, 1 when it plays.
- Physics moves bodies by their clock: an optional `[in] ?timing.ClockDelta` term, `physics_body_dt` (the frame's
  delta without one, i.e. particles); the terrain collision sweep too. tst realm_pause.

## Next

Systems still on `zox_delta_time` or `zox_current_time` that are game time, moved onto the clock a group at a time:

1. characters: controllers' movement and turning, npc ai (wander, attack timing, sight), jump, footsteps
2. animations: bones (walk phase, swings timed by `SwingStart` against `zox_current_time`, ik easing), head look
3. the world: day and night (sky), weather, block damage, regen, doors swinging, beds
4. timers: `delay_event`, `DestroyInTime`, cooldowns: game ones on the realm's time, ui ones on the frame's
5. particles: linked to what spawned them (a character's, a projectile's), not the frame's

Systems timed by an absolute time (`zox_current_time - start`) need a clock time instead: their start stamped
from the entity's `ClockTime`, compared against it.
