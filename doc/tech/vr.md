# VR

Headsets (OpenXR, `src/xr`): what keeps a player comfortable, and how a headset player moves. Building and testing the
apks: [Android](../engine/android.md); the body and its stepping: [Bones](bones.md).

## Comfort rules

- **The camera is the real head, always.** Never smooth, lag, ease or push the view against where the player's head
  is: the eyes and the inner ear disagree, and that sickens.
- **No accelerating view.** Speeding up, slowing down and sliding to a stop are felt as sickness in a headset (the
  eyes see motion the inner ear doesn't). Moving the view by the stick: a steady speed, on at once, stopped at once.
- **Shake and bob make it worse**: keep what the view rides on steady.

## Walls and ledges

How VR games handle them (Half-Life: Alyx and most others):
- **Moving by the stick (or teleporting)**: stopped by collisions, as any player.
- **Leaning the real head into a wall**: it can't be stopped (the head really moved), so the view fades to black
  while the head is inside geometry, till it's pulled back. Never push the view back out: moving the view against
  the head is one of the worst things for comfort.

## Moving by the stick (doc/plans/movement.md, step 4)

The body walks by the move like every character; the view doesn't (2026-11-27):
- **the view** moves at the stick's speed at once, steady, and stops at once (`xr_stick_velocity`, written by
  controllers3/sys/movement.c `player3_move_xr`)
- **the body** walks after it by the move (character3_move_system: physics, speeding up, sliding, collisions, the
  speed stat), pushed toward the view: its move is the stick's and `xr_stick_catch` (6) metres a second for each
  metre it's behind; a push past the walk's top speed is allowed to half again, so it catches up walking
- **the lead** (`xr_stick_lead`, xr/fun/body.c): how far the view is ahead of the body, the stick's move less the
  body's each frame, counted after physics where the eyes are drawn, at most 0.25m (a body a wall stops stops the
  view that far on, inside its own width). Room walking adds to it too: each room step (the hips past the real
  neck's reach, `xr_room_walk_system`) is lead, the move walks the body after it, no velocity set (doc/tech/physics.md)
- **the room walk runs after physics** (`zoxp_layouts`, multi threaded on the xr camera): a frame late, the body
  catching up anyway; its lean (`SpineLean`, `HipsShift`) is read next frame by the head, spine, feet and hands alike
- **never into a block**: the lead is shortened till the eyes (the camera) led, and 6cm past them its way (the near
  plane), are in the air (`xr_stick_lead_clear`). Without it a body stopped at a wall led the eyes 21cm into it
- **turning by the right stick turns the view about the head**: rotation.c turns the body about its middle, the eyes
  ~12cm ahead of it swung sideways (~0.3m/s turning on the spot); the lead takes the swing back (`xr_stick_turn`,
  only the stick's turn: the body following the real head turns the room back itself), the body walks back under
- numbers (tst xr_body, xr_stick): the view at speed in 3 frames, steady to 0.1% at 72 and 90 fps and turning,
  stopped in 3 frames; turning on the spot the eyes move under 0.005m/s; at a wall the eyes never in a block; the
  body leads by up to 6.5cm as it starts, under a mm walking, slides ~10cm past as it stops then steps back
- the body's own eyes can still reach into a wall: its collisions are shallower than its eyes' reach (~12cm ahead of
  its middle), that's the head into a wall fade (above), not done
- the balancer: the spine and feet (xr/fun/body.c, bones) already keep the body under the head for room walking;
  stick walking uses them as is. To try by play tests: the stop's overshoot, the hands lagging the real ones as the
  body starts (they're the body's, on the arms' IK)
- the real head leaning into a wall fading the view (above): not done yet

## Code

- `src/xr/fun`: `body.c` (the body under the head), `hands.c` and `controller.c`, `roam.c`, `eye_render.c` (each eye's
  view), `scale.c`, `runtime.c`; `src/xr/openxr` (the runtime), `src/xr/sim` (a headset simulated for tests)
- tests: `src/xr/tst` (xr_body, xr_stick, xr_lean, xr_grip, xr_look_down)
