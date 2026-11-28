# Day and Night

A plan, for review before code. The sun moves over a realm's day, the sky, fog, terrain, models and characters follow
it. Nothing is relit as time passes: the time of day is a tint the shaders take.

## Decided (2026-11-27)

- A day is 20 minutes of the realm's time: it stops when the game pauses (the realm clock, doc/plans/realm_clock.md).
- One sun now. Moons later, their light on some nights of the month by the moon's phase: after coloured lights.
- Characters are lit now (by the light octree and the tint). Minivoxes already sample the light octree.
- Order: the clock and the tint first, then whether to split the light (below), then coloured lights, then moons.

## The tint

Done (2026-11-27):

- `Sun`: a light source entity (src/sky: its components, prefab and `spawn_sun`), a child of its realm. Realm data
  is spawned by each game: zoxel spawns its sun in the realm's solar generation step (`zox_generate_realm_solar`,
  gam/zoxel/sys/solar.c; later its planets and their orbits there too). `DayLength` (1200s), `DayStart` (0.3, the
  morning), `ColorRGB` (high up), `SecondaryColorRGB` (its sunset: zoxel's from the realm's seed, most warm, some
  alien). A game without suns has full daylight.
- Its time of day is the realm's `ClockTime` over its day length from its start: 0 midnight, 0.5 noon. Nothing
  stores it, and it holds while the realm's paused.
- `camera_sky_system` (src/sky/sys/camera_sky.c): each camera's realm (its streamer's terrain's) sums its suns into
  the camera's `SkyTint` (at least `sky_night_tint`, a dark blue) and `SunDirection` (the brightest's). Warm and low
  at sunrise and sunset, none under the horizon.
- The camera's view block (doc/howto/shaders.md) carries them: terrain (cpu and gpu meshes), vox models, minivoxes,
  skeletons, clouds, lit meshes and the sky take it; the fog colour is darkened by it. Glowing colours keep their
  light, the 3D uis are unlit (`textured3D_unlit.vert`).
- tst `day_night`: a realm's one sun, noon, dawn and midnight light and the sun's direction, the time held paused.

Until light is split, a glow stone's light on the air round it dims at night with everything else (the glow stone
itself still glows).

## The sky and fog

Done (2026-11-27), res/shaders/skybox.frag and view.glsl (doc/howto/shaders.md, Fog):

- By direction (`normalize(mesh_pos)`): the biome's colours from the horizon up, lighter overhead, no cube corners.
- The sun a square (blocky, as the world) moving across the sky, a glow round it, the horizon glowing towards it at
  sunrise and sunset. Its colour low down is the realm's sunset colour, from its seed (most warm, some alien), as
  its biomes' colours are; the camera's `SunColor`.
- Stars at night. The moon later (with moons).
- Fog by direction and height, its colour the sky's horizon (with the sun's glow), so far terrain melts into it.
- tst `sky_day`: dawn, noon, dusk, midnight and the dusk horizon, screenshots looking at the sun.
- Later: it follows the realm's rotation if worlds rotate (the sun's direction is the realm's, not the camera's);
  `weathers/sys/day_night_system.c` is an empty file to remove.

## Characters

