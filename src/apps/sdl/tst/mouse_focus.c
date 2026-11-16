// tst: sdl_mouse_focus, when the mouse counts as in the window (sdl_mouse_in_window, inputs/sys/mouse.c)
//  - alt tabbing back into a fullscreen window, sdl (wayland) may not give it mouse focus till the pointer moves:
//    with keyboard focus and the pointer inside, it's in (else the ui raycast lost the pointer, stuck)
//  - run: bash bsh/tst.sh sdl_mouse_focus
byte zox_tst_sdl_mouse_focus_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_sdl_mouse_focus_check(ecs* world) {
    (void) world;
    const int2 size = { 1280, 720 };
    const int2 inside = { 640, 360 };
    zox_tst_check(sdl_mouse_in_window(1, 0, inside, size), "with mouse focus, it's not in the window");
    zox_tst_check(sdl_mouse_in_window(0, 1, inside, size), "alt tabbed back (keyboard focus, no mouse focus), it's not in the window");
    zox_tst_check(!sdl_mouse_in_window(0, 0, inside, size), "with no focus at all (alt tabbed away), it's still in the window");
    zox_tst_check(!sdl_mouse_in_window(0, 1, (int2) { -1, 360 }, size), "left of the window, it's in");
    zox_tst_check(!sdl_mouse_in_window(0, 1, (int2) { 640, 720 }, size), "below the window (at its height), it's in");
    zox_tst_check(sdl_mouse_in_window(0, 1, (int2) { 0, 0 }, size), "at the window's corner, it's not in");
}
