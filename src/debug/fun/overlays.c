entity dbg_ui_overlays;

// TODO: Include post processing, streaming and any other data
uint zox_dbg_ui_camera(
    ecs *world,
    entity e,
    char *buffer,
    uint size,
    uint index)
{
    if (!zox_valid(e)) {
        return index;
    }
    index += snprintf(buffer + index, size - index, "Player Cameras [%s]  \n", zox_getn(e));
    if (!zox_valid(e)) {
        return index;
    }
    entity camera = zox_get_link(world, e, Camera);
    index += snprintf(buffer + index, size - index, "   - Camera [%s]\n", zox_getn(camera));
    if (!zox_valid(camera)) {
        return index;
    }
    float3 camera_position = zox_getv(camera, Position3D);
    float4 camera_rotation = zox_getv(camera, Rotation3D);
    float3 camera_euler = quaternion_to_euler(camera_rotation);
    index += snprintf(buffer + index, size - index, " - Position [%fx%fx%f]\n", camera_position.x, camera_position.y, camera_position.z);
    index += snprintf(buffer + index, size - index, " - Rotation [%fx%fx%fx%f]\n", camera_rotation.x, camera_rotation.y, camera_rotation.z, camera_rotation.w);
    index += snprintf(buffer + index, size - index, " - Euler [%fx%fx%f]\n", camera_euler.x, camera_euler.y, camera_euler.z);
    entity skybox = zox_get_child_by_id(world, camera, zox_id(Skybox));

    index += snprintf(buffer + index, size - index, "   -- Skybox [%s]\n", zox_getn(skybox));
    /*if (zox_valid(skybox)) {
        float3 skybox_position = zox_getv(skybox, Position3D);
        float4 skybox_rotation = zox_getv(skybox, Rotation3D);
        index += snprintf(buffer + index, size - index, " - Position [%fx%fx%f]\n", skybox_position.x, skybox_position.y, skybox_position.z);
        index += snprintf(buffer + index, size - index, " - Rotation [%fx%fx%fx%f]\n", skybox_rotation.x, skybox_rotation.y, skybox_rotation.z, skybox_rotation.w);
        float3 skybox_lposition = zox_getv(skybox, LocalPosition3D);
        index += snprintf(buffer + index, size - index, " - Local Position [%fx%fx%f]\n", skybox_lposition.x, skybox_lposition.y, skybox_lposition.z);
    }*/
    if (!zox_has(camera, Streamer)) {
        index += snprintf(buffer + index, size - index, " - No Streaming\n");
        return index;
    }
    byte streamer_level = zox_getv(camera, StreamerLevel);
    entity stream_terrain = zox_getv(camera, StreamLink);
    int3 stream_position = zox_getv(camera, StreamPosition);
    int2 stream_position2 = zox_getv(camera, StreamPosition2);
    index += snprintf(buffer + index, size - index, " - Streamer Level [%i]\n", streamer_level);
    index += snprintf(buffer + index, size - index, " - Terrain [%s]\n", zox_get_name(stream_terrain));
    index += snprintf(buffer + index, size - index, " - Position [%ix%ix%i]\n", stream_position.x, stream_position.y, stream_position.z);
    index += snprintf(buffer + index, size - index, " - Position2 [%ix%i]\n", stream_position2.x, stream_position2.y);
    return index;
}

// TODO: Add Chunk UIs based on closest ones to camera
uint zox_dbg_ui_chunk_busy(ecs* world, entity e, char *buffer, uint size, uint index) {
    byte busy = zox_getv(e, Busy);
    byte render_distance = zox_getv(e, RenderDistance);
    byte render_depth = zox_getv(e, RenderDepth);
    index += snprintf(buffer + index, size - index, "- [%s]\n   -[%s] Dist [%i] Depth [%i]\n", zox_get_name(e), busy ? "Busy" : "Done", render_distance, render_depth);
    return index;
}

uint zox_dbg_ui_terrain(ecs* world, entity e, char *buffer, uint size, uint index) {
    if (!zox_valid(e)) {
        return index;
    }
    entity game = zox_get_parent(world, e);
    if (!zox_valid(game) || !zox_has(game, RealmLink)) {
        index += snprintf(buffer + index, size - index, "Player [%s]'s Invalid Game\n", zox_get_name(e));
        return index;
    }
    entity realm = zox_getv(game, RealmLink);
    if (!zox_valid(realm)) {
        index += snprintf(buffer + index, size - index, "Player [%s] has no Realm\n", zox_get_name(e));
        return index;
    }
    entity terrain = zox_get_child_by_id(world, realm, zox_id(Terrain));
    index += snprintf(buffer + index, size - index, "Player [%s] Terrain [%s]\n", zox_get_name(e), zox_get_name(terrain));
    if (!zox_valid(terrain)) {
        return index;
    }
    uint children_count = zox_get_children_count(world, terrain);
    uint chunks_count = zox_get_children_count_by_id(world, terrain, zox_id(Chunk3));
    byte loaded = !zox_has(terrain, Loading);
    byte depth = zox_getv(terrain, NodeDepth);
    float block_scale = zox_getv(terrain, BlockScale);
    index += snprintf(buffer + index, size - index, " - Children [%i of %i]\n", chunks_count, children_count);
    index += snprintf(buffer + index, size - index, " - Loaded [%i]\n", loaded);
    index += snprintf(buffer + index, size - index, " - Depth [%i]\n", depth);
    index += snprintf(buffer + index, size - index, " - Block Scale [%f]\n", block_scale);
    index += snprintf(buffer + index, size - index, " - Lod Distances [%i:%i]\n", terrain_lod_near, terrain_lod_far);
    entity chunks[8];
    uint length = zox_get_children_by_id(world, terrain, chunks, 8, zox_id(Chunk3));
    for (int i = 0; i < length; i++) {
        entity e2 = chunks[i];
        index = zox_dbg_ui_chunk_busy(world, e2, buffer, size, index);
    }
    return index;
}

