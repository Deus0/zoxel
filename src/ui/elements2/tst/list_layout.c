// tst: list_layout, a list places its rows top to bottom in order, each a padding apart, and again when one resizes
//  (transforms/layouts/containers: list_system, list_element_position_system)
//  - a list of 4 rows: each row's y is the one above's, less half of both heights and the list's padding
//  - a row made 20 taller after the first layout: the rows place again, the ones under it 20 lower
//  - before: only a list scrolling sideways (ListScrollX) placed again when a row resized, so a row sized after the
//      list's one layout stayed where it spawned (the main menu's buttons on top of each other); antitest: the second
//      wait times out
//  - run: bash bsh/tst.sh list_layout
#define zox_tst_list_layout_rows 4

// 1 if every row sits under the one before by half of both heights and the padding
static byte zox_tst_list_layout_placed(ecs* world, entity list, const entity* rows) {
    const int padding = zox_getv(list, ListPadding).y;
    for (int i = 1; i < zox_tst_list_layout_rows; i++) {
        const int above = zox_getv(rows[i - 1], LayoutPosition).y, y = zox_getv(rows[i], LayoutPosition).y;
        const int gap = (zox_getv(rows[i - 1], LayoutSize).y + zox_getv(rows[i], LayoutSize).y) / 2 + padding;
        if (abs(above - y - gap) > 1) {
            return 0;
        }
    }
    return 1;
}

static byte zox_tst_list_layout_ready(ecs* world) {
    static entity canvas, list;
    static entity rows[zox_tst_list_layout_rows];
    static int before[zox_tst_list_layout_rows];
    static int step;
    zox_tst_progress(step);
    switch (step) {
        case 0: {
            zox_tst_waiting = "the fixture canvas";
            canvas = zox_tst_ui_fixture_canvas(world);
            if (!canvas) {
                return 0;
            }
            const LayoutParentData root = { .e = canvas };
            SpawnListElement elements[] = { { .text = "New Game" }, { .text = "Options" }, { .text = "Credits" },
                { .text = "Exit" } };
            list = spawn_list(world, root, root,
                (ElementSpawnData) { .prefab = prefab_list, .anchor = float2_half, .size = { 200, 200 } },
                (SpawnList) { .elements = elements, .count = zox_tst_list_layout_rows,
                    .visible_count = zox_tst_list_layout_rows, .font_size = 12, .margins = { 4, 4 },
                    .padding = { 6, 6 }, .button_padding = { 4, 4 } },
                zox_alignment_centre, rows);
            break;
        }
        case 1:
            zox_tst_waiting = "the rows placed";
            if (!zox_tst_list_layout_placed(world, list, rows)) {
                return 0;
            }
            for (int i = 0; i < zox_tst_list_layout_rows; i++) {
                before[i] = zox_getv(rows[i], LayoutPosition).y;
            }
            zox_tst_check(before[0] > before[zox_tst_list_layout_rows - 1], "the first row isn't on top");
            // the second row 20 taller
            int2 size = zox_getv(rows[1], LayoutSize);
            size.y += 20;
            zox_setv(rows[1], LayoutSize, size);
            zox_add(rows[1], LayoutSizeDirty);
            break;
        case 2:
            zox_tst_waiting = "the rows placed again round the taller row";
            if (zox_getv(rows[2], LayoutPosition).y == before[2] || !zox_tst_list_layout_placed(world, list, rows)) {
                return 0;
            }
            zox_tst_check(zox_getv(rows[0], LayoutPosition).y == before[0], "the top row moved");
            for (int i = 2; i < zox_tst_list_layout_rows; i++) {
                zox_tst_check(abs(before[i] - zox_getv(rows[i], LayoutPosition).y - 20) <= 1,
                    "row [%i] moved [%i], not 20 down", i, before[i] - zox_getv(rows[i], LayoutPosition).y);
            }
            return 1;
    }
    step++;
    return 0;
}
