// tst: fold_arrow, a fold arrow's click flips its target in its owner's Unfolded (fun/unfold.c)
static byte zox_tst_fold_arrow_ready(ecs* world) {
    static int step = 0;
    static entity canvas, owner, target, arrow;
    zox_tst_progress(step);
    if (step == 0) {
        zox_tst_waiting = "the fixture canvas";
        canvas = zox_tst_ui_fixture_canvas(world);
        if (!canvas) {
            return 0;
        }
        owner = zox_new();
        target = zox_new();
        arrow = spawn_fold_arrow(world, canvas, canvas, owner, target, 0, 16, 32);
        step = 1;
    } else if (step == 1) {
        zox_tst_check(zox_get_link(world, arrow, RowTargetLink) == target, "Target matches");
        zox_tst_check(zox_is_linked(world, owner, Unfolded, target) == 0, "Initially not unfolded");
        zox_getv(arrow, ClickEvent)(world, (ClickEventData) { .clicked = arrow });
        step = 2;
    } else if (step == 2) {
        zox_tst_check(zox_is_linked(world, owner, Unfolded, target) == 1, "Unfolded after one click");
        zox_getv(arrow, ClickEvent)(world, (ClickEventData) { .clicked = arrow });
        step = 3;
    } else if (step == 3) {
        zox_tst_check(zox_is_linked(world, owner, Unfolded, target) == 0, "Folded after two clicks");
        zox_tst_check(zox_get_links_count(world, owner, Unfolded) == 0, "No more unfolded items");
        return 1;
    }
    return 0;
}