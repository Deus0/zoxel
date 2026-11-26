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
  `RaycastVoxelData`, `TerrainGravity` and the AABB `Bounds3D`. Game code adds those, like stats: a helper in the
  characters module, `prefab_characters3_add_voxels(world)`, adds them to every character prefab
  (`zox_prefab_character_set`), and zoxel's game code calls it; zeland's calls a capsule helper instead. (The voxel
  terrain can't add them itself: it's imported before characters, and biomes, heights and towns need it there.)
  Kept modular.
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

Later: movement inputs apart from the move system. Players' devices and npc ai both write one move input
(a direction, run, jump) on the character, and one system moves every character from it, so players and npcs move
by the same rules. Tests don't read real devices (SDL's keyboard, mouse, pads): they're automated, so they set the
move input themselves, and a test like `zeland_walk` holds W through it, not by pushing the velocity. After step 7,
in a plan of its own: it changes how zoxel feels, so first tests record today's movement (top speeds, how fast it
gets there, turning, running, jumps), then the new system is tuned until they match, then play tested.

Later, not now: capsule against capsule (npcs), against boxes, voxel terrains with capsules.

Later: a voxel terrain collider entity, a child of the body like the capsule, holding its voxel parts (chunk
position, the block it's in, voxel raycasts, the gravity field) instead of the character prefab and
`prefab_characters3_add_voxels`. Bodies stay generic and gain a terrain's parts by the collider spawned on them; tests
spawn the collider they need, self contained, not relying on game code having added parts to every character.

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
4. [x] **terrain mesh:** prefab, spawn, the mesh observer, colours; zeland spawns it on play. Until lit meshes (step 7)
   a light direction is baked into the vertex colours, so the flat faces show. Test `poly_mesh`: vertex and index
   counts, every vertex on `poly_height_at`, a screenshot.
   - `spawn_poly_terrain(world, parent, seed)` generates the heights and adds core `Dirty`; an observer builds the
     mesh (6 vertices a cell, a sea square round it) and adds `MeshDirty`; drawn as a `VoxMesh` (colored3D)
   - the game's `GameState` observer spawns it in the realm at `zox_game_terrain_spawn` (poly games only)
   - tst `poly_views`: 4 screenshots round the island; `bash bsh/tools/poly_terrain.sh` joins them in
     tst/poly/views.png
5. [x] **poly terrain physics:**
   - `Capsule` (collisions3), core `RadiusFloat` / `HeightFloat`, `ColliderLink` (a body to its capsule child);
     `spawn_capsule_collider(world, body, radius, height, offset)` makes the child and the link
   - `terrains/poly/collisions`: closest points (segment, triangle), `poly_capsule_push` over the cells under the
     capsule; `poly_capsule_collision_system` (zoxp_physics) on bodies whose `TerrainLink` is a `PolyTerrain`:
     3 push outs, the velocity into the face dropped, `Grounded` on slopes up to 45 degrees (static friction there,
     steeper slides), swept in radius steps when a frame moves further, never under the surface
   - the voxel `terrain_collision_system` skips bodies on another kind of terrain quietly (warns on an invalid one)
   - the character off voxels: its voxel parts moved into `prefab_characters3_add_voxels` (characters3/pre), called
     by gam/zoxel; the poly terrain is a clock (its realm's), so characters on it move by it
   - a realm its game gives no stats or dialogues (zeland's) leaves its characters without them, quietly
   - tests: `capsule_maths`, `poly_land` (and _10fps, _30fps), `poly_slope`, `poly_walk` (and _10fps, _30fps),
     `poly_character` (a character3 lands on the island, no voxel parts, its clock the terrain's)
   - regression, 20 zoxel tests round characters and collisions, before and after (see doc/tests.md for times)
6. [x] **third person camera:** zeland's character (a capsule collider and a capsule mesh child,
   `geometry/fun/capsule.c`) spawns south of the peak facing it, the camera orbits it.
   - the orbit camera (`controllers3/sys/orbit.c`, `zox_camera_follow_mode_orbit`): not a child, it orbits its
     `CameraTarget`; mouse or right stick turn it, the wheel zooms, it stays over a poly terrain's ground.
     Zoxel's third person uses it too: the character walks along the camera's facing, the mouse no longer turns it
   - character systems skip what a capsule character lacks: no body to dress, no realm blocks, no actionbar, no
     fly mode, towns only on voxel terrains
   - tests: `zeland_boot` (a character with a capsule, the camera orbiting it), `zeland_walk` (walks away from the
     camera, faces that way, on the ground, followed, its mesh drawn, a screenshot); regression, the 20 zoxel tests
7. [x] **lit meshes:** flat shaded meshes lit by a sun, gl and vulkan from one shader; the baked light goes
   (doc/tech/rendering.md, Lit meshes). A `lit_mesh` prefab and `spawn_lit_mesh` (rendering3);
   the capsule mesh is a geometry submodule (`geometry/capsules`), its shape data, an observer on `Dirty` building it.
   - normals from the screen (`dFdx`/`dFdy` of the world position, as `cloud.frag` does): flat shaded meshes are a
     face per triangle, so they need no `MeshNormals`, no normal buffers, no upload change. The normal is turned to
     face the camera, so vulkan's flipped y doesn't flip it. `MeshNormals` waits for smooth meshes (skinned
     characters), if they want it
   - its own material, `colored3D_lit` (`rendering3/mat/colored_lit.c`, `colored3D_lit.vert/frag`), the same mesh
     data as `colored3D` (vertices, indices, `MeshColors`, its gpu links, `TransformMatrix`), plus uniforms: the model
     matrix, the sun, the ambient, the camera's position. `colored3D` doesn't change, chunks and block voxes keep their
     baked light
   - how a mesh draws is its `MaterialLink` (non fragmenting, tables stay big); tags pick the render system only.
     Lit meshes link `material_colored3D_lit`, nothing else marks them
   - one query, one render system for both (queries cost too): `vox_render_meshes` reads the same data for every
     mesh and groups them by material as it goes: a linked material with its own gpu program (`MaterialGPULink`) draws
     with it, the rest (no link, or a data material like a slime's gel, `TransparentMaterial`) with `colored3D` as
     now. After the loop each group draws with its material bound once, the lit uniforms set only for the lit one
   - the groups: a small fixed list of materials, each a list of the call's row indices (a call is a table)
   - light: `light = ambient + sun * max(0, n . sun_direction)`, times the colour, before the glow and the fog
   - the sun: render settings for now (`sun_direction`, `sun_color`, `ambient_color`), a sun entity once there's a
     day; zeland sets its own
   - zeland: the poly terrain and the capsule link `material_colored3D_lit`, `poly_light_at` dropped from their meshes (kept in
     the heights map tool, it's 2D), the capsule's darker front a colour of its own
   - tests: `lit_mesh` (menus' camera in a void, two quads at different angles: the face to the sun brighter than
     the turned one, both brighter than ambient; gl and `--vulkan` give the same numbers), `poly_views` redrawn and looked at,
     zeland's tests; regression: vox and chunk render tests (`vox_instance_render`, `water_light`, the outline and
     glow tests), and a perf check (seed 666, release, 2 runs) since every vox draw goes through `colored3D`
8. **shadows (later, iterated on):** the mountain's shadow on the valleys, the character's on the ground. Step 7 only shades a face by its
   angle to the sun; nothing casts. Two ways, to choose when we get there:
   - a shadow map: the lit meshes drawn from the sun into a depth texture (a render target, as post processing has),
     the lit material samples it. Casts from everything, moves with the sun and the character, both backends
   - first and cheap: the terrain's own shadow baked once at its mesh build, marching `poly_height_at` towards the
     sun from each triangle. It's static, so only for a fixed sun, and the character casts nothing
