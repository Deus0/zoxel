// tst: bite_hitch, a long frame (a hitch) doesn't skip a swing's marks (bones/sys/swing_mark.c): each fires once, in
//  order (they were found from the frame's delta, capped: a hitch skipped bites and hits, a skipped first bite ate
//  nothing)
//  - a character (characters/tst/character.c, held in the void), an eat swing begun far enough back that all its
//    bites are past in one frame
//  - its bites all chew (consumables/eve/eat.c counts them), each once
//  - run: bash bsh/tst.sh bite_hitch
byte zox_tst_hitch_stage = 0;
double zox_tst_hitch_at = 0;
float3 zox_tst_hitch_start;
uint zox_tst_hitch_from = 0;
uint zox_tst_hitch_bites = 0;

byte zox_tst_bite_hitch_ready(ecs* world) {
    const entity e = zox_tst_body_ready(world, 0, (float3) { 0, 400, 0 });
    if (!e) {
        return 0;
    }
    if (!zox_tst_hitch_at) {
        zox_tst_hitch_start = zox_getv(e, Position3D);
        zox_tst_hitch_at = zox_current_time;
    }
    zox_setv(e, Position3D, zox_tst_hitch_start);
    zox_setv(e, Velocity3D, float3_zero);
    const entity skeleton = zox_get_link(world, e, SkeletonLink);
    zox_tst_progress(zox_tst_hitch_stage * 100 + (int) ((zox_current_time - zox_tst_hitch_at) * 10));
    if (zox_tst_hitch_stage == 0) {
        zox_tst_waiting = "the character's head and hand";
        if (zox_current_time - zox_tst_hitch_at < 1 || !zox_valid(zox_get_link(world, skeleton, HeadBoneLink)) ||
            !zox_valid(zox_get_link(world, skeleton, HandBoneLink))) {
            return 0;
        }
        // as if a hitch: 0.95 of the way through, every bite past
        zox_tst_hitch_from = zox_eat_bites;
        skeleton_swing_begin(world, e, 0, zox_current_time - 0.95, 1, zox_swing_eat, float3_zero);
        zox_tst_hitch_stage = 1;
        zox_tst_hitch_at = zox_current_time;
        return 0;
    }
    zox_tst_waiting = "the bites";
    zox_tst_hitch_bites = zox_eat_bites - zox_tst_hitch_from;
    return zox_current_time - zox_tst_hitch_at > 0.5;
}

void zox_tst_bite_hitch_check(ecs* world) {
    const byte bites = swings[zox_swing_eat].marks_count;
    zox_log("[tst] bite_hitch: chewed [%u] of [%i] bites after a hitch", zox_tst_hitch_bites, bites);
    zox_tst_check(zox_tst_hitch_stage == 1, "never swung");
    zox_tst_check(zox_tst_hitch_bites == bites, "chewed [%u] of [%i] bites after a hitch", zox_tst_hitch_bites, bites);
}

void zox_tsts_bite_hitch(ecs* world) {
    zox_tst_add("bite_hitch", zox_tst_bite_hitch_ready, zox_tst_bite_hitch_check, 60);
}
