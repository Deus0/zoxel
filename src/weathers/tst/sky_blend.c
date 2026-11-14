// tst: sky_blend, the sky fades to a new colour (blend_sky_to: SkyBlend tweened, sky_blend_system), its fog with it:
//  walking into another biome it snapped
//  - waits for the game to be playing and its sky to settle (on its target, no blend running), then blends it to a
//      far colour (each channel to its other end)
//  - checks: the frame after, it has not snapped there; during its tween it is between them; when its tween ends
//      (AnimationEnded, observed) it is at its target, the fog the same colour
//  - run: bash bsh/tst.sh sky_blend
int zox_tst_sb_stage = 0;
int zox_tst_sb_settled = 0;
entity zox_tst_sb_skybox = 0;
byte zox_tst_sb_ended = 0;
color_rgb zox_tst_sb_from, zox_tst_sb_to;
byte zox_tst_sb_snapped = 0, zox_tst_sb_part_way = 0, zox_tst_sb_arrived = 0, zox_tst_sb_fog = 0;
int zox_tst_sb_frames = 0;
float zox_tst_sb_blend_first = -1, zox_tst_sb_blend_last = -1;
color_rgb zox_tst_sb_seen_first, zox_tst_sb_seen_last;

static inline byte zox_tst_sb_equal(color_rgb a, color_rgb b) {
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

// the sky's tween ended
void zox_tst_sky_blend_ended(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        if (zox_tst_sb_skybox && zox_getv(e, AnimationTarget) == zox_tst_sb_skybox &&
            zox_getv(e, AnimationTargetComponent) == zox_id(SkyBlend)) {
            zox_tst_sb_ended = 1;
        }
    }
}

byte zox_tst_sky_blend_ready(ecs* world) {
    const entity skybox = zox_valid(local_game) ? zox_get_link(world, local_game, Skybox) : 0;
    // playing (a game starting snaps the sky to its colour, set_skybox_colors: it cancelled the blend)
    if (!zox_valid(skybox) || !zox_getv(skybox, MaterialGPULink) || zox_getv(local_game, GameState) != zox_game_state_playing) {
        zox_tst_waiting = "the game's sky";
        return 0;
    }
    const color_rgb now = zox_getv(skybox, ColorRGB);
    zox_tst_progress(zox_tst_sb_stage);
    if (zox_tst_sb_stage == 0) {
        zox_tst_waiting = "the sky to settle";
        const byte settled = zox_tst_sb_equal(now, zox_getv(skybox, SkyColorTarget)) &&
            !zox_valid(zox_get_link(world, skybox, SkyBlendLink));
        zox_tst_sb_settled = settled ? zox_tst_sb_settled + 1 : 0;
        if (zox_tst_sb_settled < 30) {
            return 0;
        }
        zox_tst_sb_skybox = skybox;
        zox_tst_sb_from = now;
        zox_tst_sb_to = (color_rgb) { now.r < 128 ? 255 : 0, now.g < 128 ? 255 : 0, now.b < 128 ? 255 : 0 };
        blend_sky_to(world, skybox, zox_tst_sb_to);
        zox_tst_sb_stage = 1;
        return 0;
    }
    zox_tst_waiting = "the sky to blend";
    if (zox_tst_sb_stage == 1) {
        zox_tst_sb_snapped = zox_tst_sb_equal(now, zox_tst_sb_to);
        zox_tst_sb_stage = 2;
        return 0;
    }
    if (zox_tst_sb_stage == 2) {
        // any frame of its blend between its colours (a long frame can jump most of it)
        zox_tst_sb_part_way |= !zox_tst_sb_equal(now, zox_tst_sb_from) && !zox_tst_sb_equal(now, zox_tst_sb_to);
        if (!zox_tst_sb_frames++) {
            zox_tst_sb_blend_first = zox_getv(skybox, SkyBlend);
            zox_tst_sb_seen_first = now;
        }
        zox_tst_sb_blend_last = zox_getv(skybox, SkyBlend);
        zox_tst_sb_seen_last = now;
        if (zox_tst_sb_ended) {
            zox_tst_sb_stage = 3;   // its colour is uploaded after its tween's last value, a frame on
        }
        return 0;
    }
    // its target now (a biome linking late may have set another)
    zox_tst_sb_arrived = zox_tst_sb_equal(now, zox_getv(skybox, SkyColorTarget));
    const entity camera = main_cameras[0];
    zox_tst_sb_fog = zox_valid(camera) && zox_tst_sb_equal(zox_getv(camera, FogColor), now);
    return 1;
}

void zox_tst_sky_blend_check(ecs* world) {
    (void) world;
    zox_log("[tst] sky [%i %i %i] blended to [%i %i %i]", zox_tst_sb_from.r, zox_tst_sb_from.g, zox_tst_sb_from.b,
        zox_tst_sb_to.r, zox_tst_sb_to.g, zox_tst_sb_to.b);
    zox_log("[tst] blend frames [%i] blend [%.3f to %.3f] sky [%i %i %i] to [%i %i %i]", zox_tst_sb_frames, zox_tst_sb_blend_first, zox_tst_sb_blend_last, zox_tst_sb_seen_first.r, zox_tst_sb_seen_first.g, zox_tst_sb_seen_first.b, zox_tst_sb_seen_last.r, zox_tst_sb_seen_last.g, zox_tst_sb_seen_last.b);
    zox_tst_check(!zox_tst_sb_snapped, "the sky snapped to its new colour");
    zox_tst_check(zox_tst_sb_part_way, "the sky was never between its colours during its blend");
    zox_tst_check(zox_tst_sb_ended, "the sky's blend never ended (AnimationEnded)");
    zox_tst_check(zox_tst_sb_arrived, "the sky isn't at its target when its blend ended");
    zox_tst_check(zox_tst_sb_fog, "the fog isn't the sky's colour");
}

void zox_tsts_sky_blend(ecs* world) {
    zox_tst_add("sky_blend", zox_tst_sky_blend_ready, zox_tst_sky_blend_check, 60);
    zox_on_add(zox_tst_sky_blend_ended, [none] animations.AnimationEnded);
}
