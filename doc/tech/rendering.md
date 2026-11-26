# Rendering

## Post processing

A render camera draws its 3D view into a render texture, and the canvas draws that texture through one pass
(res/shaders/render_texture_rgb.frag and _rgba.frag): outlines, blur, the tonemap, noise, under water tint,
vignette and eyelids.

- [post_processing](../../src/rendering/cameras/post_processing/_.c), a submodule of render cameras, holds it all:
  the effects' values on a camera (`CameraBlur`, `CameraVignette`, `CameraTint`, `CameraLids`, set on the camera3
  prefab by its pre), the Camera Outlines and Tonemap settings and flags, and the colour target's format (half
  floats while a tonemap is on, doc/plans/hdr.md).
- As a render texture draws, its pass reads its camera's effects (post_processing/fun/pass.c, called by rendering2's
  render_ui): nothing is copied onto the material.
- The pass' material: [rendering2](../../src/rendering/rendering2/mat/render_texture.c), a material per render
  texture.
- Tests: ui/render_textures/tst (eyelids, hdr_target), they need geometry's cube.

## Notes

- The 3D particle renderer caps each draw batch at 100,000 instances. Color and size uploads use that clamped count, while the position upload uses the full ECS batch count. A batch above the cap can request a position upload larger than its GPU buffer. Use the same clamped count for every attribute upload and the draw. [Renderer](../../src/particles/particles3/sys/render.c) · [Buffer allocation](../../src/particles/particles3/sha/particle.c)
