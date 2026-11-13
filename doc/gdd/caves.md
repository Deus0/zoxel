# Caves

Underground spaces under the terrain, explored with the player's light, a home for monsters and ores.

- Add connection points on region edge, with other edge, it uses a perlin combo of both positions to make joint positions the same
- create all tunnels from these so it feels like an endless liminal zone

## Goals
- Caves feel varied: big open caverns, long winding tunnels, narrow cracks, and they connect.
- Deterministic from the realm seed, chunks generate independently and in any order.
- Cheap enough to generate while streaming (chunks generate on worker threads).
- Tunnels sometimes reach the surface (cave mouths).

## Approaches

### Perlin worms
- A worm walks from a start point, its direction steered by noise, carving a sphere at each step.
- Long natural tunnels. Con: a worm crosses many chunks, so a chunk can't carve alone, worms must be planned first; looks samey alone.
- Pros: good for long tunnels and cave mouths.

### 3D noise "cheese" caves
- Carve where 3D noise is above a threshold. Big caverns and pillars.
- Per voxel, chunk local, easy.
- Con: blobby, isolated pockets.

### 3D noise "spaghetti" tunnels
- Carve where two 3D noises are both near zero (their zero surfaces intersect in lines).
- Long thin tunnels that are chunk local.
- Con: tunnel width hard to control, can look noisy.

### Cellular automata
- Random fill then smooth a few steps. Organic rooms.
- Needs neighbours, iterative, not chunk local.
- Con: not suitable for independent chunk carving.

### Worley (cellular) noise
- Tunnels along the edges between cells. Connected networks.
- Con: grid-like if not warped.

## Recommendation
- Mix: cheese for caverns + spaghetti for local tunnels (both chunk local, per voxel) + perlin worms for long tunnels and cave mouths.
- Worms reuse the region placement this engine has: a region plans its worms (start points with regions/fun/place.c, paths as line segments with radii, from the seed), each chunk carves the segments that cross it, like towns draw into tunks (regions/fun/tunk.c).
- Carve after landfill (terrain generation), before vegetation and blocks spawn, so caves cut terrain only.
- Depth rules: caverns deeper, spaghetti mid, worms can rise to the surface.
- Biome climate (temperature, moisture) can change caves: wet caves have water, hot ones lava.

## Data
- CaveWorm entity per worm, children of its region: segments (start, end, radius) in an array component.
- Settings: cave density, cavern threshold, tunnel width, worm count per region, max depth.

## Generation steps
1. Region spawns worms (seeded).
2. Chunk landfill.
3. Chunk carves cheese and spaghetti per voxel (3D noise at the voxel's world position).
4. Chunk carves worm segments overlapping its bounds (sphere or capsule distance).
5. Octree optimise.
6. Lights (caves dark).

## What the engine has

- 2D noise only: perlin_noise, perlin_octaves (2D), simplex_fun_2D (src/_/maths/fun/noise.c), caves need a 3D
  noise first (3D simplex or perlin, seeded, octaves), with a test that it's continuous and in range
- region placement (regions/fun/place.c) for worm starts, and the tunk pattern (regions/fun/tunk.c) for drawing
  a placed thing into the chunks it crosses
- octree chunks with locks for writers (doc/thread_safety.md), carving is an octree write

## Performance
- 3D noise per voxel is the cost: sample a coarse grid (every 4 voxels) and interpolate, skip chunks above the surface, skip air.
- Worm segments: test a chunk's bounds against each segment's box first.
- Lower lods: carve at lower octree depth.

## Tests
- cave_carve: carving is deterministic per seed, chunks carved alone match chunks carved with neighbours (no seams).
- cave_worms: worms stay connected across chunk borders (a voxel path along each worm is air).
- cave_mouths: some worms reach the surface.
- perf: carving a chunk under a budget.

## Open questions
- Cave lighting (the player's light, glowing ores).
- Monsters spawning in caves only.
- How caves show on the map.

## Status

- design only (2026-11-13), see doc/todo.md, first step: a 3D noise function and its test
