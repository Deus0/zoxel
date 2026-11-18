// tst: realm_name_input, the new realm menu's name field fits a long name
//  - main menu > new game: the new realm menu; its name field focused, the longest name typed (input_field_max_length)
//  - the name's text stays inside its field, the field inside the window, and the window on the screen (the field
//      didn't resize)
//  - run: bash bsh/tst.sh realm_name_input
extern char zox_screenshot_request[256];
int zox_tst_rname_stage = 0;
int zox_tst_rname_frames = 0;

// an element's screen bounds: left, right, bottom, top
static inline int4 zox_tst_rname_bounds(ecs* world, const CanvasViewport* viewport, entity e) {
    return get_element_pointer_bounds(world, viewport, e, zox_getv(e, Position2), zox_getv(e, LayoutSize));
}

static inline byte zox_tst_rname_inside(int4 inner, int4 outer, int slack) {
    return inner.x >= outer.x - slack && inner.y <= outer.y + slack && inner.z >= outer.z - slack && inner.w <= outer.w + slack;
}

byte zox_tst_realm_name_input_ready(ecs* world) {
    const entity player = zox_players[0];
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_rname_frames++;
    if (zox_tst_rname_frames < zox_tst_menus_settle_frames) {
        return 0;
    }
    zox_tst_progress(zox_tst_rname_stage * 1000 + zox_tst_rname_frames);
    const entity main_menu = zox_tst_menu(world, canvas, zox_id(MenuMain));
    const entity menu = zox_tst_menu(world, canvas, zox_id(MenuNewRealm));
    if (zox_tst_rname_stage == 0) {
        zox_tst_waiting = "the main menu";
        if (!zox_tst_main_menu(world, player, canvas)) {
            return 0;
        }
        if (!zox_tst_menus_click(world, player, main_menu, &button_event_new_game, "new game")) {
            return 1;
        }
    } else if (zox_tst_rname_stage == 1) {
        zox_tst_waiting = "the new realm menu";
        if (!menu) {
            return 0;
        }
        entity name = 0, seed = 0;
        menu_new_realm_fields(world, menu, &name, &seed);
        zox_tst_check(zox_valid(name), "the new realm menu has no name field");
        if (!zox_valid(name)) {
            return 1;
        }
        input_field_set_focus(world, name);
        char text[input_field_max_length + 1];
        memset(text, 'W', input_field_max_length);
        text[input_field_max_length] = 0;
        input_field_set_value(world, name, "");
        input_field_type(world, text);
    } else if (zox_tst_rname_stage == 2) {
        zox_tst_waiting = "the name typed, laid out";
        if (zox_tst_rname_frames < zox_tst_menus_settle_frames + 30) {
            return 0;
        }
        if (zox_tst_rname_frames == zox_tst_menus_settle_frames + 30) {
            snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("realm_name.ppm"));
            return 0;
        }
        entity name = 0, seed = 0;
        menu_new_realm_fields(world, menu, &name, &seed);
        const entity label = input_field_label(world, name);
        CanvasViewport viewport;
        if (!zox_valid(label) || !get_canvas_viewport(world, canvas, &viewport)) {
            zox_tst_check(0, "no name label or canvas viewport");
            return 1;
        }
        char* value = input_field_value(world, name);
        zox_log("[tst] the name typed [%s] (%i)", value, (int) strlen(value));
        zox_tst_check(strlen(value) == input_field_max_length, "the name is [%i] long, not [%i]", (int) strlen(value), input_field_max_length);
        free(value);
        const int4 l = zox_tst_rname_bounds(world, &viewport, label);
        const int4 f = zox_tst_rname_bounds(world, &viewport, name);
        const int4 w = zox_tst_rname_bounds(world, &viewport, menu);
        const int4 s = { viewport.position.x, viewport.position.x + viewport.size.x, viewport.position.y, viewport.position.y + viewport.size.y };
        zox_log("[tst] name text x [%i to %i], field x [%i to %i], window x [%i to %i], screen x [%i to %i]",
            l.x, l.y, f.x, f.y, w.x, w.y, s.x, s.y);
        zox_tst_check(zox_tst_rname_inside(l, f, 2), "the name's text [%i to %i] leaves its field [%i to %i]", l.x, l.y, f.x, f.y);
        zox_tst_check(zox_tst_rname_inside(f, w, 2), "the name field [%i to %i] leaves its window [%i to %i]", f.x, f.y, w.x, w.y);
        zox_tst_check(zox_tst_rname_inside(w, s, 2), "the window [%i to %i] leaves the screen [%i to %i]", w.x, w.y, s.x, s.y);
        input_field_unfocus(world);
        return 1;
    }
    zox_tst_rname_stage++;
    zox_tst_rname_frames = 0;
    return 0;
}

void zox_tst_realm_name_input_check(ecs* world) {
    (void) world;
}
