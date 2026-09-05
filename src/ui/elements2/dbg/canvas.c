 entity zox_dbg_canvas;

extern entity spawn_inspector(ecs*, entity, entity, entity);

// TODO: Refactor CanvasLink and just scan for root canvas
// TODO: Use ecs pair here too for canvaslinks
// DONE: Remove this and use new parent system for rendering / transform
void zox_dbg_spawn_canvas(ecs* world, ClickEventData data) {
    if (!zox_valid(prefab_canvas)) {
        zox_loge("Invalid Prefab [prefab_canvas]");
        return;
    }
    entity app = main_app;
    entity camera = ui_cameras[0];
    int2 screen_size = get_screen_size();
    float4 stc = single_screen_to_canvas;
    int2 size = int2_single(ui_scale * 64);
    int2 size2 = int2_single(ui_scale * 32);
    int2 position = int2_zero; // (int2) { size.x, size.y * 2 };
    if (zox_valid(zox_dbg_canvas)) {
        zox_log("- destroyed [%s]", zox_get_name(zox_dbg_canvas));
        zox_delete(zox_dbg_canvas);
        zox_dbg_canvas = 0;
        return;
    }
    entity canvas = spawn_canvas(world, prefab_canvas, camera, screen_size, stc, app);
    // entity canvas = zox_ins(prefab_canvas);
    zox_dbg_canvas = canvas;  // global debug ref
    zox_log("+ spawned [%s] from prefab [%s]",
        zox_get_name(canvas),
        zox_get_name(prefab_canvas));
    if (!zox_valid(canvas)) {
        zox_loge("Spawned Canvas was Invalid");
        return;
    }
    zox_log("Spawned Canvas was Valid");
    zox_set_unique_name(canvas, "tst_canvas");
    entity element = spawn_uic(
        world,
        prefab_element_frame,
        canvas,
        float2_half,
        position,
        size,
        size,
        default_fill_color_icon,
        default_outline_color_icon);
    zox_setv(element, Layer, 1);
    zox_setv(canvas, WindowToTop, element);
    entity element2 = spawn_uic(
        world,
        prefab_element_frame,
        element,
        float2_half,
        int2_zero,
        size2,
        size2,
        default_outline_color_icon,
        default_fill_color_icon);
    zox_set(element2, Layer, { 2 });
}
