// tst: options_disabled, a greyed (unsupported) setting in the options menu can't be changed (huds/ins/settings.c,
//  ui_disabled)
//  - marks HDR and Tonemap unsupported before opening options (the backend's own support doesn't matter)
//  - HDR's toggle: the fake mouse presses and releases it, twice; checks it's greyed (ClickDisabled, a tooltip), its
//    toggle and setting stay off, hdr output isn't asked for
//  - Tonemap's slider: the fake mouse drags its handle right, then a gamepad's step (navigation_slider_step); checks
//    it's greyed, its handle doesn't move, its setting stays
//  - run: bash bsh/tst.sh options_disabled
int zox_tst_odisabled_stage = 0;
int zox_tst_odisabled_frames = 0;
entity zox_tst_odisabled_toggle = 0;
int2 zox_tst_odisabled_position;
entity zox_tst_odisabled_slider = 0;
entity zox_tst_odisabled_handle = 0;
byte zox_tst_odisabled_tonemap = 0;     // its setting before
int2 zox_tst_odisabled_handle_at;       // its handle's layout position before

// the element under e linked to setting (a toggle or slider, scrolled out of view maybe)
static entity zox_tst_odisabled_find(ecs* world, entity e, entity setting) {
    if (zox_has(e, SettingLink) && zox_getv(e, SettingLink) == setting) {
        return e;
    }
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity found = zox_tst_odisabled_find(world, it2.entities[j], setting);
            if (found) {
                zox_children_end(it2);
                return found;
            }
        }
    }
    return 0;
}

