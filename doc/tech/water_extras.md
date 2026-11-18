# Water extras: a step by step guide

Status 2026-11-18: all five are done (2, 3, 4 and 5 differently from below, see [water.md](water.md): Swimming,
Under water, Light).

Five things water still needs, each one a small task done on its own: **splash**, **water physics**, **the
underwater overlay**, **light dampening** and **sound dampening**. Read [water.md](water.md) first: it says how water
works now.

Do the tasks in the order below. Finish one (code, test, commit) before starting the next. Each task lists the
exact files, what to add, a test with the values it must see, and when it's done.

## Before you start: the rules of this code base

1. **One C file is the whole program.** src/main.c includes every module, in the order of src/modules.c. Never add
   `#include` lines for project files except in the `_.c` file of the folder you add a file to (look at how the
   folder's other files are included and copy that). A function must be defined above the code that calls it.
2. **Module order matters.** A module can only use what modules before it in src/modules.c define. Today:
   `rendering` (31), `sounds` (32), `blocks` (46), `chunks` (47), `physics` (48), `lights` (49), `models` (50),
   `particles` (58), `terrain` (60), `characters` (68). So code in `lights` can't use the `BlockWater` tag (it's in
   `terrain`), but it can use `BlockMaterial` and `zox_block_material_liquid` (they're in `blocks`).
3. **Use the zox_ wrappers**, never `ecs_` calls: `zox_get`, `zox_getv`, `zox_setv`, `zox_has`, `zox_add`,
   `zox_remove`, `zox_get_link`, `zox_valid`. Copy their use from the files named below.
4. **Systems** look like the ones in the files named below: `zox_sys_on_begin()`, `zox_sys_world()`,
   `zox_sys_begin()`, `zox_sys_in(T)` / `zox_sys_out(T)`, then a `for` over `it->count` with `zox_sys_e()`,
   `zox_sys_i(T, name)`, `zox_sys_o(T, name)`, then `zox_sys_on_end();` and `} zoxd_system(name_system);`. A system
   must also be registered in its folder's `sys/_.c` with `zox_system(...)` (or `zox_system_1(...)` when it writes
   other entities or spawns things: it then runs on the main thread). Every component you read must be in the list.
5. **Build and test** with one command, from the repository's root:
   `zox_tst_egl_device=0 bash bsh/tst.sh <test name>`. It builds, runs one test and prints PASSED or FAILED.
   Add `--nobuild` to run again without building. A failing test writes tst/<test name>.log: read it before
   changing anything. Never skip or weaken a test to make it pass.
6. **Commit** each finished task: `git status`, then `git add` each file by its path (never `git add -A`), then
   `git commit -m "<short lower case summary>"`. Put changes to doc/ in a separate commit. Don't push.
7. **Keep it small.** No new frameworks, no big refactors. Short functions, few macros. If something here doesn't
   match the code, stop and ask rather than guess.

## Facts you'll need

- **The water block** is the realm child with the `BlockWater` tag (src/terrain/com/_.c). Find it with
  `zox_get_child_by_id(world, realm, zox_id(BlockWater))`. Its voxel value is `zox_getv(water, BlockIndex)`.
- **What block a character is in:** every character has `InsideBlock` (a byte: the voxel value where its body is,
  0 for air) and `InsideBlockPosition` (int3). `inside_block_system` (src/terrain/collisions/sys/inside.c) sets
  them each frame and adds the tag `InsideBlockDirty` when they change. So "the character is in water" is
  `zox_getv(character, InsideBlock) == water_index`.
- **A voxel anywhere:** `terrain_voxel_at(world, terrain, block)` (src/chunks/chunks3/fun/voxel_at.c) returns the
  voxel value at a world block position, -1 if its chunk isn't loaded. A world position to a block position:
  `real_position_to_block_position(position, zox_getv(terrain, BlockScale))`.
- **Physics** (src/physics/physics3): `Velocity3D`, `Acceleration3D` and `Gravity3D` are float3 components.
  `gravity3_system` adds `Gravity3D` to `Acceleration3D` each frame. Characters have `Grounded` (byte) and
  `CanJump`, `JumpState` (src/characters/jump). `zox_delta_time` is the frame's seconds.
