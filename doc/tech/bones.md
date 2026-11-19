# Bones and Animation

How a character's skeleton is built, how it moves (walk, crouch, swings, ik, XR), how to look at it, and what was
learned making it look right.

Code: [src/bones](../../src/bones) (the skeleton, its animation state and systems), [src/bones/ik](../../src/bones/ik)
(arms, legs, the spine, planted feet), [src/parts/sys/bones.c](../../src/parts/sys/bones.c) (bones from body parts),
[src/xr/fun/body.c](../../src/xr/fun/body.c) (a headset's body). Skinning and weights:
[done/skeletons.md](../done/skeletons.md). Characters as a whole: [character.md](../character.md).

## The skeleton

- A character links its skeleton (`get_skeleton(world, character)`), the skeleton holds the animation state and its
  bones are its children (`BoneLinks`, in the shader's order). Links: `HeadBoneLink`, `HandBoneLink`,
  `ShoulderBoneLink` (the right, it swings actions), `OffShoulderBoneLink` (the left, the off hand).
- `spawn_part_bones` walks the body's slots from the chest, a bone a part (clothes have none, they move with the part
  under them). Each bone has:
  - `BonePosition`: its **pivot, the joint** with its parent's part, in the skeleton's space at rest. The skinning turns
    vertices about it (`bone.vert`: `bone_matrix * (vertex - bone_position)`), so it must be the bone entity's rest place.
  - `BoneCentre`: its part's middle, the same space. `BoneSize`: its part's half size (plus half a voxel).
  - `LocalPosition3D`: pivot minus its parent's pivot. The chest, the root, turns about its middle.
- Where a joint is (`part_bone_pivot`): across the axis the part and its parent are furthest apart on (by their half
  sizes together) halfway between their facing sides (a gap or an overlap); on the other axes the middle of where they
  overlap, else the part's. So an arm hangs from the shoulder's bottom, a shoulder from the chest's side, a foot's ankle
  is under its shin (not mid foot), a thigh joins the hips on its own line. A part with many children has a joint each.
- Before 2026-11-19 bones sat at their parts' middles: limbs turned about mid limb, a bent elbow pulled the forearm a
  gap off the upper arm and the skin between stretched into strips. If something needs a part's middle (a camera in
  front of the face, a mugshot, a box), use `BoneCentre`, posed now: `bone_centre_now(world, bone)` (bones/ins/look.c).

## What moves it, in a frame

| When | System | Does |
|---|---|---|
| zoxp_update | `walk_state_system` (controllers3) | `WalkState` from the character's speed |
| zoxp_update | `player3d_move_system` (controllers3) | the player's `CrouchState` (left control held, L3 toggles) |
| zoxp_update | `arm_control_system` | `SwingPhase`, `SwingBlend`, `CrouchBlend`; poses every swinging limb (`walk_limb`); the body's bob (the root's `LocalPosition3D`) |
| zoxp_update | `shoulder_raise_system` | a raised arm (`RaiseShoulder`), its `SwingState` 0 so the walk leaves it |
| zoxp_update | `head_look_system` | the head to its `LookTarget` |
| zoxp_trails | `swing_ik_system` | the arms' ik targets: a guard while raised, a swing's motion, the off hand in a punch; `SpineTwist` |
| zoxp_trails | `spine_lean_system`, `spine_twist_system`, `feet_plant_system` | XR's lean and crouch, a punch's twist, planted feet |
| zoxp_layouts | `arm_ik_system`, `leg_ik_system` | two bone ik by `IKWeight` (0: the walk's) |
| zoxp_post_update | `xr_head_bone_system` | a headset's head turn |

- One system poses a skeleton's walking limbs, with the skeleton, on its thread. As systems of their own (they were
  `arm_swing_system`, `leg_swing_system`) the limbs were split across workers and some read the skeleton's phase
  before `arm_control_system` moved it: one leg a frame behind the other.
- `SwingState` on a shoulder or thigh: 0 something else has it (a raise, the ik), else the walk does. The ik leaves an
  elbow, knee and foot to the walk while it swings their limb, and straightens them at weight 0 otherwise.

## The walk

`bones/sys/arm_control.c` (timing, the bob), `arm_swing.c` (`walk_limb`, `walk_leg_angles`).

- A step a half cycle: steps a second = the body's ground speed over the stride (`2 L sin A`, L hip to ankle, A the
  thigh's `SwingAngle`), clamped 1.2 to 4. A fixed pace made the feet skate.
