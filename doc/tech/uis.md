# UIs

- [3D UIs: layers and depth](#3d-uis-layers-and-depth): how 3D canvases draw against the world and each other
- [2D and 3D from the same code](#2d-and-3d-from-the-same-code): 3D windows, prefab twins, the deferred spawn gotcha
- [3D lists and scrolling](#3d-lists-and-scrolling)
- [The navigator](#the-navigator-lb-and-rb-through-the-taskbar): LB and RB through the taskbar, every navigator alike
- [Grids](#grids), [tooltips in 3D](#tooltips-in-3d), [render textures in 3D](#render-textures-in-3d)
- [Gotchas](#gotchas) and [testing uis](#testing-uis)

# 3D UIs: Layers and Depth

How 3D UIs (a canvas in the world: npc labels, health bars, popups) draw against the world and each other.

Code: [src/ui/elements3/sys/render.c](../../src/ui/elements3/sys/render.c) (`canvas3_render_system`),
[src/rendering/rendering3/fun/element3.c](../../src/rendering/rendering3/fun/element3.c) (drawing one element, the passes),
[src/ui/elements3](../../src/ui/elements3) (canvases, elements, billboards), tests `ui3_occlusion` and `ui3_overlap`
([src/controllers/tst](../../src/controllers/tst)).

## How they draw

- A canvas (`Canvas3`; `ElementBillboard` ones turn to face the camera: `billboard_system`) is a transform in the
  world; its elements (`Element3`: frames, bars, glyphs) are textured quads laid out on it in pixels (`layout3`), the
  canvas `Scale1` metres high.
- Canvases may number 9000+, their elements under 100 each, so only elements are ordered, never all canvases:
  1. Each canvas gathers its elements (children, in tree order, at most 512) and insertion sorts them by layer
     (every frame for now; later: keep the order, rebuild it when its children or layers change).
  2. Its elements draw in that order: depth test on (the world, canvases already drawn), depth writes off, each
     layer painting over the one below. Any number of layers, no offsets.
  3. Then its depth is written (a depth only pass, colour off, `alpha_cutoff` 0.5), so canvases drawn after it hide
     behind it, whatever order the canvases draw in.
- A hidden element hides its subtree (`canvas3_gather` skips it): texts set their own `RenderDisabled` from their
  text (empty is hidden), so a hidden row's text came back when relabelled. The gather's cap is 512 elements: every
  glyph is an element, a nine row list of 20 letter labels overflowed the old 128.

## Learnings

- Depth offsets don't work for layers. Layers were offsets three times: in local z (`zox_depth_per_layer3`, then 0:
  layers fought), `glPolygonOffset(0, -5000 * layer)` (far labels and health bars showed through walls: depth units
  grow with distance), a world nudge a layer (`element3_layer_offset`, commit f96a61886: deep stacks stood out in
  front). Perspective depth packs its resolution near the camera, so any fixed offset is wrong somewhere; drawing in
  order (commit bda4636c5) is the same at every distance.
- Drawing state leaks between passes: set depth test, depth writes and blending where you draw (a layer once drew
  wholly over the one above it, all or nothing, flaky run to run).

## Translucency between canvases

Canvases aren't sorted, so a far canvas can draw after a near one. Where the near one is translucent (soft edges, a faded
panel), its depth write would clip the far one instead of it showing through.

- The depth pass discards pixels under half alpha, so only solid parts hide what's behind.
- Truly translucent panels are still slightly wrong where they overlap other canvases (rarely seen).
- Translucent world things (water, glass) drawn after UIs have the same problem.
- If it shows: sort the canvases back to front (cost: a sort of every canvas a frame), at least the translucent ones, or
  only those near each other on screen. Reassess sorting all UIs then.

# 2D and 3D from the same code

Games use both: 2D canvases (the screen) and 3D ones (labels over npcs, health bars, XR's menus). 3D uis are built by
the same spawn functions as 2D ones, so a window's code is written once (2026-11-18).

- `zox_huds3D` (src/ui/set/windows.c; on with XR) decides where menus and windows go: the 2D canvas, or a canvas3
  panel ahead of the player. It's the only global; everything else is per parent.
- `spawn_window` (windows/ins/window.c) on 3D huds builds a canvas3 root (placed by `ui_window3_pose`, set by huds:
  `huds3_pose`), a child of the player's 2D canvas, so taskbars, toggles, the navigator and closing find it as they
  find a 2D window. It gets `Window` and `PreInitialize` (window spawn systems run on `Initialize`: a map's, a
  taskbar's link). Its close button as a 2D one's (a twin): a gamepad's B clicks it (cancel_menu_system), no pop in (`LocalScale1` takes a root out of the 3D
  transform hierarchy).
- Prefab twins (src/ui/fun/prefab3.c, windows/pre/twins.c): a 2D prefab's `Prefab3` is its 3D twin, made by the 2D
  prefab's own function on a 3D base (`spawn_prefab_icon(world, element3)`, `spawn_prefab_grid(world, body3)`, ...).
  `spawn_ui`, `spawn_text`, `spawn_header` swap a prefab for its twin when the parent is 3D (`ui_prefab_for`), and
  put it a layer above its parent (`ui_layer3_set`). Twins are Elements too (selection, clicks, active states are
  every element's). Later modules add theirs on the exposed bases (`prefab_element3_twin`, `prefab_frame3_twin`):
  render textures, tooltips. Twinned: elements, frames, texts, labels (`prefab_label2t` too), icons, buttons,
  headers, bodies, grids, lists, scrollbars, handles, sliders, close buttons. `spawn_button_old` (toggles) and
  `spawn_slider` swap too.
- huds' 3D roots: `huds3_spawn_root` (huds/fun/huds3.c), a canvas3 ahead of the head, the canvas's child; the start
  label and the load ui spawn into it with their 2D code (no 3D-only copies left in huds: the options window, the realm
  menus are 3D windows; the new realm menu's fields can't be typed in XR yet, Randomise works)
- Data systems don't care: grids lay out by `LayoutPosition` (layout3 reads it as layout2 does), icons get textures
  through `TextureData`, items, stats, quests, the map all fill their 3D twins unchanged.
- Per parent, so a normal game's 2D windows stay 2D beside its 3D labels; a window spawned into a 3D parent without
  `zox_huds3D` isn't supported yet (it would need a 3D element root, not a canvas3).

## The deferred spawn gotcha

Spawned inside a system, a new entity's components aren't there yet (commands are deferred): `zox_has(window,
Canvas3)` was false on the window just spawned, so its children came out 2D. The 3D entities spawned this frame are
remembered (`ui3_spawned`, by `zox_current_time`) with their layers, so their children find them 3D; the search is
skipped when nothing 3D spawned this frame (2D's spawns pay nothing).

# 3D lists and scrolling

3D lists are 2D list windows (`spawn_window_list`) on 3D huds: spawn_list3 and its menus' branches are gone. What
follows (list3's own) is history, kept for its learnings: the list systems, scrolling and the stack are the same.

- `spawn_list3` (windows/ins/list3.c): a canvas3 window, a header, and its buttons in a list container laid out by the
  2D list systems (`list_system` places them by `ChildIndex` from `ListStart`, `list_render_dirty_system` shows
  `ListVisible` of them).
- Over `list3_rows_max` (8) rows it scrolls; navigating keeps the selection shown: `scroll_list_to_child`
  (elements2/fun/scroll.c), called by `element_navigation_system`, shared with 2D scrollviews (their handle moves too,
  through the list's `ScrollviewLink`).
- `set_list_start` splits from `set_scrollview_start`; a gotcha there: `ListStart` set in a system is deferred, read
  back it's the old value (the scrollbar handle stopped moving: tst scroll_wheel), so pass the clamped value on.
- A canvas3's height in metres is its `Scale1`; layout3 spans the canvas one high, so more rows means smaller rows
  unless the list caps its rows.
- 3D lists are `Window`s without `WindowLayer`: the 2D window stack (`canvas_stack_system`) skips them (it crashed
  reading one when a 2D window opened over a 3D list).

# The navigator: LB and RB through the taskbar

- huds/sys/navigator.c: paused, LB and RB (a headset's grips are LB and RB) step through the taskbar's uis one at a
  time, wrapping round: the one it's in closes, the next opens (through the taskbar's own toggle,
  `taskbar_frame_toggle`, so frames light as a click lights them), and the navigators' selections are cleared so
  `element_navigation_begin_system` selects in it.
- Every navigator alike: it reads LB and RB from all the player's devices; XR controllers are gamepad devices.
- "The one it's in": the one it last opened, else the selection's window, else the first open. A ui it moved into
  because it was already open (a hud: the action bar) stays open when it moves on (it closed the action bar).
- On 3D huds the taskbar is a 3D row above the menus (`spawn_taskbar3`): the same frames (`taskbar_spawn_frames`),
  its panel the `WindowBody` so `link_window_to_taskbar` and `taskbar_begin_system` find frames the same way.
- `element_navigation_begin_system`: a gamepad with nothing selected starts in an open window with something to
  select (a `NavigationElement`), not one closing (`UIClosing`): any `Window`, no tag needed. Never while its player
  is playing (`zox_player_state_playing`: in game it selected the action bar); its menus (paused), a dialogue and a
  bed (`zox_player_state_sleeping`, its "Wake") are other player states, so their windows are navigated.

# Grids

- A datagrid's icons are `NavigationElement`s (datagrid.c, datagrid3.c): inventory, skills, stats, quests,
  crafting, gear.
- Tested by `navigator_grid`: a window of its own with a grid of frames (minimal: not the game's windows).
- `element_navigation_system`: in a grid (the selection's parent within 3 levels has `GridSize`) the stick's x steps
  one along and y steps a row (its columns); elsewhere x is the selection's own: a selected slider steps a twentieth
  (a byte one 1) a push, again every 0.25s held (`navigation_slider_system`, ui/navigation/sys/gamepad_ui.c).
- Tab windows: LB and RB show the tab before and after (`navigation_tabs_system`), the selection on its button; down
  from a tab is its list, up from the list's top its tab; a hidden tab's elements aren't stepped to, and `ChildIndex`
  neighbours are siblings only (tabs and lists share indexes). tst xr_options, options_tabs
- A stick's x is stored as SDL has it: right is -x (XR's controllers flip OpenXR's to match).
- Steps are by place in the window's NavigationElements; a grid's icons are spawned row by row, so one along is +1
  and a row is +columns.

# Tooltips while navigating (2D)

- A 2D canvas's tooltip follows the mouse (`MouseElement`, `mouse_element_system`); while the player navigates (a
  gamepad, keyboard navigation) it sits under the selected ui instead, its top right at the ui's bottom right
  (`tooltip_navigated_system`): the mouse is still, so the tooltip sat where it last was (a corner on a Steam Deck).
- It runs in `zoxp_post_update`, after the mouse's (`zoxp_update`: in the same phase the mouse's ran after and put
  it back), before the layouts.
- Its top right corner lands half its size past its `LayoutPosition` (beside a mouse's cursor), so that's taken off.
- Tested in `navigator_grid` (the tooltip under the icon the stick moved to).

# Tooltips in 3D

- A 2D canvas has one tooltip following the mouse; each 3D window gets its own (spawned by `spawn_window`, from
  `prefab_tooltip`'s 3D twin: a 3D text with a frame behind it). Uis link to the nearest `Canvas`'s tooltip as in 2D
  (a canvas3 is a Canvas).
- `tooltip3_follow_system` puts it centred under the ui it shows, a little nearer the eye, turned as its window is;
  metres a pixel are the window's `Scale1 / LayoutSize.y`. It has its own transform (`DisableParentTransform`).
- A 3D canvas without a tooltip isn't an error (the tooltip lookup logged for every icon).
- It's centred (`MeshAlignment`, `TextAlignment`): a 2D one's top right corner sits at the mouse.
- Its follow system is multithreaded, writing only its own transform: as a main thread system in `zoxp_update` it
  shifted the pipeline's sync points, and a race elsewhere lost the skeleton's `ActiveMesh` (the mirror found no mesh).
- Not right yet: it draws as a thin dark strip, no text visible (doc/todo.md).

# Render textures in 3D

- A render frame (the mirror, a mug shot) is a camera drawing into a texture shown on an element; in 3D its element is
  `prefab_render_texture`'s twin (the camera's texture on an element3).
- The camera and what it films are children of the frame, and in a 3D window that's inside the window's 3D transform
  hierarchy: any child with `Position3D` is placed at its parent plus its local position, so the camera moved onto the
  window (every 3D ui vanished from the eyes) and the filmed mesh away from its camera (blank). They get
  `DisableParentTransform`: kept in the tree (deleted with it) but placed by themselves.
- Camera filtering (`CameraFilter`, `CameraRenderer`) wasn't the problem: 3D uis don't filter.

# Gotchas

- `zox_instance(prefab)` declares its own `entity e`: inside a branch it shadows an outer `e` (every 2D window crashed
  when the 3D branch was added); use `zox_ins(world, prefab)` to assign.
- Child order: a fragmenting `ChildOf` reorders children when one changes table (a row gets a `SettingLink`); lists
  order by `ChildIndex` (`get_list_children_ordered`); `spawn_ui` parents non fragmenting.
- `prefab_gamepad` carries SDL's controller: an instance with no SDL controller behind it is closed and deleted.
- A gamepad's first input switches the player's device mode (that press is spent on it), until then it's `Disabled`.

# Testing uis

- `zox_log` goes to the test's log (tst/<name>.log), not the console: grep the log for debug lines.
- `zox_tst_set_setup(name, setup, menus)`: menus 1 stays in the menus (the test drives them), 0 autostarts a game.
- Script a gamepad (huds/tst/navigator.c): a plain device (`prefab_device`, `Gamepad`, a navigator, its buttons and
  stick), its buttons written in the inputs phase (`zoxp_inputs_extract`) as pressed or released this frame, the way
  the XR simulator scripts its controllers (`xr_sim_script_*`). Wake it with a first press before the real one.
- Screenshots per step (`zox_screenshot_request`), then look at them: `montage` a set into one image. Request a
  screenshot on a frame you know the state's right, not on a fixed frame of a cycle (it missed the window).
- tsts: `navigator` (a gamepad, 2D: pause, LB and RB, every ui, a grid), `xr_navigator` (3D: every ui 3D, a tooltip),
  `xr_options` (a 3D list scrolling), `scroll_wheel`, `list_order`, `ui3_occlusion`, `ui3_overlap`
- 3D uis against the world (`ui3_occlusion`, `ui3_overlap`):
  - Turn fog off: 3D UIs are fogged like the world, far off pure red isn't red any more and a test sees nothing.
  - Flat bare land (`zox_flatlands`, `zox_disable_towns`, `zox_disable_vegetation` in the test's setup): random hills,
    homes and trees stood in front of the far UI, run to run.
  - Place things along the camera's real view (its `Rotation3D`), and keep the head camera's 180° turn
    (`is_camera_positive_z`) when overriding its rotation, else it looks back through the player's own head.
  - Count pixels of a pure colour over the whole screenshot, not one pixel at the middle.
  - Check the bug back: the test failed on the old offset (125+ red pixels through the ground 50 blocks off), and an
    earlier version passed on both (too near, the old offset held up there).
