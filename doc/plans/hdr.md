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
- tst glow_views and lamp_views (`bash bsh/tools/view_glow.sh [--lamp]`): a glow stone in a brick room, a street lamp
  in a stone cave, through the game camera, as clamp, bloom, soft and filmic with bloom (doc/engine/tools.md).
- Not measured on the 780M yet (llvmpipe's times say nothing): the budget is 0.5ms at 1080p.
- Limits: box filtered mips can look blocky and shimmer as the camera moves. The fix, if it shows: a chain of
  targets, the dual filter / COD style (13 tap down, 9 tap tent up), a bright pass with a soft knee.

### A3.1 Glow (built, 2026-11-27)

The user's design: a value carried with the data, not a material of its own (a glow material with its own mesh was
tried and dropped: one brightness for all, no glowing liquids, a mesh and a system a kind).

- **Mesh colours are rgba** (`MeshColors`, was `MeshColorRGBs`): rgb as before, the alpha a vertex's glow (0 none).
  Every cpu mesher writes it (terrain, vox models, skeletons, poly terrain); uploads 4 bytes a vertex; vulkan's rgb8
  fallback is gone. Not see through: a slime's shell is its own mesh with an `alpha` uniform.
- **Shaders**: `GLOW` 3: terrain (textured3D) `(glow > 0 ? 1 : light) * (1 + glow * 3)`, a glowing face lit at
  night too; vox models, instances and bones `colour * (1 + glow * 3)`.
- **Blocks**: `BlockGlow`, a byte a block (0 none, 255 4x white); glow stones 85 (2x: its pale stone past
  white, it blooms, its cracks show): at 255 the whole face clipped to flat white, its stone texture lost. The cpu's
  smooth lights write it into its faces' alpha (`get_block_glows`, a voxel value's table); the gpu's textured draw
  looks its voxel (in the vertex's w) up in a `glows` uniform set from the same table, the compute mesher unchanged.
- tst glow_view (gl: cpu meshes, vulkan: gpu meshes): a glow stone's middle past white (255 255 253); without glow
  (228 212 142).
- **Vox palettes** (built, 2026-11-27): `ColorGlows`, a glow byte a palette colour (doc/tech/rendering.md); the
  street lamp's glass 255. tst lamp_views: its glass past white on gl and vulkan, ~200 blue with no glow.
- **Particles** (built, 2026-11-27): a particle's `Brightness` (1 default) multiplies its rgb in particle3D.vert, its
  alpha still its see through and fade; an emitter's `Brightness` is given to what it spawns (emit and stream
  systems). Burn's flames 2. tst particle_glow (gl and vulkan): brightness 3's particles ~1.6x brightness 1's.
- Next: gravity cores (a blueprint node to set a colour's glow), combined models (characters: glowing eyes that
  animate, todo.md), fireflies round street lamps (an emitter, Brightness past 1).

### A5. Viewport cameras and xr

- **Split screen** had it already: each player's canvas has its own screen texture and pass (huds/fun/game_ui.c).
- **Viewport cameras** (built, 2026-11-27): `spawn_viewport_camera`'s views are render cameras (`CameraViewport`
  tag), each its screen texture made when its cell is set (ui/render_textures/sys/screen.c): on the game view's
  canvas in its cell, a layer over it, its own material, held by its camera (`UIHolderLink`, gone with it). So model
  views and test views get the tonemap, bloom and outlines (no vignette, blur or lids, its camera has none), the
  pass's noise, and web's `viewport_downscale`. Without post processing, a screen (no game view) or in xr, one draws
  straight to its part of the screen as before. Its texture is made on its first frame: a new view's first frame or
  two are black (tst gpu_light_views waits for it). tst hdr_viewport_bloom: hdr_bloom through one, the same numbers.
- **xr** (two eyes): not yet, its eyes draw straight to their swapchains (xr/fun/eye_render.c).

## Stage B: HDR output (vulkan first)

**Built, vulkan (2026-11-27)**, blind: this screen reports sdr, so how it looks is for an hdr screen to judge.

