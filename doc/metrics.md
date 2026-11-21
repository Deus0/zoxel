# Metrics

Measured numbers, newest first: rendering speed per backend and how alike the backends draw. Add a section a run
(its date, commit, machine and settings) rather than editing old ones, so changes show over time.

## How to measure

- **Speed**: `perf_idle` (terrain/tst, was `render_fps`) waits for streaming to settle, then counts 8 seconds of frames
  standing still (idle rendering); `perf_stream` times chunk steps settling as the player walks (stream remeshing).
  Release builds with the tests, the real gpu, no vsync, the same seed, alternating backends:
  ```bash
  zox_extra_dflags="-Dzox_tests" bash bsh/build.sh zoxel --release --logs --out perf_gl
  zox_extra_dflags="-Dzox_tests" bash bsh/build.sh zoxel --release --logs --vulkan --out perf_vk
  cd bin/perf_vk && zox_tst_seed=666 ./perf_vk.bin --tst perf_idle --autostart --far 32 --nosaves --novsync
  ```
  Compare runs by their chunks and instances drawn too (the line's `chunks [n] instances [n]`): a different scene
  isn't a comparison. Other sessions' builds and tests on the machine slow a run, note the load.
- **Alike**: `bash bsh/compare_backends.sh` runs each screenshot test on both backends on the real gpu and prints the
  table below: `diff` is how far apart the two images are after a blur (0 same, 1 opposite, over 0.08 flagged),
  `noise` each image's fine detail (a backend much noisier than the other is flagged: the vulkan sky's bug).
- Where the time goes: `ZOX_SAMPLE=<file>`, then `bash bsh/sample_report.sh <file> <binary> 1`.

## 2026-11-21, roads: noise node highways and town waypoint graphs (against de93d4c2c, before road paths)

`perf_stream`, opengl, release with tests, `zox_tst_seed=666 --far 32 --vertical 4 --nosaves --novsync`, cpu meshing,
other sessions' work running (load average 2 to 7). Alternated, base first.

| | base (de93d4c2c) | roads |
|---|---|---|
| a step settles (2 runs) | 2.208s, 2.213s | 2.281s, 2.437s |
| fps average, 1% low (2 runs) | 117.6, 54.2; 115.1, 52.8 | 112.7, 48.4; 109.4, 52.4 |
| frame ms median (2 runs) | 7.84, 8.11 | 8.22, 8.44 |
| with `--profile --profile-ms 12`: fps average, frames over 12ms | 100.6, 385 | 100.2, 301 |
| town systems in the frames over 12ms | 0 | 2 (town_walls 1.7ms, town_map 0.5ms) |

- Within noise: the plain runs read 3-5% slower, the profiled pair level (and fewer slow frames); the town systems
  are in 2 of 301 slow frames.
