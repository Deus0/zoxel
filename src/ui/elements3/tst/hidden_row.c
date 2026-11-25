// tst: ui3_hidden_row_relabel, rebuilt glyphs stay out of the renderer's list until their row scrolls back.
static int zox_tst_ui3_rendered_glyphs(ecs* world, entity canvas, entity label) {
    entity elements[canvas3_elements_max];
    const int count = canvas3_gather(world, canvas, elements, 0);
    int glyphs = 0;
    for (int i = 0; i < count; i++) {
        glyphs += zox_has(elements[i], Glyph) && zox_get_parent(world, elements[i]) == label;
    }
    return glyphs;
}

static byte zox_tst_ui3_hidden_row_ready(ecs* world) {
    static entity canvas, list, row, label;
    static int step;
    zox_tst_progress(step);
    switch (step) {
        case 0: {
            zox_tst_waiting = "the 3D list's prefabs";
            if (!zox_valid(prefab_canvas3) || !zox_valid(prefab_list)) {
                return 0;
            }
            const int2 size = { 96, 64 };
            canvas = spawn_canvas3(world, prefab_canvas3, float3_zero, 1, size);
            zox_setv(canvas, RenderDisabled, 1);   // query the same element list the renderer uses, without a camera
            SpawnListElement rows[] = {
                { .type = list_element_type_button, .text = "Old" },
                { .type = list_element_type_button, .text = "Other" }
            };
            const SpawnList data = { .count = 2, .visible_count = 1, .elements = rows, .font_size = 12 };
            entity children[2];
            list = spawn_list(world, (LayoutParentData) { .e = canvas }, (LayoutParentData) { .e = canvas },
                (ElementSpawnData) { .prefab = prefab_list, .size = size, .anchor = float2_half },
                data, zox_alignment_centre, children);
            row = children[0];
            label = zox_get_child_by_id(world, row, zox_id(Text));
            zox_tst_check(zox_has(row, Element3) && zox_valid(label), "fixture did not create a 3D labeled row");
            if (!zox_valid(label)) {
                return 1;
            }
            zox_add(label, TextRenderEnabler);
            break;
        }
        case 1:
            zox_tst_waiting = "the visible row's original glyphs";
            if (zox_get_children_count_by_id(world, label, zox_id(Glyph)) != 3) {
                return 0;
            }
            zox_tst_check(zox_tst_ui3_rendered_glyphs(world, canvas, label) == 3, "original row is missing from the render list");
            set_list_start(world, list, 1);
            break;
        case 2:
            zox_tst_waiting = "the first row to scroll out of view";
            if (!zox_getv(row, RenderDisabled)) {
                return 0;
            }
            set_entity_text(world, label, "Longer");
            break;
        case 3: {
            zox_tst_waiting = "the hidden row to build its new glyphs";
            if (zox_get_children_count_by_id(world, label, zox_id(Glyph)) != 6) {
                return 0;
            }
            entity glyphs[6];
            const int count = zox_get_children_by_id(world, label, glyphs, 6, zox_id(Glyph));
            int enabled = 0;
            for (int i = 0; i < count; i++) {
                enabled += !zox_getv(glyphs[i], RenderDisabled);
            }
            zox_tst_check(enabled > 0, "fixture relabel did not produce renderable glyphs under the hidden row");
            zox_tst_check(zox_getv(row, RenderDisabled) && !zox_tst_ui3_rendered_glyphs(world, canvas, label),
                "relabeling let hidden glyphs into the render list");
            set_list_start(world, list, 0);
            break;
        }
        default:
            zox_tst_waiting = "the relabeled row to scroll back into view";
            if (zox_getv(row, RenderDisabled)) {
                return 0;
            }
            zox_tst_check(zox_tst_ui3_rendered_glyphs(world, canvas, label) == 6, "the relabeled row did not draw all six glyphs");
            return 1;
    }
    step++;
    return 0;
}
