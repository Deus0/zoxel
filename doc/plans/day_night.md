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

So generation should cost about the same, edits more. Before deciding: count flood steps by source (beam or emitter)
and time the dark passes in `perf_stream` over a town (street lamps, glow stones), and the share of chunks with an
emitter. If emitter floods are a small share, split; if not, look at cheaper block light (a smaller reach, or block
light only near the player).

## Steps

1. Done: `Sun` and the time of day from the realm clock, the tint, the sky, the fog. tsts `day_night`, `sky_day`.
   The realm's clock is saved in its realm.txt (`time`) as the game ends, set again as it's continued (tsts
   realm_info, load_games).
2. Done: characters lit by the light octree and the sky. tst `character_light`.
3. Measure the split (above), decide.
4. If split: `BlockLightOctree` beside `LightOctree` (which stays, as sky light), mesh colours rgba, the shader max.
5. Coloured lights (rgb, its own task), then moons with phases, more suns for alien realms, face shading by the sun's
   direction.