byte zox_tst_options_disabled_ready(ecs* world) {
    const entity player = dbg_player;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_odisabled_frames++;
    const entity hdr = zox_get_child_by_id(world, main_app, zox_id(SettingHdr));
    const entity tonemap = zox_get_child_by_id(world, main_app, zox_id(SettingTonemap));
    const entity main_menu = zox_tst_menu(world, canvas, zox_id(MenuMain));
    const entity options = zox_tst_menu(world, canvas, zox_id(MenuOptions));
    const entity toggle = zox_tst_odisabled_toggle;
    const int hold = zox_tst_options_hold_frames;
    switch (zox_tst_odisabled_stage) {
        case 0:
            zox_tst_waiting = "a mouse click to open the main menu";
            zox_tst_mouse((int2) { 100, 100 }, (zox_tst_odisabled_frames / hold) % 2);
            if (!main_menu) {
                return 0;
            }
            break;
        case 1:
            zox_tst_waiting = "the main menu to settle";
            zox_tst_mouse((int2) { 100, 100 }, 0);
            if (zox_tst_odisabled_frames < hold) {
                return 0;
            }
            zox_tst_check(zox_valid(hdr) && zox_valid(tonemap), "no HDR or Tonemap setting");
            if (!zox_valid(hdr) || !zox_valid(tonemap)) {
                return 1;
            }
            zox_add(hdr, SettingUnsupported);
            zox_add(tonemap, SettingUnsupported);
            zox_tst_odisabled_tonemap = zox_getv(tonemap, SettingByte);
            if (!zox_tst_menus_click(world, player, main_menu, &button_event_menu_options, "options")) {
                return 1;
            }
            break;
        case 2: {
            zox_tst_waiting = "the options menu's layout";
            if (!options || zox_tst_odisabled_frames < 10) {
                return 0;
            }
            zox_tst_odisabled_toggle = zox_tst_odisabled_find(world, options, hdr);
            zox_tst_check(zox_tst_odisabled_toggle, "the options menu shows no HDR toggle");
            if (!zox_tst_odisabled_toggle) {
                return 1;
            }
            scroll_list_to_child(world, zox_tst_odisabled_toggle);     // the list's further down: scrolled to it
            break;
        }
        case 3: {
            zox_tst_waiting = "the HDR toggle scrolled into view";
            const entity t = zox_tst_odisabled_toggle;
            if (zox_tst_odisabled_frames < hold || !zox_has(t, RenderDisabled) || zox_getv(t, RenderDisabled)) {
                return 0;
            }
            zox_tst_check(zox_has(t, ClickDisabled) && zox_getv(t, ClickDisabled) && zox_has(t, TooltipText),
                "the unsupported HDR toggle isn't greyed (ClickDisabled, a tooltip)");
            zox_tst_check(!zox_getv(t, ActiveState), "the unsupported HDR toggle shows on");
            const byte found = zox_tst_ui_pointer_position(world, canvas, t, &zox_tst_odisabled_position);
            zox_tst_check(found, "the HDR toggle has no screen position");
            if (!found) {
                return 1;
            }
            break;
        }
        case 4:
        case 6:
            zox_tst_waiting = "pressing the greyed toggle";
            zox_tst_mouse(zox_tst_odisabled_position, 1);
            if (zox_tst_odisabled_frames < hold) {
                return 0;
            }
            break;
        case 5:
        case 7:
            zox_tst_waiting = "releasing the greyed toggle";
            zox_tst_mouse(zox_tst_odisabled_position, 0);
            if (zox_tst_odisabled_frames < hold) {
                return 0;
            }
            zox_tst_check(!zox_getv(toggle, ActiveState), "click [%i]: the greyed HDR toggle turned on", (zox_tst_odisabled_stage - 3) / 2);
            zox_tst_check(!zox_getv(hdr, SettingByte), "click [%i]: the greyed HDR setting turned on", (zox_tst_odisabled_stage - 3) / 2);
            zox_tst_check(zox_hdr_output_asked == zox_hdr_output_off, "click [%i]: hdr output was asked for",
                (zox_tst_odisabled_stage - 3) / 2);
            break;
        case 8: {
            // Tonemap's slider, its handle (the child that slides)
            zox_tst_waiting = "the Tonemap slider";
            zox_tst_odisabled_slider = zox_tst_odisabled_find(world, options, tonemap);
            zox_tst_check(zox_tst_odisabled_slider, "the options menu shows no Tonemap slider");
            if (!zox_tst_odisabled_slider) {
                return 1;
            }
            iter it2 = zox_children(world, zox_tst_odisabled_slider);
            while (zox_children_next(it2)) {
                for (int j = 0; j < it2.count; j++) {
                    zox_tst_odisabled_handle = zox_has(it2.entities[j], SlideBounds) ? it2.entities[j] : zox_tst_odisabled_handle;
                }
            }
            const entity handle = zox_tst_odisabled_handle;
            zox_tst_check(handle, "the Tonemap slider has no handle");
            if (!handle) {
                return 1;
            }
            scroll_list_to_child(world, zox_tst_odisabled_slider);
            break;
        }
        case 9: {
            zox_tst_waiting = "the Tonemap slider in view";
            const entity handle = zox_tst_odisabled_handle;
            if (zox_tst_odisabled_frames < hold || !zox_has(handle, RenderDisabled) || zox_getv(handle, RenderDisabled)) {
                return 0;
            }
            zox_tst_check(ui_disabled(world, zox_tst_odisabled_slider) && ui_disabled(world, handle) &&
                zox_has(zox_tst_odisabled_slider, TooltipText), "the unsupported Tonemap slider isn't greyed");
            zox_tst_odisabled_handle_at = zox_getv(handle, LayoutPosition);
            if (!zox_tst_ui_pointer_position(world, canvas, handle, &zox_tst_odisabled_position)) {
                zox_tst_check(0, "the Tonemap handle has no screen position");
                return 1;
            }
            break;
        }
        case 10:
            zox_tst_waiting = "pressing the greyed handle";
            zox_tst_mouse(zox_tst_odisabled_position, 1);
            if (zox_tst_odisabled_frames < hold) {
                return 0;
            }
            break;
        case 11:
            zox_tst_waiting = "dragging the greyed handle right";
            zox_tst_mouse((int2) { zox_tst_odisabled_position.x + 4 * zox_tst_odisabled_frames, zox_tst_odisabled_position.y }, 1);
            if (zox_tst_odisabled_frames < 20) {
                return 0;
            }
            break;
        case 12: {
            zox_tst_waiting = "releasing the greyed handle";
            zox_tst_mouse((int2) { zox_tst_odisabled_position.x + 80, zox_tst_odisabled_position.y }, 0);
            if (zox_tst_odisabled_frames < hold) {
                return 0;
            }
            const int2 at = zox_getv(zox_tst_odisabled_handle, LayoutPosition);
            zox_tst_check(int2_equals(at, zox_tst_odisabled_handle_at), "dragged, the greyed handle moved [%i] to [%i]",
                zox_tst_odisabled_handle_at.x, at.x);
            zox_tst_check(zox_getv(tonemap, SettingByte) == zox_tst_odisabled_tonemap, "dragged, Tonemap changed [%i] to [%i]",
                zox_tst_odisabled_tonemap, zox_getv(tonemap, SettingByte));
            // a gamepad's step, as the navigator steps a selected slider
            navigation_slider_step(world, player, zox_tst_odisabled_slider, 1);
            const int2 stepped = zox_getv(zox_tst_odisabled_handle, LayoutPosition);
            zox_tst_check(int2_equals(stepped, zox_tst_odisabled_handle_at), "a gamepad step moved the greyed handle");
            zox_tst_check(zox_getv(tonemap, SettingByte) == zox_tst_odisabled_tonemap, "a gamepad step changed Tonemap");
            zox_tst_mouse_end();
            return 1;
        }
    }
    zox_tst_odisabled_stage++;
    zox_tst_odisabled_frames = 0;
    return 0;
}

void zox_tst_options_disabled_check(ecs* world) {
    (void) world;
    zox_tst_mouse_end();
}
