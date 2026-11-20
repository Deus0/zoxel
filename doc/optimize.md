# Optimizations
- Part of the [todo](todo.md), done items go in [done.md](done.md)

- Beds and doors (two blocks a side, 64 voxels, a depth more than a block): their first mesh build made frames a
  second long in a debug software test (tst block_vox_hits, 2026-11-16, npcs held 8 physics frames took 8s). Measure
  in release on the gpu (perf_fly near placed beds); if it spikes there, build their lods off the main thread or
  once a realm, as blocks' models are shared

- Frame spikes, what's left after the chunk crossing lag (48 to 145ms, now ~20ms release on gpu, done.md):
	- spawn bursts are the worst frames (merges 46ms, observers 47ms, debug): names (435 a frame, flecs indexes
	  (Identifier,Name)), PreInitialize and Initialize (each new entity moves 4 times), 3D label layout dirties
	- DataUpdate 11k and TextureDirty 5k a run, what follows the dirties
	- block decor and vox instances are ChildOf a chunk (a table per chunk), npc ui still a child, link it instead
	- empty tables: deleting them (ecs_delete_empty_tables every 5s, tried 2026-11-15) halved perf_fly's frames
	  (6000 to 2600): most are states entities pass through, deleted then made again (each new table runs ~380
	  observers, one a cached query). Fewer states (tags added and removed as events: BuildChunkSides 40k moves a
	  flight, BuildMesh 33k, MeshDirty 23k) would be the way, but those tags keep queries idle (below)
	- lod meshes as non fragmenting children stalled terrain generation (perf_stream), still ChildOf, find why
	- move other heavy systems from process counts to zox_sys_budget
	- tst tables_stream (guarded at 300 tables a step), perf_stream, perf_fly
- Where the cpu goes flying (2026-11-15, -O3 with tests, gpu, far 8, seed 666, ZOX_SAMPLE_ALL: every thread):
	- 66% in workers, 34% the main thread; ~25% of all of it is flecs walking queries (ecs_worker_next 13.5%,
	  ecs_query_iter 10.7%, flecs_query_is_trivial_cache_search 6.5% on its own: each of 16 workers walks every
	  table a system's query matched, empty ones too), ecs_has_id 6.8%, alive checks 6.4%, ecs_get_id 6%
	- octrees are not the cost: chunk_sides_system 8.2% (build_sides_dig 3.4%), flood_light 1.6%
	- ~6000 frames in 20s; frames over 25ms: 173, then 94 to 111 with bones, the combined vox and the renderer
	  child non fragmenting (tables in them 2849 to ~2000). What's left in them: app_gpu_end_system ~10ms (the
	  swap, top system in 102 of 111), merges ~7ms
	- the event tags stay (the user, 2026-11-15): they're there so queries idle on nothing, for larger draw
	  distances; the gpu swap goes down with vulkan
	- next: systems that mostly idle on fewer workers, the vox lod meshes and bake textures still ChildOf (33 and 42 tables in slow frames), quests and
	  dialogue pairs a npc
- greedy meshing for vox models, then terrain: the plan, AO and the steps in [tech/chunks3.md](tech/chunks3.md)

-x 2300 tables again, try reduce fragmentation

-x Streaming has a spike, make sure to display the biggest system spike per graph
	-x write a test for streaming spikes: perf_stream, perf_fly (flying, --maxnpcs), --profile, ZOX_SAMPLE
	  (doc/commands.md)

# Ideas
- Can we hide behind mountains to improve culling?
- 3D uis: canvas3_render_system gathers and sorts each canvas's elements every frame; keep the order on the canvas,
  rebuilt when its children or layers change (doc/tech/uis.md)
