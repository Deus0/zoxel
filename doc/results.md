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
