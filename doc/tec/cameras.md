# Cameras and Viewports

What the cameras do now, how they're tested, and what games commonly need that's still missing.

Code: [src/cameras](../../src/cameras) (components, matrices, viewports), [src/rendering/cameras](../../src/rendering/cameras)
(drawing each camera), [src/huds/fun/game_ui.c](../../src/huds/fun/game_ui.c) (the players' cameras and canvases).

## How a frame draws

1. `projection_matrix_system`: `PixelSize`, `FieldOfView`, `CameraNearDistance`, `CameraFarDistance` → `ProjectionMatrix`
   (`orthographic_matrix_system` for `Orthographic` cameras).
2. `view_projection_matrix_system`: `TransformMatrix` → `ViewProjectionMatrix`, `FrustumCorners`, `CameraPlanes`,
   `Position3DBounds` (the frustum's box). Streaming culls chunks with these (`camera_sees_bounds`).
3. `camera_render3_system`, each 3D camera (`!CameraUI`): sets its viewport, clears only its rect (`zox_gpu_clear_rect`,
   scissored, `glClear` alone ignores the viewport), runs the render3D systems.
   - A `RenderCamera` draws to its `FrameBufferLink` texture, shown on the canvas by a render texture element
     (post processing: blur, vignette).
   - Others draw straight to their part of the screen (`spawn_viewport_camera`).
4. `camera_render_ui_system`, each `CameraUI`: the canvas' uis (including the game view's render texture).

## Viewports

- `ScreenToCanvas` is a camera's part of the window, 0 to 1: size `xy`, position `zw` (gl's y, from the bottom).
- `viewport_grid_cell(index, columns, rows)` makes one, index 0 the top left. Split screen is `viewport_grid_cell(i, players, 1)`.
- `screen_to_canvas_size` / `_position` turn it into pixels, from rounded edges so a grid covers every pixel once.
- `viewport_resize_system`: on `WindowSizeDirty`, every `ViewportCamera` linked to the app is re-placed.
  `camera_viewport_size`: a `RenderCamera` is scaled by `viewport_downscale` (4 on web), screen cameras keep full size.
- `spawn_viewport_camera(world, app, position, rotation, fov, screen_to_canvas)`: a 3D camera
  (`prefab_camera_viewport`, no render texture) drawing to its rect, kept sized with the window, delete it to remove.
- `camera_look_at(position, target)`: the rotation to face a point (cameras look down -z, y up).
- `camera_project_point(view_projection, point)`: where a point lands, -1 to 1 across the viewport.

## Filtering: who draws what

- `CameraRenderer` (a link on a renderable): only that camera draws it.
- `CameraFilter` (a tag on a camera): it only draws things linked to it.
- Unfiltered cameras draw everything not linked to another camera.
- Used by `tst model_views` (a copy of the model per view, on gray: no sky or terrain).

## Tests

| Test | Checks |
|---|---|
| `camera_viewports` | grid cells placed right, grids tile odd window sizes with no gaps or overlaps |
| `camera_projection` | fov edges land on the viewport's edges, points behind are rejected |
| `camera_orthographic` | a unit high view as wide as its aspect, no shrinking with distance |
| `camera_look_at` | 500 cameras: the target in the middle, up stays up |
| `camera_viewport_spawn` | 4 viewport cameras sized, projected, resized with the window, each its own far distance, forgotten when deleted |
| `camera_split_screen` | `--splitscreen`: each player's game and ui camera and canvas in its half, resized |
| `camera_downscale` | `viewport_downscale` 4: render cameras a quarter, screen cameras full size, through a resize |
| `camera_frustum` | known boxes and 14k random points against the projection: nothing seen is culled, nothing hidden kept |
| `camera_filter` | screenshots: a filtered camera draws only its own, an unfiltered one skips others' |
| `model_views` | a realm model from 4 viewports on gray (`bash bsh/model.sh <name>`), each view drew it |

Each was checked against the bug it's for (putting the bug back fails it): the full screen clear, the `Camera3` scaling,
the global far distance, a flipped frustum plane, a reversed yaw, filtering skipped.

## What games use that's missing

Ordered by use. Keep to the engine's way: generic shared components and tags for systems, not a component per feature.

1. **Draw order**: cameras draw in the ecs' order. Games give each a priority (Unity's `Camera.depth`), lower first,
   so a picture in picture (rear view mirror, portrait) draws over the main view, and mirror and portal cameras
   draw their textures before the main camera uses them (todo.md: Mirrors, Portals; the minimap exists). A shared order value
   (like `RenderOrder` on render systems), the render3 system drawing cameras sorted by it.
2. **Culling masks / layers**: `CameraRenderer` is one camera or all. Games use a bit mask on cameras and things:
   the player's own body hidden from the first person camera but seen in mirrors, ui only objects, debug layers.
3. **Clear options**: every camera clears color and depth. An overlay camera clears depth only (hands and weapon
   drawn over the world without clipping into walls) or nothing (layering).
4. **Orthographic size**: fixed 1 unit high (`calculate_orthographic_matrix`). Isometric, top down and map cameras
   need it from the camera (its zoom).
5. **Turning a camera off**: the generic `Disabled` tag (core, render systems already skip `!core.Disabled`) or flecs'
   own disabling, the camera render systems skipping it. For cutscene cameras, minimaps, photo modes.
6. **Smaller**: screen to world rays from any camera (raycasts only use the main one), per camera fog and sky,
   resolution scale per camera (one global `viewport_downscale` now).

## Notes

- Screenshots (`zox_gpu_save_screenshot`) read the whole window at its `WindowSize`, not the last viewport set.
- Camera rays (`calculate_new_frustum`) use their own short far distance on purpose, not the camera's.
