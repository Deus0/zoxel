# HDR

A technical plan, for review before code. todo: "hdr support", extra colours in the game.

## Built (2026-11-27)

- **Float target**: while a tonemap is on, a render camera's colour is RGBA16F on gl, gles 3 with
  `EXT_color_buffer_(half_)float` and vulkan (`zox_camera_hdr`, `zox_gpu_set_texture_color_hdr`;
  `set_render_texture_gpu` picks it). Clamp (the default) and gles2 keep bytes: half the memory, and clamp drops
  what's past 1 anyway. Turning a tonemap on or off remakes the render textures (`zox_set_tonemap`, their
  RenderTextureDirty). Vulkan has a texture pass pair per format (`vk_pass_texture_begin[2]`, the pipeline key's
  target 1 bytes, 2 half floats).
- **Tonemap** setting (video tab, `--tonemap <n>`, `zox_tonemap`): 0 clamp (the default, looks as before), 1 soft
  shoulder past 0.8, 2 filmic (the aces fit). `tone()` in render_texture_rgb/rgba.frag, after outlines and blur,
  before noise, under water, vignette and lids. Greyed where there's no float target.
- **tst hdr_target** (gl and vulkan): a cube at brightness 1.5 and 3 tonemaps differently (it fails on a byte
  target: both clamp at 1), soft leaves colours under its knee as clamp does.
- **Where**: src/rendering/cameras/post_processing (settings, flags, outlines, tonemap, the target's format), the
  pass' material in rendering2 (doc/tech/rendering.md, Post processing). Bloom goes there too.
- Found on the way: tst options_tabs set settings while iterating the app's children; a new setting's first Dirty
  made a table and ended the iteration (a segfault). It gathers them first now.

## The user's calls (2026-11-27)

- An HDR monitor is here later to test with: write stage B's code and tests first.
- No HDR display found: the HDR toggle is greyed (SettingUnsupported), as Camera Outlines is.
- A calibration slider with two handles, as the Resident Evil games have: paper white and peak brightness, a new
  setting type (a float2 of min and max, SettingFloat2Slider) and ui element.
- The tonemap is a video setting, to compare them at work.
- Realm colour generation gets redone later for the game's look; filmic and linear colour wait for that.

## Two things called HDR

| Stage | What | Needs | Shows on |
|---|---|---|---|
| A. HDR rendering | the 3D view draws into a float target, colours can go past 1, a tonemap brings them back, bloom glows | gl 3.3, gles3/webgl2 with the float buffer extension, vulkan | every screen |
| B. HDR output | the swapchain itself is HDR (scRGB or HDR10): brighter whites, a wider gamut, the literal "extra colours" | an HDR monitor, compositor and driver; vulkan first | HDR screens only |

B builds on A: without values past 1 there's nothing for an HDR screen to show. A is the visible win for everyone.

## Where things are now

- The player's `RenderCamera` draws 3D into its `FrameBufferLink` texture, RGB8 (`set_render_texture_gpu`,
  rendering/cameras/com/frame_buffer.c, used by ui/render_textures sys initialize.c, resize.c and the gpu restore).
- That texture is drawn on the canvas by `render_texture_rgb.frag`: outlines, blur, noise, under water tint,
  vignette, eyelids, brightness. The natural place for a tonemap.
- Vulkan's render texture is `VK_FORMAT_R8G8B8A8_UNORM` (graphics/vulkan/fun/targets.c:52); the swapchain picks
  B8G8R8A8 / R8G8B8A8 UNORM, sRGB non linear (target.c `vulkan_pick_surface_format`).
- **Nothing is linear or sRGB aware**: every colour (vertex colours, textures, `Color`, fog) is already display
  encoded, and stored as bytes.
- **Nothing makes values past 1**: light is a byte (16 to 255), vertex colours 8 bit, `brightness` is 1. A float
  target alone looks identical.
- Viewport cameras (split screen, model_views) and xr draw straight to the screen, no render texture.

## Stage A: HDR rendering

### A1. Float target (setting `Camera HDR`, off costs nothing)

- **RGBA16F** on both backends. Not R11F_G11F_B10F: its 5 and 6 bit mantissas band skies and fog more than bytes
  do (a 1/64 step from 0.5 to 1), and vulkan doesn't promise blending on it. RGBA16F is a colour attachment,
  blended and sampled on every vulkan gpu, and the same on both keeps the gl / vulkan screenshots alike.
