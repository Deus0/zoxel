// tst: mountain_vegetation, a mountain's middle third is cleared of vegetation (mountain_clears_vegetation, sys/maps.c)
//  - the check read `distance < 2 / (radius * 3)`, integer division: 0, nothing was ever cleared
//  - run: bash bsh/tst.sh mountain_vegetation
byte zox_tst_mountain_vegetation_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_mountain_vegetation_check(ecs* world) {
    (void) world;
    const int radius = 30;
    int cleared = 0;
    for (int d = 0; d <= radius; d++) {
        cleared += mountain_clears_vegetation(d, radius);
    }
    zox_log("[tst] [metric] mountain_vegetation [%i] of [%i] distances from the middle cleared", cleared, radius + 1);
    zox_tst_check(mountain_clears_vegetation(0, radius), "a mountain's middle keeps its vegetation");
    zox_tst_check(mountain_clears_vegetation(radius / 3 - 1, radius), "inside its middle third keeps its vegetation");
    zox_tst_check(!mountain_clears_vegetation(radius / 2, radius), "half way out is cleared");
}

void zox_tsts_mountains() {
    zox_tst_add("mountain_vegetation", zox_tst_mountain_vegetation_ready, zox_tst_mountain_vegetation_check, 30);
}
