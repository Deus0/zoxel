# Shaders

Shaders are GLSL files in [res/shaders](../../res/shaders), a `.vert` and a `.frag`. The same source builds for OpenGL,
OpenGL ES (android, web) and Vulkan (compiled to SPIR-V by shaderc, src/graphics/vulkan/fun/shaders.c).

## What's added as a shader loads

You don't write a `#version` line, a precision, or the camera's uniforms. The loader adds them, in this order, before
the source (`spawn_file_shader_at_path`, src/rendering/shd/fun/io.c):

1. `#version 300 es` (src/rendering/shd/fun/version.c)
2. `#define zox_glsl300` and `#define zox_outlines` (glsl 300 and up)
3. `precision mediump float;` (a setting, src/rendering/shd/fun/precision.c)
4. [view.glsl](../../res/shaders/view.glsl): the camera's view, then `#line 1` so errors give the source's own lines

Unity does the same with its built in variables: a shader uses them without declaring them. It's confusing the first
time (where does `fog_data` come from?), so this page is the place to look.

## The view: what every shader knows of the camera

`view.glsl` holds a uniform block, `RenderView`, and functions that read it:

| Name | What |
|---|---|
| `fog_data` | the fog's colour (xyz) and density (w): under water it's the water's |
| `zox_fog_blend(fog_level)` | how much fog covers a point, `fog_level` its depth squared (`gl_Position.z * gl_Position.z`) |

On the cpu it's `RenderView` in src/rendering/dat/camera_renderer.c. It's uploaded once a camera starts drawing
(`render_view_upload`: `camera_render3_system`, the ui camera, each xr eye), not once a draw, so render systems don't
set fog or the like themselves.

To add a value for every shader:

1. A `float4` in `RenderView` (c) and a `highp vec4` in `view.glsl`'s block, in the same place. Only `float4`s: std140
   lays them out as c does. `highp` because the vertex and fragment stages' blocks must match on gles.
2. Set it in `render_view_upload`.
3. If shaders work it out the same way, a function in `view.glsl` beside `zox_fog_blend`.

## Shared blocks

A uniform block many programs read (`RenderView`, the vox instances' `InstanceMatrices`) is shared: a module names it
and its binding point once at import, `zox_gpu_share_block("RenderView", zox_render_view_binding)`
(src/graphics/dat/shared_blocks.c). Every program is bound to them as it's made (`spawn_gpu_material_program`,
`zox_gpu_draw_program`), a gpu restore's relinked programs too, so materials don't bind blocks themselves. Its buffer
is made with `zox_gpu_ubo_initialize(buffer, binding, bytes)` and written with `zox_gpu_ubo_set`.

## Shaders in c strings

Compute shaders and the gpu mesh's draw program (src/chunks/chunks3/fun/gpu_mesh.c) are c strings. A draw program gets
the view the same way: `shader_add_view(source)` before `zox_gpu_draw_program`.
