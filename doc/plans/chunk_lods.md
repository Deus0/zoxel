# Chunk Lods: the last seams

Rare holes where two chunks meet as the land changes lod around a moving player: 2 to 10 frames of ~280 walking in
tst chunk_mesh_flicker (medium terrain, cpu meshing). The lod swap work (b53bd6fac, 2026-11-23) fixed the common ones;
this is the edge case left. The current regression fails on any missing border cell.

Code: [src/chunks/chunks3/sys/transition.c](../../src/chunks/chunks3/sys/transition.c) (`chunk_mesh_transition_system`,
the swap; `chunk_mesh_hold_system`, held meshes let go), [src/chunks/chunks3/sys/sides.c](../../src/chunks/chunks3/sys/sides.c)
(`chunk_side_depth`: an unsettled neighbour is open, `chunk_neighbors_rebuild_sides`),
[src/terrain/sys/stream.c](../../src/terrain/sys/stream.c) (a chunk's lod change starts),
[src/chunks/chunks3/sys/mesh_spawn.c](../../src/chunks/chunks3/sys/mesh_spawn.c).
Test: [src/terrain/tst/chunk_mesh_flicker.c](../../src/terrain/tst/chunk_mesh_flicker.c),
[src/terrain/tst/seams.c](../../src/terrain/tst/seams.c).

## Seeing it

`bash bsh/tst.sh chunk_mesh_flicker`, then in tst/chunk_mesh_flicker.log each seam: `seam: chunk [..] material depth
voxel has no face ..`, and both chunks' last 40 changes (`zox_tst_cmf_record`: lod dirty, render depth, generating,
sides building, each material's shown meshes: d depth, B building, M uploading, X held, + built).

## What it is (2026-11-24)

Not a swap. Two neighbouring chunks, both mid lod change (ChunkLodDirty), both regenerating finer voxels, both still
showing their old mesh, held (BuildDisabled) while their chunk generates:

1. one finishes generating first: `chunk_mesh_hold_system` lets its held mesh go, it's rebuilt in place from its new
   voxels; the coarse view of finer voxels differs, a border cell solid before is air now
2. its neighbour's shown mesh, still held (or building), hides its face against that cell (built when it was solid)
3. the first uploads: a hole till the neighbour rebuilds too

A history also showed a hole that may have stood ~60 frames before it was seen: the seam check only looks at chunks
whose drawing changed this frame.

## Tried, no better (2 to 10 frames either way)

- a chunk's materials swapped in one frame (solid and water on different frames was a guess)
- a swap waiting while a neighbour's shown mesh is held
- `mesh_spawn` unsettling a chunk rebuilding its neighbours' sides (as stream.c does)
- a held mesh let go only once no neighbour's is held, or held or building: it stalls the other, the same race from
  its side (whichever uploads first shows the other's stale face)

## Plan

Windows OpenGL, seed 666, `--far 16` (2026-11-24): after rebuilding neighbouring sides on unload and ordering
GPU grid fills and swaps after their writers, `stream_seams` counted zero missing cells across 202390 settled borders.
Its previous +y report at [3x0x3] was a flowing water quad below the chunk boundary. The check now measures its
height. During the walk, `chunk_mesh_flicker` still saw seams on 15 of 341 moving frames, plus 2 frames during
loading. Its threshold passed on that run. The in-place rebuild below is still open.

- A shown mesh isn't rebuilt in place from changed voxels mid lod change: its rebuild goes to a hidden mesh that the
  swap shows, and the swap already waits for its neighbours (`chunk_neighbors_rebuilding`): no in place upload to race
- The catch: `chunk_mesh_hold_system` lets held meshes go because a held stale mesh beside a neighbour that changed
  was a hole for as long as its swap took. With the rebuild hidden, that case needs the swap to come sooner, or the
  held mesh to rebuild only its faces into changed neighbours (old voxels, new sides)
- The seam check: also check borders that didn't change this frame, every few frames, to see long standing holes
- Done when chunk_mesh_flicker sees no seam over several runs; then its check fails on any seam again, and
  stream_seams with it


## Regression coverage (2026-11-25)

CPU refinement now waits for the adjacent displayed meshes to upload their open borders before replacing
the old voxels. The waiting chunk keeps its light tree until refinement actually starts, so neighbouring
meshes can finish their colour uploads. `lod_refine_border` holds a side build pending and checks both the
wait and the subsequent refinement. During the walk, `chunk_mesh_flicker` compares CPU faces with the
border voxels captured when each displayed mesh was built. Any missing cell fails the run.

At the far stream boundary, an unloaded neighbour used to change an upward face's shading to full sunlight.
GPU meshes retain the last border light and occlusion samples until that neighbour returns. Face culling
still uses the currently loaded neighbours. `lod_lights` exercises unload and return with known light
values. `lod_light_flash` checks rapid brightening and dark-and-back flashes on individual GPU faces.
