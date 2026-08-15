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
    // entity player = dbg_player;
    // zox_geter_value(player, CameraLink, entity, camera);
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
    zox_log("+ spawned [%s] from prefab [%s]", zox_get_name(canvas), zox_get_name(prefab_canvas));
    if (!zox_valid(canvas)) {
        zox_loge("Spawned Canvas was Invalid");
        return;
    }
    zox_log("Spawned Canvas was Valid");
    zox_set_unique_name(canvas, "tst_canvas");
    entity element = spawn_uic(world, prefab_element_frame, canvas, float2_half, position, size, size, default_fill_color_icon, default_outline_color_icon);
    zox_set(element, Layer, { 1 });
    // zox_set_parent(world, element, canvas);
    // zox_set(element, CanvasLink, { canvas });
    zox_set(canvas, WindowToTop, { element });
    entity element2 = spawn_uic(world, prefab_element_frame, element, float2_half, int2_zero, size2, size2, default_outline_color_icon, default_fill_color_icon);
    zox_set(element2, Layer, { 2 });
    // zox_set_parent(world, element2, element);
    // zox_set(element2, CanvasLink, { canvas });

    // zox_log(" + Spawned [%s] with child [%s]",  zox_get_name(canvas), zox_get_name(element));
    // zox_log("   + gchild [%s]", zox_get_name(element2));

    // debug the created data
    // zox_geter_value(player, CanvasLink, entity, pcanvas);
    // spawn_inspector(world, pcanvas, player, canvas);
    // spawn_inspector(world, pcanvas, player, element);
    // spawn_inspector(world, pcanvas, player, element2);

    // zox_print_children(world, zox_dbg_canvas);
    /*entity dbg_children[32];
    uint count = zox_get_children(world, zox_dbg_canvas, dbg_children, 32);
    for (uint i = 0; i < count; i++) {
        entity child = dbg_children[i];

        if (!zox_valid(child)) {
            zox_log("   - [%i] %s", i, "Invalid");
            continue;
        }

        zox_log("   - [%i] %s", i, zox_get_name(child));
    }*/
}
