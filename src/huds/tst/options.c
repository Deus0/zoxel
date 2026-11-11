// tst: options_toggle, toggles a setting in the options menu with the fake mouse
//  - opens options, finds the first visible toggle (skipping window mode ones, i.e. Fullscreen)
//  - pressing it doesn't toggle yet (buttons click on release)
//  - releasing it toggles once: its ActiveState and its linked setting both flip
//  - clicking again flips both back
//  - settings save to the test's throwaway saves folder (--nosaves), never the user's
//  - run: bash bsh/tst.sh options_toggle
#define zox_tst_options_hold_frames 4
int zox_tst_options_stage = 0;
int zox_tst_options_frames = 0;
entity zox_tst_options_toggle = 0;
int2 zox_tst_options_position;
byte zox_tst_options_start_state = 0;

// first visible toggle under e, with a setting that's safe to flip in a test
entity zox_tst_options_find_toggle(ecs* world, entity e) {
    if (zox_has(e, Toggle) &&
        zox_has(e, SettingLink) &&
        zox_has(e, RenderDisabled) &&
        !zox_getv(e, RenderDisabled))
    {
        const entity setting = zox_getv(e, SettingLink);
        if (zox_valid(setting) && zox_has(setting, SettingByte)) {
            const char* name = zox_get(setting, ZoxName)->value;
            if (strcmp(name, "Fullscreen") && strcmp(name, "VSync")) {
                return e;
            }
        }
    }
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity found = zox_tst_options_find_toggle(world, it2.entities[j]);
            if (found) {
                return found;
            }
        }
    }
    return 0;
}

static inline byte zox_tst_options_setting(ecs* world, entity toggle) {
    return zox_getv(zox_getv(toggle, SettingLink), SettingByte);
}

byte zox_tst_options_toggle_ready(ecs* world) {
    const entity player = dbg_player;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_options_frames++;
    const entity main_menu = zox_tst_menu(world, canvas, zox_id(MenuMain));
    const entity options = zox_tst_menu(world, canvas, zox_id(MenuOptions));
    const entity toggle = zox_tst_options_toggle;
    switch (zox_tst_options_stage) {
        case 0:
            // clicks until the start menu opens the main menu, like a player
            zox_tst_waiting = "a mouse click to open the main menu";
            zox_tst_mouse((int2) { 100, 100 }, (zox_tst_options_frames / zox_tst_options_hold_frames) % 2);
            if (!main_menu) {
                return 0;
            }
            break;
        case 1:
            zox_tst_waiting = "the main menu to settle";
            zox_tst_mouse((int2) { 100, 100 }, 0);
            if (zox_tst_options_frames < zox_tst_options_hold_frames) {
                return 0;
            }
            if (!zox_tst_menus_click(world, player, main_menu, &button_event_menu_options, "options")) {
                return 1;
            }
            break;
        case 2: {
            zox_tst_waiting = "the options menu's layout";
            if (!options || zox_tst_options_frames < 10) {
                return 0;
            }
            zox_tst_options_toggle = zox_tst_options_find_toggle(world, options);
            zox_tst_check(zox_tst_options_toggle, "options menu has no visible setting toggle");
            if (!zox_tst_options_toggle) {
                return 1;
            }
            const byte found = zox_tst_ui_pointer_position(world, canvas, zox_tst_options_toggle, &zox_tst_options_position);
            zox_tst_check(found, "toggle has no screen position");
            if (!found) {
                return 1;
            }
            zox_tst_options_start_state = zox_getv(zox_tst_options_toggle, ActiveState);
            zox_log("[tst] toggling [%s] at [%ix%i], starts [%i]",
                zox_get(zox_getv(zox_tst_options_toggle, SettingLink), ZoxName)->value,
                zox_tst_options_position.x, zox_tst_options_position.y, zox_tst_options_start_state);
            zox_tst_check(zox_tst_options_setting(world, zox_tst_options_toggle) == zox_tst_options_start_state,
                "toggle shows [%i] but its setting is [%i]",
                zox_tst_options_start_state, zox_tst_options_setting(world, zox_tst_options_toggle));
            break;
        }
        case 3:
            zox_tst_waiting = "hovering the toggle";
            zox_tst_mouse(zox_tst_options_position, 0);
            if (zox_tst_options_frames < zox_tst_options_hold_frames) {
                return 0;
            }
            break;
        case 4:
            zox_tst_waiting = "pressing the toggle";
            zox_tst_mouse(zox_tst_options_position, 1);
            if (zox_tst_options_frames < zox_tst_options_hold_frames) {
                return 0;
            }
            zox_tst_check(zox_getv(toggle, ActiveState) == zox_tst_options_start_state, "pressing the toggle toggled it before release");
            break;
        case 5:
        case 7: {
            zox_tst_waiting = "releasing the toggle";
            zox_tst_mouse(zox_tst_options_position, 0);
            if (zox_tst_options_frames < zox_tst_options_hold_frames) {
                return 0;
            }
            // flipped once after the first click, back after the second
            const byte expected = zox_tst_options_stage == 5 ? !zox_tst_options_start_state : zox_tst_options_start_state;
            const byte state = zox_getv(toggle, ActiveState);
            const byte setting = zox_tst_options_setting(world, toggle);
            zox_tst_check(state == expected, "click [%i]: toggle is [%i], expected [%i]", zox_tst_options_stage == 5 ? 1 : 2, state, expected);
            zox_tst_check(setting == expected, "click [%i]: setting is [%i], expected [%i]", zox_tst_options_stage == 5 ? 1 : 2, setting, expected);
            if (zox_tst_options_stage == 7) {
                zox_tst_mouse_end();
                return 1;
            }
            break;
        }
        case 6:
            zox_tst_waiting = "pressing the toggle again";
            zox_tst_mouse(zox_tst_options_position, 1);
            if (zox_tst_options_frames < zox_tst_options_hold_frames) {
                return 0;
            }
            break;
    }
    zox_tst_options_stage++;
    zox_tst_options_frames = 0;
    return 0;
}

void zox_tst_options_toggle_check(ecs* world) {
    (void) world;
    zox_tst_mouse_end();
}
