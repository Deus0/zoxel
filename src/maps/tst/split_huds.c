// tst: split_screen_huds, --splitscreen in game: each half's huds are scaled to it (player_ui_scale_begin), its
//  minimap, stats, quest tracker and actionbar don't overlap (the stats were over the minimap) or leave its half
//  (the quest tracker ran off its left)
//  - the test level, autostarted, the second player joined at boot
//  - run: bash bsh/tst.sh split_screen_huds
extern char zox_screenshot_request[256];
int zox_tst_shuds_frames = 0;

void zox_tst_split_screen_huds_setup() {
    zox_tst_level_setup();
    is_split_screen = 1;
}

// a hud's bounds on the screen: left, right, bottom, top
//  - a top right aligned one (the quest tracker) reaches left and down from its position, get_element_pointer_bounds
//      has it centred (alignment_to_scaler has no top right)
static inline int4 zox_tst_shuds_rect(ecs* world, const CanvasViewport* viewport, entity e) {
    const int2 size = zox_getv(e, LayoutSize);
    int4 r = get_element_pointer_bounds(world, viewport, e, zox_getv(e, Position2), size);
    if (zox_has(e, MeshAlignment) && zox_getv(e, MeshAlignment) == zox_alignment_top_right) {
        r.x -= size.x / 2;
        r.y -= size.x / 2;
        r.z -= size.y / 2;
        r.w -= size.y / 2;
    }
    return r;
}

// the canvas's huds overlap each other, logs them
static inline int zox_tst_shuds_overlaps(ecs* world, entity canvas, int player) {
    const entity ids[] = { zox_id(Minimap), zox_id(StatBars), zox_id(QuestTracker), zox_id(Actionbar) };
    const char* names[] = { "minimap", "stats", "quest tracker", "actionbar" };
    entity huds[4];
    int overlaps = 0;
    CanvasViewport viewport;
    if (!get_canvas_viewport(world, canvas, &viewport)) {
        zox_log("[tst] player [%i]'s canvas has no viewport", player);
        return 1;
    }
    for (int i = 0; i < 4; i++) {
        const entity e = zox_get_child_by_id(world, canvas, ids[i]);
        huds[i] = zox_valid(e) && zox_has(e, Position2) && zox_has(e, LayoutSize) ? e : 0;
        if (huds[i]) {
            const int4 r = zox_tst_shuds_rect(world, &viewport, e);
            zox_log("[tst] player [%i]'s %s x [%i to %i] y [%i to %i]", player, names[i], r.x, r.y, r.z, r.w);
            // inside its screen (the quest tracker ran off its half's left)
            const int slack = 2;
            if (r.x < viewport.position.x - slack || r.y > viewport.position.x + viewport.size.x + slack ||
                r.z < viewport.position.y - slack || r.w > viewport.position.y + viewport.size.y + slack)
            {
                zox_log("[tst] player [%i]'s %s leaves its screen x [%i to %i]", player, names[i],
                    viewport.position.x, viewport.position.x + viewport.size.x);
                overlaps++;
            }
        } else {
            zox_log("[tst] player [%i] has no %s", player, names[i]);
        }
    }
    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
            if (!huds[i] || !huds[j]) {
                continue;
            }
            const int4 a = zox_tst_shuds_rect(world, &viewport, huds[i]);
            const int4 b = zox_tst_shuds_rect(world, &viewport, huds[j]);
            if (a.x < b.y && b.x < a.y && a.z < b.w && b.z < a.w) {
                zox_log("[tst] player [%i]'s %s overlaps its %s", player, names[i], names[j]);
                overlaps++;
            }
        }
    }
    return overlaps;
}

byte zox_tst_split_screen_huds_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    zox_tst_waiting = "the second player playing";
    if (players_playing < 2 || zox_getv(zox_players[1], PlayerState) != zox_player_state_playing) {
        return 0;
    }
    // the huds spawned, and laid out
    // a screenshot to look at, taken before it ends
    if (++zox_tst_shuds_frames == 100) {
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("split_screen_huds.ppm"));
    }
    if (zox_tst_shuds_frames < 120) {
        return 0;
    }
    zox_tst_check(main_cameras_count == 2, "[%i] main cameras in game, expected 2", main_cameras_count);
    for (int i = 0; i < 2; i++) {
        const entity canvas = zox_get_link(world, zox_players[i], CanvasLink);
        zox_tst_check(zox_valid(canvas), "player [%i] has no canvas", i + 1);
        if (!zox_valid(canvas)) {
            continue;
        }
        zox_tst_check(zox_valid(zox_get_child_by_id(world, canvas, zox_id(Minimap))), "player [%i] has no minimap", i + 1);
        zox_tst_check(zox_valid(zox_get_child_by_id(world, canvas, zox_id(StatBars))), "player [%i] has no stats", i + 1);
        const int overlaps = zox_tst_shuds_overlaps(world, canvas, i + 1);
        zox_tst_check(!overlaps, "player [%i]'s huds overlap or leave its screen [%i] times", i + 1, overlaps);
    }
    return 1;
}

void zox_tst_split_screen_huds_check(ecs* world) {
    (void) world;
}
