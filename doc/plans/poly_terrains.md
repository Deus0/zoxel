# Poly terrains: a heightmap terrain to walk on

Status: planned, not started.

The goal: a low poly terrain like a short hike's, or unity's Terrain: one grid of heights, drawn as a flat shaded,
lit mesh coloured by height and slope, and the player character walking on it with a capsule collider as its child.
Next to the voxel terrain, not replacing it. A new game, `gam/zeland`, boots into it to test it. Building it also
pays off for zoxel: the character comes off the voxel terrain, meshes get normals and light, and bodies collide
by what they link to.

## Decisions

- **Folders and modules:** `src/terrains/` with `voxel/` (today's `src/terrain`, its module renamed
  `voxel_terrains`: every `terrains.` query string becomes `voxel_terrains.`) and `poly/` (`poly_terrains`) under it.
- **The character:** zeland uses zoxel's own character (character3), with no stats for now (early stages; stats
  are added by game code, through `zox_prefab_character_set`, never by the prefab). What ties the prefab to voxels
  comes out of it: `LinkChunk`, `ChunkPosition` / `BlockPosition`, the `InsideBlock*` components, `BlockScale`,
  `RaycastVoxelData`, `TerrainGravity` and the AABB `Bounds3D`. The voxel terrain adds those to characters itself,
  the same way. Kept modular.
- **Heights:** `PolyHeights`, a float array on the `PolyTerrain` entity, for now.
- **The capsule:** a `Capsule` tag with new core float variants `RadiusFloat` and `HeightFloat`. The engine is
  mostly bytes, so `Radius` and `Height` stay bytes (mountains, regions and towns use them as whole blocks).
- **Collisions inside the terrain, data driven:** for now each terrain kind holds its own collisions, the poly ones
  in `terrains/poly/collisions` like the voxel ones in `terrain/collisions`. A body links its collider (the capsule
  child) and what it stands on (its `TerrainLink`); the systems look at those links and handle each kind: a poly
  terrain, a voxel terrain, none. No warnings for a body that's simply on another kind. Generic parts can move to
  physics later.
- **Camera:** third person in zeland. Zoxel can swap to it too later, which needs work of its own.
- **Meshes:** need normals and light in rendering, for both the terrain and mesh characters.

## Shape

### The terrain entity
- `PolyTerrain` tag, its size in cells (`int2`, e.g. 128 x 128), the size of a cell (a float, 1 metre), and
  `PolyHeights`, `(size + 1)^2` floats, row major.
- Heights from the realm's seed: `perlin_octaves` (src/_/maths/fun/noise.c), with a falloff to the edges so the
  land ends in sea or cliffs. Pure function `poly_height_generate(heights, size, seed)`.
- `poly_height_at(heights, size, cell, x, z)`: the height on the triangle under a point (not bilinear: the surface
  the mesh draws, so feet never float or sink). The mesh, collisions and spawning all use it.
- `poly_normal_at(...)`: that triangle's normal.

### The mesh
- Built once by an observer when the heights are set (core `Dirty`): the terrain doesn't change.
- Flat shaded, low poly: three vertices per triangle, so each face is one colour and one normal. 128 x 128 is 32k
  triangles, 98k vertices: fine for one mesh. Chunks and LODs later, if bigger terrains are wanted.
- Colours by height and slope: sand near the sea, grass, rock on steep faces, snow on top.
- `MeshVertices`, `MeshIndicies`, `MeshColorRGBs` like chunks3's build_colored.c, plus the new normals.

### Lit meshes (rendering)
- Today no 3D shader has normals: `colored3D` is vertex colours only, chunk light is baked into colours.
- New: a `MeshNormals` array (float3) and its gpu link, a lit coloured material (the colours times a light
  direction plus ambient), in gl and vulkan alike (their screenshots must match, doc/tech/vulkan.md).
- The terrain uses it first; mesh characters move to it after (the skeleton meshes build their normals).

