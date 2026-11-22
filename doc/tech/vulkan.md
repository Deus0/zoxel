# Vulkan

A handover: what the vulkan backend is, how it works, how to test it, and what's left. The plan and decisions it
was built by: [plans/vulkan.md](../plans/vulkan.md); numbers: [metrics.md](../engine/metrics.md).

## What it is

The renderer's ~130 `zox_gpu_*` calls (gl style: bind a program, a buffer, set an attribute, a uniform, draw), on
vulkan. Render systems don't change: `src/graphics/vulkan` keeps the state they set and turns it into vulkan work.
Everything the opengl build draws, it draws: terrain, models, instances, skeletons, sky, particles, lines, ui, text,
render textures (post processing, lids, tint), and compute (gpu meshing). The backends' screenshots match
(`bsh/compare_backends.sh`). At far 32 it's +37% fps over gl on a 780M, at far 8 5% behind.

## Building and running

```bash
bash bsh/build.sh zoxel --vulkan --release --out zoxel_vulkan        # bin/zoxel_vulkan/zoxel_vulkan.bin
bash bsh/android.sh zoxel --vulkan --debug --logs --install          # android 7 (api 24) up, --xr too (XR_KHR_vulkan_enable2)
bash bsh/tst.sh <test> --vulkan [--gpu | --visible]                  # bin/tst_vulkan, lavapipe by default
bash bsh/compare_backends.sh                                         # every material on both backends, real gpu
```

- Needs `vulkan-headers` and shaderc to build; lavapipe (`vulkan-swrast`) for software tests,
  `vulkan-validation-layers` for debug builds (on when installed: errors fail tests).
- CI builds linux x64 and arm zips (shaderc beside the binary) and an android apk; the nightly has them.
- **Test on the real gpu too** (`--gpu`): lavapipe hid two radv bugs (undefined alpha, precision of mediump would too).
- Release builds strip `zox_log`: the startup lines (device, `vulkan points`, `vulkan meshes`) need a `--logs` build.

## How it works (src/graphics/vulkan)

| file | holds |
|---|---|
| `set/_.c`, `dat/_.c` | globals and structs: device, target, frames, tables, a context per thread |
| `instance.c` `device.c` | instance (validation in debug), device (largePoints, wideLines), memory helpers |
| `target.c` | swapchain or offscreen image (`--background`), depth, passes; resize, rotation (upright, android), surface lost and found |
| `frame.c` | a frame: opens at the first draw, `app_gpu_end_system` submits and presents; clears |
| `read.c` | reading the frame back (screenshots, tests), the frame carries on after |
| `table.c` `garbage.c` | gl style ids; serials: a resource the gpu still reads is freed when that submission's done |
| `slabs.c` | buffers carved from 4MB blocks (no allocation per mesh); gpu only blocks where memory isn't host visible |
| `ring.c` | per frame: uniforms and client arrays copied in, descriptor pools, upload commands |
| `shaders.c` | gl source to spir-v (shaderc): `#version 450`, loose uniforms gathered (`zox_uniforms`), vertex depth remapped, rgb outputs written with alpha 1; reflection |
| `programs.c` | linking: varyings and bindings patched by name; uniforms written per context |
| `buffers.c` `textures.c` `ubo.c` `state.c` | gl's buffer, attribute, texture, uniform buffer and switch calls |
| `draw.c` | a draw: pipeline from the state (cached per program), descriptor sets, the draw |
| `workers.c` | `RenderParallel` systems recorded in parts on 8 threads (secondary command buffers) |
| `targets.c` | render textures (fbos, rbos), drawn unflipped (gl's rows) |
| `compute.c` | compute programs, storage buffers, dispatch in gl's order, read back, indirect draws |
| `tst/` | `vulkan_clear`, `vulkan_shaders` |

Gl and vulkan differences and how they're met: y down (a negative viewport; render textures unflipped), depth 0 to 1
(each vertex shader wrapped), loose uniforms (gathered), per stage locations and bindings (spir-v patched),
`out vec3` (alpha 1), immediate draws (recorded, a frame's pass paused for read backs, splits, compute).

## Tests

`vulkan_clear`, `vulkan_shaders`, `sdl_vulkan_surface` (`--visible`), `particles_render`, `instance_views`,
`perf_idle`, and the whole render set runs on `--vulkan`: model_views, sky_clouds, underwater, lines_render,
body_views, terrain_render, skeleton_render, gpu_restore(_game), gpu_mesh(_views), gpu_mesh_lights, gpu_light_views.

## Left to do

- **On an android phone** (untried, no device here): rotation (upright swapchain), coming back from the background
  (the surface made again), particle size (`vulkan points: large [0]` would mean points are 1 pixel: draw them as
  quads there). A debug apk with `--logs` shows those lines in `adb logcat`.
- **A rare release crash**: once in 5 perf_idle runs at far 8 with `ZOX_VK_WORKERS=0`, never in debug (doc/todo.md).
- **XR on vulkan**: openxr has it (`XR_KHR_vulkan_enable2`), our xr code is gl es (egl, gl swapchain images).
- **Gpu meshing** (another session): compute works on vulkan as on gl; its game feature is doc/plans/gpu_meshing.md.
- Speed ideas not taken: render systems in the pipeline looping cameras (option A, its own plan), push constants for
  per draw matrices, skipping pipeline lookups when the state's unchanged (~1%).
