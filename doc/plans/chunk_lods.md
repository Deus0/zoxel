# Chunk Lods: the last seams

Rare holes where two chunks meet as the land changes lod around a moving player: 2 to 10 frames of ~280 walking in
tst chunk_mesh_flicker (medium terrain, cpu meshing). The lod swap work (b53bd6fac, 2026-11-23) fixed the common ones;
this is the edge case left. tst chunk_mesh_flicker fails past 1 frame in 20 until it's fixed, then on any again.

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

- A shown mesh isn't rebuilt in place from changed voxels mid lod change: its rebuild goes to a hidden mesh that the
  swap shows, and the swap already waits for its neighbours (`chunk_neighbors_rebuilding`): no in place upload to race
- The catch: `chunk_mesh_hold_system` lets held meshes go because a held stale mesh beside a neighbour that changed
  was a hole for as long as its swap took. With the rebuild hidden, that case needs the swap to come sooner, or the
  held mesh to rebuild only its faces into changed neighbours (old voxels, new sides)
- The seam check: also check borders that didn't change this frame, every few frames, to see long standing holes
- Done when chunk_mesh_flicker sees no seam over several runs; then its check fails on any seam again, and
  stream_seams with it
