# Outlines

Dark (or coloured) outlines on vox models: characters, npcs, items, slimes. todo.md: "Outline Rendering on Vox
Models, dark outline", gdd.md: "Outlines on models". Built on gl and vulkan (2026-11-26): the screen space pass (A),
the Camera Outlines setting (unsupported, greyed: gles2, a vulkan gpu without independentBlend). tst outline_views.

## How it works (built)

- **Targets** (graphics/opengl/fun/rbo.c): a render camera's depth "render buffer" is a depth texture now, with an
  outline mask texture beside it (RG32UI, colour attachment 1), kept by the depth texture's id so every path that
  makes, sizes, links or frees one does both. gles2 keeps its renderbuffer, no outlines.
- **Mask** (colored3D.frag, vox_instance.frag, bone.frag, under `#ifdef zox_outlines`, which the gl shader loader
  defines for glsl 300 and up): an outlined draw writes its outline id and its window depth's bits
  (`floatBitsToUint(gl_FragCoord.z)`). Integer: blending never touches it, so see through shells write it exactly.
  Only outlined draws turn it on (`outline_mask_begin` / `_end`, rendering3/fun/outlines.c, `zox_gpu_outline_mask`):
  vox meshes, vox instances (not grass), skeleton meshes. The camera clears it (`zox_gpu_clear_outline_mask`).
- **Order**: solid things that aren't outlined draw at render order 0 (cpu and gpu chunks, textured items), outlined
  vox models at 1, so nothing solid hides a mask pixel after it's written.
