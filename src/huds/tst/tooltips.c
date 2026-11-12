// tst: tooltips, sweeps the fake mouse quickly over the main menu's buttons, then rests on one
//  - each round hovers a few buttons (and empty space) for 1 or 2 frames each, like a fast mouse
//  - then rests on a button: the canvas tooltip must show that button's tooltip text, linked to it,
//      visible (not RenderDisabled) with a glyph for each character
//  - then rests on empty space: the tooltip must be cleared
//  - run: bash bsh/tst.sh tooltips
#define zox_tst_tooltips_rounds 40
#define zox_tst_tooltips_sweep 6        // hovers per round, before resting
#define zox_tst_tooltips_rest_frames 12
#define zox_tst_tooltips_max_buttons 8
int zox_tst_tooltips_stage = 0;
int zox_tst_tooltips_frames = 0;
int zox_tst_tooltips_round = 0;
int zox_tst_tooltips_step = 0;          // hover in the round
int zox_tst_tooltips_buttons_count = 0;
entity zox_tst_tooltips_buttons[zox_tst_tooltips_max_buttons];
int2 zox_tst_tooltips_positions[zox_tst_tooltips_max_buttons];
int2 zox_tst_tooltips_empty;
int zox_tst_tooltips_rest = 0;          // button rested on this round
int zox_tst_tooltips_hold = 1;          // frames per hover this round
uint zox_tst_tooltips_seed = 12345;
int zox_tst_tooltips_failed_rounds = 0;
int zox_tst_tooltips_failed_clears = 0;

static inline uint zox_tst_tooltips_random() {
    zox_tst_tooltips_seed = zox_tst_tooltips_seed * 1103515245 + 12345;
    return (zox_tst_tooltips_seed >> 16) & 0x7fff;
}

// adds a main menu button with a tooltip
static inline void zox_tst_tooltips_add_button(
    ecs* world,
    entity canvas,
    entity main_menu,
    void (*on_click)(ecs*, ClickEventData))
{
    const entity button = zox_tst_find_click(world, main_menu, on_click);
    if (!button || !zox_has(button, TooltipText) ||
        zox_tst_tooltips_buttons_count >= zox_tst_tooltips_max_buttons)
    {
        return;
    }
    int2 position;
    if (!zox_tst_ui_pointer_position(world, canvas, button, &position)) {
        return;
    }
    zox_tst_tooltips_buttons[zox_tst_tooltips_buttons_count] = button;
    zox_tst_tooltips_positions[zox_tst_tooltips_buttons_count] = position;
    zox_tst_tooltips_buttons_count++;
}

