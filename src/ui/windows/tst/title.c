// tst: window_title_fit, a window's title stays inside its header as the window narrows (sys/layout.c
//  window_header_fit, texts/sys/fit.c)
//  - two windows with a long title, one centred (no close button), one beside its close button (on the left)
//  - made narrow by their right handles: each title's font shrinks, its width inside the header beside its buttons,
//      the left one's left edge kept; widened again: the font back to its size
//  - before: titles kept their font and ran past their headers
//  - run: bash bsh/tst.sh window_title_fit
#define zox_tst_title_font 12
static const char* zox_tst_title_text = "Inventory And Gear";     // fits a narrow header at a font of 5

static void zox_tst_title_close(ecs* world, ClickEventData data) {
    (void) world;
    (void) data;
}

static entity zox_tst_title_text_of(ecs* world, entity window) {
    const entity header = zox_get_child_by_id(world, window, zox_id(Header));
    return header ? zox_get_child_by_id(world, header, zox_id(TextFit)) : 0;
}

// its glyphs' width at its font now
static int zox_tst_title_width(ecs* world, entity text) {
    const TextData* data = zox_get(text, TextData);
    return calculate_zext_size(data->value, data->length, zox_getv(text, TextFontSize), zox_getv(text, TextPadding),
        default_line_padding).x;
}

// the title's left edge, from its header's left
static int zox_tst_title_left(ecs* world, entity text) {
    return zox_getv(text, LayoutPosition).x - zox_tst_title_width(world, text) / 2;
}

static byte zox_tst_window_title_fit_ready(ecs* world) {
    static int step;
    static entity canvas, windows[2], texts[2];
    static int left_before;
    int count;
    zox_tst_progress(step);
    switch (step) {
        case 0: {
            zox_tst_waiting = "the fixture canvas";
            canvas = zox_tst_ui_fixture_canvas(world);
            if (!canvas) {
                return 0;
            }
            for (int i = 0; i < 2; i++) {
                windows[i] = spawn_window(world, prefab_window, prefab_body, (char*) zox_tst_title_text, canvas,
                    (int2) { 0, i ? -100 : 100 }, (int2) { 400, 120 }, float2_half, zox_tst_title_font, byte2_single(4),
                    i ? (void*) zox_tst_title_close : NULL).x;
            }
            break;
        }
        case 1:
            zox_tst_waiting = "the titles";
            for (int i = 0; i < 2; i++) {
                texts[i] = zox_tst_title_text_of(world, windows[i]);
                if (!texts[i] || !zox_tst_window_resize_handle(world, windows[i], (float2) { 1, 0.5f }, &count)) {
                    return 0;
                }
            }
            for (int i = 0; i < 2; i++) {
                zox_tst_check(zox_getv(texts[i], TextFontSize) == zox_tst_title_font, "[%i] title font [%i] in a wide window",
                    i, zox_getv(texts[i], TextFontSize));
                const entity handle = zox_tst_window_resize_handle(world, windows[i], (float2) { 1, 0.5f }, &count);
                zox_setv(handle, DraggingDelta, ((int2) { -260, 0 }));
                zox_setv(handle, DraggableState, 1);
            }
            left_before = zox_tst_title_left(world, texts[1]);
            break;
        case 2:
            zox_tst_waiting = "the titles to shrink into their headers";
            for (int i = 0; i < 2; i++) {
                zox_setv(zox_tst_window_resize_handle(world, windows[i], (float2) { 1, 0.5f }, &count), DraggableState, 0);
                // too narrow to read at its least font, it hides instead (TextFitMinSize)
                const byte hidden = zox_getv(texts[i], RenderDisabled);
                if (zox_getv(texts[i], TextFontSize) >= zox_tst_title_font ||
                    (!hidden && zox_tst_title_width(world, texts[i]) > zox_getv(texts[i], TextFitWidth)) ||
                    zox_getv(texts[i], TextFitWidth) > zox_getv(windows[i], LayoutSize).x) {
                    static char why[128];
                    snprintf(why, 128, "[%i] font [%i] width [%i] room [%i] window [%i] pad [%i]", i, zox_getv(texts[i], TextFontSize),
                        zox_tst_title_width(world, texts[i]), zox_getv(texts[i], TextFitWidth), zox_getv(windows[i], LayoutSize).x, zox_getv(texts[i], TextPadding).x);
                    zox_tst_waiting = why;
                    return 0;
                }
            }
            for (int i = 0; i < 2; i++) {
                const entity handle = zox_tst_window_resize_handle(world, windows[i], (float2) { 1, 0.5f }, &count);
                zox_setv(handle, DraggableState, 0);
                const int width = zox_tst_title_width(world, texts[i]);
                const int room = zox_getv(texts[i], TextFitWidth);
                const int header = zox_getv(windows[i], LayoutSize).x;
                zox_tst_check((zox_getv(texts[i], RenderDisabled) || width <= room) && room <= header,
                    "[%i] title [%i] wide, its room [%i], its header [%i]",
                    i, width, room, header);
            }
            // the close button's title keeps its left edge as its font shrinks
            zox_tst_check(abs(zox_tst_title_left(world, texts[1]) - left_before) <= 1,
                "the left title's edge moved from [%i] to [%i]", left_before, zox_tst_title_left(world, texts[1]));
            for (int i = 0; i < 2; i++) {
                const entity handle = zox_tst_window_resize_handle(world, windows[i], (float2) { 1, 0.5f }, &count);
                zox_setv(handle, DraggingDelta, ((int2) { 260, 0 }));
                zox_setv(handle, DraggableState, 1);
            }
            break;
        case 3:
            zox_tst_waiting = "the titles back to their size";
            for (int i = 0; i < 2; i++) {
                if (zox_getv(texts[i], TextFontSize) != zox_tst_title_font) {
                    return 0;
                }
            }
            for (int i = 0; i < 2; i++) {
                zox_setv(zox_tst_window_resize_handle(world, windows[i], (float2) { 1, 0.5f }, &count), DraggableState, 0);
            }
            return 1;
    }
    step++;
    return 0;
}