- **Pass** (render_texture_rgb.frag, render_texture_rgba.frag, `outline()`): per pixel, 8 directions out to 8
  pixels; a neighbour with a mask id nearer than this pixel by 4 to 24 model voxels draws its line. A pixel's model
  depth is the nearer of the depth buffer's and the mask's (a shell writes no depth). Width: a block texture's texel
  in world units (`zox_outline_width`, 0.5 / 16), as the outlines baked into block textures, over depth, 1 to 5
  pixels, x0.6 to 1 by the gap, x0.8 to 1.2 by the light's side; alpha 0.5 to 1 by the gap; colour a 0.12 shade of
  the model's own pixel, near black as the blocks' (the user: match the blocks' style, 2026-11-26). Near and far come from the
  camera's projection matrix (rendering/cameras/fun/outlines.c), the depth and mask on texture units 1 and 2.
- **Game view rgb**: it used rgba because the rgb shader's `vec3` output left alpha undefined, the blended view drew
  black on gl; the rgb shader writes alpha 1 now and the game view is rgb (`zox_game_render_texture_alpha`).
- **Found on the way**: basic3_render_system (skybox) had no `!core.Disabled`, a Disabled skybox still drew; tst
  eyelids took its open view under the screen's fader (waited frames): it waits for the fader to go.
- **Vulkan** (graphics/vulkan/fun/targets.c): the rbo has a mask image (R32G32_UINT) beside its depth, the texture
  passes a third attachment (cleared as they begin) and both left shader readable (depth read only, mask shader
  read); pipelines on a render texture get the mask's blend state (written only with `mask` in the key, which
  needs independentBlend); samplers read their unit (`sampler_units`, set by glUniform1i as gl's), units past 0 an
  rbo's depth or mask (`vk_rbo_mask_bit`). The pass binds the units even with outlines off, a sampler's image type
  is checked. lavapipe: gpu meshed skeletons vanish with the mask written (todo.md Bugs), radv draws it all.
- What isn't yet, in order (the user, 2026-11-26): viewport cameras (split screen, model_views), xr; then a
  colour per model: an `OutlineColor` on vox models, selection animates it (the mask's id picks it). Later: fog fade
  (fog gets its own rework first), terrain maybe, matching the outlines baked into block textures. Every vox model
  is outlined now, block voxes too (chests, doors), not grass.


## Wants

- **Not uniform**: a line of one pixel width everywhere looks cheap. Width and darkness vary (distance, depth gap,
  light side, per model).
- **A colour per model**: selection and targeting highlight a model by changing its outline colour.
- **Transparent models**: slimes are (or will be) see through at their edges, they still get an outline, and the
  outline never shows through their body.
- **Every vox draw path**: `vox_render_system`, `vox_transparent_render_system`, `vox_instance_render_system`
  (minivoxes, instanced characters), `skeleton3_render_system` (boned characters), gpu meshed voxes (`gpu_mesher_draw`)
  and vox lods.
- **Both backends** (gl, vulkan; gles and webgl 2 too), split screen, xr (two eyes), web's `viewport_downscale` 4.
- **An engine feature**, a setting with an off switch (`--outlines` / `--nooutlines`), off costs nothing.

## Where things are now

- Vox vertices are a position and a colour (`colored3D.vert`, `vox_instance.vert`, `bone.vert`), no normals. cpu
  meshes are greedy quads; gpu meshed vertices pack the face direction and the corner's side of the face in `w`
  (`gpu_mesh_vertex_*`, doc/plans/gpu_meshing.md).
- A player's `RenderCamera` draws into its `FrameBufferLink` texture with a depth + stencil renderbuffer
  (`RenderBufferLink`), post processed on the canvas (blur, vignette). Viewport cameras (`spawn_viewport_camera`,
  split screen, model_views) draw straight to the screen.

## Two approaches

Both stay possible; the prototype (step 1) decides. They share the per model data (Outline component and colour
table) and the tests.

### A. Screen space pass (cheapest, tried first)

One fullscreen pass per camera after the 3D draw, reading depth and a model mask.

- **Mask target**: every outlined model writes into an extra RG8 colour attachment, not blended: R the model's
  outline id (0 = none), G its width scale. Transparent models write it too (their colour blends, the mask doesn't),
  so a slime's silhouette is found without its depth.
- **Depth texture**: the render camera's depth renderbuffer becomes a sampled depth texture (gl and vulkan).
- **Edges**: a pixel is outline when a neighbour within the width has another id, or (opaque models) is nearer by more
  than a threshold, giving inner edges (an arm in front of a body). Pixels inside a transparent model aren't
  outlined by models behind it: the nearer id wins.
- **Width varies**:
  - by distance: a world width over linear depth, thick near, thin far (what a hull gives);
  - by depth gap: a big step (model against sky) thicker and darker, a small one (hand on body) thin and faint;
  - by light side: thicker away from the sun (an inked look), from the sun direction and the depth gradient;
  - per model: the mask's G;
  - optional noise along the line.
  - up to about 4px: 8 to 12 samples a pixel (a ring at the width). Wider needs a jump flood pass, not planned.
- **Colour**: the id indexes a colour table (a uniform array, 256 entries). Default entry: a darker shade of the
  model's colour, or black. Selection sets a highlight entry.
- **Fog and light**: the outline fades with the model's fog (the pass reads depth, fog's formula is known) and
  darkens or fades at night so it doesn't glow in caves.
- **Anti aliasing**: the edge blends by how far the pixel is from the width (smoothstep), so lines aren't stair
  stepped.
- Costs: an RG8 target and a fullscreen pass a camera, the same for 1 model or 500.
- Limits: viewport cameras need a frame buffer (or have no outlines); width is in render pixels, so web's
  downscale scales it (set the width from output pixels); the overlay camera (first person hands) needs its own
  mask or its hands go unoutlined.

### B. Voxel hull (the Unity way, upgraded)

A second draw of each model, front faces culled, every face pushed out. On voxels a face's corners are exact, so no
averaged normals are needed:

- Each quad moves out along its face normal **and** grows in its own plane by the same width, so neighbouring faces
  overlap at every corner: no gaps on hard voxel edges. Needs each vertex's face direction and its corner side
  (gpu meshes have both in `w`; cpu meshes would add one byte a vertex, or derive it in the mesher).
- Width in the vertex shader: world units, or scaled by clip `w` for a pixel width, varied per model.
- **Transparent models**: the hull's back faces would show through a see through body. The body writes a stencil
  bit, and the hull draws only where it's unset.
- Colour per model is a uniform, so selection is trivial; works on viewport cameras and the overlay camera.
- Costs: every outlined model drawn twice (characters are the most drawn thing), the face data in the vertex format.
  Inner edges aren't drawn, only the silhouette.

## Steps

1. **Prototype** (no engine changes): a test scene, a void camera (the eyelids pattern) on a plain background, with a
   cube, a boned character and a transparent slime. Approach A as a test-only pass, screenshots at a few widths and
   styles (distance, depth gap, light side), gl. Compared by the user against the old Unity look. If it looks wrong,
   prototype B the same way.
2. **Per model data**: an `Outline` tag on outlined models plus an id into the realm's (or the renderer's) colour
   table, reusing `Color` for the colour entity if possible (ask before adding components). Selection / targeting
   swaps the id to a highlight entry.
3. **Render targets** (A): depth texture and the mask attachment on render cameras, gl then vulkan; the vox shaders
   (all the draw paths above) write the mask, transparent ones without blending it.
4. **The pass** with its settings: width, distance scale, depth threshold, light side, fog fade, colour table; the
   `--outlines` toggle (off: no mask target, no pass).
5. **Coverage**: viewport cameras (a frame buffer, or documented as unoutlined), the overlay camera, xr, web
   downscale.
6. **Tests** (screenshots and pixel checks):
   - `outline_basic`: pixels just outside a cube are the outline colour, further out the background, off = no ring;
   - `outline_transparent`: a slime's ring is there, its inside shows the background, not a line;
   - `outline_highlight`: changing a model's colour entry changes only its ring;
   - `outline_distance`: a far model's line is thinner than a near one's;
   - each on gl and `--vulkan`.
7. **Perf** at real scale (seed 666, release, 2 runs, 1% lows and frames over 11ms), on and off, and in xr.

## Prototype (step 1, done)

Was tst outline_views before the pass (commit e8f4e24d1): the camera's colour and depth read back, outlined on the
cpu as the pass would; images none, uniform (2px black silhouettes), ink, ink_black, ink_bold, highlight. The test
now draws the real pass, on then off (tst/outline_views/on.ppm, off.ppm).

- Ink: width from distance (2.5px for the front character at 1200x900, 0.8 to 4px), times 0.45 to 1 by the depth gap
  (4 to 24 model voxels), 1.35 times thicker away from the light, alpha 0.35 to 1 by the gap, a smoothstep over the
  edge's last pixel. Lines a dark shade (0.3) of the model pixel they come from.
- What it showed: inner edges (arms over the body, one character over another) and thinner far lines work from depth
  alone; the slime's shell, writing no depth, is found by the mask (here: not the background colour), its ring
  complete, and a highlight colour per model is just a lookup. ink_bold reads best at this size.
- The slime's solid core gets a faint inner line through its shell (the shell's pixels take the slime's middle as
  their depth): the real mask would give the shell its own depth or id, decide if the core should show a line.

