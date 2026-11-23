# Physics

How things move, spin, fall and hit the terrain, and the traps along the way.

| Where | What |
|---|---|
| `src/physics/` | shared tags (`Frictioned`, `Grounded`, `DisableGravity`, ...), the exact step (`fun/`), profiling, tests (`tst/`) |
| `src/physics/physics2/` | 2D: `Velocity2D`, `Acceleration2D`, `Gravity2D`, `Torque2D` (spin), `Alpha2D`, `Bounds2D` |
| `src/physics/physics3/` | 3D: `Velocity3D`, `Acceleration3D`, `Gravity3D`, `Omega3D` (spin), `Alpha3D`, `Rolling3D`, `InitializePhysics3D` |
| `src/physics/physics3/collisions3/` | sphere overlaps (`SphereCollider`, `OverlapEvent`), bounds growing for voxel entities |
| `src/terrain/collisions/` | boxes against voxel terrain: push-out, tunneling sweep, `Grounded` |
| `src/_/maths/` | vectors, quaternions (tested in `src/_/maths/tst`) |

## The one idea: exact steps

Every frame solves the motion **exactly** for a force held over the frame and a linear drag:

```
dv/dt = a - k v        a: forces (gravity, a controller's push), k: drag (friction), 0 for none
```

So the answer doesn't depend on the frame rate. At 10, 30, 60 or 144fps:

| | Result |
|---|---|
| A fall from rest | `v = g t`, `p = g t² / 2` |
| A jump at `v` | peaks `v² / 2g` up |
| Pushed by `F` with drag `k` | tops out at `F / k` |
| Sliding to a stop from `v0` | goes `v0 / k`, slowing as `v0 e^(-k t)` |
| Falling with air drag | heads for terminal velocity `g / k` |
| Spin with `Frictioned` | slows as `w0 e^(-2 t)` (`omega_dissipation3`) |

Plain Euler (`v += a dt; p += v dt`) gets all of these a little wrong, differently at each frame rate:
a fall off by `g t dt / 2`, a slide by `v0 dt`, and drag overshoots past `k dt > 1`.

The step is `physics_step` in `src/physics/fun/_.c`, as coefficients for a frame's `k` and `dt`:

```
v' = v keep + a g          keep = e^(-k dt)
moved = v g + a h          g = (1 - keep) / k,  h = (dt - g) / k
```

They're the same for every axis and every body with that drag, so systems work them out once, not per body.
With `k = 0` they're `keep = 1, g = dt, h = dt² / 2`: constant acceleration.

Spin uses the same step for `Omega3D` / `Alpha3D` / angular drag, turning by the step's rotation vector.
That's exact when `Alpha3D` is along `Omega3D` (or zero), and very close otherwise.

## A frame

Everything is in `zoxp_physics`, in this order:

1. **Forces** go into `Acceleration3D`: gravity (`gravity3_system`), plus anything added before
   `zoxp_physics` (controllers and AI in `zoxp_update`).
2. **Spawn hold**: `InitializePhysics3D` clears velocity and forces for a body's first 8 frames.
3. **Linear step**: velocity and position, friction included, then `Acceleration3D` is cleared.
4. **Spin**: `Rolling3D` sets the spin, then a spin step turns `Rotation3D` and clears `Alpha3D`.
5. `Position3DBounds` clamps.
6. **Terrain collision** (`src/terrain/collisions`, a later module, so it runs after).

2D is the same without the spawn hold or collision.

**Each body moves in exactly one linear system and one spin system**, picked by its components:

| Body has | Linear step in | Spin step in |
|---|---|---|
| `Velocity3D` only | `velocity3_system` (`p += v dt`) | |
| `Acceleration3D` | `acceleration3_system` | |
| `Acceleration3D`, `Frictioned`, `Grounded` | `friction3_system` (drag: ground 6, air 1) | |
| `Frictioned` without `Grounded` | `acceleration3_loose_system` (no drag) | |
| `Omega3D` only | | `omega3_system` (`omega3_damped_system` if `Frictioned`) |
| `Omega3D` and `Alpha3D` | | `spin3_system` (`spin3_damped_system` if `Frictioned`) |

