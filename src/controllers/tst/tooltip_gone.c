// tst: tooltip_gone, a tooltip clears when the element it shows for goes (deleted, i.e. its window closed under the
//  mouse, or popping out): tooltips stuck in game
//  - links the canvas' tooltip to a stand in element with text, as hovering does, then deletes the element
//  - a few frames later the tooltip has no text
//  - run: bash bsh/tst.sh tooltip_gone
byte zox_tst_tg_stage = 0;
int zox_tst_tg_frames = 0;
int zox_tst_tg_shown = 0, zox_tst_tg_left = -1;

byte zox_tst_tooltip_gone_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity canvas = zox_get_link(world, dbg_player, CanvasLink);
    const entity tooltip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));
    if (!zox_valid(tooltip)) {
        zox_tst_check(0, "the canvas has no tooltip");
        return 1;
    }
    zox_tst_tg_frames++;
    zox_tst_progress(zox_tst_tg_frames);
    zox_tst_waiting = "the tooltip to clear";
    static entity element = 0;
    if (zox_tst_tg_stage == 0) {
        // hovering it: linked, its text shown
        element = zox_new();
        zox_link(world, tooltip, TooltipLink, element);
        set_entity_text(world, tooltip, "a stuck tooltip");
        zox_tst_tg_stage = 1;
        zox_tst_tg_frames = 0;
        return 0;
    }
    if (zox_tst_tg_stage == 1) {
        if (zox_tst_tg_frames < 5) {
            return 0;
        }
        zox_tst_tg_shown = zox_get(tooltip, TextData)->length;
        zox_delete(element);
        zox_tst_tg_stage = 2;
        zox_tst_tg_frames = 0;
        return 0;
    }
    if (zox_tst_tg_frames < 10) {
        return 0;
    }
    zox_tst_tg_left = zox_get(tooltip, TextData)->length;
    return 1;
}

void zox_tst_tooltip_gone_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_tg_shown > 0, "the tooltip didn't show its text while linked");
    zox_tst_check(zox_tst_tg_left == 0, "the tooltip kept [%i] characters after its element was deleted", zox_tst_tg_left);
}
