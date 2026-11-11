// tst: ui_layout, menus fit on the canvas at every ui scale
//  - ui_scale follows the screen's area, so a narrow screen (a phone in portrait) gets a big
//      scale for its width, scales 1 to 3 on the test's small canvas cover that
//  - per scale: opens options, every shown element is inside the canvas, then back to the
//      main menu (respawned at that scale), the same for it
//  - run: bash bsh/tst.sh ui_layout
#define zox_tst_layout_scales 3
int zox_tst_layout_stage = 0;
int zox_tst_layout_frames = 0;
int zox_tst_layout_scale = 1;
float zox_tst_layout_old_scale = 1;

// counts shown elements under e that leave the canvas, logs the first
int zox_tst_layout_outside(ecs* world, const CanvasViewport* viewport, entity e, const char* menu) {
    int outside = 0;
    if (zox_has(e, Element) &&
        zox_has(e, Position2) &&
        zox_has(e, LayoutSize) &&
        zox_has(e, RenderDisabled) &&
        !zox_getv(e, RenderDisabled))
    {
        const int4 b = get_element_pointer_bounds(world, viewport, e, zox_getv(e, Position2), zox_getv(e, LayoutSize));
        const int slack = 1;
        if (b.x < viewport->position.x - slack ||
            b.y > viewport->position.x + viewport->size.x + slack ||
            b.z < viewport->position.y - slack ||
            b.w > viewport->position.y + viewport->size.y + slack)
        {
            outside++;
            if (outside == 1) {
                zox_log("[tst] scale [%i] %s: [%s] at x [%i-%i] y [%i-%i] leaves the canvas [%ix%i]",
                    zox_tst_layout_scale, menu, zox_getn(e), b.x, b.y, b.z, b.w, viewport->size.x, viewport->size.y);
            }
        }
    }
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const int child_outside = zox_tst_layout_outside(world, viewport, it2.entities[j], menu);
            if (child_outside && !outside) {
                outside = child_outside;
            } else {
                outside += child_outside;
            }
        }
    }
    return outside;
}

void zox_tst_layout_check_menu(ecs* world, entity canvas, entity menu, const char* name) {
    CanvasViewport viewport;
    if (!get_canvas_viewport(world, canvas, &viewport)) {
        zox_tst_check(0, "canvas has no viewport");
        return;
    }
    const int outside = zox_tst_layout_outside(world, &viewport, menu, name);
    zox_tst_check(outside == 0, "scale [%i] %s: [%i] elements leave the canvas", zox_tst_layout_scale, name, outside);
}

byte zox_tst_ui_layout_ready(ecs* world) {
    const entity player = dbg_player;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_layout_frames++;
    if (zox_tst_layout_frames < 10) {
        return 0;
    }
    const entity main_menu = zox_tst_menu(world, canvas, zox_id(MenuMain));
    const entity options = zox_tst_menu(world, canvas, zox_id(MenuOptions));
    switch (zox_tst_layout_stage) {
        case 0:
            zox_tst_waiting = "the main menu";
            if (!zox_tst_main_menu(world, player, canvas)) {
                zox_tst_layout_frames = 0;
                return 0;
            }
            zox_tst_layout_old_scale = ui_scale;
            break;
        case 1:
            // opens options at this scale
            zox_tst_waiting = "the main menu, to open options";
            if (!main_menu) {
                return 0;
            }
            ui_scale = zox_tst_layout_scale;
            if (!zox_tst_menus_click(world, player, main_menu, &button_event_menu_options, "options")) {
                return 1;
            }
            break;
        case 2:
            zox_tst_waiting = "the options menu's layout";
            if (!options) {
                return 0;
            }
            zox_tst_layout_check_menu(world, canvas, options, "options");
            if (!zox_tst_menus_click(world, player, options, &button_event_menu_main, "options back")) {
                return 1;
            }
            break;
        default:
            zox_tst_waiting = "the main menu's layout";
            if (!main_menu || options) {
                return 0;
            }
            zox_tst_layout_check_menu(world, canvas, main_menu, "main menu");
            if (zox_tst_layout_scale >= zox_tst_layout_scales) {
                ui_scale = zox_tst_layout_old_scale;
                return 1;
            }
            zox_tst_layout_scale++;
            zox_tst_layout_stage = 1;
            zox_tst_layout_frames = 0;
            return 0;
    }
    zox_tst_layout_stage++;
    zox_tst_layout_frames = 0;
    return 0;
}

void zox_tst_ui_layout_check(ecs* world) {
    (void) world;
    ui_scale = zox_tst_layout_old_scale;
}
