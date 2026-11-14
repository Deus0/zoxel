# Lighting: Sky and Block Light, Suns, Day and Night

A technical plan, for review before code. It changes the light octrees, the flood passes and the mesh colours.

## Why

- Light is one value per voxel (`LightNode`, 16 darklight to 255 sunlight). The sun is baked into it.
- A day and night cycle on one value relights every chunk as the sun moves: the fps drop at sunset.
- Glow stones and torches would dim with the night too.
- Alien worlds want more than one sun.

## The plan: two light values, suns in the shader

The usual voxel engine split (Minecraft and most since). Two values per voxel, neither changes with the time of day:

| Value | Means | Flooded from |
|---|---|---|
| Sky light | How much open sky reaches it | The top of the world down, then sideways (as sunlight is now) |
| Block light | Light from emissive blocks | Each glow stone, torch or lava, out through air |

The time of day only exists when drawing:

- Each sun is data on the realm: intensity (0 to 1 over its day), colour, direction, its own day length. Moons are
  suns too, dim and cool.
- Once a frame, the suns sum into one sky tint: `sky = Σ sun.intensity * sun.colour`, a uniform.
- The vertex shader: `light = max(color.a * sky, color.rgb)`, and per sun a face shade `max(0, dot(normal, sun.direction))`.
- Nothing relights as suns move. Torches and glow stones stay bright at night.

## Getting it to the gpu: no new buffers

Mesh light is already a per vertex colour (`MeshColorRGBs`), today r = g = b. It becomes rgba (one byte more a vertex):

| Channel | Carries |
|---|---|
| a | Sky light at the vertex (smooth lighting averages it, as now) |
| r g b | Block light at the vertex, its colour (mana circuits glow blue), smooth too |

The buffers, their memory and their uploads stay the same. Each frame, one uniform (the sky tint) changes.

## What changes

1. **Light octrees:** `LightNode` becomes `SkyLightNode` and `BlockLightNode` (a byte a voxel, rgb later).
   - It could be one octree of byte pairs instead. Two octrees keep every octree function as it is.
2. **Flooding:** the sunlight pass (`sunlight.c`) writes sky light only.
   - The flood, dark and edit passes (`flood.c`, `dark.c`, `editing.c`) run for each value.
   - Sources: sky light from the top, block light from blocks with a `BlockLight` value.
3. **Emissive blocks:** `BlockLight` (0 to 255) on a realm block, glow stone first.
   - Placing one queues a block light source, the existing flood spreads it.
   - Removing one darkens through the dark pass.
4. **Mesh colours:** the smooth and flat builders write sky light to r and block light to g.
   - The corner rule (a corner behind two solids is dark) and the lod leaf max apply to both.
5. **Shaders:** the terrain, vox and skeleton shaders take the sky tint uniform and do the max.
   - Minivoxes and characters (`Brightness`) sample both values and get the same treatment.
6. **Suns:** a `Sun` entity per sun, children of the realm: intensity curve, colour, direction, day length.
   - A small system sums them into the sky tint each frame.
   - The sky colour follows the same sum, the fog too.
   - One sun is the default realm. Alien realms spawn two or three from their seed.

## Order, each step working and tested

1. Glow stone on the current single value (block light floods into `LightNode`), to prove emissive sources.
2. Split `LightNode` into sky and block octrees. All tests pass with the sky tint fixed at 1 (looks the same).
3. Mesh colours: r and g, and the shader max.
4. Suns and the time of day: the tint moves. Sunset costs nothing.
5. Coloured suns, more than one sun, moons, face shading by sun direction.
6. Coloured block light (rgb, a separate job): each colour floods on its own, mana circuits glow blue.

## Tests

- `smooth_lights` and `lod_lights` for both values.
- `glow_light`: a glow stone in a dark cave lights its neighbours, falling off a step a voxel, gone when removed.
- `day_night`: the sky tint at noon and midnight, and two suns summing.
- No chunk relights while the time moves (count light flood passes over a minute of game time).
- `perf_stream` and `perf_fly` don't slow.

## Cost

- Memory: one more byte a voxel where light is stored (light octrees are sparse, mostly closed nodes).
- Cpu: the flood runs twice where it ran once, but only on edits and generation, never as time passes.
- Gpu: one uniform, a max and a multiply per vertex.

## Open questions

- Shadows from a sun's direction (not just face shading) need gpu lighting: long term, with vulkan.
- Coloured block light floods three values (or one value and a colour): cost to measure.
- Should the sky light cap be lower underwater?
