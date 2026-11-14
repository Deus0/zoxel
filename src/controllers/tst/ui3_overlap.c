// tst: ui3_overlap, a 3D ui in front hides one behind it whichever draws first (canvases aren't sorted, each writes
//  its depth after its layers: canvas3_render_system, doc/tech/uis.md)
//  - on flat bare land (as ui3_occlusion), held up, looking a little down: a green canvas 8 blocks along the view, a
//      red one 16 blocks along it (smaller on screen, wholly behind the green)
//  - twice: the green spawned first, then the red first (the order they draw in)
//  - checks: each time the green shows, no red does
//  - run: bash bsh/tst.sh ui3_overlap
int zox_tst_uo_stage = 0;
int zox_tst_uo_frames = 0;
entity zox_tst_uo_canvases[2];
float3 zox_tst_uo_at;
int zox_tst_uo_green[2] = { -1, -1 }, zox_tst_uo_red[2] = { -1, -1 };

void zox_tst_ui3_overlap_setup() {
    zox_tst_ui3_occlusion_setup();
}

byte zox_tst_ui3_overlap_ready(ecs* world) {
    if (!zox_tst_uo_stage && !zox_tst_uo_frames && !zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const float scale = zox_getv(terrain, BlockScale);
    const entity camera = zox_tst_u3_camera(world, character);
    zox_tst_uo_frames++;
    zox_tst_progress(zox_tst_uo_stage);
    if (zox_tst_uo_stage == 0) {
        if (!zox_valid(camera)) {
            zox_tst_check(0, "no head camera");
            return 1;
        }
        const float3 p = zox_getv(character, Position3D);
        zox_tst_uo_at = (float3) { p.x, p.y + 12 * scale, p.z };
        zox_tst_uo_stage = 1;
        zox_tst_uo_frames = 0;
    }
    fog_density = 0;
    zox_setv(character, Position3D, zox_tst_uo_at);
    zox_setv(character, Velocity3D, float3_zero);
    zox_setv(character, RenderDisabled, 1);
    zox_setv(camera, LocalRotation3D, euler_to_quaternion((float3) { -0.3f, is_camera_positive_z ? M_PI : 0, 0 }));
    // rounds 0 and 1: settle, spawn the two (green first, then red first), screenshot, count, delete
    const int round = (zox_tst_uo_stage - 1) / 2;
    zox_tst_waiting = "the two 3D uis";
    if (zox_tst_uo_stage % 2 == 1) {
        if (zox_tst_uo_frames < 30) {
            return 0;
        }
        const float3 near = zox_tst_u3_along(world, camera, 8 * scale), far = zox_tst_u3_along(world, camera, 16 * scale);
        const entity green = round ? 0 : zox_tst_u3_frame_canvas(world, near, (color) { 0, 255, 0, 255 }, 1);
        const entity red = zox_tst_u3_frame_canvas(world, far, (color) { 255, 0, 0, 255 }, 1);
        zox_tst_uo_canvases[0] = round ? zox_tst_u3_frame_canvas(world, near, (color) { 0, 255, 0, 255 }, 1) : green;
        zox_tst_uo_canvases[1] = red;
        zox_tst_uo_stage++;
        zox_tst_uo_frames = 0;
        return 0;
    }
    if (zox_tst_uo_frames == 30) {
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("ui3_overlap.ppm"));
    }
    if (zox_tst_uo_frames < 40) {
        return 0;
    }
    zox_tst_uo_green[round] = zox_tst_u3_pixels(zox_tst_path("ui3_overlap.ppm"), 1);
    zox_tst_uo_red[round] = zox_tst_u3_pixels(zox_tst_path("ui3_overlap.ppm"), 0);
    zox_delete(zox_tst_uo_canvases[0]);
    zox_delete(zox_tst_uo_canvases[1]);
    zox_tst_uo_stage++;
    zox_tst_uo_frames = 0;
    if (round == 1) {
        zox_setv(character, RenderDisabled, 0);
        return 1;
    }
    return 0;
}

void zox_tst_ui3_overlap_check(ecs* world) {
    (void) world;
    for (int r = 0; r < 2; r++) {
        zox_log("[tst] [metric] ui3_overlap [%s] first: green [%i] red [%i]", r ? "red" : "green", zox_tst_uo_green[r], zox_tst_uo_red[r]);
        zox_tst_check(zox_tst_uo_green[r] > 200, "[%s] first, the near (green) 3D ui isn't seen", r ? "red" : "green");
        zox_tst_check(zox_tst_uo_red[r] >= 0 && zox_tst_uo_red[r] < 20, "[%s] first, the far (red) 3D ui shows through the near one [%i]",
            r ? "red" : "green", zox_tst_uo_red[r]);
    }
}