- **Particles:** `spawn_particle3D_emitter_at(world, position, emit_rate, bounds, color)`
  (src/particles/particles3/pre/emitter.c), then `zox_setv(emitter, DestroyInTime, seconds)` to end it. See
  `terrain_callback_inside_block` in src/terrain/collisions/eve/inside_block.c for one in use.
- **Sound effects:** the list is in src/sounds/dat/effects.c (an enum) and src/sounds/fun/effects.c (the table:
  name, instrument, lowest and highest note, shortest and longest seconds, quietest and loudest volume). Play one
  with `spawn_sound_effect(world, sound_effect_x, source_entity)` or `spawn_sound_effect_at(world, type, position)`.
- **Volumes:** src/sounds/set/volume.c, `get_volume_sfx()` and `get_volume_music()`: every sound and music note
  uses them.
- **Camera look:** a camera has `CameraBlur` and `CameraVignette` (floats, src/cameras/com/_.c), sent to the screen
  shader by `camera_material_system`; the game's values are in src/cameras/cameras3/_.c (`game_vignette` 1.0).
  `zox_lerp_float(camera, CameraVignette, value, seconds)` eases one (see src/controllers/controllers3/sys/dialogue_player.c).
  The fog: each camera's `FogColor`; `fog_density` in src/rendering/dat/camera_renderer.c.
- **The player's camera:** `main_cameras[0]` (src/cameras/set/_.c). The player's character:
  `zox_get_link(world, dbg_player, CharacterLink)`.
- **Light:** values 0 to 255, `darklight` 16 and `sunlight` 255 (src/lights/lights3/set/_.c). Sunlight falls down
  through blocks in `sunbeam` (src/lights/lights3/sys/lightbeam.c); light spreads in `flood_light`
  (src/lights/lights3/sys/flood.c). `update_light_solidity` (src/lights/lights3/fun/solidity.c) builds a table of
  which voxel values stop light.

## The test setup most tasks use

Tests live in src/controllers/tst. Copy from these:

- **A player in the world:** `zox_tst_player_spawn_ready(world)` returns 1 once the player has spawned. Then
  `zox_get_link(world, dbg_player, CharacterLink)` is the character.
- **A pool to stand in:** `water_block` (src/controllers/tst/water.c) finds an all air chunk over the player
  (`zox_tst_air_chunk`) and places blocks in it by adding `VoxelNodeUpdate`s to the chunk's `VoxelNodeQueue`. Build
  a pool the same way: a floor of bricks (`BlockBricks`), walls of bricks, water 3 blocks deep inside. Wait a
  second for it to be written. Then move the character into it: `zox_setv(character, Position3D, p)` and
  `zox_setv(character, Velocity3D, float3_zero)`. A block position to a world position: multiply by the terrain's
  `BlockScale` and add half a block; the chunk's own position is `ChunkPosition` times its length.
- **Waiting on time:** keep a `double` start time and compare with `zox_current_time`; return 0 to wait, 1 to end.
- **Registering a test:** in src/controllers/tst/_.c add `#include "your_file.c"` next to `#include "water.c"`,
  and `zox_tst_add("name", ready_function, check_function, 120);` next to the `water_block` line.
- **Checking:** `zox_tst_check(condition, "what went wrong [%i]", value);` and log numbers with
  `zox_log("[tst] [metric] ...")`.

---

## Task 1: splash

**Goal:** jumping or falling into water makes a splash: a burst of blue particles at the surface and a splash
sound. Climbing out makes a small one.

**Where:** a new file src/terrain/collisions/eve/water_splash.c, registered in src/terrain/collisions/eve/_.c.

**Steps:**

1. Add a sound. In src/sounds/dat/effects.c add `sound_effect_splash,` to the enum, just before the closing
   `sound_effect_end` entry (find it at the end of the enum). In src/sounds/fun/effects.c add a line to the table:
   `[sound_effect_splash] = { "splash", instrument_squelch, 34, 38, 0.3f, 0.4f, 1.8f, 2.2f },`
