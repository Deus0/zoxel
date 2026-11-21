// tst: settings, the app's settings, in the menus
//  - settings_spawn: every module's settings are on the app, each once (spawned by its InitializeEvent)
//  - settings_drag: one changed every frame (a slider dragged) reaches the game each frame, not only the first
//  - settings_load: a settings file loaded (LoadSettings run) sets the game's values, its bad lines skipped
//  - in npcs/tst: the lowest module that sees every setting's module
//  - run: bash bsh/tst.sh settings_spawn (settings_drag, settings_load)
const char* zox_tst_settings_names[] = {
    "Fog", "Downscale", "Volume", "Music", "SFX", "UI Scale", "Block Res", "Outlines", "Near RD",
    "Far RD", "Vertical RD", "No Lods", "No Frustums", "Npc Spawn", "Npc Range", "No Npcs", "No Humans", "GPU Meshing",
};
#define zox_tst_settings_names_count (int) (sizeof(zox_tst_settings_names) / sizeof(zox_tst_settings_names[0]))
#define zox_tst_settings_drag_frames 8
int zox_tst_settings_counts[zox_tst_settings_names_count];
int zox_tst_settings_unnamed = 0;
int zox_tst_settings_f = 0;
int zox_tst_settings_drag_seen = 0;     // frames whose value the game had, of zox_tst_settings_drag_frames
float zox_tst_settings_drag_last = -1;
byte zox_tst_settings_load_stage = 0;
byte zox_tst_settings_load_outlines = 0;

// the app with its Music setting (found, not set: setting it dirties it)
static inline byte zox_tst_settings_ready_app(ecs* world) {
    zox_tst_waiting = "the app's settings";
    if (!zox_valid(main_app)) {
        return 0;
    }
    iter it = zox_children(world, main_app);
    while (zox_children_next(it)) {
        for (int j = 0; j < it.count; j++) {
            const entity e = it.entities[j];
            if (zox_has(e, Setting) && zox_has(e, ZoxName) && zox_get(e, ZoxName)->value &&
                !strcmp(zox_get(e, ZoxName)->value, "Music")) {
                return 1;
            }
        }
    }
    return 0;
}

// a setting dirty or updating (changed this frame or the last)
static inline byte zox_tst_settings_changing(ecs* world) {
    iter it = zox_children(world, main_app);
    while (zox_children_next(it)) {
        for (int j = 0; j < it.count; j++) {
            if (zox_has(it.entities[j], Setting) && (zox_has(it.entities[j], Dirty) || zox_has(it.entities[j], Update))) {
                return 1;
            }
        }
    }
    return 0;
}

// settings_spawn
byte zox_tst_settings_spawn_ready(ecs* world) {
    zox_tst_progress(++zox_tst_settings_f);
    if (!zox_tst_settings_ready_app(world)) {
        return zox_tst_settings_f > 600;
    }
    iter it = zox_children(world, main_app);
    while (zox_children_next(it)) {
        for (int j = 0; j < it.count; j++) {
            const entity e = it.entities[j];
            if (!zox_has(e, Setting)) {
                continue;
            }
            if (!zox_has(e, ZoxName) || !zox_get(e, ZoxName)->value) {
                zox_tst_settings_unnamed++;
                continue;
            }
            const char* name = zox_get(e, ZoxName)->value;
            for (int n = 0; n < zox_tst_settings_names_count; n++) {
                zox_tst_settings_counts[n] += !strcmp(name, zox_tst_settings_names[n]);
            }
        }
    }
    return 1;
}

void zox_tst_settings_spawn_check(ecs* world) {
    (void) world;
    for (int n = 0; n < zox_tst_settings_names_count; n++) {
        zox_tst_check(zox_tst_settings_counts[n] == 1, "setting [%s] is on the app [%i] times, not once",
            zox_tst_settings_names[n], zox_tst_settings_counts[n]);
    }
    zox_tst_check(!zox_tst_settings_unnamed, "[%i] settings without a name", zox_tst_settings_unnamed);
}