- Only while a tonemap is on (the user, 2026-11-27): it doubles the target's memory, and clamp has no use for it.
- Supported: gl 3.3 always; gles3/webgl2 with `EXT_color_buffer_float` (or `_half_float`); gles2 never. Greyed out
  when unsupported, like Camera Outlines (`zox_camera_outlines` / `zox_gpu_outlines_supported`).
- Screenshots and readback (vulkan read.c only knows rgba/bgra unorm) read the canvas, after the tonemap: check
  they stay 8 bit.

### A2. Tonemap, a setting

- In `render_texture_rgb.frag` (and _rgba), **after outlines and blur, before noise, under water, vignette and
  lids** (those are screen space looks).
- No curve is the identity up to 1 and also squeezes colours past 1: only clamp keeps today's look exactly, so it's
  the default. Soft (identity to 0.8, then `k + (1-k) * (1 - exp(-(m-k)/(1-k)))` on the brightest channel, a white
  of 1 becomes 0.93) and filmic are there to compare.
- An `exposure` uniform (1 by default) for later eye adaptation (caves dark, outside bright).
- A filmic tonemap or a linear workflow (sRGB decode on textures and vertex colours, encode at the end) is a
  separate decision (question 2): it changes every colour.

### A3. Things brighter than white

The source of values past 1, in order:

1. **Emissive blocks and models**: `brightness` past 1 on their draws (glow stones, lava, mana crystals, eyes).
2. **Block light** (doc/plans/lighting.md): block light's colour scaled past 1 near the source; the sky tint
   uniform past 1 at noon.
3. **Sky and sun**: skybox.frag's sun disc and the horizon glow at 2 to 8.
4. **Skill particles and projectiles** (particle3D.frag): fire, lightning, heals at 2 to 4.

### A4. Bloom (what makes it show on an SDR screen)

**Built, v1 (2026-11-27): the view's mips.** No new targets or passes:

- Bloom on, the camera's half float target has mips, made after each draw: gl `glGenerateMipmap`
  (`zox_gpu_fbo_mips`, render3 after the camera's draw), vulkan blits as its pass ends (`vulkan_texture_mips` in
  `zox_gpu_reset_fbo`, outside a render pass). Vulkan: a view of every level to sample, the first's to draw to
  (`target_view`), a sampler linear between mips and nearest up close (`vk_sampler_mips`, a downscaled view stays
  pixelated); unsupported without blits and linear filtering on RGBA16F (`zox_gpu_bloom_supported`).
- The pass (`bloom_light()`): mips 2 to 7 (4 to 128 pixels), 4 taps a level, each tap's part past 1 only, added by
  `zox_bloom_strength` (1.0) before the tonemap. Colours under white add nothing, so no haze (v1 mixed the whole
  blurred view in at 0.08 and hazed everything; changed 2026-11-27 with block glow). A small bright thing still
  shows: the low mips keep it past 1 near it, the far mips lose it. Off by default.
- Setting `Bloom` (video tab, `--bloom` / `--nobloom`); turning it on or off remakes the render textures.
- tst hdr_bloom: a cube at brightness 1 then 3 on a dark background, read just below it: off unchanged, on brighter
  (at brightness 1 on equals off: no haze).
- tst glow_views (`bash bsh/tools/view_glow.sh`): a glow stone in a dark brick room through the game camera, as
  clamp, bloom, soft and filmic with bloom; the pngs to look at in `tst/glow/glow_views/`.
- Not measured on the 780M yet (llvmpipe's times say nothing): the budget is 0.5ms at 1080p.
- Limits: box filtered mips can look blocky and shimmer as the camera moves. The fix, if it shows: a chain of
  targets, the dual filter / COD style (13 tap down, 9 tap tent up), a bright pass with a soft knee.

### A3.1 Glow (built, 2026-11-27)

The user's design: a value carried with the data, not a material of its own (a glow material with its own mesh was
tried and dropped: one brightness for all, no glowing liquids, a mesh and a system a kind).

- **Mesh colours are rgba** (`MeshColors`, was `MeshColorRGBs`): rgb as before, the alpha a vertex's glow (0 none).
  Every cpu mesher writes it (terrain, vox models, skeletons, poly terrain); uploads 4 bytes a vertex; vulkan's rgb8
  fallback is gone. Not see through: a slime's shell is its own mesh with an `alpha` uniform.
- **Shaders**: `GLOW` 3: terrain (textured3D) `max(light, glow) * (1 + glow * 3)`, a glowing face lit at night too;
  vox models, instances and bones `colour * (1 + glow * 3)`.