uint zox_dbg_ui_meshes(ecs* world, entity e, char* buffer, uint size, uint index) {
    if (!zox_valid(e)) {
        return index;
    }
    entity game = zox_get_parent(world, e);
    if (!zox_valid(game) || !zox_has(game, RealmLink)) {
        index += snprintf(buffer + index, size - index, "Player [%s]'s Invalid Game\n", zox_get_name(e));
        return index;
    }
    entity realm = zox_getv(game, RealmLink);
    if (!zox_valid(realm)) {
        index += snprintf(buffer + index, size - index, "Player [%s] has no Realm\n", zox_get_name(e));
        return index;
    }
    entity terrain = zox_get_child_by_id(world, realm, zox_id(Terrain));
    if (!zox_valid(terrain)) {
        return index;
    }
    index += snprintf(buffer + index, size - index, "Debug Meshes\n");
    index += snprintf(buffer + index, size - index, "Terrain Chunks [%i]\n", zox_count_ids(TerrainChunk));
    index += snprintf(buffer + index, size - index, "Terrain Chunk Meshes [%i]\n", zox_count_ids(ChunkMesh));
    // entity chunks[8];
    //uint length = zox_get_children_by_id(world, terrain, chunks, 8, zox_id(Chunk3));
    byte display_meshes = 0;
    iter it2 = zox_children(world, terrain);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count && display_meshes < 10; j++) {
            entity e2 = it2.entities[j];
            if (!zox_has(e2, Chunk3)) {
                continue;
            }
            entity meshes[8];
            uint meshes_length = zox_get_children_by_id(world, e2, meshes, 8, zox_id(ChunkMesh));
            if (!meshes_length) {
                continue;
            }
            index += snprintf(buffer + index, size - index, "Chunk %s\n", zox_getn(e2));
            for (int k = 0; k < meshes_length; k++) {
                entity e3 = meshes[k];
                byte depth = zox_getv(e3, RenderDepth);
                byte disabled = zox_is_disabled(e3);
                index += snprintf(buffer + index, size - index, " - Mesh [%s]D [%i] Disabled [%i]\n", zox_getn(e3), depth, disabled);
            }
            display_meshes++;
        }
    }
    return index;
}

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
    index += snprintf(buffer + index, size - index, " - Terrain Chunk Meshes [%i]\n", zox_count_ids(ChunkMesh));
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
    index += snprintf(buffer + index, size - index, "Other\n");
    index += snprintf(buffer + index, size - index, " - Bone [%i]\n", zox_count_ids(Bone));
    // index += snprintf(buffer + index, size - index, "Testing\n");
    // index += snprintf(buffer + index, size - index, " - DeactivateDelay [%i]\n", zox_count_ids(zox_id(DeactivateDelay)));
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
    entity character = zox_get_link(world, e, Character);
    if (zox_valid(character)) {
        lint character_seed = zox_getv(character, Seed);
        const char* character_name = zox_has(character, ZoxName) ? zox_getv(character, ZoxName) : "None";
        index += snprintf(buffer + index, size - index, " - Character [%s]-[%s]: [%lu]\n", zox_get_name(character), character_name, character_seed);
    }
    if (zox_valid(realm)) {
        entity terrain = zox_get_link(world, realm, Terrain);
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
    entity player = dbg_player;
    entity canvas = zox_get_link(world, player, Canvas);
    entity label = zox_get_child_by_id(world, canvas, zox_id(GameDebugLabel));
    if (label) {
        zox_delete(label);
    }
    spawn_game_debug_label(world, canvas);
}

void zox_dbg_map_cycle_ui(ecs* world, ClickEventData data) {
    zox_dbg_map_cycle();
}

void zox_dbg_activate_ui_statistics(ecs* world, ClickEventData data) {
    set_prefab_debug_label(world, &zox_dbg_ui_statistics);
    refresh_debug_label(world);
}

void zox_dbg_activate_ui_meshes(ecs* world, ClickEventData data) {
    set_prefab_debug_label(world, &zox_dbg_ui_meshes);
    refresh_debug_label(world);
}

void zox_dbg_activate_ui_cameras(ecs* world, ClickEventData data) {
    set_prefab_debug_label(world, &zox_dbg_ui_camera);
    refresh_debug_label(world);
}

void zox_dbg_activate_ui_terrains(ecs* world, ClickEventData data) {
    set_prefab_debug_label(world, &zox_dbg_ui_terrain);
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

void zox_dbg_activate_ui_raycast_lights(ecs* world, ClickEventData data) {
    set_prefab_debug_label(world, &debug_ui_raycasted_light);
    refresh_debug_label(world);
}

void zox_dbg_activate_ui_chunk_link(ecs* world, ClickEventData data) {
    set_prefab_debug_label(world, &zox_dbg_label_character_links);
    refresh_debug_label(world);
}

void zox_dbg_activate_ui_towns(ecs* world, ClickEventData data) {
    set_prefab_debug_label(world, &zox_dbg_label_towns);
    refresh_debug_label(world);
}

void zox_dbg_activate_ui_inside_chunk(ecs* world, ClickEventData data) {
    set_prefab_debug_label(world, &zox_dbg_label_inside_chunk);
    refresh_debug_label(world);
}

void zox_dbg_activate_ui_system_times(ecs* world, ClickEventData data) {
    system_debug_start++;
    if (system_debug_start >= 5) system_debug_start = 0;
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
