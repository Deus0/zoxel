// tst: settings_apply, a setting changed reaches its module and is saved: each module's settings observer (an Update
//  on a Setting) sets its global, the save writes the file once
//  - one setting from each module that had a settings dirty system: rendering (Fog), sounds (Music), ui (UI Scale),
//    models (Outlines), streaming (No Frustums), npcs (Npc Range), vodes (No Vodes)
//  - in npcs/tst: the lowest module that sees every one of them
//  - run: bash bsh/tst.sh settings_apply
int zox_tst_settings_stage = 0;
int zox_tst_settings_frames = 0;
byte zox_tst_settings_found = 0;
float zox_tst_settings_old_fog = 0;
byte zox_tst_settings_old_outlines = 0;
byte zox_tst_settings_old_frustums = 0;
byte zox_tst_settings_old_vodes = 0;
byte zox_tst_settings_range = 0;

static inline byte zox_tst_settings_applied() {
    return fabsf(fog_density - 0.5f * fog_settings_mul) < 0.0001f && fabsf(volume_music - 0.5f) < 0.0001f &&
        fabsf(ui_scale - 1.5f) < 0.0001f && zox_block_outlines == !zox_tst_settings_old_outlines &&
        disable_frustum_culling == !zox_tst_settings_old_frustums && character_spawn_distance == zox_tst_settings_range &&
        zox_disable_vodes == !zox_tst_settings_old_vodes;
}

byte zox_tst_settings_apply_ready(ecs* world) {
    zox_tst_settings_frames++;
    zox_tst_progress(zox_tst_settings_stage * 1000 + zox_tst_settings_frames);
    if (zox_tst_settings_stage == 0) {
        zox_tst_waiting = "the app's settings";
        if (!zox_valid(main_app) || !set_app_setting_float(world, main_app, "Music", volume_music)) {
            return zox_tst_settings_frames > 600;
        }
        zox_tst_settings_old_fog = fog_density;
        zox_tst_settings_old_outlines = zox_block_outlines;
        zox_tst_settings_old_frustums = disable_frustum_culling;
        zox_tst_settings_old_vodes = zox_disable_vodes;
        zox_tst_settings_range = character_spawn_distance == character_spawn_distance_limits.x ?
            character_spawn_distance_limits.x + 1 : character_spawn_distance_limits.x;
        zox_tst_settings_found =
            set_app_setting_float(world, main_app, "Fog", 0.5f) +
            set_app_setting_float(world, main_app, "Music", 0.5f) +
            set_app_setting_float(world, main_app, "UI Scale", 1.5f) +
            set_app_setting_byte(world, main_app, "Outlines", !zox_tst_settings_old_outlines) +
            set_app_setting_byte(world, main_app, "No Frustums", !zox_tst_settings_old_frustums) +
            set_app_setting_byte(world, main_app, "Npc Range", zox_tst_settings_range) +
            set_app_setting_byte(world, main_app, "No Vodes", !zox_tst_settings_old_vodes);
        zox_tst_settings_stage = 1;
        zox_tst_settings_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the settings applied and saved";
    // the save follows the frame after (Update), give it a few frames past the globals
    if (zox_tst_settings_applied()) {
        zox_tst_settings_stage = 2;
    }
    return zox_tst_settings_frames > 240 || (zox_tst_settings_stage == 2 && zox_tst_settings_frames > 10);
}

void zox_tst_settings_apply_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_settings_found == 7, "found [%i] of [7] settings on the app", zox_tst_settings_found);
    zox_tst_check(fabsf(fog_density - 0.5f * fog_settings_mul) < 0.0001f, "Fog didn't set fog_density [%f]", fog_density);
    zox_tst_check(fabsf(volume_music - 0.5f) < 0.0001f, "Music didn't set volume_music [%f]", volume_music);
    zox_tst_check(fabsf(ui_scale - 1.5f) < 0.0001f, "UI Scale didn't set ui_scale [%f]", ui_scale);
    zox_tst_check(zox_block_outlines == !zox_tst_settings_old_outlines, "Outlines didn't toggle [%i]", zox_block_outlines);
    zox_tst_check(disable_frustum_culling == !zox_tst_settings_old_frustums, "No Frustums didn't toggle [%i]",
        disable_frustum_culling);
    zox_tst_check(character_spawn_distance == zox_tst_settings_range, "Npc Range [%i], not [%i]",
        character_spawn_distance, zox_tst_settings_range);
    zox_tst_check(zox_disable_vodes == !zox_tst_settings_old_vodes, "No Vodes didn't toggle [%i]", zox_disable_vodes);
    // saved: the file has the new music volume
    FILE* file = fopen(get_settings_file(settings_game_name), "r");
    byte saved = 0;
    if (file) {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            saved |= !strncmp(line, "Music:float:0.5", 15);
        }
        fclose(file);
    }
    zox_tst_check(saved, "the settings file doesn't have Music at 0.5 [%s]", get_settings_file(settings_game_name));
}

void zox_tsts_settings_apply(ecs* world) {
    (void) world;
    zox_tst_add("settings_apply", zox_tst_settings_apply_ready, zox_tst_settings_apply_check, 30);
    zox_tst_set_setup("settings_apply", NULL, 1);
}
