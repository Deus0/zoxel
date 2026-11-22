# Done

## 2026-11-22
-x A door shut loaded open: a chunk saved only on a voxel's change (ChunkUpdate), and a door's toggle changes its
  block data alone, so the file kept the door as it was at the chunk's last edit. A block data change marks the chunk
  SaveDirty (vodes/doors terrain_set_block_data), chunk3_save_dirty_system saves it once. tst door_save reads its file
  back after each toggle
-x The wall beside a shut door stayed lit in a dark room: smooth lighting averaged a face's 3 x 3 lights, and a wall's
  side face looking into the door voxel reached past the door's plane to the sunlit outside. A face into a door's part
  is lit flat by the darkest open air across it (lights3/sys/build_smooth.c smooth_seal_light, the gpu mesher's
  seal_at: GpuMeshLighting seals). tst smooth_lights
-x A shut door glowed in a dark room: a block vox took the brightest of its own voxel and its 6 sides, and a shut
  door's voxel is dark, so it took the sunlit outside. One that stops light takes the darkest of its open sides now
  (vodes3/fun/lights.c get_block_vox_light; solid sides, the wall round it, left out). tst door_light checks its
  brightness, doc/tech/lighting.md (new: the lighting rules and passes)
-x Coop: the second player couldn't pick up items. Only the first player's canvas had a held item icon (DataMouse, on
  the mouse), and the click found none. Every player's screen has one now; a player not on the mouse (a navigator)
  has it top right of its screen, not on a pointer it doesn't move (huds/sys/data_mouse.c)
-x Game uis were left open on death, and for a navigator when the game menus closed (Y): only the taskbar went.
  close_game_uis (huds/fun/taskbar.c) closes the taskbar's uis on death and on a navigator's resume, except a hud
  (TaskbarData hud: the action bar). A mouse's stay open on resume, moved out of the way. tst coop_navigator and
  navigator_inventory check the pickup, the icon's corner, and both closes
-x Light leaked into homes with their doors shut, loaded from saves before the door filler (d7832485d): they kept the
  plain filler over each door, and it passed light, the door's top half let the sun in. The plain filler passes light
  as its base does now (block_light_pass_base): over a shut door it stops it, over a bed or lamp it passes as before
  (lights3/fun/solidity.c light_stops, its base read from its block data; meshing still sees it as air).
  A room shut round lit air (a door shut, a roof placed) kept its light: the dark pass queued refills from lights
  beside it at their light then, a beam beside it went dark after and its stale light relit the room (223 inside a
  sealed room). Refills start at darklight, the flood takes the voxel's light when it runs (lights3/sys/dark.c).
  tst door_old_filler, door_save (a room in the void, shut, saved and loaded: dark)
-x Oil: a liquid as water is (flows, swum in, light dims in it), dark, its own block material and mesh with its own
  texture. Liquids are families now (WaterLevels, LiquidLink): oil spreads as oil, water as water. A liquid mesh
  takes its texture from the first block of its material that has one (a flowing block could come first and leave
  it without). Not generated in the world yet. tst oil_flow, doc/tech/water.md (Oil)
-x The options menu has tabs: Video, Sound, World, Game and Debug, a strip of buttons over its list, a tab shown at a
  time, sliders first then toggles, each by name. Each setting is tagged with its tab where its module spawns it
  (SettingTab*, doc/tech/settings.md). The XR menu (huds/ins/settings3.c) still lists them all. tst options_tabs
-x Faces where lods meet are worked out against what the neighbour draws (chunks3/sys/sides.c chunk_drawn_depth: its
  RenderDepth, its voxels can be deeper, NodeDepth only grows), and rebuilt when its lod changes (terrain/sys/stream.c);
  they were against its deeper voxels, and not rebuilt: 2859 faces held that a neighbour hid, 5 now. A mesh rebuilds
  after its sides even when it seemed built already (a deferred BuildMesh removal hid the request). The missing faces
  seen walking about didn't show in tst stream_sides on master (its stream, no player): it checks every chunk's faces
  at the depth it draws as the stream steps, and its neighbour links. Streaming unchanged (perf_stream 0.23s a step)
-x Light sources relight, and doors stop light while shut. A covered hole's dark (the sun's, or a removed lamp's)
  flooded a room and took its glow stone's light, and nothing lit it back: the dark pass lights an emitter it meets
  again (lights3/sys/dark.c dark_relight_emitter). A loaded chunk's emitters were never lit (only edits and towns
  lit them): a chunk's emitters light once its voxels are in, generated or loaded (GenerateEmitterLight,
  lights3/sys/emitters.c), towns no longer seed their own. A door and its own filler (block_light_pass_open, its
  filler picked by block_filler_voxel) stop light unless their block data is open; toggling one relights or darkens
  it (lights3/fun/openings.c). Streaming unchanged (perf_stream settles 0.23s a step before and after). tst
  glow_relight, door_light
-x Town walls curve (fun/town.c town_shape): a rounded rect from the town's seed (a superellipse) pulled in by smooth
  noise, its band, edges, spikes and gate from it; homes, roads, lamps and the gate keep inside it; highways link a
  town's gate from its rect's edge, over a mountain if no line is clear. The stream spawns the regions round each
  region it streams (highways look into them, a tunk waits on one coming, not on one 2 away). A more organic shape
  next (todo.md)