Done (2026-11-28): `character_light_system` (characters3/sys/light.c) samples its chunk's light octree at its head
(`chunk_light_at`, lights3/fun/sample.c, under the chunk's `LightLock`), its `Brightness` easing to it; the
skeleton render draws each mesh with its character's `Brightness`, and the sky's light on top (bone.vert).
tst `character_light`: bright in the open, dark under the ground, bright again.

## Splitting light: open, measure first

With one light value, emitter light can't be told from sunlight, so torches dim at night. The split (doc/plans/lighting.md)
gives sky light and block light their own octrees, the shader takes `max(sky * tint, block)`.

The worry is the flood's cost. What doubles and what doesn't:

| Pass | Now | Split |
|---|---|---|
| Sunbeams and their flood (most of the work) | once | once, the same |
| Emitter floods | into the one octree, stopping where sunlight is brighter | into their own octree, the full 12 steps even in daylight |
| Dark passes (edits) | once | once a value: about twice, edits only |
| Chunks without emitters | - | no block pass, their block octree one closed node |
| Mesh colours | one octree read a corner | two |

So generation should cost about the same, edits more.

### Measured (2026-11-28)

`perf_stream --release`, seed 1234, with and without `--noemitters` (its lights' metrics: each light system's time,
flood and dark steps, emitter sides lit; zox_tst_profile_log_matching):

| | emitters on | emitters off |
|---|---|---|
| flood steps, the first load | 1,297,355 | 1,290,521 |
| flood steps, 6 steps streaming | 1,376,824 | 1,386,146 |
| all light systems (870 frames) | 1693ms | 1716ms |
| light_flood_system | 385ms | 378ms |
| build_smooth_lights_system (mesh light colours) | 1075ms | 1092ms |
| dark steps | 0 | 0 |

- Emitters are under 1% of the flooding now (noise): 312 emitter sides lit (about 50 lamps and glow stones), most
  into sunlit air that's already brighter, so their floods stop at once.
- Split, each emitter floods its full reach into its own octree: about 1,400 voxels an emitter (255 losing 32 a step
  is 7 steps out), 50 of them about 70k steps: about 3% more flooding.
- Dark steps only come from edits (none streaming): doubled there, a few hundred steps an edit.
- The real cost is the mesh light colours (build_smooth_lights, the biggest light system): a second grid to sample.
  Only chunks with block light need it (its octree one closed node at darklight in the rest).

So: split. Light flooding grows a few percent; mesh light colours only where there's block light.

### Split, part A (2026-11-28): two trees, the look the same

- `BlockLightOctree` (a `typedef` of `LightOctree`, its own component) beside `LightOctree` (the sky's), under the
  same `LightLock`. Light and dark updates carry a `channel` (`zox_light_sky`, `zox_light_block`), so a chunk keeps
  one `LightQueue` and one `DarkQueue`, and light crossing a chunk's edge stays in its channel.
- Sunbeams and water dimming: the sky's. Emitters, their dark and their relighting: blocks'. Edits, doors, a chunk
  lit from its sides (relight.c), regenerating (render_depth.c), reducing: both.
- Readers take the brighter of the two for now (smooth grid's `block_lights`, `chunk_light_at`, block voxes,
  `chunk_light_brightest` for tests), so it looks as before. Part B: the mesh colours keep both, the shaders
  `max(sky * tint, block)`.
- Cost, perf_stream --release seed 1234, the same session: light systems 2.17ms to 2.42ms a frame (mesh light
  colours 1.41 to 1.61ms, the second grid; flooding 0.46 to 0.49ms, flood steps +3.2%). To optimise after.

### Split, part B

- CPU terrain (2026-11-28): the smooth grid keeps blocks' light apart (`block_apart`), a face's vertices both:
  r the sky's, g blocks' (build_smooth.c). textured3D.vert takes view.glsl's `zox_light(sky, block)`: the
  brighter of the sky's in its light and blocks'. Plain textured meshes are (255, 0, 0): the sky's full, no
  blocks'. tst glow_view checks blocks' light is in its tree and in the mesh colours.
- Vox models (2026-11-28): `BlockBrightness` beside `Brightness` (the sky's) on block voxes, minivox instances
  and characters; set per channel (`get_block_vox_light`'s channel, `chunk_light_at`'s). colored3D and bone take
  `brightness` and `block_brightness` in their vertex stage, vox instances the matrix's [0][3] and [1][3]; all
  `zox_lit(sky, block, glow)` (view.glsl). A lamp's light on a character or a minivox stays at night too.
- Gpu meshes (2026-11-28, vulkan's terrain): GridLights holds blocks' light after the sky's, the compute shader
  writes two uints a vertex (`light_stride` 2: the sky's corners then blocks'), the vertex array's second uint is
  location 3 (`zox_gpu_storage_mesh_vao`'s extra2_location), the draw takes `zox_light`. A lod swap's kept border
  (smooth_grid_keep_border) keeps blocks' light too. tst glow_view reads the gpu buffer back.

## Steps

1. Done: `Sun` and the time of day from the realm clock, the tint, the sky, the fog. tsts `day_night`, `sky_day`.
   The realm's clock is saved in its realm.txt (`time`) as the game ends, set again as it's continued (tsts
   realm_info, load_games).
2. Done: characters lit by the light octree and the sky. tst `character_light`.
3. Measure the split (above), decide.
4. If split: `BlockLightOctree` beside `LightOctree` (which stays, as sky light), mesh colours rgba, the shader max.
5. Coloured lights (rgb, its own task), then moons with phases, more suns for alien realms, face shading by the sun's
   direction.