## Making things move

- **Push** (walk, swim, wind): add to `Acceleration3D` each frame you want it pushed. It's cleared every frame.
  Add, don't set, if more than one system pushes the same body.
- **Kick** (jump, knockback, an explosion): change `Velocity3D` once.
- **Gravity**: give it a `Gravity3D` (`zox_default_gravity`), any direction. The `DisableGravity` tag turns it off
  (its direction still says which way is up, for rolling and the ground).
- **Friction**: the `Frictioned` tag plus a `Grounded` component. Drag is `ground_friction3` when
  grounded, `air_friction3` in the air. `Frictioned` also slows spin.
- **Spin**: `Omega3D` is angular velocity, `Alpha3D` angular acceleration, both **in the body's frame**
  (`rotation = rotation * delta`). For a world space spin, i.e. a hit from the side, use
  `physics3_add_alpha_world(rotation, &alpha, world_alpha)`, or `physics3_world_to_body` / `physics3_body_to_world`.
- **Rolling**: the `Rolling3D` tag (with `Grounded`, `Bounds3D`, `Omega3D`). While grounded it spins
  `up × v / radius`, so the point touching the ground stays still. Up is against its `Gravity3D` (`physics3_up`,
  +y without one), the radius its half extent along up (`physics3_half_along`).
- **2D spin**: `Torque2D` is the angular **velocity** (it's named before `Alpha2D` existed), `Alpha2D` the
  acceleration. `Rotation2` stays within (-2π, 2π) spinning either way.

## Terrain collision

`terrain_collision_system` (`src/terrain/collisions/sys/intersect2.c`), for boxes (`Bounds3D`, half extents,
turned by `Rotation3D`) linked to a terrain (`TerrainLink`), not `NoClip`:

1. **Sweep** (fast movers only): when a frame's move is over `collision_step_scale` of the smallest of half
   a block and the box's half extent, it's redone from where the body started, one axis at a time (y first,
   so a fall lands before it slides), stopping at the first solid face. This is what stops tunneling.
2. **Push-out**: out of every solid voxel it overlaps, along the smallest overlap. A face buried in another
   solid voxel (a seam) is skipped, so bodies don't snag or bob on block and chunk seams.
3. **Last line of defence**: if the centre is still in a solid block, walk it back the way it came (or up).
4. **Bounce**: velocity into a face is flipped and scaled by `bounce_lost_force` (x and z keep 32%, y 0).
5. **Grounded**: `ground_state_grounded` on a top face, then `ground_state_leaving` for a frame, then `ground_state_air`.

**Up is against the body's `Gravity3D`**, any direction, snapped to the nearest axis (`physics3_up_axis`, its
biggest part for gravity off any axis, +y without gravity): a top face, the sweep's first axis, the bounce that
stops a landing (`bounce_lost_force.y` along up, `.x` along the others) and the way out when stuck all follow it.
The tests run turned (`terrain_collisions_up`, `_x`, `_z`, `terrain_ledges_x`): the same terrain and bodies with
gravity +y, +x and -z, checked in the turned frame.

**Ledges and gaps** (`collision_unblock`): a body moving into a block, pushed back off it sideways, is instead

- lifted onto it, when the block's top is within `collision_step_up` (half a block) over its feet, and only with
  gravity: landing on a step while pressed against a wall over it (a house's door a block up: a body 1.969 blocks
  tall has 0.03 blocks of room, it used to be sent back off the step every time, `tst step_gap`)
- slid sideways into a gap, up to `collision_slide` (a quarter block): a body 0.81 blocks wide used to get into a
  gap a block wide only within 0.15 blocks of its middle
- or both, whichever is the smallest move that leaves the box clear. A full block in front still stops it (a lift
  of a whole block is past the limit), and a gap narrower than the box stays shut. Colliders keep their size.

The sweep does the same when a sideways axis is blocked, with the rest of its move (up to a safe step) past it.

