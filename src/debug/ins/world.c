entity dbg_test_window_world;

void zox_dbg_spawn_cube_ui(ecs* world, ClickEventData data) {
    zox_dbg_spawn_cube(world);
}

void zox_dbg_spawn_cube_textured_ui(ecs* world, ClickEventData data) {
    zox_dbg_spawn_cube_textured(world);
}

void zox_tst_spawn_tilemap2(ecs* world, ClickEventData data) {
    zox_tst_spawn_tilemap(world);
}

void zox_dbg_test_window_world(ecs* world, int32_t keycode) {
    if (keycode != zox_key_j) {
        return;
    }
    zox_log("Debug [dbg_test_window_world]: %s", dbg_test_window_world ? zox_getn(dbg_test_window_world) : "None");
    if (zox_valid(dbg_test_window_world)) {
        zox_delete(dbg_test_window_world);
        dbg_test_window_world = 0;
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
    SpawnListElement elements[zox_max_test_uis];
    byte alignment = zox_alignment_centre;
    byte can_close = 1;
    byte header_font_size = 6 * ui_scale;
    byte list_font_size = 4 * ui_scale;
    byte2 list_padding = byte2_single(2 * ui_scale);
    // Geometry
    elements[elements_count++] = (SpawnListElement) {
        .text = "Cube",
        .on_click = { &zox_dbg_spawn_cube_ui },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Cube Textured",
        .on_click = { &zox_dbg_spawn_cube_textured_ui },
    };
    // Particles
    elements[elements_count++] = (SpawnListElement) {
        .text = "Particles3",
        .on_click = { &zox_dbg_spawn_particle_emitter },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Character Particles3",
        .on_click = { &tst_spawn_character3_particles },
    };
    // Characters
    elements[elements_count++] = (SpawnListElement) {
        .text = "NPC",
        .on_click = { &zox_tst_spawn_character3_npc },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Skeleton NPC",
        .on_click = { &zox_tst_spawn_character3_npc_skeleton },
    };
    // Game
    elements[elements_count++] = (SpawnListElement) {
        .text = "Dialogue",
        .on_click = { &zox_tst_spawn_dialogue },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Death",
        .on_click = { &zox_tst_player_character_death },
    };
    // Chunk Rendering
    elements[elements_count++] = (SpawnListElement) {
        .text = "Tilemap",
        .on_click = { &zox_tst_spawn_tilemap2 },
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
    // Test our uis
    entity spawned[elements_count];
    entity3 e3 = spawn_window_list(world, prefab_window, player, "World Tests", header_font_size, list_font_size, (ClickEvent) { NULL }, can_close, 0, 0, alignment, float2_top_left, list_padding, spawned, elements, elements_count, visible_count);
    zox_set_unique_name(e3.x, "dbg_test_window_world");
    zox_add(e3.x, NavigationWindow);
    dbg_test_window_world = e3.x;
}
