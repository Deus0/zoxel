// tst: text_order, glyphs keep their order while a text animates (dialogue style typing)
//  - types a text one character at a time, then shrinks, swaps and regrows it
//  - after each step settles, every glyph must have a unique ChildIndex in [0, n),
//    show the character at that index, and sit at that character's position
//  - run: bash bsh/tst.sh text_order
#define zox_tst_text_settle_frames 4
const char* zox_tst_text_steps_final[] = {
    "Hi!",              // shrink
    "Yo?",              // same length, new characters
    "",                 // empty
    "Goodbye\nworld",   // regrow with a skipped new line
};
const char* zox_tst_text_typed = "Hello brave\nadventurer, zoxel!";
entity zox_tst_text = 0;
int zox_tst_text_step = 0;      // typed characters, then final steps
int zox_tst_text_frames = 0;    // frames since the last change
int zox_tst_text_verified = 0;  // steps checked
int zox_tst_text_failed_steps = 0;

static inline int zox_tst_text_steps_count() {
    return strlen(zox_tst_text_typed) + (int) (sizeof(zox_tst_text_steps_final) / sizeof(char*));
}

// text for a step: typed prefixes, then the final steps
static inline void zox_tst_text_for_step(int step, char* buffer, int size) {
    const int typed = strlen(zox_tst_text_typed);
    if (step < typed) {
        snprintf(buffer, size, "%.*s", step + 1, zox_tst_text_typed);
    } else {
        snprintf(buffer, size, "%s", zox_tst_text_steps_final[step - typed]);
    }
}

// checks the text's glyphs against its TextData, one check per step so a broken text doesn't spam
void zox_tst_text_verify(ecs* world, entity text, const char* label) {
    const TextData* data = zox_get(text, TextData);
    const byte font_size = zox_getv(text, TextFontSize);
    const byte alignment = zox_getv(text, TextAlignment);
    const byte2 padding = zox_getv(text, TextPadding);
    const uint expected = calculate_total_zigels(data->value, data->length);
    byte seen[256] = { 0 };
    uint count = 0;
    int errors = 0;
    char first_error[256] = { 0 };
    iter it2 = zox_children(world, text);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            if (!zox_valid(e2) || !zox_has(e2, Glyph)) {
                continue;
            }
            count++;
            const uint child_index = zox_getv(e2, ChildIndex);
            if (child_index >= expected || child_index >= 256) {
                if (!errors++) snprintf(first_error, sizeof(first_error), "glyph [%s] ChildIndex [%u] >= [%u]", zox_getn(e2), child_index, expected);
                continue;
            }
            if (seen[child_index]++) {
                if (!errors++) snprintf(first_error, sizeof(first_error), "ChildIndex [%u] used twice", child_index);
                continue;
            }
            const uint data_index = child_index_to_text_array_index(data->value, data->length, child_index);
            const byte character = data->value[data_index];
            const byte glyph = zox_getv(e2, GlyphIndex);
            if (glyph != character) {
                if (!errors++) snprintf(first_error, sizeof(first_error), "glyph [%u] shows [%c], expected [%c]",
                    child_index, convert_to_ascii(glyph), convert_to_ascii(character));
                continue;
            }
            const int2 position = calculate_position(data->value, data->length, data_index, font_size, alignment, padding, default_line_padding);
            const int2 actual = zox_getv(e2, LayoutPosition);
            if (!int2_equals(actual, position)) {
                if (!errors++) snprintf(first_error, sizeof(first_error), "glyph [%u] [%c] at [%ix%i], expected [%ix%i]",
                    child_index, convert_to_ascii(character), actual.x, actual.y, position.x, position.y);
            }
        }
    }
    if (count != expected && !errors) {
        snprintf(first_error, sizeof(first_error), "[%u] glyphs for [%u] characters", count, expected);
    }
    if (count != expected) {
        errors++;
    }
    zox_tst_check(!errors, "[%s] [%i] glyph errors, first: %s", label, errors, first_error);
    zox_tst_text_verified++;
    if (errors) {
        zox_tst_text_failed_steps++;
    }
}