// settings_drag: frame f sets Music to f / 10, the game's volume_music is checked against it a frame on
byte zox_tst_settings_drag_ready(ecs* world) {
    zox_tst_progress(++zox_tst_settings_f);
    if (zox_tst_settings_drag_last < 0) {
        if (!zox_tst_settings_ready_app(world)) {
            return zox_tst_settings_f > 600;
        }
        zox_tst_settings_f = 0;
        zox_tst_settings_drag_last = 0;
        return 0;
    }
    zox_tst_waiting = "a frame's setting reaching the game";
    if (zox_tst_settings_f > 1 && fabsf(volume_music - zox_tst_settings_drag_last) < 0.0001f) {
        zox_tst_settings_drag_seen++;
    }
    if (zox_tst_settings_f > zox_tst_settings_drag_frames) {
        return 1;
    }
    zox_tst_settings_drag_last = zox_tst_settings_f / 10.0f;
    set_app_setting_float(world, main_app, "Music", zox_tst_settings_drag_last);
    return 0;
}

void zox_tst_settings_drag_check(ecs* world) {
    (void) world;
    zox_log("[tst] settings_drag: the game had [%i] of [%i] frames' values", zox_tst_settings_drag_seen,
        zox_tst_settings_drag_frames);
    zox_tst_check(zox_tst_settings_drag_seen == zox_tst_settings_drag_frames,
        "a setting changed every frame reached the game [%i] of [%i] frames (Music [%.2f], set [%.2f])",
        zox_tst_settings_drag_seen, zox_tst_settings_drag_frames, volume_music, zox_tst_settings_drag_last);
}

// settings_load: a file with Music, Fog and Outlines (and bad lines) loaded again
byte zox_tst_settings_load_ready(ecs* world) {
    zox_tst_progress(zox_tst_settings_load_stage * 1000 + ++zox_tst_settings_f);
    if (zox_tst_settings_load_stage == 0) {
        // the boot's load done and saved: a load dirties every setting, the save a frame on rewrites the file
        if (!zox_tst_settings_ready_app(world) || zox_getv(main_app, LoadSettings) != zox_load_settings_end ||
            zox_tst_settings_changing(world)) {
            return zox_tst_settings_f > 600;
        }
        zox_tst_settings_load_outlines = !zox_block_outlines;
        FILE* file = fopen(get_settings_file(settings_game_name), "w");
        if (!file) {
            zox_tst_check(0, "couldn't write [%s]", get_settings_file(settings_game_name));
            return 1;
        }
        fprintf(file, "Music:float:0.7\nnot a setting\nFog:float:0.25\nNo Such Setting:byte:1\nOutlines:byte:%i\n",
            zox_tst_settings_load_outlines);
        fclose(file);
        zox_setv(main_app, LoadSettings, zox_load_settings_run);
        zox_tst_settings_load_stage = 1;
        zox_tst_settings_f = 0;
        return 0;
    }
    zox_tst_waiting = "the loaded settings in the game";
    const byte loaded = fabsf(volume_music - 0.7f) < 0.0001f && fabsf(fog_density - 0.25f * fog_settings_mul) < 0.0001f &&
        zox_block_outlines == zox_tst_settings_load_outlines;
    return loaded || zox_tst_settings_f > 120;
}

void zox_tst_settings_load_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_settings_load_stage == 1, "never loaded");
    zox_tst_check(fabsf(volume_music - 0.7f) < 0.0001f, "Music loaded [%.2f], not [0.7]", volume_music);
    zox_tst_check(fabsf(fog_density - 0.25f * fog_settings_mul) < 0.0001f, "Fog loaded [%f], not [%f]",
        fog_density, 0.25f * fog_settings_mul);
    zox_tst_check(zox_block_outlines == zox_tst_settings_load_outlines, "Outlines loaded [%i], not [%i]",
        zox_block_outlines, zox_tst_settings_load_outlines);
}

void zox_tsts_settings(ecs* world) {
    (void) world;
    zox_tst_add("settings_spawn", zox_tst_settings_spawn_ready, zox_tst_settings_spawn_check, 30);
    zox_tst_set_setup("settings_spawn", NULL, 1);
    zox_tst_add("settings_drag", zox_tst_settings_drag_ready, zox_tst_settings_drag_check, 30);
    zox_tst_set_setup("settings_drag", NULL, 1);
    zox_tst_add("settings_load", zox_tst_settings_load_ready, zox_tst_settings_load_check, 30);
    zox_tst_set_setup("settings_load", NULL, 1);
}
