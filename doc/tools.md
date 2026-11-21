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

## Still in bsh, to move here

These look at the game through the test harness (they run a `*_views` test): [todo.md](todo.md) has moving them.

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
