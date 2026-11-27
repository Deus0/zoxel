# Movement: input apart from the move (2026-11-27)

Players' devices and npcs' ai both say where a character wants to go; one system moves every character from that, by
the same rules. Today they share nothing:

- **Players** (`controllers3/sys/movement.c`, `player3d_move_system`, ~260 lines): reads the devices (WASD, stick, run,
  crouch), turns the stick into a direction by the camera, sets the character's rotation at once, adds acceleration
  capped per axis at `max_velocity3D` (run, crouch and water scale it). XR moves without inertia (`player3d_move_direct`).
- **Npcs** (`ai/sys`): `rotate_towards_system` turns them with torque (a spring to the target, `Alpha3D`),
  `move_forward_system` pushes them forward at their `MoveSpeed` once they face within 45 degrees of the target, and
  steps up blocks it reads from the voxel terrain (`npc_step_ahead`, `BlockScale`).

## Data

On the character, generic (core or controllers), one value each:
- `MoveDirection` (float3): where it wants to go, world space, flat against its up, length 0 to 1 (a half pushed
  stick walks slower). Zero stands still.
- `Running` (non fragmenting tag): runs while it's on. Crouching keeps `CrouchState`, jumping `JumpState` as now.

Who writes it, nothing else:
- `player3_move_input_system`: the player's devices (keyboard, stick) through the camera (first person: the
  character's facing; third person: `third_person_move_rotation`), the same maths as now.
- the ai: `move_forward_system` writes the direction to its target instead of an acceleration.
- tests: set `MoveDirection` (or hold keys, the fake keyboard).

## The move

`character3_move_system`, every character with a `MoveDirection`:
- turns it to face the direction at a turn rate (a setting; players today turn at once, npcs by torque)
- accelerates along it to its top speed (`max_velocity3D` for players, `MoveSpeed` for npcs till they're one stat)
- run, crouch and water scale it, as now
- steps and auto jump stay their own systems, reading `MoveDirection` (auto jump: voxel terrains only, as now)

## Steps

1. [x] **Record the feel first.** tst `move_feel` (src/controllers/tst, the test level: flat land, a player alone; first
   measured on zeland's start): a player's top speed, time to 90% of it and slide
   when let go, forward, back, strafing each way and running, from standing, today's numbers its checks. Zeland's
   start (the same flat ground every run; zoxel's test level spawned elsewhere each run, its strafes into walls),
   seed 666 (tst.sh `seeded_tests`), every frame 1/60s (`zox_delta_time_override`): steady to ~1%. Crouching stays
   zoxel's tst crouch (a skeleton bends). Found: at ~0.07s frames (software rendering) strafing came in pulses, a
   frame's push near its top speed held back till friction stopped it: the old move depends on frame rate.
2. [x] **Players.** The input (`player3d_move_system`, controllers3/sys/movement.c) writes `MoveDirection`,
   `FaceDirection` (camera facing modes; zero in first person, the mouse turns it) and `Running`, zero when it skips;
   `character3_move_system` (move.c) turns and pushes every `PlayerCharacter` (npcs in step 3). With it:
   - **turning by physics, the default** (a torque on its spin, a damped spring to `FaceDirection`, as npcs turn):
     `player_turn_strength` 400, `_damping` 30, `_max` 800; a 90 degree turn of the orbit camera faced in ~0.3s,
     most of it the camera easing round (tst `move_feel_turn`). `player_turn_instant` the old turn, the fallback
   - **speed from the speed stat**: 1 + (stat - 1) * `player_speed_stat_scale` (0.5), on its top speed and push
   - **exact at any frame rate**: each axis's push trimmed to land on its top speed, its drag counted (physics/fun's
     v' = v keep + a g), not dropped whole. The settings are now the top speeds reached, set to the old move's at
     60fps: `max_velocity3D` 0.99 side, 1.43 forward, `run_speed` 1.67; zoxel's 1.08, 1.28 (the old ones, 1.24 and
     1.4, zoxel's 1.3, were never reached sideways and passed forwards: the old cap counted no forward push)
   - tests: `move_feel` (the test level, zoxel's speeds, the old numbers), `move_feel_10fps` (the same numbers at 0.1s
     frames: top speeds within 5%, times two frames later), `move_feel_turn` (zeland, third person), and zoxel's
     movement tests (crouch, walk_bob, auto_jump, step_gap, footsteps, gravity_flip, underwater, xr_body)
3. **Npcs.** The ai writes `MoveDirection`; `move_forward_system`'s push and `rotate_towards_system`'s torque go.
   Npcs then turn and accelerate like players; tune with step 1's npc numbers and play tests (it changes how they
   feel: decide together).
4. [x] **XR** (2026-11-27, doc/tech/vr.md, Moving by the stick; the head into a wall fading not done yet). The body moves by the move like any character (physics, turning, the speed stat); the view stays
   comfortable and the camera stays the real head, never smoothed or lagged:
   - the view (the play space) still moves straight from the stick, steady, no speeding up or sliding (accelerating
     views sicken: the eyes see it, the inner ear doesn't)
   - a balancer keeps the body under the head: leans and steps the legs to catch up, keeps the torso and head bones
     steady meanwhile (builds on xr/fun/body.c and the leg stepping in bones). Others, mirrors and shadows see this body
   - the camera is attached as now; only the play space's motion differs from the body's. Walls (as Half-Life: Alyx
     and most VR games): the view moved by the stick stops at the body's collisions; the real head leaning into a wall
     fades the view to black, never pushed back (doc/tech/vr.md)

Each step its own commit, zoxel's movement tests (crouch, auto_jump, step_gap, footsteps, npc_speed, npc_attacks)
before and after.

## Decided (2026-11-27)

- Players turn by physics by default, like npcs; instant turning kept as a setting to fall back to.
- Players' speed from the speed stat, scaled well (a setting). Npcs' `MoveSpeed` joins it in step 3.
- Steps 1 and 2 now; 3 (npcs) and 4 (XR) later, in todo.
