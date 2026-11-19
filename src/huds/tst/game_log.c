extern char zox_screenshot_request[256];
// tst: game_log, lines logged to a player (game_log) stack at the bottom left of their screen and fade out
//  - eight lines a frame apart: the newest at the bottom, each above the one after it, their left edges together
//  - at most game_log_max stay, the oldest fading early
//  - after game_log_seconds and its fade they're all gone; a screenshot of them up, tst/game_log.ppm
//  - run: bash bsh/tst.sh game_log
int zox_tst_glog_stage = 0;
int zox_tst_glog_frames = 0;
double zox_tst_glog_time = 0;
int zox_tst_glog_open = -1, zox_tst_glog_left_off = -1, zox_tst_glog_order_bad = -1, zox_tst_glog_bottom = 0, zox_tst_glog_after = -1;

// the canvas's log lines not fading, by height
static inline int zox_tst_glog_lines(ecs* world, entity canvas, entity* lines, int capacity) {
    int count = 0;
    iter it = zox_children(world, canvas);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count && count < capacity; i++) {
            if (zox_has(it.entities[i], GameLogLine) && !zox_has(it.entities[i], UIClosing)) {
                lines[count++] = it.entities[i];
            }
        }
    }
    return count;
}

byte zox_tst_game_log_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity player = zox_players[0];
    const entity canvas = zox_get_link(world, player, CanvasLink);
    zox_tst_glog_frames++;
    zox_tst_progress(zox_tst_glog_stage * 1000 + zox_tst_glog_frames);
    if (zox_tst_glog_stage == 0) {
        zox_tst_waiting = "logging eight lines";
        char text[32];
        snprintf(text, sizeof(text), "line %i", zox_tst_glog_frames);
        game_log(world, player, text);
        if (zox_tst_glog_frames < 8) {
            return 0;
        }
        zox_tst_glog_stage = 1;
        zox_tst_glog_frames = 0;
        zox_tst_glog_time = zox_current_time;
        return 0;
    }
    if (zox_tst_glog_stage == 1) {
        zox_tst_waiting = "the lines laid out";
        if (zox_tst_glog_frames < 10) {
            return 0;
        }
        entity lines[64];
        const int count = zox_tst_glog_lines(world, canvas, lines, 64);
        zox_tst_glog_open = count;
        zox_tst_glog_left_off = 0;
        zox_tst_glog_order_bad = 0;
        for (int i = 0; i < count; i++) {
            const int2 p = zox_getv(lines[i], LayoutPosition), s = zox_getv(lines[i], LayoutSize);
            zox_tst_glog_left_off = int_max(zox_tst_glog_left_off, abs((p.x - s.x / 2) - (zox_getv(lines[0], LayoutPosition).x -
                zox_getv(lines[0], LayoutSize).x / 2)));
            char* text = get_zext_text(zox_get(zox_get_child_by_id(world, lines[i], zox_id(Text)), TextData));
            const int n = text ? atoi(text + 5) : 0;
            // the line logged later is lower
            for (int j = 0; j < count; j++) {
                char* other = get_zext_text(zox_get(zox_get_child_by_id(world, lines[j], zox_id(Text)), TextData));
                const int m = other ? atoi(other + 5) : 0;
                if (m > n && zox_getv(lines[j], LayoutPosition).y >= p.y) {
                    zox_tst_glog_order_bad++;
                }
                if (other) {
                    free(other);
                }
            }
            if (n == 8) {
                zox_tst_glog_bottom = p.y - s.y / 2;
            }
            if (text) {
                free(text);
            }
        }
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("game_log.ppm"));
        zox_tst_glog_stage = 2;
        return 0;
    }
    zox_tst_waiting = "the lines faded out";
    if (zox_current_time - zox_tst_glog_time < game_log_seconds + game_log_fade_time + 0.5) {
        return 0;
    }
    int after = 0;
    iter it = zox_children(world, canvas);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            after += zox_has(it.entities[i], GameLogLine);
        }
    }
    zox_tst_glog_after = after;
    return 1;
}

void zox_tst_game_log_check(ecs* world) {
    (void) world;
    zox_log("[tst] game_log: [%i] lines up, left edges [%i] apart, [%i] out of order, the newest's bottom [%i], [%i] left after",
        zox_tst_glog_open, zox_tst_glog_left_off, zox_tst_glog_order_bad, zox_tst_glog_bottom, zox_tst_glog_after);
    zox_tst_check(zox_tst_glog_open > 0 && zox_tst_glog_open <= game_log_max, "[%i] lines up, not 1 to %i", zox_tst_glog_open, game_log_max);
    zox_tst_check(zox_tst_glog_left_off >= 0 && zox_tst_glog_left_off <= 1, "the lines' left edges are [%i] apart", zox_tst_glog_left_off);
    zox_tst_check(!zox_tst_glog_order_bad, "[%i] lines above a newer one, the newest isn't at the bottom", zox_tst_glog_order_bad);
    zox_tst_check(zox_tst_glog_bottom > 0 && zox_tst_glog_bottom < 80 * ui_scale, "the newest line's bottom is [%i] up, not near the bottom",
        zox_tst_glog_bottom);
    zox_tst_check(zox_tst_glog_after == 0, "[%i] lines left after they faded", zox_tst_glog_after);
}

void zox_tsts_game_log(ecs* world) {
    zox_tst_add("game_log", zox_tst_game_log_ready, zox_tst_game_log_check, 90);
}