byte zox_tst_tooltips_ready(ecs* world) {
    const entity player = dbg_player;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_tooltips_frames++;
    const entity main_menu = zox_tst_menu(world, canvas, zox_id(MenuMain));
    if (zox_tst_tooltips_stage == 0) {
        // clicks until the start menu opens the main menu, like a player
        zox_tst_waiting = "a mouse click to open the main menu";
        zox_tst_mouse((int2) { 100, 100 }, (zox_tst_tooltips_frames / 4) % 2);
        if (main_menu) {
            zox_tst_tooltips_stage = 1;
            zox_tst_tooltips_frames = 0;
        }
        return 0;
    }
    if (zox_tst_tooltips_stage == 1) {
        zox_tst_waiting = "the main menu's layout";
        zox_tst_mouse((int2) { 100, 100 }, 0);
        if (!main_menu || zox_tst_tooltips_frames < 10) {
            return 0;
        }
        zox_tst_tooltips_add_button(world, canvas, main_menu, &button_event_continue_game);
        zox_tst_tooltips_add_button(world, canvas, main_menu, &button_event_new_game);
        zox_tst_tooltips_add_button(world, canvas, main_menu, &button_event_menu_options);
        zox_tst_tooltips_add_button(world, canvas, main_menu, &button_event_exit_app);
        zox_tst_check(zox_tst_tooltips_buttons_count >= 2, "main menu has [%i] buttons with tooltips, expected at least 2", zox_tst_tooltips_buttons_count);
        if (zox_tst_tooltips_buttons_count < 2) {
            return 1;
        }
        const entity camera = zox_get_link(world, canvas, CameraLink);
        const int2 canvas_position = zox_getv(camera, ScreenPosition);
        const int2 canvas_size = zox_getv(camera, PixelSize);
        zox_tst_tooltips_empty = (int2) {
            canvas_position.x + canvas_size.x - 4,
            canvas_position.y + canvas_size.y - 4
        };
        zox_log("[tst] sweeping [%i] buttons for [%i] rounds", zox_tst_tooltips_buttons_count, zox_tst_tooltips_rounds);
        zox_tst_tooltips_stage = 2;
        zox_tst_tooltips_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the tooltip sweeps";
    zox_tst_progress(zox_tst_tooltips_round);
    const entity tooltip = zox_tst_menu(world, canvas, zox_id(Tooltip));
    if (!tooltip) {
        zox_tst_check(0, "canvas has no tooltip");
        return 1;
    }
    if (zox_tst_tooltips_stage == 2) {
        // sweeping: a new hover every [hold] frames
        if (zox_tst_tooltips_step == 0 && zox_tst_tooltips_frames == 1) {
            zox_tst_tooltips_hold = 1 + (int) (zox_tst_tooltips_random() % 2);
            zox_tst_tooltips_rest = (int) (zox_tst_tooltips_random() % zox_tst_tooltips_buttons_count);
        }
        if (zox_tst_tooltips_frames < zox_tst_tooltips_hold) {
            return 0;
        }
        zox_tst_tooltips_frames = 0;
        if (zox_tst_tooltips_step < zox_tst_tooltips_sweep) {
            const uint pick = zox_tst_tooltips_random() % (zox_tst_tooltips_buttons_count + 1);
            const int2 position = pick == (uint) zox_tst_tooltips_buttons_count ?
                zox_tst_tooltips_empty : zox_tst_tooltips_positions[pick];
            zox_tst_mouse(position, 0);
            zox_tst_tooltips_step++;
            return 0;
        }
        zox_tst_mouse(zox_tst_tooltips_positions[zox_tst_tooltips_rest], 0);
        zox_tst_tooltips_stage = 3;
        return 0;
    }
    if (zox_tst_tooltips_frames < zox_tst_tooltips_rest_frames) {
        return 0;
    }
    if (zox_tst_tooltips_stage == 3) {
        // resting on a button
        const entity button = zox_tst_tooltips_buttons[zox_tst_tooltips_rest];
        zox_geter(button, TooltipText, expected);
        const TextData* text = zox_get(tooltip, TextData);
        const entity linked = zox_get_link(world, tooltip, TooltipLink);
        const byte has_text = is_zext((TextData*) text, expected->value);
        const byte hidden = zox_getv(tooltip, RenderDisabled);
        const int glyphs = zox_get_children_count_by_id(world, tooltip, zox_id(Glyph));
        const int expected_glyphs = calculate_total_zigels(text->value, text->length);
        if (!has_text || linked != button || hidden || glyphs != expected_glyphs) {
            zox_tst_tooltips_failed_rounds++;
            if (zox_tst_tooltips_failed_rounds <= 5) {
                char* shown = get_zext_text(text);
                zox_loge("[tst] round [%i] hold [%i]: resting on [%s] the tooltip has [%s] linked to [%s] hidden [%i] glyphs [%i/%i], expected [%s]",
                    zox_tst_tooltips_round, zox_tst_tooltips_hold, zox_getn(button),
                    shown ? shown : "", zox_valid(linked) ? zox_getn(linked) : "nothing",
                    hidden, glyphs, expected_glyphs, expected->value);
                if (shown) {
                    free(shown);
                }
            }
        }
        zox_tst_mouse(zox_tst_tooltips_empty, 0);
        zox_tst_tooltips_stage = 4;
        zox_tst_tooltips_frames = 0;
        return 0;
    }
    // resting on empty space
    zox_geter(tooltip, TextData, text);
    if (text->length || zox_is_linked_any(world, tooltip, TooltipLink)) {
        zox_tst_tooltips_failed_clears++;
        if (zox_tst_tooltips_failed_clears <= 5) {
            zox_loge("[tst] round [%i]: off the buttons, the tooltip still has [%i] characters", zox_tst_tooltips_round, text->length);
        }
    }
    zox_tst_tooltips_round++;
    zox_tst_tooltips_step = 0;
    zox_tst_tooltips_stage = 2;
    zox_tst_tooltips_frames = 0;
    if (zox_tst_tooltips_round < zox_tst_tooltips_rounds) {
        return 0;
    }
    zox_tst_check(zox_tst_tooltips_failed_rounds == 0, "tooltip was wrong after [%i] of [%i] sweeps", zox_tst_tooltips_failed_rounds, zox_tst_tooltips_rounds);
    zox_tst_check(zox_tst_tooltips_failed_clears == 0, "tooltip wasn't cleared after [%i] of [%i] sweeps", zox_tst_tooltips_failed_clears, zox_tst_tooltips_rounds);
    zox_tst_mouse_end();
    return 1;
}

void zox_tst_tooltips_check(ecs* world) {
    (void) world;
    zox_tst_mouse_end();
}