- Legs ±25 degrees (`SwingAngle` on the thigh), the one swinging ahead lifts its knee (`walk_knee_lift`, by the cosine
  of the phase), straight as it lands. Arms ±28 against them, elbows soft, more as the arm comes ahead. Standing, the
  legs are still and the arms sway 5 degrees.
- Legs are put on their pose, not eased (the walk's blend already eases idle to walking): eased they lagged the phase,
  at 10 fps a whole step, and the bob drawn from the phase put the stance foot 8cm off the ground. Arms ease (it
  smooths a raise ending).
- The bob: the root goes down by what the longer leg lost of its height, from each leg's thigh and knee this frame
  and its bone heights (`walk_leg_heights`): `reach = upper cos(thigh) + lower cos(thigh + knee)`. The stance foot stays
  within 1cm of the ground, the head bobs 3.5cm in step. The head has no bob of its own (its random oscillation went).
- Written in place (`zox_getm`), not `zox_setv`: deferred, the root moved a frame after the legs.
- Not a body with `RestPosition3D` on its root: XR's, the spine places it.

## Crouching

- `CrouchState` on the skeleton (bit 1 held, bit 2 toggled), eased into `CrouchBlend`. The player: left control held,
  a gamepad's L3 toggles; 0.45 of the walk's speed, no running, shorter strides (`player3d_move_system`).
- Thighs 55 degrees ahead, knees twice it, the feet turned back level: the feet stay under the hips, the bob's height
  maths lowers the body by what the legs lost (17cm), the head camera with it. The step's knee lift is 0.4 of a
  standing one (bent already, a full lift folded the leg to 168 degrees).
- A headset crouches by the player's real body instead (xr/fun/body.c, `HipsShift`, planted feet).

## Swings: punch, place, eat

`bones/dat/swings.c` (the data), `bones/ik/sys/swing.c` (playing it), `bones/fun/swing.c` (`skeleton_swing_begin`).

- A swing (`SwingStart`, `SwingSpeed`, `SwingKind`) runs 0 to 1 over its action's `SwingTime`: from the guard back,
  out to its hit, held, recovered. Poses are the hand's place in the arm's reach: out along the look, in to the body's
  middle, up, and the body's twist. Marks (`SwingMark`) are its events, the first the hit.
- The guard (raised, `swing_guard`): out 0.7, in 0.15, up 0.35, 0.8 of the reach, the elbow bent ~75 degrees. At 0.95
  the arm was held up straight, a salute from outside. It's still in the first person view (tst punch_motion).
- Looking up a pose is placed as looking level, then turned up about the eyes and kept within its level reach (in
  along its line from the eyes): turned about the shoulder with the head's up, the hand went over and behind the head
  (tst guard_pitch). Looking down it turns about the shoulder.
- The shoulder's ik turn (`ik_turn_to`) maps the upper arm's line and its rest front (+z, where the forearm folds) to
  the hand's side: the shortest turn from hanging spun the arm about itself reaching up.
- The off hand (`has_off`, `off_back`, `off_out`): in a punch the left fist comes up, reaches ahead as the right winds
  up, pulls back to the chin as it lands (the recoil), and stays up `swing_off_linger` after, so it doesn't drop
  between punches. `swing_off` gives it the swing's timing with its own poses.
- A new skill's motion is a new entry in `swings`, its reactions observers on its marks.

## XR

- The eyes ride the body's neck (the head bone's pivot), swinging about it with the headset; the view stays where the
  player's eyes are (`xr_body_view_off` takes what the body can't reach).
- The head bone's turn is the headset's with its parent's (the chest's) lean taken off. It turned by both: looking down
  the chest pitched forward and the head past the eyes, they went through it.
- Looking down pitches the upper body only a little (`xr_look_bend_max` 0.3): the head turns at its neck, so the eyes
  swing out ahead of the chest themselves. Pitched far (0.95) the body's neck sank 9.5cm under the eyes.
- `src/xr/tst/eyes.c`: the eyes against every part's box (the head's and hat's too) each frame of `xr_look_down` and
  `xr_lean`, and screenshots of what they see (`tst/xr_eyes/`).

## Looking at it

