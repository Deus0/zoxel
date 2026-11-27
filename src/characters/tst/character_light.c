// tst: character_light, a character is lit by the terrain round it (characters3/sys/light.c): its Brightness the light
//  at its head (its chunk's light octree), easing to it
//  - the player standing where it spawned (in the open, the morning): bright
//  - held 12m under the ground (in its solid, darklight): it darkens
//  - back up where it stood: bright again
//  - run: bash bsh/tst.sh character_light
byte zox_tst_char_light_stage = 0;
double zox_tst_char_light_time = 0;
float3 zox_tst_char_light_start;
float zox_tst_char_light_lit = -1, zox_tst_char_light_dark = -1, zox_tst_char_light_again = -1;

byte zox_tst_character_light_ready(ecs* world) {
    if (!zox_tst_char_light_stage) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        zox_tst_char_light_start = zox_getv(zox_tst_player_character(world), Position3D);
        zox_tst_char_light_time = zox_current_time;
        zox_tst_char_light_stage = 1;
    }
    const entity character = zox_tst_player_character(world);
    const double since = zox_current_time - zox_tst_char_light_time;
    zox_tst_progress(zox_tst_char_light_stage * 100 + since);
    zox_tst_waiting = "the character lit, in the dark, lit again";
    float3 position = zox_tst_char_light_start;
    if (zox_tst_char_light_stage == 2) {
        position.y -= 12;
    }
    if (zox_tst_char_light_stage >= 2) {
        zox_setv(character, Position3D, position);
        zox_setv(character, Velocity3D, float3_zero);
    }
    if (since < 2.5) {
        return 0;
    }
    const float brightness = zox_getv(character, Brightness);
    zox_log("[tst] character_light: stage [%i] brightness [%.2f] at [%.1f %.1f %.1f]", zox_tst_char_light_stage, brightness,
        position.x, position.y, position.z);
    if (zox_tst_char_light_stage == 1) {
        zox_tst_char_light_lit = brightness;
    } else if (zox_tst_char_light_stage == 2) {
        zox_tst_char_light_dark = brightness;
    } else {
        zox_tst_char_light_again = brightness;
        return 1;
    }
    zox_tst_char_light_stage++;
    zox_tst_char_light_time = zox_current_time;
    return 0;
}

void zox_tst_character_light_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_char_light_lit > 0.5f, "standing in the open it's dim [%.2f]", zox_tst_char_light_lit);
    zox_tst_check(zox_tst_char_light_dark >= 0 && zox_tst_char_light_dark < 0.2f, "under the ground it isn't dark [%.2f]", zox_tst_char_light_dark);
    zox_tst_check(zox_tst_char_light_again > 0.5f, "back up it's still dim [%.2f]", zox_tst_char_light_again);
}

void zox_tsts_character_light(ecs* world) {
    zox_tst_add("character_light", zox_tst_character_light_ready, zox_tst_character_light_check, 120);
}