2. In water_splash.c write an observer function `water_splash_event(iter* it)` copying the shape of
   `terrain_callback_inside_block` (src/terrain/collisions/eve/inside_block.c). For each entity:
   - get its terrain (`zox_get_link(world, e, TerrainLink)`), its realm (`zox_get_parent(world, terrain)`), the
     water block and its index;
   - keep the last block it was in: add a component `zoxc_byte(InsideBlockLast);` in src/terrain/com/_.c (with
     `zoxd_byte(InsideBlockLast);` in that file's `zox_components_terrain`), and `zox_setv(e, InsideBlockLast, 0)`
     where characters get `InsideBlock` (src/characters/characters3/pre/character.c, next to
     `zox_setv(e, InsideBlock, 0);`);
   - entering: the last wasn't water and now it is water, and its `Velocity3D.y` is below -2: spawn the splash;
   - leaving: the last was water and now it isn't: a smaller splash (half the particles, quieter);
   - then set `InsideBlockLast` to the current `InsideBlock`.
3. The splash: `spawn_particle3D_emitter_at(world, position, 12, (float3) { 0.6f, 0.2f, 0.6f }, (color) { 120, 170, 240, 200 })`,
   then `zox_setv(emitter, DestroyInTime, 0.3f);`, and `spawn_sound_effect_at(world, sound_effect_splash, position);`.
   Put the position at the water's surface: the block position over the character's feet, its y the top of the
   water block it entered (`InsideBlockPosition.y + 1`, times the block scale).
4. Register it in src/terrain/collisions/eve/_.c like the one already there, with `zox_on_add` on the
   `InsideBlockDirty` tag, reading `[in] blocks.InsideBlock`, `[in] transforms3.Position3D`, `[in] physics3.Velocity3D`,
   `[out] terrains.InsideBlockLast`.

**Test:** src/controllers/tst/water_splash.c, test name `water_splash`.
- Build the pool. Put the character 4 blocks over the water's middle, its velocity 0. Count emitters
  (`zox_count_types(ParticlesEmitter)`) before it falls.
- Wait until `InsideBlock` is water (at most 3 seconds).
- Checks: at least one new particle emitter appeared within 0.5 seconds of entering; a log line from the splash.
  Log the fall speed it entered at.
- Then move the character straight up out of the water (2 blocks over the surface): a second, small splash.

**Done when:** `water_splash` passes 3 runs in a row, and `water_block`, `water_flow`, `terrain_collisions` still
pass.

---

## Task 2: water physics

**Goal:** in water a character sinks slowly, floats up when it jumps (swims), and moves slower.

**Where:** a new system in src/terrain/collisions/sys/water_physics.c, registered in
src/terrain/collisions/sys/_.c after `inside_block_system`.

**Steps:**

1. Add three numbers at the top of the file:
   - `float zox_water_gravity_scale = 0.25f;` (gravity in water, a quarter)
   - `float zox_water_drag = 3.0f;` (how fast speed fades in water, per second)
   - `float zox_water_swim_up = 4.0f;` (the up speed a jump gives in water)
2. Write `water_physics_system`, reading `[in] blocks.InsideBlock`, `[in] physics3.Gravity3D`,
   `[out] physics3.Velocity3D`, `[out] physics3.Acceleration3D`, `[out] jumps.CanJump`. Register it with
   `zox_system_1(water_physics_system, zoxp_physics, ...)` (`zoxp_physics` so it runs with the physics).
3. For each entity whose `InsideBlock` is the water index:
   - take back most of gravity: `acceleration->value = float3_add(acceleration->value, float3_scale(gravity->value, zox_water_gravity_scale - 1));`
   - slow it down, the same at any frame rate: `const float keep = expf(-zox_water_drag * zox_delta_time);` then
     `velocity->value = float3_scale(velocity->value, keep);`
   - let it jump again in water: `can_jump->value = 1;` (the jump system then lets space push it up)
4. Make a jump in water push up gently, not a full jump: in src/characters/jump/sys/force.c, where the jump force is
   added, if the character's `InsideBlock` is water, use `zox_water_swim_up` instead of `zox_jump_power`. Look at
   the file first: copy how it reads the character's components.

**Test:** src/controllers/tst/water_swim.c, test name `water_swim`.
- Build the pool (3 deep). Put the character at the water's top, velocity 0.
- Sinking: record its y now and after 1 second. Check it went down by less than half of what it would fall in air
  (fall in air for 1 second: 0.5 times gravity's y times 1 squared; read `Gravity3D`).
- Swimming: set `JumpState` to `jump_state_trigger` on the character 3 times, a second apart (as the jump system
  does on a key press). Check its y is higher than before the first, and that it never went past 3 blocks over the
  surface.
- Speed: give it `Velocity3D` of 5 along x; after 1 second check its x speed is under 1 (drag).

**Done when:** `water_swim` passes 3 runs in a row; `terrain_collisions`, `terrain_collisions_10fps`,
`terrain_collisions_30fps`, `player_spawn` and `water_splash` still pass.

---

## Task 3: the underwater overlay

**Goal:** with the camera under water, the view turns blue and murky: thick blue fog, a little blur, a stronger
vignette. It goes back when the camera comes out.

**Where:** a new system in src/terrain/sys/underwater.c, registered in src/terrain/sys/_.c.

**Steps:**

1. Add a tag `zox_tag(CameraUnderwater);` in src/terrain/com/_.c and `zoxd_tag(CameraUnderwater);` in that file's
   `zox_components_terrain`.
2. Write `camera_underwater_system`, reading `[in] transforms3.Position3D`, `[out] colorz.FogColor`,
   `[none] cameras.Camera3` (the 3D camera tag, src/cameras/com/_.c). Register it with
   `zox_system_1(camera_underwater_system, zoxp_update, ...)`.
3. For each camera: its terrain is `zox_get_link(world, e, StreamLink)` (skip if not valid). The voxel at the
   camera: `terrain_voxel_at(world, terrain, real_position_to_block_position(position->value, scale))`. It's under
   water if that's the water index.
4. Going under (no `CameraUnderwater` tag yet, now under water):
   - remember the fog to go back to: add a `zoxc_color_rgb(FogColorAbove);` component next to the tag, and set it
     to the camera's `FogColor` now;
   - `zox_add(e, CameraUnderwater);`
   - set `FogColor` to `(color_rgb) { 20, 60, 120 }`;
   - `zox_lerp_float(e, CameraVignette, 2.0f, 0.3f);` and `zox_lerp_float(e, CameraBlur, 0.35f, 0.3f);`
5. Coming out (has the tag, not under water now): `zox_remove(e, CameraUnderwater);`, set `FogColor` back to
   `FogColorAbove`, `zox_lerp_float(e, CameraVignette, game_vignette, 0.3f);`, `zox_lerp_float(e, CameraBlur, 0, 0.3f);`
6. Thicker fog under water: `fog_density` is one number for every camera (src/rendering/dat/camera_renderer.c),
   and rendering is included before terrain, so it can't see your tag. Do it with a global instead:
   - in camera_renderer.c add `byte renderer_camera_underwater = 0;` and `float fog_density_underwater = 0.25f;`
     next to `fog_density`;
   - in `get_fog_density()` use `fog_density_underwater` instead of `fog_density` when `renderer_camera_underwater`
     is 1;
   - in your system, for the player's camera (`main_cameras[0]`) only, set `renderer_camera_underwater` to 1 under
     water and 0 out of it.
7. Watch out: walking into another biome blends the sky and fog (src/weathers/sys/sky_blend.c). While under water
   that blend must not set the fog back: in your system, set the water fog colour every frame while under water.

**Test:** src/controllers/tst/underwater.c, test name `underwater`. Use the camera only setup of tst `sea_level`
(src/controllers/tst/water.c, `zox_tst_sea_level_ready`): a realm of its own, one viewport camera.
- Put the camera at the nearest open water's middle, 1 block under the sea (`terrain_sea_height` times
  `render_distance_y`, minus 1, times the block scale). Wait for its land to stream (as sea_level does).
- Checks: the camera has `CameraUnderwater`; its `FogColor` is the water fog; after 0.5 seconds its
  `CameraVignette` is over 1.8.
- Take a screenshot (`tst/underwater.ppm`, as sea_level does), then move the camera 3 blocks over the sea: after
  0.5 seconds `CameraUnderwater` is gone and the fog is back to what it was.
- Look at the screenshot (convert it: `magick tst/underwater.ppm tst/underwater.png`): it should be blue and hazy.

**Done when:** `underwater` passes 3 runs in a row; `sea_level`, `lakes`, `head_camera` and `screenshot` still pass.

---

## Task 4: light dampening

**Goal:** sunlight gets dimmer the deeper the water: each water block it passes takes 24 off it. A lake's floor is
darker than the beach.

**Where:** src/lights/lights3 (the `sunbeam` function and `light_beam_system` in sys/lightbeam.c, and fun/solidity.c).

**Steps:**

1. In src/lights/lights3/set/_.c add `byte light_water_decay = 24;`
2. In src/lights/lights3/fun/solidity.c, copy `update_light_solidity` into a new function
   `update_light_liquids(world, manager, realm, liquids)` that fills a byte table: `liquids[j] = 1` when block j has
   `BlockMaterial` equal to `zox_block_material_liquid`, else 0. (Read how `update_light_solidity` walks the blocks
   and copy it exactly; only the test inside the loop changes.)
3. In `light_beam_system` (sys/lightbeam.c), next to the `solidity` table it makes, make a `liquids` table the same
   size, `memset` it to 0, and fill it with `update_light_liquids` right after `update_light_solidity` is called.
   Pass it to `sunbeam` as a new last argument `const byte* liquids`.
4. In `sunbeam`, in the loop going down, after the solid check: if the voxel is a liquid
   (`voxel && liquids[voxel - 1]`), lower the light before setting it:
   `light = light > light_water_decay + min_light ? light - light_water_decay : min_light;`
   (the light set for this voxel and every one under it is then the lower value).
5. `sunbeam` may be called from more than one place: search for `sunbeam(` and pass `liquids` everywhere it's
   called.
6. Flood light (light spreading sideways) can stay as it is for now.

**Test:** src/lights/tst (look at src/lights/tst/_.c: tests there build octrees by hand, no world), or in a world
with the `lakes` setup. The simplest: extend tst `lakes` (src/controllers/tst/water.c `zox_tst_lakes_check`):
- read the light at the lake's middle just under its surface and at its floor (`getv_LightNode` on the chunk's
  `LightNode`, as the glow tests in src/controllers/tst/town_maps.c do);
