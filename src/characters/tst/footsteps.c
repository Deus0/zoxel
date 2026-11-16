// tst: footsteps, the player walking on the ground plays footsteps (characters/jump/sys/footsteps.c)
//  - pushes the player along for a few seconds (its velocity, as if walking), it's grounded
//  - its FootstepDistance adds up and footsteps play, the block underfoot's hit sound (doc/sounds.md)
//  - run: bash bsh/tst.sh footsteps
double zox_tst_footsteps_start = 0;
uint zox_tst_footsteps_before = 0;
#define zox_tst_footsteps_seconds 3.0

byte zox_tst_footsteps_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    if (!zox_tst_footsteps_start) {
        zox_tst_footsteps_start = zox_current_time;
        zox_tst_footsteps_before = zox_footsteps_played;
    }
    zox_tst_waiting = "the player to walk";
    // walking: a steady push along x, the ground's friction would stop a single one
    float3 velocity = zox_getv(character, Velocity3D);
    velocity.x = 3;
    zox_setv(character, Velocity3D, velocity);
    return zox_current_time - zox_tst_footsteps_start >= zox_tst_footsteps_seconds;
}

void zox_tst_footsteps_check(ecs* world) {
    const entity character = zox_tst_player_character(world);
    const uint steps = zox_footsteps_played - zox_tst_footsteps_before;
    zox_log("[tst] [metric] footsteps [%u] in [%.0fs], grounded [%i]", steps, zox_tst_footsteps_seconds, zox_getv(character, Grounded));
    zox_tst_check(zox_has(character, Footsteps), "the player has no Footsteps tag");
    zox_tst_check(steps >= 2, "only [%u] footsteps walking for [%.0fs]", steps, zox_tst_footsteps_seconds);
}

void zox_tsts_footsteps(ecs* world) {
    zox_tst_add("footsteps", zox_tst_footsteps_ready, zox_tst_footsteps_check, 120);
}
