void toggle_fps_viewer(ecs *world, int32_t keycode) {
    if (keycode != zox_key_z) {
        return;
    }

    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }

    zox_geter_value(player, CanvasLink, entity, canvas);

    if (!zox_valid(canvas)) {
        return;
    }
    entity fps_viewer = zox_get_child_by_id(world, canvas, zox_id(FPSDisplay));
    if (fps_viewer) {
        zox_delete(fps_viewer);
    } else {
        spawn_fps_display(world, canvas);
    }

    // zox_log("FPSViewer %s", !fps_viewer ? "Enabled" : "Disabled");
}