**Where it started is worked back from the velocity, no stored position.** That keeps teleports safe:
nothing ever sweeps from an old position to a new one. With drag, an exact step moves `v_end * g / keep`,
not `v_end * dt`, so the collision corrects for that (exact under drag alone). Forces leave it off by at most
`|a| dt² / 2` (5cm at 10fps, 1.4mm at 60fps), well within the push-out's reach.

## Gravity fields (terrain/gravity)

A terrain chunk has a `GravityOctree` beside its voxels, as its `LightOctree`: where gravity pulls, a byte a voxel.
0 is the realm's own (`zox_default_gravity`, down), else a direction, `1 + axis * 2 + (1 along +axis)`
(`gravity_pull`).

- **A gravity cube** (`BlockGravity`, gam/zoxel/sys/blocks.c) switched on flips gravity in a sphere of
  `gravity_radius` (4) blocks round it, to voxel centres. On is its chunk's `ActiveOctree`, as an open door's: a right click toggles it (`block_use`, `block_toggle`), and it's saved with its chunk. Where spheres
  overlap each flips it again (two make it down).
- **Worked out from the cubes**, not stored: `gravity_rebuild` gathers the cubes switched on in a chunk and the 26
  round it (a sphere reaches one chunk at most) and flips each voxel in their spheres. Full detail chunks only.
