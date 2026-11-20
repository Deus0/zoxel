# Metrics

Measured numbers, newest first: rendering speed per backend and how alike the backends draw. Add a section a run
(its date, commit, machine and settings) rather than editing old ones, so changes show over time.

## How to measure

- **Speed**: `render_fps` (terrain/tst) waits for streaming to settle, then counts 8 seconds of frames standing still.
  Release builds with the tests, the real gpu, no vsync, the same seed, alternating backends:
  ```bash
  zox_extra_dflags="-Dzox_tests" bash bsh/build.sh zoxel --release --logs --out perf_gl
  zox_extra_dflags="-Dzox_tests" bash bsh/build.sh zoxel --release --logs --vulkan --out perf_vk
  cd bin/perf_vk && zox_tst_seed=666 ./perf_vk.bin --tst render_fps --autostart --far 32 --nosaves --novsync
  ```
  Compare runs by their chunks and instances drawn too (the line's `chunks [n] instances [n]`): a different scene
  isn't a comparison. Other sessions' builds and tests on the machine slow a run, note the load.
- **Alike**: `bash bsh/compare_backends.sh` runs each screenshot test on both backends on the real gpu and prints the
  table below: `diff` is how far apart the two images are after a blur (0 same, 1 opposite, over 0.08 flagged),
  `noise` each image's fine detail (a backend much noisier than the other is flagged: the vulkan sky's bug).
- Where the time goes: `ZOX_SAMPLE=<file>`, then `bash bsh/sample_report.sh <file> <binary> 1`.

## 2026-11-20, after render textures, the alpha and particle fixes (174843555 and on)

Machine: deusmini, Ryzen 9 7940HS, Radeon 780M (radv, mesa 26.2), 60GB. Release builds, `--novsync`, post processing
on (the game's default), seed 666, load average ~4 (other sessions' work).

| render_fps | opengl | vulkan |
|---|---|---|
| far 8 (~465 chunks, ~460 instances) | 275, 272, 269 (**272**) | 248, 265, 261 (**258**, -5%) |
| far 32 (~2300 chunks, ~340 instances) | 187, 160, 181 (**176**) | 244, 238, 240 (**241**, +37%) |

- Vulkan wins where draws pile up (far lods: chunks recorded on 8 threads), and is a little behind at near
  distances, where the frame is mostly the game's systems.
- Before the particle fix vulkan's far 8 was 136 to 249 a run: each frame's particle buffer copied gpu memory back.

| backends, real gpu | screenshot | diff | noise opengl | noise vulkan |
|---|---|---|---|---|
| sky_clouds | sky_clouds.ppm | 0.0155 | 0.0495 | 0.0494 |
| model_views | model_views.ppm | 0.0001 | 0.0309 | 0.0309 |
| instance_views | instance_views.ppm | 0.0001 | 0.0309 | 0.0309 |
| particles_render | particles_render.ppm | 0.0569 | 0.0812 | 0.0816 |
| lines_render | lines_render.ppm | 0.0482 | 0.0749 | 0.0744 |
| underwater | underwater.ppm | 0.0600 | 0.0726 | 0.0722 |
| screenshot | screenshot.ppm | 0.0140 | 0.0780 | 0.0784 |
| body_views | body_views.ppm | 0.0108 | 0.0637 | 0.0637 |
| font_glyphs | font_glyphs.ppm | 0.0669 | 0.0210 | 0.0215 |

- All alike. The larger diffs are the scene moving between runs (npcs, particles), and font_glyphs' backdrop, a
  texture of random colours (not seeded).

## 2026-11-20, earlier the same day (doc/plans/vulkan.md has each step)

| render_fps far 32 | opengl | vulkan |
|---|---|---|
| first draws, no post processing | 249, 255 | 236, 239 |
| slabs | 211 | 226 (a different scene) |
| recording on 8 threads, no post processing | 174, 173, 147 (165) | 242, 195, 204 (214) |
| render textures, post processing on | 183, 193, 188 (188) | 247, 219, 213 (226) |
