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

1. **Build, window, frames** (done): the build compiles and links (no gl), an sdl3 vulkan window, instance,
   device, swapchain (or offscreen), depth buffer, two frames in flight, clears, read backs (screenshots), resizes.
   Not ported calls do nothing ([fun/todo.c](../../src/graphics/vulkan/fun/todo.c)), so the game runs and shows only
   the clear color. Test `vulkan_clear`.
2. **Shaders and materials**: shaderc, SPIR-V reflection (the uniform block's members, attribute locations,
   samplers), programs, the uniform setters writing a program's block. Test: every shader in `res/shaders` compiles
   and reflects.
3. **Draws**: buffers, attribute state, the pipeline cache, the per frame uniform ring buffer, viewport and scissor,
   blend, depth, cull, polygon offset, lines and points; textures and samplers. The 2D ui first, then terrain and
   models. Tests: a triangle read back, then the screenshot tests on `--vulkan`.
4. **Uniform buffers and render textures**: the matrix ubos (instanced voxes, bones), fbos (cameras rendering to
   textures, post processing), the gpu restore hooks.
5. **Compute**: gpu meshing (doc/plans/gpu_meshing.md); off until then, chunks mesh on the cpu.

## Gl and vulkan differences, and how each is met

- **Clip space**: vulkan's y points down and its depth is 0 to 1, gl's -1 to 1. The plan: a negative viewport height
  (core since 1.1) flips y for every shader, and each vertex shader's `main` is renamed and wrapped so it ends with
  `gl_Position.z = (gl_Position.z + gl_Position.w) * 0.5`. Both stay in the vulkan module; no projection changes.
- **Loose uniforms**: shaderc's relaxed rules (above).
- **`gl_VertexID` / `gl_InstanceID`**: vulkan's are `gl_VertexIndex` / `gl_InstanceIndex`, defined to them in the
  source's prefix (draws start at instance 0, so they match).
- **A frame**: gl draws as it's told; vulkan records into a command buffer. The frame opens at the first call that
  draws (`vulkan_frame_begin`) and `app_gpu_end_system` submits and presents it. A read back mid frame submits and
  waits, the frame carries on in a second render pass that loads what was drawn (`vk_pass_load`).

## Running

```bash
bash bsh/build.sh zoxel --vulkan --debug --logs --out zoxel_vulkan   # bin/zoxel_vulkan/zoxel_vulkan.bin
bash bsh/tst.sh vulkan_clear --vulkan                                # lavapipe, offscreen
bash bsh/tst.sh vulkan_clear --vulkan --visible                      # the real gpu, a swapchain
```

- Build vulkan with `--out`, or into `bin/zoxel_*_vulkan_*`: tst.sh only takes `bin/zoxel_*_opengl_*_dev` as the
  test binary, its vulkan runs use `bin/tst_vulkan`.
- `bash bsh/tst.sh lint_builds` builds vulkan too (where the headers are): a gl call outside `src/graphics/opengl`
  doesn't link there.
