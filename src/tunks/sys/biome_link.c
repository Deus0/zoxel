// This keeps track of what biome our camera is in
// We can just use the average biome value of the entire Tunk
// TODO: Store BiomeLink -> Biome on Tunk after we generate the map
// extern void set_skybox_colors(ecs*, entity, color_rgb, color_rgb);
// A Streamer will Link to a Biome based on the Tunk's BiomeLink
// NOTE: Links Biome to Chunk using Tunk
zox_sys2(BiomeLinkSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(StreamDirty2);
    zox_sys_in(StreamPosition2);
    zox_sys_in(StreamLink);
    zox_sys_out(TunkLink);
    zox_sys_out(BiomeLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(StreamDirty2, dirty);
        zox_sys_i(StreamPosition2, position);
        zox_sys_i(StreamLink, terrain);
        zox_sys_o(TunkLink, tunk_link);
        zox_sys_o(BiomeLink, blink);
        if (dirty->value != zox_dirty_active && zox_valid(tunk_link->value)) {
            continue;
        }
        if (!zox_valid(terrain->value)) {
            continue;
        }
#ifdef zox_safety_checks
        if (!zox_has(terrain->value, TunkLinks)) {
            zox_loge("Terrain [%s] has no TunkLinks", zox_getn(terrain->value));
            continue;
        }
#endif
        zox_geter(terrain->value, TunkLinks, tunks);
        entity tunk = int2_hashmap_get(tunks->value, position->value);
        if (!zox_valid(tunk)) {
            continue;
        }
#ifdef zox_safety_checks
        if (!zox_has(tunk, BiomeLink)) {
            zox_loge("Tunk [%s] has no [BiomeLink]", zox_getn(tunk));
            continue;
        }
#endif
        if (tunk != tunk_link->value) {
            entity biome = zox_getv(tunk, BiomeLink);
            if (!zox_valid(biome)) {
                continue;
            }
#ifdef zox_safety_checks
            if (!zox_has(biome, BiomeSkyColor)) {
                zox_loge("Biome [%s] has no [BiomeSkyColor]", zox_getn(biome));
                continue;
            }
#endif
            tunk_link->value = tunk;
            blink->value = biome;
            zox_logv("Inside new Biome [%s]", blink->value ? zox_get_name(blink->value) : "None");
            // Set Sky Colors
            color_rgb sky_color = zox_getv(biome, BiomeSkyColor);
            game_sky_color = sky_color;
            game_sky_bottom_color = sky_color;
            // Set cameras Fog
            if (zox_has(e, FogColor)) {
                zox_set(e, FogColor, { sky_color });
            } else if (zox_has(e, CameraLink)) {
                entity camera = zox_getv(e, CameraLink);
                zox_set(camera, FogColor, { sky_color });
            }
            entity game = zox_get_parent_by_id(world, terrain->value, zox_id(Game));
            if (!zox_valid(game)) {
                zox_loge("No Game parent on Camera %s", zox_getn(terrain->value));
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
    }
} zox_sys_end(BiomeLinkSystem);
