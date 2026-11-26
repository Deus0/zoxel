// tst: underwater, the player's camera under water: blue fog, the view tinted deep blue (CameraTint, eased), and
//  back out of it (src/terrains/voxel/sys/water_view.c, res/shaders/render_texture_rgb.frag)
//  - the player's game; water (a source) put round its camera, 5 x 5 x 5
//  - a second later: Underwater, its CameraTint over 0.9, tst/underwater.ppm (it should look deep blue)
//  - the water taken out, a second later: no Underwater, its tint under 0.05
//  - run: bash bsh/tst.sh underwater
byte zox_tst_uwater_stage = 0;
double zox_tst_uwater_time = 0;
int3 zox_tst_uwater_block;
byte zox_tst_uwater_under = 0, zox_tst_uwater_out = 0;
float zox_tst_uwater_tint = -1, zox_tst_uwater_tint_out = -1;

static inline void zox_tst_uwater_fill(ecs* world, entity terrain, byte value) {
    for (int x = -2; x <= 2; x++) for (int y = -2; y <= 2; y++) for (int z = -2; z <= 2; z++) {
        water_flow_set(world, terrain, int3_add(zox_tst_uwater_block, (int3) { x, y, z }), value);
    }
}

byte zox_tst_underwater_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity camera = main_cameras[0];
    const entity terrain = local_terrain;
    if (!zox_valid(camera) || !zox_valid(terrain)) {
        return 0;
    }
    const double since = zox_current_time - zox_tst_uwater_time;
    zox_tst_progress(zox_tst_uwater_stage * 100 + (int) since);
    WaterLevels levels;
    water_levels_get(world, zox_get_parent(world, terrain), &levels);
    if (zox_tst_uwater_stage == 0) {
        zox_tst_uwater_block = real_position_to_block_position(zox_getv(camera, Position3D), zox_getv(terrain, BlockScale));
        zox_tst_uwater_fill(world, terrain, levels.voxel[0][zox_water_level_source]);
    } else if (zox_tst_uwater_stage == 1) {
        zox_tst_waiting = "the camera to go under water";
        if (since < 1.5) {
            return 0;
        }
        zox_tst_uwater_under = zox_has(camera, Underwater);
        zox_tst_uwater_tint = zox_has(camera, CameraTint) ? zox_getv(camera, CameraTint) : -1;
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("underwater.ppm"));
    } else if (zox_tst_uwater_stage == 2) {
        if (zox_screenshot_request[0]) {
            return 0;
        }
        zox_tst_uwater_fill(world, terrain, 0);
    } else if (zox_tst_uwater_stage == 3) {
        zox_tst_waiting = "the camera to come out of the water";
        if (since < 1.5) {
            return 0;
        }
        zox_tst_uwater_out = !zox_has(camera, Underwater);
        zox_tst_uwater_tint_out = zox_has(camera, CameraTint) ? zox_getv(camera, CameraTint) : -1;
    } else {
        return 1;
    }
    zox_tst_uwater_stage++;
    zox_tst_uwater_time = zox_current_time;
    return 0;
}

void zox_tst_underwater_check(ecs* world) {
    zox_log("[tst] [metric] underwater: under [%i] tint [%.2f], out [%i] tint [%.2f]",
        zox_tst_uwater_under, zox_tst_uwater_tint, zox_tst_uwater_out, zox_tst_uwater_tint_out);
    zox_tst_check(zox_tst_uwater_stage == 4, "the test didn't finish, stage [%i]", zox_tst_uwater_stage);
    zox_tst_check(zox_tst_uwater_under, "the camera in water isn't Underwater");
    zox_tst_check(zox_tst_uwater_tint > 0.9f, "under water the view's tint is [%.2f], not over 0.9", zox_tst_uwater_tint);
    zox_tst_check(zox_tst_uwater_out, "out of the water the camera is still Underwater");
    zox_tst_check(zox_tst_uwater_tint_out >= 0 && zox_tst_uwater_tint_out < 0.05f, "out of water the view's tint is [%.2f], not under 0.05",
        zox_tst_uwater_tint_out);
    FILE* file = fopen(zox_tst_path("underwater.ppm"), "rb");
    zox_tst_check(file, "the screenshot under water wasn't saved");
    if (file) {
        fclose(file);
    }
}