- check the floor's light is lower than the surface's, and the surface's is lower than the light just over the
  water (in the air). Log all three.

**Done when:** the check passes on 3 seeds (`zox_tst_seed=1`, `2`, `5`); `smooth_lights`, `lod_lights`,
`glow_light`, `terrain_render` still pass.

---

## Task 5: sound dampening

**Goal:** with the camera under water, music and sound effects are quieter and duller, and come back when it's out.

**Where:** src/sounds/set/volume.c, and the underwater system from task 3.

**Steps:**

1. In src/sounds/set/volume.c add `float volume_underwater = 1.0f;` (1 above water, lower under it), and multiply
   it in: `return master_volume * volume_sfx * volume_underwater;` in `get_volume_sfx`, and the same in
   `get_volume_music`.
2. In `camera_underwater_system` (task 3), for the player's camera (`main_cameras[0]`) only, ease
   `volume_underwater` toward 0.35 under water and toward 1 out of it, a little each frame so it doesn't jump:
   `volume_underwater += (target - volume_underwater) * (1 - expf(-8 * zox_delta_time));`
3. Music already playing reads `get_volume_music()` when each note starts (src/musics/sys/play.c), so it gets
   quieter from the next note: that's fine.
4. Duller (optional, after the rest works): new sounds spawned under water could play an octave lower or with less
   high pitch. Leave this unless asked.

**Test:** extend tst `underwater` (task 3): after going under, wait 1 second and check `volume_underwater` is under
0.5; after coming out, wait 1 second and check it's over 0.9. Log it both times.

**Done when:** `underwater` passes 3 runs in a row; `sound_balance`, `hit_sounds` and `music_crossfade` still pass.

---

## When all five are done

- Add a line for each to doc/done.md under today's date, and update [water.md](water.md): move each one out of its
  "Not yet" list and add a short section saying how it works and which test covers it.
- Commit the docs on their own.