### Collisions
A capsule is a segment (feet to head, along the body's up) and a radius. Against a heightfield, only the cells under
the capsule's bounds matter, their two triangles each:
- the closest point on each triangle to the capsule's segment; nearer than the radius pushes it out along the
  triangle's normal, and drops the velocity going into it
- `Grounded` when the push is up enough (the normal's y over `cos(max slope)`, 45 degrees to start); steeper faces
  slide it down, like unity's character controller
- a frame's move over the radius is swept in steps of the radius, so fast bodies don't tunnel

How it runs:
- a system on bodies with a `ColliderLink` (the capsule child, its `LocalPosition3D` offset and shape) and a
  `TerrainLink`: it reads the child, writes the body's own position, velocity and `Grounded` (no race: each body
  writes itself)
- what the terrain link points at picks the test: a `PolyTerrain` the heightfield one, a voxel terrain today's
  `terrain_collision_system`, which skips bodies on other kinds instead of warning "Terrain Invalid"
- the maths (closest points, segment to triangle, capsule against heights) as pure functions in
  `terrains/poly/collisions/fun`, with their own tests

Later, not now: capsule against capsule (npcs), against boxes, voxel terrains with capsules.

### Zeland
- `gam/zeland`, like `gam/zixel`: its `_.c` sets `game_name`, sky colours, its events in `eve/`.
- A game mode, `zox_game_mode_poly`: `game_state_terrain` spawns a poly terrain for it, not the voxel one, and the
  players' loading doesn't wait on voxel streaming. Until that works, zeland reacts to its own game state and spawns
  the terrain and character itself, like tst void_player.
- The character spawns at the middle, at `poly_height_at` plus its half height, the camera third person behind it.

## Steps

Each is one task: code, its tests, and its docs (doc/tech/poly_terrains.md, zeland in doc/gdd and the games list,
kept current as each step lands, not a step of their own).

1. [x] **zeland:** `gam/zeland` like `gam/zixel`: `game_name`, sky colours, boots into its menu, a play that starts
   a game with no voxel terrain (spawns nothing yet). Test `zeland_boot`: boots and plays with no errors.
   - `zox_game_mode_poly`: no voxel terrain spawns, the game doesn't wait on one
   - its players aren't `Player3`, so the voxel start (controllers3 game_start, game_end) skips them; zeland's own
     observer puts the camera free over the origin (right click toggles roaming) and plays
   - the game's ui skips what a player lacks: no character, no actionbar, stats or quest tracker; no voxel terrain,
     no minimap
   - build and run: `bash bsh/build.sh zeland --run`; test: `zox_tst_game=zeland bash bsh/tst.sh zeland_boot`
2. [x] **folders:** `src/terrain` to `src/terrains/voxel`, the module renamed `voxel_terrains` and its query strings,
   the docs' links. The terrain tests pass as before.
3. [x] **heights:** `src/terrains/poly`, `PolyTerrain`, `PolyHeights`, `poly_height_generate`, `poly_height_at`,
   `poly_normal_at`. Test `poly_heights`: pure, heights at grid points and mid triangle, a slope's normal.
   - `PolyCells` (int2, the grid's resolution) and core `Size2` (metres) beside the heights, settings in
     terrains/poly/set (128 cells over 128 metres, hills to 24, the sea at 2)
   - an island: stretched perlin hills, a mountain rising to the middle, a round falloff under the sea
   - `poly_color_at` (sand, grass, rock, snow) for the mesh; `bash bsh/tools/poly_terrain.sh` draws the heights
     from above (doc/engine/tools.md)
4. **terrain mesh:** prefab, spawn, the mesh observer, colours; zeland spawns it on play. Until lit meshes (step 7)
   a light direction is baked into the vertex colours, so the flat faces show. Test `poly_mesh`: vertex and index
   counts, every vertex on `poly_height_at`, a screenshot.
5. **poly terrain physics:**
   - `Capsule`, `RadiusFloat`, `HeightFloat`, the closest point functions in `terrains/poly/collisions`
   - `ColliderLink`, the body system dispatching by its terrain link, voxel collisions skipping other kinds
   - the character off voxels: the voxel components above move out of `spawn_prefab_character3` into the voxel
     terrain's prefab hook (`zox_prefab_character_set`), the capsule child goes on instead of the AABB; to check as
     it starts: what the body's generation still needs from the realm (its parts and colours)
   - tests: `capsule_maths` (pure), `poly_land` (a capsule dropped rests at height + radius, `Grounded`),
     `poly_slope` (slides on a steep face, stands on a gentle one), `poly_walk` (pushed across hills, never under
     the surface, no tunneling at speed); zoxel's character, terrain and collision tests pass as before
6. **third person camera:** the poly game mode, zeland's character on the terrain, a camera behind it. Test
   `zeland_walk`: plays, the character spawns on the ground and walks, the camera follows.
7. **lit meshes:** `MeshNormals`, the lit material, gl and vulkan; the terrain moves to it (the baked light goes),
   then mesh characters. Test: a lit mesh drawn in the menus' camera, the two backends' screenshots alike.
