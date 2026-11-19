// the game log: lines at the bottom left of a player's screen (over the actionbar), for anything that tells a player
//  something (a quest done, its reward)
//  - game_log(world, player, text): the newest at the bottom, the ones before pushed up a line
//  - each fades out after game_log_seconds; past game_log_max the oldest fades early
//  - later: a log window in the game menu with the old ones (they'd be kept on the player)
const double game_log_seconds = 6;
const float game_log_fade_time = 1;
#define game_log_max 6
#define game_log_bottom 46      // its lowest line's bottom, over the actionbar (scaled by ui_scale)

// its alpha and its children's (its text's glyphs) to nothing, the last tween deletes it
static void game_log_fade(ecs* world, entity e, byte deletes) {
    if (zox_has(e, Alpha)) {
        const entity tween = zox_lerp_float_ins(e, Alpha, zox_getv(e, Alpha), 0, game_log_fade_time);
        if (deletes) {
            zox_add(tween, AnimationDeletesTarget);
        }
    }
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            game_log_fade(world, it.entities[i], 0);
        }
    }
}

void game_log_end(ecs* world, entity e) {
    if (zox_valid(e) && !zox_has(e, UIClosing)) {
        zox_add(e, UIClosing);
        game_log_fade(world, e, 1);
    }
}

entity game_log(ecs* world, entity player, const char* text) {
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(canvas) || !text || !text[0]) {
        return 0;
    }
    const byte font_size = 5 * ui_scale;
    const byte2 padding = byte2_single(2 * ui_scale);
    const int2 size = { (int) strlen(text) * font_size + padding.x * 2, font_size + padding.y * 2 };
    const int margin = 8 * ui_scale;
    const int row = size.y + 2 * ui_scale;
    // over the actionbar (bottom middle, as wide as a small screen; actions comes after huds)
    const int bottom = game_log_bottom * ui_scale;
    // the ones there up a line, the oldest past the most fading early
    entity lines[64];
    int count = 0;
    iter it = zox_children(world, canvas);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count && count < 64; i++) {
            const entity e = it.entities[i];
            if (zox_has(e, GameLogLine) && !zox_has(e, UIClosing)) {
                lines[count++] = e;
            }
        }
    }
    for (int i = 0; i < count; i++) {
        const entity e = lines[i];
        int2 position = zox_getv(e, LayoutPosition);
        position.y += row;
        zox_setv(e, LayoutPosition, position);
        zox_add(e, LayoutPositionDirty);
        if (position.y > bottom + size.y / 2 + row * (game_log_max - 1)) {
            game_log_end(world, e);
        }
    }
    // its left edge at the margin (an element's position is its middle)
    const int2 position = { margin + size.x / 2, bottom + size.y / 2 };
    const entity e = spawn_uic(world, prefab_element_frame, canvas, float2_zero, position, size, size,
        header_fill, header_outline);
    zox_setv(e, Layer, get_game_overlay_layer());
    zox_set_unique_name(e, "game_log_line");
    zox_add(e, GameLogLine);
    const entity label = spawn_text(world, prefab_text, e, int2_zero, float2_half, font_size, zox_alignment_centre,
        byte2_zero, text, header_font_fill, header_font_outline);
    zox_setv(label, Layer, get_game_overlay_layer() + 1);
    delay_event(world, &game_log_end, e, game_log_seconds);
    return e;
}