byte zox_tst_text_order_ready(ecs* world) {
    if (!zox_valid(zox_tst_text)) {
        zox_tst_waiting = "a player with a canvas";
        entity canvas = zox_valid(dbg_player) ? zox_get_link(world, dbg_player, CanvasLink) : 0;
        if (!zox_valid(canvas)) {
            return 0;
        }
        zox_tst_text = spawn_text(world, prefab_text, canvas,
            int2_zero, float2_half, 24, zox_alignment_centre, (byte2) { 4, 4 },
            "", color_white, color_black);
        zox_tst_text_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the text animation steps to finish";
    zox_tst_text_frames++;
    if (zox_tst_text_frames < zox_tst_text_settle_frames) {
        return 0;
    }
    char text[128];
    if (zox_tst_text_step > 0) {
        // verify the previous step once it settled
        zox_tst_text_for_step(zox_tst_text_step - 1, text, sizeof(text));
        zox_tst_text_verify(world, zox_tst_text, text);
        // fail fast, one broken step tells us enough
        if (zox_tst_text_failed_steps >= 3) {
            return 1;
        }
    }
    if (zox_tst_text_step >= zox_tst_text_steps_count()) {
        return 1;
    }
    zox_tst_text_for_step(zox_tst_text_step, text, sizeof(text));
    set_entity_text(world, zox_tst_text, text);
    zox_tst_text_step++;
    zox_tst_text_frames = 0;
    return 0;
}

void zox_tst_text_order_check(ecs* world) {
    (void) world;
    zox_log("[tst] text steps verified [%i], failed [%i]", zox_tst_text_verified, zox_tst_text_failed_steps);
    zox_tst_check(zox_tst_text_verified > 0, "no text steps were verified");
}

// tst: text_data, the zext helpers labels and dialogue use (pure, no entities)
//  - ascii round trips through zigels for letters, digits and dialogue punctuation
//  - set_zext / is_zext, empty and NULL text
//  - set_zext_cut / is_zext_cut at every length of a 300 character line (dialogue typing, TargetText is 512)
//  - glyph counts and child index -> text index skip spaces and new lines, and agree with each other
//  - line sizes: widest line, line count (a trailing new line isn't a line)
byte zox_tst_text_data_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_text_data_check(ecs* world) {
    (void) world;
    // round trips
    // printable ascii the glyph set can't show (needs font art, so logged, not failed), i.e. '
    char unsupported[128] = { 0 };
    int unsupported_count = 0;
    for (char c = 32; c < 127; c++) {
        if (convert_to_ascii(convert_ascii(c)) != c) {
            unsupported[unsupported_count++] = c;
        }
    }
    zox_log("[tst] [%i] printable ascii without glyphs: [%s]", unsupported_count, unsupported);
    const char* expected = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 .,!?-:\n";
    char failed[128] = { 0 };
    int failed_count = 0;
    for (int i = 0; expected[i]; i++) {
        const char c = expected[i];
        const char back = convert_to_ascii(convert_ascii(c));
        if (back != c && failed_count < 120) {
            failed[failed_count++] = c == '\n' ? '|' : c;
        }
    }
    zox_tst_check(!failed_count, "[%i] characters don't round trip through zigels: [%s]", failed_count, failed);
    // set / is
    TextData text = { 0 };
    set_zext(&text, "Hello zoxel");
    zox_tst_check(text.length == 11 && is_zext(&text, "Hello zoxel") && !is_zext(&text, "Hello zoxe!") && !is_zext(&text, "Hello"),
        "set_zext / is_zext disagree");
    set_zext(&text, "");
    zox_tst_check(text.length == 0 && is_zext(&text, "") && is_zext(&text, NULL), "empty text isn't empty");
    set_zext(&text, NULL);
    zox_tst_check(text.length == 0, "NULL text has length [%i]", text.length);
    // typing a long dialogue line one character at a time
    char line[301];
    for (int i = 0; i < 300; i++) {
        line[i] = "the quick brown fox, jumps over the lazy dog!\n"[i % 46];
    }
    line[300] = 0;
    int cut_errors = 0;
    int first_bad = -1;
    for (uint n = 0; n <= 300; n++) {
        set_zext_cut(&text, line, n);
        byte ok = text.length == n && is_zext_cut(&text, line, n);
        for (uint i = 0; ok && i < n; i++) {
            ok = text.value[i] == convert_ascii(line[i]);
        }
        if (!ok) {
            cut_errors++;
            if (first_bad == -1) {
                first_bad = n;
            }
        }
    }
    zox_tst_check(!cut_errors, "typing a 300 character line: [%i] lengths wrong, first at [%i] (length [%i])", cut_errors, first_bad, text.length);
    // glyphs and indexes
    set_zext(&text, "ab c\nde  f\n");
    const uint glyphs = calculate_total_zigels(text.value, text.length);
    zox_tst_check(glyphs == 6, "[%u] glyphs in \"ab c|de  f|\", expected 6", glyphs);
    int index_errors = 0;
    for (uint c = 0; c < glyphs; c++) {
        const uint index = child_index_to_text_array_index(text.value, text.length, c);
        index_errors += index >= (uint) text.length ||
            is_skip_zigel(text.value[index]) ||
            calculate_zigel_index(text.value, text.length, c) != text.value[index];
    }
    zox_tst_check(!index_errors, "[%i] child index -> text index errors", index_errors);
    zox_tst_check(get_zext_max_x(text.value, text.length) == 5, "widest line is [%i], expected 5", get_zext_max_x(text.value, text.length));
    zox_tst_check(get_zext_max_y(text.value, text.length) == 2, "line count is [%i], expected 2", get_zext_max_y(text.value, text.length));
    zox_tst_check(get_zext_x(text.value, 6) == 1 && get_zext_y(text.value, text.length, 6) == 1, "position of [e] is [%ix%i], expected [1x1]",
        get_zext_x(text.value, 6), get_zext_y(text.value, text.length, 6));
    // wrapping: spaces become new lines where a word would pass the width, long words overflow, new lines kept
    char wrap[64];
    snprintf(wrap, sizeof(wrap), "the quick brown fox jumps");
    text_wrap(wrap, 10);
    zox_tst_check(!strcmp(wrap, "the quick\nbrown fox\njumps"), "wrapped to 10 is [%s]", wrap);
    snprintf(wrap, sizeof(wrap), "a extraordinarily long\nword ok");
    text_wrap(wrap, 6);
    zox_tst_check(!strcmp(wrap, "a\nextraordinarily\nlong\nword\nok"), "a long word wrapped to 6 is [%s]", wrap);
    snprintf(wrap, sizeof(wrap), "fits");
    text_wrap(wrap, 4);
    zox_tst_check(!strcmp(wrap, "fits"), "a line as wide as its width wrapped [%s]", wrap);
    resize_TextData(&text, 0);
}


// tst: ui_scale, a ui's LocalScale1 scales it and its children (transforms2/sys/children.c), and pops (ui/fun/pop.c)
//  - a text at half scale: its glyphs' Scale1 and matrices are half, their offsets from it half their LocalPosition2
//  - ui_pop_in grows it from nothing back to 1, ui_pop_out shrinks it and deletes it
entity zox_tst_scale_text = 0;
int zox_tst_scale_stage = 0;
int zox_tst_scale_frames = 0;
int zox_tst_scale_glyphs = 0;       // glyphs checked at half scale
int zox_tst_scale_wrong = 0;        // of them, wrong scale, matrix or position
float zox_tst_scale_popped = 0;     // its LocalScale1 after popping in

byte zox_tst_ui_scale_ready(ecs* world) {
    zox_tst_scale_frames++;
    zox_tst_progress(zox_tst_scale_stage);
    if (zox_tst_scale_stage == 0) {
        zox_tst_waiting = "a player with a canvas";
        const entity canvas = zox_valid(dbg_player) ? zox_get_link(world, dbg_player, CanvasLink) : 0;
        if (!zox_valid(canvas)) {
            return 0;
        }
        zox_tst_scale_text = spawn_text(world, prefab_text, canvas, int2_zero, float2_half, 24, zox_alignment_centre,
            (byte2) { 4, 4 }, "scale me", color_white, color_black);
        zox_setv(zox_tst_scale_text, LocalScale1, 0.5f);
        zox_tst_scale_stage = 1;
        zox_tst_scale_frames = 0;
        return 0;
    }
    if (zox_tst_scale_stage == 1) {
        zox_tst_waiting = "its glyphs at half scale";
        if (zox_tst_scale_frames < 30) {
            return 0;
        }
        const float2 origin = zox_getv(zox_tst_scale_text, Position2);
        iter it = zox_children(world, zox_tst_scale_text);
        while (zox_children_next(it)) {
            for (int i = 0; i < it.count; i++) {
                const entity glyph = it.entities[i];
                if (!zox_has(glyph, Glyph)) {
                    continue;
                }
                zox_tst_scale_glyphs++;
                const float4x4 matrix = zox_getv(glyph, TransformMatrix);
                const float2 offset = float2_sub(zox_getv(glyph, Position2), origin);
                const float2 expected = float2_multiply1(zox_getv(glyph, LocalPosition2), 0.5f);
                zox_tst_scale_wrong += fabsf(zox_getv(glyph, Scale1) - 0.5f) > 0.001f || fabsf(matrix.x.x - 0.5f) > 0.001f ||
                    float2_distance(offset, expected) > 0.0001f;
            }
        }
        ui_pop_in(world, zox_tst_scale_text);
        zox_tst_scale_stage = 2;
        zox_tst_scale_frames = 0;
        return 0;
    }
    if (zox_tst_scale_stage == 2) {
        zox_tst_waiting = "it to pop in";
        if (zox_tst_scale_frames < 60) {
            return 0;
        }
        zox_tst_scale_popped = zox_getv(zox_tst_scale_text, LocalScale1);
        ui_pop_out(world, zox_tst_scale_text);
        zox_tst_scale_stage = 3;
        zox_tst_scale_frames = 0;
        return 0;
    }
    zox_tst_waiting = "it to pop out and be deleted";
    return !zox_valid(zox_tst_scale_text) || zox_tst_scale_frames > 120;
}

void zox_tst_ui_scale_check(ecs* world) {
    zox_tst_check(zox_tst_scale_glyphs == 7, "[%i] glyphs checked, \"scale me\" has 7", zox_tst_scale_glyphs);
    zox_tst_check(!zox_tst_scale_wrong, "[%i] glyphs didn't follow their text's half scale (scale, matrix or offset)", zox_tst_scale_wrong);
    zox_tst_check(fabsf(zox_tst_scale_popped - 1) < 0.001f, "popped in to [%.3f], not 1", zox_tst_scale_popped);
    zox_tst_check(!zox_valid(zox_tst_scale_text), "it wasn't deleted after popping out");
}

void zox_tsts_texts() {
    zox_tst_add("text_data", zox_tst_text_data_ready, zox_tst_text_data_check, 30);
    zox_tst_add("text_order", zox_tst_text_order_ready, zox_tst_text_order_check, 60);
    zox_tst_add("ui_scale", zox_tst_ui_scale_ready, zox_tst_ui_scale_check, 60);
}
