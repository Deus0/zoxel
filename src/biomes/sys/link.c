// This keeps track of what biome our camera is in
// We can just use the average biome value of the entire Tunk
// TODO: Store BiomeLink -> Biome on Tunk after we generate the map
// extern void set_skybox_colors(ecs*, entity, color_rgb, color_rgb);
// A Streamer will Link to a Biome based on the Tunk's BiomeLink
// NOTE: Links Biome to Chunk using Tunk
zox_sys2(BiomeLinkSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(StreamDirty);
    zox_sys_in(StreamPosition);
    zox_sys_in(StreamLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(StreamDirty, dirty);
        zox_sys_i(StreamPosition, position);
        zox_sys_i(StreamLink, terrain);
        if (dirty->value != zox_dirty_active) {
            // NOTE: Keep checking not linked yet
            if (zox_get_link(world, e, TunkLink)) {
                continue;
            }
        }
        if (!zox_valid(terrain->value)) {
            continue;
        }
#ifdef zox_safety_checks
        if (!zox_has(terrain->value, TunkLinks)) {
            zox_loge("Terrain [%s] has no TunkLinks",
                zox_getn(terrain->value));
            continue;
        }
#endif
        int2 stream_position2 = (int2) {
            position->value.x,
            position->value.z
        };
        zox_geter(terrain->value, TunkLinks, tunks);
        entity tunk = int2_hashmap_get(
            tunks->value,
            stream_position2);
        if (!zox_valid(tunk)) {
            if (dbg_log) {
                zox_loge("[BiomeLinkSystem] Tunk Invalid [%s]: %ix%i",
                    zox_sys_e_name,
                    stream_position2.x,
                    stream_position2.y);
            }
            continue;
        }
        entity old_tunk = zox_get_link(world, e, TunkLink);
        if (tunk == old_tunk) {
            zox_log("[BiomeLinkSystem] Same Tunk [%s]",
                zox_sys_e_name,
                stream_position2.x,
                stream_position2.y);
            continue;
        }
        entity biome = zox_get_link(world, tunk, BiomeLink);
        if (!zox_valid(biome)) {
            zox_loge("[BiomeLinkSystem] Tunk's Biome Invalid [%s]",
                zox_sys_e_name,
                stream_position2.x,
                stream_position2.y);
            continue;
        }
#ifdef zox_safety_checks
        if (!zox_has(biome, BiomeSkyColor)) {
            zox_loge("Biome [%s] has no [BiomeSkyColor]",
                zox_getn(biome));
            continue;
        }
#endif
        if (zox_valid(old_tunk)) {
            zox_unlink(world, e, TunkLink, old_tunk);
        }
        zox_link(world, e, TunkLink, tunk);
        entity old_biome = zox_get_link(world, e, BiomeLink);
        if (old_biome != biome) {
            if (zox_valid(old_biome)) {
                zox_unlink(world, e, BiomeLink, old_biome);
            }
            if (zox_valid(biome)) {
                zox_link(world, e, BiomeLink, biome);
            }
        }
        if (dbg_log) {
            zox_log("Inside new Biome [%s]",
                zox_getn(biome));
        }
        // Set Sky Colors
        color_rgb sky_color = zox_getv(biome, BiomeSkyColor);
        game_sky_color = sky_color;
        game_sky_bottom_color = sky_color;
        if (dbg_log) {
            zox_log("Sky Color Set [%ix%ix%i]",
                sky_color.r,
                sky_color.g,
                sky_color.b);
        }
        // Set cameras Fog
        color_rgb fog_color = sky_color;
        if (zox_has(e, FogColor)) {
            zox_setv(e, FogColor, fog_color);
            if (dbg_log) {
                zox_log("   + Camera Fog [%s]",
                    zox_getn(e));
            }
        } else {
            entity camera = zox_get_link(world, e, CameraLink);
            zox_setv(camera, FogColor, fog_color);
            if (dbg_log) {
                zox_log("   + Camera Fog [%s]",
                    zox_getn(camera));
            }
        }
        entity realm = zox_get_parent_by_id(
            world,
            terrain->value,
            zox_id(Realm));
        if (!zox_valid(realm)) {
            zox_loge("Invalid [realm] parent on terrain %s",
                zox_getn(terrain->value));
            continue;
        }
        entity game = zox_get_link(world, realm, GameLink);
        if (!zox_valid(game)) {
            zox_loge("No GameLink on realm %s",
                zox_getn(realm));
            continue;
        }
        entity skybox = zox_get_link(world, game, Skybox);
        if (!zox_valid(skybox)) {
            zox_loge("Game has no skybox [%s]", zox_getn(game));
            continue;
        }
        set_skybox_colors(
            world,
            skybox,
            game_sky_color,
            game_sky_bottom_color);
    }
} zox_sys_end(BiomeLinkSystem);
