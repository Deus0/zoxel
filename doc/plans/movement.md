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

1. [x] **Record the feel first.** tst `move_feel` (gam/zeland/tst): a player's top speed, time to 90% of it and slide
   when let go, forward, back, strafing each way and running, from standing, today's numbers its checks. Zeland's
   start (the same flat ground every run; zoxel's test level spawned elsewhere each run, its strafes into walls),
   seed 666 (tst.sh `seeded_tests`), every frame 1/60s (`zox_delta_time_override`): steady to ~1%. Crouching stays
   zoxel's tst crouch (a skeleton bends). Found: at ~0.07s frames (software rendering) strafing came in pulses, a
   frame's push near its top speed held back till friction stopped it: the old move depends on frame rate.
2. **Players.** `player3_move_input_system` writes `MoveDirection` and `Running`; `character3_move_system` moves.
   `move_feel` passes unchanged with instant turning. Zeland and zoxel both. With it:
   - **turning by physics, the default** (a torque to face the move, as npcs turn); the old instant turn a setting,
     the fallback if play tests don't like it. `move_feel` measures both
   - **speed from the speed stat** (1 a player's start: today's speeds), scaled by a setting so agility raises it gently
   - the push capped so it never overshoots its top speed: the same at any frame rate (a `move_feel_10fps`)
3. **Npcs.** The ai writes `MoveDirection`; `move_forward_system`'s push and `rotate_towards_system`'s torque go.
   Npcs then turn and accelerate like players; tune with step 1's npc numbers and play tests (it changes how they
   feel: decide together).
4. **XR.** Its direct stick (no inertia) becomes a setting of the move (no acceleration for a headset), not its own path.

Each step its own commit, zoxel's movement tests (crouch, auto_jump, step_gap, footsteps, npc_speed, npc_attacks)
before and after.

## Decided (2026-11-27)

- Players turn by physics by default, like npcs; instant turning kept as a setting to fall back to.
- Players' speed from the speed stat, scaled well (a setting). Npcs' `MoveSpeed` joins it in step 3.
- Steps 1 and 2 now; 3 (npcs) and 4 (XR) later, in todo.