- **Setting `HDR`** (video tab, `--hdr` / `--nohdr`, saved): off by default; greyed (and off, whatever was saved or
  passed) on gl, in xr, where vulkan has no `VK_EXT_swapchain_colorspace`, and while the window's screen isn't in hdr
  (sdl's `SDL_PROP_WINDOW_HDR_ENABLED_BOOLEAN`, `zox_hdr_screen`): a compositor offers hdr10 to an sdr screen's window
  too. It follows the screen as the game runs (a window moved, hdr turned on or off). Switched as the game runs: the swapchain made again, the frame's two passes made
  again when its format changes, pipelines keyed by the frame's format (`vk_pipeline_key.frame_format`).
- **Swapchain** (`vulkan_pick_surface_format`): `A2B10G10R10` / `A2R10G10B10` in `HDR10_ST2084`, else
  `R16G16B16A16_SFLOAT` in `EXTENDED_SRGB_LINEAR` (scrgb, windows'), else sdr as before; `zox_hdr_output` says which.
- **The canvas** (rendering/cameras/post_processing/fun/output.c): on, the frame draws into a half float render
  texture the window's size (bound as the first camera or canvas draws, hooks in their systems: new main thread
  systems moved the frame's merges and broke tst tooltips; `zox_gpu_reset_fbo` goes back to it, `zox_gpu_default_fbo`),
  then one fullscreen triangle (`zox_gpu_render_triangles`) encodes it into the swapchain: the game's display values
  made linear (srgb's curve, carried on past 1), times paper white (203 nits), rec.709 to rec.2020, pq; scrgb linear
  over 80 nits. No other shader changed.
- **The tonemap's peak**: the pass's `peak` uniform, 1 off, the screen's headroom on (sdl's
  `SDL_PROP_WINDOW_HDR_HEADROOM_FLOAT`, read as the window's made and on `SDL_EVENT_WINDOW_HDR_STATE_CHANGED`, else 4,
  ~800 nits): clamp cuts at it, soft and filmic take the soft shoulder up to it. Render cameras' targets are half
  floats while it's on (`zox_camera_hdr`), so clamp keeps what's past 1.
- **Read back** (screenshots, tests): 10 bit's top 8 bits, scrgb's halves clamped at 1: the encoded values.
- **Tests**: `hdr_output_screen` (`--vulkan --visible --hdr` on an sdr screen: greyed and off), `hdr_output` (hdr10 asked before boot, offscreen into a 10 bit image: the cube at 3 encodes past white,
  381 to 573 of 765, pq ~800 nits), `hdr_output_switch` (switched on mid run, the same; `--visible` on the real gpu
  too), `--hdr-canvas` (the canvas over an 8 bit swapchain, passed through: hdr_bloom, model_views and others as off).
- **To check on an hdr screen**: the ui at a comfortable white (paper white 203), the sky and glow past it, no
  colour cast (the rec.2020 matrix), banding in dark areas (10 bits pq). Not yet: a paper white and peak slider.

- **B0, check first**: log every format and colour space `vulkan_pick_surface_format` gets on deusmini (GNOME 51
  on wayland; mesa exposes HDR spaces through `VK_EXT_swapchain_colorspace` with wayland colour management). Only
  `SRGB_NONLINEAR` back means B is blocked here; say so and stop.
  - **Checked 2026-11-27** (deusmini, radv phoenix, GNOME on wayland, `bash bsh/tst.sh vulkan_clear --vulkan --visible
    --verbose`): with `VK_EXT_swapchain_colorspace` on, every format (8 bit, `A2R10G10B10` / `A2B10G10R10`, 16 bit
    unorm and float) comes in sRGB, Display P3, BT.709, BT.2020 linear, **HDR10 ST2084** and pass through; not scRGB
    (extended sRGB linear). SDL's window reports hdr off (headroom 1.0): the monitor isn't in hdr mode (or can't).
    So B can be built here, `A2B10G10R10` + `HDR10_ST2084`, but only seen with an hdr screen turned on in GNOME.
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
2. A4 bloom (built, v1), then A3.1: glow stones past 1 (built), vox palette glow (built), particles' glow (built).
3. A3.2 to A3.4 as lighting and sky work land.
4. B0 check (built), then B on vulkan (built, blind: an hdr screen to judge it).
5. A5: viewport cameras (built), xr.

## Who does what

- Local AI (opencode, qwen3-coder-32k): the tonemap and bloom shader drafts from the formulas above with
  render_texture_rgb.frag as the template; the three test files from a template test ("no #include lines"); the
  B encode maths as a pure function and its test. Reviewed, fixed and built by Claude.
- Claude: the format plumbing across gl, vulkan and headless (exact signatures), the setting, restore paths.
