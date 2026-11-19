# Vulkan

A Vulkan build of the game beside the OpenGL one: `bash bsh/build.sh zoxel --vulkan`. The renderer stays as it is;
the backend under it changes. Code: [src/graphics/vulkan](../../src/graphics/vulkan/_.c), the window's side in
[src/apps/sdl/fun/vulkan.c](../../src/apps/sdl/fun/vulkan.c).

## Decisions (the user, 2026-11-20)

- **A state tracker under `zox_gpu_*`.** The renderer drives the gpu through ~130 gl style calls (bind a program, bind
  a buffer, set an attribute, set a uniform, draw). The vulkan module implements the same calls: it keeps the state
  they set and, at a draw, finds or makes a `VkPipeline` for it (program, vertex layout, blend, depth, cull) and copies
  the program's uniforms into the frame's uniform buffer. No render system changes, all vulkan stays in its module.
- **Shaders compiled at runtime with libshaderc**, the same path as gl: the engine builds the source (version,
  defines) and compiles it at load. shaderc's relaxed vulkan rules put the shaders' loose `uniform`s into one uniform
  block, so `res/shaders` compile unchanged (33 of 35 as files, the other 2 need the engine's `zox_ubo_max` define).
  Uniform offsets and attribute locations are read from the SPIR-V.
- **Tests draw offscreen.** With `--background` there's no surface: frames draw to one `VkImage` that screenshots and
  tests read back. A window gets a swapchain.
- **System headers** (`vulkan-headers`), linked with `-lvulkan`. Tests need lavapipe (`vulkan-swrast`), debug
  builds use the validation layer when it's installed (`vulkan-validation-layers`).

## Parts

1. **Build, window, frames** (done): the build compiles and links (no gl), an sdl3 (or sdl2) vulkan window, instance,
   device, swapchain (or offscreen), depth buffer, two frames in flight, clears, read backs (screenshots), resizes.
   Not ported calls do nothing ([fun/todo.c](../../src/graphics/vulkan/fun/todo.c)). Test `vulkan_clear`.
2. **Shaders and materials** (done): shaderc, SPIR-V reflection and patching, programs, uniform setters.
3. **Draws** (done): buffers, attributes (client arrays too), the pipeline cache, the per frame ring, viewport,
   blend, depth, cull, polygon offset, lines and points, textures, the instance matrix ubos. The game draws as on gl
   (tst screenshot, `--vulkan`), without post processing.
4. **Render textures**: fbos (cameras rendering to textures, post processing), the gpu restore hooks. Until then
   the vulkan module turns post processing off at its import (`--nopost` does the same on gl).
5. **Compute**: gpu meshing (doc/plans/gpu_meshing.md); off until then, chunks mesh on the cpu.

## Gl and vulkan differences, and how each is met

- **Clip space**: vulkan's y points down and its depth is 0 to 1, gl's -1 to 1. A negative viewport height (core
  since 1.1) flips y for every shader (front faces stay counter clockwise), and each vertex shader's `main` is
  renamed and wrapped so it ends with `gl_Position.z = (gl_Position.z + gl_Position.w) * 0.5`. Both stay in the
  vulkan module; no projection changes.
- **Loose uniforms**: shaderc's relaxed rules (above).
- **`gl_VertexID` / `gl_InstanceID`**: vulkan's are `gl_VertexIndex` / `gl_InstanceIndex`, defined to them in the
  source's prefix (draws start at instance 0, so they match).
- **A frame**: gl draws as it's told; vulkan records into a command buffer. The frame opens at the first call that
  draws (`vulkan_frame_begin`) and `app_gpu_end_system` submits and presents it. A read back mid frame submits and
  waits, the frame carries on in a second render pass that loads what was drawn (`vk_pass_load`).

## How the state tracker works (fun/)

- **Tables** (`table.c`): buffers, textures, shaders, programs; a gl style id is an index, freed ones reused.
- **Serials** (`garbage.c`): each submission is numbered, a resource keeps the last one that read it. Written while
  a submission still reads it, it gets new memory (gl's orphaning) and the old is destroyed when that one is done.
- **The ring** (`ring.c`): per frame in flight, host memory for each draw's uniforms (a stage's default block,
  copied when it changed), the instance matrix ubos and client vertex arrays; and descriptor pools, reset with it.
- **Shaders** (`shaders.c`): the source's `#version` swapped for 450 and the vertex depth fix; compiled with
  relaxed rules, auto bound and located; reflected from the SPIR-V (members, offsets, strides, blocks, samplers,
  inputs, outputs and the words of their decorations).
- **Programs** (`programs.c`): each stage's locations and bindings are its own, so linking patches the SPIR-V: the
  fragment's inputs to the vertex outputs' locations by name, bindings to one plan (vertex block 0, fragment block
  1, named blocks and samplers 2 up by name). A uniform's gl location is its index, written into each stage's block.
- **Draws** (`draw.c`): the state (program, each input's format, stride and rate, topology, blend, depth, cull,
  colour write, bias) is a key; each program keeps its pipelines by key. A descriptor set is reused while its ring
  buffers and texture match. Inputs with no attribute read a zero buffer.
- **Textures** (`textures.c`): rgba (rgb widened), copied in the frame's upload commands, submitted before it.
- Vsync off picks mailbox, else immediate (`vulkan_set_vsync`, from the app's setting).

## Numbers (2026-11-20, the 780M, release, `render_fps`, no vsync, `--nopost`, seed 666)

| | opengl | vulkan |
|---|---|---|
| tiny terrain, 3 runs | 431, 437, 456 fps | 413, 409, 397 fps |
| far 8 | 417, 339 fps | 396, 390 fps |

Even: rendering is about a quarter of the main thread on both (sampled, `ZOX_SAMPLE`: gl's render systems 13% and
its swap 10%; vulkan's 18% and its present 3%), the rest is the game's systems. Vulkan's win needs the renderer to
be the bottleneck, and the tracker to do less a draw (`vulkan_draw_begin` is 8%): ways on, in order of payoff:
- record draws on worker threads (secondary command buffers per camera or per system), gl can't
- device local buffers with staging for meshes (all are host visible now, fine on an integrated gpu, slow on a
  discrete one), one allocation for many buffers (each has its own now)
- keep a draw's key and pipeline while its state doesn't change, push constants for the per draw matrix

## Running

```bash
bash bsh/build.sh zoxel --vulkan --debug --logs --out zoxel_vulkan   # bin/zoxel_vulkan/zoxel_vulkan.bin
bash bsh/tst.sh vulkan_clear --vulkan                                # lavapipe, offscreen
bash bsh/tst.sh vulkan_clear --vulkan --visible                      # the real gpu, a swapchain
zox_tst_seed=666 bash bsh/tst.sh render_fps --gpu --release --novsync --nopost [--vulkan]   # frames a second
```

- Build vulkan with `--out`, or into `bin/zoxel_*_vulkan_*`: tst.sh only takes `bin/zoxel_*_opengl_*_dev` as the
  test binary, its vulkan runs use `bin/tst_vulkan`.
- `bash bsh/tst.sh lint_builds` builds vulkan too (where the headers are): a gl call outside `src/graphics/opengl`
  doesn't link there.