## The setting

- **Camera Outlines**, a toggle on the options menu's Video tab (`SettingTabVideo`), saved like the others; launch
  args `--outlines` / `--nooutlines` set it (args win over the file, as `--tiny` does).
- **Unsupported, greyed out**: where the pass can't run (a backend without it yet, i.e. vulkan until its step), the
  toggle shows greyed, can't be toggled, and its tooltip says "Not supported on this device". A general
  `SettingUnsupported` tag on a setting, so any setting can use it (the options menu greys any so tagged).
- Off (or unsupported): no mask target, no pass, nothing drawn differently.

## Decisions (2026-11-26)

- Style: ink_bold (the prototype's): width by distance, depth gap and light side, a dark shade of the model's colour.
- Models only, not terrain (a later style setting can add it).
- Viewport cameras (split screen, model_views): no outlines at first, they have no frame buffer.
- Perf measured last, on and off (Perf below).


## Perf (2026-11-26, 780M, release, --gpu, seed 666, far 8)

| Run | perf_idle fps avg / 1% low | frames over 11.1ms | perf_stream fps avg / 1% low | over 11.1ms | step settle avg |
|---|---|---|---|---|---|
| before outlines (588df0bbb) | 196.3 / 132.3, 197.2 / 127.3 | 1, 1 | | | |
| outlines on | 389.4 / 288.3, 386.1 / 277.1 | 0, 0 | 193.5 / 82.4 | 14 | 0.85s |
| outlines off | 196.1 / 137.8, 199.0 / 146.5 | 1, 0 | 87.6 / 39.7 | 404 | 1.90s |

- Not the pass' cost: the gpu's clock. On `auto` (power_dpm_force_performance_level) the 780M stayed at its lowest
  clock, 800MHz, all through an off run; with the pass' extra work it went up to 1516MHz sometimes, and the whole
  frame (and streaming, which has a frame budget) ran twice as fast. Before outlines is the same as off.
- So the game runs at the gpu's lowest clock most of the time, half its frame rate: worth more than outlines (vr
  lows). To measure the pass itself, pin the clock (`echo high | sudo tee
  /sys/class/drm/card0/device/power_dpm_force_performance_level`, `auto` after) and run perf_idle on and off again.
