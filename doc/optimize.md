# Optimizations
- Part of the [todo](todo.md), done items go in [done.md](done.md)

- Frame spikes, what's left after the chunk crossing lag (48 to 145ms, now ~20ms release on gpu, done.md):
	- spawn bursts are the worst frames (merges 46ms, observers 47ms, debug): names (435 a frame, flecs indexes
	  (Identifier,Name)), PreInitialize and Initialize (each new entity moves 4 times), 3D label layout dirties
	- DataUpdate 11k and TextureDirty 5k a run, what follows the dirties
	- block decor and vox instances are ChildOf a chunk (a table per chunk), npc ui still a child, link it instead
	- delete empty tables now and then (15k of 17k are empty after streaming)
	- lod meshes as non fragmenting children stalled terrain generation (perf_stream), still ChildOf, find why
	- move other heavy systems from process counts to zox_sys_budget
	- tst tables_stream (guarded at 300 tables a step), perf_stream, perf_fly
- greedy meshing for vox models

- 2300 tables again, try reduce fragmentation

- Streaming has a spike, make sure to display the biggest system spike per graph
	-x write a test for streaming spikes: perf_stream, perf_fly (flying, --maxnpcs), --profile, ZOX_SAMPLE
	  (doc/commands.md)

# Ideas
- Can we hide behind mountains to improve culling?
