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

Then (2026-11-17, second pass):

- **y was flipped**: `camera_projection_from_angles` mapped up to -1 and render.c drew with `glFrontFace(GL_CW)`.
  On the simulator that drew the world upside down with its faces inside out (the ground on top, magenta backs).
  Fixed: y up, `GL_CCW` as every other camera; tst xr_sim_eyes checks the ground is under the horizon
- **the pose handed back to OpenXR** was mixed (the matrix's rotation, its inverse's position). Each eye's located
  pose (`views[i].pose`) is now kept and submitted as is
- **free roam in the engine**: xr/fun/roam.c, the left stick moves where the head looks, the right turns the body,
  the headset turns the head on top. The camera entity follows (streaming, sound)
- **layout**: xr/openxr (OpenXR only, behind `zox_xr`, tst/cube.c builds from it alone), xr/fun (shared: free roam,
  an eye's render), xr/sim (the simulator), xr/pre and ins (the xr camera). `XrCamera` is in cameras and the screen
  render skips it. `zox_xr_active` replaced `#ifdef zox_xr` in huds and the skybox
- **the simulator** (`--xr-sim`, step 1's first half): both eyes side by side, the head on Q E (turn) R F (tilt),
  the sticks on W A S D and the arrows. Uses the same eye render and free roam as a headset
- src/xr compiles (syntax checked on the board with the OpenXR SDK's headers and `-Dzox_xr`), not yet built for
  Android. A headset still has to show: the swapchain format, tracking, stick directions, comfort
- the 2D ui pass is gone from the XR render (it was commented out); the menu is spawned once, by huds

Then (2026-11-17, third pass):

- **XR controllers** (step 3): a device a hand, derived from the device prefab, its pose in Position3D and
  Rotation3D, its stick and buttons as zevices (no state outside the ecs). OpenXR binds A B X Y, menu, triggers,
  grips and the aim poses (Oculus Touch); the simulator's keys fill them (W A S D the left stick, arrows the right,
  space or enter A, backspace B, escape menu). They play as a gamepad: the right hand navigates and clicks (A or
  its trigger). Free roam reads its sticks from them, only while playing (in menus the sticks are the menus')
- **3D list ui** (step 4's start): spawn_list3, a canvas3 panel of 3D buttons, parented to the player's canvas
  so huds finds and closes it as a 2D menu. zox_huds3D (on with XR) spawns the start label and main menu as 3D
  ahead of the camera. tst xr_menu: A opens the main menu, navigation selects, the stick moves it
- **into a game** (tst xr_play): New Game goes straight in (no 3D text input for the realm menu yet), the XR
  camera streams terrain and raycasts as the game camera does, free roam starts at the spawn (it takes up the
  camera whenever something else places it). No player yet: the head roams free (step 5 is the player)
- to do: rays from the hands (pointer zevices) at 3D buttons, the options and pause menus as 3D lists, tooltips

Then (2026-11-17, lag and room scale):

- the frame begins first (xr_frame_begin_system, zoxp_begin): xrWaitFrame paces the engine's frame and everything
  after it reads this frame's predicted display time (the controllers read the last frame's); a frame not to be
  rendered is still ended; the eyes are located last, just before drawing
- room scale: the camera entity is at the head (the roam's body, then the head's tracked place), turns pivot on the
  head (tst xr_room_scale); the player's character follows it once there is one (step 5)
- OpenXR gives the head and the controllers' poses; the body is the engine's: its arms reach the controllers with
  ik (bones/ik, xr/fun/hands.c)
- desktop syntax check of src/xr/openxr: OpenXR-SDK's headers and a jni.h stub, `-Dzox_xr -DXR_USE_PLATFORM_ANDROID
  -DXR_USE_GRAPHICS_API_OPENGL_ES -fsyntax-only`

## The plan

Each step works on every platform before the next, and each has tests that run on desktop.

### 1. An XR simulator on desktop (the key to the rest)

Most XR work isn't about the headset: two eyes, a head pose, controllers, rays at 3D ui. Make it testable
without one.

- an `xr_runtime` interface: begin a frame, get two eyes (pose, fov, image size), the controllers' state,
  submit. OpenXR implements it on Android and, later, desktop Linux (SteamVR / Monado, and Steam Frame's ARM
  Linux), so keep the OpenXR backend free of Android-only code (the loader, the GL binding and the swapchain
  sit behind small platform calls). A simulator implements it everywhere else: two eyes 64mm apart,
  a fixed headset fov, the head turned by the mouse, controllers from a gamepad or keyboard
- done: `--xr-sim` draws both eyes side by side through the same eye render a headset uses; `zox_xr_active`
  replaced the `#ifdef zox_xr` splits (main.c keeps its own: they're platform start up); tst `xr_sim_eyes`
- to do: the `xr_runtime` interface proper (the simulator and OpenXR still each have their own frame system), the
  mouse turning the head, controllers from a gamepad, `xr_sim_frame` (begin, eyes, submit in order)

### 2. A free roam camera in the engine

- a free roam camera mode (cameras): `free_roam_step` each frame from the active input's two sticks, the head's
  rotation from the runtime (identity without one). Head and sticks combine, as in cube.c: the right stick
  turns the body (yaw), the headset turns the head on top of it, and the left stick moves along where the head
  looks. Desktop already has a free roam camera; it becomes this mode, with the head's rotation identity (or
  the mouse, in the simulator), so one code path serves both
- the XR camera parents the eyes: eye pose, then the camera (camera_xr_eyes checks the order)
- retire tst/cube.c once the engine does what it does: its free roam cubes become a test level scene
  (`zox_tst_level_setup` with the camera in free roam)
- tests: `xr_free_roam` on the simulator: sticks held, the camera moves as camera_free_roam says, the eyes
  follow; the head turned 90 degrees with the left stick held forward moves sideways from the body's facing

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

- builds: desktop, web, Android, Android XR and (once it exists) desktop Linux XR built for every commit that touches cameras, inputs, ui or
  xr (an NDK on the dev board, or a CI runner)
- the simulator tests run on desktop with the rest; a headset check list for what only a headset shows
  (comfort, the y flip, tracking, the swapchain's format)

## Order

1 then 2 gives a free roam camera in the engine on desktop and headset. 3 is needed before the player
(5). 4 can go alongside 3: the ray pointer is both.
