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

## 2026-11-20, gpu meshing on both backends (doc/plans/gpu_meshing.md)

Release builds on the 780M (`--gpu`), `zox_tst_seed=666 --stress --nouis`, one run each, `--gpu_meshing` for the gpu
columns (solid chunk meshes on the gpu, water on the cpu). Diffs: `bash bsh/compare_meshing.sh [--vulkan]`.

**Idle rendering** (`perf_idle`, was `render_fps`: frames a second standing still once streaming settles)

| | opengl cpu | opengl gpu | vulkan cpu | vulkan gpu |
|---|---|---|---|---|
| far 8: fps | 383 | 368 (-4%) | 465 | 409 (-12%) |
| far 8: slowest frame | 4.7ms | 4.6ms | 4.1ms | 16.7ms |
| far 32, vertical 4: fps | 245 | 238 (-3%) | 313 | 246 (-21%) |
| far 32, vertical 4: slowest frame | 16.4ms | 16.7ms | 15.0ms | 16.8ms |
| after the parallel gpu draw: far 8 fps, slowest | | 364, 4.3ms | | 405, 7.2ms |
| after the parallel gpu draw: far 32 fps | | | | **301** (-4%) |

**Stream remeshing** (`perf_stream`: the player steps a chunk at a time, how long until streaming, lights and meshes
settle)

| | opengl cpu | opengl gpu | vulkan cpu | vulkan gpu |
|---|---|---|---|---|
| far 8: a step settles (average, slowest) | 0.248s, 0.277s | 0.279s, 0.299s | 0.229s, 0.263s | 0.646s, 0.670s |
| far 8: slowest frame | 9.1ms | 9.0ms | 8.2ms | 7.9ms |
| far 32, vertical 4: a step settles | 2.00s, 4.01s | **1.84s**, 3.93s (-8%) | 1.93s, 3.87s | 5.81s, 6.10s |
| far 32, vertical 4: slowest frame | 9.3ms | 12.4ms | 9.3ms | 11.6ms |

| `terrain_meshing_view` diff, cpu against gpu | opengl 0.049 | vulkan 0.071 |
|---|---|---|

- Opengl: gpu meshing streams faster at far 32 (more chunks, the cpu's sides and mesh builds are the bigger cost
  there), a little slower at far 8; idle it draws 3-4% slower (a face per voxel: more triangles than the cpu's merged
  quads, merging is planned).
- Vulkan gpu streams ~3x slower: a mesh's count comes back ~3 frames after its dispatch (frames in flight, 1 on gl),
  a mesh that grows waits that again. Fix: a better first room guess, counts read a submission sooner.
- Vulkan gpu drew slower, worse with distance (-12% far 8, -21% far 32): `chunk_gpu_render_system` ran on the main
  thread, the cpu terrain's draw on 8 worker threads. Now it only reads (its programs made with the compute one, its
  vertex arrays when built, by the mesh systems) and is recorded in parts like the cpu's: far 32 -4%, the 16.7ms
  spike (its draw program compiled mid frame) gone. Far 8 is still -13%: not the threads, likely its triangles.
- Found by these runs: far lods at depth 0 never meshed on the gpu (a 27 byte grid, dcc37b440) and had no buffers on
  vulkan (c8443a1b7).

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