- **Blocks**: a `BlockGlow` tag (glow stones). The cpu's smooth lights write 255 into its faces' alpha
  (`get_block_glows`, a voxel value's table); the gpu's textured draw looks its voxel (in the vertex's w) up in a
  `glows` uniform set from the same table, so the compute mesher is unchanged.
- tst glow_view (gl: cpu meshes, vulkan: gpu meshes): a glow stone's middle clipped white; without the tag ~150 blue.
- Next: a glow byte per vox palette colour (the street lamp's bulb, gravity cores), particles' glow, glowing eyes
  that animate (todo.md).

### A5. Later, as outlines did

Viewport cameras (split screen, model_views), xr (two eyes), web's `viewport_downscale`.

## Stage B: HDR output (vulkan first)

- **B0, check first**: log every format and colour space `vulkan_pick_surface_format` gets on deusmini (GNOME 51
  on wayland; mesa exposes HDR spaces through `VK_EXT_swapchain_colorspace` with wayland colour management). Only
  `SRGB_NONLINEAR` back means B is blocked here; say so and stop.
- Pick `R16G16B16A16_SFLOAT` + `EXTENDED_SRGB_LINEAR` (scRGB) if offered, else `A2B10G10R10` + `HDR10_ST2084`.
- **One encode pass at the end**, not every shader changed: the canvas (3D view, ui, viewport cameras) draws into
  an 8 bit or float target as now, a last fullscreen pass encodes to the swapchain: SDR content at paper white (SDL3
  `SDL_PROP_WINDOW_HDR_HEADROOM_FLOAT` / sdr white level), the 3D view's values past 1 up to the screen's headroom.
  The tonemap's shoulder moves out to the headroom instead of 1.
- **Calibration**: the two handled slider sets paper white (the ui's and SDR content's brightness) and peak (the
  tonemap's top), shown over a test image, as console games' HDR screens.
- SDL3: `SDL_PROP_DISPLAY_HDR_ENABLED_BOOLEAN` to offer it (else the HDR toggle is greyed), `SDL_PROP_WINDOW_HDR_ENABLED_BOOLEAN` and the headroom
  each frame (it changes when HDR is toggled or the window moves screens). A setting, `--hdr` / `--nohdr`.
- gl later, if ever (EGL colour space extensions are patchy); android/xr later.

## Tests

- `hdr_target`: a camera in a void (menus camera, skybox `Disabled`), a mesh at brightness 2, read the float target:
  values past 1 kept. Ready in under a second.
- `hdr_tonemap`: a pixel below the knee comes out of the canvas unchanged, one at 4 comes out under 1 and brighter
  than one at 2.
- `hdr_bloom`: a bright pixel lights its neighbours; with bloom off they're unchanged.
- The existing gl/vulkan screenshot match, with HDR on and off: identical below the knee.
- B: a pure test of the encode maths (scRGB and PQ at given values) and a log of the picked swapchain format.

## Later: vox colours past 1 and a wider range

After realm colour generation is redone (the user, 2026-11-27):

- **Glow per palette colour**: a vox palette entry gets an intensity (a byte, 0 to 8 times), not float colours: a
  vox stays bytes, its mesh vertices carry it (rgb bytes + a glow byte, or the alpha), the shader multiplies. Eyes,
  crystals, lava blocks, magic items glow past 1 and bloom.
- **Wider range**: palettes generated in a wide gamut (Display P3 / Rec 2020 primaries) for HDR screens, mapped to
  sRGB for SDR ones in the output pass (stage B). More saturated reds and greens than sRGB can show.
- **Precision**: 10 bits a channel only if banding shows in dark caves; bytes first.

## Order, each step working and tested

1. A1 + A2: float target and the tonemap setting, gl then vulkan. Nothing looks different at clamp (built).
2. A4 bloom (built, v1), then A3.1: glow stones past 1 (built), vox palette glow, particles' glow.
3. A3.2 to A3.4 as lighting and sky work land.
4. B0 check, then B on vulkan if the driver offers it.
5. A5: viewport cameras, xr.

## Who does what

- Local AI (opencode, qwen3-coder-32k): the tonemap and bloom shader drafts from the formulas above with
  render_texture_rgb.frag as the template; the three test files from a template test ("no #include lines"); the
  B encode maths as a pure function and its test. Reviewed, fixed and built by Claude.
- Claude: the format plumbing across gl, vulkan and headless (exact signatures), the setting, restore paths.
