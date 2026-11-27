# Todo

# Notes
- Bugs first, then by module (play test notes first in each). Done items move to [done.md](done.md).
- Refactors are a section here (# Refactors). Also see:
  [Test Fixes](#test-fixes) (failing tests), [Optimizations](#optimizations) (performance), [New](#new) (new modules and features), [Delayed](#delayed), [Whimsy](#whimsy), [Future](#future) (bigger
  ideas), [gdd/caves.md](gdd/caves.md) (caves design)
  
# Dropbox (Sort These)
- A block didnt get removed when i destroyed it - rare bug
- cant see the block damage overlay atm
-x Make level 1 slimes weaker
	- theyre hard to hit atm
	- curve difficulty by stats, which should be scaled by level too (speed and intelligence are, 2026-11-23)
	- npcs wander at the prefab's speed till their first battle (their speed stat is read as their behaviour changes)
	- tentacle: stop at other characters on its way too (blocks stop it now)
- Give Humanoids in town clothes
- Add a female humanoid
- rework color picking, centralize in realm, give themes, color entities
	- pick themes per realm
	- color entities
	- assign colors to blocks
- Fade in terrain chunks
- Fade between chunk meshes
- Light up character models like block voxes
- Glowing eyes that animate: eye colours glow (ColorGlows, doc/tech/rendering.md) and pulse or flare (anger, magic,
  night); combine_vox_system merges palettes by colour, it needs to carry their glows (and keep a glowing and a plain
  colour apart)
- Gravity cores glow: a blueprint node setting a palette colour's glow (models/ins/block_blueprint.c), kept through
  its animation frames' models
- Outline Rendering on Vox Models, dark outline (doc/plans/outlines.md)
- outlines in xr: its eyes draw straight to their swapchains (xr/fun/eye_render.c), they need a render target with
  depth and mask and the pass before the swapchain; the setting shows greyed (unsupported) there till then
- outlines on viewport cameras (spawn_viewport_camera: model_views, some tests): no frame buffer, no pass
- experiment: the outline pass on terrain too (blocks have outlines baked into their textures now), see if it reads
  better or replaces them (doc/plans/outlines.md)
- Day Night Cycle
- we should lower houses anyway so theyre on ground, but still be good to fix this
- Use waveform collapse on the slime model, like pengus and chooks, slems
- Add loading screen info, underneat 'Loading\n[1/299] Nearby Tunks Loaded'
- npcs outside nnear distance appear as lod 0, giant blocks, we should probably hide them instead
- optimize and reduce tests times
	- use void space / min entities
	- tackle the bigges test times first
	- keep the records of latest test results up in doc/results.md to compare
- instead of extending cameras, just spawn a child stream point and attach to them
States, events and tags, by doc/tech/states.md (nf tags and observers for events, systems for heavy work, fewer tags;
profile each, systems and observers counted together):
- texts' dirty: keep the core Dirty / Update (doc/plans/core_dirty.md option 1) until texts show in a profile's
  "moved most", then their own pair and a non fragmenting core Dirty (option 2)
- the game and player states together (GameStateDirty, PlayerStateDirty: a 3 frame machine, tags made the game clean
  up before the players saw the end): tag states (Paused, Playing..) with observers, the guard as its own step
- the action timers (ActivateBegin, WarmupState, Activate, CooldownState, TriggerActionA/B/E, ActiveActionDirty):
  are they steps, or tags with times
- Select's pair (tooltip_select_system needs a system tag) and Begin / End / Exit (model node systems, heavy): keep,
  or one fragmenting tag removed by a phase end system
- a grid laid out again when its elements change (only at spawn now)
- the bulk byte states (StreamDirty, StreamDirty2, NpcSpawnZoneDirty, LightOctreeDirty, RenderDepthDirty,
  RenderDistanceDirty): leave, unless a profile shows their increment systems
- dead commented systems: find and remove the rest (5 removed 2026-11-21)
Bugs from the full test run 2026-11-21 (Bugs above): end_game_cleanup, lod_swap, door_rays.

- Gravity fields (terrain/gravity, 2026-11-23), more:
	- particles and water flow pulled by the field too, driven top down (from the chunk's field to what's in it, not
	  each particle looking it up) so it scales
	- npcs turning toward a target (ai rotate_towards) and the third person camera along a character's up
	- a test of walking on a wall (sideways gravity, a player's controls: only a ceiling's flip is tested)
	- dropped items, pickups and npcs with TerrainGravity (npcs have it: characters), check they cope
	- a sound for a gravity cube switched on, and its model showing which way it pulls (an arrow, the core's side)
	- terrain faces against a block vox that fills its block (a gravity cube) culled: drawn now, hidden inside it
	- a test that a cube saved switched on flips gravity again once loaded (the load's VoxelNodeDirty rebuilds it)

- Power (doc/tech/blocks.md, ActiveOctree): manawires, power generator blocks, a PowerOctree beside the ActiveOctree
  (worked out and not saved, as light), power spread from generators through wires, doors, lamps and gravity cubes
  switched by it; coarse lod reads a parent's max

- Base blocks (stone, bricks, soil, sand) as blueprints (models/ins/block_blueprint.c, as the gravity cube)
- Model nodes: one observer on Begin calling the node's function by its NodeType (a table), in place of a system or
  observer a node type each that all check every node (sys/fill.c, colors.c, size.c, eve/mutate.c, traces.c)

- Maths: quaternion_normalized sets w to 0 (unused, remove it)

# Test Fixes
Failing tests from full runs, worked through slowly; making tests smaller and faster is ongoing too (onboarding
section 7, doc/engine/tests.md). Each: what fails, solo or only under load, what's known.
- 2026-11-24 full runs: 391 of 395 pass (the rest pass alone, flaky under load: stream_seams, tables_stream,
  punch_wind_up, map_pixel, door_save, town_furniture)
- door_glow (2026-11-27, solo, every run, master too): "open: no light came in by the door [255], shut it was [255]",
  the light by the door is full both ways
- dig_light on vulkan (2026-11-27, every run, master too): "the hole's faces never showed (0 of 5)"; gl passes
- bed_sleep (2026-11-27, solo, 1 run in 5): "asleep it isn't still: DisableMovement [0]" on climbing on, then
  "getting off it can already move"; the next 3 runs passed
- canvas_resize_input (2026-11-27, master too): at [440x300] and [300x440] the anchored button doesn't draw at its
  anchor ([260,110], [120,250]) and gets no clicks
- chunk_mesh_flicker, stream_seams: rare seams left, doc/plans/chunk_lods.md
- aesthetic_town, aesthetic_particles: the player spawns after ~80s (far 16), slower since b53bd6fac's light budgets
- coop_pause: its shared canvas only sometimes takes a low id (logged), a sure way needs flecs: ask first
- worlds: tests on the whole game get a random world each run, so some fail only on some worlds; give a test a seed
  (zox_tst_seed in its setup, as vegetation_chunks) where its check needs a kind of land
- making them faster: the biggest test times first, a world of their own or the void where they don't need a player
  (onboarding "Small and fast"), keep the latest times to compare (doc/results.md, by module: done to ui)
- every realm's wave function collapse (4 chooks, 4 pengus) takes ~2.6s of a debug realm's ~3.5s, in an observer on
  the main thread (models/eve/wfc.c): every test with a game or the void waits on it. On worker threads (a system, or
  the 8 models in parallel) it'd be a fraction: ask first, it changes how model nodes run

# Bugs
- tst input_focus_lifecycle fails: "first click did not activate text input" (on master too, 2026-11-26)
- tst.sh removes /tmp/zoxel_nosaves_<pid> folders whose pid isn't alive: from a sandbox with its own pid namespace
  (codex exec) every other running test's pid looks dead, so a parallel test lost its save folder mid run
  (coop_respawn_navigator, 100 save errors, 2026-11-26)
- tst resume_no_punch times out waiting for the pause menu (on 4e9f9fd3c too, 2026-11-26)
- tst outline_views --vulkan fails on lavapipe (software vulkan): gpu meshed skeleton meshes drawn with the outline
  mask pipeline (a second, integer colour attachment written) don't show at all; cpu meshed ones do
  (--vox_cpu_meshing passes), and on radv (--gpu) all of it draws and passes. Not found yet: a lavapipe bug, or
  undefined behaviour lavapipe shows
- tst gpu_restore fails on master (2026-11-26, gl): the picture's average colour changes after the restore (36 to 40)
- tst block_vox_hits failed once on vulkan (15 of 72 rays at doors' tops), then passed 4 times; once on gl (6 of 72)
- tst lod_light_flash flakes on vulkan (4 faces dark then lit), gpu or cpu vox meshing alike, 1 in 2 or 3
- Block voxes never spawn on the test level (zox_tst_level_setup: BlocksSpawned stays off, even the player's chunk);
  tests of them run in a game (tst doors, gravity_cube_vode)
- physics on lower lod chunks used to bug (not sure anymore, might still glitch)
- alt tab on windows, mouse lock stays locked
- mouse now doesnt go over hte shared player canvas, make mouse on a seperate canvas over others
- alt tabbing enough, input stops working when you get back
	- particularly mouse raycasting
- houses: add holes as windows (for now)
- save a bed's respawn point (HomePosition on the player, lost on load)
- realm clock, later: animations, the world (sky, doors), timers and particles onto the realm's time (doc/plans/realm_clock.md)
- a log window in the game menu with the game log's old lines (game_log keeps none yet)
- door open animation swings through nearby blocks
- tentacles shouldnt pierce through terrain lol
-x arm fix regressed arms, the shoulders need to rotate up, so arm reaches into the sky
	-x the arm now is out the way, but it bends unnaturally to the side rather than feeling like a game hand
- seams still flash where lods meet walking, rarely: doc/plans/chunk_lods.md
- lights at lods (2026-11-23): tst chunk_mesh_flicker logs steps as lods swap (a coarse lod's upward faces lighter
	than a fine one's, 27 to 238 at the coarsest: one light a cell, its lit air's mean): a coarse cell's light from the
	air over its faces' own voxels, not its whole cell's; and play test the dark flicker flying (vulkan) after the fix
- before committing 2026-11-23's lod work: tst all had 21 fail (other tests ran beside it); alone gpu_restore,
	gpu_restore_game (the picture's colour changed after a restore) and camera_filter (cubes not drawn) fail, sky_clouds and
	vegetation_chunks pass; highways, coop_skybox, scroll_wheel, navigator, place_hit, crouch, walk_bob, resume_no_punch,
	xr_play, xr_body, xr_lean not rerun. Suspect windows' get_time_ms fix (budgets now spent) or first meshes waiting
	for the lod swap; lint_systems: chunk3_save_dirty_system and maps_dump_system lack zox_sys_on_begin/end (older)
-x The mirror no longer updating when the player mesh does, will need a test for that and a fix (when i update hat it stays the same as old hat), mirror ui
- release builds have a ton of debug uis in them
	- just finish the terminal ui
	- it shoud have input field and log lines
	- do a help and have all the ui stuff there, or /testui gizmos etc
- full test run 2026-11-21 (320 tests): 5 failed, none from today's work
	- end_game_cleanup fails every run: entities after ending a game differ from the round before (+8 to -208), also
		on a7d8d3ff0 (before the day's work)
	- lod_swap fails about 1 in 4: "[1] swaps drew a mesh with its light colours unbuilt (dark)", also before the
		day's work (36f54ad2c)
	- door_rays fails about 1 in 2: "closed, [0] rays at the door found its neighbour, [12] nothing"
	- coop_pause: "the shared canvas didn't reuse a low id" (from 2026-11-20)
	- quest_dialogue timed out once waiting for the player (the machine busy), passes alone
- play test 2026-11-20:
	- cloth sticks to a forearm: part of the shirt follows it (cloth should stick together)
	- the crack overlay is missing in coop (the second player's screen), not reproduced: tst coop_block_crack shows
		both players' cracks, software and --gpu. Hit by melee or a skill? Near the first player or far? Which build?
	- "talk to someone in town": the tracker didn't mark off the first quest (likely the stale tracker, fixed 2026-11-20: check)
	- players talk to each other, but the second player didn't take part
- the second player flying away, off the minimap, crashed it (play test 2026-11-19), not reproduced: tst
	coop_map_markers flies them 6000 blocks off (tiny, the whole world, --stress) without a crash. Which build and
	platform, was the map open, flying up or along?
- tst player_spawn on vulkan with gpu meshing fails 2 of 6: "character is inside a solid block [43]", likely the
	check's timing (the gpu mesh lands a frame later), not a real spawn in a block (2026-11-20)
- XR on vulkan (--xr --vulkan, openxr/vulkan.c) is untested in a headset: check the eyes draw upright, both eyes,
	and the [XR] fps log against the gl es build (2026-11-21)
- a crash switching windows (play test 2026-11-19), not reproduced: tst navigator_inventory and coop_navigator switch
	fast (RB and LB) with an item in hand and a tooltip up; no core dump left. How were they switched (keys, mouse,
	gamepad), in split screen? Maybe the gamepad item pick up abort (fixed in a179f4344); coop_respawn_navigator (a
	death and respawn first) doesn't crash either
- XR lag, what's left (the frame's start and the hands' timing are fixed, see done.md): needs a headset to confirm;
	if it's still laggy, read the headset's frame timing log ([XR] fps, openxr/render.c)
- XR lean feels 2-3 times larger than real life (play test, on the build that slid the whole body): tst xr_body has
	it 1:1 scaled at 72 fps, the body now leans its spine first (xr_lean_angle_max); check again in the headset
- the cookie gather quest failed: now talking to a villager in a town finishes it, wherever the player stands (was:
	the player inside the walls); critters don't count. Talking still needs first person (dialogue_begin.c). Should
	critters talk at all?
- tst furniture_place stalls on "air in the player's chunk" (2026-11-18), passed 4 of 4 on 2026-11-20: rare or gone
- perf_stream and tables_stream sometimes fail: lod swaps don't settle in 20s (flaky before the test move too)
- grass shows overlapping copies, two lods at once (maybe rare): tst grass_overlap (vodes3/tst) sees no copies at the
	same position yet, compare by block cell next (an offset copy, i.e. another lod's scale)
	- wait this happens only when loading a new realm
	- the loading broke tilemap due to water
	- not occuring in new games, can ignore i think?
	- i think this can trigger still but its super rare, maybe race bug with lod changes
- play test the clothes' skinning (fixed 2026-11-18, doc/done/skeletons.md): sleeves, shorts, the shirt moving with
  the parts under them; Paint Weights (debug settings) shows each bone's colour, bash bsh/poses.sh --weights
- Crosshair will flicker siize too fast
    - give crossbar more natural animations when you see npc
    - seperate animation for the terrain or air
- Third Person Camera a bit buggy
    - rework this mode
    - make camera follow more naturally behind player
    - fix any edge cases with fps to third person modes so game feels same
- Test disabling lods, option used by users
- windowed mode, window decor is missing
- Big Terrain Disposal Bug
	- set render distance to 32
	- terrain very big, and delete, it causes it to hang
	- actually it did end up working
	- also deleting while its spawning, causes crash
	- I think we can just add a destroy tag, and then move destroy to another phase initialization issue
- Flying in disabled chunk - disabled due to no voxels - causes player to be disabled
- Top of world bugs
	- blocks placed on top of world cause no face to show
	- slime npcs arnt hitting back
		- they cannot raycast me on top of the world at 16 height
		- npc cant raycast me
			- only small ones
			- when im far away from origin
			- super weird
- Fix / test unstuck system and test it with no clip
	- like if we teleported into ground
- Fix flickering issue of raycast
	- maybe we should use a fixed rotation integral
- debug label - the font will dissapear sometimes and reappear
	- Looks like a memory issue causing the mashes to dissapear? when streaming
- Lighting was broken - the inside of a house was mostly bright
    - needs: a real save copy (zox_tst_save=<realm folder>, from the play machine) and where the house is
    - then a tst load_light: load it, find the enclosed air in the house, compare to a sealed room's light
    - light is not saved, it is rebuilt on load (src/lights/lights3/sys: refresh, sunlight, editing, flood)
    - happens when loading game
    - the lighting issues in house is from lights issues (play test 2026-11-15, fourth)
        - tested with light gizmos
        - the lights in half the house is all bright, suggesting is generated before the house was placed
        - Or didnt regenerate after?
- Fix No post processing mode / Rawdog Camera Mode
- Cube mesh doesnt restore on alt tab - gpu restore issue?
    - tst gpu_restore (menu, dispose and restore with the gl context kept) restores it fine: android's lost context?
    - 2026-11-20: a restore missed the lines, particles and 2D square materials (globals), bone index buffers, the
      instance ubo and disabled (lod) meshes, all fixed; play test on android again
- FIX: Set CubeLines Faces based on VoxelSides thats generated, then make sure it Renders Over Top of the overlays - disabled depth
- Fix chunk bounds debug visuals - the lines debugger
- scrollbar broken on hierarchy in editor
- hierarchy scrollbar goes into header
- text in buttons on inspector go outside the layoutsize
    - make the scrollrect just have horizontal scroll too like normal scroll areas
    - only when size exceeds
    - so make inspector/hierarchy buttons resize with text
- Bug: Realm Generation: Memory leak somewhere, breaks during VoxGenerationSystem - color spawning - test with valgrind on x86 later
- Loops endless - when exiting game now
	- can test by spawning/destroying terrain in main menu
	- i disabled mesh, vodes, npcs, lights, still stuck on dispose terrain
	- I disabled chunks.. its tunks or regions
	- by Elimination, we find its hashmap disposal hanging
	- Maybe we just build spinlock into the octree types root, would be a massive refactor tho
- Memory: ~6mb still lost a test run (AddressSanitizer, build.sh --debug --memory: finalize_arrayd float3/float2/int
  from other mesh builders, TextureData resizes, wav data), and flecs keeps a table for each humanoid's unique
  children (~100kb each, tst humanoid_leak)
- Heap grows ~22mb a game ended and started again (tst end_game_cleanup), entities, gpu buffers, textures and
  tables don't: trace allocations (glibc mtrace, or a flecs os api malloc hook) to tell a leak from flecs pools
	- check if flecs can own our mesh data (vertices, indices, colors, uvs are malloc'd arrays in components):
	  flecs allocators or sparse/vector components with hooks, it might fix the heap growth and frees on delete
- Web:
	- npcs missing
	- Dev version stuck on load game (-O0 and verbose logs, too slow in a browser): bsh/web.sh --release --logs
	  logs from a release build meanwhile
- Android:
	- Touch UI not showing up in game
		- Fix touchscreen touch ui [player_state_touch_ui]
		- device mode not set if not spawn
	- NOTE: Zoxel android release working now, was issue  with no safety checks Crash
- Gamepad navigation faulty neighbors
    - Implement position checking for any interactable items in canvas tree
- Linux (Khadas):
	- Decor (SDL window edge) missing on khadas - try compile sdl together?
		- test sdl window
		- test wayland window
- Flaky: humanoid_leak (sometimes almost stops after round 1 and times out, else passes in 65s), npc_leash (the npc
  walks away from home, -x, passes and fails on the same commits), quest_dialogue (the first talk can take 15s+)

# Optimization
- Steady 120 fps (8.3ms a frame), streaming too (vr): tst perf_terrain, its stream's frames over 8.3ms and profile
	- far 32 (2026-11-21): ~75 to 80 of ~1000 stream frames over 8.3ms, 10 to 11.5ms each: ~6ms of many small
	  systems and a ~2.5 to 3ms merge (that frame's table changes), no one system spiking (the stream sweep was 6.6ms,
	  spread over frames now)
	- consolidate chunk dirty states into a DirtyState (byte bitmask, core): RenderDepthDirty, RenderDistanceDirty,
	  LightOctreeDirty, NpcSpawnZoneDirty, 2 bits each (trigger, active, end); present only while a bit's on, so their
	  increment systems (~0.4ms a frame over every chunk) and readers only see dirty chunks, removed when all are off;
	  more than 8 bits: a 16 bit DirtyState2. Setters from worker threads add a non fragmenting event tag, an observer
	  ORs it in at the merge (two threads setting different bits in one byte lose one). Profile before and after
	- then the build chain's tags (BuildChunkSides, BuildMesh, MeshDirty, ChunkLodDirty, ChunkUpdate, FindNeighbors):
	  400 to 600 table moves a frame, most of the ~0.85ms merges and ~0.35ms observers an average frame

# Refactors
Code to restructure (refactors.md merged in here 2026-11-21). Done items go to [done.md](done.md).
- Remove the `extern` uses (~200 `extern` declarations in src and gam): a module calling a later module's function
  through an `extern` means the code is in the wrong module or the wrong order. Move each function to the module
  that owns it (or the call to a later module, an observer there), so the include order (src/modules.c, each _.c)
  is enough and no `extern` is needed
- Prefab events to observers (the user, 2026-11-27): `InitializeEvent` (a listener list of function pointers on
  prefab_app, `zoxc_listener`, src/_/com) has 17 users, mostly settings spawning (`add_to_InitializeEvent`). Each
  becomes an observer in its module (on the app's Initialize), and the listener component goes
- Clean up the module organization: features in the module that owns them (doc/engine/architecture.md "Code goes
  where it belongs"), folders as the coding standards lay out (com, pre, ins, fun, eve, sys, tst), no reaching into
  another module's includes for load order
- Movement inputs apart from the move system: players' devices and npc ai write one move input on a character, one
  system moves them all by the same rules; tests read no real devices and set that input. Discuss first
  (doc/plans/poly_terrains.md, Later)

# Tools out of the test harness (2026-11-22)
- Move the looking tools into bsh/tools (doc/engine/tools.md): body, poses, ik_poses, walk, punch, model, gpu_lights,
  gpu_mesh, compare_backends, compare_meshing, sample_report, code_stats. Those that run a `*_views` test become a
  game flag doing the job and quitting (as `--maps`, towns/dbg/maps_dump.c), the test then calls the same functions
  and checks what they make (the views a test checks stay tests, the ones only looked at become tools)
- A blocks tool (bsh/tools/blocks.sh, doc/engine/tools.md): every block's baked faces into tst/blocks/, and a scene of
  the placed model blocks (grass, flowers, doors) several times each, to see their variations

# The mouse turns the head, the camera rides it (2026-11-22)
- the look's pitch is the first person camera's own turn on the head: the head bone stays straight ahead, so other
  players see every player's head looking straight ahead (multiplayer), and what reads the look (the guard's swing,
  the body's lean, the dialogue aim) finds the camera first
- the mouse pitches the head (head_look_system's ahead), the camera on it at the eyes: the look is the character's
  state, free roam keeps it with no copy (LookHeld, LookHeldEye and first_person_look go), the lean's head turn and
  the dialogue's aim are turns of the one head
- check: the camera's height and looking down into the body (head_camera_offset turns with the head about the
  neck), tests pitching the camera (guard_pitch, free_roam_pose, dialogue_aim) pitch the head

# RenderDisabled to tags (2026-11-22)
- RenderDisabled a tag (Invisible), and one for not raycast or navigated to (DisableInteraction): it's a byte each
  draw reads; a window's tabs (ui/windows/ins/tabs.c) hide theirs with it for now

# Tests in their own place (2026-11-22)
- tests that span modules sit where the build order lets them, not with what they test: find them a module after
  the ones they use (a tst module last in the build)
  - actions/tst/mirror_hat.c: the mirror (huds) following the body (equipment), huds builds before equipment
  - vodes/doors/tst/door_sky.c and door_light.c use terrain/tst/glow_relight.c's room helpers
  - door_light, door_sky and glow_relight to an air chunk (terrain/tst/air_chunk.c, as door_load): no player, a
    few seconds less each
  - items/tst/all.c, the all items cheat, lives in tests: a debug tool, tools out of the test harness
- lod_flyby (terrain/tst/lod_flyby.c, bash bsh/tools/flyby.sh), unfinished (2026-11-23): a camera flying the terrain,
  recorded (flyby.gif), each frame's blinks and light pops against the 2 before, its worst frames as pngs
  - works: no player or menu (zox_tst_world_setup), its own camera a streamer (only a streaming camera culls chunks),
    its local transform set too (set alone, Position3D stayed at the origin), the game's --far 16
  - to do: its height over the land (zox_tst_flyby_ground) looked under it, check a frame; ~0.5 fps on lavapipe (the
    pixel checks a frame and the gif): try --gpu, fewer checks; then thresholds for its blinks and pops, a check on them
  - perf_terrain has the same culling problem: its viewport camera isn't a streamer, so the menu's camera culls chunks

## Older
- Move blocks from biome spawning back to realm
	- we need rules that make each biome feel unique still
	- we also still need variety of blocks to feel abundance
- merge mesh building code from colored voxes and terrain vox
	- uv placement would be a seperate system
- refactor generation to use nodegraphs
	- textures
	- block vox models
	- flesh out node processes to have more functions, using observers and nodes
	- write more tests to maintain this

- A player's canvases (CanvasLink is many since 2026-11-20: its own, and while paused the shared one, huds
  pause_canvas.c): ~90 places take `zox_get_link(player, CanvasLink)` as "its canvas", flecs keeps no order for
  the targets; a helper (the canvas whose PlayerLink is the player) swapped in as files are touched. Input already
  walks all of them (ui navigation begin.c, interaction raycast.c); tst coop_pause checks the first is its own paused
- Activated block states (a door open, later redstone power) to an activation octree a chunk, out of BlockDatas,
  which keeps data that doesn't change (directions, filler offsets): doc/tech/blocks.md "Planned"
- Body parts by entities, not SlotType ids: realm parts found by find_slot_type, gear is by tag now (Hat, Shirt),
  and character_body_spawn_system's slot tree as data (parent, anchor, offset, part) walked once (doc/tech/body_equips.md)
- refactor GenerateModel to tag event
	- check any other bytes that can be tag events too
	- the idea is to reduce idling systems when the game isnt using them
- remove any hooks and keep it all inside flecs pipelines
	- we want it consistent and easier to debug the logic
	- ecs practices
	- observers for reaction based logic
- make text ascii instead of currently uses a custom indexing, ascii will make using strings easier with our text data
- Merge the voxel build code
	- same side octree building too
	- merged mesh face building
	- uv pass in a seperate system
	- ao in a seperate system
- greedy meshing for terrain (vox models done 2026-11-26, doc/plans/chunks_refactor.md phase 5): a big task, steps 5
  in [tech/chunks3.md](tech/chunks3.md) (Greedy meshing): textures repeating inside a merged quad (a tile's uv by
  fract, or a texture array), then lights (bake a face's 4 corner lights into its key, or relight by position), both
  meshers (cpu grid, gpu); measure perf_terrain, dig_light, lod_light_flash
- Refactor character vox into sub entity
- Single Vox for Grass
	- split into sub chunks
- Move blockVox from models into blocks
	- just link it to model and tag it, no need to have it models
- Tooltip systems: a deselect closes by an observer (Deselect); the select stays a system, as an observer on Select a
  fast flick lost the last icon's text (tst tooltip_flick); tooltip_deselect_system is the flick's safety net
- Add locks onto all VoxelNode setters and getters
- Refactor character mesh from its prefab
	- vox / vox instance will be child
	- test function to set it as unique when decaying its mesh

- Refactor the shaders and element render system
- Refactor Block Model Spawning out of block_vox_meta functions
- Refactor Texture Generation into Nodegraphs

### Modules

### Events

### Unsorted
- Make MeshDirty + and other bytes tags
- Make initialize systems just use an event / observer
    - moving them to callbacks
- make pickup collider a sub entity on player
- Move the key events out of the test functions
    - just add to a KeyListener component
- Refactor GenerateChunk to Generate, Generating, etc
	- GenerateModel
- Make chunk datas more self contained
    - dont use terraindepth, just use maxdepth property
- Remove state components and just use tags + dontfrag
- Refactor our timings from on system entities
    - spawn a profile entity (dataset)

### Android
- move the keys into Apps folder on phone
    - android keys
    - build same apk from other machines using it

### Custom Phases
- remove update loops and just use systems as functions

### UIs
- Centralize different uis
    - atm theyre a bit spread through different state events
    - For example screen fader is in 3 events
    
### Refactor Events/Properties To Tags
	* make event tag
	* add/remove instead of set
	* remove/add sysetm for a Update variant tag
	* update systems use hte Update tag [none]
	-o GenerateTexture
	- PlayerStateDirty
	- RenderLodDirty
	- NpcSpawnZoneDirty
	- ActiveActionDirty
	- StreamDirty
	- RenderDepthDirty
	- ChunkLodDirty
	- RenderDistanceDirty
	- GlyphSpawnedDirty
	
### Refactor Properties
- RenderDisabled
- BlocksSpawned
- ZeviceDisabled
	
### Refactor Reactionary Systems to Events
	- Tooltip Systems
	- Settings Spawning - InitializeEvent on App
    
### Remove old links
	- BlockLinks (lookup table, voxel value - 1 is the index)
	- BiomeLinks (lookup table, biome id is the index)
	- Still zoxc_entity, need a decision:
		- Target, ActiveAction, Clicker, ClickingEntity, WindowToTop, GlyphPrefab
		- not links: ComponentTarget, TaskbarWindowID (component ids), EventInput/2 (event payloads), DungeonWallType
	- Indexed arrays (order matters, not links): ModelLods, Chunk3Stack, ChunkNeighbors, Chunk2Neighbors, CombineList
	- ChunkEntities (reverse of ChunkLink, could become links)
	- Children + ParentLink (old hierarchy, move to ChildOf)

### Input Refactor
- Make input actions events instead of systems
    - Pause key - player_pause_system
	
### Remove old structs
	- LayoutParentData
	- ElementSpawnData

### Character Refactors
- Refactor character mesh out of character - as child
- Add character CombatState

### Tags / Events
- Use a generic Dirty tag, use for stats
- Refactor RenderDepthDirty, RenderDistanceDirty
- Refactor MeshDirty as tag
- Refactor BuildMesh - tag - BuildSkeletonMesh after
- FindNeighbors - make it trigger this on spawning - triggers neighbors if exists

### Unsorted
- SDL should be a module again
    - it will just add onto the App prefab and events
    - This makes it easier for Glut to be implemented or others 
- Rename Cubes - Geometry
- Move Bodys and Equipment to Parts with tags
- Move Texture tag from rendering to Textures
    - Just wrap gpu stuff in graphics, then do it all in textures before our rendering
- Rename Rendering: Geometry, used for meshes
- Rename LayoutSize as PixelSize, as well as ScreenSize, add to Core (generic af)
- Refactor settings spawning back to hooks
- Make Initialize a tag - dont fragment
    - remove at end of frame
- Make ChunkEntities use links instead of components
- Remove DeviceLink from zevices, use parent
- Remove Camera Datas - main_cameras ui_cameras
- Remove DeviceLink from Zevice, use parent
- Spawn finger entity during finger down - instead of just setting
- We can refactor all the links between enttiies as pairs instead of components


### Events
- Move sound systems to events too
    - pickup_sound_system
    - land_sound_system
- Remove state systems and just use event components with listeners

### Devops
- We need docker libraries to be seperated from regular ones
    - for docker.sh - add docker tag in build.sh
    - then make libs use seperate folder

### Tooltips
- Refactor Tooltips as events
    - Make the event store multiple functions
    # Probably the tooltips can all just be events - systems arnt needed for these

### Apps
- Move sdl/glut/glfw/headless into core as wrappers or not idk... we havnt actually wrapped yet!

### Data
- Remove build states when not building (60ms)
    - chunk_mesh_colors_trigger_system
    - ChunkTexturedBuildSystem
    - SmoothLightsBuildSystem
    - BuildMesh

# Refactors
- Refactor: Stream Terrain Chunks from terrain, not chunks, just spawn there
	- Keep setting the lods in chunk systems though as can be multithreaded

### Nodes
- Add color hsv limits like the size later to the color node
- Color node should set a reference name
    - fill can use this instead of hard values

### transforms2
- Add a matrix path for entities with `Position2`, `Rotation2`, and `Scale2`. Keep `DisableTransform` filtering and let `Scale2` take precedence when both scale components are present.
- Extend `transform2_matrices` with a rotated, nonuniformly scaled entity and verify its matrix against a known point.

### Ui
- Refactor taskbar as entities and add the Mirror UI there
    - so its easier to add the new mirror button

### Skinning
- Add BoneOctree
- Set Bone values when we place items into body
- Convert Bone values into weights instead of using sizes
	
### ChunkTextured Refactor
- Spawn a Terrain chunk as test function
- Spawn a TerrainChunk with no Mesh and child Renderer
	- fix systems to work with new terrain chunk

### Chunks
- seperate Chunk into Chunk + Render
- after chunk3 refactor, use a render per lod, instead of regenerating everytime
- add local chunk lookups for block indexes
	- Prevents Updates messing up Chunk Save Data

### Chunk Refactor
- Fade in Chunks
- Spawn Hierarchy + inspector when we spawn our Vox Test
- Spawn the Vox test without renderer, and use child as render
	- refactor the mesh building functions for this use
- Switching LOD Meshes should
	- fade in and out the switch
	- when fadeout completely they get disabled
	- when fading in, they get enabled (entity enabling)
- Refactor Chunk to Chunk + BlockMaterial + Lod Render
- chunk = chunk + renders (seperate)
- Generate sides per Chunk3Material (sub entity of Chunk3)
- Spawn Chunk3Render entity per LOD level when RenderDepth set
- Refactor models to just spawn one vox model and multiple render objects as children
	- Make lods just use the same model, no need to create 5 models per slime
	- just generate per each level - set with shapes per node level
	
### Extras

- Refactor Settings to Entities
- Move Damage outside of MeleeSystem and add a DamageQueue onto the terrain chunk
- Change Settings / Options UI as entities data
- Change Taskbar as entities data
- Move TexturesSize + TexturesDirty to Textures module from Rendering
- Remove any Hook use and just use system states
- Reduce systems total

### Refactors
- Remove refactor Vodes spawn code
- Replace Chunk VoxLinks with Parent calls
- Refactor Model Lods -> one model, just create mesh per lod
- Refactor 3D UIs to use 2D UI stuff
- Remove struct use from spawn_block_vox_meta
- Remove Duplicate UI prefabs
- Remove structs from UI prefab use
- Remove spawn_window_users use
- Refactor Header Spawning to system
- Remove all spawn_window_users's
- Spawn character data from biomes
- Compare raycasts based on priority
	- Raycast Solid Blocks, Character, Non Solid Blocks (Grass)
- make shape type and centering part of painting as well - use fill system just with diff byte for checks
- refactor soil/blocks into nodegraphs for models
- link nodegraphs to realm's nodegraphLinks
- list uis should just reposition inside system when children dirty
- taskbar data into entities
- settings data into entities
- remove sdl_image and use BMP imports
- ~~remove sdl_mixer and use another simpler audio lib~~ (sdl audio, mixed in src/sounds/mix)
- Refactor texture generation to blueprints
- upgrade the texture for the item frames
- Terrain loading and finishing should be overseen by GameState and not Player events
- Move character/block name generation to system under new module Names
- Remove any random delays, and make the state changes timed instead
- Add a tooltip that just stores a string - no need for event every time

# Future Ideas

### Voxel Links Side Table
- VoxelNode reuses `ptr` for children or an OctreeLink (type 255), so every octree function must check the node type (`octree_children` helper)
- Idea: move links out of the octree into a per chunk table keyed by position + depth, then `ptr` always means children and every octree function is generic
- VoxelNode could drop its `type` byte (stays 16 bytes from alignment anyway)
- Cost: a table lookup to get a voxel's entity, instead of the octree walk
- History: links used a table before and were moved into the octree, as the table had speed issues. Benchmark (stress test) before trying again

# Sounds and Music
- different music per biome too
- cant hear footsteps well unless very loud, maybe balance is off
	- can turn down the music a little
- add synths instruments
- add battle music, when in combat, sped up, pump blood
- Add synth instruments (or padsynth?), think metroid prime, i really love that sound track
- Add harmonica, like zelda OOT, they have it in song of storms, or accordion? idk
- Make a second song, play in town, thats based off song of storm, a bit more cheerie, but a bit eerie
- Make a third battle track, using synths, inspired by metroid prime theme song
- Lag spikes (up to 96ms): sound generation, a whole sound in one frame when it spawns
	- measured (debug): a 1.3s choir note 290-670ms, a 2.8s cello note 60-100ms, choir 330ms per second of sound
	- music pads and jingles play choir and cello notes, the spikes
	-x 1. choir and cello are wavetables: one cycle built per note, read at its phase (choir 330ms -> 3ms a second)
	- still slowest (debug, a second of sound): unique 29ms, flute 27ms, harp 24ms, soft piano 17ms
	- 2. cached instrument tables: generate base waveforms once, reuse them, add noise, breath and vibrato
	  in a pass while playing (see Sounds: optimization)
	- 3. long notes generated over a few frames, the start first
	- tst: every instrument generates a second of sound under a budget (sound_instruments logs timings)
- Optimize sound generation (see Lag spikes above)
	- reuse base instrument waveforms (sin tables, harmonics) instead of generating every note
	- noise and modulation as a processing pass while playing, in the mixer
	- instruments as entities: their settings and cached wavetables as components, notes link to them
- Musicians' mistakes: niow and then a wrong note (a neighbour), quickly corrected, a little out of time
	- in perform_song, tst music_compose allows them (it checks every note is in key)
	- noise and detuning added to a note after it's made, per instrument entity
- Opt: sound_generate_system by using preset sound data + mods
- Opt: Spawn Sound Samples on Realm Start, then use this data in generated sounds - optimized sound generation
- sound_generate_system - uses too much still
- Add damage and sound if you land strongly
- Change music track when dialogue starts too

# Terrain and Chunks
- Flying's slowest frames now: new tables while streaming (16%), colored chunk meshes (9%), tst perf_fly
- Water in the world (the block, its rippling texture and blended meshes are in, tst water_block)
	- (done: a sea at 18, lakes in bowls, doc/tech/water.md) rivers later
	- (done: flowing with levels, thinner as it spreads, drains, doc/tech/water.md Flowing)
	- play test water levels: dig by a lake, pour water down a hill, take a source away (does it look right as it
	  drains? the falling column's sides)
	- (done: swimming, the blue view under it, muffled sounds, meshes far to near: tst swim, chunk_order)
	- play test swimming: sinking speed, the stroke up, climbing out onto a shore, the underwater fog's thickness
- Remove trigger system for lods
	- just update neighbor of updating chunks inside stream systems
	- atm the faces stay sometimes between lods
- Current Terrain Chunk Refactor:
	- Move spawn gpu stuff to initialize mesh systems
	- Frustum should account for sub meshes
	- We should set bounds of chunk sepertate to mesh
	- Next:
		- Add extra stage for Sides - ChunkMaterial
		- Add second block with lava material for test
	- Systems: GenerateChunk, GenerateSides, Generate Mesh (children)
	- New Systems: chunk_mesh_spawn_system, Chunk3MeshTrigger2System, Chunk3TexturedBuild2System, Chunk3Sides2System, chunk_mesh_slow2_system
- Refactor Terrain Chunks to use LOD mesh entities instead of singular chunk
- Prioritize Chunks higher lod first
- Also process double if the lod is less, per depth
	- use a point system for counting processing based on depth levels
- When chunk updating, create an update group
	- first check neighbrs, if it has an update group
	- if any do, we use that instead
	- When pushing update, just check the group if all chunks on are done
	- This basically keeps them updating in groups without the need to check entire terrains Chunks
- Make a better slow chunk system - perhaps a queue to terrain, when theyre done just remove them from it, to track
- Queue new chunks - so it doesnt lag
	- TerrainStreamSystem is basically just spawning new chunks
	- Tunks Lod updates happen elsewhere in threads
- Add a queue for spawning new positions, do like 256 checks per frame for terrain for spawning, so its delayed a bit? only if still lagging with new stream systems
- Chunk meshes should only be spawning if materials per side > 0
	- Ignore spawn mesh if air or if no visible edges
- Add material + system for Fading chunks in
	- Tag then Remove tag after faded for TransparentRendering
- After it works, lets fade between the lod meshes
	- handle interupts so it can fade the other direction again
- also switch chunk depth to 5 + double scale so its 32x32x32
- if our body is overlapping chunk edges, we shouldnt update the chunk streaming, wait until its not overlapping
- Flag if chunk has file, then progressively load the contents per LOD level
- Add dictionary to chunk data, so it maps the blocks to the block ids, and then make the block ids generate from realm ids so it stays consistent after changes
- Can we give each chunk a seed
	- then use seed for spawning npcs so it spawns the same every time
- Opt: ChunkNeighborSystem
	- On Init - add to neighbor directly from the init chunk
	- On Spawn - grab directly there from dictionary on terrain
- Opt: also add back the color uploads for chunk meshes - MeshColorsDirty
- Remove DeactivateDelay on chunk meshes and build new system for this
- ChunkTexturedBuildSystem - 28ms idle??
	- should we be removing states after?
- ChunkFrustumSystem [20ms]
- ChunkColoredBuildSystem
- Ref: Remake RenderDepthDirty for TerrainChunks, setting per system instead of zoxc_state
- make it fade between lod chunks instead of instance switch, so its less jarring visually


# Generation, Biomes and Regions
- Seeds: one seed should always make the same world
	- done (594b25b10): rand has a state each thread (zox_rand, zox_srand), so block models seeding and drawing
	  on workers no longer change each other (one seed made two worlds, tst screenshot alternated)
	- left: what generates from zox_rand without zox_srand first takes the thread's last state, so depends on
	  which thread and what ran before: find the world's generators that don't seed (towns, npcs, items, vegetation,
	  dungeons, names) and seed each from the realm's seed and its position or id
	- gameplay rand (ai, drops, particles, sounds) can stay unseeded
	- a test: two runs of one seed, the realm's block colours, town positions and first npcs compared (seed_world)
- Population: more info per tunk (who lives there, counts), a level per region (like Trove), regions harder the
  further from the origin (world scale to decide)
- More natural mountains
- grass and mushroom placement doesnt feel natural enough i think
- place a dungeon 'zone' in the game (zone can be what mountain and towns are)
	- spawns a dungeon core there
	- slowly builds over time
- let dungeon entity exist if its inside region of SimulationLod = 0 (so where player is and ones next to that)
- Consolidate structure placement in generation: trees (vegetation_draw_tree), town walls, homes, roads, dungeons
  each place a shape and clip it to the chunk drawing it, the same pattern written again each time: one way to
  place a structure (where, its seed, its bounds) and draw a chunk's part of it, with one across chunks test
- Structures across chunks: trees stay in their tunk by design (cells line up with tunks), their trunks and
  canopies cross chunks vertically fine (tst vegetation_across), but anything bigger than a cell (big trees,
  ruins, houses) needs planning before chunks draw it: place it in its region (regions/fun/place.c), each chunk
  draws the voxels of the structures overlapping it, like towns into tunks (regions/fun/tunk.c)
- --findtree: the player character walks to the nearest tree on its own, using the npc ai controller code (move to
  a target), so npcs can use it too (find a tree, a home, food), and a test can watch it get there
- Caves (design in doc/gdd/caves.md): 3D noise first (and its test), then cheese and spaghetti carving, then worms
  planned by regions
- Biomes could use their climate: hot ones drier blocks, cold ones snow on top, moisture for vegetation density
- Tune zox_biome_blend (climate distance a blend fades over) by eye in game, 0.035 now
- Use Realm Seed + region positions to set Region Seeds
- Use Region Seeds to generate mountain dat
- base biome color on prior biomes so it stands out too
- Create a biomemap at the region level that helps coordinate
- Opt: height_map_system lagging, use LODin g for HeightMaps / Tunk Generation
- Refactor: Tunk Generation should have lodding as it currently lags
	- if still lag at per system limitations
- Tunks to use Quadtrees and LODing
- Optimize the different block generation
	- lags on realm load (for my arm pc)
- Regions
	- destroy regions
	- Region lods
	- region town/mountain links
- Add lookouts around map - just big ole scary towers
- Create a fullscreen ui scroll view, render textures, of spinning vox models
	- each one will be a part of a population
	- population breeds based on rules
	- the final population becomes the biome blocks
- Tunk LOD Levels
	- So we dont generate maps for far away chunks

# Towns
- play test: make towns feel more natural (less grid like)
- windows in homes, a transparent block that light passes through, only in some home types
- Towns: an age level, older towns are flatter (heights), bigger, with more buildings
- Homes: tilted foundations for poor npcs (homes stand level now)
- Homes: keep their door steps clear of neighbours (two homes close by, steps meet the other's levelled ground)
- Regions as top level data (2026-11-22):
	- Regions unload like tunks (they stay once spawned now), spawned 2 regions round the streamer (now: the 3 x 3 round
	  each streamed tunk's region, for highways), with lod levels like tunks
	- Tunks drawn before a region 2 away came see none of its towns (highways at the stream's edge can differ)
- Town walls more organic (2026-11-22): the wall is a rounded rect with a little noise now (fun/town.c town_shape),
  still reads too regular. Next: grown from the seed, a blob of overlapping district rects (a core round the square,
  districts grown off it and each other), the wall the band in from its outside edge, the edge wobbled a block or two
  (town_shape_depth's API stays: homes, roads, lamps and the gate already use it)
- town_homes: a door now and then has no town road with the curved walls (a door near the wall, lines out of it
  leave the curve), to look into
- home_foundations since the curved walls, 1 run in 4: once [59] columns of a home's room filled, once [2] steps
  taller than a block (a home near the curve: its wall's band or a road's ground?), to look into
- Roads and highways: bridges over water (a causeway now, the ground filled up to the water's top); highways cut
  round mountains by dropping nodes and links (a gap in the lattice), links could bend round instead;
  npcs walking them between towns; highways' lattice links are straight lines, a bend at a midpoint from noise
- street lamps: firefly particles round their heads (regular ones for now); a rounder model (think hollow knight)
- Multi room homes: inner walls splitting a home into rooms, doors between them, rooms sized by the home
- InTown for every character (in_town_system is players only now): town buffs, villagers knowing their town
- Towns: Average out heights between chunk neighbors
- Towns: Set Block Links inside Homes, for Home Floor, Roof, Wall, Door blocks
- Make homes built with different materials / Blocks
- Make Road from middle of town to gate
- Make Walls Thicker
- Make Town Gates more overhanggy
- Spawn / Respawn point should be in a Home - facing outward

# Blocks
- play test: particles when you hit blocks (see new block destruction animations below)
- rotate a bed or door before placing it (a key or the wheel); placing turns it to your facing now (52d6d0b3d)
- multi blocks (beds, doors):
	- beds: sleeping lies you down, the eyes close, "Wake" stands you up (vodes/beds); play test it
		- the climb with the bones (ik): pushing up with the arms, a leg up onto the mattress
		- npcs sleep in their homes' beds (character_sleep / character_wake)
		- after day and night: sleeping skips the night, sets your respawn point to the bed, heals while asleep
	- play test doors (right click opens and closes them, vodes/doors): the swing's direction (to its back, away from
	  whoever placed it), the creak and the shut, walking through; it's passable as soon as it starts opening and
	  solid as soon as it starts closing (someone in the doorway is pushed out)
	- npcs open doors
- Blocks: refactor them onto the realm, biomes pick theirs by random rules (still variety in dirts and others)
- sway grass more when you walk through, and better foliage pass through sounds
- new block destruction animations 
    - Add simple generated cracks to darkness texture
    - add a animated cube in cubes, that takes textures, makes a tilemap scroll, then it just animates through it (idk the specifics, think minecraft but unique every time you damage a block)
    - start cracks where you hit
    - make particles spew out of the block in the color of the mini voxels of the block
    - when hitting side of block, make popups come out that way
    - make popups better, curves, or maths magic, and color changing? idk exactly, make leet
- Implement a tall grass block model
	- support for linking a world model (minivox/blockvox) to multiple voxel links
	- add a second block, tall_grass
	- multi chunk support needed for vox models
- Make Grass Taller, 2-3 blocks tall sometimes
- seperate block spawning more from the realm,break it into biomes and modules
- remove use of global voxel indexes and use BlockLinks from realm
- Add Regen to BlockHealths
- Use BlockHealth prefabs with health stat as children
- Vodes: Destroy Block Health when full for >= 3 seconds
- Add strange names for blocks
	- just show label as Soil type of block
- Refactor spawn_block_vox_meta and remove structs
- Finish removning all spawn_block_vox
- For soil vox addition, while it digs down, use a random chance to alter the color?
- Improve Dirt Model
- Improve Wood Model
- Add Town Dirt Model - more like stone ish, rounded pebbles
- add variant models for more block voxes

# Models and Vox
- Refactor Generation systems into nodegraphs, for block voxes
	- bricks_model_generation_system for example
	- this will let us reuse many of the same functions easier with node graphs
- we should add in the wave form collapse for vox models, and generate hats off that, so they fit the heads (magic equipment in the story)
    - import the flower, make 8 variations from this, its seed, and wave form collapse
    - see how flowers use variant models - model groups  
- Create a chicken man head, similar to human one, red, white, or similar levels of color hue contrast, and a mohawk/father, beak, etc
	- be creative
	- use modelview test to iterate design of the blueprint
-x cookies need crumbes, we need a sprinkle node for this
- Node (Runner) Process Entity should shift the seed its using
- Add SetSizeNode with min max properties
	- min max get shifted depth to the depth of the model
	- this lets us use same blueprint for all slimes
	- This way we wont need float3 ratio in the model spawning too
- Add paint detail - clamp options - for tiny details
- Once its done, get ai to spawn chickens and cookies
- add spawn spots node that adds some variation
- Refactor Model Blueprints to vary size based on seed - set size node
- Refactor the model to generate all chunk meshes
	- refactor vox color builder to use parent
- Refactor Models to nodegraphs for soil, stone, etc
- Ref: Make models just use one vox with multiple Renders Underneath, atm it spawns 5 models and regenerates everytime
- Refactor Colored Vox to use same ChunkSides system
- Convert Vox Model to use ChunkMeshes
- Refactor Vox Model so meshes can be underneath them
- Big one is making Vox + terrain use same ChunkSides code
	- make vox use chunksides
	- refactor vox mesh outside of vox itself
	- Seperate out UVs generation from Faces, so they both use same Mesh Building
- Make the chunk colors use same system to build mesh
	- we can just add uvs in for textured system
- MeshDirty should set for diff data - bitfield - Mesh + UVs + Colors etc
	- and keep set in systems that upload them
	- keep them all modular
	- keep it all centralized in rendering so we dont have to do the same work everywhere
- Eventually we need multi chunk characters, cause their updates lag (see Future, multi chunk models)
	- max depth 4-5 for mesh build times
- Vox Frames - different vox models we swap between
- make voxel item textures 2 pixels larger, one is outline, one is air

# Characters and Npcs
- npcs in towns wear random hats (a third none), hats generated per realm with nodegraphs
- humanoids need more variation in town
- add female variant humanoids
- patrol paths for npcs, and routines (need time of day, the day night cycle)
- Add a sphere collider under npc
	- detect nearby characters
	- event useful when spawning/despawning name labels or markers
- Noticing (post mvp, close by is fine for now): npcs notice you more naturally, by hearing your footsteps (louder running, nearer) or seeing you in a
  vision cone (in front, in range, not behind walls); their glance (ai/sys/glance.c) and turning come after noticing
- Boneless npcs (slimes, critters) turn their body to a player they notice (rotate_towards), steady: rotate_towards'
  PD is underdamped (kP 4, kD 0.5), measure overshoot first; a tumble or hop fights stay_upright_system
- Npcs remember where they were and walk back home: a despawned npc returns at its home now (NpcHome, its seed),
  it should come back where it was and path home, needs A* pathfinding and pathing octrees (none yet)
	- pathing octrees: walkable space from the voxel octrees, coarse nodes for long paths
	- A* over them, npcs follow the path, wandering and returning home use it
- When Add/Remove body part
	- remove the associated item slots
	- refresh the body ui
	- keep same pose - camera position and head
- for body parts we can detatch:
	- add a tooltip detachable line
	- change frame color slightly to show
- Npc Item drops - should pick an item item randomly based on rarity of the body parts + inventory
	- it should not drop all items
	- player characters should still drop all loot though for chance to pick them up
- Npcs: Add Monster spawn zones for types - remove around towns
- Spawn NPCs based on spawn maps in tunks
- optimize npcs
	- just spawn closer to player, so not as many for now
- Optimize the npc spawning
- Spawn/Despawn npc labels when they enter near player
- if npc is being baited, by the player keep running around them
	- they can run away
- make some npcs attack you
- Make a AOE skill that npc can use when its low health
	- adds particles
- NPC RaycastRange add ActiveAction to NPCs - active_action_range_system
- NPC - Raycast Range should be Skill Range
- Pause NPCs when press escape
	- pause should pause the npcs too, cherrie wants dat
- Add death event for character
	- xp gain
	- sound spawn
- Handle player death by removing camera on death state
- debug npc voxel positions - porcupine style
- Refactor Character - Vox model can be underneath them
- Rft: Make GenerateCharacter states and byte type
- CharacterLoadSystem - Just load character from system using load state
	- streaming should just work seperately to character positioning

# Bodies and Animations
- Crouch polish (doc/tech/bones.md, tst crouch, bash bsh/walk.sh)
	- the upper body leans a little ahead crouched (the spine's lean is XR's now and owns the root's place: share it)
	- a smaller collision body crouched (Bounds3D lower, stand up only with room above), to fit under low gaps
	- crouched you don't walk off a ledge (stop at the edge, as sneaking does)
	- first person: the camera takes part of the walk's head bob (3.5cm on a 1m body), a setting for it
- Feel like a AAA game: weight, readable motion, the body reacting to the world (each with a gif test, tst walk_views)
	- starts and stops: a lean into the first step, a settle when stopping, not the walk's blend alone
	- turning: turn in place stepping (the feet planted past an angle), lean into turns walking and running
	- running: its own gait (longer strides, lean ahead, arms bent 90 pumping), not the walk sped up
	- feet on slopes and stairs: each foot's ik to the ground under it, the hips down to the lower foot
	- landing: a squash on the knees by the fall's speed, a stumble past a height; a jump's crouch before it
	- hit reactions: the body flinches from where it was hit (an additive turn of the spine and head, eased off), a
	  stagger on a heavy hit, hit stop (a few frames held) on a landed punch for weight
	- anticipation and follow through on every swing: the body winds up, overshoots, settles (dat/swings.c poses)
	- look at: the head and then the spine turn to what the eyes look at (npcs to the player, the player to its aim)
	- hands on the world: a hand on a door it opens, on a wall it walks by close, on a ledge it climbs
	- idles: breathing (the chest's scale), weight shifts, a look around; each character kind its own
	- death: a ragdoll (the bones to physics bodies, joints limited), or a fall animation into it
	- first person viewmodel: the arms sway with the look's speed (inertia), bob with the steps, a recoil kick
	- the camera: a small shake on hits and landings, a dip on landing, a kick on punching (settings to turn them down)
	- silhouettes readable from far: poses that say what a character's doing (a raised guard, a wind up)
	- additive layers: a breathing, a flinch, an aim on top of the walk, so they mix instead of fighting
- taller characters too <= 3 blocks, 2 wide, etc with bigger doors later for other races
- limbs in general need polish on the organic shape (shoulders poking clothes fixed: tst chest_weights)
-x IK on arms and legs, for more bending (a doc/tec doc first)
- shoulders poking out of shirt, maybe go over limbs again and give it a art pass
- Make equipment and body uis placed based on the body itself
    - dynamically place the slot uis
    - base on skeleton form
    - remember you could be a dog or a 8 armed being, just keep it dynamic
-x Improve head shape, needs to be more humanoid like
	-x it needs a neck part at bottom too
-x Improve chests too, it doesnt really connect the two parts
- Go over parts and slots
    - slots should be on the item itself, like chest can have a shirt slot, a backpack slot etc
    - when we remove foot, it should remove the shoe slot, and be removed from the ui
    - if we had shoe equiped, it should go to inventory or be dropped from the player character into the world
    - only gear can do this, but the parts, you need to remove the end of limbs first
- Tweens: an AnimationEndEvent (a function, like StreamEndEvent) called when one ends, AnimationDeletesTarget becomes
  one handler of it (ui_pop_out passes a delete), anything can react to an animation finishing
- Animate ui for punch, overlay
- Disable Switching Action when one is active
- Global Cooldown When Switching
- When place last block, dont release arm raise until after animation is done
- Base Speed of swing off walk speed and state - WalkSpeed (byte based on max speed)
- Animations: Add Sine Scale on chest for breathing animation
- Animations: Sine Bob + rotate the World Items
- Footstep Prints on ground
- Body / Equip Tooltips
	- Add Slot Names to Body Parts + Equipment in tooltips
	- Add tooltip of slot type over empty slots
- Show unuseable slots as grayed out or something
- Check body slot type before placing body items
	- can we place a hand in arm slot?
- Add Cylinder Shape for Fill Nodes
- Add Neck Node for Headblueprint
- Use Cylinder for Bicep / Forearm Items
- Retype ChunkSize as byte3
- Add Neck to head + chest blueprints
	- cylinder shape at bottom+top
- Designate a hand bone
	- Spawn Held Item onto handbone
- use chest shape data from zoxel for the chest
- handle color combos by making head a different color
	- uses lookup table when placing
- Body Part Tooltip
	- show PartLinks
	- show AttachLinks
- export character, mesh and bones, to blender so i can make animations
	- reimport animation back in

# Physics and Movement
- faster fly speed (check the fly perf tests after, they fly at a set speed)
- dust particles when you land on ground
- add little particles when you walk on dirt
- add rotated character per voxel selection for raycasts, similar to how we do for grass
	- keeping bounds as a sweep pass
- Check when spawning if we are inside blocks
- Raycast x 4, for player, and compare results
	- so we dont get shuffling between cubes when camera shakes
	- like squarecast
- Make raycast from fixed position, not off moving head
- Add particles when you land
- Dust Particles for jumping and landing
- Add dust particles when we jump
- Add standard ground dust particles
- Tumbling: rotational physics from collisions, cubes tip, rock and roll on their own (see doc/tech/physics.md)
	- for cube particles, dropped items, knocked over things, so they land and settle realistically
	- terrain collision pushes out and bounces but never makes spin, Rolling3D only spins like a ball
	- contact point: where the body touches the voxel face (lowest corner / face of the rotated box)
	- push out at an off centre point = torque, spin += (r x impulse) / inertia, as Alpha3D or straight into Omega3D
	- inertia per body: a box's from its Bounds3D and mass (new component, i.e. Inertia3D or Mass), opt in tag (Tumbles3D?)
	- collide the rotated box (its corners), not float4_rotate_bounds' grown AABB, or it bobs as it turns
	- friction at the contact point: sliding turns into rolling, so a sliding cube tips over its front edge
	- rest / sleep: settle flat on a face and stop jittering (also helps the character shake bug above)
	- tests, like terrain_collisions: a cube pushed over an edge falls off rotating, a sliding cube tips,
	  a dropped tilted cube lands flat, a resting cube stays still, all at _10fps / _30fps too

# Combat, Skills and Stats
- add + 5 movement speed when its equiped
	- add movement speed stat
	- add a row of tooltip text on equipment for the stat when you view it
- Damage system refactor: more streamlined (one path from a hit to its damage, its sounds, its effects), with a stress
  test for big npc battles (many npcs fighting at once: damage events, frame time, nothing lost or doubled)
- We can just have a more general Damage/Defence type stats
	- With Physical and Magical tags
	- DamageStat, DefenceStat
- Link Aura to magial damage
- link projectile to magical damage too
- Projectiles get destroyed on hit characters (use same system as pickups)
- add a simple skilltree
	- with a skill to unlock button
	- click and hold it to unlock
	- screen shake and particles when unlock
- start game with 3 skills, so you can try the combat styles of the skill tree
- Refactor Punch mechanic as just the way damage is added
- When low on damage - add more downscale on the camera
	- make it a camera property too
- Add red screen overlay when you are hit
- add destruction particles
- particles for skeleton death
- bleed effect
- When you have stat points, add + button on Attributes
- When attributes go up, increase base stats too!
- Vitality should increase health stat
- When equiping gear, add particles buff around character
- remove StatLinks just use children and queries
- debuffs scale with their skill's level: a debuff links its Skill, its Damage and Rate scaled by that skill's level
  (the realm's burn and poison are the base, skills/fun/on_hit.c)
- Add countdown timer on respawn

# Items and Placing
- a rare item: head2 rare
    - a purple frame in its slot, a "Rare" line in its tooltip, 2D sparkles over its icon (like a shiny)
- placement gizmo for multiple block placement should show more
- a dirt recipe for a bed: 3 dirt along the bottom, 2 on the middle of the sides (a 3x3 grid, 9 slots and an output)
    - taking the bed from the output uses the ingredients
- crafting no implemented, add recipes and fix crafting ui
    - make use of ui modules, if it lacks features, add them so we can implement it
    - a recipe can link to items
    - not sure if flecs can also have say a quantity in its links, like we need 9 dirts to make one super dirt?
- Make placement animate voxels
    - using blocks voxel data (of original vox) spawn in, fade in particles
    - improve placement sound too
- Add 5% chance for chickens to drop them
- Drop item button
- Spawn Vox model items in hand
- Spawn Vox model items in world (flowers drop)
- Add Cookie Item
- Add chicken head item
- Vox Items - hold chicken head in hands
- spawn item model as child of character - when switch action
- Add a timer for new items to be picked up
- item ++ shhould occur after it reaches you (the world item)
- Event for pickup we can latch onto in system
	- trigger the body dirty if body item picked up
- Add different rarity on items and frame them differently
- Remove crafting users reference and use normal item ui for craft ui
- Fix item drop from terrain, so it works in same pipeline as others (sync point reduction)
- Show a gizmo where block would be if holding block - 3D cube gizmo
- Animate particles fade in when placing over time
- Lock in place position from when clicked
	- Slow rotation of camera during lock
- set particle grass color to that of grass itself - block color
- Refactor OverlapEvent to Overlapped state with a system in items

# Quests and Dialogue
- the starting quest (find a town) tracked from the start
- a coin model, given for the find town quest
- a quest to craft a bed, the npc tells you how (the bed recipe, Items)
- player feels no direction on start
	- have an npc target you, and come up to you, and talk to you, when you first spawn
	- you will have a tag for them to find you if they see you
	- they give you a town starter quest
- come back with cookies, still no cookie collector quest
- Dialogue (not high priority): a line too long for its box is cut with ... (text_truncate), show it in parts instead
  (Next shows the rest, the window animating between them), or scale its text to fit
- add dialogue node - conditional node (in nodes)
    - dialogue node choices
    - add some options for replies in the games dialogues
    - expand footer when there is options there
- add collect cookie quest
- add quest reward nodes, give the player a hat
- add + 3 strength and + 2 agility on the hat
- Add rewards on quest handin
	- Goal: Give 20 xp for handing in quest
	- Quest Rewards - More Cookies - Particles
- Collect 5 cookies for quest
	- Quest Reward: Top Hat
- 10% chance for Boney npc to have a quest to give
	- change dialogue if no quest
- Quest accept UI shows between dialogue
- stack tracked quests in a list like fashion
- Track NearbyCharacters on npcs for UI spawning
	- spawn new marker based on surrounding characters quest states
		- link marker to nearby player character
	- marker should set when player goes near npc, it should update based on quest
- set overlays based on quest tracking when we spawn datagrid
- Add a text speed option - 1, 2, 3 (default 2)
- Escape key should exit dialogue
- spawn dialogue inside character spawning for realm, so its in one spot
- Each character type - spawn dialogue there
- Dialogue Generation
	- Greetings
	- based sentence gen on their personality types
	- give each npc a personality type
- improve dialogue contrast, make the dialogue ui block background
- Aim down when speaking to slime/chicken etc
- Fade it in when you get closer to them, like within a chunk distance
- Reduce ai's dialogue distance a little
- Move NPC dialogue distance back 1 units
- Aim head at the npc when talking to them
	- making sure the dialogue is above them
- Black bars to animate from top and bottom when dialogue mode is dirty (remove the fade effect)
- Add little vox model above npc if you can talk to them - little cube that animates
	- Animate it more when talking to the npc
- Hand in Quest Node - linked to two answer
	- Dialogue Nodes: QuestDoneLink - QuestActiveLink

# UI and Huds
- click slider in between to move it there
    - like a normal slider ui
    - same with scrollbars
- ui sccale setting in settings should also set it as you set it
    - needs: a design call first (live resize ends the slider drag, ui_settings_on_dirty sets ui_scale, used by uis spawned after)
      then respawn the scale-dependent uis, or re-layout them, from src/ui/sys/settings.c
    - so you can immediately see results
- add options in the pause ui (return, exit game, make it seccond)
    - add options tests, they can regress, test in game too
- Textures from blueprints (like models): frames, icons and others written as nodes, for reuse
- set the slider text block depth [5] when sliding
- Slider UI to snap on integer points - when integer - on release
- for ui we should keep a Size2 property - real float - this is for the mesh resizing
- Refactor ListPositionDirty and ListSizeDirty into tags
- Make layout positions recursive too - so it updates in a single frame
- Make all ui shaders use transforms like render texture does
-x Refactor the taskbar button to window using links
	-x Fix taskbars again properly linking to windows
- Add Stats panel to the taskbar and give header / window
- Pause UI to use blacked out screen, game ui to just use blur
	- block clicking on other uis
	- close game uis when opened
- Give game uis the realm colors so feels different to the main menu
- Use realm colors for UI
- Set UI hightlight color of buttons, and highlight outline colors
	- have it lerp there over time
- Use a HighlightColor - instead of just adjusting brightness
- Make close button smaller square with a circle pattern
	- the X is ugly there need icon
- Add Glyphs for Key/Buttons on UI
- Loading screen polish
	- with cancel button
	- show loading chunks count
- delete realm Confirm Screen
- rename data_grid stuff to ui_grid if its there
- Add Logs UI at bottom left
	- add a simple log, with the entry line fade out
	- log at bottom left
	- when level up add a line about it
- Add quantity label as child of icon - for mouse pickup too
- Add Tooltips to Empty Part Slot Frames
- Update frame texture outline when icon is selected!
- Selected Text option - update the Glyph Colors
- Add Version of build date to main menu bottom right
-x sort load games by latest played
-x Windows and headers: revamped texture generation, better frames, a bit more style
-x add tab options, sort by general, input, graphics, advanced
-x Add ui scale to settings
-x Refactor Layouts by using LocalPosition2 instead of CanvasLayout
-x Seperate Pause UI from other Game UIs (Taskbar etc)
-x UI: Toast UI when you enter Town
-x Add Character Render Texture UI to show your character - mirror taskbar icon too
-x Status UI
	-x Test button first
	-x Spawn render texture
	-x Spawn render camera facing this
	-x Spawn characters mesh object (mesh clone)
-x Spawn Status UI + player model through the UI
-x Add mirror UI for viewing player model
-x Add fancier frames to the uis
-x Make the bars dissapear when not used
-x Spawn load text / ui during fade out / game event
	-x just show loading for now
-x Redo Realm Select UI
	-x add date in tooltip
	-x Sort latest to top
	-x Add Realm Names as labels instead of seed
	-x Shows play time on Load Game

# 3D UI
-x Make 3D ui use layouts
	-x test the canvas
	-x layout to local position
	-x Position2 on prefab stack gets shifted to Position3
	-x TextSpawnSystem just uses a ZigelPrefabLink instead, handling both types
-x Refactor 3D UI to use layouts
-x Refactor Statbar3D to use Element2D data, merge the ui code
	-x we simply translate the layout2D information into the 3D transform space
-x I really should just link player to a canvas 3D - then i can spawn/remove from that normally

# Map
- markers on the minimap (npcs, quest givers, as above)
- a town marker at the minimap's edge with a town icon, hinting where to go (needs map markers, and edge markers
  for important ones)
- npc markers on the map, with their quest giver states
- Add markers on map uis
	- npcs
		- peaceful npcs
		- npcs with quests
		- hostile npcs
- Add ? and ! markers on the map
	- ? new quest
	- ! hand in quest
- Maps
	- Make maps use GridDirty instead of manually positioning
	-x Move pieces pixel by pixel around player - set centre position of map
	- add a mask around map
	- update ony pieces and not entire map
	- zoom + - buttons on header
	- map texture can be seperate
		- optimized
		- spawn when map spawns
		- child of tunk
		- disable when ui closes / dies
		- updates when enabled / chunk changes
-x Move map UI as player moves
	- mask the map ui
	- or just disable when goes over edge for now
-x Make map move around player position
- Make Camera Z + and remove maps ui FLIP hacks code
- Zoom in / out buttons / scroll wheel
- Keep track of chunks player has explored - fog of war
- Show percentage explored of Region - name the region
- When you open map, fade it in from player, one piece at a time
- show name label over town on map
- Blend in heights with voxel colors to give more of a mappy feel
- Add a compass symbol to show north etc

# Rendering, Lights and Cameras
- Plain gl buffers still kept in globals, remade by gpu_restore hooks: the line instance buffers (lines3), particle
  instance buffers (particles3), the 2D textured square mesh (rendering2); could be mesh entities
- grass meshes: a tall grass block is 18k triangles at full detail (tst vox_instances_stress --gpu --release logs
  it), each stalk voxel its own shade so no faces merge; one colour a stalk would let its sides merge into a few
  quads (asked the user first: it changes the grass's look); greedy meshing plan: doc/tech/chunks3.md
- clouds can be a bit dark
- dust particles in the air round the terrain (ergo proxy, dystopian vibes)
- tst eyelids fails on gl and vulkan alike (2026-11-20): shut, a spot at [0.80 0.50] still lit 37 of 100
- tst coop_pause fails (2026-11-20): gl "resumed, player 2's screen is still dark", vulkan "the shared canvas didn't
  reuse a low id"; the pause fades changed today
- vulkan release crashed once in 5 perf_idle runs at far 8 with ZOX_VK_WORKERS=0 ("Segfault Detected" ~2s after
  the player spawned, 2026-11-20), never in debug: get a backtrace (a release build with -g)
- Vulkan renderer (handover: [tech/vulkan.md](tech/vulkan.md)): every part of [plans/vulkan.md](plans/vulkan.md) in; on a real android phone: rotation, restore
  and particle size (largePoints) untried
- make the walking camera animation feel more natural
    - just animate the head better and body
    - add tremble in the gait
    - think resident evil
- add trail particles like unity engine
- add Rainy Weather
	- rain particles from sky (transparent cubes instanced)
	- when they hit terrain, they melt, flatten animation (y scale) or something better
	- play generated rain drop souds in 3D - has to feel like rain
	- gray clouds, and many more clouds
- Cloud Voxes to spawn and draft through sky
	- transparent material
- Day and night cycle
    - dark at night
    - X minutes for day and night
    - hostile undead npcs spawn at night
- Mirrors: a mirror block or surface shows the world reflected (see [tech/cameras.md](tech/cameras.md))
	- a render camera per mirror, its view the player's camera reflected through the mirror's plane, drawn to the
	  mirror's texture; an oblique near plane at the mirror so nothing behind it draws
	- needs camera draw order (mirror cameras before the main one) and culling masks (the player's own body hidden
	  from its first person camera but seen in mirrors)
	- only render mirrors in view and near, at a lower resolution, skip them when off screen (Disabled)
	- tst: a mirror in front of a coloured cube, the screenshot shows the cube flipped
- Portals: two linked surfaces, each shows the world from the other, walking through one moves you to the other
	- like a mirror: a render camera per portal, the player's camera moved by the portal pair's transform (in one,
	  out the other), clipped at the exit portal's plane
	- crossing: physics moves the character and its velocity through the pair's transform, the camera with it
	  (no frame showing the wrong side)
	- recursion (a portal seen through a portal) limited to a depth or a flat colour past it
	- portals between realms or dungeons later: the other side's chunks streamed around the exit
	- tst: a portal pair, the view through the entry matches a camera at the exit; a character walked through comes
	  out the exit facing the same way relative to it
- Cube lines (cube_line_render_system) and quad lines still draw an entity at a time: instance them too (gizmos)
-x Gpu meshing (doc/plans/gpu_meshing.md): terrain meshes on the gpu, vulkan's default (2026-11-20)
	-x opengl: off by default, it's slower there (perf_terrain far 32: idle -10% fps, far 8 streaming -16% fps,
	  doc/engine/metrics.md); its draw is every chunk's indirect draw from the main thread: fewer triangles (merging
	  faces), or one multi draw indirect per terrain, then default it on (--gpu_meshing to try it)
	-x after: a lost gpu context (android gl) remakes gpu meshes, water on the gpu, vox models and skeletons
- Light character models from the environment (like minivoxes)
	- sample the LightOctree at the character's voxel position (get_block_vox_light in vodes3/fun/lights.c), set Brightness
	- update when the character moves to a new voxel, or its chunk's LightOctreeDirty goes active
	- pass Brightness to the skeleton/character shaders (uniform, like vox_render)
	- tst: character walks from sunlight into a dark area and gets darker
- Lights: Add smooth lights for terrain faces, so the corners are darkened
- Smooth Lighting for terrain lights - smoothed edges
- Add slight directional light over the AO on colored voxes
- Make MeshColorsDirty do similar check to MeshReady - MeshColorsReady
	- fix it up in light3_build_system
- import texture outlines as a seperate mask texture
	- making it easier to change outlines
	- bitmask - 0 / 1 ?
- Use a MaskTexture in the generation data, use this to apply different materials on frames (mask just black and white array)
- Refactor texture generation to use nodegraph + nodeprocess
- remove GenerateTexture after use
- Make GenerateTexture a byte again - so we set per system
- Calculate the camera transform before uploading to shader
	- atm it calculates in the vert shader, not good
- Seperate character depth from block depth in settings
- Lines: Line Instancing
- Add instancing to lines so we can test it easier the lights
- PostProcessing: Add a Post Processor Noise option - Slider - we can reduce it or turn off
- PostProcessing: Add a Post Processor Vignette option - Slider - we can reduce it or turn off
- Fix the fog, make look nicer
- Rendering: Make it easier to add a custom material / shader
	- dynamically gets/adds components per shader file?
- We could have MegaChunks again, 4x4 chunks, and then cull them first with the Frustum + bounds checks (this is just a quicker sweep)
- Move particle and bone rendering systems into rendering module
- Refactor Chunk Render systems back into Rendering module
- Move chunk render back to rendering module, and just link to material
	- keep all rendering stuff there
- Redo all my destruction hooks like MeshGPULink - remove DTOR for OBSR
- Finish converting icons to bmp
- We can even raycast check against chunks to further reduce whats rendered...?
    - test out better chunk culling using raycasts against the terrain
    - Can we hide behind mountains to improve culling?

# Gizmos, Debug and Tools
- A gizmo button in the "gizmos" ui for skeletons:
   - toggle skeleton bones visuals
   - toggle their transparency
   - toggle a face view, to debug the meshes
- add a mode where it shows block ids in item tooltips
- Draw X inside chunk after it builds sides - to debug this
- Tool: Make a region debugger, show the region bounds with a cross through each corner (X) A big X
- Debug Town Gizmo
- Debug House Gizmo
- Player Respawn Point Gizmos
- Render Lines at chunk when action happens
	- using an entity called ChunkBeamDebugger
	- One when it updates the Voxels
	- One when it updates the Lighting
	- One when it updates the Render Depth
- tools (this will help fix bugs)
	- press 'f' while raycasting chunk to select it as debug target
	- this will show the 'chunk ui' which shows all chunk info
- Remove Debug options and just put in Gizmos
	- Transparent Meshes
	- Render Bones
	- Paint Bone Weights (the Paint Weights debug setting, paint_weights)
	- Disable NPCs
- F6 key to toggle bone render debugs
- Make a easy way to add hotkey functionality
	- zox_on_key_pressed(key_z, test_thingo);
- Add a delete all savegames to the test window
- Click SoundData to play the sound
- Profiler should show exact frames
	- a second graph over top can show max system times
- Show max system time's graph, in red, overlay over the regular graph
- Make lines in plotgraph use a set value of the graph
	- set the graph based on max value in that frame, but use multiple curves
	- display frame time behind, and system time in front
- Refactor DoubleDataMax - system timings - use a seperate entity
	- with system link
- Add Terrain Depth to options - debug options

# Editor
- Virtual list scrollbar drag: one walk (it counts the rows in one pass, then walks to the page)
- Virtual list: respawned slots (ListVisible changed, i.e. a dock resize) show empty for a frame before they are
  bound; spawn them hidden until their first fill
- File window grid view: folders and files as icon tiles in a grid (like most file viewers), a toggle with the list
  view it has now
- Game level build defaults: a game sets its own (zedit keeps logs in release), then bsh/zedit.sh goes; zedit
  logs "Game has no skybox" at boot (an editor without a sky: not an error)
- Editor play controls (doc/plans/editor.md): a play / pause toggle and a stop button
- Editor docks: stacking several windows on one side (split it), then an editor option for what a drop on a
  taken side does: swap (the default now) or stack
- Shared ui textures: spawn_image clones its texture per image (clone_texture_data), so every fold arrow, icon and
  item image uploads its own copy. Let images link one texture instead (MeshTextureLink to a source with the
  TextureGPULink), without a per element lookup every frame in element_render_system. Measure gpu buffers and
  frame time before and after, test it (icons, items, fold arrows)
- editor uis need close buttons
- Add a Toggle when entity has children in hierarchy, toggle will remove/add their children uis
- Arrows to fold/unfold children in hierarchy
- Add mask on ScrollArea, so it hides off buttons when they overflow
- Indent the buttons instead of adding "-" to them
- For Edit mode - remove main menu
	- add canvas lines to show
- Dock window - inspector on right, hierarchy on left of canvas
- Set editor text contrast alot higher, black and white
- Click TextureData label to open a texture window
- Click MeshIndicies or MeshVertices label to open a Mesh Window
- Shrink Game Viewport and put Editor Buttons as icon buttons around side, like gizmos - docked
- Add Statistics Overlay
	- Show counts for many things

# Inputs
- We need a target window for player, whichis the last selected one
- Button Mapping
	- Add Button Mapping Module
	- so the player systems use those instead
- Gamepads
	- Tooltip should just be at top right of screen when finger is off ui
	- add controller vibration support
	- Add haptic feedback rumble when hit
	- Add GameMenu RB and LB to switch between UIs
	- Navigation on UIs should also auto scroll down on the options menu
- Touchscreen
	- Add auto jump option - for touchscreen + option to disable
	- make jump button use - ClickingEvent instead of ClickEvent
		- so we can hold jump button
		- Make button activate while holding - on clicked rather than first click
- consider prompts for inputs when character is less than a day old
    - when new character, show prompts for things, based on device
    - this will help new players get used to the buttons
    - e.g. X to Talk at bottom right
- Add option tab: key bindings, so we can bind each key to a new input slot
    - requires input slots feature

# Saves, Realms and Game States
- export / import button for game data (saves, settings), on android too: a new signing key or a reinstall wipes the
  app's data, so players need a way to keep their saves
- save data for characters: only what changed from their base, low cost on disk, a stress test
- Add realm names
    - need ui input field / TextInputs
    - block should block at position of new character
- load names instead of ids in the load realm ui
- sort load realm ui by latest played
- add a cconfirm screen when deleting realm
- characters need io: dirty flagging, performant (thousands of npcs later), everything users can edit (items,
  levels), future proof; a complex problem, a doc/plans doc first
- Save and load the player: its items (inventory, gear, actionbar), stats (levels, health, experience), skills,
  quests and body parts, with the realm, so a continued game has what it had
	- by entity, not ids: save what each item or stat is (its realm item's name or seed), relink on load
	- tst: play, change items and stats, save, continue, everything matches
- save player character slot data
	- items, skills, etc whatever is there
- load player character slot data
- SaveLoad Items - using new Saves Module for better workflow
- Save/Load Realm Blocks + data to future proof updates
- SaveTransformSystem using States
- Save Options to disk from a system - OptionsDirty
- Player State Refactor
	- Remove just use disables etc
- Game States
	- Controller all players centrally from game states
	- single state machine for games
	- Remove GameStateTarget
	- Use a listener event for modules
- remove all realm macros and just use state in their own systems (RealmItemsSpawnSystem, ClearSystem, etc) - wait clear can be a macro tho
- Realms: use node system
- Refactor: load files on boot and not per realm
- Remove Realm Links components, just use Children
- Replace Generate components (GenerateTexture, GenerateRealm, GenerateTunk) with core one
- Add GenerateProgress, and add to it when generate state begins, remove when ends

# Local Coop
- columns left behind a moving streamer aren't removed: only the ones it reaches past the far lod are (corners),
  the one straight behind it stays (terrain_stream_system)
- use second camera only in game for second player / inputs too or whatever

# Engine and Refactors
- Events: StreamEndEvent (and other function events) to observers
- add a features.md with all features by module
	- we can write whats missing / vs game engines
- Thread safety (doc/engine/thread_safety.md, tst pipeline_syncs):
	- 10 merges (from 19), under 10: the transform chain is 5, billboards could rotate inside the hierarchy walk
	- stream_npcs segfaulted once in a full suite run (before the octree barrier), watch for it
- Refactor BlocksDirty to non state
- Add WindowTitle property and spawn Header of window in system
	- decouples the header code from spawning
	- keeps spawning purely for data setting!
- We should not have restore/dispose systems
	- just use query
	- as systems cache their results per frame
- Add event listeners that can handle multiple events
	- use this for game state
	- modules can add too prefab_game's state change listener
	- use this for options
	- can massively reduce systems
- Add delays to queue so its frame timing independent
- Fix logs in build system, should become alot cleaner
- Fix import workflow - make import call
- Make compile shell work per module!
- Engine: Compile modules seperately into .o and then combine, during debug builds - developing
- Look into compiling parts modularly so it compiles faster - important for development speed
- Remove more VoxLink use if its from chunk/tunk/region / use parents
- put more entity component checks everywheres
    - zox_has, safety_checks, catch them during testing

# Devops
- Github actions: more targets in .github/workflows/build.yml (see doc/engine/devops.md)
	- web
	- a versioned release when a v* tag is pushed
- Codeberg: all the builds there (.forgejo/workflows/build.yml builds linux x64 vulkan and windows, copies the rest
  from github), see doc/engine/devops.md Todo

# Platforms
- Make Glut Build work
	- Glut should spawn a window
	- We should wrap all other sdl functions properly for glut

# XR
- Spawn 2 XR Eye entities that are cameras
	- XRRender Systems for them
- XR on vulkan: openxr has it (XR_KHR_vulkan_enable2), our xr code is gl es (egl context, gl swapchain images,
  src/xr/openxr): a vulkan session and its swapchain images as the vulkan module's render targets; android.sh
  --xr --vulkan errors until then
- play test (3D huds in XR):
	- the stats panel, actionbar, minimap spawn in the same spot
		- stats panel: spawn it over your right hand when you look at it for a few seconds, fade out after
	- the actionbar should stay below your head, so you see it when you look down; it didn't follow the player
	- minimap: remove it in XR for now (the map is there), keep it in the normal game
	- the uis sometimes spawn overlapping the taskbar
	- uis spawned in 3D are not always in front of the camera
	- billboards: align them to the camera, not rotated like now; tests for billboards, polish them
	- fix tooltips in 3D (see 3D tooltips look wrong below)
	- last: a 3D mouse option, switching to it when the stick moves it
- UI consolidation (2D and 3D from the same code, doc/tech/uis.md), each removing a 3D-only copy:
	3. one "under the selected ui" tooltip rule: tooltip3_follow_system (3D) and tooltip_navigated_system (2D)
	- check the size after each: bash bsh/tst.sh code_stats (baseline doc/data/code_stats.txt)
- 3D raycasting uis: rays from the hands (the controllers' aim poses, their pointer zevices) at 3D uis (canvas3
	panels, the 3D windows), hovered, clicked and dragged as a mouse's are, a ray drawn from each hand; tooltips on
	hover; then a 3D window's close button
- XR body, what's left (xr/fun/body.c): confirm on a headset
- XR scale: the voxel character is about 1m tall, the player's real arms and height aren't: controllers sit out of
	the character's arm reach (its arms point at them, straight). Scale the tracking space to the character (the
	world feels bigger), or the hands' offsets from the head by its arm length, or the character to the player
- XR turning option in settings: smooth (the default, the right stick turns the body left and right) or snap turns
	(smooth turns sicken some players)
- Punch in VR as a motion (inspired by Sword Art Online): the player's hands always drive the body's arms (xr/fun/hands.c,
	the body has to match the real hands or it disorientates), the punch skill only shows the movement (a ghost of the
	arm's ik path, bones/ik/sys/swing.c, dat/swings.c) and scores the real hand's path against it over its swing time: a close match
	does extra damage

# Tests
- XR (doc/plans/xr.md): rays from the hands at 3D ui, every menu as a 3D list, the player; build src/xr for Android (untouched by a compiler since the cleanup, no NDK on the
	board); on a headset: the projection's flipped y with glFrontFace(GL_CW), and the mixed pose handed to OpenXR
- Test realm: tests make their own data, not the game's (doc/engine/coding_standards.md)
	- a realm of a few hand made blocks (bricks, one plant) and one npc kind, no generation stages: the player's
		character spawned on the test level (_/tst zox_tst_level_setup) still waits ~12s on the game's realm
		generation (biomes, models, items, characters, quests, music; gam/zoxel/sys)
	- terrain_collisions builds its realm by hand already; player tests need autostart's new game to take it
	- then npc_glance and the npc tests spawn the test's own npc, not the realm's generated humanoid
- Void tests (2026-11-22): tests that don't need terrain run with the player's character in the void
	(zox_tst_void_setup, controllers/tst/void_player.c: no game or terrain, the body alone): held_view, guard_pitch,
	punch_motion and free_roam_pose do; move the others that only pose or animate the body, hold items or open uis
	(i.e. arm_reach, eat_swing, held_item, held_block, bone_rebuild, head_camera, the ui and stats tests) to save
	their wait on the terrain
- More tests on the test level (zox_tst_level_setup: flat, one chunk far, no towns, plants, block voxes or
	streamed npcs), or no game at all: npc_attacks, the furniture tests (a ray from a fixed point, not the
	player's), head_camera, the quest and stats ui tests
	- on it now: npc_glance, npc_attacks, npc_leash, head_camera, pickup_pop, the quest, stats and held item tests,
		clothes, bone_rebuild, gizmo_owner, player_respawn
	- not yet: block_vox_hits and furniture_rotation (they want block voxes), quest_dialogue (the app closed)
- add a test for streaming between chunks, this can be reused to test its speed for optmization too
- Test WaveFunctionCollapse
- Spawn NPC - Humanoid -Test
- Write a test function to spawn a world item to pickup
- write pickedup item test
- Test Dialogue function
	- animate scale for each new character 0->1.2->1 (3 frames)
	- remove the glitchy first letter - probably due to it spawning
- go overe uis and add more tests in that module
    - when you add in tabs
    - keep tabs simple but reuseable
- for our model test - change mesh every 5 seconds to a variant model
	- showcase variants
- test for spawning all models under realm
- Simple test function to spawn vox mesh
	- just the mesh itself, usign a entity reference
- New Test: Spawns MeshViewer ui based on inside chunk data
- write up some editor module tests too
    - making sure the data is all updating properly to the ui
- A stress test with 100k terrain chunks, to try fewer states: a byte component (even one generate state in core)
  instead of event tags added and removed. The tags fragment tables (each new table runs ~380 observers, 20k to 40k
  moves a flight) but they let systems skip idle chunks entirely; a byte means every system checks 100k idle
  chunks every frame, so idle frame time may go up. Measure both at the high end (idle frame time with 100k chunks
  loaded, and flying) before changing anything (the user, 2026-11-15; numbers so far in # Optimizations)
- add a table test, for when in game, check total flecs tables, this needs to be reduced to reduce our query times - can add this to tst module in src, it sits on top of engine
- Test Pipeline sync points before i finish
- write a test for binary size
    - are tests shipped with release?
    - done: tst binary_size logs the size, budget via zox_tst_binary_mb (log only until a number is picked)
    - needs: a budget number from the user; check the release build (bsh/build.sh, no --debug) for test code in it
    - try to reduce, propose if any bigger changes first
- Add node and model tests
- --nohud for screenshot tests (hide the hud), and a clear sky view (lift the player, its body hidden: the camera in
  its head sees its hat looking up), then add tst sky_clouds (written, not added)
- lights settle: after a block changes, light propagation finishes within a few frames
- tst body_views into models (src/controllers/tst/body_views.c now, in game round the player): spawn a character in
  the void like model_views (its own realm on the main menu, no terrain, gray, each view filtered to it), any
  character kind (zox_tst_model=<name>), bash bsh/body.sh
	- later for debugging animations: walk, swing, raise an arm, a screenshot a few frames apart (a strip per view)
- tst model_variants: one model from 16 realm seeds, screenshotted in a 4x4 grid of viewports, so similar objects
  can be made to vary more (every realm should feel unique)
	- like tst model_views (src/models/tst/model_views.c): realms generated one after another, a copy of the model's
	  mesh from each, a camera per cell filtered to its copy (CameraFilter, CameraRenderer), gray, --1080p
	- checks every cell drew, logs a variety score (pairs of cells that look different), fails if most look the same
	- bash bsh/variants.sh <name> -> tst/variants/<name>.png, like bsh/model.sh
- write a grid spawn test for every model
- write a grid spawn test for every character
- Add test function for spawning a Terrain Chunk
- Animate Text test
- add a section on readme.md with some razzle dazzle stats from tests
	- add a test that might be impressive and measure engine capacity for this
- write bsh/test.sh that takes in test name
	- it will find the test in the modules tst folders
	- it will compile that file with zoxels src and flecs
	- it outputs a test to run
- Add 2D rendering testss
- reduce time of quests tests - just spawn minimal entities, spawn realm children manually, seperate world etc

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
- greedy meshing for terrain: see Refactors (vox models done)
-x 2300 tables again, try reduce fragmentation
-x Streaming has a spike, make sure to display the biggest system spike per graph
	-x write a test for streaming spikes: perf_stream, perf_fly (flying, --maxnpcs), --profile, ZOX_SAMPLE
	  (doc/engine/commands.md)

# Ideas
- Can we hide behind mountains to improve culling?
- 3D uis: canvas3_render_system gathers and sorts each canvas's elements every frame; keep the order on the canvas,
  rebuilt when its children or layers change (doc/tech/uis.md)

# New
- Part of the [todo](todo.md), done items go in [done.md](done.md)
- New modules and features

# Quadpeds
- add a second skeleton type, thats a four walker, and a wolf npc, so itd need new body parts etc
	- this is an expansion of current body parts and animations
	- we will need a head generated differently to current humanoid ones
	- we should polish the blueprint system too that body parts use
	- it also needs a tail, later we can add more bones per part, but for now itll still use the same simple methods

# Turrets
- Turrets are items
- Can place like a minivox
- They have seperate parts, so different entity
	- base vox model
	- cannon model
- aim cannon at targets
- instanced rendering
- uses projectiles to hit nearby targets

# Crafting
- Fix Crafting and use Slots for them
- Output Craft Slot, pickup only
- Pickup item to change after any of the other slots change, using recipe data to match

# Pets
- Spawn mr penguin on screen
- Give the ability to change his hat
- Make a penguin module
- Adds a penguin to taskbar
- Adds a penguin render scene and 3 buttons to change his hat
- remove any use of user macros from user data
	- just reuse their prefabs and add tags
- use functions like spawn_window, or spawn_user_grid inside those uis
- Spawn label top left above stats - show block / npc selected
- Generate a mouse texture - arrow
- Maximize a window - button on map header
- Resize window grabber at corner
- move sand/wood/stone into biome blocks

# Genetics
- Add dna, array of bytes, per npc
- only for unique npcs
- dna linked in their body parts too
- its used as a seed, so all parts use it
- population is at a region level, it has data for the entire coutn
- npcs can breed and cross dna for their kids

# Affection
- npcs can have affection or hostility for others
- this can effect clan alignment
- it will also increase chance for breeding

# Towns
- villager tags for npcs in towns
- they can have roles, like mayor, 'jobs' module

# Doors
-x add door model, two blocks tall
-x when right click, swing open
- collider to use mini voxels when rotating too there
- door open close sound
- save load open state
	
# 3D UI Refactor
-x Create a simple Canvas with Elements in each corner
-x Use same data but pass through layout3Ds, that use 3D rendering and transform data
-x Test will render the canvas in front of camera at different distances and rotations

# Settings
- Fix No post processing mode / Rawdog Camera Mode
	- add to settings

# Low Priority
-x Make MeshColorsDirty do similar check to MeshReady - MeshColorsReady
    - fix it up in light3_build_system
- Player Respawn Point Gizmos
- Stream Level - Region only for initial game town spawning!
- Tunk LOD Levels
	- So we dont generate maps for far away chunks
- Update frame texture outline when icon is selected!
- Save/Load Realm Blocks + data to future proof updates
- NPC RaycastRange add ActiveAction to NPCs - active_action_range_system
- Long pause breaks physics - test big frame times
- Refactor spawn_block_vox_meta and remove structs
- Add Tooltips to Empty Slot Frames
- Selected Text option - update the Glyph Colors
- Refactor OverlapEvent to Overlapped state with a system in items
- Add instancing to lines so we can test it easier t he lights
- Debug Town Gizmo
- Debug House Gizmo
- Make Glut Build work
	- Glut should spawn a window
	- We should wrap all other sdl functions properly for glut
- tools (this will help fix bugs)
  - press 'f' while raycasting chunk to select it as debug target
    - this will show the 'chunk ui' which shows all chunk info
    - itll have a close button on it
    - Stats
      - Verts
      - States?
      - Idk
      
## Rendering
-x push voxel data and generate mesh on gpu

# Whimsy
-x everyone needs more
- An npc should shift resolutions, when they have a debuff called unknown
- An npc thats from the void dimension
	- when its dying, its death animation is lowering its resolution
- Add npc that dies when you talk to it
	- death node in dialogue
- Add cracked bricks, a weaker version of bricks, 1 / 8 chance to spawn on wall
- Why cant i touch and drag two windows at once???
	- with multiple fingers
- Make a mushroom we can throw on ground
	- does disease damage once it hits an npc
- Create jingle, a bunch of sounds that play in sequence, can be spawned from events like sounds, when a new ui opens etc
- Stress test mode - add a multiplying slime that dies fast

# Future
- Bigger ideas for later
### Alien Languages
- npcs speak their own languages, not english, for the alien world feeling (like hyperlight drifter)
	- the dialogue box is a translator, what they say is translated, not spoken
	- major cities speak a common tongue, the wilds their own languages
- each language its own sounds: a set of phonemes, syllable shapes and rhythm
	- voices already babble syllables (sounds/bop/voice.c, dialogues/fun/voice.c)
	- a language could pick which consonants and vowels it uses, and how syllables are built (cv, cvc, ccv)
	- the babble would follow the language, not the english letters
- the translator could be imperfect: untranslated words shown in glyphs, learning a language translates more

### Multi Chunk Models
- if models grow past what a frame can mesh (a depth 5 mesh is ~5ms): models split into chunks, like Teardown
	- cap a model chunk at depth 4 (16^3), bigger models are several chunks
	- each chunk meshes on its own, so the mesh budget splits a model across frames
	- an edit only rebuilds its chunk, how chunked voxel engines keep rebuilds small
	- bones / skeleton meshes per chunk, lods per chunk

# Zixel
- Add tilemap
	- create mesh based on solids
	- apply textures
- generate mesh using smooth terrain
	- 2D marching cubes
- paint in grass over soils
- Add simple main menu - just play and exit
- Restore material2 when restoring gpu resources
- Make start cube only for zoxel
	- add to boot event
- Fix 2D grid
- Tilemap
	- chunks2
-x Package /gam/zixel/res too
-x when testing - test with local resources too
