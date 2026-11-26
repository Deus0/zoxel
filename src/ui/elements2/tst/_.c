// tst: input_field, an input field takes typing while focused (fun/input.c)
//  - on the main menu: an input field on the canvas, "Realm" in it, clicked (focused): it shows "Realm_" and turns
//    the app's text input on
//  - typed text (zox_text_typed, as SDL's text events give it) goes in through input_field_system, then typing,
//    a backspace, more typing: its value "Realm Zox"
//  - unfocused: no '_', text input off, and typing changes nothing
//  - run: bash bsh/tst.sh input_field
entity zox_tst_input = 0;
int zox_tst_input_stage = 0;
int zox_tst_input_frames = 0;
char zox_tst_input_focused[64];     // its label while focused, before typing
char zox_tst_input_typed[64];       // its value after typing through the system
char zox_tst_input_value[64];       // its value after typing and a backspace
char zox_tst_input_after[64];       // its label after unfocusing and typing
byte zox_tst_input_wanted_on = 0;
byte zox_tst_input_wanted_off = 1;

static inline void zox_tst_input_label(ecs* world, char* out, int size) {
    const entity label = input_field_label(world, zox_tst_input);
    char* text = zox_valid(label) ? get_zext_text(zox_get(label, TextData)) : NULL;
    snprintf(out, size, "%s", text ? text : "-");
    free(text);
}

byte zox_tst_input_field_ready(ecs* world) {
    zox_tst_input_frames++;
    zox_tst_progress(zox_tst_input_stage);
    if (zox_tst_input_stage == 0) {
        zox_tst_waiting = "a player with a canvas";
        const entity canvas = zox_valid(dbg_player) ? zox_get_link(world, dbg_player, CanvasLink) : 0;
        if (!zox_valid(canvas) || zox_tst_input_frames < 10) {
            return 0;
        }
        zox_tst_input = spawn_button(world, prefab_button, canvas, "Realm", int2_zero, int2_zero, float2_half,
            zox_alignment_centre, 24, (byte2) { 6, 4 }, button_fill, button_outline, button_font_fill, button_font_outline).x;
        zox_add(zox_tst_input, TextInput);
        zox_setv(zox_tst_input, ClickEvent, &on_click_input_field);
        zox_tst_input_stage = 1;
        zox_tst_input_frames = 0;
        return 0;
    }
    if (zox_tst_input_stage == 1) {
        if (zox_tst_input_frames < 5) {
            return 0;
        }
        // clicked, as a click event gives it
        on_click_input_field(world, (ClickEventData) { .clicker = dbg_player, .clicked = zox_tst_input });
        zox_tst_input_stage = 2;
        zox_tst_input_frames = 0;
        return 0;
    }
    if (zox_tst_input_stage == 2) {
        if (zox_tst_input_frames < 3) {
            return 0;
        }
        zox_tst_input_label(world, zox_tst_input_focused, sizeof(zox_tst_input_focused));
        zox_tst_input_wanted_on = zox_text_input_wanted;
        // typed, as SDL's text events add it, taken by input_field_system
        add_text_typed(" Zo");
        zox_tst_input_stage = 3;
        zox_tst_input_frames = 0;
        return 0;
    }
    if (zox_tst_input_stage == 3) {
        zox_tst_waiting = "the field to take the typed text";
        char* value = input_field_value(world, zox_tst_input);
        const byte took = !strcmp(value, "Realm Zo");
        free(value);
        if (!took && zox_tst_input_frames < 30) {
            return 0;
        }
        char* typed = input_field_value(world, zox_tst_input);
        snprintf(zox_tst_input_typed, sizeof(zox_tst_input_typed), "%s", typed);
        free(typed);
        input_field_type(world, "q");
        input_field_backspace(world);
        input_field_type(world, "x");
        char* final = input_field_value(world, zox_tst_input);
        snprintf(zox_tst_input_value, sizeof(zox_tst_input_value), "%s", final);
        free(final);
        input_field_unfocus(world);
        zox_tst_input_wanted_off = zox_text_input_wanted;
        input_field_type(world, "nope");
        zox_tst_input_stage = 4;
        zox_tst_input_frames = 0;
        return 0;
    }
    if (zox_tst_input_frames < 3) {
        return 0;
    }
    zox_tst_input_label(world, zox_tst_input_after, sizeof(zox_tst_input_after));
    return 1;
}

