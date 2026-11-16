// tst: xr_menu, the XR controllers drive the 3D menus as a player would on the simulator (--xr-sim)
//  - the start menu is a 3D label; A on the right controller (scripted) switches the device mode to the
//      controllers (gamepad) and opens the main menu, a 3D list (Canvas3) among the canvas's children
//  - navigation selects its first button; the right stick down (the navigating hand) moves it to the next
//  - screenshots it (tst/xr_menu.ppm): the panel ahead in both eyes
//  - run: bash bsh/tst.sh xr_menu
int zox_tst_xr_menu_stage = 0;
int zox_tst_xr_menu_frames = 0;
entity zox_tst_xr_menu_first = 0;
entity zox_tst_xr_menu_second = 0;
byte zox_tst_xr_menu_opened = 0;

static inline void zox_tst_xr_menu_next() {
    zox_tst_xr_menu_stage++;
    zox_tst_xr_menu_frames = 0;
}

static inline entity zox_tst_xr_menu_target(ecs* world) {
    const entity right = xr_controllers[xr_hand_right];
    return zox_valid(right) ? zox_getv(right, EntityTarget) : 0;
}

byte zox_tst_xr_menu_ready(ecs* world) {
    const entity player = zox_players[0];
    if (!zox_valid(xr_camera) || !zox_valid(player)) {
        zox_tst_waiting = "the xr camera";
        return 0;
    }
    const entity canvas = zox_get_link(world, player, CanvasLink);
    zox_tst_xr_menu_frames++;
    switch (zox_tst_xr_menu_stage) {
        case 0:
            zox_tst_waiting = "the start menu";
            if (zox_valid(zox_get_link(world, player, MenuStart)) && zox_tst_xr_menu_frames > 30) {
                xr_sim_head_euler = float3_zero;
                xr_sim_script_a = 1;
                zox_tst_xr_menu_next();
            }
            return 0;
        case 1:
            zox_tst_waiting = "A to open the main menu";
            if (zox_tst_xr_menu_frames == 3) {
                xr_sim_script_a = 0;
            }
            if (zox_valid(zox_get_child_by_id(world, canvas, zox_id(MenuMain)))) {
                zox_tst_xr_menu_opened = 1;
                xr_sim_script_a = 0;
                zox_tst_xr_menu_next();
            }
            return 0;
        case 2:
            zox_tst_waiting = "navigation to select the first button";
            zox_tst_xr_menu_first = zox_tst_xr_menu_target(world);
            if (zox_valid(zox_tst_xr_menu_first) && zox_tst_xr_menu_frames > 10) {
                xr_sim_script_stick = (float2) { 0, -1 };
                zox_tst_xr_menu_next();
            }
            return 0;
        case 3:
            zox_tst_waiting = "the stick to select the next button";
            zox_tst_xr_menu_second = zox_tst_xr_menu_target(world);
            if (zox_tst_xr_menu_second != zox_tst_xr_menu_first || zox_tst_xr_menu_frames > 120) {
                xr_sim_script_stick = float2_zero;
                zox_tst_xr_menu_next();
            }
            return 0;
        case 4:
            zox_tst_waiting = "the screenshot";
            if (zox_tst_xr_menu_frames == 60) {
                remove(zox_tst_path("xr_menu.ppm"));
                snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("xr_menu.ppm"));
            }
            return zox_tst_xr_menu_frames > 60 && !zox_screenshot_request[0];
    }
    return 0;
}

void zox_tst_xr_menu_check(ecs* world) {
    const entity player = zox_players[0];
    const entity canvas = zox_get_link(world, player, CanvasLink);
    const entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuMain));
    zox_tst_check(zox_tst_xr_menu_opened, "A didn't open the main menu");
    zox_tst_check(zox_valid(menu) && zox_has(menu, Canvas3), "the main menu isn't a 3D list");
    zox_tst_check(zox_getv(player, DeviceMode) == zox_device_mode_gamepad, "the controllers didn't take the device mode [%i]", zox_getv(player, DeviceMode));
    zox_tst_check(zox_valid(zox_tst_xr_menu_first) && zox_has(zox_tst_xr_menu_first, Element3), "no 3D button selected");
    zox_tst_check(zox_valid(zox_tst_xr_menu_second) && zox_tst_xr_menu_second != zox_tst_xr_menu_first, "the stick didn't move the selection");
    zox_tst_check(zox_get_link(world, player, MenuStart) == 0 || !zox_valid(zox_get_link(world, player, MenuStart)), "the start menu stayed open");
}

void zox_tsts_xr_menu(ecs* world) {
    zox_tst_add("xr_menu", zox_tst_xr_menu_ready, zox_tst_xr_menu_check, 90);
    zox_tst_set_setup("xr_menu", zox_tst_xr_sim_setup, 1);
}
