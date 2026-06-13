entity dbg_ui_overlays;

// NOTE: General overview of games data
uint zox_dbg_ui_statistics(ecs *world, entity e, char *buffer, uint size, uint index) {
    index += snprintf(buffer + index, size - index, "Statistics\n");
    index += snprintf(buffer + index, size - index, " - Systems [%i]\n", zox_count_ids(EcsSystem));
    index += snprintf(buffer + index, size - index, " - Components [%i]\n", zox_count_ids(zox_id(EcsComponent)));
    index += snprintf(buffer + index, size - index, "Realm\n");
    index += snprintf(buffer + index, size - index, " - Blocks [%i]\n", zox_count_ids(Block));
    index += snprintf(buffer + index, size - index, " - Stats [%i]\n", zox_count_ids(RealmStat));
    index += snprintf(buffer + index, size - index, " - Items [%i]\n", zox_count_ids(Item));
    index += snprintf(buffer + index, size - index, " - Skills [%i]\n", zox_count_ids(Skill));
    index += snprintf(buffer + index, size - index, " - Dialogues [%i]\n", zox_count_ids(Dialogue));
    index += snprintf(buffer + index, size - index, " - Quests [%i]\n", zox_count_ids(Quest));
    index += snprintf(buffer + index, size - index, " - Characters [%i]\n", zox_count_ids(RealmCharacter));
    index += snprintf(buffer + index, size - index, "World\n");
    index += snprintf(buffer + index, size - index, " - Regions [%i]\n", zox_count_ids(Region));
    index += snprintf(buffer + index, size - index, " - Tunks [%i]\n", zox_count_ids(Tunk));
    index += snprintf(buffer + index, size - index, " - Terrain Chunks [%i]\n", zox_count_ids(TerrainChunk));
    index += snprintf(buffer + index, size - index, " - Colored Chunks [%i]\n", zox_count_ids(ColorChunk));
    index += snprintf(buffer + index, size - index, " - Total Stats [%i]\n", zox_count_ids(Stat));
    index += snprintf(buffer + index, size - index, "Characters\n");
    index += snprintf(buffer + index, size - index, " - Player Characters [%i]\n", zox_count_ids(PlayerCharacter));
    index += snprintf(buffer + index, size - index, " - Npcs [%i]\n", zox_count_ids(Npc));
    index += snprintf(buffer + index, size - index, "Primitives\n");
    index += snprintf(buffer + index, size - index, " - Lines [%i]\n", zox_count_ids(Cube));
    index += snprintf(buffer + index, size - index, " - Cubes [%i]\n", zox_count_ids(Line));
    index += snprintf(buffer + index, size - index, " - Particles [%i]\n", zox_count_ids(Particle));
    index += snprintf(buffer + index, size - index, "UIs\n");
    index += snprintf(buffer + index, size - index, " - Total [%i]\n", zox_count_ids(Element));
    return index;
}

uint debug_ui_seeds(ecs *world, entity e, char *buffer, uint size, uint index) {
    if (!e) {
        index += snprintf(buffer + index, size - index, "Invalid Player\n");
        return index;
    }
    index += snprintf(buffer + index, size - index, "Seeds\n");
    index += snprintf(buffer + index, size - index, " - Player [%s]\n", zox_get_name(e));
    entity game = zox_getv(e, GameLink);
    entity realm = zox_getv(game, RealmLink);
    if (zox_valid(realm)) {
        lint realm_seed = zox_getv(realm, Seed);
        index += snprintf(buffer + index, size - index, " - Realm [%s] [%lu]\n", zox_get_name(realm), realm_seed);
    }
    entity character = zox_getv(e, CharacterLink);
    if (zox_valid(character)) {
        lint character_seed = zox_getv(character, Seed);
        const char* character_name = zox_has(character, ZoxName) ? zox_getv(character, ZoxName) : "None";
        index += snprintf(buffer + index, size - index, " - Character [%s]-[%s]: [%lu]\n", zox_get_name(character), character_name, character_seed);
    }
    if (zox_valid(realm)) {
        entity terrain = zox_getv(realm, TerrainLink);
        if (zox_valid(terrain)) {
            lint terrain_seed = zox_getv(terrain, Seed);
            index += snprintf(buffer + index, size - index, " - Terrain [%s] [%lu]\n", zox_get_name(terrain), terrain_seed);
            entity regions[zox_children_capacity];
            uint regions_length = zox_get_children_by_id(world, terrain, regions, zox_children_capacity, zox_id(Region));
            index += snprintf(buffer + index, size - index, " - Total Regions [%i]\n", regions_length);
            for (uint i = 0; i < regions_length; i++) {
                entity region = regions[i];
                lint region_seed = zox_getv(region, Seed);
                index += snprintf(buffer + index, size - index, "  -- Region [%s] [%lu]\n", zox_get_name(region), region_seed);
            }
        }
    }
    return index;
}