## 2026-11-21
-x A map tool: `bash bsh/tools/maps.sh` runs the game with `--maps` (towns/dbg/maps_dump.c): a world of its own, no
  player, a stream point in the middle of region 0, 0, far 68 (the circle reaches the 9 regions' corners), tunks
  only (zox_terrain_no_chunks; --chunks for the top blocks map), and writes tst/maps, a pixel a block: height, biome,
  vegetation, town, lod, overview (heights, towns, highways, region borders). 17s, the same seed the same images
  byte for byte. A* kept as a tool (towns/fun/astar.c, its checks in town_build). Found with it: doors boxed in by
  neighbours had no town road (each home's corners are waypoints now), a town's highway link looks 2 lattice cells
  round; coop_homes allows players sharing a home's door to stand 2 blocks apart (towns of one home)
-x Roads and highways go round things, cheaply. Highways are a lattice over the world (towns/fun/highways.c): a node
  jittered every 128 blocks from the terrain's seed, linking east or north (some both), nodes and links near towns and mountains
  left out, each town's gate linked to its nearest node; a tunk works out the links crossing it from the nodes round
  it (0.04ms), no step, nothing stored, no region borders to agree on; level between their ends' heights, a
  causeway over water. Town roads are a waypoint graph inside its walls (a point every 8 blocks, none in homes,
  linked where a line misses every home), one shortest-path tree from the square to the gate and each door,
  straightened (0.04 to 0.17ms a town, a GenerateTown step on a worker); on the ground as it is, homes' steps keep
  theirs. The A* over sampled terrain before it cost 1 to 19ms (metrics.md). Fixed on the way: homes spawned twice
  while their town waited on a later step, homes were placed into walls up to 12 thick. tst highways, town_homes
  (roads round homes), town_build (a town's roads round a home, highway nodes)
-x Towns have street lamps, homes doors and beds, regions highways. The lamp is a 1 x 3 x 1 block (vox_type_lamp, a
  4 block model cube, BlockLamp) beside town roads every 16 blocks and at the square's corners, its head lighting the
  street. Homes' doorways are a block wide and two tall with a door in them, a bed in the back corner (two in wide
  homes), written with their block data like placed ones (towns/fun/build.c pieces). Highways join each region's
  towns gate to gate (nearest first) and run to one crossing on each border, from the terrain's seed and the border,
  so neighbours meet; 5 wide, paved at the sea's top over water. The block ray sized a 3 tall model's cube as 3
  blocks, now the power of two (4). tst town_furniture, town_build's new checks
-x A placed street lamp lights from its head, not its base, and a removed one darkens from there: a tall emitter's
  light comes from the block's BlockLightOffset (byte3, turned with it; the lamp's 0 2 0, lights3 update_light_emission), the chunk over it when its head is
  there. tst lamp_light (lit from the base, over its head was three steps of decay down)
-x The core Dirty is an event: non fragmenting, observers, a cleanup observer removes it; Refresh / Update are
  the fragmenting pair for systems (texts, grids, the settings' save). The stat and item Dirty writes are back
  (events uis can use), quests react on Dirty alone (the Update twin gone). perf_stream and perf_fly no slower.
  doc/plans/core_dirty.md, states.md
-x Slime's tentacle not drawn (android vulkan, and desktop too): particles became a vox voxel in size, its 78
  dots 1 to 3 pixels, and faded over its life. Particles have a size in voxels (Scale1, an instance attribute), the
  tentacle's 8 at its body to 4 at its tip, no fade; tst slime_tentacle checks its green is drawn
-x Core Dirty audit (doc/plans/core_dirty.md): stats and items stop adding an unread Dirty (13 writes, the reader
  was dead code), sounds play from observers on Dirty (3 systems now observers). Quests keep Update (a quest just
  given links its tracker a frame later), texts and grids keep systems; a non fragmenting Dirty waits on texts
-x Dirty pairs 6 to 3: LayoutPosition removed (nothing read it, 21 adds moved tables), grids on the core Dirty /
  Update, InsideBlock a non fragmenting tag and observers (on Update it fired once a walk through grass). Profiled
  each (states.md), dead copies of 5 systems gone
-x Stats count observers too: lint_systems prints [n] observers (registrations in the source), pipeline_syncs the
  observers registered at run time; states.md's refactor log counts both (the day: 672 systems and observers to 662)
-x Ui events Click, Deselect, Press and RightClick are one non fragmenting tag each: the Trigger tags and their 8
  swap and remove systems gone, the 6 readers observers, a cleanup observer removes each (14 systems fewer, 10 observers more: 4 fewer together; 4 tags fewer).
  Select, Begin, End, Exit keep their pairs (a system reader that needs them). Before and after in states.md
-x Light events are non fragmenting tags and observers (measured cheapest, no table moves): InspectorDirty,
  HierarchyUIDirty, GlyphSpawnedDirty, AnimateTextEnded, ActiveStateDirty; 6 systems gone (4 of them main thread in
  zoxp_spawn; lint 586 to 580). The editor's windows add theirs at spawn, after their target (on the prefab, the
  observer fired before it). Docs: architecture.md, coding standards, states.md
-x Three dirty pairs are one tag each with an observer (the coding standards' one-off event): Attach, WindowSize and
  List, their 6 remover systems and 3 readers' systems gone (lint 589 to 586). Grid stays a pair: as an observer it
  laid icons out before their sizes settled (tst navigator_inventory). perf_fly couldn't tell them apart. The web
  build set WindowSizeDirty as a byte, fixed. doc/tech/states.md
-x More byte states gone: TriggerSound is the core Dirty/Update on a sound (players match Update, tst sound_trigger
  checks one reaches the mixer); ActiveStateDirty is a tag (elements get the core Dirty for texts), the link system
  removes it after the outline one; Bounds3Dirty and bounds3_enable_system gone (grow_bounds3 lets it move)
- kept as bytes: GameStateDirty (tags made the game clean up before the players saw the end: its 3 frame guard
  matters), PlayerStateDirty, Generate, GenerateCharacter, the action timers, PickedUp, StreamDirty, the render ones
-x Byte states to tags (one reader, set then read once): GenerateSound, ProcessSound, CombineVox, InspectorDirty,
  HierarchyUIDirty, GlyphSpawnedDirty, AnimateTextEnded are tags their system matches and removes; their 7 increment
  systems are gone; ItemDirty (never set) removed
-x Settings apply on Dirty, not Update: a slider dragged reached the game only on its first frame (Update stays on while
  dirtied again, its observer didn't fire again). Every settings observer (15) is on Dirty now. Tests settings_spawn,
  settings_drag, settings_load; doc/tech/settings.md
-x Tooltips: a ui deselected closes its tooltip in an observer on Deselect (was a system every frame), dead code gone;
  the select stays a system (as an observer a fast flick lost the last icon's text). refactors.md: the systems style
  and CanvasData were already done
-x Settings are events: the last 8 modules' settings systems (models, npcs, rendering, sounds, streaming, ui, vodes,
  xr) spawn their settings on the app's InitializeEvent and apply a change in an observer on Update (as ai, apps,
  bones and controllers did), 15 systems gone; SettingDirty (a byte state and its increment system) is gone, the save
  runs on Update, one write for a frame's changes. tst settings_apply changes one of each and checks the file
-x Logs minimal by default: boot paths, settings, terrain sizes, autostart, the skybox, splashes and a missing optional
  folder log only with --verbose (zox_logv, gated at run time now, it was compiled out unless zox_verbose); --verbose
  is read first thing in main. A minute of play logs ~10 lines. The rule is in coding_standards.md (Logs)
-x No compiler warnings in debug builds, with or without logs (~230 before): unused variables and fields removed, logs
  in builds without them keep their arguments used (never run), truncations in tests sized
-x The 16 printable ascii without zigels (' " # $ & ( ) * ; @ ^ ` { | } ~) have them, from the ttf font: dialogue's
  "I'm" showed "I?m". tst text_data checks every printable one round trips
-x A deadlock in game (tst stream_npcs hung): npc sight held a chunk's entities lock while its lines locked octrees,
  the crosshair raycast held an octree while it took that entities lock. A merge between the two systems kept them
  apart until the huds' main thread systems left pre_physics. Sight copies the chunk's players out, lets the lock go,
  then checks its lines
-x tst pipeline_syncs: 13 merges (over 11). The huds' main thread systems (load_lids, navigator_shift, huds3_follow,
  huds3_options_stick) sat in pre_physics between multi threaded ones (2 merges); they're in zoxp_spawn with the rest.
  There no merge comes between the input extracts and navigator_shift, so the inputs it reads write in place (as
  gamepad_extract_system did): the xr controllers (zox_setm, and read in zoxp_begin, xr is imported after huds) and
  the tests' pad (zoxp_begin). Deferred, a grip's press reached it a frame late, already reset
-x tst camera_filter: the filtering was right (C drew only the right cube), the check counted any changed pixel and
  C's view has film grain; it looks for the cube's red now
-x tst lint_systems, sky_clouds, dig_light, xr_frustum pass again (3 of 3 runs each), fixed by earlier work
-x Web: the camera spun after alt tabbing back (a 3 year old bug): the browser drops its pointer lock, sdl only asks
  again on a click, and till then gives where the cursor is; the warp to the centre does nothing there, so each frame
  read the cursor's distance from the centre as movement. Locked but not locked by the browser, the mouse doesn't
  move the camera (web_pointer_locked), a click locks it again
-x Web: mouse too fast windowed: the canvas was stretched by css but its pixels weren't the page's, so movement was
  scaled; it fills the page now (SDL_WINDOW_FILL_DOCUMENT), its pixels follow the page's size
-x Web: no save games: saves were in HOME, memory only (lost on a reload) and missing its folders. They're in /saves,
  the browser's IndexedDB (IDBFS, src/_/platforms/web/pre.js), loaded before main and written back every 2 seconds
-x Web builds on windows: web.sh finds or installs python (winget, user scope) for emscripten, only for web builds;
  --logs for a release build that logs (the debug build is too slow to load a game in a browser)
-x Web: characters had no stats (no damage, regen or levels), "the realm has invalid stats". The realm's stat colours
  saturate by 1.4, past 100, so hsv_to_color_rgb made a channel below 0 and its float to byte was undefined: gcc
  wrapped it (a slightly wrong colour), clang (emscripten, -O3) folded the constant colours, saw it, and dropped the
  rest of stats_realm_spawn_system as unreachable, so the realm never got its stats. Channels clamp to a byte,
  saturation to 100, a grey's hue is 0 (it was NaN). tst colors_hsv. Found by the system's strings missing from the
  wasm; the error says what the realm has now (children, stats, which of the four)
-x Windows, nvidia: a crash in about 1 of 3 game starts, an access violation in the driver's own thread
  (nvoglv64.dll +0xb8bf10, reading [0x1]). A chunk remeshed while its lights built (MeshDirty, BuildMeshColors) kept
  drawing its old mesh, but mesh_colors_gpu_system uploaded its new colors alone (MeshColorsDirty from the light
  before): fewer colors than the old vertices (none for an emptied chunk), and nvidia, converting rgb8 vertex colors
  on the cpu, read them from null. Colors alone wait for a mesh's new geometry now (!MeshDirty), the full upload
  sends both. tst mesh_colors_wait. Found with zox_gl_sync=1 (src/_/platforms/tst/gl_thread.c: a glFinish before
  each gl call, the crash logs the gl calls before it); dev builds also log gl off the main thread and buffers bound
  after they were deleted (gl_buffers.c)

## 2026-11-20
-x Sdl3 windows: back from fullscreen the window disappeared, off every display at -32760x16537 at the fullscreen size.
  get_monitor_bounds called SDL_GetDisplayBounds with sdl2's index and success (0): sdl3 takes a display id and
  returns true, so the failure read as success and the window went to garbage bounds (zox_sdl_get_display_bounds per
  sdl). Windowed again it's as the app was, maximized or its restore size (sdl3 left it unmaximized and screen size).
  zox_app_set_monitor centred the wrong way. tst sdl_window_mode, sdl_window_restore (--visible to see it)
-x Windows release builds don't open a terminal beside the game (-mwindows, a gui program); debug and --logs keep it
-x The file system the same on every platform (src/_/io): zox_mkdir, zox_mkdirs, zox_copy_file,
  zox_copy_directory, zox_executable_path. mkdir had an #ifdef at each call (a mode on linux, none on windows) and
  tests ran mkdir -p, rm -rf and cp through system(), which is cmd on windows: load_games, continue_game and four
  more were posix only, realm_info and binary_size (/proc/self/exe) failed there
-x Windows test runs shared one nosaves folder beside the game, so each test saw the ones before's saves: it's
  %TEMP%/zoxel_nosaves_<pid> now (git bash's /tmp), tst.sh deletes it after (pid_alive, tasklist for windows pids)
-x One pause menu over the whole window for every player (huds/fun/pause_canvas.c): a shared canvas with its own ui
  camera, spawned as the game pauses; each player links it too (CanvasLink is many now), so any gamepad navigates it
  and the mouse clicks it; spawn_window_list_on puts a window on a canvas. With 3D huds (a headset) it stays on the
  player's. tst coop_pause
-x Npc ai on the realm clock: wandering, random jumps, attack beats (AttackAt) and combat time (LastCombatTime) by
  ClockDelta and ClockTime (clock_time_of): paused they hold; tst realm_pause checks the npcs don't move
-x The game menus (Y, R; a touch button) apart from pause (Start, Escape): the game goes on in a player's own menus,
  the first one opens for a gamepad or headset; the pause menu isn't a taskbar ui any more, its close resumes.
  tst navigator, navigator_inventory, xr_navigator (Y on the simulator) rewritten for it
-x Pause stops the game: a realm clock (doc/plans/realm_clock.md), ClockTime, ClockDelta, ClockScale and ClockLink,
  the realm's scale 0 paused; terrains, characters and projectiles follow it, physics and collisions move by it.
  tst realm_pause
-x Npc hits didn't land on the player: an npc's ray pointed along its body's facing, which lagged a moving player, so
  every punch missed (tst npc_attacks, now with the player strafing). It aims at its attacker (RaycastAim)
-x Start then RB on a gamepad crashed: the Character window's render texture read its new shader's gpu ids back while
  deferred (null); they're passed in. Its window was a global, a second coop player's open closed the first's
-x Npc combat: humanoids punch and shoot (picked by distance, SkillReach), swing in reach on an uneven beat, back off
  and lunge, flee hurt; monsters attack who they see (npc sight, Target); slimes pounce (squash and leap) and whip a
  tentacle; shots burn (the realm's burn, OnHitLinks), shots trail and burns flame (particle streamers). Shots were
  1.4 a second (a one frame force), now launched at their speed. tsts npc_skill_switch, npc_flee_hurt, monster_sight,
  slime_pounce, slime_tentacle, shoot_burn, aura_poison
-x Game log (huds/ins/game_log.c): game_log(world, player, text) puts a line at the bottom left of that player's
  screen over the actionbar, older lines pushed up, each fading out after 6s, 6 at most. Quests done and their rewards
  log there (handed in by dialogue had nothing). tst game_log
-x The held item sat on the wrist since bones sit at joints: it's off the hand bone by its part's middle (BoneCentre -
  BonePosition); tst held_item measured from the bone's pivot, now from the hand's middle
-x The quest tracker kept a quest's text from the frame it was given (???, tst quest_dialogue failed): track_quest
  unlinked the old QuestTracked then linked the new, both deferred on an exclusive non fragmenting link, leaving
  neither; and the Dirty observer ran before its tracker link was there. It relinks without unlinking (exclusive),
  and redoes the text on Update too; a tracker spawning dirties its quest
-x Doors facing their first way (direction 0) never swung: an unturned block vox has no Rotation3D, door_swing_system
  needs it; their block data opened (passable), they stayed shut. door_on_spawned sets it. tst door_rays (two doors
  side by side, rays across the first's face find it, closed and open); tst furniture_hits aims at the panel now
-x A gamepad plugged back in didn't reach a lone player who'd used the keyboard since (it stayed the app's, A or
  start did nothing): reclaim_gamepads gives it to them, disabled, a press switches to it. tst gamepad_reconnect_keyboard
-x The ik through npcs' heads: the guard looking up (2026-11-19, tst guard_pitch)
-x Global shaders to entities: the lines' (2D, 3D, 3D instanced) and particles' shader and material are entities
  (spawn_material_source), their uniform locations components (MaterialLine2D, MaterialLine3D,
  MaterialLine3DInstanced, MaterialParticle3D) relocated on a restore; removed the 2D square instance material
  (instance_render2_system was never registered)
-x A gpu restore looks up the materials' uniform locations again (zox_gpu_relocate), they were the old program's;
  tst gpu_restore scrambles material_basic3D's and checks they come back. opengl_mode is in graphics now (dat/opengl_mode.c)
-x Opengl es (android, tests) logged GL_INVALID_ENUM every frame: GL_POLYGON_OFFSET_LINE and GL_PROGRAM_POINT_SIZE
  aren't in es, skipped there (zox_gpu_is_es)
-x At boot a render texture logged "Invalid [TextureGPULink]": it spawns PreInitialize, its texture is made the frame
  after, render_texture_initialize_system skips PreInitialize too
-x The instance ubo's block is bound explicitly (glUniformBlockBinding, binding 0, both vox instance materials), and
  again after a restore relinks them; it only worked as InstanceMatrices happened to be block 0
-x Removed unused ComputeShaderLink and MaterialInstancedGPULink
-x Deleting a disabled entity (a hidden lod mesh) leaked its gpu buffers and textures: the gpu links' OnRemove
  observers skipped disabled entities, zoxd_dest uses zox_observe_disabled now (EcsQueryMatchDisabled), tst gpu_links
-x Gpu restore and dispose are functions, not systems: each module adds a gpu_dispose / gpu_restore hook that walks
  its gpu links (zox_each_begin), disabled entities too, not prefabs; a restore makes again only what the dispose
  let go of (making the rest leaked); it also restores the lines, particles and 2D square materials, bone index
  buffers and the instance ubo (all stayed dead after android's lost context); the sky no longer sets colours on a
  disposed material; live counts for shaders, programs, fbos and rbos; tst gpu_restore checks them, tst
  gpu_restore_game (in a game), tst gpu_links (bare entities)

## 2026-11-19
-x Split screen: one player in a dialogue or asleep darkened every screen: the game views shared one render texture
  material, its vignette, blur and lids uniforms the last camera's. Each joined player's view has its own material
  (spawn_material_render_texture_from, the shader shared); its shader was parented to a gpu id, now its material
-x A lighter dialogue vignette (dialogue_vignette 3.1 to 1.8)
-x Local coop: a player's device only switches on its disconnect screen (DeviceLost, device_switch_coop): the
  keyboard or a spare gamepad took the first player off its gamepad
-x Doors hang on their block's front edge (build_vox_door, door_hinge_z), swinging from the corner into their block,
  not from its middle
-x The guard looking up: the hand went over and behind the head, out of view, the arm locked straight and spun half
  round. Looking up it's posed level then turned up about the eyes, kept in its level reach (in along its line from
  the eyes); the shoulder's ik turn fixes the arm's twist too (ik_turn_to: the elbow to the pole, not the shortest
  turn). tst guard_pitch. In a dialogue the head looks up at the speaker, the arm rose with it
-x Crouching: hold left control, or a gamepad's left stick pushed in toggles it (controllers3/sys/movement.c,
  CrouchState on the skeleton): the thighs tip ahead 55 degrees, the knees twice it, the feet level, the body down by
  what the legs lost (the head and a first person camera 17cm lower), slower (0.45 of the walk), no running; the body's
  bob now from each leg's real height (thigh and knee angles, its bone lengths), the feet within 2cm crouched or not.
  tst crouch (L3 on a test gamepad), tst walk_views records walk_views/crouch.gif; a headset crouches by the body
-x XR: looking down the eyes went into the head (the user: the head through the body crouching or looking down): the
  head bone turned by the headset on top of the chest's lean (its parent), and the look's bend (0.95 radians) sank the
  body's neck 9.5cm under the eyes; the head's turn takes the lean off (xr_head_bone_system), the bend is 0.3 (the head
  turns at its neck now, the eyes swing out ahead of the chest by themselves). tst xr_look_down crouches looking down
  too; tst/eyes.c checks the eyes against every part, the head's and hat's too, each frame of xr_look_down and xr_lean
  (2.2cm clear at worst), and screenshots what they see (tst/xr_eyes/)
-x The off hand in a punch (OffShoulderBoneLink, swings' off_back / off_out, ik/sys/swing.c): the left fist comes up with
  it, out ahead as the right winds up, pulled back to the chin as it lands (the recoil), up a moment after; tst
  punch_views checks it
-x The walk's gait (bones/sys/arm_control.c, arm_swing.c, tst walk_views, bash bsh/walk.sh): steps from the speed over
  the stride (the feet skated at a fixed pace), strides 25 degrees (60 scissored) and arms 28 (110 swung them up level),
  the swinging leg's knee lifts its foot, elbows soft; the body bobs over its stance foot (L (1 - cos) of its angle), so
  that foot holds within 1cm (it rose and sank 16cm) and the head bobs 3.5cm in step (its random oscillation is gone);
  the limbs are posed by arm_control with their skeleton (as systems of their own, split across workers, a leg read
  the phase a frame late); RestPosition3D is a bones component now
-x The bones debug view (Bones in the Gizmos debug ui, --bones): bone gizmos over the skeleton meshes see through and
  painted by bone; the boxes turn with their bones round their parts' middles, in their bone's paint; the Bone Gizmos
  setting never applied (its handler matched "Bones Gizmos")
-x Bones turn at their joints, not their parts' middles (part_bone_pivot, parts/sys/bones.c): a bent elbow pulled the
  forearm a gap off the upper arm, the skin between stretched into strips (tst punch_views); legs swing from the hips,
  the head turns at the neck. BoneCentre keeps the part's middle (the head camera, look_point, the mugshot, the gizmo)
-x A gif recorder (src/graphics/fun/gif.c): zox_gif_record(path, fps), zox_gif_stop(), frames taken before the
  swap (zox_gpu_read_screen, which screenshots use too); tst punch_views and bash bsh/punch.sh record the punch from 4
  angles
-x The raised guard held the arm up straight (0.95 of its reach, a salute from outside): a fighting guard now, the
  fist ahead of the chin, the elbow bent (0.8); eating brings the hand nearer the mouth (tst swing_motion measured it
  against the guard)
-x A castbar above the actionbar: fills over a cast (an action's warmup) with its name, hidden after; instant skills
  tagged SkillInstant, skill tooltips say instant or cast [time]; tst castbar
-x A long frame skipped swing marks (found from the frame's capped delta): bites and hits lost, a lost first bite ate
  nothing; marks fire in order, each once; tst bite_hitch
-x Beds: climbing on and lying down, getting up and off, eased (SleepMotion, sleep_motion_system after the physics):
  up onto the mattress with a hop, then onto its back; up onto its feet, then down beside the bed, moving again once
  off; held lying while asleep; tst bed_sleep checks the climb never jumps and stands on the mattress half way
-x Sleeping in a bed (vodes/beds): a right click on one (block_activate_system) puts its character to sleep (Sleeping,
  SleepBlock): lying on its back over the mattress, its head on the pillow, still (DisableMovement, DisableGravity,
  NoClip, its velocity and forces zeroed); a player's eyes close (CameraLids), its mouse let go, an "Asleep" window
  with "Wake"; waking (Waking, a one-off event) stands it beside the bed (ground under it first), the eyes open;
  multi_block_base_at finds a sized block's base from any part; tst bed_sleep, block_activate clicks a bed
-x Eating forever (a bug from d890c5569): at full health the eat swing played on every press and ate nothing (the
  swing started at the warmup, the eat refused a full stat). The press checks it can eat first (911eb2f63); tst
  eat_swing presses at full health (no swing, none eaten) and eats the stack until it's gone
-x Loading closes the eyes by each player's camera's CameraLids, in 2D too (load_lids_system, was the 3D huds'
  huds3_darkness global and 2D's canvas fades); the "Loading" label over them; it lets go once open (load_lids) so
  sleeping has its own; removed the unused fog darkening (render_darkness); tst load_lids
-x Swings as one table (bones/dat/swings.c): each kind's poses, its strike's ease and its marks (events); played by
  bones/ik/sys/swing.c (was ik/sys/punch.c); zox_ease_in_out, zox_ease_between in animations
-x A swing's own time on skills and items (SwingTime, attack speed), separate from the cooldown; the melee tooltip
  shows damage, swing and cooldown
-x Placing a block and eating are instant: the press starts the swing, the block placed at its hit (items/eve/place.c),
  the food eaten at its first bite; tst place_hit, eat_swing
-x items/consumables submodule (stat, food spawn, eating); consumables set their timings then had them reset (0.5s)
-x Eyelids: CameraLids (0 open to 1 shut) on cameras closes the view from the top and the bottom, the middle first
  (the headset's lid curve), in the render texture shaders; the headset's eyes close by it too (with loading's
  huds3_darkness); tst eyelids (screenshots open, part shut and shut)
-x One right click path for blocks: block_activate_system (vodes/activations, multithreaded) uses the block by its kind,
  doors moved onto it from their OnSet observer; a door's sound is a one-off event (DoorSound) as a worker can't
  spawn; tst block_activate (a raycaster clicking a door in and out of reach)
-x The punch is instant: no cast (warmup 0), the press starts its swing, and it only damages at the swing's hit mark
  (melee_hit, an observer: skills/eve/melee_hit.c), what's aimed at then; the windup is animation. The skeleton
  links what's swung (SwingAction), eating's crumbs read the food from it. tst punch_wind_up (no hit before the mark,
  one ~10 ms after); doc/plans/skill_timing.md
-x Animation events: a swing passing its marks (a punch's hit, a place's set, eating's bites) sets SwingMark on the
  skeleton (bones/sys/swing_mark.c), observers react: eating's munch and crumbs are one (actions/eve/eat.c, was a
  system on the food's cooldown)
-x Particles from a vox: spawn_particle3_burst (particles3/fun/burst.c), each a colour of its palette; crumbs use it
-x The swing starters (melee_swing_system, item_swing_system) ran on the main thread between threaded ones in update
  (4 more merges, tst pipeline_syncs): in zoxp_spawn with the other main thread systems now
-x Eating chews: three bites while the hand's at the mouth (over the food's cooldown), each a crunchy munch and
  crumbs in the food's colours thrown out ahead of the face (items/sys/chew.c); tst eat_swing counts the bites and
  saves tst/eat_swing.ppm mid chew. The eating hand sits a little lower and further out (it filled the view)
-x Particles close to the camera drew huge (their size over their distance): no bigger than at one unit away
  (particle3D.vert)
-x Doors open and close: a right click on one (TriggerActionB, on_door_activate) toggles block_data_open in its
  parts' block data (saved with the chunk), its block vox swings a quarter turn about its hinge (door_swing_system),
  spawned already swung when loaded, a creak opening and a thump with a latch closing (instruments creak, shut),
  terrain collisions pass through an open door's parts (intersect2.c); tst doors (vodes/doors)
-x Eating swings the hand to the mouth (zox_swing_eat), from the item's warmup (item_swing_system, blocks and food);
  just under the first person view, in front of the face; tst swing_motion (was place_motion), eat_swing
-x Placing a block the head turns part way to it (SwingTarget, the ray's hit; head_look, head_place_nudge), not a
  first person head (its camera aims there already); tst place_glance
-x Placing a block swings as a place, not a punch: the block lifted up by the head, then pressed out where you look
  (SwingKind on the skeleton, zox_swing_place), from the item's warmup (item_place_swing_system, skeleton_swing_begin);
  tst place_motion
-x The punch turns the body: away winding up, into the strike (SpineTwist, spine_twist_system); the hips and head turn
  back, so the legs and the first person view hold still (head_look keeps the twist off); arm ik reads the chest's
  turn this frame (a frame behind, the hand missed its target); tst punch_wind_up checks both
-x A punch looked like a pat: its swing started at the hit (after the warmup), so it pulled back after contact. A
  melee skill's warmup starts the swing now (melee_swing_system), the fist winds back low by the ribs, snaps out
  (eased out) to the arm's full reach as the warmup ends and it hits, holds, then recovers (bones/ik/sys/punch.c);
  tst punch_wind_up
-x Towns stood in the sea or a lake (homes over the water, air cleared under the sea): a town's place is tried again
  if any of its ground (every 4 blocks) is under the sea or a lake (terrain_rect_over_water), lakes from the cells
  round a region kept out too; tst sea_level checks no town air under the sea, lakes no lake columns in towns
  (seeds 1, 2, 5, 666); sea_level's water over the sea leaves lakes out (seed 5 counted a lake's)
-x A square patch of water lighter than the rest (seed 666, tst/sea.ppm): gone, the sunbeams lit chunks still
  generating (fixed 2026-11-18)
-x Lighting flickered destroying a block (water block refactor): fixed, not seen since (tst dig_light)
-x Under water the blue tint lighter: it multiplied the dark down too much
-x Light flooding into water dims more: 24 a block over the air's 32 (light_flood_decay), tst water_light
-x The gpu mesher draws flowing water at its level too (gpu_mesher_set_heights), tst gpu_mesh
-x Under water the view is tinted deep blue (CameraTint, eased, the render texture's shader), tst underwater
-x The player swims as npcs do: jump in water strokes up (it waited for CanJump), tst swim steadier

## 2026-11-18
-x Water levels, Minecraft style: water is a source, flowing water a block a level (water 7 to 1), a level less a
  block as it spreads, falls as 7, drains when nothing feeds it (a source taken out drains all it fed); drawn
  thinner, a flowing top at level / 8; flowing edits drop nothing; a check waiting is pushed later, not lost
  (doc/tech/water.md Flowing, tst water_flow)
-x Sunlight dims in water: 24 a block (light_water_decay), a beam known by its light, not by being full sunlight
  (light_beam_above, dark_sunbeam carries the light it expects); placed water relights the beam under it dimmer,
  drained water brighter; the dark flood clears per voxel (a beam beside it stayed lit); doc/tech/water.md Light
  (tst water_light, lakes on seeds 1, 2, 5)
-x Sunbeams lit chunks still generating (before landfill) as air, their water and caves too: they wait for it now
-x Under water the sounds are muffled, not just quieter: a 500hz low pass on the mix, eased (tst sound_mixer)
-x local coop, terrain lods fought (laggy): a streamer moving set every column round it to its own distance, even
  ones another streamer was closer to, so the lods swapped each time either moved; a column's distance is now its
  closest streamer's (terrain_stream_distance), and a player starting no longer drops the other players' queued
  land (their streamers queue it again) (tst coop_streamers: 539 frames of swapped lods before, 0 after)
-x local coop, the skybox: it was the first camera's child, the second player saw its walls and the world cut off
  past them; RenderAtCamera (rendering) draws it round each camera (tst coop_skybox)
-x local coop, spawning: every player in the first one's town (any loaded region's, a random region often had
  none), each in front of their own home's door (home_door_front, the spawn finder tries that column first)
  (tst coop_homes)
-x vox instance rendering (lag on the khadas): a mesh's instances were sent in one go, past the uniform buffer's
  size (zox_get_safe_ubo_size: 1024 on llvmpipe, 256 on some mobile gpus) it overflowed, gl errors and missing
  grass; now batched to it, the buffer orphaned before each batch (filling it again while the gpu read it stalled);
  instances outside the camera's view aren't sent (is_sphere_in_frustum, now in maths/planes); the command lists
  kept between frames; tst vox_instances_stress (1000 to 16000 grass, half behind the camera), bash bsh/tst.sh
  <name> --release (an optimised build with the tests: debug's -O0 cpu times mislead). On the 780M, release: the
  render system under 1ms at 16000, the frame 117ms all in view, 60ms half behind: the grass mesh's 18k triangles
  a block are the cost now
-x grass more natural, less of a mass: tall grass in clumps with open grass between (a clump field 6 blocks apart
  scales tall grass and flowers from none to twice their share, the shares hold), grasslands 0.55 to 0.30 tall
  grass, jungle 0.62 to 0.40; a tall grass block is tufts: 12 to 22% of its columns a stalk each, tallest in the
  middle (build_vox_tufts, seeded by the model; it filled most columns, every variant alike); tst vegetation_cover
  (clumped), grass_tufts
-x marked off, fixed earlier and left in the todo: arms out of step on a character not the player's (tst arms_in_step
  passes), furniture_place stalling (passes), body part seeds (now body_seed + spawn_count, not the realm's index)
-x mountains clear their middle third of vegetation (mountain_clears_vegetation): the check was
  `distance < 2 / (radius * 3)`, integer division, 0, nothing cleared; tst mountain_vegetation
-x aura particles: an aura turned on spawned an emitter it never kept (ParticlesEmitter unlinked), so turning it off
  removed nothing and each turn on added another, their particles never stopping; linked now; tst aura_particles
  (a real aura on a character: on, moved, off, on again)
-x windows build: near and far are empty macros in windows' headers (minwindef.h), 2 names in the game (ik feet,
  xr body) and 7 tests used them; renamed, the rule in doc/engine/coding_standards.md (Naming), and lint_builds compiles a
  syntax only build with them defined (build.sh reads zox_extra_dflags) so a new one fails there
-x the body fits a doorway (play test: too big for a 2 tall, 1 wide door): it was 2 blocks and a hat's top tall at
  Block Res 5 (131 voxels of 128), the head and what's on it now sink into the neck past 2 blocks
  (body_parts_fit_height); narrower shoulders (0.4 of a blueprint across, were 0.6: 0.97 blocks wide, now 0.78); a
  body's bounds are its flesh (FleshSize, the merge's parts without clothes: a sleeve doesn't catch a doorway); tst
  doorway (centred, zox_tst_doorway_off=0.1 off it)
-x taskbar icons had no tooltips (2D or 3D): a has check left prefab_icon's without its TooltipEvent; tst tooltip_flick
  passes again, the 3D tooltip's text checked unclipped in the simulator (xr_navigator --xr-sim, tst/xr_tooltip.ppm)
-x the XR simulator's window is 1440x720 (each eye square, a square window squeezed each eye to a thin strip)
  unless --1080p: bash bsh/tst.sh screenshot --xr-sim saves both eyes side by side (doc/engine/tests.md)
-x 3D tooltips show their text: its glyphs sat at the world's origin (the hierarchy skipped a DisableParentTransform's
  children), now it keeps its own transform and places its children from it (its own scale kept, or its parent's
  passed on); 1cm nearer than its window (4mm hid it behind the frame); a ui without a canvas or tooltip no longer
  stops the rest linking theirs (tst xr_navigator checks its glyphs are on it)
-x the new realm menu's name field resizes to what's typed (it stayed its spawn size, a long name ran off the
  screen): a TextFit tag (its TextFitFontSize, TextFitWidth) sizes its field to it and shrinks its font past the
  list's width, on every list input field (tst realm_name_input); names stay at most input_field_max_length (24)
-x quest tracker showed (null): an unnamed quest or slay target printed its missing entity name, now its ZoxName,
  its realm quest's, its entity name or ???; its text is always allocated (an invalid quest's literal "" was freed
  as a tracker spawned) (tst quest_tracker_text)
-x imported voxes: a .txt beside a .vox says the depth it was made at ("depth 4"), it's scaled up to depth 5 on
  import, so the top hat (and cavalier and bowler hats) aren't half size on a Block Res 5 body; only .vox files are
  read as voxes; doc/tech/imports.md (tst vox_import)
-x local coop: joining and leaving tested with a test gamepad's A and B (tst split_screen), A on the main menu no
  longer switches the first player to a gamepad it joins with (gamepad_a_joins); split screen huds scaled to their
  half and to fit (player_ui_scale_begin), the quest tracker's font scales with the ui (it was 12 at any), the
  stats no longer cover the minimap (tst split_screen_huds); a newly joined player's device mode read the frame it
  joined crashed the join ui
-x local coop: on the main menu players join with a gamepad's A (one the first player isn't using) and leave with
  its B, the players show at the bottom centre (P1 P2 next to their device's icon, "A join" while a gamepad can);
  split screen reworked: the second player's cameras and canvas spawn only as a game loads, back at the main menu
  they go and the first player's screen fills the window (--splitscreen joins a second player at boot) (tst
  split_screen, camera_split_screen)
-x the starting quest (find a town) tracked from the start, the tracker shows it (tst find_quest checks it)
-x quest markers: ! yellow has a quest to give, ? gray you have it, ? yellow you can hand it in; a quest giver
  respawning from its seed (chunk unload/load) shows the player's state of its quest, not ! again (tst quest_marker)
-x clothes' skinning: clothes voxels take the body part they're nearest to through the clothes (a flood out from the
  body a layer at a time, not the first found, which crawled along them), parts not joined kept a voxel apart
  (CombineParents), the touching-parts double faces (vox_mesh_parts) removed; Paint Weights works again (bone.vert
  colours by bone); tst body_weights (bash bsh/poses.sh --weights); the tests list holds 512 (256 was full, the
  last test, xr_continue, was dropped) (doc/done/skeletons.md)
-x swimming: InWater with the splash, swim_system (it floats, drags, jump strokes up), jump out at a shore, half
  speed walking; the view under water blue and thick fog (cameras.Underwater, render_underwater, the sky too), sounds
  muffled (sound_underwater); see through chunk meshes drawn far to near (render_chunk_order); tst swim, chunk_order,
  sea_level with zox_tst_sea_under=1 (doc/tech/water.md)
-x the crosshair system no longer logs an error a frame for a player character without a player (the tests'
  characters: footsteps failed fast on it)
-x spawn_list3 and spawn_huds3_list gone: the 3D options are a spawn_window_list too; the 3D taskbar 0.09m high again
  (taskbar3_height: a window's own height made it about an eye's width)
-x ui consolidation 1 and 2: the main, pause and saves menus' 3D branches gone (spawn_window_list builds them in 3D:
  spawn_button, spawn_list and the scrollview swap to 3D twins), spawn_taskbar3 and its prefabs gone (spawn_taskbar
  on 3D huds, raised above the menus); spawn_list3 kept for the 3D options until they're the 2D window
-x 3D grids show their selection (each element's Brightness drawn: element3_draw, SelectedBrighter as 2D's); 3D lists
  have a 2D list's scrollbar (spawn_scrollview's 3D twins), its handle following navigation (scroll_list_to_child read
  the scrollview link with zox_has: links are pairs, it never found one, 2D's too); every canvas3 a 3D root for its
  children's twins (spawn_canvas3); tst xr_options checks the handle, xr_navigator the brightness
-x code stats: bash bsh/tst.sh code_stats (bsh/code_stats.sh): src's size per module (code, tests, comments, files),
  the biggest files, the change since the baseline (doc/data/code_stats.txt, --save), growth over 5% flagged
-x tooltips under the selected ui for every navigating device: a 2D tooltip follows the mouse with the mouse, while
  navigating (a gamepad, a Steam Deck, keyboard navigation) it's under the selected ui (tooltip_navigated_system,
  after the mouse's); tst navigator_grid checks it
-x navigational grids: a datagrid's icons are NavigationElements, the stick steps them (x one along, y a row:
  element_navigation_system); tst navigator_grid (a window of its own, minimal)
-x 3D tooltips: each 3D window has its own, its uis link to it as a 2D canvas's do, shown centred under the selected ui
  (tooltip3_follow_system, multithreaded: a main thread one in zoxp_update shifted the pipeline's sync points and the
  mirror found no character mesh); xr_navigator selects the action bar's punch and checks its tooltip (it doesn't look
  right yet: doc/todo.md)
-x doc/tech/uis.md: one current doc for uis (3D layers and depth, 2D and 3D from the same code, lists, the navigator,
  grids, tooltips, render textures, gotchas, testing); doc/plans/xr.md points to it
-x navigator tests for every navigator: tst navigator (a gamepad in 2D: start pauses with the selection in the pause
  menu, RB and LB step, every ui opens; the test's gamepad a device without SDL: prefab_gamepad's SDL controller
  closed it) beside xr_navigator (a headset's grips, 3D)
-x render textures in 3D windows (the mirror, the identity's mug shot): their camera and what it films keep their own
  transforms (DisableParentTransform: the window's 3D hierarchy moved the camera onto it, every 3D ui vanished, and
  the mesh away from it, blank); prefab_render_texture's 3D twin
-x XR looking down: the eyes swung down into the chest (the neck still, a voxel chest deeper than a person's); the
  upper body pitches forward and the hips go back as the head pitches below the horizon (xr_look_bend), at once (no
  physics step, no view delay), the eyes the player's; tst xr_look_down
-x XR actions: the right trigger punches and places a block with the 3D action bar up; tst xr_actions
-x 3D windows beside the 2D ones: on 3D huds every taskbar ui (status, identity, skills, items, body, gear, actions,
  quests, crafting, map, mirror) opens as a panel ahead of the player, from its same spawn code: spawn_window builds a
  canvas3 root (ui_window3_pose), spawn_ui, spawn_text and spawn_header swap a 2D prefab for its 3D twin under a 3D
  parent (Prefab3, made by the 2D prefab's own function on a 3D base: windows/pre/twins.c), the 3D ones spawned this
  frame remembered (deferred: their components aren't there yet); per parent, so a game's 2D and 3D uis live side by
  side; no tooltips or render textures in 3D yet (the mirror's empty, the identity's mug shot blank: their camera
  broke the eyes' 3D uis); the navigator leaves a ui it moved into (already open: the action bar) open; tst
  xr_navigator steps RB round every ui, each 3D (screenshots tst/xr_window_<n>.ppm)
-x navigator: LB and RB (a headset's grips) step through the taskbar's uis while paused, one open at a time, the
  gamepad's selection in the new one (huds/sys/navigator.c); the taskbar a 3D row above the menus on 3D huds
  (spawn_taskbar3, the 2D taskbar's frames and toggle: taskbar_frame_toggle); the 2D window stack skips 3D lists
  (crashed on a 2D window opened over one); gamepad navigation starts in the newest open window, not a closing one,
  and a window with nothing to select isn't an error; tst xr_navigator (screenshot tst/xr_navigator.ppm)
-x XR runtime: one interface (xr/fun/runtime.c) for a headset (openxr/runtime.c) and the simulator (sim/): the
  frame, the head and eyes, the controllers' sticks, buttons and poses run through one set of systems, not two
  copies (openxr/render.c, openxr/controller.c and the simulator's own systems gone)
-x XR hands from the grip pose (where the controller's held, /input/grip/pose), the controller's pose (rays) from the
  aim; tst xr_grip (the body's hand on the grip with the aim apart)

## 2026-11-17
-x XR options: a 3D list of the settings (the 2D window was unseen in a headset), A flips a toggle or steps a slider
  (round past its end), the stick left and right steps a selected slider, its row's text kept to its value
-x 3D lists scroll: over 8 rows, navigating keeps the selection shown (scroll_list_to_child, 2D scrollviews too, their
  handle with it); a 3D list's buttons are a 2D list's (list_system, list_render_dirty_system, by ChildIndex); a
  hidden 3D element hides its children, a canvas3 draws up to 512 (glyphs overflowed 128); the 3D saves list shows
  every save, not the newest 6; tst xr_options (scrolls, a slider, Back; screenshot tst/xr_options.ppm)
-x XR loading transitions: the eyes close (dark lids from the top and bottom, curved, the middle first: xr_eye_lids,
  scissored clears after the world), shut only the "Loading" label is drawn, fading in (element3_brightness), the eyes
  open once loaded and the camera's on the body (huds3_dark_hold: it teleported there in view, a pop); the fog fade
  didn't blacken a headset's view; tst xr_play screenshots the lids closing (tst/xr_lids.ppm), checks no early open
-x XR bending: the spine solved from the real neck (xr_spine_solve): closer than the spine is long it bends, the hips
  back (HipsShift, was HipsDrop) and the chest pitched down to ~110 degrees, then crouches; farther it leans; past the
  body's reach across the ground the body moves; the view takes what's left so the eyes stay the player's; it leaned
  only by the neck's x z (bending read as leaning forward, the body walked forward); tst xr_lean bends
-x XR turning eases after the head from 20 degrees (at once past 52), recentring (a headset's reference space event)
  turns the body by the room's turn, the head straight on it, the view still; tst xr_lean recentres
-x arms reach past their length up and out: the shoulder moves after the target (ik_shoulder_reach, 0.2 of the reach),
  the XR hands retarget from its rest (from where it moved, the target went with it); tst xr_reach up_far, side_far
-x tst xr_lean: xr_body's later phases (crouch, turn round, 72 fps, bend, recentre), xr_body ran past the test's time
-x tst ik_poses: the ik rig posing a character (reaching up, out, ahead, down, wrists rolled, leaning ahead and to
  the side, crouching, both): hands on target, feet where they stood, crouched the knees ahead and the hips lower,
  the neck the lean's way; bash bsh/ik_poses.sh saves tst/ik_poses.png
-x XR arms reach every way: the hands' targets from the shoulders (the player's from their neck, the body's facing
  and lean; the body's leaned as it is now), the player's arm scaled to the body's; from the eyes the body's
  shoulders (lower, further back, wider) overreached up and ahead (the hand short of the controller) and bent down
  and out (0.8 of reach); tst xr_reach: 0.8 of a real arm is 0.8 every way, the hand on its target; the simulator
  takes scripted hands
-x XR body leans and crouches: the real neck off the hips leans the upper body about them (SpineLean, bones/ik
  sys/spine.c: the chest turned about the hips' pivot, the hips kept upright) up to 40 degrees, past it the body
  moves and the feet step; lower than the lean has it the hips drop (HipsDrop, the knees bend), past 0.45 of the leg
  the view itself; it slid the whole upright body, bending the eyes sank into the chest; tst xr_body leans, crouches,
  and leans past the reach at 72 fps (1:1 scaled)
-x XR body turns after the head: past 1 radian off it the body turns, the room turned back on it (xr_body_room_turn)
  so the view stays; tst xr_body turns round
-x feet settle back under a still body (feet_settle_distance); tst feet_planted leans the spine, walks by velocity
-x the wrist's twist shared: the forearm takes half the hand's twist about it (ik_forearm_twist); tst arm_reach
-x leg ik and planted feet: a thigh's IKTarget is its foot (two bone: thigh, calf the knee, foot kept level);
  PlantFeet on a skeleton (XR's body) holds each foot where it's planted as the body shifts, stepping back under the
  hip in an arc when past 0.3 of the leg's length, one foot at a time; walking fast or in the air (by vertical
  speed, Grounded flickers) the swing has the legs; tst feet_planted, xr_body checks the feet looking about
-x arms choppy turning and walking: the guard's reach was measured between bones of two frames (the shoulder carried
  to now, the elbow not), short by the body's move, the guard pulled in and out; tst guard_steady (0.39 of reach off,
  now 0.03); the guard aims by the head and camera's turn this frame, not the hierarchy's last
-x XR hand upside down: the hand onto the controller's turn was the shortest turn down to forward, its thumb side
  ending down; now its front goes up the controller's up (ik_hand_rest_to_target); tst arm_reach checks the roll
-x XR pause on Y as well as the menu button (headsets keep theirs); xr_continue only in test builds (make run failed)
-x XR the real neck: the headset's eyes swing about the player's neck as they look about; only the neck's moves
  count now (xr_neck_room): its x z walks the body (looking down slid it forward), its y lowers the view (crouching;
  looking down dropped the eyes into the chest); the simulator's head turns about a neck; tst xr_body checks both
-x XR continue and pause: the load menu and pause menu are 3D lists on 3D huds (they were 2D, unseen in a headset),
  a save goes straight in (no realm menu); tst xr_continue: continue, the newest save, pause by the menu button, return
-x XR loading fades: to black as loading starts and back after (huds3_darkness, rendering's render_darkness: the
  clear colour and fog darken, the fog thickening so the far goes first, 3D uis unfaded); tst xr_play checks both ways
-x XR device mode: the keyboard, mouse and touchscreen are feeders on a headset too, only the controllers (and
  gamepads) switch it; tst xr_body checks the controllers enabled playing
-x XR frustum culling: the xr camera culled by the desktop's FieldOfView and the window's aspect, not its eyes; now
  it projects from FieldOfViewAngles (cameras), the union of both eyes' off centre angles plus a margin (the eyes'
  offset, the head turning before they draw), set each frame from the headset's eyes or the simulator's
  (xr/fun/cull.c); tst xr_frustum: the eyes' view edges inside the camera's planes, the head turned about
-x XR loading: on 3D huds the view darkens to black (huds3_dark, the eyes draw only the 3D uis) with a 3D "Loading"
  label easing ahead of the head (huds3_follow_system); the world isn't seen loading; tst xr_play screenshots it
  (tst/xr_load.ppm: each eye black but for the label), lit and gone once playing
-x XR scale: the player's real height (setting "XR Height", 1.7m) onto the character's (~1m): the eyes, their
  distance apart, the room's walk and the hands are scaled by the character's height over the player's (fun/scale.c),
  so the world looks the character's size and the hands land in its reach; 1 in menus (roam); tst xr_body checks it
-x body fits 2 blocks tall, 1 wide (a door's way): head and calves smaller, arms longer to reach as a person's;
  tst body_views checks the bounds
-x XR hands on the body: controllers' tracked poses kept and put on the body after physics (zoxp_trails), the arms
  solved after it (zoxp_layouts) from bones carried to where the body is now (skeleton_now); in a game they were on
  the roam (left behind once the character became the body), and walking trailed them a frame; the punch the same;
  tst xr_body: walking, the hand doesn't move on the body
-x XR walking through physics: walking in the room is the body's velocity for the frame (xr_room_walk_system,
  before physics), walls stop it (it moved the character straight through them); the stick and room velocities are
  scaled against the frame's drag (physics3_drag_compensation), friction kept: drag flickering with Grounded on
  seams made the walk's speed bump; tst xr_body: steady speed per frame, a wall stops the room's walk
-x XR walking without inertia: the left stick sets the ground velocity (at speed at once, stopped at once), the
  acceleration and friction coast felt like lag; the headset logs its frame timing every 2s (fps, ms waiting,
  simulating, drawing the eyes, late frames) to find what's slow on it
-x XR body fought the head (playtest: laggy moving, head turns fighting the body): in a game the xr camera rides
  the character's head bone, the roam took it up every frame (the hierarchy moved it), folding the head's turn into
  the body's and pulling against the sticks. Now the character is the body (xr/fun/body.c): the sticks move and
  turn it (as a gamepad), the eyes ride its neck with the headset's pose on top, walking in the room moves it, its
  head bone turns as the headset (the voxel head matches, raycasts look where you look), no head bob, no stick
  pitch in XR; tst xr_body
-x Arms bend in normal play (bones/ik/sys/punch.c): a raised arm holds a guard ahead, a little up and in, seen in
  first person (held low by the chest it looked gone, a dead hang) with the elbow relaxed, a punch pulls back
  with the elbow bent, strikes out to near full reach in towards the body's middle, recovers; weights ease on and
  off; tst punch_motion
-x XR lag: the headset's frame begins at the top of the engine's frame (xr_frame_begin_system, zoxp_begin), so the
  controllers' poses are predicted for this frame's display time (they were the last frame's, the hands a frame
  behind); a frame the runtime said not to render is ended (it was left open, breaking its frame pacing); every
  OpenXR event read a frame, not one
-x XR room scale: the head's tracked position moves the camera entity (streaming, sound, raycasts followed the room's
  centre), stick turns turn about the head, not the room's centre; tst xr_room_scale
-x Arm IK (bones/ik, a submodule): a shoulder's IKTarget and IKWeight bend its arm with the skeleton's own bones
  (shoulder, the hand's parent as the elbow, the hand), two bone ik solved analytically, blended over the swing and
  raise; a punch reaches the right arm where the head looks (replacing arm_raise's swing wobble); XR hands hold
  each shoulder on its controller (IKHeld, waits on the XR player body); tst ik_two_bone, arm_reach
-x Into a game with XR (tst xr_play, the simulator's controllers alone): New Game enters a realm with the defaults
  while the huds are 3D (no text input for the realm menu); the XR camera has the game camera's parts (streaming:
  terrain never loaded, raycasts: a crash) without its post processing; free roam takes up the camera when something
  else places it (the game's start); blur and vignette only on cameras that have them (the dialogue's lerp crashed)
-x XR controllers as input devices (doc/plans/xr.md step 3): a device a hand (prefab_device, XrController,
  XrHand), its pose in Position3D and Rotation3D, its stick and buttons as zevices, written by OpenXR (A B X Y menu,
  triggers, grips, aim poses) or the simulator's keys; they play as a gamepad, the right hand navigates and clicks
  (A or its trigger); the simulator's keyboard and mouse feed them (DeviceFeeder), not a device mode of their own
-x 3D list ui (spawn_list3, button3): a header and buttons on a canvas3 panel, navigated and clicked as a 2D list;
  with XR on (zox_huds3D) the start label and the main menu are 3D, ahead of the camera, A on the start opens it;
  the XR camera is a Camera3 (billboards face it); tst xr_menu; tst xr_sim_eyes checks upright by colour, not green
## 2026-11-16
-x Water off the tilemap: its own texture and gpu texture, liquid meshes bind it, it animates alone (the whole
  tilemap was uploaded 8 times a second); the quarter size water (Block Res 5) reproduced and checked, tst water_tile
-x Flowing water: an edit next to water fills it, water falls and spreads 4 blocks, tst water_flow
-x A realm without a save folder no longer saves its chunks beside the binary (the tests' realms did, the next run
  loaded them)
-x Lakes: a bowl carved into the land in some 64 block cells, full to a block under its rim, nothing growing in it,
  towns and mountains kept out (a town square sat in one, a mountain filled another); tst lakes; doc/tech/water.md
-x Water's texture fills its block's face (it was a quarter of it with Block Res 5: its texture kept the boot size)
-x Touch virtual joysticks are broken: the joystick system checked the finger's ElementLink, spawn_virtual_joystick
  links the virtual joystick's, so it never dragged or let go (the stick stayed 0, its ui stuck)
-x Touch uis arnt always perfect size: the touch buttons are at most a seventh of the screen's short side (on a
  small screen they covered it, every touch hit one, i.e. pause), ints (a byte wrapped 66 * ui_scale from 4)
-x add touch ui tests: fake fingers (zox_touch_fake, like the fake mouse); tst sdl_touch (a finger down, dragged,
  up reaches its zevice), tst touch_play (in game: a tap switches to touch mode and its ui, the left joystick walks,
  let go it goes, the right one turns); no "Device not attached to player" error at boot
-x tst gpu_restore (rendering/tst): disposes and restores every gpu resource on the main menu, checks every gpu link
  is let go of and holds something again, no gl errors, the picture's average colour the same; it found the sky
  coming back magenta: the skybox never kept its menu colours (only set on the gpu), a restore set the prefab's
-x Sometimes when i alt tab, the raycasting UI breaks: fullscreen minimizes on focus loss (rendering and time
  stopped), wayland may never send restored, so focus gained restores too; back in, the mouse counts as in the
  window with keyboard focus and the pointer inside (sdl may not give mouse focus till it moves); tst sdl_alt_tab,
  sdl_mouse_focus
-x SDL tests: sdl_window (window, its size, gl context), sdl_resize, sdl_quit (apps/sdl/tst), sdl_text_input
  (ui/elements2/tst)
-x you can place a bed with only one free block: placing now checks its whole box (a29383dda), check again
-x player character looks at npc body and not their head
    - player still not facing npc when walking to them
-x npcs shouldnt jump randomly when you are talking *unless theyre hyper*
-x Blocks checked by eye: the bed and door with bsh/model.sh; tst block_frames for animated blocks (water's 16 frames
  side by side, they loop and tile); tst sea_level looks down at open water and sees it move
-x A sea: ground below 18 is under water up to it (heights/sys/landfill.c terrain_sea_height), about a tenth of the
  land; sand moved up to 21 for its shores (the land is mostly 17 to 30 high, tst sea_level logs its spread); tst
  sea_level flies to the nearest sunken ground, checks its water and screenshots it (tst/sea.ppm)
-x Water blocks: a liquid material (chunk materials, 50299a0cf), walked through, light passes, an animated ripple
  texture (16 frames tiling in space and time, moved in the tilemap: tilemap_animate_system), its chunk meshes
  drawn blended after the solid ones, both faces (chunk3_transparent_render_system); faces between waters and
  against solids hidden, a solid's face against water kept; tst water_block. Nothing places water yet
-x Placing a block in an all air chunk gives it a mesh: not reproduced (a mesh within ~1s, 3 runs), tst place_mesh
  guards it (a block in the middle and one in the corner)
-x minimap flashes black when updating lods
-x Item in hand dissapears when updating body - parts
-x Skeleton pose resets when we rebuild skeleton
-x Skeleton flickers when pose reset
-x holes still in skeleton
    -x make a test that poses the skeleton in several poses so can quickly inspect the results

## 2026-11-15
-x Tooltips stuck: a ui selected and deselected in the same frame (a fast mouse flick) unlinked before its deferred link landed, the link and text stayed; the tooltip now lets go of a ui no longer selected (SelectState), tst tooltip_blink (stuck 5 of 10 before)
-x Bug: Character shakes - smooth the physics - put to sleep when not moving
-x Smooth the camera or sleep the physics cause it flickers the lighting
-x Fix little teleport when loading player character
-x Fix destruction material
	-x use the depth offset instead
	-x reduce z offset to 0
-x Fix overlay of destruction materials for block destruction
-x Destruction Material Flickering - increase depth until i fix it
-x Bug: lines of selected block should render over destruction overlay
-x Bug: died holding block, stuck to model?
-x When dialogue animates - first fontt will flash...!
-x Render frames with alpha 1 (the opaque render texture material) show nothing, the alpha one works (mug shots use 0.95)
-x Font sometimes scrambled on ui
-x Text font just scrambled positions again - maybe it repositions before child index is set
-x Line2Ds are broken, need to add to new 2D render stack that sorts layers
-x fix text label size when resizing - button size
-x Slider UI isnt accounted for in list ui
-x List ui calculating wrong cause sliders different heights then buttons!
-x Fix Scale2 working on UIs
-x Pause ui toggle not showing on spawn taskbar + pause
-x stats panel didnt spawn when respawn
-x Tooltips: If ui selects while didnt deselect..!
	- select main menu header after button selects
	- the tooltip updates to wrong one - shows load game when you  mouse move fast between uis
	- it can show nothing selected even though log shows it set, teh issue therefor must be that it gets set and unset in same frame
-x glyphs j and y fill solid, their outlines in res/fonts/monocraft.zox close the whole box (the fill follows the outlines, tst font_glyphs_fill, look at tst/font_fill.ppm)
-x Glitchy depth fighting on 3D UIs, and on Block Destruction Materials
-x Bug: Edge of map UI is glitchy
-x Map UI slightly off still - the flipping part
-x Map Arrow is backwards, but its pointing right way, when we move right in character, the map ui goes left... So rotation is flipped of map and arrow
-x Sometimes the lighting doesnt propogate properly into the house
-x Smooth out camera at low fps - its very glitchy
-x skybox doesnt follow to main menu?
-x PNG errors - convert rest to bmp
-x corners of light still off (needs check solids)
-x when moving between chunks can throw lights off
	- i think due to light at lower depth is more sunny
-x Fix light face on bigger lods, needs tojust pick brightest if at a lower division
-x At start, terrain lights don't update in some chunks
-x Gizmos: Selected Block Gizmo - is flickering - only when attached to character, if detatch is stops...
-x Fix post procesing on android
-x Fix Android Build - on arm pcs
-x Fix viewport not resizing
-x if building a tower up - shows invisible face
	-x Its when its next to a chunk with no chunk meshes - it doesnt build the edge
-x Fix Lighting Seems between Chunks, only sometimes??
-x Vegetation seems off, when moving between LODs
-x Fix Vegation not building at same scale
-x Sometimes chunk didnt update until we destroyed a block from it
-x When i spawn new chunk meshes, it breaks the old ones...!
-x Terrain Chunk faces missing when streaming
	-x update neighbors directly from the queue
	-x just rebuild their mesh, no need for voxelnodeupdate etc
	-x When streaming terrain - flag neighbor of updated chunks dirty
		- the outside chunks have missing faces now
		- move a little around chunks
		- look at LOD horizon and notice chunk faces can be missing
	-x Sometimes flying around we see missing faces now in chunks
		- probably didnt trigger neighbors..
	-x Im getting neighbor updates not triggering at map edges
-x Bug: NPCs can spawn before the chunk is finished, which makes them fall through the chunkx
-x Redo skinning, has overlapping issues if parts close atm
	-x Cellular automata voxels around bone joints for better rotation
	-x use octree data for bone indexes, so we can remember what part we placed the voxels from, then we can use this when building up the weights
	-x we can probably blend them better between as well
-x First person camera doesn't adjust for the head's size: the head has a neck now, its eyes higher (head.c)
-x Fix timing of arm swing
-x arm should only slerp rotation - no setting
-x Fix jumps working same at diff time deltas
	- physics, we just need to give it the same accelaration juice
-x Bug: Taskbar toggle not deactivated when window closes (80ec42f82, tst window_toggle)
-x Windows no longer close taskbar toggle (80ec42f82, tst window_toggle)
-x Bug: The 3D uis are flickering due to depth fighting atm (f96a61886 bda4636c5, tst ui3_overlap)
-x Fix the layering of the 3D UIs, text and such has depth issues (f96a61886 bda4636c5, tst ui3_occlusion)
-x Smooth Lights Error: Colors Past Limits [n] (logged during terrain load, seen in most tst runs) (d36ca556a)
	- [17:32:51.562] [ERROR] Smooth Lights Error: Colors Past Limits [1200] - Face [5] at Pos [12x8x13] Depth [4] of [4]
-x fix end game fade out, really bad atm (5fb34823a, tst end_game_cleanup)
-x Main Menu music missing (spawn_menu_music, on the game, crossfades with the realm's)
-x Fix music importer (make convert, tst music_file)
-x Music load error on linux (the note count was read as a size_t, tst music_file)
-x Sound effects quiet in game (too low for small speakers, tst sound_balance)
-x Quest givers say "You did well..." without giving a quest (NodeLink was exclusive, tst quest_dialogue)
-x healthbar isnt first still in the panel, soul ended up last (c033af902, tst statbars_hide)
	- soul needs to be first... lol
-x 3D uis are visible through buildings at distances, i was unsure how to fix this before (bda4636c5, tst ui3_overlap)
-x raycast gizmos not cleared upon player death or exit realm (d93eb8a4e, tst gizmo_owner)
-x The map moves per block, one view drawn from cached tunk colours (tst map_pixel)
-x Bed and door models, bigger than a block (doc/plans/multi_block_vox.md, steps 1 and 2 of 5): generated models
  two blocks a side (vox_type_bed, vox_type_door), blocks on the realm with a BlockSize, their block vox placed
  from the base voxel's corner at a block's detail; tst furniture_models, furniture_place. Not yet: fillers,
  rotation, the door's state
-x From the other session (committed ba6c6e9c0 to ba6d09296): the town toast nearer the top, the quest tracker
  under the minimap, the tracker's "slay ???", talking with no action selected, grass items as big as mushrooms,
  road steps heavier (hit_sound_road), the health bar first in the stats panel, a sound when a ui opens
  (sound_effect_ui_open), the starting find quest ("Find a Town": talk to anyone in a town, 3 cookies), npcs hop
  up a one block step (tst npc_leash), four suite tests fixed
-x Small playtest polish (grouped): grass vox items sized like mushrooms (`e240ed16d`, `tst item_sizes`); roads sound
  heavier than dirt when walking (`49f575b11`); window open/toggle plays a sound (`fa6e61c24`, `tst window_toggle`);
  npcs can be talked to/raycast without an action selected (`c4baafdfa`, `tst quest_dialogue`); health bar shown first
  in the stats panel (`c033af902`, `tst statbars_hide`)
-x Smooth lights light from a grid: a chunk's lights and voxels flattened once a mesh (octree_fill_grid), a face
  indexes them (it walked a tree and took a lock 21 times a face); 2.6% to 1.8% of the cpu flying; tst
  smooth_lights and lod_lights check the grid against the trees; doc/tech/octrees.md
-x Holes at the shoulders when animating: 7 corners of a humanoid cracked, a merged face reaching past its own
  bone's voxels gave its far corners its bone; every face at a corner now takes the corner's own two bones and
  weight (through joined bones: chest, shoulder, bicep), coarser lods take a node's bone from its children; tst
  chest_weights counts cracked corners (0); doc/done/skeletons.md
-x A deleted entity's handle no longer comes back alive: instancing a prefab revived deleted ids for its children
  (a flecs patch, doc/engine/flecs_patches.md), a deleted chunk's id became an npc's stat and its neighbours' links read
  alive ("Neighbor [(null)] has invalid components", crashes in the lights and chunk_sides_system flying), tst
  instance_id_reuse; chunk_sides_system no longer leaves a lock unset for a neighbour that isn't a chunk
-x tables_stream passes: child bones had a second, ChildOf parent (~45 tables a humanoid), a town's chunk step makes
  244 tables, was 727
-x tst arms_in_step passes (3 runs, and on the build before the bone parent fix: fixed earlier, not by it)
-x Bones from the voxels, blended at joints: a BoneNode octree written at the merge, box weights gone (they gave
  6% of a humanoid's vertices the wrong bone at every joint), tst chest_weights; doc/done/skeletons.md

## Moved from todo (2026-11-14)
-x Lod swaps without flicker: no holes or dark chunks swapping lods flying, timers gone (tst lod_swap)
-x Terrain streams 2.4x faster, the land near you keeps up flying (doc/done/streaming.md: plan 1 to 4), tst perf_fly
  near lod latency; lights skip a neighbour chunk mid spawn (a crash)
-x 3D uis drawn a canvas at a time: layers in order, then the canvas writes its depth (canvas3_render_system, no
  depth offsets), tst ui3_overlap and ui3_occlusion; doc/tech/uis.md
-x Map moves per block (one view, redrawn from each tunk's cached top down colours, 1ms), tst map_pixel
-x Npcs out in the wild again (spawn rolls rounded down to 0 out of towns, npc_spawn_count); tooltips no longer stick
  when the mouse leaves the window (tst mouse_leave), check in play
-x Smooth Lights Colors Past Limits: a stale mesh (older than its sides) is rebuilt instead; tst tween_end (animations)
-x Tests left out of release builds (zox_tests, set by --debug); tweens get an end event (AnimationEnded, observed)
-x Sky blends between biomes (blend_sky_to, sky_blend_system), its fog with it; the gpu upload moved off the worker
  thread biome_link_system ran on; tst sky_blend
-x Start gear colours: dull, a hue away from the skin's, light on dark skin and dark on light (realm_skin_hsv, realm_gear_colors)
-x Cookie: crumbs in the dough's second shade (they were its own colour, unseen), a bite out of its edge
-x Font: g q y p j get their tails below the baseline (the ttf conversion wrapped negative points round to the top as
  bytes, g and y read as 8 and 9; its bounds now by character code), res/fonts/monocraft.zox reconverted
-x Entering a town: InTown and TownLink on the player's character (in_town_system, cleared on leaving), a toast
  "Welcome to <town>" (spawn_toast, huds), the town's own music (a second realm track, flute), the overworld's on
  leaving (play_realm_track); tst town_enter
-x Npc quests: a third of villagers give quests (by seed, the same after a load), three slay quests each with its
  own dialogue (Slem Infestation, Slem Cull, Slem Scouting), a giver gives its dialogue's quest (QuestLink); tst
  npc_quests
-x Shoes shaped like shoes: low at the toe, up round the ankle at the heel, the heel under the leg (it stuck out
  behind), a flatter, slimmer foot; arms no longer tear from their sleeves: a vertex outside every bone's box goes
  with the nearest box, not the chest (an arm's front strip stayed still when it swung); tst chest_weights checks it
-x Quest tracking: the tracker shows the quest given (it read the deferred instance: (null), Find [???]), the quest
  log shows the tracked quest when opened (quest_icon_tracked_event); tst quest_log_tracked
-x Play test ui: every window pops in and out, id and mirror windows close buttons, footers their own strip, stat
  bars hide each on their own, footsteps an octave up and louder; npcs after loading couldn't be reproduced (tst
  npcs_after_load); tst window_toggle, tooltip_gone
-x Character mug shots (a render frame of the head, stats/ui/ins/mugshot.c) in the id window, framed, lines underlined
-x Chest came apart round the ribs when the arms swung: a vertex now moves with the bone it's deepest in, not the last
  whose box reaches it (778 rib voxels moved with the shoulders); tst chest_weights
-x Shorts rounded: the hips and each leg bevelled, a gap between the legs
-x Player's character named from its seed again (generate_name, kept on load)
-x Id window (taskbar, Identity): name, level, title, race, class, job, clan a line each, each from its link
  (TitleLink, RaceLink, ClassLink, JobLink, ClanLink), none until they're made; tst id_window
-x Dialogue window: a footer along its body for its buttons (spawn_footer, WindowFooter), its text above it, at least
  its footer and two lines tall on small screens, text taller than its box cut with ... (text_truncate, text_rows)
-x Town building helpers tested (tst town_build, drafted by the local AI from a spec, reviewed)
-x Held and dropped vox items (cookies) sized and centred from their voxels: biggest side a block's width, their
  voxels' middle on the hand, out of it by half their depth (vox_filled_bounds); tst held_item, held_block
-x Shirt sat behind the chest: the body combined before the shirt's model finished generating (its size read as 64),
  bodies wait for Generating models now (f0ad65340, tst body_views checks clothes are centred)
-x Stat bars hide after 5 seconds without changing, pop back in when they change (AutoHide, ui_poke, auto_hide_system,
  ui_shrink); tst statbars_hide
-x Memory leak: array and queue components' move hooks dropped the old value when assigning (a skeleton mesh's arrays,
  ~700kb a humanoid, a game left 22-46mb now 8mb); the npc glance read chunks' entities on a worker as they were
  reallocated (now main thread); tests humanoid_leak, monster_leak; --nohumans; test runs check leaks before _exit
  under AddressSanitizer
-x Towns: a square in the middle (homes placed round it), roads from each home's door to it and from it to the gate,
  the gate on a random wall at a random spot (TownGateSide, TownGateAt), the rest of the town its natural ground
  (towns/fun/town.c); tst town_homes (off the square, door roads, gate on its wall)
-x Buildings as boxes (towns/fun/build.c, like model nodes): build_home writes a home in its own frame (floor,
  walls, doorway, roof layers, gables, glow stone), turned to face its door to the town's middle (HomeFacing), filled
  into chunks; HomeFloor set once at spawn; tst home_roofs (boxes, turned), home_foundations (doorways open),
  town_view (a screenshot over the town)
-x Homes stand level: the ground under them (and a block round) levelled to one floor (the terrain at their middle,
  terrain_height_at, the same in every tunk), steps from their doors a block a column; home building in one file
  (towns/fun/home.c), walls.c simpler; glow stones on a chunk's edge light into the next chunk; tst home_foundations
-x Towns: few monsters, mostly villagers: character types tagged Villager, Monster, Critter, the spawn roll weighed by
  how town like a chunk is (its population, npc_spawn_weight), the wild as before; tst population
-x Dialogue window pops in and out (ui_pop_in, ui_pop_out: LocalScale1 tweened, eased, AnimationEase,
  AnimationDeletesTarget), its Next button pops in, input comes back once it's gone (an observer on the window);
  tst ui_scale (a ui's scale reaches its glyphs, pops), quest_dialogue
-x Heads look: LookTarget on head bones (bones/sys/head_look.c, eased with camera_look_at, within the neck's reach),
  dialogue speakers look at each other (cleared by an observer when the process goes), humanoid npcs glance at players
  walking by (ai/sys/glance.c, their chunk and its 6 neighbours' ChunkEntities, across edges); tst npc_glance
  (across a chunk edge), quest_dialogue
-x Dialogue text wraps to its box: TextWrap (lines wrap to its LayoutSize, the size system skips it), text_wrap,
  TextSizeDirty removed; tst text_data, quest_dialogue (tst/dialogue.ppm)
-x First person camera at the front of the head, past a thick chest, at eye height; tst head_camera

## Moved from todo (2026-11-13)
-x Voxel indexing: realms save blocks.txt (each index's block, name and seed), and once their blocks are indexed remap
  their saved chunks if any index moved (gone blocks to air), tst block_table, block_table_remap; misses whoosh like
  wind (airy rush, a rising howl, by the local AI)
-x Removing a glow stone darkens what it lit: darkness from it at its light, the edit light system handles each edit
  once after it's set (it ran every frame an edit queued, before old_value was known), emitters seed a step down,
  placing one lights around it; tst glow_light
-x Homes: roofs in their biome's wood, peaked (two in three, a ridge along the long side, gables) or flat
  (HomeStyle), a glow stone on top lighting it (BlockLight, BlockGlowStone, its light seeded on the queue); house
  placement fixed (counted only placed, seeded per try, its count from the town's seed); tst home_roofs, town_homes,
  home_glow
-x Placement zones in one place: PlacementZone children (Radius or ZoneSize), region_zones_find and
  region_zones_place_circle/_rect, mountains, towns and dungeons all use them, tst region_zones; the test list held
  128, 4 tests were silently dropped, now 256
-x Placement zones avoid earlier ones: dungeons avoid towns (and mountains), placers count only placed zones (a
  failed try left a gap later ones checked against), tst region_place_prior
-x Npcs stay: region PopulationMap (a cell a chunk column, busy at towns) scales spawns, npcs despawn where they
  are not where they spawned, homes respawn only missing npcs (NpcHome), wanderers leashed home; tst population,
  region_population, npc_leash
-x Arms keep in step: the skeleton owns the swing timing (SwingPhase, SwingBlend), every limb reads it, a raised
  arm swung out of step after (tst arms_in_step); unique sounds twice as fast (shared harmonic rotation), harp by the
  local AI (its cost is its decays, not its sines)
-x Footsteps: walking on the ground plays the block underfoot's hit sound each stride, softer (Footsteps tag,
  players for now), tst footsteps; flute generates 40% faster (its harmonics by rotation, by the local AI)
-x The rare terrain loading stall: a region with no towns left its tunks at the towns step forever (a continue
  skipped ending them), their chunks waited on them and the player's spawn on the chunks, tst town_maps_empty
-x Lag spikes crossing chunks (48 to 145ms): non fragmenting children (400 to 2500 tables a step to ~100), flecs
  patches, mesh and sides budgets, stat dirties; worst flying frame ~20ms (release, gpu), the rest in todo.md (Optimizations)
-x Player respawn test: dies, waits out the 16s death screen, respawns alive at full health (tst player_respawn,
  drafted by the local AI)
-x zox_geter and zox_geter_value removed, 409 calls are zox_get and zox_getv now (const T* name = zox_get(e, T))
-x Npcs attack the player again: their skill's resource (energy) wasn't found, their stats are copies of their
  kind's (a step from the realm's), stat lookups walk the prefab chain now (is_realm_stat_of), tst npc_attacks
-x Lighting between chunk lods: coarse lod meshes read stale open light nodes (darklight), dark faces in the
  distance, they read their leaves' max now (getv_nearby_max_LightNode), tst lod_lights, lod_chunk_lights
-x Smooth lighting: no light leaking round corners (a corner behind two solid edges is dark), faces lit from a table
  of their axes, tst smooth_lights; tests run on x86 again (egl device by machine), npcs link their hit sounds
-x Shirts with shapes: a boxy sleeved shirt (worn) and a tight sleeveless vest (inventory), box nodes in blueprints,
  clothes merge last; fixed: the second hat never reached the inventory, slot names out of order, calves' slot types
-x Body and equipment doc (doc/tech/body_equips.md), a shirt: generated from a blueprint like body parts, worn in a
  shirt slot over the chest (anchor over, no bone of its own), players get the realm's, tst shirt
-x Realm sounds: hits sound like what they hit, stone knocks, dirt thuds, sand crunches, wood tocks, leaves rustle,
  slimes squelch, bodies thump and grunt in their own voice (distorted), sound entities of the realm linked by
  block and character kinds (doc/tech/sounds.md), tst hit_sounds, hit_sound_links, sound_balance
-x Font filling was off: outlines walking back over themselves (g, q) left parts empty, fill by the glyph's
  winding now, tst font_fill, font_glyphs_fill, texture tests (texture_noise caught dark noise wrapping bright)
-x Trees spawned too rarely and in clumps: a jittered grid of tree cells (vegetation/fun), spaced 5+ apart, with leaves
  (a leaves block a biome), tst vegetation_trees, vegetation_chunks (trees across chunks)
-x The grassy biome wasn't grassy: ground cover is a biome's shares now (tall grass 55% in grasslands, was rare),
  tst vegetation_cover
-x Tests for terrain generation: biome_heights, biome_map, region_place, town_maps, vegetation_trees,
  vegetation_cover, vegetation_chunks
-x Flying spikes (78-164ms, tst perf_fly): deleting an entity walked all 14k non fragmenting records (flecs patch,
  doc/engine/flecs_patches.md), flying is 20-22ms worst now, even with --maxnpcs
	-x npc spawning walked every non fragmenting record too (flecs patch), worst merge 18.6 -> 11ms
-x Refactor town and mountain placement in regions: regions/fun/place.c (circles, rects, used by mountains,
  towns, homes, dungeons), regions/fun/tunk.c (a tunk's region setup, the columns a thing covers),
  tst region_place, town_maps (towns drawn across tunks)
-x Biomes by climate (temperature, moisture), a center each, heights blend near edges (raises gone, tst biome_heights)
-x 3D lines draw instanced like particles (2000 lines in a draw, was a draw each), tst lines_render, lines_spawn

## Moved from todo (2026-11-12)
-x Set minivox/world voxels brightness when lighting updates
	- use observer on light change event on terrain chunk types
-x speedup the loading of terrain
-x Add --autostart tag that autos through the uis, into a new game
-x Make quest tracker track new quest when you get it
-x Make items pop a little when dropped, like borderlands (tst pickup_pop)
-x LAG - 150 ms before now 50...
	-x Disable ChunkMesh and just set Transform + position data
	-x same for chunk
-x Refactor the sdl mixer into audio - using my own mixer code (src/sounds/mix, doc/tech/audio.md)
-x Bug: Fix music on Main Menu (spawn_menu_music, crossfades with the realm's)
-x Bug: Fix music loading properly (tst music_file)
-x Dialogue: add sounds per syllable (npc voices, tst voice_sounds)
-x Fix Font imports for "?" (quest markers use ?)
-x dialogue conditional node: has quest (tst quest_dialogue)
-x simple kill slime quest, hand in once you killed them (tst quest_dialogue)
-x Give npc a quest, dialogue option to give the user a quest
-x Quest rewards: sounds (quest taken and handed in jingles)
-x Right click to split items (tst item_split)
-x Editor: let mouse scrollwheel work on the scrollbar (tst scroll_wheel)

-x add min, max to fps display
	- "60 [55-62]", min and max over the last 10 seconds
-x When return to main menu - set camera back to origin
	- already done by player_state_game_end, tst menu_flow checks it now
-x add a test for exit game, its crashing on exit atm
	- no longer crashes: main doesn't dispose the world anymore (the hashmap disposal hang)
	- tst exit_menu (exit button) and exit_game (closing while playing), tests can pass on close (zox_tst_set_exits)
-x fix error Smooth Lights Error: Colors Past Limits
	- the mesh builder ran while its chunk's sides rebuilt on another thread (same phase), reading half built sides,
	  and the sides system skipped BuildMesh as the mesh was still building, so it never rebuilt, now it waits for BuildChunkSides
	- smooth lights also skip air with sides, like the mesh builder
	- player_spawn and stream_npcs fail on any of these errors
-x make lists sorting use child index - after sort it shuffles atm
	- children iterate by table, hovering moved elements between tables, lists now lay out by ChildIndex
	- tst list_order
-x When right click on an item ui, split the item quantity in half, the other half in the mouse
-x When right click empty spot, place a single item, or stack a single item
	- RightClick event (on press), data_ui_right_click_system (main thread, it spawns items), tst item_split
-x game_state_system log error after exiting game
	- the_clean deleted the realm, then it errored every frame, now it reaches none (tst menu_flow)
-x for our tests, just link to current inspector if its opened
	- inspect_entity only retargets an open inspector (tst editor)

-x I noticed some faces are not culled between chunks
	- write some tests in chunks/chunks3/tst
-x Make projectile collide with Terrain
-x Make projectile collide with npc
-x Arm animation needs to slerp to new rotations and now instantly snap during walk and attacking.
-x Fix lagspikes in [chunk_sides_system] > 200ms
	- 200+ ms with locks..
	- 80ms without locks..
	- just make it generate at depth we are rendering instead of per level
	- flag levels as dirty when building, just dont update them until render lod changes
-x Map UI doesnt initialize properly
-x Sometimes the npcs labels dont spawn - name labels
-x noticed the item label didnt reduce when placing item
-x Crashed on start a few times randomly.. then stopped
	- tst stream_npcs segfaulted once ~11s in while terrain loaded (not under --gdb)
-> Use the flecs profiler to make sure chunk components are reduced properly
-> Focus on terrain chunks for now
-x More Components (to remove, when not in use)
    -x VoxelNodeDirty
    -x BuildChunkSides
    -x FindNeighbors
-x Remove systems when idle, by removing components
    - Chunks
        - ChunkTexturedBuildSystem [6ms]
        - ChunkMeshToggleSystem [6ms]
        - VodesSpawnSystem [2ms]
    - Lighting
        - chunk_mesh_colors_trigger_system [9ms]
        - SmoothLightsBuildSystem [8ms]
-x The next system in timing is Camera3RenderSystem [2ms]
-x Chunk3TexturedRenderSystem (11ms) + ElementRenderSystem (3ms)
-x FIX: Make arms never snap, just lerp the rotation over time
-x Flecs skipped all target cleanup for some deleted entities (ghost npc uis, dead links, labels on random objects)
	- cause, flecs v4.2.0: deleting an npc deletes its uis through (UIHolderLink, npc) (non fragmenting, OnDeleteTarget Delete)
	  inside flecs_on_delete_clear_ids, after the marked ids were counted, so the ids those deletes marked ((*, ui)) were dropped
	- the leaked (*, ui) record stayed MarkedForDelete, pair ids ignore generations, so the next entity recycling that id
	  skipped all (*, e) cleanup when deleted
	- fix: bsh/flecs_fix.sh "ZOXEL PATCH cleanup" runs those deletes in their own pass and repeats until no new ids
	- tests: links_cleanup 12/12 cases, stream_npcs no npc errors
-x Player spawn sometimes stalled (~1 in 3 new games), player never placed, terrain streaming stopped
	- cause: the town placer cleared the TerrainSpawnQueue with count = 0, its positions hash kept them,
	  so those columns were "already queued" forever, if the camera's column was one the player never spawned
	- fix: clear_TerrainSpawnQueue clears both, the hash also rehashes when tombstones fill it (lookups never ended)
	- tests: spawn_queue, player_spawn / stream_npcs dump the camera's column when streaming stops
	- the 8 - 20s frames seen while stalled were another build running at the same time
-x player_spawn test checks the character stands on ground, tst.sh --repeat <n> for intermittent bugs

-x We going to need to make two systems for voxel dirty and for when lod updates, to spawn new meshes - just use same iterator and pass it in
-x  Remove components when done - ChunkLodDirty
	-x ChunkMeshToggleSystem
	-x VodesSpawnSystem
	-x chunk_mesh_spawn_system
-x Glitchy Ass Terrain Meshes
-x Fix Chunks from mesh updating before their neighbors do
    -x use Ready systems and timers
-x Fix Chunks Lights flashing when updating
-x Integral test on falling through map
	- fly up
	- fall
	- lag spike
	- wham into ground
-x Fix low frame rates - clip through ground - by checking deltas between intersections
-x Fall through map
	- fix unstuck -
	- Make sure collision checks deltas
-x Lighting flashes when chunks update
	- stop mesh updating until its finished building lights
	- we shouldn't update mesh when terrain updates until lighting does
	- atm it flashes white during updates
-x doesnt spawn new chunk mesh ... again
	- test by building upwards generating, the chunk
-x Block didnt update when spawning on top of world
-x Add VoxelNodeLock for Octree Safety
	- Crashed on load...!
-x Placing twice in a row can crash it still
-x Let body parts meshes generate
	- so we can loot them!
-x Make sure all mesh code is out of characters, inside rendering or models where appropriate
-x Bodys' right arm doesnt swing when no action selected
-x Add Projectiles Damaging Terrain
-x Add Projects Damaging NPCs
-x Add Color of projectile, based on skill
-x This next patch: please refactor characters so that vox or mesh (instance reference too) are children to the character, this seperation is important to seperate concerns of modules too
	-x also cleanup old render data on skeleton characters
	-x skeleton shouldnt inherit vox too, needs to be seperate?
-x Refactor any old link code, that uses entity, into zox_link:
	-x ModelLink
	-x ModelLinks (half done)
	-x DataLink
	-x SkeletonLink
	-x ChunkLink
	-x ElementLink
	-x DeviceLink
	-x ZeviceLink
	-x PlayerLinks
	-x NoteLinks
	-x UIHolderLink
	-x ElementLinks
-x Refactor vox models to use a single vox entity, instead of per lod models + generation
	-x generate only at max detail
	-x spawn chunk mesh per lod level instead
	-x similar to how i refactored the terrain chunks into chunk + lod meshes
	-x It needs to also link up / work properly with all the games data, so this is a hard task
	-x we have imported models and generated models
	-x all the models and game data is created in the game module
-x Animate grass shader
	-x add grass noise animation, move only top verts, as a shader in res/shaders
	-x add setting for this, use the newer logic thats observer based
	-x so we can turn off grass animation
	-x since grass uses vox instance system, the hard part is making it work with the old and new shader, since other minivox/block voxes use this static shader
-x Bug: crashes when player character dies
-x Shake camera when hit - return to OG position after
	-x Create a animation entity that shakes camera for x seconds
	-x use observer for when player character is hit
-x More Components (to remove)
    -x BuildChunkSides
    -x Busy
    -x Ready
    -x RenderDepthDirty
    -x RenderDistanceDirty
    -x GenerateChunk
    -x GenerateLights
-x light_flood_system [7ms]
-x ChunkSidesSystem [5ms]
-x Target: 230fps at the current position
-x keep testing against old builds
-x Streaming gets 20ms spikes atm...
	-x debug with mangohud
-x Lower FPS, investigate the chunk meshes tables
    -x we can see by debugging pipelines
-x Add base stat "Physical Damage" that punch uses, strength should increase this, and use physical defence to decrease it, when damage is applied
-x Intelligence stat should increase mana, and Wisdom should increase mana regen, Strength increase health, Vitality increases health regen, endurance increases physical defence, Agility increases energy, dexterity increases energy regen.
-x When Level up + 3 stat points
-x The stat + button is not very visible, generate a texture for it
-x Add 1 skill point and 3 stat points to player character when level up
	-x add this base stat to realm stats too
	-x make a currency stat type
	-x add skillpoints and stat points
	-x when stat points added, you can see a + button added to top left corner of attributes
	-x click these plus buttons to decrease stat points and increase the attributes
-x consumable items (cokie) shouldnt select terrain (placement gizmo)
-x Fix light flickering issue
-x regen not using proper stats
	-x make regening state depend on linked regen stat
-x regen should be based on regen stat thats linked - StatLink
-x link our data / slot to the ui frame
    - icons/texts will just grab parent data links
-x spawn vox item in hand when selected
	- instead of cube
-x tag our humanoid with 'QuestGiver' so only they can handout quests
-x reduce health regen
	-x when in combat
-x github contributors didnt update
	- remove old ones
	- try push to new project?
-x Fix Stat UI Labels - the icon label, needs to show value and update when stat updates - using observer for when stat dirty (data dirty? i forgot)
-x Refactor LayoutDirty
-x IconOverlayLink
-x SlotLink
-x DraggerLink
-x DraggedLink
-x NodeLink
-x write giveall quests test
-x mirror mesh doesnt update when game model does
	- make a system that grabs the mesh clone link
	- when it updates, we can update our mesh clone too
	- link it to the characters mesh
-x Add GiveAllQuests cheat function to the test ui, like GiveAllItems
-x Fix Dialogue Animation
	-x Dialogue animated text is buggy atm
-x quantity of item didnt update when moving items
-x multiply mvp matrix in render systems before the upload, instead of doing per gpu
-x Upgrade Flecs to 4.2
	-x skybox missing
	-x destroy cube missing
-x fix fall through map
	-x make test fall button
	-x do interpolated collision checks
-x refactor block sound into a observer event
-x AttachDirty
-x WindowLayerDirty
-x InsideBlockDirty
-x WindowSizeDirty
-x DataDirty
-x LayoutPositionDirty
-x LayoutSizeDirty
-x SkeletonDirty
-x GridDirty
-x ListDirty
-x ListPositionDirty
-x Why is npc quest not QuestGiving tagged?
-x Quest Tracker ui under map
-x click on quest to link / unlink quest to tracker
-x add QuestIcon tag
-x Add a Clicked event, like our select event
-x update tracker ui when quest is dirty
-x show overlay when tracking quest
-x When give quest...
-x When spawn marker, link to player characters quest
-x When give quest to player, link it to marker then
-x quest marker not updated
-x if they are doing quest, update it, observe its quest
-x remove marker when quest is completed
-x spawn vox mesh in hands
-x vox item textures did not generate
    -x pickup grass
    -x notice no texture
-x item label didnt update when it was removed
-x item pickedup was a block item - test hat pickups
-x item didnt stack when picked up and dropped in ui
-x actions issues
	-x active state not action index
	-x when close/open after pickup item, it will spawn them at wrong indexes - the pickup go to 10th ui
	-x and then it didnt show in hand - well it did for2, just not 10
	-x action indexes off when selecting
-x now body uis arnt showing...
-x hats model links broken - spawns cube
-x texture block is broken too
-x block vox - broken too - no flowers or stalks shown
-x grass not spawning atm in terrain
    - block voxes..
-x cokie didnt add when pickedup
-x placed label didnt always update
-x item pickup didnt go to actionbar
-x item tooltip didnt update when stacked
-x npc hits arnt hitting back - or raycast fails?
-x stats issues
	-x first hit on npc - 3 damage, shows -1 on healthbar
	-x healtbar getting stuck when npc dies now
	-o recently changed the way statdirty works
-x tooltip now not showing..
	-x game icons - itll dissapear
	-x gone in taskbar too
-x crashes sometimes when npc dies
-x Wood, Sand textures didn't update
	- sometimes itll happen now
	- i made it stop tilemap atm
-x Add conditional node, and link it to two speech nodes
	- condition is if the speaker A has the quest
-x Add dialoguenode for handing in quest
-x slime 50% chance drop cookies
-x 45% should be drop dirt
-x 5% drop hat
-x just fix the missing textures now
	- item textures missing off block voxes
	- sand wasnt added to tilemap too
-x block voxes are offset by half a block now
	- i changed the spawning so maybe thats why
	- fix position of them
-x we should centre them,  and just reposition them when placing in terrain
	- for block voxes
-x World Item - Vox Mesh
	-x use model linked to block for this
	-x grab the top model i guess
-x Refactor node events and links
-x Add conditional dialogue node for quest
	- when node activate, use observer for checking HasQuestNode
	- NodeTrue added -> triggering it to go to the TrueLink
	- NodeFalse added -> triggering it to go to False Link
	- This is node module works
	- but HasQuest implemented in quests events
-x spawn vox item pickup
    - use mesh clone by linking blocks model
-x Set a NextNodeLink on NodeRun
-x Our conditional events will just set that and trigger end
	- if no next linked, it just picks first, as is the current behaviour
-x test for mesh clone of character
## Observers
-x We can add a On Tag observer for dirty tags
-x we can use filter ecs on add for the tag
-x the issue is how do we add this per entity prefab?
# NOTE: Relationships fragment by default (including ChildOf)
-x Bug: Spawning health on realm character errors
    - with the non fragmenting parenting set
-x Add spinlock onto all ChunkSides access
-x Add VoxelNodeLocks onto any read/wrists
    - ChunkSidesSystem - needs to use neighbors, and push spinlocks into octree functions
-x remove TilemapIndex and use regular index since relations are ordered
-x Lighting node at top suddenly dark - only when sunlight and reduce are on too
-x When the light octree increases scale, it doesnt get blacked out? this causes bigger lights to lightup house sides
-x NPC stat spawning crashes in character
	- doesnt crash when gdb on khadas, just rund
-x item quantity ui not updating properly
-x slot index for action is off
-x item not showing now when  i pickup
	- issue is the state logic changed in slot->ui
-x action ui not sync with children indexes
	- logici of action changed
	- pickup item, it appears on 10th item
	- just find empty slot, check action indexes?
-x Slot Refactors
	-x DataDirty
	-x SlotLink
	- Convert systems to observers
		- IconTooltipSystem
		- IconLabelSystem
	- UI is connected to Slot
	- Slot is connected to Data
	-x atm - just add DataDirty  onto ui, from system, when detecting slot is dirty
-x SkeletonMeshDirty
-x GlyphDirty
-x TextDirty
-x BlocksDirty
-x TextureLinks
-x TextureLink
-x Android issue might be with restore/dispose resources
    - its happening on pc the glitchy
    - android build is broken - graphics glitching
-x steamdeck build broken...
-x Fix icon not updating sometimes
    -x pickup item event
-x Mouse scrollwheel broken for switching items
Fix Crashes: (Infrequent)
-x On Player death
-x On load
-x Crashes when opens inventory (after allitems)
-x Crashes on end game (sometimes)
-x Crashes when i spawn stat onto character
	- Bug: flecs.c: 20794: abort(): cannot change children of prefab 'app_sdl_2647.game_2646.realm_8589938051.Boney_4096' after it has been instantiated (INVALID_OPERATION)
-x QuantityDirty
-x Block didnt update again... this was mesh not spawning issue
-x Block (regular) didnt remove/add
	- loaded game
	- removed block
	- it didnt dissapeart
-x Crashes - wrap all octrees - might be more
-x lag spikes now that its wrapped
    - i think the wraps stopped bugs though
    - but lag spikes are back damn it
-x Crash on exit game
    -x fatal: flecs.c: 20794: abort(): cannot change children of prefab 'realm_8589938080.Boney_4124' after it has been instantiated (INVALID_OPERATION)
    - I think we fixed this by reverting most transforms back to relationships
-x Crash on start
	-x loaded game
	-x crashed
-x action selection didnt work
    - press 2
    - observe no selection ui switch
-x Fix scrollwheels
-x Fix actionbar ui setting
    - frames are missing off button?
-x make realm / game / app seperate, all should have no parent, since our links are better now
-x Add custom Phases -> EcsOnUpdate -> zoxp_update
-x Add links back for active mesh, fuck the rules
-x Body lagging still behind camera
    - Only sometimes?
    - Suddenly, body lagging again
    - pipelines should be fine though
-x Billboard lags a frame as it needs position/rotation off cameras
    - then it will impact all children of it 
    - for now we can just recursively set Rotation3D
    - tried this, needs to reposition though
-x HandBoneLink
-x ShoulderBoneLink
-x HeadBoneLink
-x Fix queue for chunk updates - block placement broken again
    -x Block isnt placing after killed npc
-x Crashed on open Inventory again
-x Humanoid NPCs not spawning
-x Make custom phases
    - input
    - state
    - physics
        - forces
        - apply
        - collision
    - transforms
    - cameras
    - rendering
-x ChunkLodDirty stuck
    -x vodes_spawn_system shows active
    -x chunk showed preparing and active mesh as same
    -x timer showed constant reset
-x Add LightLock in build_smooth_lights_system
-x Fix: Camera lags behind body one frame
    -x ...? easiest way to test is to fly up and drop
-x GameLink
-x PlayerLink
-x MaterialLink
-x ShaderLink
-x LastDamager
-x Lag: When streaming it spikes 30-50ms
-x Debugging lagspike
    - removed BuildMesh
    - removed PreInitialize
        - spawn_prefab_mesh3
        - spawn_prefab_chunk_textured
-x make a lod update queue
    -x tunk_lod_system
        - we can just add in stream system
        - can we get an update from hashmap
        - we will need to override previous entries in lod update queue
    -x update them over time from terrain
    -x this solves the issue of NxM complexity on terrain updates vs frames
    -x main issue isnt system timings, its table update rates on chunks
-x Lag spike between streaming
    - the remove/add for events is failing
    - it works to reduce querying though
    - we can just add a generic Spawning state
        - initialize can be first state
        - can we add generic callbacks for initializing
            - replace the gpu initialization with callbacks on mesh
        - this creates 1 component per chunk/mesh that removes when down
        - as they process at different speeds, this will be removed at end of their processing
-x Bug: Map UI doesnt initially generate
-x RealmLink
-x Bug: Fog Color of camera not set
-x Fix skybox, make sure fog color blends properly though
-x Refactor ChunkMesh to use Non fragment parenting
    -x atm creates 4000 tables
-x Optimize characters_can_spawn_system
    x- check only when depth changes
-x Bug: Fog color not sky color
-x GizmoLink
-x SkillResourceLink
-x ParticlesEmitterLink
-x SpawnerLink
-x TilemapLink
-x TerrainLink
-x BiomeLink
-x Block Damage Overlay
	-x use custom material
	-x prefab_cube_overlay
-x Block Overlay to use depth change instead
	-x set depth at 0
	-x use depth offset in render system for it
-x MeshLink
-x VoxLink
-x Remove clear systems realm_clear_system
-x SkillLink
-x PlaylistLink/PlaylistLinks
-x AppLink
-x CanvasLink
-x CharacterLink
-x RegionLink
-x TunkLink
-x PlayerLink
-x CameraLink
-x Tag quests, some will be Greetings, others Quester?
-x Add quest (GiveQuest) on npc - 5% chance
-x Add dialoguenode for giving me quest
    -x add NodeListener
    -x Add NodeGiveQuest tag
    -x Add prefab event for giving quest
    -x Link the NodeProcess (dialogue) to character (if not)
    -x The event can check node process's current node and grab its quest
## Mirror UI
-x Make it so camera can filter an object itself, instead of just the object filtering camera?
    -x Make a renderer clone that just copies another entities mesh.. somehow
-x Destroy Render Camera when the ui dies - just parent it?
-x Fix taskbar linking
-x Loading screen now that we've slowed systems down
	-x Add events for game state
	-x Add loading screen spawn/destroy on those
-x Landfill LOD issues
	- When loading map?
	- missmatch of tunk / chunk
-x GenerateChunk not running when increasing depths
-x fix prior refactor bugs...!
    -x issue is initialize needs time to process
    -x i tried spawn at start of ppipeline but it breaks
-x VoxelDirty made into tag
-x used links for chunks active mesh instead of children
- Also lag spikes, huge
-x Now... chunk meshses lag the game
    - disable meshes - normal fps
    - enabled meshes - 40 fps... ffujckc
-x Lod Increase - Generation isnt triggering
    - probably state not detected
-x placing crashes / freezes it
-x Write test to spawn realm
-x Write test to spawn streamer + terrain
-x Test to spawn query and print the tables
-x if theres multiple streamers with different levels
    -x idk... why this breaks for new game
    -x oh i think it uses position for has, we should remove previous position if level is higher then
    -x we clear atm which is fine for now
-x Fix Depth Invalid issues - when i added queue
	-x perhaps its due to lod setting earlier
-x Add listener component on Game
    -x GameStateEvent
    -x Passes in game event with function
-x Add loading screen event
-x Vodes spawning / grass placement broken
-x Also placing block bricked game - infinite loop
Removing Links
	-x DialoguetreeLinks
	-x NodegraphLinks
	-x just use children by tags
-x Loading UI
-x Fix Taskbar events on close
-x Refactor add_taskbar_button - just spawn as children of taskbar prefab
-x Add mirror ui - player render texture
-x Fix Dialogue font character flickering
-x Dungeon placement also gets stuck in infiinite loop
	-x i think its the placement queue
-x Fix Chunk Generate LOD issues
	-x TunkLod is behind Chunks
	-x i think this waas due to loading then
-x Remove SkillLinks, StatLinks, QuestLinks
-x statbar2D isnt changing now too
-x Statsbar isnt shrinking
-x Add editor uis over top of canvas layer
-x Test XR with --log - instantiate
-x fix character particles
-x Add frame recording in System with filter set as 0 (update system)    
-x Remove SDL window when using XR
-x we should rotate the input by the curretn transform
-x for now just movee the xr code all into helpers
-x then jst call same functions in the xr render system
-x test to get a clear sky
-x For now give sunlight beam a budget per light set
	-x Make top chunk generate positions instead of beaming them all too
-x Remove sunlight queue, just pass down entire chunk stack!
	-x this saves us time
	-x we can run this from a Tunk instead of chunk
-x Remove the lag on the fps display, kinda annoying
-x healtbar cannot read it
-x make Initialize a tag - dontfragment
-x Initialize gets removed in same frame as spawn
    -x Make a pre spawn update where it gets removed
-x Skeleton mesh doesnt render
    -x Initialize
-x Sometimes chunk meshes dont update
-x Big lagspikes during Streaming
-x use entity component instead of links for chunks
-x Fix Mouse UI
-x Skeleton Weights didnt upload
-x Fix Terrain loading at all
-x Move the test uis into Debug
-x Add spawn realm test
-x Add spawn terrain test
-x Mesh didnt transition sometimes
	-x just stayed disable
	-x add active/preparing to debug ui
-x Voddes didnt die on chunk deaths
-x Create a nonfragtag macro
-x Fix render texture restoring
-x sdl2 linux broken
    -x sdl3 window not working anymore
    + was a libs issue
-x player3d_trigger_system:
> The log shows that sometimes doesnt get called, i.e. resets faster
    [13:25:38] Raw Bumper [1]: [1.000000] - button [0]
    [13:25:38] RT Pressed
    [13:25:38] Raw Bumper [1]: [1.000000] - button [2]
    [13:25:39] Raw Bumper [1]: [1.000000] - button [2]
    [13:25:43] Raw Bumper [1]: [1.000000] - button [0]
    [13:25:43] Raw Bumper [1]: [1.000000] - button [2]
-x I put reset pipelines one pipeline greater
-x Tooltip refactor
    -x link to a ui
    -x when not linked, we can close the tooltip (TooltipCloseSystem)
-x Noticed gamepad - joining - doesnt work (turning on)
-x Gamepad click button now broken - clicking selected
    -x used to be device_click_system
    -x target should of been set in zevice though - but i dont think buttons have this component - only pointers
-x Punch always activates when using touch buttons
-x Add icons to the Touch uis
-x Touch uis too small when phone rotated action should work no matter what
-x Make Touch Buttons bigger
-x Fix JumpState - needs to be set deterministically
    -x Make jump button always work - the jump
-x When we click, the virtual joystick spawns, it uses last position of finger, not good
-x make /bsh/android.sh defaults so we dont need variables every time
-x Change game name on android
-x Fix terrain mesh not restoring
-x Almost fixed, just need to link app to cameras without parenting
    -x just use relationships here! a tag that links app to camera, then query for those
    -x as game camera gets reparented to the character
-x Fix dungeon core texture issues
-x Fix DungeonCore not showing texture properly
-x Sometimes map arrow is off - the rotation is wrong...
-x Mesh didn't show on NPC randomly
-x Main issue with collisions is how it decides on which face we collided with, using a distance check, so you can teleport a little if its the wrong face, best to use velocity to determine face, last position
-x Collisions: When going underneath a block, itll detect the collision of DOWN face first before the proper face - Use velocity to decide face? - can calculate last position from velocity
-x Android Build - Fix on x86 machine
-x Add BarUI Test function
-x Issue with ui2D is transparency sorting
    -x we can set depth per layer but will need to sort calls
    -x took 255 calls to the same systems is not efficient...
    -x We can just throw them all into transform system and use that? less math on shaders too
    -x Then we can just sort in that single system based on layers
-x Fix set_octree_value Warnings
-x Fix Raycast Error: Invalid Vox warning
-x Sometimes npcs spawn in same chunk twice
-x Sometimes character missing a mesh too
-x Characters sometimes small - due to block depth
-x Characters sometimes fall through map
-x Segfault has been introduced when i added hat
	- occasionally breaks during games load now
	- its breaking when body rebuilds
	- disable body rebuilding and see if crashes
	- issue is it only sometimes breaks
	- memory leak somewhere
	- now it only crashes when not in gdb when re-enter game
-x Crashes atm if any other model swaps the chest - when shoulder is added
-x Higher Resolution mode is bugged for player model
	-x skinning issues
	-x max length chunk is 128, increase length..
	-x put depth to 5
-x now that body is bigger, we are seeing more physics glitching
-x Fix UI3D Test
-x Fix UI Empty textures
    -x theyre orange atm
-x Fix loading sounds in sdl3
-x Spawn Humanoid NPC test function
-x block placing still broken every 2nd place
-x Keep spawned Character links seperate from ChunkEntities, one for position ref, other for spawn links
-x Dialogue to use black topbottom bars instead of fade
-x move dialogue text top left
-x I noticed Slimes have some missing faces
-x I think I fucked up the chunk mesh lighting
    -x wait its just the camera being a bit laggy on the charcter collider
    -x even the ui overhead will glitch a little - make it slerp to position overheads
-x Fix the new blueprint + slime character workflow
-x Button doesnt work when click first time - touch issue
-x Get Android build working
-x Left touchpad - X axis inverted
-x No audio when no mixer - sdl 3
-x Fix the UI scaling issues
-x Fix the rotation too 
-x Fix loading sounds on sdl3
-x Get sdl3 to build for windows
-x Fullscreen issues
-x We should use lib folder for dll or .so files instead of bin
-x Added SDL3 Support
-x Make it so slime blueprint size is set in first node, others will use this blueprint component - like anchor it
-x add a ChunkSize setter function
    -x a get octree size function that scans the octree
-x Fix Statbars
    -x Statbar 2Ds - the frontbar are not appearing in front properly
    -x We can just anchor bar to left, make it easier maths
    -x Even better, we can just change the SizeAnchor instead of using bar specific stuff
-x Set ZigelPrefabLink and use same text systems
-x Fix Popups
-x Fix 3D uis - theyre missing
-x Fix NameLabels
-x Font difference - main difference is extra parenting
-x Find what overwrites the character position, so weird
-x Seems to be done  using scale though
-x Remove VoxelNodeDestroy hook:
    initialize_hook_on_destroyed_VoxelNode();
    add_hook_on_destroyed_VoxelNode(destroy_node_link_VoxelNode);
-x Reduce the octree macros, theyre scary atm
-x Fix lighting when rendering it through the render camera (characteer lights is off)
-x Top hat is missing from all items
-x Add color to camera - for background (clear color)
-x Fix Render Texture test
-x Crashing on windows still
-x RenderCameraInit system - with create_camera_rbo_and_fbo - Just use ScreenDimensions of camera
-x Crashes when test render texture
-x Fix head stuck in roof collision
-x Fix clipping into block when jumping from side
-x Cube in hand is gone
-x Fix Textures on UI, half not showing up in shader
	-x ... Something to do with UIMesh system
	-x Doesnt highlight ui refresh the texture?
-x Fix Terrain Chunks not uploading
-x .... Fix Chunks not working with UVs...
-x Make TextureDirty a byte
-x Remove TexturedMeshDirty
-x Make MeshDirty a byte and clear in systems that use it
-x use Size2 instead of LayoutSize for Raycasting UI
-x Children Position System for Transforms2
-x Convert layouts to use LocalPosition2 
-x Get skeleton to shrink
-x set Vignette on Dialogue insteaad of Blur
-x 'Skeleton Mesh invalid GPU Links' - use initializemesh
-x Make scales work with parenting
-x loaded chunk doesnt generate light properly
-x store spawned characters seperately to chunk entities
-x Fix colors missmatch on lighting
-x Fix chunk meshes being active at once
-x Fix occassionally text zigels not rendering
	-x this was debugged that they all stopped rendering for several frames
	-x visible when it dissapears too
-x Lighting Systems: Color Verts Missmatch warning
-x Zigel position is off when updating
    -x we can probably make a entity for each text line
	-x can just skip spaces, no need for extras too
-x Remember to disable max_process' skips when windows chunks
-x Refactor Vegetation to its module
-x Add text/font option tocenter the shape points in the grid (use for Markers)
-x slider text didnt update
-x Bug: IconLabel on dirt pickup didnt update
-x Do 3+ biomes instead of two that we have
-# Maybe it was grabbing the Voxels of side chunks that were generating?
    -x Landfill delay was cause of crash on windows
-# If i turn max_process on for Sides, i get weird missing chunk meshes again...
-x Fix Crash
    -x octree race conditions
    -x check octree locks
-x Mabybe just use another lock as component for such use of octree daym
    -x shit i had removed that
-x Switch UI camera to use orthographic project matrix
-x Hat doesnt spawn on heads - just doesnt get updated with mesh fast enough
-x Towns seem to spawn overlapping sometimes
-x VoxelNodeQueue VoxelDamageQueue VoxelDropQueue (add this)
    -x Block's Items not dropping, fix queues
    -x When place block - sometimes doesnt update
	    -x I think this was for air - just try build towers to test
	    -x After some testing, appears to just randomly miss the updates
	    -x Probably due to trigger not hitting - state miss
-x Map Textures for pieces are off - when no tunks
-x NPCs arnt spawning past initial - state issue with spawning npcs
-x I think its just not uploading mesh, debug that
-x Lighting broken between lods, state issue? used to rely on pipelines before
-x Load Game Borked ,,, or not idk
-x 3D bars arn't lerping size
-x 3D UIs / items broken
	-x NameLabels
	-x Healtbars
	-x Popups are gone
	-x Item in hand is missing too
-x Zigel Positions not always set atm for UIs
-x For Lights pass, add a BuildChunkMesh state to trigger light colors
	1 needs to update (ChunkLodDirty 4)
	2 cannot because it is waiting on build
	3 cannot build as its waiting to update..
-x Arm swing brokens - Probably transform changes
-x Landfill isn't the same for low - high depth
-x Landfill isn't the same for low - high depth
    -x we need better functions for increasing depth
    -x High res and low res should produce same results at scale
-x Lighting doesnt update after mesh does
    -x in some chunks only
    -x seems to be a Sunlight issue
-x Initialize - make byte from state - removing Increment systems
Bug!
-x Lesser chunks dissapear
-x UVs get messed up when flying around on chunks
-x Maybe it uploads before it's done generating?
	- Was Sides issues
-o Inside Chunk to work on camera instead of player character
-x fly around to outer chunks to debug them
-x display more data
-o render lines over mesh on chunk - as a debug method to show mesh itself - tests whether its mesh issue, or the renderer itself
Lets assume
-o Lod Updates
-o Spawns Mesh
-o Mesh updated
-o Somehow not rendering
Setting all chunks to build sides again
-x it rebuilds and shows mesh
-x then neighbor updates
-x then it dissapears
Disabling Lods means:
-x Chunk Mesh spawns once
-x No need to spawn new mesh links
-x Issue must be, sides is created per lod level - so if new sides is made and chunk generates, at wrong depth, it will create these issues?
	- but the data was the same so it doesnt support this - mesh was still there at full depth
-x Test Method:
	- fly around until chunk dissapears
	- fly into it
	- note the position
	- check debug logs for system processes
	- didnt seem to be rendering?
-x Issue occurs only when switching LODs
	-x spawning a new one didn't appear to work..
	-x it disabled last, spawned a new one butt...
-x Change all opengl data to gint and gint2 and guint and guint2
	- this might be issue
-x Okay its probaly race conditions with Side updates, mesh updates, etc
-x Make a toggle for disabling .. etc
-x Sides issues
-x Spawn Chunk Mesh when VoxelNodeDirty 
-x Build Mesh from ChunkMesh
-x Busy Tracks the chunk update from Generate to mesh building?
-x Lighting wasn't accounted for
-x Spawning Player before the chunks are generated, underneath map too
-x Make it so it only spawns ChunkMesh if there is voxels... if there is ChunkSides!
-x Make all chunk states bytes and set explictly
-x test zox_time_systems later as might of broke it during pipeline changes
-x BuildChunkMesh caused font textures to break
-x Disabling seemed to fix lag: Npcs
-x Make the statspanel text brighter so it stands out better
-x Remove corners and outlines from windows + headers so they are more blocky
-x Add option in tests to cycle through map debug modes
-x Fix game fade in, needs to do it after terrain loaded
-x Body appears to have visible facesi nside it
-x Refactor BodyDirty
-x finish maps reworks
    -x mountains
-x Finish Towns lod refactor
-x Bug: Map Arrow pointing wrong way
-x Towns are spawning differently every load...
-x Bug: Sometimes towns spawn inside one another?? weird af
-x StatsBars sometimes missing at start!
-x Bug: Some VoxTextures not created - Debris, Flowers, Decayed (theyre generated but not uploaded?? idk)
-x Spawn Regions in stream system - link it to tunk there
-x Bug: Item Text still there after we pickup item
-x Bug: Free Roam not working with f1 + right click
-x Bug: Hat dissapears when Swapping
    - x sometimes just doesnt appear??
-x Fix Map - make it use colors of voxels - raycast downwards in chunk
-x Fix Map Arrow Rotations
-x Fix the rotation make sure all faces align
-x Do i need to rotate around top face uvs?
-x Get Nearby Octree Value - using 27 chunks (for neighbors)
-x Reduce Lights based on brightest child node
-x Add Smooth Light to Options
-x Fix Sunlight not working when you approach town from afar after loading - does it generate lights there?
-x Improve lighting states for generation, make sure set in those systems
-x Fix lighting - houses it bugged - LOD difference in light propogations?
-x Towns are spawning differently every load...
-x Fix the rotation make sure all faces align
-x Do i need to rotate around top face uvs?
-x Get Nearby Octree Value - using 27 chunks (for neighbors)
-x Reduce Lights based on brightest child node
-x Add Smooth Light to Options
-x Fix Sunlight not working when you approach town from afar after loading - does it generate lights there?
-x Improve lighting states for generation, make sure set in those systems
    - also add back the color uploads for chunk meshes
-x Fix lighting - houses it bugged - LOD difference in light propogations?
-x Refactor Streaming to be simpler - TerrainStreamSystem
	-x position xz detected
	-x Update existing Lods
	- Spawn new Region
	-x Spawn new Tunks
	-x Spawn new Chunks
	-x Link while spawning
-x Refactor rest of block functions to model + block function
-x Refactor spawn_block_vox - spawn model seperately
	-x remove the struct use
-x Body: Arms not moving anymore - Debug WalkState
-x Towns: Generate Town Names - use generic name generation system, tag with GeneratedName, itll use Seed
-x Towns: Town Placement should avoid mountains
-x Towns: Place more Homes in each Town, use a position allocator for home spawning
-o Generate: Make Region Generation multiple frames state?
-x Towns: Spawn Houses using Town Place Types
-x Towns: Town Heightmap should average out more, smooth the whole thing based on average
-x Slider UI to show Text of Number
-x Disable next button until animating dialogue finishes
-x spawn town based on region + town data
-x spawn player in town
-x Simple Health Damage Overlay Cube - Transparent Mesh
-x Raycast Range should be Skill Range
	-x Change when skill is equiped - using actions
-x Simple Block Overlay Effect - alpha from health
-x Arm Swing when attackingblock
-x Add the Highest timing system on Profiler labels - Min/Max/System(SystemName) 13ms - for the last mintute there
-x BUG: Lower depth chunks have issues with npcs atm
-x Add simple transparent cube where damage is
-x set alpha based on health for WorldBlock entities
-x disable next button until dialogue animation finished
-x Use SdlJoystick instead of Joystick for better Compatibility
-x Block Damage Overlay
-x Redo the near RenderDistance Option and add a second option for it, Near Distance
-x Fix Towns at lower res
-x Fix Town/Moutnains spawning in other Regions - Debug Regions more
-x Test towns in other regions
-x Debug Region Features
-x Region Position < 0 needs to account better
-x When you walk into corner block you can push yourself through by turning - nvm it stopped when i turned up resolution of increments
-x Remove WallJump ability
-x When hitting block, it will teleport you to side
-x Check Collision Face is it facing the air, adjacent voxel checks
-x Sometimes body is dissapearing - culling? idk
	- RenderDisabled issue for AirChunk's
-x Clearly unstuck isnt working properly
-x Fix collisions
	-x make better code, simpler
	-x work with rotations
-x ChunkFrustumSystem last one (11ms)
-x If you are placing, show a preview placement block - change GizmoMode in Actions
-x Make Raycast Block Overlay cover all sides of the selected block
-x Add Text [float] to settings slider labels
-x Increase Options padding
-x Add tooltip for new game?
-x Refactor Options
-x Queue for BlockDamage
-x Transparent Cube for Block Damage
-x Use prefab for block_health
-x Move block health to vodes module
-x Fade out death for block health overlay (instead of instant)
+ Chunks at 15000 gives 10ms ish per chunk system:
+ Chunk3TexturedRenderSystem, ChunkFrustumSystem, chunk_neighbor_light_trigger_system, Chunk3NeighborMeshTriggerSystem, ChunkSpawnSystem, ChunkFindNeighborSystem, chunk3_death_system, Characters3SpawnZoneSystem
+ Make sure if chunk isn't doing anything that it sleeps, i.e. just a quick Dirty check, don't do anything else like zox_getv etc
-x I can make it so FindNeighborSystem works on chunk init, and it also sets neighbors to them as well, this eliminates per frame calls
-x TunkLinkSystem was taking 3ms... now 0.3ms
-x Make Towns not close to Region Borders
-x Make mountains more stone, less dirty
-x Make Towns less Grassy, more Rough Dirt + dirt
-x Add Minimap
-x Atm it just breaks when loading at player state 3
-x map UI - show tunk heightmap for now
-x map icons - and character ones - show player arrow overlay
-x refactor: towns tunk and chunk3 systems into its own module, self contained addons
-x Spawn Minimap
-x Convert Chunk to MapTexture
-x Sound on new character, chanced
	-x Add Sound chance to each new animated zigel
-x Disable Next button until it finishes
-x Dialogue speed should be per character and not per animation line
-x Use SdlJoystick instead of Joystick for better compatibility
-x Mountains to be more stone and less dirty
-x Use a floodfill algorithm to build a list of chunks that are conneected and updating and use this to check
    -x is ChunkGroup still updating?
    -x This is better than entire terrain as it allows faster updates
-x Somehow the Town dissapeared
-x The Humanoid Mesh dissapeared
+ Added slow fps mode to debug this flashing mesh
+ It is mostly fixed hacky fix that checks neighbor loading and slows it
+ I could just put a queue for updates and reduce it when theyre done - wayy to track when i should allow them all to update
-x Fix flashing chunks when they update mesh
    -x The issue is when bigger chunks dissapear, it will leave them empty, but the ones
-x Fix Lights Flashing Dark too!
-x Why isnt light passing through grass?
-x Fix lighting again, now that ihave gate its easy to see it doesnt always light properly
-x Bigger octrees to use brightest neighbor for lighting
-x Fix multiple stream points working
-x Swing Arms for walking
-x Raise arm more when attacking
-x Base arm swing speed on walk state
-x RaiseTime - for arm rotation to swing up more
-x Start Animation when activating punch
-x Fix Vegetation map for lower Lods
-x Seed Debug UI - to see all seeds available: Realm, Region, Player Character, Towns, etc
-x Set Debugger UI from a list (TestUI List)
-x Disable movement if ouitside chunks
-x Spawn Player in Town - pick random one in first region
-x Spawn Towns per Region - link to Town Placecment
-x Spawn Towns per Region
-x Link Region to Tunks
-x not seeing many npcs atm...
-x Sometimes some blocks loaded where my grass was
	-x this might be a indexing issue where one biome loads before the realm blocks? or vice versa?
	- xlooks like it, it swapped again
-x Fix Sunlight by delaying until all chunks are spawned in that column
-x Add Slot tooltips to body/equip item
-x Add outlines to VoxTextures (body parts)
-x Make ui selected use seperate colors for Active/Highlighted/Disabled etc
-x Remove DotLinks
-x Fix NPC RaycastRange
-x Respawn Start should remove game uis
-x Should not be able to unpause game when we are respawning
-x Spawn the test render texture in a window container
-x load ui for a save game
-x shape data on a node will go through with a vox data to generate sphere for now
-x add eyes after using a fill node
-x first chunk / tunk to move from controllers to stream begin systems
-x spawn chest and head items and add to body ui
-x spawn arm bones
-x town layers walls
-x spawn/destroy regions per 32x32 tunks
-x disable pickup part if it has attached parts
-x if pickup part - refresh body model
-x generate head position from slot position of chest, and slot position of head
-x handle increasing depth for > 32^3 max size
-x Spawn bones from item slot positions
-x NPC to hit back when combating me
-x Spawn 2 Thighs onto character, left + right
-x Spawn Shoulders Body Parts
-x Spawn Biceps Body Parts
-x Body Updates (using debug)
	-x when parts change
	-x remove / readd head to body
-x detects ui pickup in system
-x Update body when picking up head (in ui)
-x Item [Body] Pickups - only allow to pickup shoulder if bicep is up
-x Set body positions based on item anchor types dynamically
-x Equipment - Vox Layers
-x Combine vox TopHat onto body as well - Equipment layer
-x Center the vox textures for body parts
-x Generate better biome colors randomly - more chaos
-x Spawn RenderCamera + Render Texture + Render UI
-x Add test function for spawning a Colored Chunk
-x build linux
-x build windows
-x ui off on khadas
-x Generate a Chest Item for the realm bodys
-x generate texture for the chest item
-x create a head model / item
-x Unique Character Tag
-x Link the body parts together
-x combine the two character items into one model for use
-x add position and sizes to the combine list
-x bug: player body still smaller during low res mode
-x bug: hangs atm, due to paint size of eyes
-x bug: low res mode has eye issue, just sets entire head green
-x finish eyes on head of body
-x bug: blockdepth not savings / loading
-x bug: eye painting still not right
-x bug: flower block vox scaled wrong when lower depth
-x play sound when enter grass
-x unstuck should perfectly reposition above nearest ground
	-x atm if you flymode and teleport under, it moves you up randomly + 1
-x option for transparent meshes
-x option for painting weight
-x Sides Generation to use new sides data
-x Slice mesh properly for voxel faces
-x spawn particles when hit grass
-x Grass sound cuts off midway
-x add option for rendering bones
-x Render Part Position / Sizes
-x Spawn Part Position / Sizes from Parts Vox Combining
-X Hierarchy UI for realm and children
-x Inspector UI
	-x fix opening with target entity
	-x fix refreshing buttons like hierarchy system
-x color coding on component types
-x update component text system
	-x watch character position update in real-time
-x debug key to open inspector for player
-x Left Align Hierarchy / Inspector Buttons / List
-x Keep hierarchy elements selected - active state
-x Component Tooltips, e.g.:
	-x IconType [byte] [3]
-x Fix 'inside block' updating when on edge of block
	- hard physics issue
	- triggers position updates per frame
	- for now we just fixed by moving detection point
-x Fix bones they're half scale atm
-x Spawn camera onto headbone
-x Spawn Hips Body Part onto character
-x Handle falling through terrain
-x Fix tilemap errors
	-x Wood Texture is off atm - debug tilemap
-x Head position - transform - lags behind rest of body
	-x make hierarchy update recursively from root bones - just for skeleton for now
-x Fix free camera toggle with new parents / hierarchys
-x System that handles parenting - reparenting
	-x use for camera attachment
	-x Children links are more important as transforms are recursive
-x Transform layout data to transform so we can position the render textures
-x Filter the camera, so it only shows the cube
-x Test camera filter for multiple cubes
-x make editor text use fixed font res (FixToLayout) - a bit blurry atm
-x fixed raycasting for mesh alignments of centre
-x Remove padding in close button - big X
-x Try header text just black
-x Fix Icon Label Text (cant read)
-x Change outlines of windows to White
-x improve contrast with their texture fills
-x add BLUR to render texture post processor shader
-x Fix headless mode
	-x Move all OpenGL code to stubs for headless mode
	-x Remove sdl completely from project - headless mode is compiling without it
	-x linux-headless.sh
-x Fix realm / Terrain not deleting when game ends
	-x omg this was camera issue - didnt update it
-x Fix ./bsh/windows-headless.sh
-x Fix ./bsh/windows.sh
	-x use bsh/windows.sh
-x 3D uis are broken atm due to changes in parenting
-x Tooltip text isnt moving
-x Header text is missing for user uis
-x icon text all just dissapeared
-x tooltip doesnt dissapear when the ui dies - keep a link on it and react if it dies / becomes non selected
-x test particles
-x test function for character spawning
-x Spawn a Test UI, toggle it, add function in zoxel code
-X Find out why dialogue ui header is inside window 
-x Move first chunk spawning into StreamSpawnSystem - so it will spawn whatever we are inside of
	-x issue with first chunk raycasting at start
-x Fix rest of children bugs, including skill overlays
-x Refactor headers to window module
-x Move first chunk spawn to stream systems - StreamKickstartSystem
-x Fix new game UI (actionbar and stats ui) / Code cleanup
-x Fix DeviceGizmo icon
-x Fix NPC streaming
    -x issues on LODs
    -x issues on LOD Scales
-x Use proper flecs functions for core transform childrens, so it processes them all
-x Move camera first for respawning
-x Refactor Skills to use Children and no Users
-x Replace all find_random_position_on_ground with getv_VoxelNode's
-x Remove all old chunk getters/setters and just use latest ones
-x Fix Skillbook to use datagrid3
-x Finish Slot Links Refactor for UI
-x Refactor ActionLinks
-x Test Quest Tooltips before
-x Refactor Quests to use new parenting
-x Fix Skill Indexing though for Skillbook
-x Fix Item Stacking when picking up items
-x Handle the icon dropping into other Frames
	-x Set a Frame Tag?
	-x Only can place Skill in Skillbook or Actions
	-x Only can place Item in Inventory or Actions
-x Item Stacking when dropping item onto another - combine two items of same prefab type together
-x remove struct use spawn_datagrid spawn_datagrid3 as well
-x Npcs spawning in lesser chunks - just disable this for now, collision broken there
-x Slimes not adapting to block vox depths
-o fix collisions in lesser resolution terrain chunks
	-o test by forcing all terrain chunks at a sub res for easier testing
-x Skills UI - harder refactor
    -x Needs to account for UI Slot Indexes
    -x Create a component that stores entities array on character, this can be used for the UIs itself (when ui is closed/opened)
    -x Or we create a UI child for character, basically just keeps UI references, slots to items themselves
-x Spawn Held Item
-x when hit block - create entity for block health
-x Make user data ChildOf relationships
-x Remake Character Meta to be actual Characters
-x remove use of list window / list ui, just spawn elements into a listview and have system handle the rest
-x Refactor PartLinks
-x Refactor EquipLinks
-x region stream position / detect system
-x spawn regions
-x Center the vox textures, for body parts
-x Make parts attach to previous parts, not to entire body dimensions
-x Camera lock toggles when removing hat from ui
-x Create Slots for Body / Equip
-x Grass Texture causes crash
	- generating vox / texture conflicts?
-x Disabled decor blocks in realm gen
-x Grass model in between others messes up indexing for block -> uvs for second biome
	- put individual uvs on each block instead
	- refactor tilemap uvs onto block dirty system
	- tilemap_generation_system generates here - just add individual uvs here too for quick fix
	- Chunk3TexturedBuildSystem - get the block uvs here - entitymap
-x Sometimes vegetation didn't generate
-x Fix Chunk Loading
-x NPCs didnt spawn?
-x Mesh isnt updating when render depth updates perfectly
-x Obsidian not found
-x Dungeon cores not??
-x Popups now dont face camera
+ Made some octree component macros for non type ones
+ Fixed block index setting after they update
+ Refactored landfill out of Grasslands for better generation flow
+ SoilGrass now drops soil block
-x Remove any global byte refs to blocks and use biome ones or tags
-x Seems to be issues on frustum culling, debug terrain chunk sizes
-x Refactor Blocks Hook out, used by dungeons atm
-x Move slots module up to src from ui module
-x spawn slime model from shapes3 data
-x Generate unique color per biome
-x Refactor more blocks into per biome
-x Grass placed over town bricks, check for air for grass placement
-x B to close menu - shortcut6312
-x Fix controller mapping
-x Spawn a Colored Chunk as a test function
-x Remove all DeviceLinks and use Children of player
-x issue is the gamepads links to players atm
-x Increase crosshair scale when can hit NPC!
-x hide stats panel and actionbar when dialogue
-x give blocks different health from meta
-x Sometimes NPCs dont spawn in chunk, debug chunk states
-x Tests render in software (mesa llvmpipe, SDL offscreen, --gles), a mali fault during a test crashed gnome-shell and logged the user out
	- tst.sh kills a software run that opens /dev/mali, --gpu / --visible to use the real gpu, --tst-timescale for slower runs
-x Npcs despawned and respawned when the camera bobbed across a chunk border (jumping, landing)
	- fix: spawn zones turn off one chunk past character_spawn_distance (hysteresis)
-x Chunks deleted any npc that walked into them when unloaded (ChunkEntities owned its entities, could delete the player too)
	- fix: ChunkEntities is weak, npcs are owned by their spawn chunk (ChunkCharacters)
-x Stream queue starved far updates (unloads, npc despawns) while the streamer kept moving, closest always went first
	- fix: kills first, then updates queued over 2s (oldest), then closest (remove_next_TerrainSpawnQueue)
-x Streaming speed followed the frame rate (1 update per frame), now stream_updates_per_second with a per frame cap
-x Continue stuck on the loading screen for some saves
	- cause: Loading waited for 95% of the near square of chunks, streaming only spawns a circle (int2_distance),
	  45 of 49 columns, 91.8%, so it never ended (position dependent)
	- fix: stream_end_system counts the near circle (get_stream_near_chunks_count)
	- test: zox_tst_save=<realm folder> bash bsh/tst.sh continue_game loads a copy of a real save
-x Onboarding doc for new users (doc/onboarding.md), linked from readme.md
-x Doc on command line flags for the binary (doc/engine/commands.md), linked from readme.md and onboarding
-x Some faces weren't culled between chunks (and some border faces went missing)
	- cause: side_infos (src/_/octrees/data.c) had every side flipped, so when a chunk's node was coarser than
	  its neighbor's voxels (lod chunks, lod seams), it checked the neighbor's far face instead of the touching one
	- fix: side_infos side is the neighbor's touching face (build_colored already did this via reverse_direction)
	- test: chunk_sides (src/chunks/chunks3/tst/sides.c), 6 directions x every render depth, lod seams both ways
-x Minivoxes (block voxes) are lit like the terrain around them, and relit when lights change
	- Brightness from the brightest of its voxel + 6 sides (minivox voxels stop light, so never get any), light / 255
	- instanced: model matrices now, the unused bottom row x carries brightness, shaders apply the camera
	- block_vox_lights_system relights a chunk's (and its neighbors') minivoxes on LightNodeDirty
	- fixed chunk_colors_trigger_system being inverted: a chunk's own light changes didn't recolor its own meshes
	- tests: block_vox_lights (game), block_vox_light_sampling (logic)
- Water splash (doc/tech/water_extras.md task 1)
	- InsideBlockDirty observer: a splash and sound falling into water (faster than 2 a second), a small one coming out
	- test: water_splash
- Tests out of controllers/tst, each to the lowest module it checks (doc/engine/coding_standards.md Test Placement)
	- controllers/tst keeps only controller tests (input, menus) and the player and world fixtures
	- terrain tests on air_chunk.c (no player), block_data_queue too; body_views a skeleton character, not the player's
	- add_item_to_slot_manager moved from pickups to items/fun (quests reached up for it)
	- tests: maths_matrices, maths_bounds, dynamic_arrays, transform_hierarchy, euler_limits, grass_overlap
- Tests made smaller, no game or player where they don't need one (about 2 to 12s each, a game start is 25s+)
	- characters/tst/character.c: a skeleton character in a realm of its own (footsteps on an air chunk floor)
	- npcs/tst: zox_tst_npc_world, npcs spawned in the void of a realm of its own (npc_skeletons, npc_quests,
		humanoid_leak, monster_leak, chest_weights, hit_sound_links)
	- tooltip_gone on the menus, item_sizes on a realm of its own, furniture_fillers and ground_removed on an air chunk
	- perf_fly logs the characters peak (InsideBlock), not npcs
-x UI 3Ds Lagging behind character 
    - the ordering was the problem
    - put trail_system, billboard_system, transform3_children_system onto main thread to temporary fix
    
-x Body lagging behind camera
    - make sure phases there.. seperate timings
    - write a simple cube append test
        - cube will move forward, camera will ride i
    - GOT IT: its actually todo with parenting
    - when another humanoid exists, it will grab the wrong bones
    - EcsParent vs EcsChildOf
-x I made EcsChildOf default for now
    - fixes body lagging behind camera
    - fixed npc stats
    - due to difference in prefab spawning

