// tst: tooltip_back_to_back, a tooltip cleared then set on the very next frame shows its new text
//  - the case of 8158b738b: a Dirty the frame after another lost its Update (the old update_system), the glyph spawn
//      saw the old glyph count and left the new text hidden. Dirty is an observer tag now, that cause is gone: with the
//      glyph spawn's visibility check put back after its count check, this (and tst tooltips' sweeps) still pass
//  - links the canvas' tooltip to a stand in element showing "Abc", clears it (as a deselect does), and the frame after
//      links a second one showing "Xyz", the same length
//  - checks: it shows "Xyz", visible, with its 3 glyphs
//  - run: bash bsh/tst.sh tooltip_back_to_back
static byte zox_tst_tbb_ready(ecs* world) {
    static int step, frames;
    static entity tooltip, first, second;
    zox_tst_progress(step);
    zox_tst_waiting = "the player's canvas and its tooltip";
    const entity canvas = zox_valid(dbg_player) ? zox_get_link(world, dbg_player, CanvasLink) : 0;
    tooltip = zox_valid(canvas) ? zox_get_child_by_id(world, canvas, zox_id(Tooltip)) : 0;
    if (!zox_valid(tooltip)) {
        return 0;
    }
    frames++;
    switch (step) {
        case 0:
            first = zox_new();
            second = zox_new();
            set_tooltip_text(world, first, tooltip, "Abc");
            break;
        case 1:
            zox_tst_waiting = "the first text to show";
            if (frames < 5) {
                return 0;
            }
            zox_tst_check(!zox_getv(tooltip, RenderDisabled), "the first text didn't show");
            zox_unlink(world, tooltip, TooltipLink, first);
            set_entity_text(world, tooltip, "");
            break;
        case 2:
            set_tooltip_text(world, second, tooltip, "Xyz");    // the very next frame
            break;
        case 3: {
            zox_tst_waiting = "the second text to settle";
            if (frames < 5) {
                return 0;
            }
            char* text = get_zext_text(zox_get(tooltip, TextData));
            const int glyphs = zox_get_children_count_by_id(world, tooltip, zox_id(Glyph));
            zox_tst_check(text && !strcmp(text, "Xyz"), "the tooltip says [%s], not [Xyz]", text ? text : "");
            zox_tst_check(!zox_getv(tooltip, RenderDisabled) && glyphs == 3,
                "the tooltip set the frame after it cleared is hidden [%i] with [%i] glyphs, not 3",
                zox_getv(tooltip, RenderDisabled), glyphs);
            free(text);
            zox_delete(first);
            zox_delete(second);
            return 1;
        }
    }
    step++;
    frames = 0;
    return 0;
}

static void zox_tst_tbb_check(ecs* world) {
    (void) world;
}

void zox_tsts_tooltip_back_to_back(ecs* world) {
    (void) world;
    zox_tst_add("tooltip_back_to_back", zox_tst_tbb_ready, zox_tst_tbb_check, 60);
    zox_tst_set_setup("tooltip_back_to_back", NULL, 1);
}
