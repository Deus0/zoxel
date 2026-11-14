// This keeps track of what biome our camera is in
// We can just use the average biome value of the entire Tunk
// TODO: Store BiomeLink -> Biome on Tunk after we generate the map
// extern void set_skybox_colors(ecs*, entity, color_rgb, color_rgb);
// A Streamer will Link to a Biome based on the Tunk's BiomeLink
// NOTE: Links Biome to Chunk using Tunk
void biome_link_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(StreamDirty);
    zox_sys_in(StreamPosition);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(StreamDirty, dirty);
        zox_sys_i(StreamPosition, position);
        entity terrain = zox_get_link(world, e, StreamLink);
        if (dirty->value != zox_dirty_active) {
            // NOTE: Keep checking not linked yet
            if (zox_get_link(world, e, TunkLink)) {
                continue;
            }
        }
        if (!zox_valid(terrain)) {
            continue;
        }
#ifdef zox_safety_checks
        if (!zox_has(terrain, TunkLinks)) {
            zox_loge("Terrain [%s] has no TunkLinks",
                zox_getn(terrain));
            continue;
        }
#endif
        int2 stream_position2 = (int2) {
            position->value.x,
            position->value.z
        };
        const TunkLinks* tunks = zox_get(terrain, TunkLinks);
        entity tunk = int2_hashmap_get(
            tunks->value,
            stream_position2);
        if (!zox_valid(tunk)) {
            if (dbg_log) {
                zox_loge("[biome_link_system] Tunk Invalid [%s]: %ix%i",
                    zox_sys_e_name,
                    stream_position2.x,
                    stream_position2.y);
            }
            continue;
        }
        entity old_tunk = zox_get_link(world, e, TunkLink);
        if (tunk == old_tunk) {
            if (dbg_log) {
                zox_logw("[biome_link_system] Same Tunk [%s]",
                    zox_sys_e_name,
                    stream_position2.x,
                    stream_position2.y);
            }
            continue;
        }
        entity biome = zox_get_link(world, tunk, BiomeLink);
        if (!zox_valid(biome)) {
            zox_loge("[biome_link_system] Tunk's Biome Invalid [%s]",
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
        // the sky and fog blend to the biome's (blend_sky_to, sky_blend_system: it snapped)
        const color_rgb sky_color = zox_getv(biome, BiomeSkyColor);
        entity realm = zox_get_parent_by_id(
            world,
            terrain,
            zox_id(Realm));
        if (!zox_valid(realm)) {
            zox_loge("Invalid [realm] parent on terrain %s",
                zox_getn(terrain));
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
        blend_sky_to(world, skybox, sky_color);
    }
    zox_sys_on_end();
} zoxd_system(biome_link_system);