void refresh_debug_label(ecs* world) {
    entity canvas = zox_getv(dbg_player, CanvasLink);
    entity label = zox_get_child_by_id(world, canvas, zox_id(GameDebugLabel));
    if (label) {
        zox_delete(label);
    }
    spawn_game_debug_label(world, canvas);
    // canvas_toggle_ui(world, canvas->value, label, spawn_game_debug_label);
}

void zox_dbg_activate_ui_statistics(ecs* world, ClickEventData data) {
    set_prefab_debug_label(world, &zox_dbg_ui_statistics);
    refresh_debug_label(world);
}

void zox_dbg_activate_ui_filepaths(ecs* world, ClickEventData data) {
    set_prefab_debug_label(world, &zox_dbg_ui_filepaths);
    refresh_debug_label(world);
}

void zox_dbg_activate_seed_ui(ecs* world, ClickEventData data) {
    set_prefab_debug_label(world, &debug_ui_seeds);
    refresh_debug_label(world);
}

void zox_dbg_activate_player_state_ui(ecs* world, ClickEventData data) {
    set_prefab_debug_label(world, &debug_ui_player);
    refresh_debug_label(world);
}

void zox_dbg_activate_ui_raycasting(ecs* world, ClickEventData data) {
    set_prefab_debug_label(world, &debug_ui_raycasting);
    refresh_debug_label(world);
}

void zox_dbg_activate_ui_chunk_link(ecs* world, ClickEventData data) {
    set_prefab_debug_label(world, &debug_label_chunk_link);
    refresh_debug_label(world);
}

void zox_dbg_activate_ui_system_times(ecs* world, ClickEventData data) {
    set_prefab_debug_label(world, &debug_ui_system_times);
    refresh_debug_label(world);
}

void zox_dbg_activate_ui_canvas(ecs* world, ClickEventData data) {
    set_prefab_debug_label(world, &zox_dbg_ui_canvas);
    refresh_debug_label(world);
}

void zox_dbg_activate_ui_player_character(ecs* world, ClickEventData data) {
    set_prefab_debug_label(world, &zox_dbg_ui_player_character);
    refresh_debug_label(world);
}

void zox_dbg_ui_overlays(ecs* world, int32_t keycode) {
    byte zox_tsts_count = 9;
    if (keycode != zox_key_v) {
        return;
    }
    zox_log("Toggling Debug UI [Overlays]: %s", dbg_ui_overlays ? zox_get_name(dbg_ui_overlays) : "None");
    if (zox_valid(dbg_ui_overlays)) {
        zox_delete(dbg_ui_overlays);
        dbg_ui_overlays = 0;
        return;
    }
    entity player = dbg_player;
    zox_geter_value(player, CanvasLink, entity, canvas);
    if (!zox_valid(canvas)) {
        return;
    }
    zox_log("   + spawning [zox_dbg_test_window] on player %s on canvas %s", zox_get_name(player), zox_get_name(canvas));
    // # List #
    int elements_count = 0;
    byte visible_count = 6;
    SpawnListElement elements[zox_tsts_count];
    byte alignment = zox_alignment_centre;
    byte can_close = 1;
    byte header_font_size = 6 * ui_scale;
    byte list_font_size = 4 * ui_scale;
    byte2 list_padding = byte2_single(2 * ui_scale);
    // UIs
    elements[elements_count++] = (SpawnListElement) {
        .text = "Statistics",
        .on_click = { &zox_dbg_activate_ui_statistics },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "System Times",
        .on_click = { &zox_dbg_activate_ui_system_times },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "File Paths",
        .on_click = { &zox_dbg_activate_ui_filepaths },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Seeds",
        .on_click = { &zox_dbg_activate_seed_ui },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Player Character",
        .on_click = { &zox_dbg_activate_ui_player_character },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Canvas",
        .on_click = { &zox_dbg_activate_ui_canvas },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Player States",
        .on_click = { &zox_dbg_activate_player_state_ui },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Raycaster",
        .on_click = { &zox_dbg_activate_ui_raycasting },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Chunk Link",
        .on_click = { &zox_dbg_activate_ui_chunk_link },
    };
    // Test our uis
    entity spawned[elements_count];
    // int2 position = (int2) { -16, -16 };
    // float2 anchor = { 1.0f, 1.0f };
    entity3 e3 = spawn_window_list(world, prefab_window, player, "Overlays", header_font_size, list_font_size, (ClickEvent) { NULL }, can_close, 0, 0, alignment, list_padding, spawned, elements, elements_count, visible_count);
    zox_set_unique_name(e3.x, "dbg_ui_overlays");
    zox_add_tag(e3.x, NavigationWindow);
    dbg_ui_overlays = e3.x;
}
