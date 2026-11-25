// tst: input_field_boundaries, text stays capped and navigation keys leave both empty and full fields valid.
static byte zox_tst_input_boundaries_ready(ecs* world) {
    static entity field, keyboard;
    static int step;
    static char full[input_field_max_length + 1];
    zox_tst_progress(step);
    const entity canvas = zox_valid(dbg_player) ? zox_get_link(world, dbg_player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the fixture canvas";
        return 0;
    }
    switch (step) {
        case 0: {
            field = zox_tst_focus_field(world, canvas, 0);
            keyboard = zox_new();
            zox_set(keyboard, Keyboard, { 0 });
            memset(full, 'x', input_field_max_length);
            char overlong[input_field_max_length + 5];
            snprintf(overlong, sizeof(overlong), "%sabcd", full);
            input_field_set_focus(world, field);
            input_field_set_value(world, field, overlong);
            break;
        }
        case 1: {
            char marked[input_field_max_length + 2];
            snprintf(marked, sizeof(marked), "%s_", full);
            zox_tst_check(zox_tst_focus_has_text(world, field, marked), "the length limit swallowed the focus marker or kept excess characters");
            add_text_typed("extra");
            break;
        }
        case 2: {
            if (zox_text_typed[0]) {
                return 0;
            }
            char* value = input_field_value(world, field);
            zox_tst_check(!strcmp(value, full), "the full field changed after more typing");
            free(value);
            zox_set(keyboard, Keyboard, { .enter.pressed_this_frame = 1 });
            break;
        }
        case 3:
            zox_tst_check(!input_field_focus && !zox_text_input_wanted && zox_tst_focus_has_text(world, field, full),
                "Enter did not clear focus and preserve the field's capped value");
            zox_set(keyboard, Keyboard, { .backspace.pressed_this_frame = 1 });
            input_field_set_focus(world, field);
            input_field_set_value(world, field, "");
            break;
        case 4:
            zox_tst_check(input_field_focus == field && zox_tst_focus_has_text(world, field, "_"),
                "Backspace on empty changed focus or erased its marker");
            zox_set(keyboard, Keyboard, { .escape.pressed_this_frame = 1 });
            break;
        case 5: {
            const entity label = input_field_label(world, field);
            const byte cleared = !input_field_focus && !zox_text_input_wanted && zox_get(label, TextData)->length == 0;
            zox_tst_check(cleared,
                "Escape did not clear focus and leave the empty field empty");
            if (!cleared) {
                return 1;
            }
            break;
        }
        default:
            zox_tst_waiting = "the empty field's focus glyph to be removed";
            return !zox_get_children_count_by_id(world, input_field_label(world, field), zox_id(Glyph));
    }
    step++;
    return 0;
}
