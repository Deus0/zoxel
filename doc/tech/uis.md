# 3D UIs: Layers and Depth

How 3D UIs (a canvas in the world: npc labels, health bars, popups) draw against the world and each other.

Code: [src/ui/elements3/sys/render.c](../../src/ui/elements3/sys/render.c) (`canvas3_render_system`),
[src/rendering/rendering3/fun/element3.c](../../src/rendering/rendering3/fun/element3.c) (drawing one element, the passes),
[src/ui/elements3](../../src/ui/elements3) (canvases, elements, billboards), tests `ui3_occlusion` and `ui3_overlap`
([src/controllers/tst](../../src/controllers/tst)).

## Before (until 2026-11-14)

- A canvas (`Canvas3`, `ElementBillboard`) is a transform in the world turned to face the camera (`billboard_system`
  sets the rotation of it and every child); its elements (`Element3`: frames, bars, glyphs) are textured quads laid
  out on it in pixels (`layout3`), scaled by the canvas's `Scale1`.
- Layers first went into the layout: an element sat `layer * zox_depth_per_layer3` in front of its canvas in local z.
  It was set to 0 (layers then fought, or the billboard's facing put higher layers behind).
- Then `element3_render_system` drew with `glPolygonOffset(0, -5000 * layer)`: a constant in depth buffer units. Near,
  it was a hair; far, the same units are a long way in the world, so far labels and health bars showed through walls
  and buildings ("visible through buildings at distances").
- The draw order was whatever order the query returned; with depth writes on, the offset alone kept layers apart.

## Learnings

- Depth precision isn't linear: perspective depth packs most of its resolution near the camera. Any fixed offset in
  depth units is wrong somewhere; offsets in the world (or no offsets, drawing in order) behave the same at every
  distance.
- Drawing state leaks between passes: set depth test, depth writes and blending where you draw (a layer once drew
  wholly over the one above it, all or nothing, flaky run to run).
- Testing it:
  - Turn fog off: 3D UIs are fogged like the world, far off pure red isn't red any more and a test sees nothing.
  - Flat bare land (`zox_flatlands`, `zox_disable_towns`, `zox_disable_vegetation` in the test's setup): random hills,
    homes and trees stood in front of the far UI, run to run.
  - Place things along the camera's real view (its `Rotation3D`), and keep the head camera's 180° turn
    (`is_camera_positive_z`) when overriding its rotation, else it looks back through the player's own head.
  - Count pixels of a pure colour over the whole screenshot, not one pixel at the middle.
  - Check the bug back: the test failed on the old offset (125+ red pixels through the ground 50 blocks off), and an
    earlier version passed on both (too near, the old offset held up there).

## Then (2026-11-14, commit f96a61886)

- Each element is depth tested and written like the world, its layer nudged toward the camera in the world:
  `element3_layer_offset` (0.01) a layer, or `element3_layer_offset_far` (0.002) of its distance when further.
- Before: `glPolygonOffset(0, -5000 * layer)`. Depth units grow with distance (depth precision falls off), so far UIs
  were pulled through walls and buildings.
- Weakness: every layer is a little nearer, so deep stacks (100 layers) end up visibly in front of the canvas.

## Now: layers in order, the depth buffer orders the canvases (2026-11-14, commit bda4636c5)

Canvases may number 9000+, their elements under 100 each, so only elements are ordered, never all canvases:

1. Each canvas gathers its elements (children, in tree order, at most 128) and insertion sorts them by layer.
   For now this runs every frame; later: keep the order and rebuild it only when its children or layers change.
2. Draw a canvas's elements in that order: depth test on (the world, canvases already drawn), depth writes off, each
   layer painting over the one below. Any number of layers, no offsets.
3. Then write the canvas's depth (a depth only pass of its elements, colour off, `alpha_cutoff` 0.5), so canvases drawn after it are hidden behind
   it, whatever order the canvases draw in.

## Gotcha: translucency between canvases

Canvases aren't sorted, so a far canvas can draw after a near one. Where the near one is translucent (soft edges, a faded
panel), its depth write would clip the far one instead of it showing through.

- The depth pass discards pixels under half alpha, so only solid parts hide what's behind.
- Truly translucent panels are still slightly wrong where they overlap other canvases (rarely seen).
- Translucent world things (water, glass) drawn after UIs have the same problem.
- If it shows: sort the canvases back to front (cost: a sort of every canvas a frame), at least the translucent ones, or
  only those near each other on screen. Reassess sorting all UIs then.
