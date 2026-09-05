entity dbg_ui_cheats;

void zox_dbg_toggle_free_roam(ecs* world, ClickEventData data) {
    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }
    entity character = zox_get_link(world, player, Character);
    entity camera = zox_get_link(world, player, Camera);
    if (!zox_valid(camera)) {
        zox_loge("[zox_dbg_toggle_free_roam] No Camera on Player");
        return;
    }
    byte roaming = zox_has(camera, Roaming);
    roaming = !roaming;
    byte state;
    if (!roaming) {
        state = zox_camera_state_first_person;
        attach_camera_to_character(
            world,
            camera,
            character);
    } else {
        state = zox_camera_state_free;
        set_camera_free(
            world,
            camera);
    }
    zox_setv(camera, CameraState, state);
    zox_log("[FreeRoam] Camera [%s] State [%i] Roam [%i]",
        zox_getn(camera),
        state,
        roaming);
}

void zox_dbg_toggle_fly_mode(ecs* world, ClickEventData data) {
    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }
    entity character = zox_get_link(world, player, Character);
    if (!zox_valid(character)) {
        return;
    }
    zox_log("Toggling Fly Mode: %s", zox_get_name(character));
    zox_setv(character, FlyMode, !zox_getv(character, FlyMode));
}

void zox_dbg_add_no_clip(ecs* world, ClickEventData data) {
    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }
    entity character = zox_get_link(world, player, Character);
    if (!zox_valid(character)) {
        return;
    }
    zox_log("Adding No Clip: %s", zox_get_name(character));
    if (zox_has(character, NoClip)) {
        zox_remove(character, NoClip);
    } else {
        zox_add(character, NoClip);
    }
}

void zox_dbg_ui_cheats(ecs* world, int32_t keycode) {
    if (keycode != zox_key_l) {
        return;
    }
    zox_log("Toggling Debug UI [Cheats]: %s", dbg_ui_cheats ? zox_get_name(dbg_ui_cheats) : "None");
    if (zox_valid(dbg_ui_cheats)) {
        zox_delete(dbg_ui_cheats);
        dbg_ui_cheats = 0;
        return;
    }
    entity player = dbg_player;
    entity canvas = zox_get_link(world, player, Canvas);
    if (!zox_valid(canvas)) {
        return;
    }
    // # List #
    int elements_count = 0;
    byte visible_count = 6;
    SpawnListElement elements[zox_max_test_uis];
    byte alignment = zox_alignment_centre;
    byte can_close = 1;
    byte header_font_size = 6 * ui_scale;
    byte list_font_size = 4 * ui_scale;
    byte2 list_padding = byte2_single(2 * ui_scale);
    // UI
    elements[elements_count++] = (SpawnListElement) {
        .text = "Free Roam",
        .on_click = { &zox_dbg_toggle_free_roam },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Fly Mode",
        .on_click = { &zox_dbg_toggle_fly_mode },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "No Clip",
        .on_click = { &zox_dbg_add_no_clip },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "All Items",
        .on_click = { &zox_tst_all_items },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "All Skills",
        .on_click = { &zox_tst_all_skills },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Level Up",
        .on_click = { &zox_tst_level_up },
    };
    // Test our uis
    entity spawned[elements_count];

    entity3 e3 = spawn_window_list(
        world,
        prefab_window,
        player,
        "Cheats",
        header_font_size,
        list_font_size,
        (ClickEvent) { NULL },
        can_close,
        0,
        0,
        alignment,
        dbg_ui_alignment,
        list_padding,
        spawned,
        elements,
        elements_count,
        visible_count);
    zox_set_unique_name(e3.x, "dbg_ui_cheats");
    // zox_add(e3.x, MenuTest);
    zox_add(e3.x, NavigationWindow);
    // zox_set(e3.z, TooltipEvent, { &tooltip_event_zoxel_header });
    // zox_set(spawned[0], TooltipEvent, { &tooltip_event_main_menu });
    dbg_ui_cheats = e3.x;
}