- Where the time went, in tests: a town's roads 0.04 to 0.17ms (its waypoint graph, once, on a worker); a tunk's
  highways 0.04ms average, 0.06 most (the lattice nodes round it, their regions' towns). The A* version before it
  (f7819671a, not measured this way) sampled the terrain over a grid: 1 to 9ms a town, 3 to 19ms a region.

## 2026-11-20, perf_terrain: terrain alone, cpu against gpu meshing (8443663f7)

`perf_terrain`: a world of only terrain (no player, npcs, hud, particles), a streamer and a fixed camera; release on
the 780M (`--gpu`), `zox_tst_seed=666 --stress`, **two runs each** (both shown). The cleanest terrain numbers here.

| far 8 (vertical 3) | opengl cpu | opengl gpu | vulkan cpu | vulkan gpu |
|---|---|---|---|---|
| load: settled in | 0.59s, 0.56s | 0.57s, 0.59s | 0.57s, 0.56s | 0.56s, 0.57s |
| idle: fps average | 661, 662 | 614, 626 (-6%) | 852, 830 | 834, 840 |
| idle: 1% low | 418, 445 | 429, 427 | 540, 554 | 570, 572 |
| idle: frame ms median, 99th, slowest | 1.49, 1.88, 3.6 | 1.60, 2.00, 4.7 | 1.17, 1.55, 3.1 | 1.18, 1.51, 3.3 |
| stream: a step settles (average) | 0.262s, 0.243s | 0.243s, 0.257s | 0.241s, 0.241s | 0.253s, 0.257s |
| stream: fps average, 1% low | 347-359, 85-87 | 294-298 (-16%), 79-84 | 374-378, 46 | 357-362 (-4%), 48 |
| stream: frames under 30 fps | 0, 0 | 0, 0 | 1, 1 (a 62ms frame) | 1, 1 (a 62ms frame) |

| far 32, vertical 4 | opengl cpu | opengl gpu | vulkan cpu | vulkan gpu |
|---|---|---|---|---|
| load: settled in | 8.35s, 8.25s | **7.58s, 7.56s** (-9%) | 8.37s, 8.25s | **6.78s, 6.84s** (-18%) |
| idle: fps average | 390, 382 | 346, 347 (-10%) | 419, 422 | **457, 455** (+8%) |
| idle: 1% low | 272, 267 | 243, 254 | 299, 311 | 326, 322 |
| idle: frame ms median, 99th, slowest | 2.52-2.58, 3.3, 4.9-5.2 | 2.83, 3.7, 5.9-6.9 | 2.36, 2.9-3.0, 4.2-4.7 | 2.17, 2.8, 4.5-4.6 |
| stream: a step settles (average) | 2.53s, 2.70s | 2.84s, 2.48s | 2.48s, 2.62s | 2.98s, 3.14s (+20%) |
| stream: fps average, 1% low | 57, 56; 42, 41 | 52, 53; 35, 35 | 59, 58; 33, 34 | 59, 57; 35, 34 |
| stream: frames under 30 fps | 0, 0 | 0, 0 | 1, 1 (a 66ms frame) | 1, 1 (a 66ms frame) |

- Loading: gpu meshing loads a big world faster, vulkan 18%, opengl 9%; the same at far 8.
- Idle: vulkan gpu draws far 32 8% faster than cpu meshes, level at far 8; opengl gpu draws 6-10% slower (its draw
  isn't split, every chunk's indirect draw from the main thread).
- Streaming: level at far 8; at far 32 vulkan gpu settles steps 20% slower here (perf_stream, with the game around
  it, had it 12% faster: different land, the player's streamer), opengl about level; opengl gpu streams at 16% fewer
  fps at far 8.
- Every vulkan stream had one 62-66ms frame, cpu meshing too: not gpu meshing's, to look into.

## 2026-11-20, gpu meshing on both backends (doc/plans/gpu_meshing.md)

Release builds on the 780M (`--gpu`), `zox_tst_seed=666 --stress --nouis`, one run each, `--gpu_meshing` for the gpu
columns (solid chunk meshes on the gpu, water on the cpu), at 56b15125d (parallel gpu draw, grids uploaded new).
How to read them: [tests.md](tests.md) "Reading the perf numbers". Diffs: `bash bsh/compare_meshing.sh [--vulkan]`.

**Idle rendering** (`perf_idle`: 8 seconds standing still once streaming settles)

| far 8 (vertical 3) | opengl cpu | opengl gpu | vulkan cpu | vulkan gpu |
|---|---|---|---|---|
| frames in 8s | 2998 | 2907 | 3277 | 2984 |
| fps average | 375 | 363 (-3%) | 410 | 373 (-9%) |
| fps 1% low | 241 | 237 | 273 | 166 |
| frame ms median | 2.63 | 2.74 | 2.41 | 2.48 |
| frame ms 99th | 3.45 | 3.73 | 3.09 | 4.85 |
| frame ms slowest | 6.52 | 5.53 | 6.24 | 14.41 |

| far 32, vertical 4 | opengl cpu | opengl gpu | vulkan cpu | vulkan gpu |
|---|---|---|---|---|
| frames in 8s | 1969 | 1713 | 2261 | 2030 |
| fps average | 246 | 214 (-13%) | 283 | 254 (-10%) |
| fps 1% low | 180 | 160 | 180 | 155 |
| frame ms median | 4.02 | 4.65 | 3.49 | 3.71 |
| frame ms 99th | 5.07 | 5.69 | 4.93 | 5.96 |
| frame ms slowest | 6.03 | 7.34 | 7.81 | 8.69 |

**Stream remeshing** (`perf_stream`: the player steps a chunk at a time, 6 steps, every frame while it streams)

| far 8 (vertical 3) | opengl cpu | opengl gpu | vulkan cpu | vulkan gpu |
|---|---|---|---|---|
| a step settles: average (fastest, slowest) | 0.243s (0.231, 0.273) | 0.275s (0.254, 0.300) | 0.255s (0.236, 0.292) | 0.254s (0.239, 0.289) |
| frames | 162 | 167 | 164 | 163 |
| fps average, 1% low | 111, 54 | 102, 51 | 107, 52 | 107, 58 |
| frame ms median, 99th, slowest | 8.4, 18.0, 18.7 | 9.4, 18.4, 19.8 | 8.9, 18.9, 19.1 | 9.0, 17.1, 17.4 |
| frames under 60 fps, under 30 | 6, 0 | 6, 0 | 8, 0 | 4, 0 |

| far 32, vertical 4 | opengl cpu | opengl gpu | vulkan cpu | vulkan gpu |
|---|---|---|---|---|
| a step settles: average (fastest, slowest) | 2.29s (1.59, 4.29) | **1.97s** (1.32, 4.22) | 2.06s (1.51, 4.16) | 2.14s (1.27, 4.45) |
| frames | 587 | 516 | 587 | 516 |
| fps average, 1% low | 43, 25 | 44, 30 | 48, 31 | 40, 25 |
| frame ms median, 99th, slowest | 22.0, 37.2, 42.6 | 22.3, 33.1, 33.7 | 20.4, 31.3, 32.8 | 24.3, 35.4, 44.3 |
| frames under 60 fps, under 30 | 586, 30 | 515, 5 | 586, 0 | 515, 20 |

| `terrain_meshing_view` diff, cpu against gpu | opengl 0.051 | vulkan 0.034 |
|---|---|---|

**Vulkan, repeated runs** (76c5f2b3f: its spawn arrow's particles still alive during perf_idle, both lower than
above for that). Single runs swung up to ~10%: the far 8 -9%, its 14ms frame and far 32's -10% above were noise.

| vulkan | cpu meshing | gpu meshing |
|---|---|---|
| idle far 8: fps average (3 runs) | 324, 319, 337 | 320, 327, 329 |
| idle far 8: 1% low, slowest frame | 234-248, 5.1-7.6ms | 226-261, 4.9-5.9ms |
| idle far 32, vertical 4: fps average (2 runs) | 271, 275 | 272, 265 |
| idle far 32: 1% low, frame ms median | 197-211, 3.62-3.68 | 202-215, 3.67-3.76 |
| streaming far 32: a step settles (2 runs) | 2.02s, 1.98s | **1.76s, 1.74s** (-12%) |
| streaming far 32: fps average, 1% low | 48.5-49.3, 31.2-31.8 | 48.5-49.1, 32.0-32.2 |
| streaming far 32: frames under 30 fps | 0, 0 | 0, 0 |

- Streaming: gpu meshing now matches cpu on both backends at far 8, and at far 32 opengl's settles 14% sooner with
  fewer slow frames (5 under 30 fps against 30); vulkan's about matches.
- Idle: gpu meshes draw 3-13% slower (a face per voxel: more triangles than the cpu's merged quads, merging is
  planned); vulkan gpu at far 8 has a 14ms frame and a lower 1% low (166 against 273), to look into.
- Fixed on the way: vulkan streamed ~3x slower (grids written into a buffer still read: vulkan copied its old 39KB
  first, 0.18ms a mesh), far lods at depth 0 never meshed (dcc37b440, c8443a1b7), the gpu draw ran on the main thread
  only (now recorded in parts on vulkan's workers).

## 2026-11-20, after render textures, the alpha and particle fixes (174843555 and on)

Machine: deusmini, Ryzen 9 7940HS, Radeon 780M (radv, mesa 26.2), 60GB. Release builds, `--novsync`, post processing
on (the game's default), seed 666, load average ~4 (other sessions' work).

| perf_idle | opengl | vulkan |
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
| gpu_mesh_views (compute) | gpu_mesh_views.ppm | 0.0114 | 0.0629 | 0.0629 |
| gpu_light_views (compute) | gpu_light_views.ppm | 0.0004 | 0.0365 | 0.0365 |

- All alike. The larger diffs are the scene moving between runs (npcs, particles), and font_glyphs' backdrop, a
  texture of random colours (not seeded).

## 2026-11-20, earlier the same day (doc/plans/vulkan.md has each step)

| perf_idle far 32 | opengl | vulkan |
|---|---|---|
| first draws, no post processing | 249, 255 | 236, 239 |
| slabs | 211 | 226 (a different scene) |
| recording on 8 threads, no post processing | 174, 173, 147 (165) | 242, 195, 204 (214) |
| render textures, post processing on | 183, 193, 188 (188) | 247, 219, 213 (226) |
