// tst: ui3_deferred_spawn, the shared spawn functions select 3D twins before deferred parent components land.
static byte zox_tst_ui3_deferred_ready(ecs* world) {
    static entity canvas, frame, button, label;
    if (!canvas) {
        zox_tst_waiting = "the 3D canvas and widget prefabs";
        if (!zox_valid(prefab_canvas3) || !zox_valid(prefab_button)) {
            return 0;
        }
        const int2 size = { 96, 64 };
        zox_defer_begin();
        canvas = spawn_canvas3(world, prefab_canvas3, float3_zero, 1, size);
        zox_setv(canvas, RenderDisabled, 1);
        zox_tst_check(!zox_has(canvas, Canvas3), "fixture spawned the canvas outside the deferred queue");
        frame = spawn_ui(world, prefab_frame, canvas, float2_half, int2_zero, size, size);
        button = spawn_button(world, prefab_button, frame, "Deferred", int2_zero, size, float2_half,
            zox_alignment_centre, 12, byte2_single(2), button_fill, button_outline, color_white, color_black).x;
        zox_defer_end();
        label = zox_get_child_by_id(world, button, zox_id(Text));
        zox_tst_check(zox_has(canvas, Canvas3), "merged canvas has no Canvas3");
        zox_tst_check(zox_get_parent(world, frame) == canvas && zox_has(frame, Element3), "deferred frame is missing or 2D");
        zox_tst_check(zox_get_parent(world, button) == frame && zox_has(button, Element3), "deferred button is missing or 2D");
        zox_tst_check(zox_valid(label) && zox_has(label, LocalPosition3D), "deferred label is missing or 2D");
        if (!zox_has(frame, Element3) || !zox_has(button, Element3) || !zox_has(label, LocalPosition3D)) {
            return 1;
        }
        zox_tst_check(zox_getv(frame, Layer) < zox_getv(button, Layer) && zox_getv(button, Layer) < zox_getv(label, Layer),
            "deferred widget layers do not paint from parent to child");
    }
    zox_tst_progress(1);
    zox_tst_waiting = "the deferred text's glyphs";
    entity glyphs[8];
    if (zox_get_children_by_id(world, label, glyphs, 8, zox_id(Glyph)) < 8) {
        return 0;
    }
    for (int i = 0; i < 8; i++) {
        zox_tst_check(zox_has(glyphs[i], Element3) && zox_getv(glyphs[i], Layer) > zox_getv(button, Layer),
            "deferred glyph [%i] cannot draw above its button in 3D", i);
    }
    return 1;
}
