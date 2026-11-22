# Lighting

Voxel light: each chunk has a light octree (`LightNode`) beside its voxels. The light is worked out on the cpu, and
meshes and block voxes are colored by it. The code is in [src/lights/lights3](../../src/lights/lights3). Water's
part is in [Water](water.md) (Light).

## Values

| Name | Value | What it is |
|---|---|---|
| `sunlight` | 255 | full sunlight, a sunbeam in air |
| `darklight` | 16 | the least light: solids, unlit air |
| `light_air_decay` | 32 | lost a block as light floods through air |
| `light_water_decay` | 24 | lost on top of that in a liquid, and a sunbeam loses it in each liquid block |
| `light_propogation_distance` | 12 | how many steps a flood may take |

(lights3/set/_.c.) Brightness is `light / 255`, the same for terrain vertex colors and block voxes.

## What stops light: solidity

A block's `BlockLightPass` sets whether light goes through it. It is cached a realm in a solidity table indexed by
`voxel - 1` (`update_light_solidity`, fun/solidity.c):

| BlockLightPass | Solidity | Light |
|---|---|---|
| 0 (none) | 1 | stops it (stone, a glow stone too) |
| 1 | 0 | passes (air, water, minivoxes) |
| `block_light_pass_open` 2 | `light_solidity_open` | stops it unless its block data is `block_data_open` (a door and the door's filler) |
| `block_light_pass_base` 3 | `light_solidity_base` | passes as its base does: the plain filler over a shut door stops it (doors from old saves), over anything else it passes |

`light_stops(solidity, datas, voxels, depth, voxel, position)` is the one rule every pass uses: flood, dark, sunbeams,
edits and block voxes. Smooth lighting and gpu meshing count `light_solidity_base` as air (it has no block of its own
to shade).

## Passes

- **Sunbeams** (sys/sunlight.c, sys/lightbeam.c): a beam goes down from the top of the world at `sunlight` and
  lights each voxel to the first solid. It is dimmed in each liquid (`light_dim`), queues a flood from each voxel it
  lights, and carries on into the chunk below. There's no marker: a beam is known by its light (`light_beam_above`
  walks up a column while each voxel's light is what a beam would give it).
- **Flood** (sys/flood.c): a breadth first fill from a `LightQueue`. Each step goes to the 6 sides, minus
  `light_flood_decay` (air, plus water in a liquid). It stops at a voxel that `light_stops`, and only writes where it
  makes a voxel brighter. A side past the chunk edge is queued on the neighbour chunk's queue.
- **Emitters** (fun/emitters.c, sys/emitters.c): a block with `BlockLight` (a glow stone) is solid, so it lights the
  air on its sides a step down. They light up when placed, when their chunk is generated or loaded
  (`GenerateEmitterLight`), and again when the dark passes them.
- **Dark** (sys/dark.c): taking light away. A dark flood clears every voxel lit dimmer than the one before it (light
  that came from there), then queues the light round its edge to flood back in. A block placed in a beam sends a dark
  beam down (`zox_light_type_beam_start`), and water placed in one dims it (`zox_light_type_dim_start`, `_dim` into
  the chunk below). An emitter the dark meets lights again.
- **Edits** (sys/editing.c, `voxel_light_system`): from a chunk's `VoxelNodeQueue`. A block removed floods its sides
  in (and the beam over it down). A block placed sends dark from it (and down its beam). An emitter placed or removed
  lights or darkens round it.
- **Openings** (fun/openings.c): a voxel that starts or stops passing light while its voxel stays the same, because
  its block data changed (a door opened or shut, `door_toggle`). Opened, its sides flood into it and on through it.
  Shut, dark floods out from it and anything lit another way fills back in.
- **Trigger** (sys/trigger.c, trigger_mesh.c): once a chunk's queues are empty and its `LightNodeDirty` is set, its
  mesh colors are rebuilt (`BuildMeshColors`), and so are its neighbours' (their edges read its lights).

## Meshes: smooth lighting

Each vertex of a terrain face is lit by the air around it, not by the face's one light (sys/build_smooth.c):

- A vertex averages four lights: the face's (the voxel the face looks into), its two edges' and its corner's.
- A corner behind two solid edges is hidden, so it counts as `darklight` (its light would leak round the corner).
- Solids hold `darklight`, so vertices in creases are darker (ambient occlusion).
- **A face looking into a door's part** (a door or its filler, `light_solidity_open` or `_base`: it stops light only
  while shut, and it's drawn as a block vox, not a cube) is lit flat. All 4 of its vertices take the darkest open
  air in the 3 x 3 across it, leaving out solids and door parts (`smooth_seal_light`, the gpu's `seal_at`). The 3 x 3
  reaches past the door's plane, so a wall's side face beside a shut door averaged the sunlit outside into its edge
  while the room was dark. This is the same rule as a shut door's own brightness (below). The cpu has no block datas
  here, so it applies to an open door too: there both sides are lit through it, and the darker is close to the other.
- Lights are read from a smooth grid: the chunk's lights and voxels with a voxel of border from its 26 neighbours,
  flattened once a mesh (fun/smooth_grid.c). A coarse light counts as its leaves' brightest.
- Gpu meshing reads the same grid in its compute shader (fun/gpu.c, sys/gpu_mesh.c), and tst gpu_mesh_lights
  compares the two.

## Block voxes (minivoxes, doors)

A block vox has one `Brightness` (vodes/vodes3/fun/lights.c, `get_block_vox_light`), set when it spawns and again when
its chunk's or a neighbour's lights change (`block_vox_lights_system`):

- **It passes light** (a minivox): the brightest of its own voxel and its 6 sides. The sides cover a block that
  stops light.
- **It stops light** (a shut door, the filler over one): its own voxel holds no light, so it takes the **darkest of
  its sides that pass light**. Solid sides (the wall round a door) are left out, since they always hold `darklight`.
  Lit by its brightest side, a shut door between a sunlit outside and a dark room glowed inside the room. Lit by its
  darker side, it matches the room (tst door_light), but it is dim seen from outside. Lighting each face from the
  side it faces would fix both, and needs a light per face in its mesh.
- A side past the chunk edge is read from the neighbour chunk, and with no neighbour chunk that side is skipped.

## Tests

| Test | Checks |
|---|---|
| `door_light` | light through a door only while it's open; the shut door's brightness is its dark side's |
| `door_sky`, `door_old_filler`, `door_save` | sunlight and doors, old saves' plain fillers, a door saved shut |
| `smooth_lights` | the corner leak; a wall's face into a shut door takes the room's light, not the outside's |
| `gpu_mesh_lights` | the gpu's corner lights against the cpu's |
| `block_vox_light_sampling`, `block_vox_lights` | block vox brightness from its sides; brightness matches the lights round it |
| `glow_relight`, `dig_light`, `water_light` | emitters lit again after the dark, digging, water dimming beams |
