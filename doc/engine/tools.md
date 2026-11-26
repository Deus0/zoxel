# Tools

Scripts in [bsh/tools](../bsh/tools) for looking into the game, not testing it: each runs the game with a flag of its
own that does one job and quits, and writes what it found in `tst/<tool>/`. They aren't tests (`tst.sh all` doesn't
run them, nothing passes or fails), but tests can call the same functions.

How a tool is made: a game flag (`--maps`) parsed by its module (`add_hook_terminal_command`) turns on a main thread
system (registered disabled, `zox_set_enabled_id`) that sets up what it needs, waits for it, writes its output and
calls `engine_end()`. The script builds, runs the dev binary with the flag (hidden: `--background --nosaves`) and turns
its `.ppm`s into `.png`s if imagemagick is there.

## maps: world generation as images

```bash
bash bsh/tools/maps.sh                 # seed 666, tunks only (~17s)
bash bsh/tools/maps.sh --seed 42       # another world
bash bsh/tools/maps.sh --chunks        # chunks too, for the top blocks map (~60s)
bash bsh/tools/maps.sh --nobuild       # the last build
```

The maps of the 3 x 3 regions round region 0, 0 (1536 x 1536 blocks with 16 block chunks), a pixel a block, x to the
right and z down from the regions' low corner, in `tst/maps/`:

| Map | Shows |
|---|---|
| `height` | the ground, dark low to light high, water blue over it |
| `biome` | a colour a biome |
| `vegetation` | dirt, grass, weeds, trees, flowers (what the tunk planted) |
| `town` | towns' maps: walls, gates, homes, roads, street lamps, highways |
| `blocks` | each column's top block's colour (`--chunks` only) |
| `lod` | each tunk's lod, dark coarse to light full: maps far from the middle are coarser |
| `overview` | heights with towns and highways over them, region borders white |

- A world of its own, no player and no game: a realm and terrain made from the menus, a stream point in the middle of
  region 0, 0, streamed far 68 (streaming is a circle, 68 reaches the 9 regions' corners).
- Tunks only by default (`zox_terrain_no_chunks`): their maps, the regions' towns and the towns' roads generate, no
  chunks, no meshes.
- The same seed draws the same images, byte for byte: compare a change's maps before and after.
- Game flags: `--maps`, `--maps-chunks`, `--maps-seed <n>`, `--maps-out <folder>` ([commands.md](commands.md)).
- Code: [towns/dbg/maps_dump.c](../src/towns/dbg/maps_dump.c).

## poly_terrain: a poly terrain's heights as an image

```bash
bash bsh/tools/poly_terrain.sh             # seed 666 (~2s after the build)
bash bsh/tools/poly_terrain.sh --seed 42   # another island
bash bsh/tools/poly_terrain.sh --nobuild   # the last zeland build
```

`tst/poly/views.png`: the island's mesh from 4 sides (yaw 45, 135, 225, 315, 100 metres out, 55 up), from tst
`poly_views` (`poly_view_1.png` to `_4`, a test build in bin/tst_poly). `tst/poly/heights.png`: the island seen from above, 4 pixels a cell, x right and z down, coloured as the mesh will be
(sand, grass, rock on steep faces, snow on top, `poly_color_at`), lit from the north west, the sea blue over it.

- The map needs no world: the heights are pure (`poly_height_generate`), written at boot and the game quits. The
  views spawn the terrain in the menus, the ui hidden, and move the menu camera round it.
- The same seed draws the same image: compare a change to the generator before and after.
- Game flags: `--poly-map`, `--poly-seed <n>`, `--poly-out <folder>` ([commands.md](commands.md)).
- Code: [terrains/poly/dbg](../../src/terrains/poly/dbg/_.c), the plan: [poly terrains](../plans/poly_terrains.md).

## Town screenshots

`bash bsh/tools/view_town.sh` captures the wall, the town interior, and the surrounding
landscape in `tst/town/aesthetic/`. Pass `--readme` to publish resized screenshots to
`doc/screenshot/` with ImageMagick.

## Particle GIF

`bash bsh/tools/view_particles.sh` records the colored bursts over the player and saves
`tst/particles/particles.gif`. The capture uses tiny, flat terrain with UI and NPCs disabled.
Pass `--readme` to resize and publish it to
`doc/gifs/particles.gif` with ImageMagick. The capture uses its own build and logs through the
test runner. `tst.sh all` runs `spawn_particles` for particle behavior coverage.

## Glow views

`bash bsh/tools/view_glow.sh` runs tst glow_views: a glow stone on the floor of a closed brick room (no sky, lit by
the stone alone) through the game's camera, four views as pngs in `tst/glow/glow_views/`: `glow_clamp`, `glow_bloom`,
`glow_soft_bloom`, `glow_filmic_bloom` (doc/plans/hdr.md). `--vulkan` for the vulkan backend (gpu meshed terrain),
`--nobuild` to rerun.

## Still in bsh, to move here

These look at the game through the test harness (they run a `*_views` test): [todo.md](../todo.md) has moving them.

| Script | Runs | Saves |
|---|---|---|
| `bsh/body.sh` | tst body_views | the player's character from 4 sides |
| `bsh/poses.sh`, `bsh/ik_poses.sh` | tst body_poses, ik_poses | its poses, for holes in its skin |
| `bsh/walk.sh`, `bsh/punch.sh` | tst walk_views, punch_views | the walk and the punch as gifs |
| `bsh/model.sh` | tst model_views | a realm model from 4 sides |
| `bsh/gpu_lights.sh`, `bsh/gpu_mesh.sh` | tst gpu_light_views, gpu_mesh_views | gpu meshing's views |
| `bsh/compare_backends.sh`, `bsh/compare_meshing.sh` | screenshot tests on both | how alike opengl and vulkan, cpu and gpu meshing draw |
| `bsh/sample_report.sh` | (a ZOX_SAMPLE file) | where slow frames go |
| `bsh/code_stats.sh` | (src) | each module's size |
| `bsh/tools/flyby.sh` | tst lod_flyby | a camera flying the terrain (`flyby.gif`), its frames' blinks and light pops, the worst as pngs; unfinished (todo), a test for now, not a tool flag |
