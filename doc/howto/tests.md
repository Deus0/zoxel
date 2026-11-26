# Fast Tests

How to make an automated test small and fast, so it catches an edge case every run without costing the suite.
Writing tests in general, tst.sh and every test by area: [engine/tests.md](../engine/tests.md). What a test can start
with and what each costs: [onboarding.md](../onboarding.md) (Small and fast).

## Test the function, set its data by hand

When a bug lives in one function, call that function on data you build yourself, not through a world that has to get
there on its own.

- **The example: `tst dark_last_step`** ([src/lights/tst/dark_last_step.c](../../src/lights/tst/dark_last_step.c)).
  The bug: a dark reaching a voxel on its last step darkened it without looking round it, so a glow stone beside it
  never relit (a home stayed dark once its door shut).
  - Through the game (`home_door_glow`): a town, its homes, every door opened and shut. It took ~30s and failed ~1 run
    in 3, depending on the path the dark took.
  - Through one chunk (an air chunk, a sealed room, a glow stone in its roof): 20-27s, almost all of it the chunk's
    sunlight being taken out of the room. It still didn't hit the case: the dark rarely reaches that voxel on its
    last step.
  - Set by hand: a light octree of depth 2, a glow stone's voxel, the air under it darkened as a dark's start does,
    then `dark_flood_light` with its distance spent. One frame, the same every run, and it fails on the old code.
- **Build only what the function reads.** `dark_flood_light` takes octrees, queues, a solidity and a glow table: an
  octree set with `set_VoxelNode` / `set_LightOctree`, queues made with `i_LightQueue`, tables as plain arrays. Pass
  NULL for what it handles missing (neighbour chunks, actives).
- **Put the case in directly**, the input that's rare in play: a distance of 0, a voxel at a chunk's edge, an empty
  queue, a value at its limit. Don't wait for the world to make it.
- **Check what the function does**, not what the world shows later: here, the light it queues back. The world turning
  that into light and a mesh is checked by other tests.
- **Prove it with the bug back**: put the old code back, see it fail (this one: no light queued), then restore.

## When a world is needed

- The least it needs: a void, an air chunk, the menus' camera ([onboarding.md](../onboarding.md), Small and fast).
- **Wait on what you check**, not everything settling: one voxel's light, not the whole chunk's. Waiting for a chunk's
  sunlight to settle was most of `glow_views`' and `door_glow`'s time.
- **No second edit on one voxel in a frame**: two edits queued for one voxel, the second one wins. A room's walls laid
  over its glow stone's spot left bricks there; its light at placement still lit the room once, so the test looked right
  and checked nothing.

## Tools, not tests

A test playing the whole game (towns, homes, a player walking) is slow and depends on what the world made. Keep it as
a tool to check by hand, out of `bash bsh/tst.sh all` (its exclude list in [bsh/tst.sh](../../bsh/tst.sh)), with a
minimal test for the case it found: `home_door_glow` is the tool, `dark_last_step` the test.