- **When**: an observer on `VoxelNodeDirty` (a chunk generated, loaded or edited: a cube dug out while on, in its
  queue's old data, rebuilds the chunks it reached) and a toggle (`gravity_dirty_around`) add `GravityDirty`, a non
  fragmenting event; its observer rebuilds the chunk on the main thread, between systems.
- **Bodies**: one with the `TerrainGravity` tag (characters) takes its `Gravity3D` from the field at its centre every
  frame (`terrain_gravity_system`, zoxp_update), so a cube toggled under it flips it without it moving. A body
  without the tag keeps its own (particles, the physics tests').
- A jump pushes against its gravity (`jump_force_system`, `physics3_up`).
- **Debug**: the gizmos' Gravity button draws every node that doesn't pull down, coloured by direction, in the
  camera's chunk and a chunk round it (as the lights' button, which reaches as far now).
- tst gravity_cube: a sphere exactly, across a chunk's side, overlaps, dug out, a body flipped and rising.

## Setting a velocity each frame

Movement that sets a velocity rather than pushing (XR walking, `player3d_move_direct`, `xr_room_walk_system`)
scales it by `physics3_drag_compensation`: the drag the body gets this frame (ground or air, by `Grounded`) would
otherwise trim it, by an amount that changes as `Grounded` flickers on seams (the walk bumps).

## Gotchas

- **`zox_delta_time` is a frame stale.** `iterate_time_system` runs at `zoxp_end`, so frame N's physics uses
  the time frame N-1 took. Read after a frame, `zox_delta_time` is the *next* frame's dt.
- **Never use flecs' `it->delta_time` for physics.** It isn't clamped to `max_zox_delta_time` (0.1s) on a lag
  spike or overridden, so it isn't what moved the bodies. The terrain sweep used it once, and reconstructed
  the wrong start.
- **Physics skips frames over `zox_physics_max_delta_time` (0.2s)**, but `max_zox_delta_time` already clamps
  dt to 0.1s, so it only happens with a `zox_delta_time_override` (see `src/physics/dbg/big_frame.c`).
- **Forces added during `zoxp_physics` by a later module land next frame**, i.e. `jump_force_system`. Add forces
  before `zoxp_physics` (`zoxp_update`) to have them this frame.
- **Adding or removing `Acceleration3D`, `Alpha3D`, `Frictioned` or `Grounded` changes which system moves a
  body** (see the table). If you write a new movement system, exclude the others' bodies (`[none] !...`)
  or bodies move twice.
- **Friction needs a `Grounded` component**, `Frictioned` alone gives no drag (only spin damping).
- **Drag in floats**: with a tiny `k`, `a / k` is huge and `(v - a/k) e^(-k dt) + a/k` cancels away gravity
  completely. `physics_step_new` uses a series for small `k dt`. Keep the coefficient form.
- **`Position3DBounds` all zero means off.** When on, the whole box stays inside (`Bounds3D` counts).
- **`float4_inverse` is the conjugate**, only the inverse for a unit quaternion. `quaternion_inverse` is
  the true inverse at any length (it used to just normalize).
- **`InitializePhysics3D` holds a new body still for `physics_disable_frames` (8)** frames, while it settles
  into the terrain.
- **`bounce_lost_force.y` is 0**: landing kills vertical speed, nothing bounces up off the floor.

## Tests

`bash bsh/tst.sh <name>`, see [tests.md](../engine/tests.md).

| Test | Checks |
|---|---|
| `physics_steps` | the step alone: one big step = many small steps, closed forms, no overshoot, float precision |
| `physics2_motion` `physics2_rotation` | 2D: drift, push, throw, friction, top speed, bounds, spin both ways, spin up |
| `physics3_motion` | 3D: fall, push, slide, top speed, terminal velocity, spawn hold, `DisableGravity`, bounds |
| `physics3_rotation` | spin, tilted spin, spin up, damped spin, unit length, tiny spin, rolling without slipping |
| `physics3_gravity` | falling and terminal velocity with gravity along each axis both ways and off any axis |
| `physics3_rolling_gravity` | rolling on walls and ceilings: up from gravity, the radius along it, no slip |
| `terrain_collisions` | landing, 60 and 400 m/s falls, walls at 80-90 m/s, a ceiling at 40 m/s, seams, walking off an edge, buried spawns, jump height, a step a block tall stays a wall, sliding along a wall, a hole narrower than the body stays shut, one wider lets it through (`_up` `_x` `_z`: gravity turned) |
| `terrain_ledges` | a body as big as a character (pushed as the controls push, jumping as a character) gets up a step into a gap 2 blocks tall, in its middle and off it, and into a flat gap off its middle (`_x`: gravity turned) |
| `*_10fps` `*_30fps` | the same at another frame rate, 10fps is the biggest dt the engine allows |
| `physics2_perf` `physics3_perf` | cpu time in the physics systems for 50,000 bodies |
| `maths_vectors` `maths_quaternions` | vector and quaternion identities over 1000 random samples |

How the physics tests work (`src/physics/tst/common.c`):

- They run at a **fixed dt** (`zox_delta_time_override`). `zox_tst_clock_prepare` waits a frame for it to
  take hold before spawning (it's set at `zoxp_end`), `zox_tst_clock_tick` counts each frame.
- Each body is checked twice: against a **frame by frame model** of the systems (catches a system missing,
  running twice, or out of order) and against the **analytic answer** (catches a model wrong the same way
  as the code). With exact steps the analytic checks are tight, to float precision, at every frame rate.
- `terrain_collisions` builds its own tiny terrain (a realm, a block, two chunks), no streaming.
- Each of these was checked by putting a bug back and watching it fail: the old 2D angle wrap,
  the old bounds, no sweep (bodies fell through the floor), the old `quaternion_inverse`.

**Performance**: set `zox_physics_profiling = 1` and read `zox_physics_profile_ns` / `zox_physics_profile_bodies`:
cpu time inside the physics systems, summed over threads, timed per table (never per body). Frame times are
useless for this under `--autostart`, terrain streaming swings them by ±20ms. In the `-O0` debug build on the
khadas board: about 150ns a 2D body a frame, 470ns a 3D body (2026-11-12).

## Not done yet

- Rolling is exact at constant speed. A rolling body slowing under friction slips a little within a frame.
- Spin about a changing axis (alpha not along omega) is very close, not exact.
- Bodies against each other: only sphere overlap events (`sphere_collide_system`, O(n²)), no response.
- Movement, auto jump, swimming and the camera still take up as +y, and a character isn't turned to its up (it
  stands on a ceiling feet first, upright); jumps, the physics and terrain collisions follow gravity.
- Tumbling: collisions never make spin, so a cube can't tip, rock or roll over its edges (in doc/todo.md, Physics).
