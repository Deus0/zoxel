# Shaders

Shaders are GLSL files in [res/shaders](../../res/shaders), a `.vert` and a `.frag`. The same source builds for OpenGL,
OpenGL ES (android, web) and Vulkan (compiled to SPIR-V by shaderc, src/graphics/vulkan/fun/shaders.c).

## What's added as a shader loads

You don't write a `#version` line, a precision, or the camera's uniforms. The loader adds them, in this order, before
the source (`spawn_file_shader_at_path`, src/rendering/shd/fun/io.c):

1. `#version 300 es` (src/rendering/shd/fun/version.c)
2. `#define zox_glsl300` and `#define zox_outlines` (glsl 300 and up)
3. `precision mediump float;` (a setting, src/rendering/shd/fun/precision.c)
4. `#define zox_fragment` in a `.frag` (the view's fragment only parts: the fog reads `gl_FragCoord`)
5. [view.glsl](../../res/shaders/view.glsl): the camera's view, then `#line 1` so errors give the source's own lines

Unity does the same with its built in variables: a shader uses them without declaring them. It's confusing the first
time (where does `fog_data` come from?), so this page is the place to look.

## The view: what every shader knows of the camera

`view.glsl` holds a uniform block, `RenderView`, and functions that read it:

| Name | What |
|---|---|
| `fog_data` | the fog's colour (xyz: the sky's at the horizon, in its light) and density (w): under water the water's |
| `sky_tint` | the sky's light on the world (xyz): the realm's suns summed, at least the night's (src/sky/sys/camera_sky.c) |
| `sun_direction`, `sun_color` | towards the brightest sun, and its colour (the realm's sunset, from its seed, low down) |
| `camera_position`, `frag_viewport`, `clip_to_view` | where the camera is, and a pixel back to its point from it |
| `zox_sky_light(glow)` | the sky's light on a colour, `glow` its glow (0 to 1): glowing colours keep their own at night |
| `zox_sun_glow(direction)`, `zox_horizon(direction)` | the sky's glow round a low sun, the horizon's colour: sky and fog both |
| `zox_fog(color)` | fragment: the colour fogged, the end of every 3D fragment shader |
| `zox_fog_amount()`, `zox_fog_color()` | fragment: its parts (lines fade out to clear with them) |

## Fog

All of it is in `view.glsl`, one place to tune it (`FOG_BASE`, `FOG_HEIGHT`, `FOG_HAZE`):

- Per pixel: its point from the camera is its depth back through the view (`zox_view_offset`), so no shader passes
  a fog value from its vertex stage.
- Its colour is the sky's horizon behind it (`zox_horizon`): the biome's sky colour, and the glow towards a low sun.
  The sky shader draws the same horizon, so far terrain melts into the sky, at sunset too.
- It's thickest low down and thins higher up (height fog): hill tops and clouds fade less than valleys.
- Under water (a thick fog) it's the water's colour, no height or sun glow.

Write a fragment's output once, at the end, from locals: `frag_color = vec4(zox_fog(color), alpha);`. Writing it,
reading it back and fogging it drew nothing on vulkan (lavapipe, tst body_views).

On the cpu it's `RenderView` in src/rendering/dat/camera_renderer.c. It's uploaded once a camera starts drawing
(`render_view_upload`: `camera_render3_system`, the ui camera, each xr eye), not once a draw, so render systems don't
set fog or the like themselves.

To add a value for every shader:

1. A `float4` in `RenderView` (c) and a `mediump vec4` in `view.glsl`'s block, in the same place. Only `float4`s:
   std140 lays them out as c does. `mediump` because the vertex and fragment stages' blocks must match on gles, and
   a `highp` one in a `mediump` fragment's maths drew nothing on vulkan (lavapipe).
2. Set it in `render_view_upload` (from the camera drawing: its components, i.e. `SkyTint`).
3. If shaders work it out the same way, a function in `view.glsl` beside `zox_fog_blend`.

## Lit and unlit

A shader for things in the world takes the sky's light: multiply the colour by `zox_sky_light(glow)` where its glow is
applied (colored3D.vert, vox_instance.vert, bone.vert, textured3D.vert), or by `sky_tint.xyz` (clouds, the sky).
Things not in the world don't: the 3D uis draw with `textured3D_unlit.vert` (material_element3), the same at night.

## Shared blocks

A uniform block many programs read (`RenderView`, the vox instances' `InstanceMatrices`) is shared: a module names it
and its binding point once at import, `zox_gpu_share_block("RenderView", zox_render_view_binding)`
(src/graphics/dat/shared_blocks.c). Every program is bound to them as it's made (`spawn_gpu_material_program`,
`zox_gpu_draw_program`), a gpu restore's relinked programs too, so materials don't bind blocks themselves. Its buffer
is made with `zox_gpu_ubo_initialize(buffer, binding, bytes)` and written with `zox_gpu_ubo_set`.

## Shaders in c strings

Compute shaders and the gpu mesh's draw program (src/chunks/chunks3/fun/gpu_mesh.c) are c strings. A draw program gets
the view the same way: `shader_add_view(source)` before `zox_gpu_draw_program`.
