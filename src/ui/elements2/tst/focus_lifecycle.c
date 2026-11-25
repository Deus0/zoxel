// tst: input_focus_lifecycle, real clicks transfer focus and deleting the focused panel releases text input.
static entity zox_tst_focus_field(ecs* world, entity parent, int x) {
    const entity field = spawn_button(world, prefab_button, parent, "", (int2) { x, 0 }, (int2) { 120, 32 },
        float2_half, zox_alignment_centre, 16, byte2_single(4), button_fill, button_outline,
        button_font_fill, button_font_outline).x;
    zox_add(field, TextInput);
    zox_setv(field, ClickEvent, &on_click_input_field);
    return field;
}

static byte zox_tst_focus_has_text(ecs* world, entity field, const char* expected) {
    const entity label = input_field_label(world, field);
    char* text = zox_valid(label) ? get_zext_text(zox_get(label, TextData)) : NULL;
    const byte matches = text && !strcmp(text, expected);
    free(text);
    return matches;
}

static byte zox_tst_input_focus_ready(ecs* world) {
    static entity canvas, panel, first, second, next;
    static int step, click_step;
    static int2 first_point, second_point;
    zox_tst_progress(step);
    switch (step) {
        case 0:
            zox_tst_waiting = "the input fixture canvas";
            canvas = zox_tst_ui_fixture_canvas(world);
            if (!canvas) {
                return 0;
            }
            panel = spawn_layout2(world, prefab_layout2, canvas, int2_zero, (int2) { 320, 64 }, float2_half, 8);
            first = zox_tst_focus_field(world, canvas, -80);
            second = zox_tst_focus_field(world, panel, 80);
            break;
        case 1:
            zox_tst_waiting = "clicking the first field";
            if (!zox_tst_ui_pointer_position(world, canvas, first, &first_point) ||
                !zox_tst_ui_click_at(first_point, &click_step)) {
                return 0;
            }
            zox_tst_check(input_field_focus == first && zox_text_input_wanted, "first click did not activate text input");
            add_text_typed("Alpha");
            break;
        case 2:
            zox_tst_waiting = "the first field to receive Alpha";
            if (zox_text_typed[0]) {
                return 0;
            }
            zox_tst_check(zox_tst_focus_has_text(world, first, "Alpha_"), "first field did not show Alpha and its focus marker");
            break;
        case 3:
            zox_tst_waiting = "clicking the second field";
            if (!zox_tst_ui_pointer_position(world, canvas, second, &second_point) ||
                !zox_tst_ui_click_at(second_point, &click_step)) {
                return 0;
            }
            zox_tst_check(input_field_focus == second && zox_text_input_wanted, "focus did not transfer to the second field");
            zox_tst_check(zox_tst_focus_has_text(world, first, "Alpha"), "first field retained its focus marker after transfer");
            add_text_typed("Beta");
            break;
        case 4:
            zox_tst_waiting = "the second field to receive Beta";
            if (zox_text_typed[0]) {
                return 0;
            }
            zox_tst_check(zox_tst_focus_has_text(world, second, "Beta_"), "second field did not show Beta and its focus marker");
            zox_tst_check(zox_tst_focus_has_text(world, first, "Alpha"), "typing into the second field changed the first");
            zox_delete(panel);
            break;
        case 5:
            zox_tst_waiting = "the deleted field's input to be released";
            if (input_field_focus || zox_text_input_wanted) {
                return 0;
            }
#ifdef zox_sdl
            if (zox_sdl_text_input_active(zox_getv(main_app, SDLWindow))) {
                return 0;
            }
#endif
            zox_tst_check(!zox_valid(second), "closing the panel left its field alive");
            add_text_typed("Discard");
            break;
        case 6:
            zox_tst_waiting = "discarding text typed with no focused field";
            if (zox_text_typed[0]) {
                return 0;
            }
            next = zox_tst_focus_field(world, canvas, 80);
            break;
        default:
            zox_tst_waiting = "clicking a fresh field after closing the old one";
            if (!zox_tst_ui_pointer_position(world, canvas, next, &second_point) ||
                !zox_tst_ui_click_at(second_point, &click_step)) {
                return 0;
            }
            zox_tst_check(input_field_focus == next && zox_tst_focus_has_text(world, next, "_"),
                "fresh field did not focus empty after discarding text");
            zox_tst_check(zox_tst_focus_has_text(world, first, "Alpha"), "closing the second field changed the first");
            return 1;
    }
    step++;
    return 0;
}
