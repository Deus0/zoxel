// tst: tooltip_flick, flicking the mouse between two icons, the tooltip ends up showing the last one's text, its
//  background shown and sized to it (in game its background didn't set, flicking between them)
//  - in game, the game menus open (the taskbar up, the mouse free): the fake mouse moves between two of its icons (different
//      tooltips) every frame,
//      then rests on the second
//  - checks: the tooltip's text is the second's, it's shown (RenderDisabled 0), its size fits its text
//  - run: bash bsh/tst.sh tooltip_flick
int zox_tst_tf_stage = 0;
int zox_tst_tf_frames = 0;
entity zox_tst_tf_icons[2];
int2 zox_tst_tf_at[2];
char zox_tst_tf_text[128];
byte zox_tst_tf_shown = 0, zox_tst_tf_sized = 0;
int2 zox_tst_tf_size, zox_tst_tf_expected;

// up to count elements with a tooltip text under e
static inline int zox_tst_tf_find_all(ecs* world, entity e, entity* found, int count, int had) {
    if (had < count && zox_has(e, TooltipText) && zox_has(e, TooltipEvent) && zox_getv(e, TooltipText)[0]) {
        found[had++] = e;
    }
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            had = zox_tst_tf_find_all(world, it.entities[i], found, count, had);
        }
    }
    return had;
}

byte zox_tst_tooltip_flick_ready(ecs* world) {
    // spawned and settled before it starts (in the menus, the settle check can fail again)
    if (!zox_tst_tf_stage && !zox_tst_tf_frames && !zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity canvas = zox_get_link(world, dbg_player, CanvasLink);
    zox_tst_tf_frames++;
    zox_tst_progress(zox_tst_tf_stage);
    static byte taskbar = 0;
    if (zox_tst_tf_stage == 0) {
        zox_tst_waiting = "two taskbar icons";
        // the game menus: the taskbar up, the mouse free
        if (!taskbar) {
            toggle_game_menus(world, dbg_player);
            taskbar = 1;
            return 0;
        }
        // a press on empty ground wakes the mouse (devices start disabled, until real input picks the device mode)
        if (zox_tst_tf_frames < 30) {
            zox_tst_mouse((int2) { 40, 40 }, zox_tst_tf_frames > 10 && zox_tst_tf_frames < 14);
            return 0;
        }
        // two on screen (the taskbar is wider than a small window), their tooltips differing
        entity all[16];
        const int count = zox_tst_tf_find_all(world, canvas, all, 16, 0);
        int found = 0;
        for (int k = 0; k < count && found < 2; k++) {
            int2 at;
            if (!zox_tst_ui_pointer_position(world, canvas, all[k], &at) || at.x < 8 || at.y < 8) {
                continue;
            }
            if (found && !strcmp(zox_getv(all[k], TooltipText), zox_getv(zox_tst_tf_icons[0], TooltipText))) {
                continue;
            }
            zox_tst_tf_icons[found] = all[k];
            zox_tst_tf_at[found++] = at;
        }
        if (found < 2) {
            return 0;
        }
        snprintf(zox_tst_tf_text, sizeof(zox_tst_tf_text), "%.127s", zox_getv(zox_tst_tf_icons[1], TooltipText));
        zox_tst_tf_stage = 1;
        zox_tst_tf_frames = 0;
        return 0;
    }
    if (zox_tst_tf_stage == 1) {
        // a frame on each, back and forth, then rests on the second
        zox_tst_waiting = "flicking between them";
        const int which = zox_tst_tf_frames < 24 ? zox_tst_tf_frames % 2 : 1;
        zox_tst_ui_pointer_position(world, canvas, zox_tst_tf_icons[which], &zox_tst_tf_at[which]);
        zox_tst_mouse(zox_tst_tf_at[which], 0);
        if (zox_tst_tf_frames < 40) {
            return 0;
        }
        const entity tooltip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));
        char* text = get_zext_text(zox_get(tooltip, TextData));
        const byte same = text && !strcmp(text, zox_tst_tf_text);
        zox_log("[tst] the pointer targets [%s], the second icon is [%lu], the mouse lock [%i]", zox_getn(zox_tst_mouse_target(world, dbg_player)), zox_tst_tf_icons[1], zox_tst_mouse_lock(world, dbg_player));
        if (!same) {
            zox_log("[tst] the tooltip says [%s], not [%s]", text ? text : "", zox_tst_tf_text);
        }
        free(text);
        zox_tst_tf_shown = same && !zox_getv(tooltip, RenderDisabled);
        const TextData* data = zox_get(tooltip, TextData);
        zox_tst_tf_size = zox_getv(tooltip, LayoutSize);
        zox_tst_tf_expected = calculate_zext_size(data->value, data->length, zox_getv(tooltip, TextFontSize),
            zox_getv(tooltip, TextPadding), default_line_padding);
        zox_tst_tf_sized = int2_equals(zox_tst_tf_size, zox_tst_tf_expected);
        zox_tst_mouse_end();
        toggle_game_menus(world, dbg_player);
        return 1;
    }
    return 0;
}

void zox_tst_tooltip_flick_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_tf_shown, "the tooltip isn't showing the last icon's text [%s]", zox_tst_tf_text);
    zox_tst_check(zox_tst_tf_sized, "the tooltip is [%ix%i], its text [%ix%i]", zox_tst_tf_size.x, zox_tst_tf_size.y,
        zox_tst_tf_expected.x, zox_tst_tf_expected.y);
}
