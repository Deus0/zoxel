# Test Results

The latest test times, by module, to compare against (todo.md: optimize and reduce test times). Each run alone
(`bash bsh/tst.sh <name> --nobuild`, debug build, llvmpipe, deusmini), nothing else running. Times from
`tst/<name>.log`: boot is start to `[tst] running` (~0.3s for every test), own is from there to the result.

## src/_ (2026-11-24)
36 tests, all pass. Every one but these is under 1s with its boot:

| Test | Own time | Notes |
|---|---|---|
| pipeline_syncs | 17.4s | waits for a player in the whole game: flecs counts merges only between active systems, so it needs npcs, towns and terrain running (the test level or menus would hide them) |
| flecs_event_scaling | 4.0s (was 10.8s) | 10000 events a frame timed over 8 frames, not 40; entities bulk spawned |
| flecs_wildcard_remove | 0.7s | |

## transforms, apps/sdl, colors, nodes, cameras (2026-11-24)
23 tests, all pass, all under 1s own time. sdl_vulkan_surface is vulkan only (`--vulkan --visible`).

| Test | Own time | Notes |
|---|---|---|
| sdl_window_mode, sdl_window_restore | 0.45s (was 1.9s) | each toggle waits for the window's mode, then 4 frames, not 30 frames |
| camera_viewport_spawn | 0.8s | |

## rendering, sounds, animations, textures, geometry, musics, ui (2026-11-24)
36 tests, all pass.

| Test | Own time | Notes |
|---|---|---|
| gpu_restore_game | 17.8s | the whole game's gpu objects restored (block voxes, npcs, towns): kept in the game, gpu_restore is the quick one |
| skeleton_render | 4.5s (was 13.6s) | in the void: the player's skeleton, no npcs to wait for |
| vox_instance_render | 7.1s | in the game: the void has no instanced renderers |
| terrain_render | 6.4s | the test level (a world of its own is in controllers, above rendering) |
| music_theme | 4.1s (was 7.0s) | the test mixer synthesizes a repeated note once; the realm song's notes are nearly all different |
| sound_balance, music_compose | 3.4s, 2.6s | mixing realm songs (few repeated notes) |
| text_order | 1.7s | |

## Every realm (2026-11-24)
Realm generation takes ~3.5s in a debug build, ~3s of it wave function collapse (the 4 chook and 4 pengu critter
models, one after another in an observer on the main thread, models/eve/wfc.c): every test with a game or the void
pays it. wfc.c is built -O2 in debug builds too now (3.2s to 2.6s); on threads it'd be a fraction (asked, todo.md).