| Command | Shows |
|---|---|
| `bash bsh/punch.sh` | the punch from 4 angles: `tst/punch_views/punch.gif` (its speed), `punch_slow.gif` (2s), `tst/punch_views.png` |
| `bash bsh/walk.sh` | the walk: `tst/walk_views/walk.gif`, `crouch.gif`, `tst/walk_views.png` |
| `bash bsh/poses.sh` | `body_poses`: held poses from 4 views, holes painted magenta (`--weights`: painted by bone) |
| `bash bsh/ik_poses.sh` | the ik rig's poses (reaching, leaning, crouching) |
| `bash bsh/body.sh` | `body_views`: the whole character, still |
| `--bones` (any of them) | bone gizmos over the body see through and painted by bone: a line joint to joint, a box round each part turned with it, in its bone's paint. In game: the Gizmos debug ui (g), Bones |
| `--xr-sim` | `xr_look_down` and `xr_lean` save what the eyes see to `tst/xr_eyes/` |

- The gif recorder (`src/graphics/fun/gif.c`): `zox_gif_record(path, fps)` from the next frame, `zox_gif_stop()`.
  Each frame keeps its real time, so a slow frame rate plays at the game's speed. `zox_tst_gif_fps` for more frames.
- The scripts pass `--1080p` and your settings (`zox_tst_settings=` for the tests' defaults). At 1080p software gl runs
  ~10 fps: good for finding frame rate bugs, see below.
- Frames out of a gif, and a strip of one view (each view is a quarter of the frame):

```bash
magick tst/walk_views/walk.gif -coalesce /tmp/f_%02d.png
magick /tmp/f_0[0-9].png -crop 960x540+0+0 +repage -resize 50% +append /tmp/strip.png
```

## Workflow

1. Look first: record it (a gif, the 4 views), cut a strip of the view that shows the motion, say what's wrong.
2. Measure it in the test: the thing you see as a number (the hand's reach, the lower foot's height range, the head's
   bob, the eyes' distance into a part), logged each run, then a check on it. A gif alone can't fail.
3. Find the cause before tuning: log the per frame values (phase, each limb's angle, the root) for a few frames. Most
   of these bugs looked like tuning and were structure (pivots, a frame of lag, a race, a double turn).
4. Compare against the commit before: a worktree (`git worktree add --detach /tmp/c2wt <hash>`), the same test
   there. Copy a new test across, swapping what the old code lacks (`BoneCentre` → `BonePosition`).
5. Run it at both window sizes (the test's and `--1080p`): a fix that only holds at one frame rate isn't one.
6. Then the touched tests: `punch_views walk_views crouch punch_motion guard_steady punch_wind_up swing_motion
   eat_swing arms_in_step body_poses body_weights ik_poses skeleton_render bone_rebuild feet_planted head_camera
   xr_body xr_lean xr_look_down`.

## Lessons

- **Pivots at joints.** Limbs turning about their middles looked like a skinning bug (strips at the elbow).
  `bash bsh/punch.sh --bones` shows each box turning about its joint.
- **`zox_setv` is deferred** to the next merge: in a multi threaded system writing another entity, it lands after the
  hierarchy, a frame late. Write in place (`zox_getm`) when the entity is yours to write (a skeleton's own bones).
- **Systems read other entities racily.** In one pipeline step each worker runs every system on its slice: a bone's
  system reading its skeleton can see it before or after the skeleton's own system. Do the work in the skeleton's
  system.
- **Eased poses lag.** Easing a limb towards a moving target lags it by a phase that grows with the frame time; anything
  computed from the target (the bob) then disagrees with the pose.
- **Measure what the bones were placed from.** A test resetting a falling body's position each frame measured against
  the reset place while the bones were placed from the fallen one (5cm at 10 fps): measure against the skeleton.
- **Turns compound down the hierarchy.** A local turn set "in the body's space" is wrong once a parent turns (the chest's
  lean): take the parent's off.
- **A gamepad's first input only switches to it.** The first press after the keyboard is not seen; a test touches the
  stick first.
- **SDL rewrites the keyboard every frame**, a test can't hold a key: drive a gamepad (`zox_tst_spawn_pad`,
  `zox_tst_pad_held`, add buttons with `spawn_device_button`) or set the state the key sets.

## Still to do

- The upper body leaning a little crouched (the lean is the XR spine's now, it owns the root's place).
- A smaller collision body crouched; not walking off edges crouched.
- The first person head bob (3.5cm on a 1m body) might want the camera to take part of it.
- A wider joint blend for sharp bends; animations to and from Blender (todo.md, Bodies and Animations).
