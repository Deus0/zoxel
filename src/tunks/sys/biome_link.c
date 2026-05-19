// This keeps track of what biome our camera is in
// We can just use the average biome value of the entire Tunk
// TODO: Store BiomeLink -> Biome on Tunk after we generate the map
extern void set_skybox_colors(ecs*, color_rgb, color_rgb);
// A Streamer will Link to a Biome based on the Tunk's BiomeLink
zox_sys2(BiomeLinkSystem) {
    // const uint seed = global_seed;
    // double height_frequency = terrain_frequency;
    zox_sys_world();
    zox_sys_begin();
    // zox_sys_in(StreamDirty2);
    zox_sys_in(StreamPoint2);
    zox_sys_in(StreamLink);
    zox_sys_out(TunkLink);
    zox_sys_out(BiomeLink);
    for (int i = 0; i < it->count; i++) {
        // zox_sys_i(StreamDirty2, state);
        zox_sys_i(StreamPoint2, position);
        zox_sys_i(StreamLink, terrain);
        zox_sys_o(TunkLink, tlink);
        zox_sys_o(BiomeLink, blink);
        /*if (state->value != zox_dirty_active) {
            continue;
        }*/
        if (!zox_valid(terrain->value) || !zox_has(terrain->value, Chunk2Links)) {
            continue;
        }
        zox_geter(terrain->value, Chunk2Links, chunks);
        entity tunk = int2_hashmap_get(chunks->value, position->value);
        if (!zox_valid(tunk) || !zox_has(tunk, BiomeLink)) {
            continue;
        }
        if (tunk != tlink->value) {
            zox_geter_value(tunk, BiomeLink, entity, biome);
            if (zox_valid(biome)) {
                tlink->value = tunk;
                blink->value = biome;
                zox_logv("Inside new Biome [%s]", blink->value ? zox_get_name(blink->value) : "None");
                // Set Sky Colors
                zox_geter(biome, Colors, bcolors);
                color_rgb sky_color = color_to_color_rgb(bcolors->value[0]);
                game_sky_color = sky_color;
                game_sky_bottom_color = sky_color;
                // viewport_clear_color = sky_color;
                set_camera_fog_color(world, game_sky_color);
                set_skybox_colors(world, game_sky_color, game_sky_bottom_color);
            }

        }
    }
} zox_sys_end(BiomeLinkSystem);