void zox_tst_input_field_check(ecs* world) {
    (void) world;
    zox_log("[tst] input field: focused [%s] typed [%s] value [%s] after [%s]", zox_tst_input_focused, zox_tst_input_typed,
        zox_tst_input_value, zox_tst_input_after);
    zox_tst_check(!strcmp(zox_tst_input_focused, "Realm_"), "focused it shows [%s], not [Realm_]", zox_tst_input_focused);
    zox_tst_check(zox_tst_input_wanted_on, "focused, the app's text input wasn't wanted");
    zox_tst_check(!strcmp(zox_tst_input_typed, "Realm Zo"), "typed [ Zo] through the system, its value [%s]", zox_tst_input_typed);
    zox_tst_check(!strcmp(zox_tst_input_value, "Realm Zox"), "after q, backspace, x its value is [%s], not [Realm Zox]",
        zox_tst_input_value);
    zox_tst_check(!zox_tst_input_wanted_off, "unfocused, the app's text input was still wanted");
    zox_tst_check(!strcmp(zox_tst_input_after, "Realm Zox"), "unfocused and typed, it shows [%s], not [Realm Zox]",
        zox_tst_input_after);
}

static entity zox_tst_ui_color_button(ecs* world, entity parent, int2 position, float2 anchor,
    color fill, void (*clicked)(ecs*, ClickEventData))
{
    const int2 size = { 48, 32 };
    const entity button = spawn_uic(world, prefab_button, parent, anchor, position, size, size, fill, fill);
    zox_setv(button, ElementFillColor, fill);
    zox_setv(button, ElementOutlineColor, fill);
    zox_setv(button, SelectedFillColor, fill);
    zox_setv(button, SelectedOutlineColor, fill);
    zox_setv(button, ClickEvent, clicked);
    return button;
}

#include "focus_lifecycle.c"
#include "input_boundaries.c"
#include "slider_drag.c"
#include "scroll_mask.c"
#include "fold_arrow.c"
#ifdef zox_sdl3
    #include "sdl_text.c"
#endif

void zox_tsts_elements2() {
    zox_tst_add("slider_drag", zox_tst_slider_drag_ready, zox_tst_ui_fixture_check, 30);
    zox_tst_set_setup("slider_drag", NULL, 1);
    zox_tst_add("scroll_mask", zox_tst_scroll_mask_ready, zox_tst_ui_fixture_check, 20);
    zox_tst_set_setup("scroll_mask", NULL, 1);
    zox_tst_add("fold_arrow", zox_tst_fold_arrow_ready, zox_tst_ui_fixture_check, 20);
    zox_tst_set_setup("fold_arrow", NULL, 1);
    zox_tst_add("input_field_boundaries", zox_tst_input_boundaries_ready, zox_tst_ui_fixture_check, 30);
    zox_tst_set_setup("input_field_boundaries", NULL, 1);
    zox_tst_add("input_focus_lifecycle", zox_tst_input_focus_ready, zox_tst_ui_fixture_check, 30);
    zox_tst_set_setup("input_focus_lifecycle", NULL, 1);
    zox_tst_add("input_field", zox_tst_input_field_ready, zox_tst_input_field_check, 60);
    zox_tst_set_setup("input_field", NULL, 1);
#ifdef zox_sdl3
    zox_tst_add("sdl_text_input", zox_tst_sdl_text_input_ready, zox_tst_sdl_text_input_check, 30);
    zox_tst_set_setup("sdl_text_input", NULL, 1);
#endif
}
