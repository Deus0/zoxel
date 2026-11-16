# XR: free roam, inputs and 3D ui in the engine

Where XR is (2026-11-17) and the plan to bring it into the engine without breaking desktop, web, Android or Steam Deck.

## Where it is

- **tst/cube.c** is a standalone Android app: OpenXR, free roam among cubes, both sticks. It works. It isn't the engine.
- **The engine path** (src/xr, `zox_xr`, Android only) spawns one camera at the menu's spot and renders the 3D
  render systems once per eye (xr/sys/render.c). The 2D ui pass is commented out. Nothing reads the sticks:
  `xr_get_left_stick` is only called by cube.c.
- **Platform splits are compile time**: `#ifdef zox_xr` in main.c, huds/fun/game_ui.c (no player cameras, no
  render texture), weathers/ins/skybox.c (a smaller sky). So a desktop build never runs any XR code, and every
  XR bug needs a headset to find.

Cleaned up now:

- the shared maths left xr: `camera_projection_from_angles` and `free_roam_step` are in cameras/fun, built and
  tested on every platform (cameras/tst/xr.c: camera_projection_angles, camera_free_roam, camera_xr_eyes)
- xr's copies of engine maths went: `pr_to_matrix` was `float4x4_transform`, `matrix_multiply(a, b)` was
  `float4x4_multiply(b, a)` (checked equal over 1000 random poses). The engine's multiply is "a, then b",
  OpenXR's samples are "b, then a": use the engine's, in its order
- the xr module is always imported (its OpenXR parts behind `zox_xr`)

Found, needing a headset to settle:

- **y is flipped** in the projection (`2 / (tan down - tan up)`, up maps to -1; tst camera_projection_angles logs
  it), and render.c sets `glFrontFace(GL_CW)` for "XR triangle direction". A flipped y reverses winding, so the
  two likely cancel. Fix both together: y the usual way (`2 / (tan up - tan down)`) and `GL_CCW` like the rest
  of the engine, then look.
- **The pose handed back to OpenXR is mixed** (xr/fun/helpers.c `float4x4_to_xr_pose`): the matrix's rotation
  with its inverse's position. OpenXR wants each eye's pose as located (`views[i].pose`); passing those straight
  back is simplest and right.
- None of src/xr has been compiled since this cleanup: there's no Android NDK on the dev board. Build with
  `bash bsh/android.sh zoxel opengl sdl --debug --xr` before anything else.

## The plan

Each step works on every platform before the next, and each has tests that run on desktop.

### 1. An XR simulator on desktop (the key to the rest)

Most XR work isn't about the headset: two eyes, a head pose, controllers, rays at 3D ui. Make it testable
without one.

- an `xr_runtime` interface: begin a frame, get two eyes (pose, fov, image size), the controllers' state,
  submit. OpenXR implements it on Android. A simulator implements it everywhere else: two eyes 64mm apart,
  a fixed headset fov, the head turned by the mouse, controllers from a gamepad or keyboard
- `--xr-sim` draws both eyes side by side in the window, through the same render path a headset uses
- tests: `xr_sim_eyes` (two eyes drawn, each with its own view: screenshot halves differ by parallax, not
  content), `xr_sim_frame` (begin, eyes, submit in order, every frame)
- replace the `#ifdef zox_xr` splits in huds, skybox and main with a runtime `zox_xr_active`, so the simulator
  takes the same branches a headset does

### 2. A free roam camera in the engine

- a free roam camera mode (cameras): `free_roam_step` each frame from the active input's two sticks, the head's
  rotation from the runtime (identity without one). Desktop gets a fly camera from it too (gamepad or WASD)
- the XR camera parents the eyes: eye pose, then the camera (camera_xr_eyes checks the order)
- retire tst/cube.c once the engine does what it does: its free roam cubes become a test level scene
  (`zox_tst_level_setup` with the camera in free roam)
- tests: `xr_free_roam` on the simulator: sticks held, the camera moves as camera_free_roam says, the eyes follow

### 3. XR inputs as an input device

The inputs module builds devices from parts (`zevice_stick`, `zevice_button`, `zevice_pointer`). An XR
controller is one more device, so the existing controllers work without knowing about XR:

- two XR controllers (left and right), each with a stick, trigger, grip and a/b buttons, and a pointer (its pose,
  for rays). OpenXR fills them on Android, the simulator from a gamepad
- the left stick moves the player as a gamepad's does, the right turns (snap turning as an option, it's more
  comfortable in VR)
- the action bindings (xr/fun/input.c) grow from two sticks to all of the above, with profiles besides Oculus Touch
- tests: `xr_inputs` (a simulated controller's stick moves the player as a gamepad's would; its trigger clicks)

### 4. 3D ui mode

- a runtime ui mode: every canvas as a 3D canvas (ui/elements3, layout3) in front of the head, not a screen
  overlay. XR forces it on, desktop can turn it on to test (`--ui3d`)
- pointing: the controller's ray hits a 3D canvas, its element is hovered and clicked, as a mouse's is. The
  ray is a pointer device (zevice_pointer), so tooltips, clicks and drags work unchanged
- the menus, pause and inventory each spawn as a 3D panel a metre ahead, turned to face the head
- tests (simulator or `--ui3d`): `ui3d_menu` (the start menu is a 3D canvas in front of the camera, a ray at
  its new game button starts a game), `ui3d_pointer` (hover and click through a ray, a tooltip shows)
  ui3_occlusion and ui3_overlap already check 3D ui draws right against the world

### 5. The player in XR

- the head camera follows the character, the headset's pose added to it; the body turns with the right stick,
  the head with the headset
- the hud as a 3D panel at the wrist or below the view
- tests: `xr_player` on the simulator (spawned, walks with the left stick, the eyes stay at its head)

### 6. Keeping every platform working

- builds: desktop, web, Android and Android XR built for every commit that touches cameras, inputs, ui or
  xr (an NDK on the dev board, or a CI runner)
- the simulator tests run on desktop with the rest; a headset check list for what only a headset shows
  (comfort, the y flip, tracking, the swapchain's format)

## Order

1 then 2 gives a free roam camera in the engine on desktop and headset. 3 is needed before the player
(5). 4 can go alongside 3: the ray pointer is both.
