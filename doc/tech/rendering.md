# Rendering

## Post processing

A render camera draws its 3D view into a render texture, and the canvas draws that texture through one pass
(res/shaders/render_texture_rgb.frag and _rgba.frag): outlines, blur, bloom, the tonemap, noise, under water tint,
vignette and eyelids.

- [post_processing](../../src/rendering/cameras/post_processing/_.c), a submodule of render cameras, holds it all:
  the effects' values on a camera (`CameraBlur`, `CameraVignette`, `CameraTint`, `CameraLids`, set on the camera3
  prefab by its pre), the Camera Outlines and Tonemap settings and flags, and the colour target's format (half
  floats while a tonemap or bloom is on, with mips for bloom, doc/plans/hdr.md).
- As a render texture draws, its pass reads its camera's effects (post_processing/fun/pass.c, called by rendering2's
  render_ui): nothing is copied onto the material.
- The pass' material: [rendering2](../../src/rendering/rendering2/mat/render_texture.c), a material per render
  texture.
- Tests: ui/render_textures/tst (eyelids, hdr_target), they need geometry's cube.

## Vertex colours and glow

How a vertex's colour reaches the shaders, per mesh path (2026-11-27, block glow, doc/plans/hdr.md A3.1):

| Path | Per vertex | Glow from |
|---|---|---|
| CPU terrain (and water) | rgba bytes (`MeshColors`): rgb its light, all three the same | alpha: 255 on a `BlockGlow` block's faces (`get_block_glows`) |
| CPU vox models, skeletons, poly terrain | rgba bytes: the palette colour, darkened at corners | alpha: its palette colour's `ColorGlows` byte (poly terrain 0) |
| GPU meshed vox models | one 32 bit value: r, g, b and a 4th byte | the 4th byte: the palette uniform carries `ColorGlows` there, ao keeps it, greedy merges by it |
| GPU meshed terrain | the face's 4 corner lights in a 32 bit value; the position's w packs the voxel | the draw shader looks the voxel up in a `glows` uniform table, the compute mesher unchanged |

- Alpha is glow, not see through: a slime's shell is its own mesh with an `alpha` uniform.
- Shaders: terrain `max(light, glow) * (1 + glow * 3)` (a glowing face is lit at night too), vox models, instances
  and bones `colour * (1 + glow * 3)`. Past 1 needs a render camera's half float target; bloom and the tonemap bring
  it back to the screen.
- Glow is a value carried with the data, not a material or mesh of its own: new glowing things set a value (a block
  tag, a palette colour's byte, a particle's), with no new render system.
- Uploads are 4 bytes a vertex on every backend (vulkan's rgb8 fallback is gone).
- Palette glow: `ColorGlows` (colors), a byte a palette colour beside `ColorRGBs` (empty: none), on every vox prefab;
  a generator sets it (the street lamp's glass, models/generation/furniture.c), clone_vox_system copies it to block
  voxes, the meshers read it from the palette's owner. Not yet: combined models (combine.c, characters), .vox files'
  emission, blueprints (gravity cores).
- The gpu greedy merge's flags sit in its run word (bits 30, 31), so a face's colour keeps all 32 bits.
- Tests: lights/tst glow_view (cpu meshes on gl, gpu on vulkan), terrains/voxel/tst glow_views and lamp_views (a
  street lamp's glass, `bsh/tools/view_glow.sh --lamp`).

## Lit meshes

Flat shaded colored meshes lit by a sun (2026-11-27, doc/plans/poly_terrains.md step 7): the poly terrain, zeland's
capsule.

- How a mesh draws is its `MaterialLink`. A mesh linking `material_colored3D_lit`
  ([colored_lit.c](../../src/rendering/rendering3/mat/colored_lit.c)) is lit; every other colored mesh draws with
  `colored3D` as before (no link, or a data material like a slime gel's `TransparentMaterial`), its light baked.
- One query, one system ([vox_render.c](../../src/rendering/rendering3/sys/vox_render.c)): `vox_render_meshes` reads
  the same mesh data for both, groups a table's meshes by material, then draws each group with its material bound once.
  Tags pick render systems, links pick materials: no tag marks a lit mesh, so tables stay big.
- No normals stored: a face is a triangle with its own 3 vertices, and the shader takes its normal from the screen
  (`dFdx`/`dFdy` of the world position), turned to face the camera so vulkan's flipped y can't flip it. Smooth meshes
  would need `MeshNormals`.
- Light: `ambient + sun * max(0, n . sun)`, times the colour; glow (the alpha) is added unlit. The sun is three
  settings ([light.c](../../src/rendering/set/light.c)): `render_sun_direction`, `render_sun_light`,
  `render_ambient_light`.
- Spawning one: `spawn_lit_mesh(world, parent, position, vertices, indices, colors)` (prefab `lit_mesh`), or a
  variant of the prefab with an observer building its mesh, as [capsules](../../src/geometry/capsules/_.c) does:
  `spawn_capsule_mesh(world, parent, radius, height, color)`, its shape `RadiusFloat`, `HeightFloat`, `ColorRGB`,
  built again when it's given `Dirty`.
- Tests: ui/render_textures/tst lit_mesh (gl and `--vulkan`, the same numbers), capsule_mesh, capsule_rebuild, zeland_walk,
  poly_views.

## Notes

- The 3D particle renderer caps each draw batch at 100,000 instances. Color and size uploads use that clamped count, while the position upload uses the full ECS batch count. A batch above the cap can request a position upload larger than its GPU buffer. Use the same clamped count for every attribute upload and the draw. [Renderer](../../src/particles/particles3/sys/render.c) · [Buffer allocation](../../src/particles/particles3/sha/particle.c)
