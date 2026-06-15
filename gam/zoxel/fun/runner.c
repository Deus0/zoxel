entity dbg_ui_tests;

void zox_tst_spawn_tilemap2(ecs* world, ClickEventData data) {
    zox_tst_spawn_tilemap(world);
}


void zox_tst_toggle_red_sky(ecs* world, ClickEventData data) {
    override_sky = !override_sky;
    zox_log("Set override_sky to [%i]", override_sky);
    refresh_weather(world);
}

void zox_tst_toggle_low_fps(ecs* world, ClickEventData data) {
    if (!target_fps) {
        target_fps = 30;
    }  else if (target_fps == 30) {
        target_fps = 6;
    } else {
        target_fps = 0;
    }
    // NOTE: Had to use real_world
    ecs* real_world = (ecs*) ecs_get_world(world);
    ecs_set_target_fps(real_world, target_fps);
    zox_log("Set Target FPS to [%i]", target_fps);
}

void zox_dbg_ui_tests(ecs* world, int32_t keycode) {
    byte zox_tsts_count = 19;
    if (keycode != zox_key_h) {
        return;
    }
    zox_log("Toggling Debug UI [Tests]: %s", dbg_ui_tests ? zox_get_name(dbg_ui_tests) : "None");
    if (zox_valid(dbg_ui_tests)) {
        zox_delete(dbg_ui_tests);
        dbg_ui_tests = 0;
        return;
    }
    entity player = dbg_player;
    zox_geter_value(player, CanvasLink, entity, canvas);
    if (!zox_valid(canvas)) {
        return;
    }
    zox_log("   + spawning [dbg_ui_tests] on player %s on canvas %s", zox_get_name(player), zox_get_name(canvas));
    // # List #
    int elements_count = 0;
    byte visible_count = 6;
    SpawnListElement elements[zox_tsts_count];
    byte alignment = zox_alignment_centre;
    byte can_close = 1;
    byte header_font_size = 6 * ui_scale;
    byte list_font_size = 4 * ui_scale;
    byte2 list_padding = byte2_single(2 * ui_scale);
    // UI
    elements[elements_count++] = (SpawnListElement) {
        .text = "Low FPS",
        .on_click = { &zox_tst_toggle_low_fps },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Red Sky",
        .on_click = { &zox_tst_toggle_red_sky },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Canvas",
        .on_click = { &zox_dbg_spawn_canvas },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Element2",
        .on_click = { &zox_dbg_spawn_element },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Text",
        .on_click = { &zox_tst_spawn_text2 },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Window List",
        .on_click = { &zox_tst_spawn_window_list },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Dialogue",
        .on_click = { &zox_tst_spawn_dialogue },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Render Texture",
        .on_click = { &zox_tst_render_texture },
    };
    // 3D
    elements[elements_count++] = (SpawnListElement) {
        .text = "Particles3",
        .on_click = { &zox_dbg_spawn_particle_emitter },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Character Particles3",
        .on_click = { &tst_spawn_character3_particles },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Popup3",
        .on_click = { &zox_dbg_spawn_popup3 },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "NPC",
        .on_click = { &zox_tst_spawn_character3_npc },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Tilemap",
        .on_click = { &zox_tst_spawn_tilemap2 },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Streamer",
        .on_click = { &zox_dbg_spawn_streamer },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Death",
        .on_click = { &zox_tst_player_character_death },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Spawn Chunk3",
        .on_click = { &zox_dbg_spawn_chunk3_button },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Mass Spawn Chunk3",
        .on_click = { &zox_dbg_spawn_chunk3_mass },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Spawn Chunk3 Textured",
        .on_click = { &zox_dbg_spawn_chunk3_textured },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Frame Drop",
        .on_click = { &zox_dbg_test_big_frame },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "RefreshTerrain",
        .on_click = { &zox_dbg_terrain_refresh },
    };
    /*elements[elements_count++] = (SpawnListElement) {
        .text = "Refresh Sunlights",
        .on_click = { &zox_dbg_lights3_refresh_sunlight },
    };*/
    /*elements[elements_count++] = (SpawnListElement) {
        .text = "Rebuild Terrain Mesh Colors",
        .on_click = { &zox_dbg_terrain_refresh_mesh_colors },
    };*/
    // Test our uis
    entity spawned[elements_count];
    entity3 e3 = spawn_window_list(world, prefab_window, player, "Tests", header_font_size, list_font_size, (ClickEvent) { NULL }, can_close, 0, 0, alignment, list_padding, spawned, elements, elements_count, visible_count);
    zox_set_unique_name(e3.x, "dbg_ui_tests");
    zox_add_tag(e3.x, NavigationWindow);
    dbg_ui_tests = e3.x;
}
